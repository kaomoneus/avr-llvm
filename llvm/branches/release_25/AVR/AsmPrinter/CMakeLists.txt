include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_partially_linked_object(LLVMAVRAsmPrinter
  AVRAsmPrinter.cpp
  )

target_name_of_partially_linked_object(LLVMAVRCodeGen n)

add_dependencies(LLVMAVRAsmPrinter ${n})
