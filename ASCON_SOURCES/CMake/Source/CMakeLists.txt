cmake_minimum_required(VERSION 3.14)
project(Test VERSION "2022.1.11794.1")

SET(CMAKE_SUPPRESS_REGENERATION True)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)

# Libraries
if(MSVC)
  SET(Math_MATH_LIBRARY
      optimized ${CMAKE_BINARY_DIR}/../Release/c3d.lib
      debug ${CMAKE_BINARY_DIR}/../Debug/c3d.lib)
else()
  SET(Math_MATH_LIBRARY
      c3d)
endif()

SET(math_target ${Math_MATH_LIBRARY} CACHE FILEPATH "" FORCE)

SET(Test_OUTPUT "Test")

# Unicode build
OPTION(MathTest_USING_UNICODE "Enable Unicode support" ON)

OPTION(MathTest_L10N "Enable localization" OFF)

# Math names are not included by default
OPTION(MathTest_QT "Use Qt Library" OFF)
if(MathTest_QT)
  find_package( Qt5Core )
  find_package( Qt5Gui )
  find_package( Qt5OpenGL )
endif()

IF(MathTest_L10N)
  ADD_DEFINITIONS(
    -D__NATIVE_LANGUAGE__
    )
ENDIF(MathTest_L10N)


set(_C3D_FD "\"@pl@-bit VS2015 @symb@ \"")
set(_C3D_PV "\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}\"" )
set(_C3D_FV "\"${PROJECT_VERSION}\"" )
add_definitions( -DBI_FILEVERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions( -DBI_PRODUCTVERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions( -DBI_FILEVERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions( -DBI_PRODUCTVERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions( -DBI_FILEVERSION_BUILD=${PROJECT_VERSION_PATCH})
add_definitions( -DBI_PRODUCTVERSION_BUILD=${PROJECT_VERSION_PATCH})
add_definitions( -DBI_FILEVERSION_REVISION=${PROJECT_VERSION_TWEAK})
add_definitions( -DBI_PRODUCTVERSION_REVISION=${PROJECT_VERSION_TWEAK})
add_definitions( -DBI_PRODUCTVERSION=${_C3D_PV} )
add_definitions( -DBI_FILEVERSION=${_C3D_FV} )
message("Set project version to [${_C3D_FV}]")

if(CMAKE_CL_64)
  string(REPLACE "@pl@" "64" _C3D_FD ${_C3D_FD})
else()
  string(REPLACE "@pl@" "32" _C3D_FD ${_C3D_FD})
endif()

if(MathTest_USING_UNICODE)
  string(REPLACE "@symb@" "Unicode" _C3D_FD ${_C3D_FD})
else()
  string(REPLACE " @symb@" "" _C3D_FD ${_C3D_FD})
endif()

add_definitions( -DBI_FILEDESCRIPTION=${_C3D_FD} )
message("Set project description to [${_C3D_FD}]")

set(_C3D_TW "_T(\"Testing of geometrical kernel C3D [@qt@Visual C++ (@pl@), @rev@]\")")
if(MathTest_QT)
  string(REPLACE "@qt@" "Qt &" _C3D_TW ${_C3D_TW})
else()
  string(REPLACE "@qt@" "" _C3D_TW ${_C3D_TW})
endif()
if(CMAKE_CL_64)
  string(REPLACE "@pl@" "x64" _C3D_TW ${_C3D_TW})
else()
  string(REPLACE "@pl@" "Win32" _C3D_TW ${_C3D_TW})
endif()
string(REPLACE "@rev@" ${PROJECT_VERSION_PATCH}${PROJECT_VERSION_TWEAK} _C3D_TW ${_C3D_TW})
add_definitions( -D_TITLE_MAIN_WND_C3D_BLDNUM=${_C3D_TW} )
if(MathTest_L10N)
  add_definitions( -D__NATIVE_LANGUAGE__=Russian)
endif()

set(Create_SRC
  ./Create/test_constraint.cpp
  ./Create/test_arc.cpp
  ./Create/test_assembly.cpp
  ./Create/test_contour.cpp
  ./Create/test_curve.cpp
  ./Create/test_plane.h
  ./Create/test_line.cpp
  ./Create/test_multiline.cpp
  ./Create/test_fair_curve.cpp
  ./Create/test_fair_curve.h
  ./Create/test_fair_surface.cpp
  ./Create/test_fair_surface.h
  ./Create/test_point.cpp
  ./Create/test_curve3d.cpp
  ./Create/test_sheet.cpp
  ./Create/test_shell.cpp
  ./Create/test_map.cpp
  ./Create/test_solid.cpp
  ./Create/test_space.h
  ./Create/test_surface_.cpp
  ./Create/test_point3d.cpp
  ./Create/test_surface.cpp
  ./Create/test_user.cpp
  ./Create/test_multithreading.cpp
  ./Create/test_mesh.cpp
  ./Create/test_mesh.h
  )
source_group(Create FILES ${Create_SRC})

set(Edit_SRC
  ./Edit/test_edit_contour.cpp
  ./Edit/test_edit_curve.cpp
  ./Edit/test_edit_plane.h
  ./Edit/test_edit_multiline.cpp
  ./Edit/test_edit_curve3d.cpp
  ./Edit/test_edit_map.cpp
  ./Edit/test_edit_solid.cpp
  ./Edit/test_edit_space.cpp
  ./Edit/test_edit_space.h
  ./Edit/test_edit_surface.cpp
  )
source_group(Edit FILES ${Edit_SRC})

set(Main_SRC
  ./Main/test_draw.cpp
  ./Main/test_draw.h
  ./Main/test_frame.h
  ./Main/test_frame1.cpp
  ./Main/test_frame2.cpp
  ./Main/test_frame3.cpp
  ./Main/test_service.cpp
  ./Main/test_service.h
  ./Main/test_variables.cpp
  ./Main/test_variables.h
  )
source_group(Main FILES ${Main_SRC})

set(Make_SRC
  ./Make/test_computation.cpp
  ./Make/test_computation.h
  ./Make/test_converter.cpp
  ./Make/test_converter.h
  ./Make/test_mates.cpp
  ./Make/test_mates.h
  ./Make/test_rendering.cpp
  ./Make/test_rendering.h
  ./Make/test_rendering_.cpp
  ./Make/test_rendering_context.cpp
  ./Make/test_rendering_context.h
  )
source_group(Make FILES ${Make_SRC})

set(Manager_SRC
  ./Manager/test_comanager.h
  ./Manager/test_comanager.cpp
  ./Manager/test_gr_draw.cpp
  ./Manager/test_gr_draw.h
  ./Manager/test_manager.cpp
  ./Manager/test_manager.h
  ./Manager/test_property.cpp
  ./Manager/test_property.h
  ./Manager/test_property_title.h
  ./Manager/test_style.h
  ./Manager/test_temporal_plane.cpp
  ./Manager/test_temporal_plane.h
  ./Manager/test_temporal.cpp
  ./Manager/test_temporal.h
  ./Manager/test_tree.cpp
  ./Manager/test_tree.h
  ./Manager/test_tree_filter.cpp
  ./Manager/test_tree_filter.h
  ./Manager/test_window.cpp
  ./Manager/test_window.h
  ./Manager/test_window_add.cpp
  ./Manager/test_window_find.cpp
  ./Manager/test_window_move.cpp
  )
source_group(Manager FILES ${Manager_SRC})

set(Samples_SRC
  ./Samples/test_samples.h
  ./Samples/test_sample_animation_of_bending.cpp
  ./Samples/test_sample_attributes.cpp
  ./Samples/test_sample_user_attributes.h
  ./Samples/test_sample_user_attributes.cpp
  ./Samples/test_sample_solid_elementary.cpp
  ./Samples/test_sample_solid_splitting.cpp
  ./Samples/test_sample_solids_classification.cpp
  ./Samples/test_sample_parametric_sketch.cpp
  ./Samples/test_sample_read_write_constraints.cpp
  ./Samples/test_sample_read_write_c3d_as_part.cpp
  ./Samples/test_sample_spinning_block.cpp
  ./Samples/test_sample_check_collisions.cpp
  ./Samples/test_sample_collision_detection.cpp
  ./Samples/test_sample_wireframe.cpp
  ./Samples/test_sample_converter.cpp
  ./Samples/test_sample_lofted_bend.cpp
  ./Samples/test_sample_dodecahedron.cpp
  ./Samples/test_sample_close_corner.cpp
  )
source_group(Samples FILES ${Samples_SRC})

if(MathTest_QT)
  set(QtTest_SRC
    ./Qt/test_main.cpp
    ./Qt/test_main_window.h
    ./Qt/test_main_window.cpp
    ./Qt/test_child_window.h
    ./Qt/test_child_window.cpp
    ./Qt/test_property_dialogs.h
    ./Qt/test_property_dialogs.cpp
    ./Qt/test_main_dialogs.h
    ./Qt/test_main_dialogs.cpp
  )
  source_group(QtTest FILES ${QtTest_SRC})

  set(MOC_Files
    ./Qt/test_main_window.h
    ./Qt/test_child_window.h
    ./Qt/test_property_dialogs.h
    ./Qt/test_main_dialogs.h
  )

  QT5_WRAP_CPP(MOC_Gui
    ${MOC_Files}
  )

  set(RCC_Files
    ./Qt/test.qrc
  )

  QT5_ADD_RESOURCES(RCC_Gui
    ${RCC_Files}
    )

  include_directories(${QT_USE_FILE})

else(MathTest_QT)
  set(WinTest_SRC
    ./Win/test.cpp
    ./Win/test.h
    ./Win/test.rc
    ./Win/test_dialogs.cpp
    ./Win/test_frame.cpp
    ./Win/test_prompt.h
    ./Win/test_set_filter.cpp
    ./Win/test_set_property.cpp
    ./Win/test_set_tree.cpp
    ./Win/test_std_afx.cpp
    ./Win/test_std_afx.h
    ./Win/test_window_graphic.cpp
  )
  source_group(WinTest FILES ${WinTest_SRC})
endif(MathTest_QT)

if(MathTest_WITH_INSTALLER)
  include_directories(
    ${VLD_INCLUDE_DIRS}
    ${CMAKE_INSTALL_PREFIX}/Include
    ${CMAKE_INSTALL_PREFIX}/c3d/Include
    ${CMAKE_INSTALL_PREFIX}/Include/c3d
    ${Test_SOURCE_DIR}/Create
    ${Test_SOURCE_DIR}/Edit
    ${Test_SOURCE_DIR}/Main
    ${Test_SOURCE_DIR}/Make
    ${Test_SOURCE_DIR}/Manager
    ${Test_SOURCE_DIR}/Samples
    ${Test_SOURCE_DIR}/Win
    ${Test_SOURCE_DIR}/Qt
  )
  link_directories(
    ${CMAKE_INSTALL_PREFIX}/Release
    ${CMAKE_INSTALL_PREFIX}/Debug
    ${CMAKE_INSTALL_PREFIX}/Release/x64/Exe
    ${CMAKE_INSTALL_PREFIX}/Debug/x64/Exe
  )
#  endif()
else(MathTest_WITH_INSTALLER)
  include_directories(
    ${VLD_INCLUDE_DIRS}
    ${Test_SOURCE_DIR}/Create
    ${Test_SOURCE_DIR}/Edit
    ${Test_SOURCE_DIR}/Main
    ${Test_SOURCE_DIR}/Make
    ${Test_SOURCE_DIR}/Manager
    ${Test_SOURCE_DIR}/Samples
    ${Test_SOURCE_DIR}/Win
    ${Test_SOURCE_DIR}/Qt
    ) 
endif(MathTest_WITH_INSTALLER)

if(MathTest_USING_UNICODE)
  add_definitions(
    -DUNICODE
    -D_UNICODE
    )
endif(MathTest_USING_UNICODE)

if(MSVC)
  add_definitions(
    -D_WIN32_WINNT=0X0502
    -D_CRT_SECURE_NO_WARNINGS
    )
endif(MSVC)

if(MathTest_QT)
  add_definitions( 
    -D__USE_QT__
    )
endif(MathTest_QT)
  
if(MathTest_WITH_VLD)
  add_definitions(
    -DENABLE_VLD
    )
endif(MathTest_WITH_VLD)

# These strings are necessary to define version info and test app main window title.
  
if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG_INIT} -D_DEBUG -D_DRAWGI -DNOMINMAX /Zi /openmp /W4 /MP")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -D_SECURE_SCL=0 -DNOMINMAX /Zi /openmp /W4 /MP")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
  set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG_INIT} -D_DRAWGI -Wno-deprecated-declarations -fno-operator-names -pthread -fvisibility=hidden -Wno-attributes")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -D_SECURE_SCL=0 -Wno-deprecated-declarations -fno-operator-names -pthread -fvisibility=hidden -Wno-attributes")
elseif(APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG_INIT} -D_DRAWGI -Wno-deprecated-declarations -fno-operator-names -fvisibility=hidden -Wno-attributes")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -D_SECURE_SCL=0 -Wno-deprecated-declarations -fno-operator-names -fvisibility=hidden -Wno-attributes")
else()
  set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG_INIT} -Wno-deprecated-declarations -fno-operator-names -fopenmp -fvisibility=hidden -Wno-attributes")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -D_SECURE_SCL=0 -Wno-deprecated-declarations -fno-operator-names -fopenmp -fvisibility=hidden -Wno-attributes")
  if(NOT MathTest_WITH_ADDRESS_SANITIZER AND NOT MathTest_WITH_THREAD_SANITIZER AND NOT MathTest_WITH_UB_SANITIZER)
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -D_DRAWGI")
  endif()
endif()

if(MathTest_QT)
  if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_INIT} /ENTRY:mainCRTStartup")
  endif(MSVC)

  add_executable(${Test_OUTPUT} WIN32
    ${Create_SRC}
    ${Edit_SRC}
    ${Main_SRC}
    ${Make_SRC}
    ${Manager_SRC}
    ${Samples_SRC}
    ${QtTest_SRC}
    ${MOC_Gui}
    ${RCC_Gui}
    )
else(MathTest_QT)
  add_executable(${Test_OUTPUT} WIN32
    ${Create_SRC}
    ${Edit_SRC}
    ${Main_SRC}
    ${Make_SRC}
    ${Manager_SRC}
    ${Samples_SRC}
    ${Win_SRC}
    ${WinTest_SRC}
    )
endif(MathTest_QT)

if(NOT MSVC)
endif()

if(MathTest_QT)
  qt5_use_modules( ${Test_OUTPUT} Core Gui OpenGL )
endif(MathTest_QT)

if(MSVC)
  target_link_libraries(${Test_OUTPUT}
    ${math_target}
    ${VLD_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES}
    opengl32
    glu32
    comctl32
    )
else(MSVC)
  find_package(OpenGL REQUIRED)
  set(THREAD_LIBRARY)
  if(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    set(THREAD_LIBRARY thr)
  endif()
  target_link_libraries(
    ${Test_OUTPUT}
    ${math_target}
    ${VLD_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLU_LIBRARY}
    ${THREAD_LIBRARY}
  )
endif(MSVC)

install(TARGETS ${Test_OUTPUT}
        CONFIGURATIONS Release
        RUNTIME DESTINATION Release
        ARCHIVE DESTINATION Release
)
install(TARGETS ${Test_OUTPUT}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION Debug
        ARCHIVE DESTINATION Debug
)
if(MSVC)
  install(FILES $<TARGET_PDB_FILE:${Test_OUTPUT}>
          CONFIGURATIONS Release
          DESTINATION Release
          OPTIONAL)
  install(FILES $<TARGET_PDB_FILE:${Test_OUTPUT}>
          CONFIGURATIONS Debug
          DESTINATION Debug
          OPTIONAL)
endif()

if(MathTest_WITH_INSTALLER)
  install(TARGETS ${Test_OUTPUT}
          CONFIGURATIONS Release
          RUNTIME COMPONENT c3d-kompas DESTINATION Release/x64/Exe
          ARCHIVE COMPONENT c3d-kompas DESTINATION Release/x64/Exe
          
  )
  install(TARGETS ${Test_OUTPUT}
          CONFIGURATIONS Debug
          RUNTIME COMPONENT c3d-kompas DESTINATION Debug/x64/Exe
          ARCHIVE COMPONENT c3d-kompas DESTINATION Debug/x64/Exe
  )
  if(MSVC)
    install(FILES $<TARGET_PDB_FILE:${Test_OUTPUT}>
            CONFIGURATIONS Release
            DESTINATION Release/x64/Exe
            COMPONENT c3d-kompas
            OPTIONAL)
    install(FILES $<TARGET_PDB_FILE:${Test_OUTPUT}>
            CONFIGURATIONS Debug
            DESTINATION Debug/x64/Exe
            COMPONENT c3d-kompas
            OPTIONAL)
  endif()
  add_custom_command(TARGET ${Test_OUTPUT}
    POST_BUILD
    COMMAND cmake -DBUILD_TYPE=$<CONFIGURATION> -DCMAKE_INSTALL_COMPONENT=c3d-kompas -P cmake_install.cmake)
endif()

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/../Include) 
