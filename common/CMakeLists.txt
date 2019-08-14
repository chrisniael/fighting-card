project(common)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/opt/asio/asio/include)
include_directories(${CMAKE_SOURCE_DIR}/opt/spdlog/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} pthread curl fmt)
target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_SOURCE_DIR}
         ${CMAKE_SOURCE_DIR}/opt/asio/asio/include
         ${CMAKE_SOURCE_DIR}/opt/spdlog/include)
