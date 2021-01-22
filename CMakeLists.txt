cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME 2048)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)


set(DEBUG 1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# *******************************************
# *********** GEGELATI LIBRARY **************
# *******************************************

if(WIN32)
    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    MESSAGE("LIBS_DIR : ${LIBS_DIR}")
    # find the gegelatilib-x.y.z folder in the lib directory.
    file(GLOB GEGELATI_ROOT_DIR "${LIBS_DIR}/gegelatilib-[\\.|0-9]*")
    MESSAGE("GEGELATI_ROOT_DIR : ${GEGELATI_ROOT_DIR}")
    set(GEGELATI_DIR ${GEGELATI_ROOT_DIR}/CMake)
    MESSAGE("GEGELATI_DIR : ${GEGELATI_DIR}")
endif()
find_package(GEGELATI)

if (WIN32)
    file(GLOB
         GEGELATI_DLL
         ${GEGELATI_ROOT_DIR}/bin/*.dll
    )
    MESSAGE("Copy GEGELATI DLLs into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else()
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    endif()
endif()

# *******************************************
# ************** MON BORDEL *****************
# *******************************************

include_directories(${GEGELATI_INCLUDE_DIRS})
add_executable(${PROJECT_NAME}
               ../src/main.cpp
               ../src/game2048.cpp
               ../include/game2048.h
)
target_link_libraries(${PROJECT_NAME} ${GEGELATI_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")
