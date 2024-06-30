////////////////////////////////////////////////////////////////////////////////
//
// Изогеометрическое моделирование кривых высокого качества по критериям плавности.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TEST_FAIR_SURF_H
#define __TEST_FAIR_SURF_H

#include <mb_data.h>
#include <cur_nurbs3d.h>
#include <tool_cstring.h>

//--------------------------------------------------------------------
// Тестовое построение сплайновой поверхности
//---
void CreateFairSurface( int type );

//--------------------------------------------------------------------
// Построение сплайновой поверхности
//---
int Test_CreateSplineSurface();

int Test_CreateSplineSurface2();

int Test_CreateSplineSurfaceKlein();

// ---------------------------------------------------------------- -
// Построение сплайновой поверхности на S-многограннике в открытом формате.
//---
int Test_NetToNurbs();


//------------------------------------------------------------------------------------
// Анализ Nurbs поверхности.
//---
int Test_AnalyzeNurbsSurface();

//------------------------------------------------------------------------------------
// Построение B-поверхности.
//---
int Test_PllnesToNurbsSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ );

//---------------------------------------------------------------------------------
// Установить параметры построения Nurbs поверхности.
// ---
int  SetParamsToCreateNurbs( MbFairCurveData & fData, bool & closed, MbeFairSplineFormat & initFormat );

//------------------------------------------------------------------------------
// Читать NURBS кривую. \en Read the NURBS curve.
// ---
int ReadNurbsCurve( MbNurbs3D *& curv,
                    size_t & degreeBspline,
                    bool & closedBspline,
                    int & typeControlPolygonBspline,
                    size_t & nBspline

);
//---------------------------------------------------------------------------------
// Напрямую создать Nurbs поверхность на каркасе B-кривых.
// ---
int Test_BCurvesToNurbsSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ );

//---------------------------------------------------------------------------------
// Построить Nurbs поверхность степеней (1, m) на каркасе ломаных, объединенных в Nurbs степеней (1, 1).
// Построить Nurbs поверхность степеней (n, m) на каркасе B-кривых Nurbs поверхности степеней (1, m).
// ---
int Test_CreateBSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ );

int Test_CreateFairSurface( MbFairCurveData & curveData );

//------------------------------------------------------------------------------
// Открыть файл для отладочной печати.
// ---
int init_surf(/*MbFairCurveData & fData*/);

//------------------------------------------------------------------------------
// Закрыть файл для отладочной печати.
// ---
int close_surf();

//---------------------------------------------------------------------------------
// Транспонировать Nurbs поверхность.
// ---
int Test_TransponBSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ );

//---------------------------------------------------------------------------------
// Образцы построения B-поверхности.
// ---
int Test_Samples(/*MbFairCurveData & curveData*/ /*, MbFairBSurfData & surfData*/);

//---------------------------------------------------------------------------------
// Вытащить управляющую кривую из поверхности.
// ---
int Test_CurveFromSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ );

//---------------------------------------------------------------------------------
// Вставить управляющую кривую в поверхность.
// ---
int Test_CurveIntoSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ );

//----------------------------------------------------------------------------
// Преобразовать замкнутый S-полигон в открытый незамкнутый.
//
int ClosedToOpen( SArray<MbCartPoint3D> & v, //исходный локальный полигон/
                  size_t n,
                  size_t degree,            //степень кривой
                  SArray<MbCartPoint3D>  & v2,
                  size_t & n2);

//--------------------------------------------------------------------------------
// Читать 3D полилинии из Геометрического Буфера.
//---
int ReadPllnesFromGeomBuffer();

//---------------------------------------------------------------------------------
// Прямое определение Nurbs поверности на точках сети.
// ---
int PllnesToNurbsSurface11( MbFairCurveData & curveData,
                            SArray<MbCartPoint3D> pnts,
                            size_t kArray,
                            size_t lArray,
                            MbeFairSplineFormat initFormat );


#endif // __TEST_FAIR_SURF_H
