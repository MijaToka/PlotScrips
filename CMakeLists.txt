cmake_minimum_required(VERSION 3.26.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(H4l-scripts VERSION 0.1)

find_package(ROOT 6.40 CONFIG REQUIRED COMPONENTS Core Tree RIO Hist ROOTDataFrame)

add_executable(H4l_Fill H4lPlots/src/H4l_Fill.C)

install(TARGETS H4l_Fill 
  RUNTIME DESTINATION bin
)

target_include_directories(H4l_Fill
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(H4l_Fill PUBLIC ${ROOT_LIBRARIES})

