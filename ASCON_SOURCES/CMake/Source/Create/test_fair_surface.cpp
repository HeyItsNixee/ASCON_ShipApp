#include <test.h>
#include <test_manager.h>
#include <test_fair_curve.h>
#include <test_service.h>
#include <action_curve3d.h>
#include <space_instance.h>
#include <cur_nurbs3d.h>
#include <cur_nurbs.h>
#include <alg_circle_curve.h>
#include <plane_instance.h>
#include <math_namespace.h>
#include <surf_spline_surface.h>
#include <test_fair_surface.h>

#include <last.h>

using namespace c3d;

FILE *sf_prt;


//15 10 2019
// Исправлена нумерация в списке ошибок на английском.
//------------------------------------------------------------------------------
// Программа для отладки, тестирования и демонстрации функциональных возможностей комплекса 
// программ FairCurveModeler изогеометрического моделирования  кривых высокого качества по критериям плавности.
// ---
void CreateFairSurface( int type )
{
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;
  SArray<double> knots;
  SArray<double> wts;
  //  MbCurve3D * resCurve = nullptr;
  //  MbCurve3D * pllne = nullptr;
  //  MbCurve3D * result = nullptr;
  //  MbNurbs3D * nurbsCurve = nullptr;
  //  ptrdiff_t degree = Math::curveDegree; 
  //  bool closed = false;

  MbFairCurveData fData;

  init_surf( /*fData*/ );
  //bool yes_examples = false;
  //int num_task = 0;

  /*DEBUG*/// WrBSurfaceToGB(fData); close_surf();  return;

  try {  // Перехват аппаратного исключения. 
    // В проекте для пункта меню C/C++ ->Code Generation->Enable C++ Exceptions 
    // устанавливаем значение Yes With SEH Exceptions (/EHa).

    //End DEBUG Отладка программ. Перевод на стандарт C3D

    //  TEST.  Методы для разработчиков САПР и приложений
    switch ( type ) {
      case 1: //CM_FAIRBSURFACE : 
        Test_CreateFairSurface( fData );
        break;
    }

    // Закрытие отчетных файлов.
    close_surf();

    return;
  } // try

  catch ( ... ) { // Обработка аппаратного исключения.
    MessageBoxEx( _T( "*** НЕКОРРЕКТНЫЕ ОБЪЕКТ / ПАРАМЕТРЫ!" ), IDS_TITLE_ERR );
    // Закрытие отладочных файлов.
    CloseFcm();
  }

  return;
}


//-----------------------------------------------------------------
// Построение сплайновой поверхности по S-фрейму в открытом формате
//---
int Test_CreateSplineSurface()
{
  /*
  MbSplineSurface( size_t uDeg, size_t vDeg, bool uCls, bool vCls, const Array2<MbCartPoint3D> & initPoints, const SArray<double> & initUKnots, const SArray<double> & initVKnots );
  MbSplineSurface( size_t uDeg, size_t vDeg, bool uCls, bool vCls, const Array2<MbCartPoint3D> & initPoints, const Array2<double> & initWeights, const SArray<double> & initUKnots, const SArray<double> & initVKnots );
  */

  size_t uCount = 5;
  size_t vCount = 6;
  MbCartPoint3D p;

  Array2<MbCartPoint3D> initPoints;
  Array2<double> initWeights;
  SArray<double> arParamsU; // создать узловой вектор для u
  SArray<double> arParamsV; // создать узловой вектор для u

  //if (initPoints.SetSize(std_max(vCount, (ptrdiff_t)1), std_max(uCount, (ptrdiff_t)1))) {
  if ( initPoints.SetSize( vCount, uCount ) && initWeights.SetSize( vCount, uCount ) ) {
    for ( size_t i = 0; i < vCount; i++ ) {
      for ( size_t j = 0; j < uCount; j++ ) {
        p.x = (double)( j * 10 );
        p.y = (double)( i * 10 );
        p.z = (double)( j * 10 )*sin( ( 3.14 / uCount )*(double)j ) + (double)( i * 10 )*cos( ( 3.14 / vCount )*(double)i );
        //initPoints.Init(i, j, pList[i*uCount + j]);
        initPoints.Init( i, j, p );
        initWeights.Init( i, j, 1. );
      }
    }
  }
  else {
    return 19; // Недостаточно памяти.
  }
  SArray<double> initUKnots;
  SArray<double> initVKnots;

  size_t uDeg = 4;
  size_t vDeg = 4;

  //ptrdiff_t uDots = uCount - uDeg;
  //ptrdiff_t vDots = vCount - vDeg;

  //Открытый формат по U
  for ( size_t j = 0; j < uCount + uDeg; ++j ) {
    initUKnots.Add( (double)( j ) );
  }

  //Открытый формат по V
  for ( size_t i = 0; i < vCount + vDeg; ++i ) {
    initVKnots.Add( (double)i );
  }

  bool iUcls = false;
  bool iVcls = false;

  MbSplineSurface * nurbsSurface = nullptr;

  //MbResultType res = ::NurbsSurfacesShell(nsParams, snMaker, isPhantom, solid, progBar);
  //virtual MbSplineSurface * NurbsSurface(double, double, double, double, bool bmatch = false) const; // \ru Построить NURBS копию поверхности. \en Construct a NURBS copy of the surface. 
  //nurbsSurface = new MbSplineSurface(4, 3, false, false, spoints, sweghts, arParamsU, arParamsV);  //-V112
  nurbsSurface = new MbSplineSurface( uDeg, vDeg, iVcls, iUcls, initPoints, initWeights, initUKnots, initVKnots );

  //TestVariables::viewManager->AddObject( TestVariables::SOLID_Style, parts[i] );
  TestVariables::viewManager->AddObject( TestVariables::SURFACE_Style, nurbsSurface );

  // Отрисовать S-фрейм

  SArray<MbCartPoint3D> vrtx;
  MbCurve3D * result( nullptr );
  for ( size_t i = 0; i < vCount; ++i ) {
    for ( size_t j = 0; j < uCount; ++j ) {
      vrtx.Add( initPoints( i, j ) );
    }
    // Нарисовать ломаную строки
    ::SplineCurve( vrtx, iUcls, st_Polyline3D, result );
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
    vrtx.clear();
  }
  for ( size_t j = 0; j < uCount; ++j ) {
    for ( size_t i = 0; i < vCount; ++i ) {
      vrtx.Add( initPoints( i, j ) );
    }
    // Нарисовать ломаную столбца
    ::SplineCurve( vrtx, iUcls, st_Polyline3D, result );
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
    vrtx.clear();
  }

  return 0;
}


//--------------------------------------------------------------
//  Построить NURBS поверхность по S-фрейму в закрытом (Clamped) формате.
//---
int Test_CreateSplineSurface2()
{
  MbSplineSurface * splineSurface = nullptr;

  double u1 = 0.;
  double u2 = 1.;
  double v1 = 0.;
  double v2 = 1.;

  const size_t uDegree = 4; ///< Порядок по u.
  const size_t vDegree = 4; ///< Порядок по v.
  const size_t uCount = 5;  ///< Число точек по u.
  const size_t vCount = 6;  ///< Число точек по v.

  Array2<MbCartPoint3D> points; // точки
  Array2<double>        weights; // веса точек
  points.SetSize( vCount, uCount ); // точки
  weights.SetSize( vCount, uCount ); // веса точек

  SArray<double>        uKnots( uDegree + uCount, 1 );
  SArray<double>        vKnots( vDegree + vCount, 1 );

  {
    double du = ( u2 - u1 ) / (double)( uCount - 1 );
    ptrdiff_t p = uDegree - 1;
    size_t i = 0;
    SArray<double> pars( uDegree + uCount, 2 );
    for ( i = 0; i < uDegree + uCount; i++ ) {
      pars.Add( u1 + du * i ); //-V113
    }
    for ( i = 0; i < uDegree + uCount; i++ ) {
      if ( i < uDegree )
        uKnots.Add( u1 );
      else {
        if ( i < uCount/*+1*/ ) {
          // Попытка сделать равномерную параметризацию
          double knot = 0;
          for ( size_t j = i - p; j < i; j++ )
            knot += pars[j];

          knot /= p;  //-V113
          uKnots.Add( knot );
          //uKnots.Add( u1 + du * (double)(i + 1 - uDegree) );
        }
        else
          uKnots.Add( u2 );
      }
    }
  }
  {
    double dv = ( v2 - v1 ) / (double)( vCount - 1 );
    ptrdiff_t p = vDegree - 1;
    size_t i = 0;
    SArray<double> pars( vDegree + vCount, 2 );
    for ( i = 0; i < vDegree + vCount; i++ ) {
      pars.Add( v1 + dv * i ); //-V113
    }
    for ( i = 0; i < vDegree + vCount; i++ ) {
      if ( i < vDegree )
        vKnots.Add( v1 );
      else {
        if ( i < vCount/*+1*/ ) {
          // Попытка сделать равномерную параметризацию
          double knot = 0;
          for ( size_t j = i - p; j < i; j++ )
            knot += pars[j];

          knot /= p;  //-V113
          vKnots.Add( knot );
          //vKnots.Add( v1 + dv * (double)(i + 1 - vDegree) );
        }
        else
          vKnots.Add( v2 );
      }
    }
  }
  // Определим веса и точки 
  for ( size_t i = 0; i < vCount; i++ ) {
    MbCartPoint3D p;
    double w = 1.0;
    for ( size_t j = 0; j < uCount; j++ ) {
      p.x = (double)( j * 10 );
      p.y = (double)( i * 10 );
      p.z = (double)( j * 10 )*sin( ( 3.14 / uCount )*(double)j ) + (double)( i * 10 )*cos( ( 3.14 / vCount )*(double)i );
      //initPoints.Init(i, j, pList[i*uCount + j]);
      w = 1.;
      points.Init( i, j, p );
      weights( i, j ) = w;
    }
  }
  bool uClosedNurbs = false;
  bool vClosedNurbs = false;
  splineSurface = new MbSplineSurface( uDegree, vDegree, uClosedNurbs, vClosedNurbs, points, weights, uKnots, vKnots );
  if ( splineSurface != nullptr ) {
    ( (MbSplineSurface *)splineSurface )->SetLimitParam( u1, u2, v1, v2 );
    TestVariables::viewManager->AddObject( TestVariables::SURFACE_Style, splineSurface );

  }
  SArray<MbCartPoint3D> vrtx;
  MbCurve3D * result( nullptr );
  for ( ptrdiff_t i = 0; i < vCount; ++i ) {
    for ( ptrdiff_t j = 0; j < uCount; ++j ) {
      vrtx.Add( points( i, j ) );
    }
    // Нарисовать ломаную строки
    ::SplineCurve( vrtx, false, st_Polyline3D, result );
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
    vrtx.clear();
  }
  for ( ptrdiff_t j = 0; j < uCount; ++j ) {
    for ( ptrdiff_t i = 0; i < vCount; ++i ) {
      vrtx.Add( points( i, j ) );
    }
    // Нарисовать ломаную столбца
    ::SplineCurve( vrtx, false, st_Polyline3D, result );
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
    vrtx.clear();
  }

  return 0;
}


//----------------------------------------------------------------------------------
// Оснастить точку координатами.
//---
int setq_xyz( double p[3], double x, double y, double z )
{
  p[0] = x;
  p[1] = y;
  p[2] = z;

  return 0;
}


//-----------------------------------------------------------------
// Построение сплайновой поверхности на S-многограннике в открытом формате
// на примере моделирования односторонней поверхности "бутылки" Клейна.
//---
int Test_CreateSplineSurfaceKlein()
{
  double Klein[14][9][3];

  //0
  setq_xyz(Klein[0][0], 40.00000, 0.00000, 30.00000);
  setq_xyz(Klein[0][1], 55.00000, -16.00000, 25.00000);
  setq_xyz(Klein[0][2], 70.00000, 0.00000, 20.00000);
  setq_xyz(Klein[0][3], 55.00000, 16.00000, 25.00000);
  setq_xyz(Klein[0][4], 40.00000, 0.00000, 30.00000);
  setq_xyz(Klein[0][5], 55.00000, -16.00000, 25.00000);
  setq_xyz(Klein[0][6], 70.00000, 0.00000, 20.00000);
  setq_xyz(Klein[0][7], 55.00000, 16.00000, 25.00000);
  setq_xyz(Klein[0][8], 40.00000, 0.00000, 30.00000);

  //1
  setq_xyz(Klein[1][0], 68.00000, 0.00000, 52.00000);
  setq_xyz(Klein[1][1], 75.00000, -10.00000, 45.0000000);
  setq_xyz(Klein[1][2], 82.00000, 0.00000, 38.00000);
  setq_xyz(Klein[1][3], 75.00000, 10.00000, 45.000000);
  setq_xyz(Klein[1][4], 68.00000, 0.00000, 52.00000);
  setq_xyz(Klein[1][5], 75.00000, -10.00000, 45.0000000);
  setq_xyz(Klein[1][6], 82.00000, 0.00000, 38.00000);
  setq_xyz(Klein[1][7], 75.00000, 10.00000, 45.000000);
  setq_xyz(Klein[1][8], 68.00000, 0.00000, 52.00000);

  //2
  setq_xyz(Klein[2][0], 88.00000, 0.00000, 70.00000);
  setq_xyz(Klein[2][1], 95.00000, -7.00000, 70.00000);
  setq_xyz(Klein[2][2], 102.00000, 0.00000, 70.00000);
  setq_xyz(Klein[2][3], 95.00000, 7.00000, 70.00000);
  setq_xyz(Klein[2][4], 88.00000, 0.00000, 70.00000);
  setq_xyz(Klein[2][5], 95.00000, -7.00000, 70.00000);
  setq_xyz(Klein[2][6], 102.00000, 0.00000, 70.00000);
  setq_xyz(Klein[2][7], 95.00000, 7.00000, 70.00000);
  setq_xyz(Klein[2][8], 88.00000, 0.00000, 70.00000);

  //3
  setq_xyz(Klein[3][0], 80.00000, 0.00000, 85.00000);
  setq_xyz(Klein[3][1], 80.00000, -5.00000, 90.000000);
  setq_xyz(Klein[3][2], 80.00000, 0.00000, 95.000000);
  setq_xyz(Klein[3][3], 80.00000, 5.00000, 90.00000);
  setq_xyz(Klein[3][4], 80.00000, 0.00000, 85.00000);
  setq_xyz(Klein[3][5], 80.00000, -5.00000, 90.000000);
  setq_xyz(Klein[3][6], 80.00000, 0.00000, 95.000000);
  setq_xyz(Klein[3][7], 80.00000, 5.00000, 90.00000);
  setq_xyz(Klein[3][8], 80.00000, 0.00000, 85.00000);

  //4
  setq_xyz(Klein[4][0], 72.00000, 0.00000, 78.00000);
  setq_xyz(Klein[4][1], 60.00000, -10.00000, 85.00000);
  setq_xyz(Klein[4][2], 58.00000, 0.00000, 92.00000);
  setq_xyz(Klein[4][3], 60.00000, 10.00000, 85.00000);
  setq_xyz(Klein[4][4], 72.00000, 0.00000, 78.00000);
  setq_xyz(Klein[4][5], 60.00000, -10.00000, 85.00000);
  setq_xyz(Klein[4][6], 58.00000, 0.00000, 92.00000);
  setq_xyz(Klein[4][7], 60.00000, 10.00000, 85.00000);
  setq_xyz(Klein[4][8], 72.00000, 0.00000, 78.00000);

  //5
  setq_xyz(Klein[5][0], 70.00000, 0.00000, 70.00000);
  setq_xyz(Klein[5][1], 50.00000, -20.00000, 70.00000);
  setq_xyz(Klein[5][2], 30.00000, 0.00000, 70.00000);
  setq_xyz(Klein[5][3], 50.00000, 20.00000, 70.00000);
  setq_xyz(Klein[5][4], 70.00000, 0.00000, 70.00000);
  setq_xyz(Klein[5][5], 50.00000, -20.00000, 70.00000);
  setq_xyz(Klein[5][6], 30.00000, 0.00000, 70.00000);
  setq_xyz(Klein[5][7], 50.00000, 20.00000, 70.00000);
  setq_xyz(Klein[5][8], 70.00000, 0.00000, 70.00000);

  //6
  setq_xyz(Klein[6][0], 85.00000, 0.00000, 50.00000);
  setq_xyz(Klein[6][1], 50.00000, -35.00000, 50.00000);
  setq_xyz(Klein[6][2], 15.00000, 0.00000, 50.00000);
  setq_xyz(Klein[6][3], 50.00000, 35.00000, 50.00000);
  setq_xyz(Klein[6][4], 85.00000, 0.00000, 50.00000);
  setq_xyz(Klein[6][5], 50.00000, -35.00000, 50.00000);
  setq_xyz(Klein[6][6], 15.00000, 0.00000, 50.00000);
  setq_xyz(Klein[6][7], 50.00000, 35.00000, 50.00000);
  setq_xyz(Klein[6][8], 85.00000, 0.00000, 50.00000);

  //7
  setq_xyz(Klein[7][0], 90.00000, 0.00000, 30.00000);
  setq_xyz(Klein[7][1], 50.00000, -40.00000, 30.00000);
  setq_xyz(Klein[7][2], 10.00000, 0.00000, 30.00000);
  setq_xyz(Klein[7][3], 50.00000, 40.00000, 30.00000);
  setq_xyz(Klein[7][4], 90.00000, 0.00000, 30.00000);
  setq_xyz(Klein[7][5], 50.00000, -40.00000, 30.00000);
  setq_xyz(Klein[7][6], 10.00000, 0.00000, 30.00000);
  setq_xyz(Klein[7][7], 50.00000, 40.00000, 30.00000);
  setq_xyz(Klein[7][8], 90.00000, 0.00000, 30.00000);

  //8
  setq_xyz(Klein[8][0], 80.00000, 0.00000, 10.00000);
  setq_xyz(Klein[8][1], 50.00000, -30.00000, 10.0000);
  setq_xyz(Klein[8][2], 20.00000, 0.00000, 10.00000);
  setq_xyz(Klein[8][3], 50.00000, 30.00000, 10.00000);
  setq_xyz(Klein[8][4], 80.00000, 0.00000, 10.00000);
  setq_xyz(Klein[8][5], 50.00000, -30.00000, 10.0000);
  setq_xyz(Klein[8][6], 20.00000, 0.00000, 10.00000);
  setq_xyz(Klein[8][7], 50.00000, 30.00000, 10.00000);
  setq_xyz(Klein[8][8], 80.00000, 0.00000, 10.00000);

  //9
  setq_xyz(Klein[9][0], 70.00000, 0.00000, 20.00000);
  setq_xyz(Klein[9][1], 55.00000, -16.00000, 25.000);
  setq_xyz(Klein[9][2], 40.00000, 0.00000, 30.00000);
  setq_xyz(Klein[9][3], 55.00000, 16.00000, 25.0000);
  setq_xyz(Klein[9][4], 70.00000, 0.00000, 20.00000);
  setq_xyz(Klein[9][5], 55.00000, -16.00000, 25.000);
  setq_xyz(Klein[9][6], 40.00000, 0.00000, 30.00000);
  setq_xyz(Klein[9][7], 55.00000, 16.00000, 25.0000);
  setq_xyz(Klein[9][8], 70.00000, 0.00000, 20.00000);

  //10
  setq_xyz(Klein[10][0], 82.00000, 0.00000, 38.00000);
  setq_xyz(Klein[10][1], 75.00000, -10.000, 45.00000);
  setq_xyz(Klein[10][2], 68.00000, 0.00000, 52.00000);
  setq_xyz(Klein[10][3], 75.00000, 10.0000, 45.00000);
  setq_xyz(Klein[10][4], 82.00000, 0.00000, 38.00000);
  setq_xyz(Klein[10][5], 75.00000, -10.000, 45.00000);
  setq_xyz(Klein[10][6], 68.00000, 0.00000, 52.00000);
  setq_xyz(Klein[10][7], 75.00000, 10.0000, 45.00000);
  setq_xyz(Klein[10][8], 82.00000, 0.00000, 38.00000);

  //11
  setq_xyz(Klein[11][0], 102.00000, 0.00000, 70.00000);
  setq_xyz(Klein[11][1], 95.00000, -7.00000, 70.00000);
  setq_xyz(Klein[11][2], 88.00000, 0.00000, 70.00000);
  setq_xyz(Klein[11][3], 95.00000, 7.00000, 70.00000);
  setq_xyz(Klein[11][4], 102.00000, 0.00000, 70.00000);
  setq_xyz(Klein[11][5], 95.00000, -7.00000, 70.00000);
  setq_xyz(Klein[11][6], 88.00000, 0.00000, 70.00000);
  setq_xyz(Klein[11][7], 95.00000, 7.00000, 70.00000);
  setq_xyz(Klein[11][8], 102.00000, 0.00000, 70.00000);

  //12
  setq_xyz(Klein[12][0], 80.00000, 0.00000, 95.00000);
  setq_xyz(Klein[12][1], 80.00000, -5.00000, 90.0000);
  setq_xyz(Klein[12][2], 80.00000, 0.00000, 85.00000);
  setq_xyz(Klein[12][3], 80.00000, 5.00000, 90.00000);
  setq_xyz(Klein[12][4], 80.00000, 0.00000, 95.00000);
  setq_xyz(Klein[12][5], 80.00000, -5.00000, 90.0000);
  setq_xyz(Klein[12][6], 80.00000, 0.00000, 85.00000);
  setq_xyz(Klein[12][7], 80.00000, 5.00000, 90.00000);
  setq_xyz(Klein[12][8], 80.00000, 0.00000, 95.00000);


  //13
  setq_xyz(Klein[13][0], 58.00000, 0.00000, 92.00000);
  setq_xyz(Klein[13][1], 60.00000, -10.00000, 85.000);
  setq_xyz(Klein[13][2], 72.00000, 0.00000, 78.00000);
  setq_xyz(Klein[13][3], 60.00000, 10.00000, 85.0000);
  setq_xyz(Klein[13][4], 58.00000, 0.00000, 92.00000);
  setq_xyz(Klein[13][5], 60.00000, -10.00000, 85.000);
  setq_xyz(Klein[13][6], 72.00000, 0.00000, 78.00000);
  setq_xyz(Klein[13][7], 60.00000, 10.00000, 85.0000);
  setq_xyz(Klein[13][8], 58.00000, 0.00000, 92.00000);

  /*
  MbSplineSurface( size_t uDeg, size_t vDeg, bool uCls, bool vCls, const Array2<MbCartPoint3D> & initPoints, const SArray<double> & initUKnots, const SArray<double> & initVKnots );
  MbSplineSurface( size_t uDeg, size_t vDeg, bool uCls, bool vCls, const Array2<MbCartPoint3D> & initPoints, const Array2<double> & initWeights, const SArray<double> & initUKnots, const SArray<double> & initVKnots );
  */

  //size_t uOrder = 6;
  //size_t vOrder = 6;
  size_t uCount = 9;
  size_t vCount = 14;
  MbCartPoint3D p;

  Array2<MbCartPoint3D> initPoints;
  Array2<double> initWeights;
  SArray<double> arParamsU; // создать узловой вектор для u
  SArray<double> arParamsV; // создать узловой вектор для u

  //if (initPoints.SetSize(std_max(vCount, (ptrdiff_t)1), std_max(uCount, (ptrdiff_t)1))) {
  if (initPoints.SetSize(vCount, uCount) && initWeights.SetSize(vCount, uCount)) {
    for ( size_t i = 0; i < vCount; i++) {
      for ( size_t j = 0; j < uCount; j++)
      {
        p.x = Klein[i][j][0];
        p.y = Klein[i][j][1];
        p.z = Klein[i][j][2];
        //initPoints.Init(i, j, pList[i*uCount + j]);
        initPoints.Init(i, j, p);
        initWeights.Init(i, j, 1.);

        //midPnt += initPoints(i, j);
      }
    }
    //midPnt /= (double)(vCount * uCount);
  }
  else {
    return 19; // Недостаточно памяти.
  }
  SArray<double> initUKnots;
  SArray<double> initVKnots;

  size_t uDeg = 4;
  size_t vDeg = 4;

  //ptrdiff_t uDots = uCount - uDeg;
  //ptrdiff_t vDots = vCount - vDeg;

  //Открытый формат по U
  for ( size_t j = 0; j < uCount + uDeg; ++j) {
    initUKnots.Add((double)(j));
  }

  //Открытый формат по V
  for ( size_t i = 0; i < vCount + vDeg; ++i) {
    initVKnots.Add((double)i);
  }

  bool iUcls = false;
  bool iVcls = false;

  MbSplineSurface * nurbsSurface = nullptr;

  //MbResultType res = ::NurbsSurfacesShell(nsParams, snMaker, isPhantom, solid, progBar);
  //virtual MbSplineSurface * NurbsSurface(double, double, double, double, bool bmatch = false) const; // \ru Построить NURBS копию поверхности. \en Construct a NURBS copy of the surface. 
  //nurbsSurface = new MbSplineSurface(4, 3, false, false, spoints, sweghts, arParamsU, arParamsV);  //-V112
  nurbsSurface = new MbSplineSurface(uDeg, vDeg, iVcls, iUcls, initPoints, initWeights, initUKnots, initVKnots);

  //TestVariables::viewManager->AddObject( TestVariables::SOLID_Style, parts[i] );
  TestVariables::viewManager->AddObject(TestVariables::SURFACE_Style, nurbsSurface);

  // Отрисовать S-фрейм

  SArray<MbCartPoint3D> vrtx;
  MbCurve3D * result(nullptr);
  for ( size_t i = 0; i < vCount; ++i) {
    for ( size_t j = 0; j < uCount; ++j) {
      vrtx.Add(initPoints(i, j));
    }
    // Нарисовать ломаную строки
    ::SplineCurve(vrtx, iUcls, st_Polyline3D, result);
    TestVariables::viewManager->AddObject(TestVariables::CURVE_Style, result);
    vrtx.clear();
  }
  for ( size_t j = 0; j < uCount; ++j) {
    for ( size_t i = 0; i < vCount; ++i) {
      vrtx.Add(initPoints(i, j));
    }
    // Нарисовать ломаную столбца
    ::SplineCurve(vrtx, iUcls, st_Polyline3D, result);
    TestVariables::viewManager->AddObject(TestVariables::CURVE_Style, result);
    vrtx.clear();
  }

  return 0;
}


//------------------------------------------------------------------------------------
// Выделить каркас ломаных. Преобразовать в NURBS степеней (1х1).
//---
int Test_NetToNurbs()
{
  int outError = 0;

  MbCurve3D * pllne = nullptr;
  bool closed = false, cls = false;
  size_t kArray = 0, lArray = 0;
  SArray<MbCartPoint3D> pnts;
  size_t n = 0;
  //size_t number = 0;
  for ( size_t ir = 0; ; ir++ ) {
    //MbCurve3D *& pllne, bool & closed, size_t & n
    if ( ( outError = ReadPllne( pllne, cls, n ) ) > 0 ) {
      // Выбор ломаных закончен.
      if ( outError == 47 )
        break;
      continue;
    }
    if ( ir == 0 ) {
      closed = cls;
      kArray = n;
    }
    if ( closed != cls || kArray != n ) {
      // Ломаные несогласованы. Выберите правильную ломаную.
      ::MessageBoxEx( _T( "*** Ломаные несогласованы. Выберите правильную ломаную." ), IDS_TITLE_ERR );
      continue;
    }
    // Перенести точки в список
    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );

    for ( size_t irr = 0; irr < kArray; irr++ ) {
      pnts.Add( cvrts[irr] );
    }
    lArray++;
  }
  {
    /*
    MbSplineSurface( size_t uDeg, size_t vDeg, bool uCls, bool vCls, const Array2<MbCartPoint3D> & initPoints, const SArray<double> & initUKnots, const SArray<double> & initVKnots );
    MbSplineSurface( size_t uDeg, size_t vDeg, bool uCls, bool vCls, const Array2<MbCartPoint3D> & initPoints, const Array2<double> & initWeights, const SArray<double> & initUKnots, const SArray<double> & initVKnots );
    */

    ptrdiff_t uCount = kArray;
    ptrdiff_t vCount = lArray;
    //MbCartPoint3D p;

    Array2<MbCartPoint3D> initPoints;
    Array2<double> initWeights;

    //if (initPoints.SetSize(std_max(vCount, (ptrdiff_t)1), std_max(uCount, (ptrdiff_t)1))) {
    if ( initPoints.SetSize( vCount, uCount ) ) {
      for ( ptrdiff_t i = 0; i < vCount; i++ ) {
        for ( ptrdiff_t j = 0; j < uCount; j++ ) {
          //p.x = pnts[i*lArray + j].x;
          //.y = pnts[i*lArray + j].y;
          //p.z = pnts[i*lArray + j].z;
          //initPoints.Init(i, j, pList[i*uCount + j]);
          initPoints.Init( i, j, pnts[i*kArray + j] );
          //midPnt += initPoints(i, j);
        }
      }
      //midPnt /= (double)(vCount * uCount);
    }
    else {
      return 19; // Недостаточно памяти.
    }
    SArray<double> initUKnots;
    SArray<double> initVKnots;

    size_t uDeg = 2;
    size_t vDeg = 2;

    //ptrdiff_t uDots = uCount - uDeg;
    //ptrdiff_t vDots = vCount - vDeg;

    //Открытый формат по U
    for ( size_t j = 0; j < uCount + uDeg; ++j ) {
      initUKnots.Add( (double)( j ) );
    }

    //Открытый формат по V
    for ( size_t i = 0; i < vCount + vDeg; ++i ) {
      initVKnots.Add( (double)i );
    }

    bool iUcls = false;
    bool iVcls = false;

    MbSplineSurface * nurbsSurface = nullptr;


    //MbResultType res = ::NurbsSurfacesShell(nsParams, snMaker, isPhantom, solid, progBar);
    //virtual MbSplineSurface * NurbsSurface(double, double, double, double, bool bmatch = false) const; // \ru Построить NURBS копию поверхности. \en Construct a NURBS copy of the surface. 
    //nurbsSurface = new MbSplineSurface(4, 3, false, false, spoints, sweghts, arParamsU, arParamsV);  //-V112
    nurbsSurface = new MbSplineSurface( uDeg, vDeg, iVcls, iUcls, initPoints, initUKnots, initVKnots );

    //TestVariables::viewManager->AddObject(TestVariables::SURFACE_Style, nurbsSurface);
    TestVariables::viewManager->AddObject( TestVariables::WIRE_Style, nurbsSurface );

    MbControlData3D  Cvrts;
    nurbsSurface->GetBasisPoints( Cvrts );        // Вершины
    n = Cvrts.Count();                          // Общее количество вершин
    //int uDegree = nurbsSurface->GetUDegree();   // Степень по U
    //int vDegree = nurbsSurface->GetVDegree();

    uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = nurbsSurface->GetPointsVCount();

  }

  return 0;
}


//------------------------------------------------------------------------------------
// Анализ Nurbs поверхности.
//---
int Test_AnalyzeNurbsSurface()
{
  MbSplineSurface * nurbsSurface = nullptr;
  int outError = 0;
  MbItem * obj( nullptr );
  //MbCurve3D * pllne( nullptr );
  if ( GetGObj( IDS_SELECT_SURFACE, st_SplineSurface, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    nurbsSurface = (MbSplineSurface *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( nurbsSurface->IsA() == st_SplineSurface ) {
      //bool cl = pllne->IsClosed();
      TestVariables::viewManager->RefreshObjectModel( obj ); // Погасить поверхность. // Hide a curve.
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не Nurbs поверхность" ), IDS_TITLE_ERR );
      outError = 25;
      return outError;
    }
    MbControlData3D  Cvrts;
    nurbsSurface->GetBasisPoints( Cvrts );        // Вершины
    //int n = Cvrts.Count();                      // Общее количество вершин
    //int uDegree = nurbsSurface->GetUDegree();   // Степень по U
    //int vDegree = nurbsSurface->GetVDegree();

    //int uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    //int vCount = nurbsSurface->GetPointsVCount();

    Array2 <double> weights;
    nurbsSurface->GetWeights( weights );              // Веса вершин. 
    //int nWeights = weights.Count();

    SArray <double> uKnots;
    nurbsSurface->GetKnots( true, uKnots );          // Узловой вектор по U.  
    //int nUKnots = uKnots.Count();

    SArray <double> vKnots;
    nurbsSurface->GetKnots( false, vKnots );          // Узловой вектор по V.  
    //int nVKnots = vKnots.Count();

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(sf_prt, "\nnUKnots = %d", nUKnots);
    //for (int ir = 0; ir < nUKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, uKnots[ir]);
    //}
    //fprintf(sf_prt, "\nnVKnots = %d", nVKnots);
    //for (int ir = 0; ir < nVKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, vKnots[ir]);
    //}
#endif

    return 0;
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не не выбран" ), IDS_TITLE_ERR );
    outError = 25;
    return outError;
  }
}


//------------------------------------------------------------------------------------
// Поверхность.
//---
int Test_CreateFairSurface( MbFairCurveData & curveData )
{
  //MbFairBSurfData surfData;
  int select = 1;
  /*DEBUG*/// ReadPllnesFromGeomBuffer();
  // Установить параметры по умолчанию
  //DEBUG 2020 11 25
  // Параметры по умолчанию
  bool closed = false;
  curveData.degreeBSpline = 8;
  MbeFairSplineFormat initFormat = fairFormat_Close;
  curveData.subdivision = fairSubdiv_Single;
  curveData.fairing = fairSmooth_Yes;
  curveData.arrange = false;
  curveData.create = fairMethod_Polyline;
  curveData.approx = fairApprox_IsoSpline;
  curveData.accountInflexVector = (MbeFairVector)0;
  //END DEBUG 2020 11 25

  while ( select != 9 ) {
    if ( !GetInt( _T( "1- on Pllnes, 2- on B-Curves, 3 - Params, 4 - Transpon, 5 - Create Surf, 6 - Curve from Surf, 7 - Curve into Surf, 8 - Samples, 9 - Exit" ), select ) )
      select = 9;
    switch ( select ) {
      case 1: // 1- Pllnes > NURBS
        Test_PllnesToNurbsSurface( curveData /*, surfData*/ );
        break;

      case 2: // 2- B-Curves > NURBS
        Test_BCurvesToNurbsSurface( curveData /*, surfData */ );
        break;

      case 3: // 3 - set Params
        SetParamsToCreateNurbs( curveData, closed, initFormat );
        break;

      case 4: // 4 - Transpon
        Test_TransponBSurface( curveData /*, surfData*/ );
        break;

      case 5: // 5 - Create Surf
        Test_CreateBSurface( curveData /*, surfData*/ );
        break;

      case 6: // 5 - Create Surf
        Test_CurveFromSurface( curveData /*, surfData*/ );
        break;

      case 7: // 5 - Create Surf
        Test_CurveIntoSurface( curveData /*, surfData*/ );
        break;

      case 8: // 8 - Samples
        Test_Samples( /*curveData*/ /*, surfData*/ );
        break;

      case 9: //8 - Exit
        select = 9;
        break;
    }

    if ( curveData.error > 0 ) { // Регистрация ошибки.
      string_t message( _T( "Error" ) );
      message = MessageError( curveData.error );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );   // Выводит сообщение.
    }
  }
  return 0;
}


//---------------------------------------------------------------------------------
// Прямое определение Nurbs поверности на каркасе ломаных.
// ---
int Test_PllnesToNurbsSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ )
{
  int outError = 0;

  Array2<MbCartPoint3D> initPoints;

  MbCurve3D * pllne = nullptr;
  bool closed = false, cls = false;
  bool uClosed = false;
  bool vClosed = false;
  size_t kArray = 0, lArray = 0;
  SArray<MbCartPoint3D> pnts;
  size_t n = 0;
  //size_t number = 0;

  size_t uCount, vCount;
  for ( size_t ir = 0; ; ir++ ) {
    if ( ( outError = ReadPllne( pllne, cls, n ) ) > 0 ) {
      // Выбор ломаных закончен.
      if ( outError == 47 )
        break; // Отказ от ввода
      continue;
    }
    if ( ir == 0 ) {
      closed = cls;
      kArray = n;

    }
    if ( closed != cls || kArray != n ) {
      // Ломаные несогласованы. Выберите правильную ломаную.
      ::MessageBoxEx( _T( "*** Ломаные несогласованы. Выберите правильную ломаную." ), IDS_TITLE_ERR );
      continue;
    }
    // Перенести точки в список
    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );

    for ( size_t irr = 0; irr < kArray; irr++ ) {

      pnts.Add( cvrts[irr] );

    }
    if ( cls ) pnts.Add( cvrts[0] );
    lArray++;
  }

  {
    // Начальные данные.
    uCount = kArray;
    vCount = lArray;

    // Параметры по U.
    MessageBoxEx( _T( "Enter params by U: topology, degree and format" ), IDS_TITLE_MSG );
    MbeFairSplineFormat initFormat = fairFormat_Open;
    SetParamsToCreateNurbs( curveData, closed, initFormat );
    size_t initFormatU = initFormat;
    uClosed = closed;
    if ( cls )
      uClosed = true;
    if ( initFormatU != 1 )
      uClosed = false;
    size_t uDeg = curveData.degreeBSpline;
    if ( uDeg > kArray - 1 && !uClosed )uDeg = kArray - 1;
    size_t uOrder = uDeg + 1;

    if ( uDeg == 1 ) {
      initFormatU = 1;
      uClosed = false;
    }

    // Параметры по V.
    MessageBoxEx( _T( "Enter params by V: topology, degree and format" ), IDS_TITLE_MSG );
    SetParamsToCreateNurbs( curveData, closed, initFormat );
    size_t initFormatV = initFormat;
    vClosed = closed;
    if ( initFormatV != 1 )
      vClosed = false;

    size_t vDeg = curveData.degreeBSpline;
    if ( vDeg > lArray - 1 && !vClosed )vDeg = lArray - 1;
    size_t vOrder = vDeg + 1;
    //if (vDeg == 1)initFormatV = 1;

    if ( vDeg == 1 ) {
      initFormatV = 1;
      vClosed = false;
    }

    if ( !uClosed && !vClosed ) { // Незамкнутые ломаные.
      if ( initPoints.SetSize( lArray, kArray ) ) {
        for ( size_t i = 0; i < lArray; i++ ) {
          for ( size_t j = 0; j < kArray; j++ ) {
            initPoints.Init( i, j, pnts[i * kArray + j] );
          }
        }
      }
      else {
        return 19; // Недостаточно памяти.
      }
    }

    if ( uClosed && uDeg > 1 && !vClosed ) {  // Только по U
      // Преобразование замкнутого S-полигона по U в открытый незамкнутый.

      size_t n2;
      size_t nSegmsU = kArray;
      uCount = nSegmsU + uDeg;
      if ( initPoints.SetSize( vCount, uCount ) ) {

        for ( size_t i = 0; i < vCount; i++ ) {
          SArray <MbCartPoint3D> v;
          v.resize( kArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( uCount );
          for ( size_t j = 0; j < kArray; j++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[j] = pnts[i * kArray + j];
          }
          ClosedToOpen( v, kArray, uDeg, v2, n2 );
          for ( size_t j = 0; j < n2; j++ ) {
            //initPoints.Init(i, j, v2[j]); 
            initPoints( i, j ) = v2[j];
          }
        }
      }
    } // Только по U

    if ( vClosed && vDeg > 1 && !uClosed ) { // Только по V
      // Преобразование замкнутого S-полигона по v в открытый незамкнутый.

      size_t n2;
      size_t nSegmsV = lArray;
      vCount = nSegmsV + vDeg;
      if ( initPoints.SetSize( vCount, uCount ) ) {

        for ( size_t j = 0; j < kArray; j++ ) {

          SArray <MbCartPoint3D> v;
          v.resize( lArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( vCount );
          for ( size_t i = 0; i < lArray; i++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[i] = pnts[i * kArray + j];
          }
          ClosedToOpen( v, lArray, vDeg, v2, n2 );
          for ( size_t i = 0; i < vCount; i++ ) {
            initPoints.Init( i, j, v2[i] );
          }
        }
      }
    } // Только по V


    if ( uClosed && uDeg > 1 && vClosed && vDeg > 1 ) {  //  по U и по V
      // Преобразование замкнутого S-полигона по U в открытый незамкнутый.

      size_t n2;
      size_t nSegmsU = kArray;
      uCount = nSegmsU + uDeg;
      Array2<MbCartPoint3D> prePoints;
      if ( prePoints.SetSize( vCount, uCount ) ) {

        for ( size_t i = 0; i < vCount; i++ ) {
          SArray <MbCartPoint3D> v;
          v.resize( kArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( uCount );
          for ( size_t j = 0; j < kArray; j++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[j] = pnts[i * kArray + j];
          }
          ClosedToOpen( v, kArray, uDeg, v2, n2 );
          for ( size_t j = 0; j < n2; j++ ) {
            prePoints.Init( i, j, v2[j] );
          }
        }
      }

      size_t nSegmsV = lArray;
      vCount = nSegmsV + vDeg;
      if ( initPoints.SetSize( vCount, uCount ) ) {

        for ( size_t j = 0; j < uCount; j++ ) {

          SArray <MbCartPoint3D> v;
          v.resize( lArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( vCount );
          for ( size_t i = 0; i < lArray; i++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[i] = prePoints( i, j );
          }
          ClosedToOpen( v, lArray, vDeg, v2, n2 );
          for ( size_t i = 0; i < vCount; i++ ) {
            initPoints.Init( i, j, v2[i] );
          }
        }
      }
    }   //  по U и по V

    SArray<double> initUKnots;
    SArray<double> initVKnots;

    ptrdiff_t uDots = uCount - uDeg + 1;
    ptrdiff_t vDots = vCount - vDeg + 1;

    //Открытый формат по U

    if ( initFormatU == 1 ) {
      for ( size_t j = 0; j < uCount + uDeg + 1; ++j ) {
        initUKnots.Add( (double)( j ) );
      }
    }
    else {
      for ( size_t ir = 0; ir < uDeg; ++ir ) {
        initUKnots.Add( (double)( 0 ) );
      }

      for ( ptrdiff_t ir = 0; ir < uDots; ++ir ) {
        initUKnots.Add( (double)( ir ) );
      }
      for ( size_t ir = 0; ir < uDeg; ++ir ) {
        initUKnots.Add( (double)( uDots - 1 ) );
      }
    }

    //Открытый формат по V
    if ( initFormatV == 1 ) {
      for ( size_t i = 0; i < vCount + vDeg + 1; ++i ) {
        initVKnots.Add( (double)i );
      }
    }
    else {
      for ( size_t ir = 0; ir < vDeg; ++ir ) {
        initVKnots.Add( (double)( 0 ) );
      }

      for ( ptrdiff_t ir = 0; ir < vDots; ++ir ) {
        initVKnots.Add( (double)( ir ) );
      }
      for ( size_t ir = 0; ir < vDeg; ++ir ) {
        initVKnots.Add( (double)( vDots - 1 ) );
      }
    }

    bool iUcls = false;
    bool iVcls = false;

    MbSplineSurface * nurbsSurface = nullptr;
    // Печать матрицы s-фрейма

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(sf_prt, "\n initPoints vCount = %d uCount = %d", vCount, uCount);
    //for (ptrdiff_t i = 0; i < vCount; i++) {
    //  for (ptrdiff_t j = 0; j < kArray; j++) {
    //    fprintf(sf_prt, "\n i = %d j = %d xyz = %f, %f, %f",
    //      i, j, initPoints(i, j).x, initPoints(i, j).y, initPoints(i, j).z);
    //  }
    //}
#endif
    nurbsSurface = new MbSplineSurface( uOrder, vOrder, iVcls, iUcls, initPoints, initUKnots, initVKnots );

    TestVariables::viewManager->AddObject( TestVariables::WIRE_Style, nurbsSurface );

    MbControlData3D  Cvrts;
    nurbsSurface->GetBasisPoints( Cvrts );  // Вершины
    n = Cvrts.Count();                    // Общее количество вершин

    uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = nurbsSurface->GetPointsVCount();
  }

  if ( curveData.error == 0 ) {
    MessageBoxEx( _T( "Поверхность успешно определена!" ), IDS_TITLE_ALL_RIGHT );
    return 0;
  }
  else {
    MessageBoxEx( _T( "Поверхность не опрелена." ), IDS_TITLE_ERR );
    return curveData.error; // Некорректная структура данных.
  }
}


//---------------------------------------------------------------------------------
// Напрямую создать Nurbs поверность на каркасе B-кривых.
// ---
int Test_BCurvesToNurbsSurface( MbFairCurveData & curveData /*MbFairBSurfData & surfData*/ )
{
  int outError = 0;

  MbNurbs3D * curve = nullptr;
  bool closed = false, cls = false;
  size_t kArray = 0, lArray = 0;
  int format = 1, frm = 1;
  size_t degree = 1, dgr = 1;

  size_t n = 0;
  SArray<MbCartPoint3D> pnts;
  // number = 0;
  for ( size_t ir = 0; ; ir++ ) {
    outError = 0;

    // MbItem * obj = nullptr;
    // MbCurve3D * resCurve = nullptr;

    //bool closedBspline;
    //int degreeBspline;
    outError = ReadNurbsCurve( curve, dgr, cls, frm, n );
    if ( outError == 47 )
      break; // Отказ от ввода.
    if ( outError > 0 ) {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      continue; // Некорректная структура данных.
    }

    if ( ir == 0 ) {
      closed = cls;
      format = frm;
      degree = dgr;
      kArray = n;
    }

    if ( closed != cls || kArray != n || format != frm || degree != dgr ) {
      // Кривые  несогласованы. Выберите правильную ломаную.
      ::MessageBoxEx( _T( "*** Ломаные несогласованы. Выберите правильную ломаную." ), IDS_TITLE_ERR );
      continue;
    }
    // Перенести точки в список
    MbControlData3D cvrts;
    curve->GetBasisPoints( cvrts );

    for ( size_t irr = 0; irr < kArray; irr++ ) {
      pnts.Add( cvrts[irr] );
    }
    lArray++;
  }
  outError = 0;
  {
    size_t uCount = kArray;
    size_t vCount = lArray;
    //MbCartPoint3D p;

    Array2<MbCartPoint3D> initPoints;
    Array2<double> initWeights;

    // Параметры по V.
    MessageBoxEx( _T( "Enter params by V: topology, degree and format" ), IDS_TITLE_MSG );
    MbeFairSplineFormat initFormat = fairFormat_Open;
    SetParamsToCreateNurbs( curveData,  closed, initFormat );
    size_t initFormatV = initFormat;
    int vClosed = closed;
    if ( initFormatV != 1 )
      vClosed = false;

    size_t vDeg = curveData.degreeBSpline;
    if ( vDeg > lArray - 1 && !vClosed )vDeg = lArray - 1;
    size_t vOrder = vDeg + 1;
    //if (vDeg == 1)initFormatV = 1;

    if ( vDeg == 1 ) {
      initFormatV = 1;
      vClosed = false;
    }

    if ( !vClosed ) { // Незамкнутые V-ломаные.
      if ( initPoints.SetSize( vCount, uCount ) ) {
        for ( size_t i = 0; i < vCount; i++ ) {
          for ( size_t j = 0; j < uCount; j++ ) {
            initPoints.Init( i, j, pnts[i*kArray + j] );
          }
        }
      }
      else {
        return 19; // Недостаточно памяти.
      }
    } // Незамкнутые V-ломаные.

    if ( vClosed && vDeg > 1 ) { // Только по V
      // Преобразование замкнутого S-полигона по v в открытый незамкнутый.

      size_t n2;
      size_t nSegmsV = lArray;
      vCount = nSegmsV + vDeg;
      if ( initPoints.SetSize( vCount, uCount ) ) {
        for ( size_t j = 0; j < kArray; j++ ) {
          SArray <MbCartPoint3D> v;
          v.resize( lArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( vCount );
          for ( size_t i = 0; i < lArray; i++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[i] = pnts[i*kArray + j];
          }
          ClosedToOpen( v, lArray, vDeg, v2, n2 );
          for ( size_t i = 0; i < vCount; i++ ) {
            initPoints.Init( i, j, v2[i] );
          }
        }
      }
    } // Только по V

    SArray<double> initUKnots;
    SArray<double> initVKnots;
    SArray<double> wts;

    curve->GetKnots( initUKnots );

    size_t uDeg = degree;
    size_t uOrder = uDeg + 1;

    //ptrdiff_t uDots = uCount - uDeg + 1;
    ptrdiff_t vDots = vCount - vDeg + 1;

    //Открытый формат по V
    if ( initFormatV == 1 ) {
      for ( size_t i = 0; i < vCount + vDeg + 1; ++i ) {
        initVKnots.Add( (double)i );
      }
    }
    else {
      for ( size_t ir = 0; ir < vDeg; ++ir ) {
        initVKnots.Add( (double)( 0 ) );
      }

      for ( ptrdiff_t ir = 0; ir < vDots; ++ir ) {
        initVKnots.Add( (double)( ir ) );
      }
      for ( size_t ir = 0; ir < vDeg; ++ir ) {
        initVKnots.Add( (double)( vDots - 1 ) );
      }
    }

    bool iUcls = false;
    bool iVcls = false;

    MbSplineSurface * nurbsSurface = nullptr;

    //MbResultType res = ::NurbsSurfacesShell(nsParams, snMaker, isPhantom, solid, progBar);
    //virtual MbSplineSurface * NurbsSurface(double, double, double, double, bool bmatch = false) const; // \ru Построить NURBS копию поверхности. \en Construct a NURBS copy of the surface. 
    //nurbsSurface = new MbSplineSurface(4, 3, false, false, spoints, sweghts, arParamsU, arParamsV);  //-V112
    nurbsSurface = new MbSplineSurface( uOrder, vOrder, iVcls, iUcls, initPoints, initUKnots, initVKnots );

    //TestVariables::viewManager->AddObject(TestVariables::SURFACE_Style, nurbsSurface);
    TestVariables::viewManager->AddObject( TestVariables::WIRE_Style, nurbsSurface );

    MbControlData3D  Cvrts;
    nurbsSurface->GetBasisPoints( Cvrts );        // Вершины
    //size_t nVertex = Cvrts.Count();           // Общее количество вершин
    uOrder = nurbsSurface->GetUDegree();        // Порядок по U
    vOrder = nurbsSurface->GetVDegree();

    uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = nurbsSurface->GetPointsVCount();
  }

  if ( outError == 0 ) {
    MessageBoxEx( _T( "Поверхность успешно построена!" ), IDS_TITLE_ALL_RIGHT );
    return 0;
  }
  else {
    MessageBoxEx( _T( "Поверхность не построена." ), IDS_TITLE_ERR );
    return outError; // Некорректная структура данных.
  }
}


//---------------------------------------------------------------------------------
// Построить Nurbs поверность степеней (1, m) на каркасе ломаных, объединенных в Nurbs степеней (1, 1).
// Построить Nurbs поверность степеней (n, m) на каркасе B-кривых Nurbs поверхности степеней (1, m).
// ---
int Test_CreateBSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData*/ )
{
  MbSplineSurface * nurbsSurface = nullptr;
  MbControlData3D  cvrts;
 
  Array2<MbCartPoint3D> surfVrts;
  MbCurve3D * resCurve = nullptr;
  SArray <double> uKnots;
  SArray <double> vKnots;

  int outError = 0;
  MbItem * obj( nullptr );
  //MbCurve3D * pllne( nullptr );
  size_t uOrder, uDegree, vOrder, vDegree, uCount, vCount, nrr = 0;

  if ( GetGObj(IDS_SELECT_SURFACE, st_SplineSurface, &obj) && (obj->IsA() == st_SpaceInstance) ) {
    nurbsSurface = (MbSplineSurface *)((MbSpaceInstance *)obj)->GetSpaceItem();
    if ( nurbsSurface->IsA() == st_SplineSurface ) {
      //bool cl = pllne->IsClosed();
      //viewManager->RefreshObjectModel(obj); // Погасить поверхность. // Hide a curve.
    }
    else {
      ::MessageBoxEx( _T("*** Объект не Nurbs поверхность"), IDS_TITLE_ERR );
      outError = 25;
      return outError;
    }
   
    nurbsSurface->GetBasisPoints( cvrts );        // Вершины
    //int n = cvrts.Count();                      // Общее количество вершин
    uOrder = nurbsSurface->GetUDegree();    // Порядок по U
    uDegree = uOrder - 1;
    vOrder = nurbsSurface->GetVDegree();
    vDegree = vOrder - 1;

    uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = nurbsSurface->GetPointsVCount();

   // Array2 <double> weights;
   // nurbsSurface->GetWeights(weights);              // Веса вершин. 
   // int nWeights = weights.Count();
   
    if ( uDegree != 1 ) {
      MessageBoxEx( _T("Не единичная степень по строкам"), IDS_TITLE_WRN ); // Выводит сообщение.
    }
    nurbsSurface->GetKnots( true, uKnots );         // Узловой вектор по U.  
    //int nUKnots = uKnots.Count();
    
    
    nurbsSurface->GetKnots(false, vKnots);          // Узловой вектор по V.  
#ifdef C3D_DEBUG_FAIR_CURVES
    //int nVKnots = vKnots.Count();
    //fprintf(sf_prt, "\nnUKnots = %d", nUKnots);
    //for (int ir = 0; ir < nUKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, uKnots[ir]);
    //}
    //fprintf(sf_prt, "\nnVKnots = %d", nVKnots);
    //for (int ir = 0; ir < nVKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, vKnots[ir]);
    //}
#endif
  }
  else {
    ::MessageBoxEx(_T("*** Объект не выбран"), IDS_TITLE_ERR);
    outError = 25;
    return outError;
  }

  // Построение
  MbCurve3D * pllne( nullptr );
  if ( uDegree == 1 ) {  // Построение по U
    SArray<MbCartPoint3D> vrtx;
    size_t degreeBSpline = curveData.degreeBSpline;
    size_t nSegms = uCount - 1;
    for ( int ir = 0; ir < curveData.subdivision; ir++ ) {
      nSegms = nSegms * 2;
    }
    
    size_t vrtsBSpline;
    // Узнать размер выходного фрейма на первой строке сети.
    for ( size_t jr = 0; jr < uCount; jr++ ) {
      vrtx.Add( cvrts[jr] );
    }
    ::SplineCurve( vrtx, false, st_Polyline3D, pllne );
    bool closed = false; // Где устанавливается closed?
    if ( closed ) {
      // Установить признак замкнутости.  // Set a sign of closure.
      ( (MbPolyCurve3D *)pllne )->SetClosed( true ); // Замкнуть / разомкнуть кривую. // Close / unclose a curve.
    }
    vrtx.clear();
    if ( curveData.create == 1 ) {
      ::CreateFairBSplineCurveOnBasePolyline( pllne, curveData, resCurve );
    }
    else {
      ::CreateFairBSplineCurveOnTangentPolyline( pllne, curveData, resCurve );
    }

    if ( curveData.error > 0 ) { // Регистрация ошибки.
      string_t message( _T("Error") );
      message = MessageError( curveData.error );            // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.

      return curveData.error;
    }
    MbControlData3D  splCvrts;
    resCurve->GetBasisPoints(splCvrts);        // Вершины
    vrtsBSpline = splCvrts.Count();

    // Память под фрейм поверхности.
    if (surfVrts.SetSize(vCount, vrtsBSpline)) { //М

    }
    else {
      return 19; // Недостаточно памяти.
    }
   
    for ( size_t i = 0; i < vCount; i++ ) {  // Обход строк
      for ( size_t jr = 0; jr < uCount; jr++ ) {
        vrtx.Add( cvrts[i * uCount + jr] );
      }
      ::SplineCurve( vrtx, false, st_Polyline3D, pllne );
      if ( closed ) { // Где устанавливается closed?
        // Установить признак замкнутости.  // Set a sign of closure.
        ( (MbPolyCurve3D *)pllne )->SetClosed( true ); // Замкнуть / разомкнуть кривую. // Close / unclose a curve.
      }
      vrtx.clear();
      //viewManager->AddObject(POLYLINE_Style, pllne);


      if ( curveData.create == 1 ) {
        ::CreateFairBSplineCurveOnBasePolyline( pllne, curveData, resCurve );
      }
      else {
        ::CreateFairBSplineCurveOnTangentPolyline( pllne, curveData, resCurve );
      }
      if ( curveData.error > 0 ) { // Регистрация ошибки.
        string_t message( _T("Error") );
        message = MessageError( curveData.error );            // Формирует текст сообщения по номеру ошибки.
        MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
        return curveData.error;
      }
      // Нарисовать кривую.
      if ( curveData.error == 0 ) {
        Style style = TestVariables::BSPLINE_Style;
        //TestVariables::viewManager->AddObject(style, resCurve);

      }
      else
        return curveData.error;

      // Заполнить S-frame.
      MbControlData3D  resCvrts;
      resCurve->GetBasisPoints( resCvrts );        // Вершины
      nrr = resCvrts.Count();
      if ( nrr != vrtsBSpline ) {
        return 23; // Заданные параметры не совпадают с фактичскими значениями.
      }

      for ( size_t j = 0; j < vrtsBSpline; j++ ) {
        surfVrts.Init( i, j, resCvrts[j] );
      }
    }  // Обход строк
    
   // Create NURBS surf
    uKnots.clear();
    size_t uDots = nrr - degreeBSpline + 1;
    nSegms = uDots - 1;
    int outFormatU = curveData.outFormat;
    if ( outFormatU == 1 ) {
      for ( size_t j = 0; j < nSegms + degreeBSpline + 2; ++j ) {
        uKnots.Add( (double)(j) );
      }
    }
    else {
      for ( size_t ir = 0; ir < degreeBSpline; ++ir ) {
        uKnots.Add( (double)(0) );
      }

      for ( size_t ir = 0; ir < nSegms + 1; ++ir ) {
        uKnots.Add( (double)(ir) );
      }
      for ( size_t ir = 0; ir < degreeBSpline; ++ir ) {
        uKnots.Add( (double)(nSegms) );
      }
    }
  
    nurbsSurface = new MbSplineSurface( degreeBSpline + 1, vOrder, false, false, surfVrts, uKnots, vKnots );
    //TestVariables::viewManager->AddObject(TestVariables::SURFACE_Style, nurbsSurface);
    TestVariables::viewManager->AddObject(TestVariables::WIRE_Style, nurbsSurface);
    if ( curveData.error == 0 ) {
      MessageBoxEx( _T("Поверхность успешно построена!"), IDS_TITLE_ALL_RIGHT );
      return 0;
    }
    else {
      MessageBoxEx( _T("Поверхность не построена."), IDS_TITLE_ERR );
      return curveData.error; // Некорректная структура данных.
    }
   
 } // Построение по U
  
 return 0;
}


//---------------------------------------------------------------------------------
// Установить параметры построения Nurbs поверхности.
// ---
int  SetParamsToCreateNurbs( MbFairCurveData & fData, bool & setClosed, MbeFairSplineFormat & initFormat )
{
  int select = 1;

  //DEBUG 2020 11 25
  // Параметры по умолчанию
  int closed = 0;              //closed = false;
  int degree = 8;              //fData.degreeBSpline = degree;
  int initF = 2;               //initF = (fairFormat_Close);
  int subdivision = 1;         //fData.subdivision = (MbeFairSubdivision)subdivision;
  int fairing = 1;             //fData.fairing = fairSmooth_Yes;
  bool arrange = false;        //fData.arrange = arrange;
  int create = 1;              //fData.create = create;
  int approx = 1;              //fData.approx = (MbeFairApprox)approx;
  int accountInflexVector = 0; //fData.accountInflexVector = (MbeFairVector)accountInflexVector;
  //END DEBUG 2020 11 25
  while ( select != 9 ) {
    GetInt( _T( "1 - Closed, 2- Degree, 3 - Init Format, 4 - Fairing, 5 - Subdivide, 6 - TypeCreate, 7 - TypeApprox, 8 - Inflections, 9 - Exit" ), select );
    switch ( select ) {
      case 1: // 1- Closed
        // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
        if ( GetInt( _T( "Замкнутость: 0 - незамкнута, 1 - замкнута" ), closed ) ) {
          if ( closed < 0 
               ) closed = 0;
          if ( closed > 1 )
            closed = 1;
        }
        setClosed = false;
        if ( closed == 1 )
          setClosed = true;
        break;
      case 2: // 1- Degree
        // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
        if ( GetInt( _T( "Степень B-сплайна: 1,2,...,10" ), degree ) ) {
          if ( degree < 0 )
            degree = 1;
          if ( degree > 10 )
            degree = 10;
        }
        fData.degreeBSpline = degree;
        break;

      case 3: //   Init Format
        // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
        if ( GetInt( _T( "Исходный формат: 1 (float) или 2 (clamped)" ), initF ) ) {
          if ( initF < 1 )
            initF = 1;
          if ( initF > 2 )
            initF = 2;
        }
        initFormat = (MbeFairSplineFormat)initF;
        break;

      case 4: //   Fairing
        if ( GetInt( _T( "0 - не сглаживать, 1- сглаживать, 3 - сглаживать и гармонизировать распределение точек" ), fairing ) ) {
          if ( fairing < 0 )
            fairing = 0;
          if ( fairing > 0 )
            fairing = 1;
          if ( fairing > 1 )
            arrange = true;
        }

        fData.fairing = (MbeFairSmoothing)fairing;
        fData.arrange = arrange;
        break;

      case 5: // Subdivide
        if ( GetInt( _T( "Уплотнение кривой: не уплотнять = 0, однократно = 1, двукратно = 2" ), subdivision ) ) {
          if ( subdivision < 0 )
            subdivision = 0;
          if ( subdivision > 2 )
            subdivision = 2;
        }
        //fData.subdivision = subdivision;
        fData.subdivision = (MbeFairSubdivision)subdivision;
        break;

      case 6: //  TypeCreate
        if ( GetInt( _T( "Построение: 1 - на опорной ломаной, 2 - на касательной ломаной" ), create ) ) {
          if ( create < 1 )
            create = 1;
          if ( create > 2 )
            create = 2;
        }
        fData.create = (MbeFairMethod)create;
        break;
      case 7: // TypeApprox
        fData.approx = (MbeFairApprox)1;

        if ( GetInt( _T( "Построение B-сплайна: изогеометрическое = 1, по узлам = 0" ), approx ) ) {
          if ( approx < 0 )
            approx = 0;
          if ( approx > 1 )
            approx = 1;
        }
        fData.approx = (MbeFairApprox)approx;

        if ( approx == 0 ) {
          //fData.subdivision = 0;
          fData.subdivision = (MbeFairSubdivision)0;
          fData.arrange = 0;
        }
        break;

      case 8: //  Inflections

        if ( GetInt( _T( "Векторы в точках перегиба: направления звеньев S-полигона = 0, касательные = 1" ), accountInflexVector ) ) {
          if ( accountInflexVector < 0 )
            accountInflexVector = 0;
          if ( accountInflexVector > 1 )
            accountInflexVector = 1;
        }
        fData.accountInflexVector = (MbeFairVector)accountInflexVector;
        break;

      case 9:
        select = 9;  //9 - Exit
        break;
    }
  } // While

  return 0;
}


//------------------------------------------------------------------------------
// Читать NURBS кривую. \en Read the NURBS curve.
// ---
int ReadNurbsCurve( MbNurbs3D *& curv,
                    size_t & degreeBspline,
                    bool & closedBspline,
                    int & typeControlPolygonBspline,
                    size_t & nBspline )
{
  MbItem * obj = nullptr;

  if ( GetGObj( IDS_SELECT_CURVE, st_Curve3D, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    curv = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( curv != nullptr && ( curv->IsA() == st_Nurbs3D || curv->IsA() == st_Bezier3D || curv->IsA() == st_Hermit3D ) ) {
      MbControlData3D cvrts;
      SArray<double> knots;
      SArray<double> wts;
      curv->GetBasisPoints( cvrts );
      curv->GetKnots( knots );
      curv->GetWeights( wts );

      //      double length = curv->GetParamLength();
      bool closed = curv->IsClosed();

      if ( closed 
           ) return 24; // Некорректная структура данных;
      size_t n = cvrts.Count();
      size_t degree = curv->GetDegree() - 1;

      // Проверка формата NURBzS
      int typeControlPolygon = 2;
      double a = knots[degree + 2];
      double b = knots[degree + 3];

      if ( a == b )
        typeControlPolygon = 3;

      size_t nDots;
      if ( typeControlPolygon == 3 )
        nDots = ( n - 1 ) / degree + 1;
      if ( typeControlPolygon == 2 )
        nDots = ( n - degree + 1 );

      nBspline = n;
      degreeBspline = degree;
      typeControlPolygonBspline = typeControlPolygon;
      closedBspline = closed;
      return 0;
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }
  else { // Отказ от ввода 
    return 47;
  }
}


//------------------------------------------------------------------------------
// Открыть файл для отладочной печати.
// ---
int init_surf( /*MbFairCurveData & fData */ )
{
  int out_error = 0;
  char Path[128] = "C:/FairCurveModeler_TEMP/Temp/";
  char PathFile[128];
  strcpy( PathFile, Path );
  sf_prt = fopen( strcat( PathFile, "$fair_surf_report.txt" ), "w" );
  if ( sf_prt == nullptr ) {
    out_error = 47;  // Неизвестная ошибка.
    return out_error;
  }
  //strcpy(PathFile, Path);
  //sf_out = fopen(strcat(PathFile, "pnt_name.pnt"), "w");
  //if (sf_out == nullptr) {
  //  out_error = 47;  // Неизвестная ошибка.
  //  return out_error;
  //}
  //strcpy(PathFile, Path);
  //sf_out_cmps = fopen(strcat(PathFile, "pnt_name_kmps.txt"), "w");
  //if (sf_out_cmps == nullptr) {
  //  out_error = 47;  // Неизвестная ошибка.
  //  return out_error;
  //}

  /*DEBUG*/// fData.prt = sf_prt;
  return out_error;
}


//------------------------------------------------------------------------------
// Закрыть файл для отладочной печати.
// ---
int close_surf()
{
  fclose( sf_prt );
  //fclose(sf_out);
  //fclose(sf_out_cmps);
  return 0;
}


//---------------------------------------------------------------------------------
// Транспонировать Nurbs поверность.
// ---
int Test_TransponBSurface( MbFairCurveData & curveData/*, MbFairBSurfData & surfData*/ )
{
  MbSplineSurface * nurbsSurface = nullptr;
  MbControlData3D  cvrts;

  Array2<MbCartPoint3D> surfVrts;
  //MbCurve3D * resCurve = nullptr;
  SArray <double> uKnots;
  SArray <double> vKnots;
  int outError = 0;
  curveData.error = rt_Success;
  MbItem * obj( nullptr );
  //MbCurve3D * pllne( nullptr );
  size_t uOrder;
  size_t uDegree;
  size_t vOrder;
  size_t vDegree;
  size_t uCount;
  size_t vCount;
  //size_t nrr;

  if ( GetGObj( IDS_SELECT_SURFACE, st_SplineSurface, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    nurbsSurface = (MbSplineSurface *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( nurbsSurface->IsA() == st_SplineSurface ) {
      //bool cl = pllne->IsClosed();
      //viewManager->RefreshObjectModel(obj); // Погасить поверхность. // Hide a curve.
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не Nurbs поверхность" ), IDS_TITLE_ERR );
      outError = 25;
      return outError;
    }

    nurbsSurface->GetBasisPoints( cvrts );        // Вершины
    //int n = cvrts.Count();                      // Общее количество вершин
    uOrder = nurbsSurface->GetUDegree();    // Порядок по U
    uDegree = uOrder - 1;
    vOrder = nurbsSurface->GetVDegree();
    vDegree = vOrder - 1;

    uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = nurbsSurface->GetPointsVCount();

    // Array2 <double> weights;
    // nurbsSurface->GetWeights(weights);              // Веса вершин. 
    // int nWeights = weights.Count();

    nurbsSurface->GetKnots( true, uKnots );          // Узловой вектор по U.  
    //int nUKnots = uKnots.Count();

    nurbsSurface->GetKnots( false, vKnots );          // Узловой вектор по V.  
    //int nVKnots = vKnots.Count();

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(sf_prt, "\nnUKnots = %d", nUKnots);
    //for (int ir = 0; ir < nUKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, uKnots[ir]);
    //}
    //fprintf(sf_prt, "\nnVKnots = %d", nVKnots);
    //for (int ir = 0; ir < nVKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, vKnots[ir]);
    //}
#endif
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не не выбран" ), IDS_TITLE_ERR );
    outError = 25;
    return outError;
  }

  // Память под фрейм поверхности.
  if ( surfVrts.SetSize( uCount, vCount ) ) { //М

  }
  else {
    return 19; // Недостаточно памяти.
  }
  // Транспонирование.   
  for ( size_t i = 0; i < vCount; i++ ) {  // Обход строк
    for ( size_t j = 0; j < uCount; j++ ) {
      surfVrts.Init( j, i, cvrts[i * uCount + j] );
    }
  }

  SArray<MbCartPoint3D> vrtx;
  MbCurve3D * pllne( nullptr );
  for ( size_t i = 0; i < uCount; i++ ) {  // Обход строк
    for ( size_t jr = 0; jr < vCount; jr++ ) {
      vrtx.Add( surfVrts( i, jr ) );
    }

    ::SplineCurve( vrtx, false, st_Polyline3D, pllne );
    //viewManager->AddObject(POLYLINE_Style, pllne);
    vrtx.clear();
  }

  nurbsSurface = new MbSplineSurface( vOrder, uOrder, false, false, surfVrts, vKnots, uKnots );
  //TestVariables::viewManager->AddObject(TestVariables::SURFACE_Style, nurbsSurface);
  TestVariables::viewManager->AddObject( TestVariables::WIRE_Style, nurbsSurface );
  if ( outError == 0 ) {
    MessageBoxEx( _T( "Поверхность успешно транспонирована!" ), IDS_TITLE_ALL_RIGHT );
    return 0;
  }
  else {
    MessageBoxEx( _T( "Поверхность не транспонирована." ), IDS_TITLE_ERR );
    return outError; // Некорректная структура данных.
  }
}


 //---------------------------------------------------------------------------------
 // Примеры построения B-поверхностей.
 // ---
int Test_Samples( /*MbFairCurveData & curveData*/ /*, MbFairBSurfData & surfData*/ )
{
  int select = 1;
  while ( select != 4 ) {
    GetInt( _T( "1- Clamped S-frame, 2- Float S-frame, 3 - Bottle of Klein, 4 - Exit" ), select );
    switch ( select ) {
      case 1: // 1- Clamped S-frame
        Test_CreateSplineSurface2();
        break;

      case 2: // 2- Float S-frame
        Test_CreateSplineSurface();
        break;

      case 3: // 3 -  Bottle of Klein
        Test_CreateSplineSurfaceKlein();
        break;

      case 4: //4 - Exit
        select = 4;
        break;
    }

  }

  return 0;
}


//---------------------------------------------------------------------------------
// Вытащить управляющую кривую из поверности.
// ---
int Test_CurveFromSurface( MbFairCurveData & curveData /*, MbFairBSurfData & surfData */ )
{
  MbSplineSurface * nurbsSurface = nullptr;
  MbControlData3D  cvrts;

  Array2<MbCartPoint3D> surfVrts;
  MbCurve3D * resCurve = nullptr;
  SArray <double> uKnots;
  SArray <double> vKnots;
  int outError = 0;
  curveData.error = rt_Success;
  MbItem * obj( nullptr );
  //MbCurve3D * pllne( nullptr );
  int uOrder;
  int uDegree;
  int vOrder;
  int vDegree;
  int uCount;
  int vCount;
  //size_t nrr;

  int initNumber = 0;
  int number = 1;

  if ( GetGObj( IDS_SELECT_SURFACE, st_SplineSurface, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    nurbsSurface = (MbSplineSurface *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( nurbsSurface->IsA() == st_SplineSurface ) {
      //bool cl = pllne->IsClosed();
      //viewManager->RefreshObjectModel(obj); // Погасить поверхность. // Hide a curve.
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не Nurbs поверхность" ), IDS_TITLE_ERR );
      outError = 25;
      return outError;
    }

    nurbsSurface->GetBasisPoints( cvrts );        // Вершины
    //int n = cvrts.Count();                      // Общее количество вершин
    uOrder = (int)nurbsSurface->GetUDegree();    // Порядок по U
    uDegree = uOrder - 1;
    vOrder = (int)nurbsSurface->GetVDegree();
    vDegree = vOrder - 1;

    uCount = (int)nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = (int)nurbsSurface->GetPointsVCount();

    initNumber = vCount - 1;
    GetInt( _T( "Укажите номер начальной кривой" ), initNumber );
    if ( initNumber > vCount - 1 )
      initNumber = vCount - 1;
    number = vCount - initNumber;
    GetInt( _T( "Укажите количество кривых" ), number );
    if ( number > vCount - initNumber )
      number = vCount - initNumber;

    // Array2 <double> weights;
    // nurbsSurface->GetWeights(weights);              // Веса вершин. 
    // int nWeights = weights.Count();

    nurbsSurface->GetKnots( true, uKnots );          // Узловой вектор по U.  
    //int nUKnots = uKnots.Count();

    nurbsSurface->GetKnots( false, vKnots );          // Узловой вектор по V.  
    //int nVKnots = vKnots.Count();

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(sf_prt, "\nnUKnots = %d", nUKnots);
    //for (int ir = 0; ir < nUKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, uKnots[ir]);
    //}
    //fprintf(sf_prt, "\nnVKnots = %d", nVKnots);
    //for (int ir = 0; ir < nVKnots; ir++) {
    //  fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, vKnots[ir]);
    //}
#endif
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не не выбран" ), IDS_TITLE_ERR );
    outError = 25;
    return outError;
  }

  // Память под фрейм поверхности.
  if ( surfVrts.SetSize( uCount, vCount ) ) { //М
  }
  else {
    return 19; // Недостаточно памяти.
  }

  SArray<MbCartPoint3D> vrtx;
  MbCurve3D * pllne( nullptr );
  SArray<double> weights;
  for ( int i = 0; i < vCount; i++ ) {  // Обход строк
    if ( i >= initNumber && i < initNumber + number ) {
      for ( ptrdiff_t j = 0; j < uCount; j++ ) {
        vrtx.Add( cvrts[i*uCount + j] );
        weights.Add( 1. );
      }

      // Нарисовать ломаную. 
      ::SplineCurve( vrtx, false, st_Polyline3D, pllne );
      TestVariables::viewManager->AddObject( TestVariables::POLYLINE_Style, pllne );
      // Нарисовать кривую
      ::NurbsCurve( vrtx, weights, uOrder, uKnots, false, resCurve );
      //:NurbsCurve(pnts, wts, degree, knots, closed, resCurve);
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
      vrtx.clear();
      weights.clear();
    }
  }

  if ( outError == 0 ) {
    MessageBoxEx( _T( "Управляющие кривые успешно выделены из поверхности!" ), IDS_TITLE_ALL_RIGHT );
    return 0;
  }
  else {
    MessageBoxEx( _T( "Управляющие кривые не выделены." ), IDS_TITLE_ERR );
    return outError; // Некорректная структура данных.
  }
}


//---------------------------------------------------------------------------------
// Вставить управляющую кривую в поверхность.
// ---
int Test_CurveIntoSurface( MbFairCurveData & curveData/*, MbFairBSurfData & surfData*/ )
{
  MbSplineSurface * nurbsSurface = nullptr;
  MbControlData3D  cvrts;

  Array2<MbCartPoint3D> surfVrts;
  //MbCurve3D * resCurve = nullptr;
  SArray <double> uKnots;
  SArray <double> vKnots;
  int outError = 0;
  curveData.error = rt_Success;
  MbItem * obj( nullptr );
  //MbCurve3D * pllne( nullptr );
  int uOrder;
  int uDegree;
  int vOrder;
  int vDegree;
  int uCount;
  int vCount;
  //size_t nrr;

  // Для внешней кривой
  //MbNurbs3D * сurve = nullptr;
  bool cls;
  int frm;
  size_t dgr;
  size_t n;

  // Параметры вставки
  int initNumber = 0;
  //int number = 1;

  if ( GetGObj( IDS_SELECT_SURFACE, st_SplineSurface, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    nurbsSurface = (MbSplineSurface *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( nurbsSurface->IsA() == st_SplineSurface ) {
      //viewManager->RefreshObjectModel(obj); // Погасить поверхность. // Hide a curve.
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не Nurbs поверхность" ), IDS_TITLE_ERR );
      outError = 25;
      return outError;
    }

    nurbsSurface->GetBasisPoints( cvrts );        // Вершины
    n = cvrts.Count();                      // Общее количество вершин
    uOrder = (int)nurbsSurface->GetUDegree();    // Порядок по U
    uDegree = uOrder - 1;
    vOrder = (int)nurbsSurface->GetVDegree();
    vDegree = vOrder - 1;

    uCount = (int)nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = (int)nurbsSurface->GetPointsVCount();

    initNumber = vCount - 1;
    GetInt( _T( "Укажите номер начальной кривой" ), initNumber );
    if ( initNumber > vCount - 1 )initNumber = vCount - 1;
    /*
    number = vCount - initNumber;
    GetInt(_T("Укажите количество кривых"), number);
    if (number > vCount - initNumber)number = vCount - initNumber;
    */
    // Array2 <double> weights;
    // nurbsSurface->GetWeights(weights);              // Веса вершин. 
    // int nWeights = weights.Count();

    nurbsSurface->GetKnots( true, uKnots );          // Узловой вектор по U.  
    //int nUKnots = uKnots.Count();

    nurbsSurface->GetKnots( false, vKnots );          // Узловой вектор по V.  
    //int nVKnots = vKnots.Count();

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(sf_prt, "\nnUKnots = %d", nUKnots);
    //for (int ir = 0; ir < nUKnots; ir++) {
    //fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, uKnots[ir]);
    //}
    //fprintf(sf_prt, "\nnVKnots = %d", nVKnots);
    //for (int ir = 0; ir < nVKnots; ir++) {
    //fprintf(sf_prt, "\n ir = %d vKnot = %f", ir, vKnots[ir]);
    //}
#endif
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не не выбран" ), IDS_TITLE_ERR );
    outError = 25;
    return outError;
  }

  // Память под фрейм поверхности.
  if ( surfVrts.SetSize( vCount, uCount ) ) { //М
  }
  else {
    return 19; // Недостаточно памяти.
  }
  // Вставка кривых.
  SArray<MbCartPoint3D> vrtx;
  MbNurbs3D * curve = nullptr;;

  for ( int i = 0; i < vCount; i++ ) {  // Обход строк
    if ( i >= initNumber && i < vCount && outError != 47 ) {  // Ввод кривой.
     // while (outError != 0) {

      outError = ReadNurbsCurve( curve, dgr, cls, frm, n );
      if ( outError == 47 ) { // // Отказ от ввода.
        for ( ptrdiff_t j = 0; j < uCount; j++ ) {
          surfVrts.Init( i, j, cvrts[i*uCount + j] );
          //vrtx.Add(cvrts[i*uCount + j]);
        }
        continue;
      }

      if ( outError > 0 ) {
        MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
        return 28; // Не выбран нужный объект.
      }

      if ( (int)n != uCount ) {
        MessageBoxEx( _T( "Размер кривой не соответсвует размеру поверхности" ), IDS_TITLE_ERR );
        return 28; // Не выбран нужный объект.
      }
      // Перенести точки в список
      MbControlData3D cvrtcs;
      curve->GetBasisPoints( cvrtcs );

      for ( size_t jrr = 0; jrr < n; jrr++ ) {
        surfVrts.Init( i, jrr, cvrtcs[jrr] );
      }
      continue;
    } // Ввод кривой.

    for ( ptrdiff_t j = 0; j < uCount; j++ ) {
      surfVrts.Init( i, j, cvrts[i*uCount + j] );
    }
  }

  if ( outError == 47 )
    outError = 0;
  nurbsSurface = new MbSplineSurface( uOrder, vOrder, false, false, surfVrts, uKnots, vKnots );
  TestVariables::viewManager->AddObject( TestVariables::SURFACE_Style, nurbsSurface ); //SURFACE_Style, WIRE_Style
  if ( outError == 0 ) {
    MessageBoxEx( _T( "Управляющие кривые успешно вставлены!" ), IDS_TITLE_ALL_RIGHT );
    return 0;
  }
  else {

    MessageBoxEx( _T( "Кривые не вставлены." ), IDS_TITLE_ERR );
    return outError; // Некорректная структура данных.
  }
}


 //----------------------------------------------------------------------------
 // Преобразовать замкнутый S-полигон в открытый незамкнутый.
 //
int ClosedToOpen(
  SArray<MbCartPoint3D> & v,  //double v[][3],     //исходный локальный полигон/
  size_t n,
  size_t degree,                 //степень кривой
  SArray<MbCartPoint3D> & v2, //double v2[][3],
  size_t & n2 )
{
  size_t i;
  size_t cut, mod;
  cut = degree / 2;
  mod = degree - ( degree / 2 ) * 2;
  n2 = n + degree;

  for ( i = 0; i < n; ++i ) {
    v2[cut + i] = v[i];
  }

  for ( i = 0; i < cut; ++i ) {
    v2[cut - 1 - i] = v2[n + cut - 1 - i];
    v2[n + cut + i] = v2[i + cut];
  }

  if ( mod > 0 ) {
    v2[n + 2 * cut] = v2[2 * cut];
  }
  n2 = n + degree;

  return 1;
}


 //--------------------------------------------------------------------------------
 // Читать 3D полилинии из Геометрического Буфера.
 //---
int ReadPllnesFromGeomBuffer()
{
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;
  Style style = TestVariables::BSPLINE_Style;
  bool closed = false; // Только незамкнутый формат выходе
  size_t nVertex, nVertexWeights, nKnots;

  FILE * in;
  size_t alt, n, j, mr;
  char * p[10]; // указатели на числа в строке
  double xyz[3];
  int out_error = 0;

  char Path_File[] = "C:/FairCurveModeler_TEMP/Temp/pnt_name.pnt";

  if ( ( in = fopen( Path_File, "r" ) ) == nullptr ) {
    out_error = 3;
    return out_error;
  }

  char stroke[128];

  for ( int i = 0; ; i++ ) { // Чтение каркаса
    if ( fgets( stroke, 120, in ) == nullptr ) {
      nVertex = 0; nVertexWeights = 0; nKnots = 0;
      fclose( in );
      return i;
    }

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(fData.prt, "\nВершины ");
#endif

    for ( j = 0; ; j++ ) {
      alt = AnalizeLine( stroke, p, 3, n );

      // запись чисел
      if ( n == 3 ) {
        for ( mr = 0; mr < n; ++mr ) {
          xyz[mr] = atof( p[mr] );
        }
        pnt.x = xyz[0];
        pnt.y = xyz[1];
        pnt.z = xyz[2];

        pnts.Add( pnt );
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(sf_prt, "\n j = %d x = %f y = %f z = %f", j, pnt.x, pnt.y, pnt.z);
#endif
      }
      else {
        out_error = 6;
        fclose( in );
        return out_error;
      }

      if ( alt == 2 )
        break;

      if ( fgets( stroke, 120, in ) == nullptr ) {
        // Неправильная структура файла. Отсутствуют '))'
        nVertex = 0;
        nVertex = 0;
        nKnots = 0;
        // Message_Error( ach_msg_error[6] );
        out_error = 17; // Структура данных
        fclose( in );
        return out_error;
      }
    }
    nVertex = j + 1;

    // Нарисовать ломаную S-полигона
    MbCurve3D * result( nullptr );
    ::SplineCurve( pnts, closed, st_Polyline3D, result );
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
    pnts.clear();
  } // Чтение каркаса

}


//---------------------------------------------------------------------------------
// Прямое определение Nurbs поверности на точках сети.
// ---
int PllnesToNurbsSurface11( MbFairCurveData & curveData,
                            SArray<MbCartPoint3D> pnts,
                            size_t kArray,
                            size_t lArray,
                            MbeFairSplineFormat initFormat )
{
  Array2<MbCartPoint3D> initPoints;

  bool cls = false;
  bool uClosed = false;
  bool vClosed = false;
  size_t n = 0;
  size_t uCount;
  size_t vCount;

  //size_t number = 0;
  //size_t kArray = 0, lArray = 0;
  //SArray<MbCartPoint3D> pnts;
  //for (size_t ir = 0; ; ir++) {
  //  if ((outError = ReadPllne(pllne, cls, n)) > 0) {
  //    // Выбор ломаных закончен.
  //    if (outError == 47) break; // Отказ от ввода
  //    continue;
  //  }
  //  if (ir == 0) {
  //    closed = cls;
  //    kArray = n;
  //  }
  //  if (closed != cls || kArray != n) {
  //    // Ломаные несогласованы. Выберите правильную ломаную.
  //    ::MessageBoxEx(_T("*** Ломаные несогласованы. Выберите правильную ломаную."), IDS_TITLE_ERR);
  //    continue;
  //  }
  //  // Перенести точки в список
  //  MbControlData3D cvrts;
  //  pllne->GetBasisPoints(cvrts);
  //  for (size_t irr = 0; irr < kArray; irr++) {
  //    pnts.Add(cvrts[irr]);
  //  }
  //  if (cls) pnts.Add(cvrts[0]);
  //  lArray++;
  //}
  {
    // Начальные данные.
    uCount = kArray;
    vCount = lArray;

    // Параметры по U.
    ///MessageBoxEx(_T("Enter params by U: topology, degree and format"), IDS_TITLE_MSG);
    ///SetParamsToCreateNurbs(curveData);
    size_t initFormatU = initFormat;
    uClosed = false; /// curveData.closed;
    if ( cls )uClosed = true;
    if ( initFormatU != 1 )uClosed = false;
    size_t uDeg = 1; /// curveData.degreeBSpline;
    if ( uDeg > kArray - 1 && !uClosed )uDeg = kArray - 1;
    size_t uOrder = uDeg + 1;

    if ( uDeg == 1 ) {
      initFormatU = 1;
      uClosed = false;
    }

    // Параметры по V.
    ////MessageBoxEx(_T("Enter params by V: topology, degree and format"), IDS_TITLE_MSG);
    ////SetParamsToCreateNurbs(curveData);
    size_t initFormatV = initFormat;
    vClosed = false; // curveData.closed;
    if ( initFormatV != 1 )
      vClosed = false;

    size_t vDeg = 1; ///curveData.degreeBSpline;
    if ( vDeg > lArray - 1 && !vClosed )vDeg = lArray - 1;
    size_t vOrder = vDeg + 1;
    //if (vDeg == 1)initFormatV = 1;

    if ( vDeg == 1 ) {
      initFormatV = 1;
      vClosed = false;
    }

    if ( !uClosed && !vClosed ) { // Незамкнутые ломаные.

      if ( initPoints.SetSize( lArray, kArray ) ) {
        for ( size_t i = 0; i < lArray; i++ ) {
          for ( size_t j = 0; j < kArray; j++ ) {
            initPoints.Init( i, j, pnts[i * kArray + j] );
          }
        }
      }
      else {
        return 19; // Недостаточно памяти.
      }
    }

    if ( uClosed && uDeg > 1 && !vClosed ) {  // Только по U
      // Преобразование замкнутого S-полигона по U в открытый незамкнутый.

      //uClosed = true;
      size_t n2;
      size_t nSegmsU = kArray;
      uCount = nSegmsU + uDeg;
      if ( initPoints.SetSize( vCount, uCount ) ) {

        for ( size_t i = 0; i < vCount; i++ ) {
          SArray <MbCartPoint3D> v;
          v.resize( kArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( uCount );
          for ( size_t j = 0; j < kArray; j++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[j] = pnts[i * kArray + j];
          }
          ClosedToOpen( v, kArray, uDeg, v2, n2 );
          for ( size_t j = 0; j < n2; j++ ) {
            //initPoints.Init(i, j, v2[j]); 
            initPoints( i, j ) = v2[j];
          }
        }
      }
    } // Только по U

    if ( vClosed && vDeg > 1 && !uClosed ) { // Только по V
      // Преобразование замкнутого S-полигона по v в открытый незамкнутый.

      size_t n2;
      size_t nSegmsV = lArray;
      vCount = nSegmsV + vDeg;
      if ( initPoints.SetSize( vCount, uCount ) ) {

        for ( size_t j = 0; j < kArray; j++ ) {

          SArray <MbCartPoint3D> v;
          v.resize( lArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( vCount );
          for ( size_t i = 0; i < lArray; i++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[i] = pnts[i * kArray + j];
          }
          ClosedToOpen( v, lArray, vDeg, v2, n2 );
          for ( size_t i = 0; i < vCount; i++ ) {
            initPoints.Init( i, j, v2[i] );
          }
        }
      }
    } // Только по V

    if ( uClosed && uDeg > 1 && vClosed && vDeg > 1 ) {  //  по U и по V
      // Преобразование замкнутого S-полигона по U в открытый незамкнутый.

      size_t n2;
      size_t nSegmsU = kArray;
      uCount = nSegmsU + uDeg;
      Array2<MbCartPoint3D> prePoints;
      if ( prePoints.SetSize( vCount, uCount ) ) {

        for ( size_t i = 0; i < vCount; i++ ) {
          SArray <MbCartPoint3D> v;
          v.resize( kArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( uCount );
          for ( size_t j = 0; j < kArray; j++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[j] = pnts[i * kArray + j];
          }
          ClosedToOpen( v, kArray, uDeg, v2, n2 );
          for ( size_t j = 0; j < n2; j++ ) {
            prePoints.Init( i, j, v2[j] );
          }
        }
      }

      size_t nSegmsV = lArray;
      vCount = nSegmsV + vDeg;
      if ( initPoints.SetSize( vCount, uCount ) ) {

        for ( size_t j = 0; j < uCount; j++ ) {

          SArray <MbCartPoint3D> v;
          v.resize( lArray );
          SArray <MbCartPoint3D> v2;
          v2.resize( vCount );
          for ( size_t i = 0; i < lArray; i++ ) {
            //v.Add(pnts[i*uCount + j]);
            v[i] = prePoints( i, j );
          }
          ClosedToOpen( v, lArray, vDeg, v2, n2 );
          for ( size_t i = 0; i < vCount; i++ ) {
            initPoints.Init( i, j, v2[i] );
          }
        }
      }
    }   //  по U и по V

    SArray<double> initUKnots;
    SArray<double> initVKnots;

    ptrdiff_t uDots = uCount - uDeg + 1;
    ptrdiff_t vDots = vCount - vDeg + 1;

    //Открытый формат по U

    if ( initFormatU == 1 ) {
      for ( size_t j = 0; j < uCount + uDeg + 1; ++j ) {
        initUKnots.Add( (double)( j ) );
      }
    }
    else {
      for ( size_t ir = 0; ir < uDeg; ++ir ) {
        initUKnots.Add( (double)( 0 ) );
      }

      for ( ptrdiff_t ir = 0; ir < uDots; ++ir ) {
        initUKnots.Add( (double)( ir ) );
      }
      for ( size_t ir = 0; ir < uDeg; ++ir ) {
        initUKnots.Add( (double)( uDots - 1 ) );
      }
    }

    //Открытый формат по V
    if ( initFormatV == 1 ) {
      for ( size_t i = 0; i < vCount + vDeg + 1; ++i ) {
        initVKnots.Add( (double)i );
      }
    }
    else {
      for ( size_t ir = 0; ir < vDeg; ++ir ) {
        initVKnots.Add( (double)( 0 ) );
      }

      for ( ptrdiff_t ir = 0; ir < vDots; ++ir ) {
        initVKnots.Add( (double)( ir ) );
      }
      for ( size_t ir = 0; ir < vDeg; ++ir ) {
        initVKnots.Add( (double)( vDots - 1 ) );
      }
    }

    bool iUcls = false;
    bool iVcls = false;

    MbSplineSurface * nurbsSurface = nullptr;
    // Печать матрицы s-фрейма

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(sf_prt, "\n initPoints vCount = %d uCount = %d", vCount, uCount);
    //for (ptrdiff_t i = 0; i < vCount; i++) {
    //  for (ptrdiff_t j = 0; j < kArray; j++) {
    //    fprintf(sf_prt, "\n i = %d j = %d xyz = %f, %f, %f",
    //      i, j, initPoints(i, j).x, initPoints(i, j).y, initPoints(i, j).z);
    //  }
    //}
#endif

    nurbsSurface = new MbSplineSurface( uOrder, vOrder, iVcls, iUcls, initPoints, initUKnots, initVKnots );

    TestVariables::viewManager->AddObject( TestVariables::WIRE_Style, nurbsSurface );

    MbControlData3D  Cvrts;
    nurbsSurface->GetBasisPoints( Cvrts );  // Вершины
    n = Cvrts.Count();                    // Общее количество вершин

    uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = nurbsSurface->GetPointsVCount();
  }

  if ( curveData.error == 0 ) {
    MessageBoxEx( _T( "Поверхность успешно определена!" ), IDS_TITLE_ALL_RIGHT );
    return 0;
  }
  else {
    MessageBoxEx( _T( "Поверхность не опрелена." ), IDS_TITLE_ERR );
    return curveData.error; // Некорректная структура данных.
  }
}

 