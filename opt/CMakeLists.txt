# pugixml
include_directories(${CMAKE_SOURCE_DIR}/opt/pugixml/src)
aux_source_directory(${CMAKE_SOURCE_DIR}/opt/pugixml/src PUGIXML_SOURCE_FILES)
add_library(pugixml ${PUGIXML_SOURCE_FILES})
target_include_directories(pugixml PUBLIC ${CMAKE_SOURCE_DIR}/opt/pugixml/src)

# fmt
include_directories(${CMAKE_SOURCE_DIR}/opt/fmt/include)
aux_source_directory(${CMAKE_SOURCE_DIR}/opt/fmt/src FMT_SOURCE_FILES)
add_library(fmt ${FMT_SOURCE_FILES})
target_include_directories(fmt PUBLIC ${CMAKE_SOURCE_DIR}/opt/fmt/include)
