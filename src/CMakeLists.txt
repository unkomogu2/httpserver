include_directories(
  PUBLIC ${PROJECT_SOURCE_DIR}/include/httpserver/
)
add_executable(main.bin main.cpp)
if(WIN32)
  target_link_libraries(main.bin wsock32 ws2_32)
endif()

