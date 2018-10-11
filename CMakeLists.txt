cmake_minimum_required(VERSION 3.12)
set(EXECUTABLE_NAME ReactionDiffusion2)
project(${EXECUTABLE_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wno-class-memaccess -Wextra -Wnon-virtual-dtor -pedantic")

set(SOURCES src/main.cpp include/Convolution.hpp include/ReactionState.hpp include/CellConcentration.hpp include/Util.hpp include/Random.hpp include/ReactionDiffusion.hpp include/Seeders.hpp include/ReactionModel.hpp)
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Set cmake module path
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# Include SFML
cmake_policy(SET CMP0074 OLD)
find_package(SFML REQUIRED system window graphics network audio)
if(SFML_FOUND)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SFML_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
else()
    MESSAGE(WARNING "SFML Not Found")
endif()