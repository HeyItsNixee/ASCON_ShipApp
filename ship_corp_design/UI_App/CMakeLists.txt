cmake_minimum_required(VERSION 3.14)

project(TestAppProj LANGUAGES CXX)

find_package( Qt6Core )
find_package( Qt6Widgets )
find_package( Qt6Gui )


set(BACK_ ${CMAKE_CURRENT_SOURCE_DIR}/../BackEnd_App)
include_directories(${BACK_})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestAppProj WIN32
  main.cpp
  MainWnd.h
  MainWnd.cpp
  DrawingWdgt.h
  DrawingWdgt.cpp
  MapWdgt.h
  MapWdgt.cpp
  ui_MainWnd.h
  MainWnd.qrc
)


target_link_libraries(TestAppProj PRIVATE
    Qt::Widgets Qt::Gui Qt::Core ShipSection
)
