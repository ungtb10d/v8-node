// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/init/v8.h"
#include "test/common/wasm/wasm-interpreter.h"
#include "test/common/wasm/wasm-macro-gen.h"
#include "test/unittests/test-utils.h"
#include "testing/gmock/include/gmock/gmock.h"

using testing::MakeMatcher;
using testing::Matcher;
using testing::MatcherInterface;
using testing::MatchResultListener;
using testing::StringMatchResultListener;

namespace v8 {
namespace internal {
namespace wasm {

struct ExpectedControlTransfer {
  pc_t pc;
  pcdiff_t pc_diff;
  uint32_t sp_diff;
  uint32_t target_arity;
};

// For nicer error messages.
class ControlTransferMatcher
    : public MatcherInterface<const ControlTransferEntry&> {
 public:
  explicit ControlTransferMatcher(pc_t pc,
                                  const ExpectedControlTransfer& expected)
      : pc_(pc), expected_(expected) {}

  void DescribeTo(std::ostream* os) const override {
    *os << "@" << pc_ << ": pcdiff = " << expected_.pc_diff
        << ", spdiff = " << expected_.sp_diff
        << ", target arity = " << expected_.target_arity;
  }

  bool MatchAndExplain(const ControlTransferEntry& input,
                       MatchResultListener* listener) const override {
    if (input.pc_diff == expected_.pc_diff &&
        input.sp_diff == expected_.sp_diff &&
        input.target_arity == expected_.target_arity) {
      return true;
    }
    *listener << "@" << pc_ << ": pcdiff = " << input.pc_diff
              << ", spdiff = " << input.sp_diff
              << ", target arity = " << input.target_arity;
    return false;
  }

 private:
  pc_t pc_;
  const ExpectedControlTransfer& expected_;
};

class ControlTransferTest : public TestWithZone {
 public:
  static constexpr pc_t kPcShiftForLocalsCount = 1;

  template <int kCodeLen>
  void CheckTransfers(
      const byte (&code)[kCodeLen],
      std::initializer_list<ExpectedControlTransfer> expected_transfers) {
    // Add a leading '0' for number of locals and append the 'end' opcode.
    byte function_body[kCodeLen + 2];
    function_body[0] = 0;
    memcpy(function_body + 1, code, kCodeLen);
    function_body[kCodeLen + 1] = kExprEnd;

    ControlTransferMap map = WasmInterpreter::ComputeControlTransfersForTesting(
        zone(), std::begin(function_body), std::end(function_body));
    // Check all control targets in the map.
    for (auto& expected_transfer : expected_transfers) {
      pc_t pc = expected_transfer.pc + kPcShiftForLocalsCount;
      EXPECT_TRUE(map.map.count(pc) > 0) << "expected control target @" << pc;
      if (!map.map.count(pc)) continue;
      auto& entry = map.map[pc];
      EXPECT_THAT(entry, MakeMatcher(new ControlTransferMatcher(
                             pc, expected_transfer)));
    }

    // Check there are no other control targets.
    CheckNoOtherTargets(std::begin(function_body), std::end(function_body), map,
                        expected_transfers);
  }

  void CheckNoOtherTargets(
      const byte* start, const byte* end, const ControlTransferMap& map,
      std::initializer_list<ExpectedControlTransfer> targets) {
    // Check there are no other control targets.
    for (pc_t pc = 0; start + pc < end; pc++) {
      bool found = false;
      for (auto& target : targets) {
        if (target.pc + kPcShiftForLocalsCount == pc) {
          found = true;
          break;
        }
      }
      if (found) continue;
      EXPECT_TRUE(map.map.count(pc) == 0) << "expected no control @ +" << pc;
    }
  }
};

TEST_F(ControlTransferTest, SimpleIf) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprEnd        // @4
  };
  CheckTransfers(code, {{2, 2, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleIf1) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprNop,       // @4
      kExprEnd        // @5
  };
  CheckTransfers(code, {{2, 3, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleIf2) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprNop,       // @4
      kExprNop,       // @5
      kExprEnd        // @6
  };
  CheckTransfers(code, {{2, 4, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleIfElse) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprElse,      // @4
      kExprEnd        // @5
  };
  CheckTransfers(code, {{2, 3, 0, 0}, {4, 2, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleIfElse_v1) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprI32Const,  // @4
      0,              // @5
      kExprElse,      // @6
      kExprI32Const,  // @7
      0,              // @8
      kExprEnd        // @9
  };
  CheckTransfers(code, {{2, 5, 0, 0}, {6, 4, 1, 0}});
}

TEST_F(ControlTransferTest, SimpleIfElse1) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprElse,      // @4
      kExprNop,       // @5
      kExprEnd        // @6
  };
  CheckTransfers(code, {{2, 3, 0, 0}, {4, 3, 0, 0}});
}

TEST_F(ControlTransferTest, IfBr) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprBr,        // @4
      0,              // @5
      kExprEnd        // @6
  };
  CheckTransfers(code, {{2, 4, 0, 0}, {4, 3, 0, 0}});
}

TEST_F(ControlTransferTest, IfBrElse) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprBr,        // @4
      0,              // @5
      kExprElse,      // @6
      kExprEnd        // @7
  };
  CheckTransfers(code, {{2, 5, 0, 0}, {4, 4, 0, 0}});
}

TEST_F(ControlTransferTest, IfElseBr) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprIf,        // @2
      kVoidCode,      // @3
      kExprElse,      // @4
      kExprBr,        // @5
      0,              // @6
      kExprEnd        // @7
  };
  CheckTransfers(code, {{2, 3, 0, 0}, {4, 4, 0, 0}, {5, 3, 0, 0}});
}

TEST_F(ControlTransferTest, BlockEmpty) {
  byte code[] = {
      kExprBlock,  // @0
      kVoidCode,   // @1
      kExprEnd     // @2
  };
  CheckTransfers(code, {});
}

TEST_F(ControlTransferTest, Br0) {
  byte code[] = {
      kExprBlock,  // @0
      kVoidCode,   // @1
      kExprBr,     // @2
      0,           // @3
      kExprEnd     // @4
  };
  CheckTransfers(code, {{2, 3, 0, 0}});
}

TEST_F(ControlTransferTest, Br1) {
  byte code[] = {
      kExprBlock,  // @0
      kVoidCode,   // @1
      kExprNop,    // @2
      kExprBr,     // @3
      0,           // @4
      kExprEnd     // @5
  };
  CheckTransfers(code, {{3, 3, 0, 0}});
}

TEST_F(ControlTransferTest, Br_v1a) {
  byte code[] = {
      kExprBlock,     // @0
      kVoidCode,      // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprBr,        // @4
      0,              // @5
      kExprEnd        // @6
  };
  CheckTransfers(code, {{4, 3, 1, 0}});
}

TEST_F(ControlTransferTest, Br_v1b) {
  byte code[] = {
      kExprBlock,     // @0
      kVoidCode,      // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprBr,        // @4
      0,              // @5
      kExprEnd        // @6
  };
  CheckTransfers(code, {{4, 3, 1, 0}});
}

TEST_F(ControlTransferTest, Br_v1c) {
  byte code[] = {
      kExprI32Const,  // @0
      0,              // @1
      kExprBlock,     // @2
      kVoidCode,      // @3
      kExprBr,        // @4
      0,              // @5
      kExprEnd        // @6
  };
  CheckTransfers(code, {{4, 3, 0, 0}});
}

TEST_F(ControlTransferTest, Br_v1d) {
  byte code[] = {
      kExprBlock,     // @0
      kI32Code,       // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprBr,        // @4
      0,              // @5
      kExprEnd        // @6
  };
  CheckTransfers(code, {{4, 3, 1, 1}});
}

TEST_F(ControlTransferTest, Br2) {
  byte code[] = {
      kExprBlock,  // @0
      kVoidCode,   // @1
      kExprNop,    // @2
      kExprNop,    // @3
      kExprBr,     // @4
      0,           // @5
      kExprEnd     // @6
  };
  CheckTransfers(code, {{4, 3, 0, 0}});
}

TEST_F(ControlTransferTest, Br0b) {
  byte code[] = {
      kExprBlock,  // @0
      kVoidCode,   // @1
      kExprBr,     // @2
      0,           // @3
      kExprNop,    // @4
      kExprEnd     // @5
  };
  CheckTransfers(code, {{2, 4, 0, 0}});
}

TEST_F(ControlTransferTest, Br0c) {
  byte code[] = {
      kExprBlock,  // @0
      kVoidCode,   // @1
      kExprBr,     // @2
      0,           // @3
      kExprNop,    // @4
      kExprNop,    // @5
      kExprEnd     // @6
  };
  CheckTransfers(code, {{2, 5, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleLoop1) {
  byte code[] = {
      kExprLoop,  // @0
      kVoidCode,  // @1
      kExprBr,    // @2
      0,          // @3
      kExprEnd    // @4
  };
  CheckTransfers(code, {{2, -2, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleLoop2) {
  byte code[] = {
      kExprLoop,  // @0
      kVoidCode,  // @1
      kExprNop,   // @2
      kExprBr,    // @3
      0,          // @4
      kExprEnd    // @5
  };
  CheckTransfers(code, {{3, -3, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleLoopExit1) {
  byte code[] = {
      kExprLoop,  // @0
      kVoidCode,  // @1
      kExprBr,    // @2
      1,          // @3
      kExprEnd    // @4
  };
  CheckTransfers(code, {{2, 4, 0, 0}});
}

TEST_F(ControlTransferTest, SimpleLoopExit2) {
  byte code[] = {
      kExprLoop,  // @0
      kVoidCode,  // @1
      kExprNop,   // @2
      kExprBr,    // @3
      1,          // @4
      kExprEnd    // @5
  };
  CheckTransfers(code, {{3, 4, 0, 0}});
}

TEST_F(ControlTransferTest, BrTable0) {
  byte code[] = {
      kExprBlock,     // @0
      kVoidCode,      // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprBrTable,   // @4
      0,              // @5
      U32V_1(0),      // @6
      kExprEnd        // @7
  };
  CheckTransfers(code, {{4, 4, 0, 0}});
}

TEST_F(ControlTransferTest, BrTable0_v1a) {
  byte code[] = {
      kExprBlock,     // @0
      kVoidCode,      // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprI32Const,  // @4
      0,              // @5
      kExprBrTable,   // @6
      0,              // @7
      U32V_1(0),      // @8
      kExprEnd        // @9
  };
  CheckTransfers(code, {{6, 4, 1, 0}});
}

TEST_F(ControlTransferTest, BrTable0_v1b) {
  byte code[] = {
      kExprBlock,     // @0
      kVoidCode,      // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprI32Const,  // @4
      0,              // @5
      kExprBrTable,   // @6
      0,              // @7
      U32V_1(0),      // @8
      kExprEnd        // @9
  };
  CheckTransfers(code, {{6, 4, 1, 0}});
}

TEST_F(ControlTransferTest, BrTable1) {
  byte code[] = {
      kExprBlock,     // @0
      kVoidCode,      // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprBrTable,   // @4
      1,              // @5
      U32V_1(0),      // @6
      U32V_1(0),      // @7
      kExprEnd        // @8
  };
  CheckTransfers(code, {{4, 5, 0, 0}, {5, 4, 0, 0}});
}

TEST_F(ControlTransferTest, BrTable2) {
  byte code[] = {
      kExprBlock,     // @0
      kVoidCode,      // @1
      kExprBlock,     // @2
      kVoidCode,      // @3
      kExprI32Const,  // @4
      0,              // @5
      kExprBrTable,   // @6
      2,              // @7
      U32V_1(0),      // @8
      U32V_1(0),      // @9
      U32V_1(1),      // @10
      kExprEnd,       // @11
      kExprEnd        // @12
  };
  CheckTransfers(code, {{6, 6, 0, 0}, {7, 5, 0, 0}, {8, 5, 0, 0}});
}

TEST_F(ControlTransferTest, BiggerSpDiffs) {
  byte code[] = {
      kExprBlock,     // @0
      kI32Code,       // @1
      kExprI32Const,  // @2
      0,              // @3
      kExprBlock,     // @4
      kVoidCode,      // @5
      kExprI32Const,  // @6
      0,              // @7
      kExprI32Const,  // @8
      0,              // @9
      kExprI32Const,  // @10
      0,              // @11
      kExprBrIf,      // @12
      0,              // @13
      kExprBr,        // @14
      1,              // @15
      kExprEnd,       // @16
      kExprEnd        // @17
  };
  CheckTransfers(code, {{12, 5, 2, 0}, {14, 4, 3, 1}});
}

TEST_F(ControlTransferTest, NoInfoForUnreachableCode) {
  byte code[] = {
      kExprBlock,        // @0
      kVoidCode,         // @1
      kExprBr,           // @2
      0,                 // @3
      kExprBr,           // @4 -- no control transfer entry!
      1,                 // @5
      kExprEnd,          // @6
      kExprBlock,        // @7
      kVoidCode,         // @8
      kExprUnreachable,  // @9
      kExprI32Const,     // @10
      0,                 // @11
      kExprIf,           // @12 -- no control transfer entry!
      kVoidCode,         // @13
      kExprBr,           // @14 -- no control transfer entry!
      0,                 // @15
      kExprElse,         // @16 -- no control transfer entry!
      kExprEnd,          // @17
      kExprEnd           // @18
  };
  CheckTransfers(code, {{2, 5, 0, 0}});
}

}  // namespace wasm
}  // namespace internal
}  // namespace v8
