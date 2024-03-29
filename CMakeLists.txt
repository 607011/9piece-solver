cmake_minimum_required(VERSION 3.20)
PROJECT(9piece_solver CXX)

set(CMAKE_CXX_STANDARD 11)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -pedantic")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
endif (UNIX)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /wd4365 /wd4710 /wd4711")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Od")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /O2")
endif (MSVC)

add_executable(9piece_solver 
  src/main.cpp
  src/puzzle.cpp
  src/solver.cpp
  src/util.cpp
)

if (UNIX)
	add_custom_command(TARGET 9piece_solver 
	POST_BUILD
	COMMAND strip 9piece_solver)
endif (UNIX)


install(TARGETS 9piece_solver
  CONFIGURATIONS Release
  RUNTIME DESTINATION "$ENV{HOME}/bin")
