// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_HEAP_MARKING_BARRIER_INL_H_
#define V8_HEAP_MARKING_BARRIER_INL_H_

#include "src/base/logging.h"
#include "src/heap/incremental-marking-inl.h"
#include "src/heap/incremental-marking.h"
#include "src/heap/marking-barrier.h"

namespace v8 {
namespace internal {

void MarkingBarrier::MarkValue(HeapObject host, HeapObject value) {
  DCHECK(IsCurrentMarkingBarrier());
  DCHECK(is_activated_ || shared_heap_worklist_.has_value());
  DCHECK(!marking_state_.IsImpossible(value));

  // Host may have an impossible markbit pattern if manual allocation folding
  // is performed and host happens to be the last word of an allocated region.
  // In that case host has only one markbit and the second markbit belongs to
  // another object. We can detect that case by checking if value is a one word
  // filler map.
  DCHECK_IMPLIES(
      !host.is_null(),
      !marking_state_.IsImpossible(host) ||
          value == ReadOnlyRoots(heap_->isolate()).one_pointer_filler_map());

  if (V8_UNLIKELY(uses_shared_heap_) && value.InSharedWritableHeap()) {
    if (ProcessSharedObject(value)) return;
  }

  DCHECK_IMPLIES(value.InSharedWritableHeap(), is_shared_heap_isolate_);

  if (!is_activated_) return;

  if (is_minor()) {
    // We do not need to insert into RememberedSet<OLD_TO_NEW> here because the
    // C++ marking barrier already does this for us.
    if (Heap::InYoungGeneration(value)) {
      WhiteToGreyAndPush(value);  // NEW->NEW
    }
  } else {
    if (WhiteToGreyAndPush(value)) {
      if (V8_UNLIKELY(v8_flags.track_retaining_path)) {
        heap_->AddRetainingRoot(Root::kWriteBarrier, value);
      }
    }
  }
}

bool MarkingBarrier::ProcessSharedObject(HeapObject value) {
  if (v8_flags.shared_space) {
    if (is_shared_heap_isolate_) {
      // The main isolate processes objects in the shared heap as regular local
      // objects.
      return false;
    } else {
      // Background threads need to mark shared objects when incremental marking
      // in the shared heap is active.
      if (shared_heap_worklist_.has_value() &&
          marking_state_.WhiteToGrey(value)) {
        DCHECK(v8_flags.shared_space);
        DCHECK(!is_shared_heap_isolate_);
        shared_heap_worklist_->Push(value);
      }

      // Background threads do not process shared objects as regular local
      // objects.
      return true;
    }

  } else {
    // All threads ignore shared objects during incremental marking.
    return true;
  }
}

template <typename TSlot>
inline void MarkingBarrier::MarkRange(HeapObject host, TSlot start, TSlot end) {
  auto* isolate = heap_->isolate();
  const bool is_compacting = is_compacting_;
  for (TSlot slot = start; slot < end; ++slot) {
    typename TSlot::TObject object = slot.Relaxed_Load();
    HeapObject heap_object;
    // Mark both, weak and strong edges.
    if (object.GetHeapObject(isolate, &heap_object)) {
      MarkValue(host, heap_object);
      if (is_compacting) {
        DCHECK(is_major());
        major_collector_->RecordSlot(host, HeapObjectSlot(slot), heap_object);
      }
    }
  }
}

bool MarkingBarrier::WhiteToGreyAndPush(HeapObject obj) {
  if (marking_state_.WhiteToGrey(obj)) {
    current_worklist_->Push(obj);
    return true;
  }
  return false;
}

}  // namespace internal
}  // namespace v8

#endif  // V8_HEAP_MARKING_BARRIER_INL_H_
