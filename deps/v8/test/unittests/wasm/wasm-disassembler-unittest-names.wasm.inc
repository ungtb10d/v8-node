  0x00, 0x61, 0x73, 0x6d,  // wasm magic
  0x01, 0x00, 0x00, 0x00,  // wasm version

  0x01,                    // section kind: Type
  0x0a,                    // section length 10
  0x02,                    // types count 2
  0x60,                    //  kind: func
  0x00,                    // param count 0
  0x00,                    // return count 0
  0x60,                    //  kind: func
  0x03,                    // param count 3
  0x7f, 0x7f, 0x7e,        // i32 i32 i64
  0x00,                    // return count 0

  0x02,                    // section kind: Import
  0x30,                    // section length 48
  0x02,                    // imports count 2
                           // import #0
  0x03,                    // module name length: 3
  0x65, 0x6e, 0x76,        // module name: env
  0x0f,                    // field name length: 15
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x67, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 
                           // field name: imported_global
  0x03, 0x7f, 0x00,        // kind: global i32 immutable
                           // import #1
  0x03,                    // module name length: 3
  0x65, 0x6e, 0x76,        // module name: env
  0x11,                    // field name length: 17
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 
  0x6e,                    // field name: imported_function
  0x00, 0x00,              // kind: function

  0x03,                    // section kind: Function
  0x03,                    // section length 3
  0x02,                    // functions count 2
  0x01,                    // 1 $function_with_name (param i32 i32 i64)
  0x00,                    // 2 $exported_function_with_name

  0x04,                    // section kind: Table
  0x04,                    // section length 4
  0x01, 0x70, 0x00,        // table count 1:  funcref no maximum
  0x00,                    // initial size 0

  0x05,                    // section kind: Memory
  0x03,                    // section length 3
  0x01, 0x00,              // memory count 1:  no maximum
  0x00,                    // initial size 0

  0x06,                    // section kind: Global
  0x0b,                    // section length 11
  0x02,                    // globals count 2
  0x7f, 0x00,              // global #1: i32 immutable
  0x41, 0x00, 0x0b,        // i32.const 0
  0x7f, 0x00,              // global #2: i32 immutable
  0x41, 0x00, 0x0b,        // i32.const 0

  0x07,                    // section kind: Export
  0x27,                    // section length 39
  0x02,                    // exports count 2
                           // export # 0
  0x0f,                    // field name length: 15
  0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x67, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 
                           // field name: exported_global
  0x03, 0x02,              // kind: global index: 2
                           // export # 1
  0x11,                    // field name length: 17
  0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 
  0x6e,                    // field name: exported_function
  0x00, 0x02,              // kind: function index: 2

  0x09,                    // section kind: Element
  0x05,                    // section length 5
  0x01, 0x01, 0x00,        // segment count 1: flag: passive, element type: function
  0x01, 0x01,              // number of elements 1: index: 1

  0x0a,                    // section kind: Code
  0x1d,                    // section length 29
  0x02,                    // functions count 2
                           // function #1 $function_with_name
  0x0b,                    // body size 11
  0x00,                    // 0 entries in locals list
  0x20, 0x00,              // local.get $param_with_name_1
  0x1a,                    // drop
  0x20, 0x01,              // local.get $param_with_name_2
  0x1a,                    // drop
  0x20, 0x02,              // local.get $param_with_name_3
  0x1a,                    // drop
  0x0b,                    // end
                           // function #2 $exported_function_with_name
  0x0f,                    // body size 15
  0x02,                    // 2 entries in locals list
  0x02, 0x7f,              // 2 locals of type i32
  0x01, 0x7e,              // 1 local of type i64
  0x20, 0x00,              // local.get $local_with_name_1
  0x1a,                    // drop
  0x20, 0x01,              // local.get $local_with_name_2
  0x1a,                    // drop
  0x20, 0x02,              // local.get $local_with_name_3
  0x1a,                    // drop
  0x0b,                    // end

  0x0b,                    // section kind: Data
  0x0b,                    // section length 11
  0x01, 0x00,              // data segments count 1: flag: active no index
  0x41, 0x00, 0x0b,        // i32.const 0
  0x05,                    // source size 5
  0x66, 0x6f, 0x6f, 0x0a, 0x00,  // segment data

  0x00,                    // section kind: Unknown
  0xd8, 0x02,              // section length 344
  0x04,                    // section name length: 4
  0x6e, 0x61, 0x6d, 0x65,  // section name: name
  0x01,                    // name type: function
  0x4f,                    // payload length: 79
  0x03,                    // names count 3
  0x00, 0x1b,              // index 0 name length: 27
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 
  0x6e, 0x5f, 0x77, 0x69, 0x74, 0x68, 0x5f, 0x6e, 
  0x61, 0x6d, 0x65,        // name: imported_function_with_name
  0x01, 0x12,              // index 1 name length: 18
  0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 0x6e, 
  0x5f, 0x77, 0x69, 0x74, 0x68, 0x5f, 0x6e, 0x61, 
  0x6d, 0x65,              // name: function_with_name
  0x02, 0x1b,              // index 2 name length: 27
  0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 
  0x6e, 0x5f, 0x77, 0x69, 0x74, 0x68, 0x5f, 0x6e, 
  0x61, 0x6d, 0x65,        // name: exported_function_with_name
  0x02,                    // name type: local
  0x79,                    // payload length: 121
  0x03,                    // outer count 3
  0x00, 0x00,              // outer index 0 inner count 0
  0x01, 0x03,              // outer index 1 inner count 3
  0x00, 0x11,              // inner index 0 name length: 17
  0x70, 0x61, 0x72, 0x61, 0x6d, 0x5f, 0x77, 0x69, 
  0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x5f, 
  0x31,                    // name: param_with_name_1
  0x01, 0x11,              // inner index 1 name length: 17
  0x70, 0x61, 0x72, 0x61, 0x6d, 0x5f, 0x77, 0x69, 
  0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x5f, 
  0x32,                    // name: param_with_name_2
  0x02, 0x11,              // inner index 2 name length: 17
  0x70, 0x61, 0x72, 0x61, 0x6d, 0x5f, 0x77, 0x69, 
  0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x5f, 
  0x33,                    // name: param_with_name_3
  0x02, 0x03,              // outer index 2 inner count 3
  0x00, 0x11,              // inner index 0 name length: 17
  0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x5f, 0x77, 0x69, 
  0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x5f, 
  0x31,                    // name: local_with_name_1
  0x01, 0x11,              // inner index 1 name length: 17
  0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x5f, 0x77, 0x69, 
  0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x5f, 
  0x32,                    // name: local_with_name_2
  0x02, 0x11,              // inner index 2 name length: 17
  0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x5f, 0x77, 0x69, 
  0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x5f, 
  0x33,                    // name: local_with_name_3
  0x05,                    // name type: table
  0x12,                    // payload length: 18
  0x01,                    // names count 1
  0x00, 0x0f,              // index 0 name length: 15
  0x74, 0x61, 0x62, 0x6c, 0x65, 0x5f, 0x77, 0x69, 
  0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 
                           // name: table_with_name
  0x06,                    // name type: memory
  0x13,                    // payload length: 19
  0x01,                    // names count 1
  0x00, 0x10,              // index 0 name length: 16
  0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x5f, 0x77, 
  0x69, 0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 
                           // name: memory_with_name
  0x07,                    // name type: global
  0x49,                    // payload length: 73
  0x03,                    // names count 3
  0x00, 0x19,              // index 0 name length: 25
  0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x67, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x5f, 
  0x77, 0x69, 0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 
  0x65,                    // name: imported_global_with_name
  0x01, 0x10,              // index 1 name length: 16
  0x67, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x5f, 0x77, 
  0x69, 0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 
                           // name: global_with_name
  0x02, 0x19,              // index 2 name length: 25
  0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 
  0x5f, 0x67, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x5f, 
  0x77, 0x69, 0x74, 0x68, 0x5f, 0x6e, 0x61, 0x6d, 
  0x65,                    // name: exported_global_with_name
  0x08,                    // name type: element segment
  0x11,                    // payload length: 17
  0x01,                    // names count 1
  0x00, 0x0e,              // index 0 name length: 14
  0x65, 0x6c, 0x65, 0x6d, 0x5f, 0x77, 0x69, 0x74, 
  0x68, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 
                           // name: elem_with_name
