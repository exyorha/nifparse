add_library(nifparse STATIC
  include/nifparse/bytecode.h
  include/nifparse/BytecodeReader.h
  include/nifparse/ConstantDataStream.h
  include/nifparse/FileDataStream.h
  include/nifparse/INIFDataStream.h
  include/nifparse/NIFFile.h
  include/nifparse/PrettyPrinter.h
  include/nifparse/Serializer.h
  include/nifparse/SerializerContext.h
  include/nifparse/Symbol.h
  include/nifparse/SymbolTable.h
  include/nifparse/Types.h
  include/nifparse/TypeDescription.h
  nifparse/BytecodeReader.cpp
  nifparse/ConstantDataStream.cpp
  nifparse/FileDataStream.cpp
  nifparse/NIFFile.cpp
  nifparse/PrettyPrinter.cpp
  nifparse/Serializer.cpp
  nifparse/SerializerContext.cpp
  nifparse/Symbol.cpp
  nifparse/SymbolTable.cpp
  nifparse/TypeDescription.cpp

  ${CMAKE_CURRENT_BINARY_DIR}/nif_bytecode.cpp
)
target_include_directories(nifparse PUBLIC include)
target_link_libraries(nifparse PRIVATE halffloat)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nif_bytecode.cpp
  COMMAND
    ${RUBY_EXECUTABLE}
    ${PROJECT_SOURCE_DIR}/generator/generate_bytecode
    ${PROJECT_SOURCE_DIR}/nifxml/nif.xml
    ${CMAKE_CURRENT_BINARY_DIR}/nif_bytecode.cpp
  MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/nifxml/nif.xml
  VERBATIM)
