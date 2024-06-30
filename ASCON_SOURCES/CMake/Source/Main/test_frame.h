﻿#ifndef __TEST_FRAME_H
#define __TEST_FRAME_H


#include <system_types.h>


//------------------------------------------------------------------------------
// Обработка команд главного меню окна видов
// ---
bool MenuCommand( uint wParam );


//------------------------------------------------------------------------------
// Выполнение расчетов
// ---
bool MakeAll( uint wParam );


//------------------------------------------------------------------------------
// Построение и редактирование объектов на плоскости
// ---
bool CreatePlaneObjects( uint wParam );


//------------------------------------------------------------------------------
// Построение и редактирование объектов
// ---
bool CreateSpaceObjects( uint wParam );


#endif // __FRAME_H
