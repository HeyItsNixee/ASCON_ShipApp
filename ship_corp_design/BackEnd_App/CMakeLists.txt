#в 2023practice/TestApp/Source
cmake_minimum_required (VERSION 3.14)
project ("ShipProject")
add_definitions(-DUNICODE -D_UNICODE)

set(DEBUG ${C3D_DEBUG_DIR}/c3d.lib)
set(RELEASE ${C3D_RELEASE_DIR}/c3d.lib)
set(INCLUDES ${C3D_INCLUDE_DIR})
include_directories(${INCLUDES})


set(SOURCE_LIB ShipSection.h ShipSection.cpp)
add_library(ShipSection STATIC ${SOURCE_LIB})
set(FILES_ FileManipulation.h FileManipulation.cpp)
add_library(SaveLoadFiles STATIC ${FILES_})


target_link_libraries(ShipSection ${DEBUG})
target_link_libraries(ShipSection ${RELEASE})

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR/Build/Libraries})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR/Build/Executables})

target_link_directories(ShipSection PRIVATE ${C3D_DEBUG_DIR}/../)
target_link_directories(SaveLoadFiles PRIVATE ${C3D_DEBUG_DIR}/../)