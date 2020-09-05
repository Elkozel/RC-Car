# CMakeList.txt : CMake project, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(include)

# Add source to this project's executable.
add_executable (RC-Car "src/server.cpp" "src/server.h" "src/public/Drive.cpp" "include/RC-Car/Drive.h")

# TODO: Add tests and install targets if needed.
find_library(wiringPi_LIB wiringPi)
find_package( Threads )
target_link_libraries(RC-Car ${wiringPi_LIB})
target_link_libraries(RC-Car ${CMAKE_THREAD_LIBS_INIT} )

add_compile_definitions(SOFTPWM)
