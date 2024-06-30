cmake_minimum_required(VERSION 3.14)

project(TestAppProj LANGUAGES CXX)

set(C3D_INCLUDE_DIR "C:/" CACHE PATH "Choose C3D Include directory")
set(C3D_DEBUG_DIR "C:/" CACHE PATH "Choose C3D Debug directory")
set(C3D_RELEASE_DIR "C:/" CACHE PATH "Choose C3D Release directory")

include_directories(${C3D_INCLUDE_DIR})

add_subdirectory ( UI_App )
add_subdirectory ( BackEnd_App )