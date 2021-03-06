cmake_minimum_required(VERSION 3.2)
project(cc_log)

## Adjust to your needs

## DART Implementation
set(DASH_DART_IMPL "MPI")
## Project Sources
set(SOURCES "main.cc")

set(CXX_FLAGS "-O0 -Wall -g")

## Do not edit below this line unless you know what you are doing

if("$ENV{DASH_BASE}" STREQUAL "")
  set(DASH_BASE_DIR "$ENV{HOME}/dash-graph")
else()
  set(DASH_BASE_DIR $ENV{DASH_BASE})
endif()

set(DASH_BASE_CMAKE_DIR "${DASH_BASE_DIR}/cmake")

find_package("DASH-${DASH_DART_IMPL}" REQUIRED HINTS ${DASH_BASE_CMAKE_DIR})

message("Looking for DASH installation in: ${DASH_BASE_DIR}")
if(DASH-MPI_FOUND)
  message("DASH ${DASH_DART_IMPL} found")
endif()

if(DART-MPI_FOUND)
  message("DART ${DASH_DART_IMPL} found")
endif()

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${DASH_CXX_FLAGS} ${CXX_FLAGS}" )
string(TOLOWER ${DASH_DART_IMPL} DASH_DART_IMPL_LC)

## set dependencies and build options
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${DASH_LIBRARIES})

## Print summary
message("Sources:         ${SOURCES}")
message("DASH Libraries:  ${DASH_LIBRARIES}")
message("CXX Flags:       ${CMAKE_CXX_FLAGS}")
