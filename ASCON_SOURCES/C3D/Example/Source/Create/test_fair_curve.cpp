#include <test.h>
#include <test_manager.h>
#include <test_fair_curve.h>
#include <test_service.h>
#include <action_curve3d.h>
#include <space_instance.h>
#include <fair_curves_utils.h>
#include <point_frame.h>
#include <cur_nurbs3d.h>
#include <cur_nurbs.h>
#include <alg_circle_curve.h>
#include <alg_draw.h>
#include <plane_instance.h>
#include <math_namespace.h>
#include <surf_spline_surface.h>
#include <action_surface_curve.h>
#include <test_fair_surface.h>
#include <array>


#include <last.h>

using namespace c3d;
using namespace TestVariables;

FILE *prt;
FILE *sf_out;
FILE *sf_out_cmps;

MbFairCurveData fData; // Старые интерфейсы работают с глобальной переменной.

int outErrors = 0;

typedef std::vector<std::pair<size_t, MbVector3D*> >  IndexVectorPtrArray;

//#define FAIR_CURVE_ADVANCED

///////////////////////////////////////////////////////////////////////////////
//                Предварительные объявления.
///////////////////////////////////////////////////////////////////////////////

// Функции для команд.
int Test_CreateFairBSplineCurveOnBasePolyline( bool createWire );
int Test_CreateFairBSplineCurveOnBasePolylineOfHermiteGD( bool createWire );
int Test_CreateFairBSplineCurveOnTangentPolyline( bool createWire );
int Test_CreateFairBSplineCurveOnTangentsOfHermiteGD( bool createWire );
int Test_CreateFairBezierCurveOnBasePolyline( bool createWire );
int Test_CreateFairBezierCurveOnTangentPolyline( bool createWire );
int Test_CreateBezierCurveOnHermiteGD( bool createWire );
int Test_CreateBSplineCurveOnHermiteGD( bool createWire );
int Test_CreateHermiteGDOnBezierCurve( bool createWire );
int Test_CreateFairBezierCurveOnBasePolylineOfHermiteGD( bool createWire );
int Test_CreateFairBezierCurveOnTangentsOfHermiteGD( bool createWire );
int Test_ElevateDegreeBezierCurve( bool createWire );
int Test_SubdivideNurbsCurve( bool createWire );
int Test_ReParamCurve( bool createWire );
int Test_ExtractChangeFormatNurbsCurve( bool createWire );
int Test_AddKnotNurbsCurve( bool createWire );
int Test_ConvertBezierCurveToConicBSpline( bool createWire );
int Test_CreateClothoid( bool createWire );
int Test_CreateSectrix( bool createWire );
int Test_CreateFairBezierFilletOnPolyline( bool createWire );
int Test_CreateFairBSplineFilletOnSPolygon();
int Test_ExtractExtrapolOneSegment();
int Test_Smoothing_Noisy_Data( bool createWire );
#ifdef FAIR_CURVE_ADVANCED
int Test_GeomPreProcessor();
#endif

 // TODO: перенести в c3d.dll, перенесено построение по умолчанию (построение дуги).
int CreateOneSegmentOnBasePolyline( c3d::SpaceCurveSPtr & pllne, bool bSpline, bool createWire );
int TangentsOnCirle( SArray<MbCartPoint3D> & polyLine, SArray<MbVector3D> & tangents, double & cvt );
int TangentOnSecondPoint( const SArray<MbCartPoint3D> & polyLine, SArray<MbVector3D> & tangents, double & cvt );
bool StructToGDH( SArray <MbCartPoint3D> & polyLine, SArray <MbVector3D> & tangents, SArray <double> & curvatures, size_t kArray,
                  SArray <int> & arrayInfectionSign, SArray <int> & arrayStraightSign, SArray <MbVector3D> & vectorCurvature );
int CreateHermiteGDOnStructure( SArray <MbCartPoint3D> & polyLine, SArray <MbVector3D> & tangents,
                                SArray <double> & curvatures, size_t kArray, MbCurve3D *& pllne );
#ifdef FAIR_CURVE_ADVANCED
int GeomPreProcessor( size_t inType, bool smooth, const MbCurve3D * pllne, size_t & n, size_t & nSites,
                      size_t & nStrSites, size_t & nMinPntSite, MbCurve3D *& rezPllne );
#endif

 // TODO: перенести в c3d.dll?
bool CurveToSPolygon( MbNurbs3D * curve, MbCartPoint3D point, int dir, MbFairCurveData & data, MbCurve3D *& sPolyCurve );

// Вспомогательные функции.
int TestAccuracyHermite( MbCurve3D * pllne, MbCurve3D * curve );
int AnalyzeNurbs( MbNurbs3D * curv, int & typeControlPolygon, int & nSegments, size_t & degree );
int TestAccuracy( const MbCurve3D * pllne, const MbCurve3D * curve, std::array<double, 3> * maxAccuracy = nullptr );
bool IsHermit( MbCurve3D * pllne );
int Expired();
#ifdef FAIR_CURVE_ADVANCED
void DescreteCurvature( const MbCartPoint3D & t1, const MbCartPoint3D & t2, const MbCartPoint3D & t3, double delta_prm, MbVector3D & v, double & s );
#endif

// Убрать.
int Utilites();
double EstimateAccuracy();
int ReadPllneFromGeomBufferCompas();
int Test_EditHermiteGD();
#ifdef FAIR_CURVE_ADVANCED
int ExchangeGB();
#endif

///////////////////////////////////////////////////////////////////////////////
//                      Ввод - вывод данных.
///////////////////////////////////////////////////////////////////////////////
static
int GetIntValue( int initVal, int minValue, int maxValue, const TCHAR* message )
{
  int value = initVal;
  if ( GetInt( message, value ) ) {
    if ( value < minValue )
      value = minValue;
    if ( value > maxValue )
      value = maxValue;
  }
  return value;
}


static
float GetFloatValue( float initVal, float minValue, float maxValue, const TCHAR* message )
{
  float value = initVal;
  if ( GetFloat( message, value ) ) {
    if ( value < minValue )
      value = minValue;
    if ( value > maxValue )
      value = maxValue;
  }
  return value;
}


//---------------------------------------------------------------------------------
// Чтение 3D полилинии
// ---
int ReadPllne( MbCurve3D *& pllne, bool & closed, size_t & n )
{
  int outError = 0;
  MbItem * obj( nullptr );
  //MbCurve3D * pllne( nullptr );
  if ( GetGObj( IDS_SELECT_CURVE, st_Curve3D, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    pllne = (MbCurve3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( pllne->IsA() == st_Polyline3D ) {
      //bool cl = pllne->IsClosed();
      TestVariables::viewManager->RefreshObjectModel( obj ); // Погасить кривую. // Hide a curve.
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не 3D Ломаная" ), IDS_TITLE_ERR );
      outError = 25;
      return outError;
    }

    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );
    n = cvrts.Count();

    closed = pllne->IsClosed();
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не выбран" ), IDS_TITLE_ERR );
    outError = 47; // Не выбран нужный объект
    return outError;
  }

  return outError;
}


//---------------------------------------------------------------------------------
// Чтение 3D полилинии сопряжения
// ---
int ReadPllneFillet( MbCurve3D *& pllne, bool & closed, size_t & n )
{
  int outError = 0;
  MbItem * obj( nullptr );
  //MbCurve3D * pllne( nullptr );
  if ( GetGObj( _T( "Specify fillet polyline" ), st_Curve3D, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    pllne = (MbCurve3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( pllne->IsA() == st_Polyline3D ) {
      //bool cl = pllne->IsClosed();
      viewManager->RefreshObjectModel( obj ); // Погасить кривую. // Hide a curve.
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не 3D Ломаная" ), IDS_TITLE_ERR );
      outError = 25;
      return outError;
    }

    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );
    n = cvrts.Count();

    closed = pllne->IsClosed();
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не выбран" ), IDS_TITLE_ERR );
    outError = 47; // Не выбран нужный объект
    return outError;
  }

  return outError;
}


//-----------------------------------------------------------------------------     
// Получить кривую из каркаса (создает копию).
//---
c3d::SpaceCurveSPtr CurveFromWireFrame( const MbWireFrame * wire )
{
  c3d::SpaceCurveSPtr curve;
  RPArray<MbCurve3D> curves;
  wire->MakeCurves( curves );
  if ( ( curves.size() == 1 ) && ( curves[0] != nullptr ) )
    curve = curves[0];
  ::DeleteItems( curves );
  return curve;
}


//-----------------------------------------------------------------------------     
// Чтение 3D полилинии
//---
c3d::SpaceCurveSPtr ReadPolyline( bool & closed, size_t & n, int & err )
{
  err = 0;
  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curve;

  if ( GetGObj( IDS_SELECT_CURVE, st_SpaceItem, &obj ) ) {
    if ( obj->IsA() == st_SpaceInstance ) {
      curve = (MbCurve3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    }
    else if ( obj->IsA() == st_WireFrame ) {
      curve = CurveFromWireFrame( static_cast<MbWireFrame *>( obj ) );
    }

    if ( curve != nullptr && curve->IsA() == st_Polyline3D ) {
      TestVariables::viewManager->RefreshObjectModel( obj );

      MbControlData3D cvrts;
      curve->GetBasisPoints( cvrts );
      n = cvrts.Count();

      closed = curve->IsClosed();
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не 3D Ломаная" ), IDS_TITLE_ERR );
      err = 25;
      curve = nullptr;
    }
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не выбран" ), IDS_TITLE_ERR );
    err = 47; // Не выбран нужный объект
  }

  return curve;
}


//-----------------------------------------------------------------------------     
// Чтение 3D Nurbs 
// Возможен выбор 3D-кривой или проволочного каркаса.
//---
c3d::SpaceCurveSPtr ReadNurbs( int & err, MbItem *& obj )
{
  err = 0;
  obj = nullptr;
  c3d::SpaceCurveSPtr curve;

  if ( GetGObj( IDS_SELECT_CURVE, st_SpaceItem, &obj ) ) {
    if ( obj->IsA() == st_SpaceInstance ) {
      curve = (MbCurve3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    }
    else if ( obj->IsA() == st_WireFrame ) {
      MbWireFrame * wf = static_cast<MbWireFrame *>( obj );
      RPArray<MbCurve3D> curves;
      wf->MakeCurves( curves );
      if ( ( curves.size() == 1 ) && ( curves[0] != nullptr ) )
        curve = curves[0];

      ::DeleteItems( curves );
    }

    if ( curve != nullptr && curve->IsA() == st_Nurbs3D ) {
      TestVariables::viewManager->RefreshObjectModel( obj );
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не Nurbs" ), IDS_TITLE_ERR );
      err = 25;
      curve = nullptr;
    }
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не выбран" ), IDS_TITLE_ERR );
    err = 47; // Не выбран нужный объект
  }

  return curve;
}


//-----------------------------------------------------------------------------     
// Вывод сообщения о точности аппроксимации.
//---
inline
bool IsSuccess( MbResultType err )
{
  return err == rt_Success || err == rt_AccuracyError;
}


//-----------------------------------------------------------------------------     
// Вывод сообщения о точности аппроксимации.
//---
void PrintApproxErr( const MbCurve3D * pllne, MbCurve3D * curve, const MbApproxError & err, MbResultType res )
{
  std::array<double, 3> acc = { MB_MAXDOUBLE, MB_MAXDOUBLE, MB_MAXDOUBLE };
  TestAccuracy( pllne, curve, &acc );

  double maxAcc = err.GetMaxError() == MB_MAXDOUBLE ? acc[2] : err.GetMaxError();

  if ( maxAcc != MB_MAXDOUBLE ) {
    const TCHAR * format1 = _T("%8.16f");
    const TCHAR * format2 = _T("%e");
    const size_t bufSize = 512;
    TCHAR * buf = new TCHAR[bufSize];
    size_t len = 0;
    len += (size_t)_sntprintf( buf + len, bufSize - len, _T("Максимальная ошибка:      ") );
    len += (size_t)_sntprintf( buf + len, bufSize - len, maxAcc > 0.0001 ? format1 : format2, maxAcc );
    len += (size_t)_sntprintf( buf + len, bufSize - len, _T("\nОшибка в первой точке:    "));
    len += (size_t)_sntprintf( buf + len, bufSize - len, acc[0] > 0.0001 ? format1 : format2, acc[0] );
    len += (size_t)_sntprintf( buf + len, bufSize - len, _T("\nОшибка в последней точке: ") );
    len += (size_t)_sntprintf( buf + len, bufSize - len, acc[1] > 0.0001 ? format1 : format2, acc[1] );
    MessageBoxEx( buf, _T( "Точность аппроксимации" ) ); // Сообщение о точности.
    delete[] buf;
    if ( res == rt_AccuracyError )
      MessageBoxEx( _T( "Заданная точность аппроксимации не достигнута!" ), IDS_TITLE_WRN );

    TestVariables::drawImp->DrawPoint( &err._ptMax, TRGB_BLACK );
  }
  else {
    MessageBoxEx( _T( "Точность аппроксимации не определена" ), _T( "Точность аппроксимации" ) ); // Сообщение о точности.
  }
}


static const char * gPrintFormat = "%8.16f";

void PrintPolylineCurve( const MbCurve3D * curve, const char * name )
{
  const MbPolyCurve3D * nurbs = static_cast<const MbPolyCurve3D *>( curve );
  std::vector<MbCartPoint3D> points;
  nurbs->GetPoints( points );
  std::ofstream out( name );
  if ( out.is_open() ) {
    char buff[512];
    out << "Points:" << std::endl;
    for ( size_t i = 0; i < points.size(); ++i ) {
      for ( size_t j = 0; j < 2 && i < points.size(); j++ ) {
        snprintf( buff, 512, gPrintFormat, points[i].x );
        out << buff << ", ";
        snprintf( buff, 512, gPrintFormat, points[i].y );
        out << buff << ", ";
        snprintf( buff, 512, gPrintFormat, points[i].z );
        out << buff << ", ";
        if ( j != 1 ) i++;
      }
      out << std::endl;
    }
    out.close();
  }
}

void PrintNurbsCurve( const MbCurve3D * curve, const char * name )
{
  const MbNurbs3D * nurbs = static_cast<const MbNurbs3D *>( curve );
  std::vector<MbCartPoint3D> points;
  nurbs->GetPoints( points );
  std::vector<double> weights, knots;
  nurbs->GetWeights( weights );
  nurbs->GetKnots( knots );
  std::ofstream out( name );
  if ( out.is_open() ) {
    char buff[512];
    out << "Points:" << std::endl;
    for ( size_t i = 0; i < points.size(); ++i ) {
      for ( size_t j = 0; j < 2 && i < points.size(); j++ ) {
        snprintf( buff, 512, gPrintFormat, points[i].x );
        out << buff << ", ";
        snprintf( buff, 512, gPrintFormat, points[i].y );
        out << buff << ", ";
        snprintf( buff, 512, gPrintFormat, points[i].z );
        out << buff << ", ";
        if ( j != 1 ) i++;
      }
      out << std::endl;
    }
    out << std::endl;

    out << std::endl << "Weights:" << std::endl;
    for ( size_t i = 0; i < weights.size(); ++i ) {
      for ( size_t j = 0; j < 6 && i < weights.size(); ++j ) {
        snprintf( buff, 512, gPrintFormat, weights[i] );
        out << buff << ", ";
        if ( j != 5 ) i++;
      }
      out << std::endl;
    }
    out << std::endl;

    out << std::endl << "Knots:" << std::endl;
    for ( size_t i = 0; i < knots.size(); ++i ) {
      for ( size_t j = 0; j < 6 && i < knots.size(); ++j ) {
        snprintf( buff, 512, gPrintFormat, knots[i] );
        out << buff << ", ";
        if ( j != 5 ) i++;
      }
      out << std::endl;
    }

    out.close();
  }
}

//----------------------------------------------------------------------------------------
///< \ru  Вывести сообщение по номеру ошибки. \en Display a message by error number.
// ---
const string_t & MessageError( int number )
{
  static const string_t errorRu[] = {

    //   СООБЩЕНИЯ ОБ ОШИБКАХ // НЕ СОРТИРОВАТЬ !!!
    //   МОЖНО ТОЛЬКО РЕДАКТИРОВАТЬ

    /*****************************************************************************/
    /* 0 */   _T( "*** НЕКОРРЕКТНЫЕ ОБЪЕКТ / ПАРАМЕТРЫ!" ),
    /* 1 */   _T( "*** НЕКОРРЕКТНЫЕ ДАННЫЕ!" ),
    /* 2 */   _T( "Некорректные формы ломаных / направления касательных." ),
    /* 3 */   _T( "Некорректная структура ломаной с прямолинейными участками." ),
    /* 4 */   _T( "Слишком мало точек." ),
    /* 5 */   _T( "Совпадение точек." ),
    /* 6 */   _T( "Слишком острый угол между сегментами ломаной." ),
    /* 7 */   _T( "Обратный ход сегмента." ),
    /* 8 */   _T( "Резкое кручение 3D ломаной." ),
    /* 9 */   _T( "Некорректное направление касательной в начальной точке." ),
    /* 10 */  _T( "Некорректное направление касательной в конечной точке." ),
    /* 11 */  _T( "Первая касательная задана к прямолинейному участку." ),
    /* 12 */  _T( "Последняя касательная задана к прямолинейному участку." ),
    /* 13 */  _T( "Касательные векторы не корректны для локально-выпуклого участка." ),
    /* 14 */  _T( "Некорректная структура исходной кривой Безье." ),
    /* 15 */  _T( "Нет пересечения касательных. Некорректная структура ломаной." ),
    /* 16 */  _T( "Критическая конфигурация для B-сплайновой аппроксимации." ),
    /* 17 */  _T( "Некорректные исходные данные." ),
    /* 18 */  _T( "Данная команда не реализована в FairCurveModeler & C3D." ),
    /* 19 */  _T( "Недостаточно памяти." ),
    /* 20 */  _T( "Некорректное направление первой касательной." ),
    /* 21 */  _T( "Некорректное направление последней касательной." ),
    /* 22 */  _T( "Конечный прямолинейный участок. Касательный вектор не нужен." ),
    /* 23 */  _T( "Некорректная структура 3D ломаной с прямолинейными участками." ),
    /* 24 */  _T( "Некорректная структура данных в модели." ),
    /* 25 */  _T( "Объект - не пространственная ломаная." ),
    /* 26 */  _T( "Отсутствует вектор узлов." ),
    /* 27 */  _T( "Замкнутый сплайн во внешнем файле." ),
    /* 28 */  _T( "Не выбран нужный объект." ),
    /* 29 */  _T( "Резкое изменение кривизны на участке перегиба." ),
    /* 30 */  _T( "Близость к прямолинейности." ),
    /* 31 */  _T( "Слишком мало точек на выпуклом участке начального участка." ),
    /* 32 */  _T( "Запрет на замкнутую конфигурацию при количестве точек < 5." ),
    /* 33 */  _T( "Некорректная структура исходного геометрического определителя." ),
    /* 34 */  _T( "Размеры массивов точек." ),
    /* 35 */  _T( "Касательные не пересекаются." ),
    /* 36 */  _T( "Точки совпадают." ),
    /* 37 */  _T( "Мало точек для замкнутой ломаной." ),
    /* 38 */  _T( "Слишком мало точек на выпуклом участке." ),
    /* 39 */  _T( "Точки на прямой." ),
    /* 40 */  _T( "Слишком много точек." ),
    /* 41 */  _T( "Трехсегментный участок не выпуклый." ),
    /* 42 */  _T( "Кривизна за пределами допустимого диапазона." ),
    /* 43 */  _T( "Объект не ГО Эрмита." ),
    /* 44 */  _T( "Объект не NURBzS." ),
    /* 45 */  _T( "V-кривая вырождается. Отключите опцию улучшения / Измените форму ГО." ),
    /* 46 */  _T( "Пилообразная опорная ломаная / опорная ломаная на касательной ломаной." ),
    /* 47 */  _T( "Отказ от ввода." )

  };
  static const string_t errorEn[] = {

    //   СООБЩЕНИЯ ОБ ОШИБКАХ // НЕ СОРТИРОВАТЬ !!!
    //   МОЖНО ТОЛЬКО РЕДАКТИРОВАТЬ

    /*****************************************************************************/
    /* 0 */   _T( "*** INCORRECT OBJECT / PARAMS!" ),
    /* 1 */   _T( "*** INCORRECT DATA!" ),
    /* 2 */   _T( "Incorrect polylines / tangent directions." ),
    /* 3 */   _T( "Incorrect structure of 3d poly with straight sites." ),
    /* 4 */   _T( "Too few points." ),
    /* 5 */   _T( "Coincidence of points." ),
    /* 6 */   _T( "Too acute angle between segments of polyline." ),
    /* 7 */   _T( "Return motion of segment." ),
    /* 8 */   _T( "Sharp torsion of spatial 3d-polyline." ),
    /* 9 */   _T( "Incorrect direction of first tangent vector." ),
    /* 10 */  _T( "Incorrect direction of last tangent vector." ),
    /* 11 */  _T( "First tangent vector is set to a rectilinear site of a polyline." ),
    /* 12 */  _T( "Last tangent vector is set to a rectilinear site of a polyline." ),
    /* 13 */  _T( "Tangent vectors are not suitable to convex shape of curve." ),
    /* 14 */  _T( "Incorrect structure of the initial Bezier curve." ),
    /* 15 */  _T( "No intersection of tangents. Incorrect structure of polyline." ),
    /* 16 */  _T( "Critical configuration for B-Spline approximation." ),
    /* 17 */  _T( "Incorrect initial data." ),
    /* 18 */  _T( "This command is not implemented in the FairCurveModeler & C3D." ),
    /* 19 */  _T( "Insufficient memory." ),
    /* 20 */  _T( "Incorrect direction of first tangent." ),
    /* 21 */  _T( "Incorrect direction of last tangent." ),
    /* 22 */  _T( "Straighten last site. Last tangent Ignored." ),
    /* 23 */  _T( "Incorrect structure of 3d poly with straight sites." ),
    /* 24 */  _T( "Incorrect data structure in the model." ),
    /* 25 */  _T( "Object is not a 3D polyline." ),
    /* 26 */  _T( "Missing knots vector." ),
    /* 27 */  _T( "Closed spline in an external file." ),
    /* 28 */  _T( "Object not selected." ),
    /* 29 */  _T( "A sharp change in curvature at the inflection site." ),
    /* 30 */  _T( "Сlose to straightness." ),
    /* 31 */  _T( "Too few points on convex start site of curve" ),
    /* 32 */  _T( "A ban on a closed configuration when the number of points < 5." ),
    /* 33 */  _T( "Incorrect structure of the initial geometric determinant." ),
    /* 34 */  _T( "Dimensions of points arrays." ),
    /* 35 */  _T( "Tangents do not intersect." ),
    /* 36 */  _T( "Points coincide." ),
    /* 37 */  _T( "Few points for a closed polyline." ),
    /* 38 */  _T( "Too few points on convex end site of curve." ),
    /* 39 */  _T( "Points on the straight line." ),
    /* 40 */  _T( "Too many points." ),
    /* 41 */  _T( "three-linked site is not convex" ),
    /* 42 */  _T( "curvatute out of range " ),
    /* 43 */  _T( "The object is not Hermite GD." ),
    /* 44 */  _T( "The object is not NURBzS." ),
    /* 45 */  _T( "V-curve degenerates. Disable the fairing option / Change GD form." ),
    /* 46 */  _T( "Sawtooth base polyline / base polyline on tangent polyline." ),
    /* 47 */  _T( "Refusing of input" )

  };

#if defined ( __NATIVE_LANGUAGE__ )
  const string_t * error = errorRu;
#else
  const string_t * error = errorEn;
#endif

  return error[number - (int)rt_FullFilletError];
}


//----------------------------------------------------------------------------------------
///< \ru  Вывести сообщение по номеру предупреждения. \en Display a message by warning number.
// ---
const string_t & MessageWarning( int number )
{
  static const string_t warnRu[] = {

    //   СООБЩЕНИЯ ДЛЯ S_MODELLER // НЕ СОРТИРОВАТЬ !!!
    //   МОЖНО ТОЛЬКО РЕДАКТИРОВАТЬ

    /*****************************************************************************/
    /* 0 */    _T( "" ),
    /* 1 */    _T( "Некорректное направление первой касательной. Игнорируется" ),
    /* 2 */    _T( "Прямолинейный первый участок. Первый касательный вектор игнорируется." ),
    /* 3 */    _T( "Некорректные формы ломаных / направления касательных." ),
    /* 4 */    _T( "" ),
    /* 5 */    _T( "" ),
    /* 6 */    _T( "" ),
    /* 7 */    _T( "" ),
    /* 8 */    _T( "Критическая конфигурация." ),
    /* 9 */    _T( "Конфигурация ломаной не позволяет установить кривизну с заданным знчением." ),
    /* 10 */   _T( "Точность на прямолинейных / критических участках" ),
    /* 11 */   _T( "Точность на прямолинейных участках" ),
    /* 12 */   _T( "Конфигурация ломаной не позволяет установить кривизну с заданным знчением." ),
    /* 13 */   _T( "Критический участок. Погрешность превышает заданную точность." ),
    /* 14 */   _T( "" ),
    /* 15 */   _T( "Неуместный перегиб" ),
    /* 16 */   _T( "Значение кривизны вне диапазона допустимых значений" ),
    /* 17 */   _T( "Пилообразная ломаная" ),
    /* 18 */   _T( "" ),
    /* 19 */   _T( "" ),
    /* 24 */   _T( "" )

  };

  static const string_t warnEn[] = {

    //   СООБЩЕНИЯ ДЛЯ S_MODELLER // НЕ СОРТИРОВАТЬ !!!
    //   МОЖНО ТОЛЬКО РЕДАКТИРОВАТЬ

    /*****************************************************************************/
    /* 0 */   _T( "" ),
    /* 1 */   _T( "Incorrect direction of first tangent. Ignored." ),
    /* 2 */   _T( "Straighten start site. First tangent ignored." ),
    /* 3 */   _T( "Incorrect polylines / tangent directions." ),
    /* 4 */   _T( "" ),
    /* 5 */   _T( "" ),
    /* 6 */   _T( "" ),
    /* 7 */   _T( "" ),
    /* 8 */   _T( "Critical configuration" ),
    /* 9 */   _T( "Configuration of polyline does not allow set this value of curvature" ),
    /* 10 */  _T( "Accuracy of approximation rectilinear / critical sites" ),
    /* 11 */  _T( "Accuracy of approximation on straight sites" ),
    /* 12 */  _T( "Configuration of polyline does not allow set this value of curvature" ),
    /* 13 */  _T( "The Critical site. The error is more than set accuracy." ),
    /* 14 */  _T( "" ),
    /* 15 */  _T( "Unexpected inflection" ),
    /* 16 */  _T( "Curvatute out of range " ),
    /* 17 */  _T( "Sawtooth polyline" ),
    /* 18 */  _T( "" ),
    /* 19 */  _T( "" ),
    /* 24 */  _T( "" )

  };

#if defined ( __NATIVE_LANGUAGE__ )
  const string_t * warn = warnRu;
#else
  const string_t * warn = warnEn;
#endif

  return warn[number];
}


///////////////////////////////////////////////////////////////////////////////
//                      Строки сообщений.
///////////////////////////////////////////////////////////////////////////////
const TCHAR * gFairingMsg1 =    _T( "Улучшать кривую: нет = 0, да = 1" );
const TCHAR * gFairingMsg2 =    _T( "Улучшать кривую: нет = 0, да = 1, улучшить с коррекцией углов = 2, улучшить со сглаживанием в коридоре = 3" );
const TCHAR * gFairingMsg3 =    _T( "Улучшать кривую: нет = 0, да = 1, с коррекцией углов = 2,  на зашумленных точках: в коридоре = 3, по средней линии = 4" );
const TCHAR * gFairingMsg4 =    _T( "Сглаживать кривую на зашумленных точках: нет = 0, да = 1" );
const TCHAR * gFairingMsg5 =    _T( "Улучшать кривую на зашумленных точках: в коридоре = 3, по средней линии = 4, комбинированно = 5" );
const TCHAR * gFairingMsg6 =    _T( "Улучшать кривую: 0-нет,1-да,2-с острыми углами. Сглаживать: в коридоре-3,по средней линии-4,комби-5" );

const TCHAR * gSubdivisionMsg = _T( "Уплотнение кривой: не уплотнять = 0, однократно = 1, двукратно = 2" );
const TCHAR * gInflexMsg =      _T( "Участки перегиба: фиксированные точки перегиба = 0, плавные  = 1, супер плавные = 2" );
const TCHAR * gArrangeMsg =     _T( "Гармонизировать распределение точек: нет = 0, да = 1" );
const TCHAR * gApproxMsg =      _T( "Построение сплайна: по точкам = 0, B-сплайн = 1, Nurbs = 2" );
const TCHAR * gApproxMsg2 =     _T( "Тип аппроксимации: NURBzS = 0, B-Spline = 1");
const TCHAR * gApproxMsg3 =     _T( "Репараметризация по узлам ломаной = 0, по количеству сегментов = 1, гармоничное = 2" );
const TCHAR * gDegreeMsg =      _T( "Степень B-сплайна: от 2 до 10" ); // Компас может прислать значение degree = 2
const TCHAR * gTangentCorrMsg = _T( "Сохранить в B-кривой касательные в точках перегиба: нет = 0, да = 1" );
const TCHAR * gTorsionMsg =     _T( "Сглаживать кручение: нет = 0, да = 1" );
const TCHAR * gAccurChangeMsg = _T( "Изменить параметры точности: 0 - не изменять, 1- изменить" );
const TCHAR * gBoundCond =      _T( "Задать граничные условия (да = 1, нет = 0)" );
const TCHAR * gTangentType1 =   _T( "Тип касательной в первой точке: Координаты вектора = 0, Указать точку = 1, Указать объект = 2" );
const TCHAR * gTangentType2 =   _T( "Тип касательной в последней точке: Координаты вектора = 0, Указать точку = 1, Указать объект = 2" );
const TCHAR * gCoordX =         _T( "Координата по X: " );
const TCHAR * gCoordY =         _T( "Координата по Y: " );
const TCHAR * gCoordZ =         _T( "Координата по Z: " );
const TCHAR * gPointMsg =       _T( "Укажите точку" );
const TCHAR * gCurveMsg =       _T( "Укажите кривую" );
const TCHAR * gCurvatureType1 = _T( "Тип кривизны в первой точке:  Значение = 0, Вектор кривизны = 1, Вектор радиуса = 2, Указать объект = 3" );
const TCHAR * gCurvatureType2 = _T( "Тип кривизны в последней точке: Координаты вектора = 0, Указать точку = 1, Указать объект = 2" );
const TCHAR * gCurvatureVal =   _T( "Значение кривизны: " );
const TCHAR * gCurvatureVec =   _T( "Укажите точку вектора кривизны" );
const TCHAR * gRadiusVec =      _T( "Укажите радиус-точку вектора" );
const TCHAR * gConstructType =  _T( "Способ построения:  Дуга окружности = 0, Conica  = 1" );
const TCHAR * gDiscrimMsg =     _T( "Значение инженерного дискриминанта ( 0.1 < d < 0.9 ):" );
const TCHAR * gDrawMsg =        _T( "Показать: Кривую = 0, ГО Эрмита = 1" );
const TCHAR * gTypeGDMsg =      _T( "Тип ГО: ломаная = 0, касательная = 1, ГО Эрмита = 2" );
const TCHAR * gTypeGraphMsg =   _T( "Отобразить: график кривизны = 0, эволюты = 1" );
const TCHAR * gGraphScaleMsg =  _T( "Значение масштаба отображения  ( cvtMax / lengthCurve):" );
const TCHAR * gGraphNumSegmsMsg = _T( "Количество сегментов на дуге кривой:" );
const TCHAR * gTypeDrawMsg    = _T( "Вид отображения: линия = 0, ""ежик"" = 1" );
const TCHAR * gTypeDrawCvtMsg = _T( "Вид отображения: над кривой = 0, вида F(x) = 1" );


//15 10 2019
// Исправлена нумерация в списке ошибок на английском.
//------------------------------------------------------------------------------
// Программа для отладки, тестирования и демонстрации функциональных возможностей комплекса 
// программ FairCurveModeler изогеометрического моделирования  кривых высокого качества по критериям плавности.
// ---
void CreateFairCurve( int curveType )
{
  bool createWire = GetBool( FAIRCURVE_AS_WIREFRAME, false );

  InitFcm();
#ifdef C3D_FAIR_CURVES_COMMERCIAL
  if (Expired() > 0) {
    MessageBoxEx(_T("Истек срок лицензии / expired"), IDS_TITLE_DIAG_ERR);
    // Закрытие отчетных файлов.
    CloseFcm();
    return;

  }
#endif

  try { // Перехват аппаратного исключения. 
        // В проекте для пункта меню C/C++ ->Code Generation->Enable C++ Exceptions 
        // устанавливаем значение Yes With SEH Exceptions (/EHa).

    //Begin DEBUG Отладка программ. Перевод на стандарт C3D
    /*DEBUG*/// Test_CreateSplineSurface();  CloseFcm( ); return;
    /*DEBUG*/// Test_CreateSplineSurface2();  CloseFcm( ); return;
    /*DEBUG*/// Test_CreateSplineSurfaceKlein(); CloseFcm( );  return;
    /*DEBUG*/// Test_NetToNurbs();  CloseFcm( ); return;
    /*DEBUG*/// Test_AnalyzeNurbs();  CloseFcm(); return;
    /*DEBUG*/// ReadPllneFromGeomBufferCompas(); CloseFcm(); return;
    //End DEBUG Отладка программ. Перевод на стандарт C3D
    //Test_Smoothing_Noisy_Data(createWire); CloseFcm(); return;
    //  TEST.  Методы для разработчиков САПР и приложений
    switch ( curveType ) {
      case 1: //CM_FAIRCURVEONPOINTS_BZ   : // Плавный Безье-сплайн на опорной ломаной, Fair Bezier-Spline on base polyline
        Test_CreateFairBezierCurveOnBasePolyline( createWire ); //CreateFairCurve( 1 );
        break;

      case 2: //CM_FAIRCURVEONTANGENTS_BZ : // Плавный  Безье-сплайн на касательной ломаной, Fair Bezier-Spline on tangent polyline
        Test_CreateFairBezierCurveOnTangentPolyline( createWire ); // CreateFairCurve( 2 );
        break;
      case 3: // CM_FAIRCURVEONPOINTS_BS : // Плавный B-сплайн на опорной ломаной.
        Test_CreateFairBSplineCurveOnBasePolyline( createWire ); //CreateFairCurve( 3 );
        break;

      case 4: //CM_FAIRCURVEONTANGENTS_BS : // Плавный B-сплайн на касательной ломаной, Fair B-Spline on tangent polyline
        Test_CreateFairBSplineCurveOnTangentPolyline( createWire ); //CreateFairCurve( 4 );
        break;

      case 5: //CM_FAIRCURVEONPNTHGD_BZ : // Плавный Безье-сплайн на опорной ломаной ГО Эрмита, Fair Bezier-Spline on base polyline of Hermite GD
        Test_CreateFairBezierCurveOnBasePolylineOfHermiteGD( createWire ); //CreateFairCurve( 5 );
        break;

      case 6: // CM_FAIRCURVEONTNGHGD_BZ : // Плавный Безье-сплайн на касательных ГО Эрмита, Fair Bezier-Spline on tangents of Hermite GD 
        Test_CreateFairBezierCurveOnTangentsOfHermiteGD( createWire ); //CreateFairCurve( 6 );
        break;

      case 7: //CM_FAIRCURVEONPNTHGD_BS : // Плавный B-сплайн на опорной ломаной ГО Эрмита, Fair B-Spline on base polyline of Hermite GD
        Test_CreateFairBSplineCurveOnBasePolylineOfHermiteGD( createWire ); //CreateFairCurve( 7 );
        break;

      case 8: //CM_FAIRCURVEONTNGHGD_BS : // Плавный B-сплайн на касательных ГО Эрмита, Fair B-Spline on tangents of Hermite GD
        Test_CreateFairBSplineCurveOnTangentsOfHermiteGD( createWire ); //CreateFairCurve( 8 );
        break;

      case 9: //CM_CURVEONPNTHGD_BZ : // Безье-сплайн на ГО Эрмита, Bezier-Spline on Hermite GD
        Test_CreateBezierCurveOnHermiteGD( createWire ); //CreateFairCurve( 9 );
        break;

      case 10: //CM_CURVEONTNGHGD_BS : // B-сплайн на ГО Эрмита, B-Spline on Hermite GD
        Test_CreateBSplineCurveOnHermiteGD( createWire ); //CreateFairCurve( 10 );
        break;

      case 11: //CM_FAIRCURVECLOTHOID_GD : // Клотоиду, Clothoid
        Test_CreateClothoid( createWire ); //CreateFairCurve( 11 );
        break;

      case 12: //CM_FAIRCURVEONSECTRIXZ_GD : // Сектрису Маклорена,Sectrix of Maclaurin
        //  Построение кривой с монотонной кривизной в соприкасающемся треугольнике.
        Test_CreateSectrix( createWire ); // CreateFairCurve( 12 );
        break;

        // МЕТОДЫ ПРЕОБРАЗОВАНИЯ КРИВЫХ

      case 21: //CM_FAIRCURVEELEVDEGREE_BZ : // Повысить степень
        Test_ElevateDegreeBezierCurve( createWire );  //CreateFairCurve( 21 );
        break;

      case 22: // CM_FAIRCURVESUBDIVIDE_BS : // Уплотнить
        Test_SubdivideNurbsCurve( createWire ); // CreateFairCurve( 22 );
        break;

      case 23: //CM_FAIRCURVECHANEFORMAT_BS : // Изменить формат / усечь NURBS кривую.
        Test_ExtractChangeFormatNurbsCurve( createWire ); //CreateFairCurve( 23 );
        break;

      case 24: //CM_FAIRCURVEADDKNOT_BS : // Вставить узел в NURBS кривую.
        Test_AddKnotNurbsCurve( createWire );  //CreateFairCurve( 24 );
        break;

      case 25: //CM_FAIRCURVECONIC_BS : // Преобразовать конический NURBzS в конический B-Spline.
        Test_ConvertBezierCurveToConicBSpline( createWire ); //CreateFairCurve( 25 );
        break;

      case 26: //CM_FAIRCURVETOHERMITE_GD : // Построить ГО Эрмита на сплайновой кривой Безье.
        Test_CreateHermiteGDOnBezierCurve( createWire );
        break;

      case 27: //CM_FAIRCURVEONHERMITE_GD : // Оценка точности построения.
        EstimateAccuracy();
        break;

      case 28: //CM_FAIRCURVEEXCHANGE_GB : // Утилиты.
#ifdef FAIR_CURVE_ADVANCED
        Utilites();
#endif
        break;
      case 29: //CM_FAIRFILLETONPLLNE_BZ   : // Построение FairCurve Безье-сплайн сопряжения на ломаной, Bezier-Spline fillet on polyline
        Test_CreateFairBezierFilletOnPolyline( createWire );
        break;

      case 30: //CM_FAIRSEGMCHANEFORMAT_BS : // Изменить формат / усечь NURBS кривую.
        Test_ExtractExtrapolOneSegment();
        break;

      case 31: //CM_FAIRFILLETONPLLNE_BZ   : // Построение B-сплайн сопряжения на S-полигоне, B-Spline fillet on S-polygon
        Test_CreateFairBSplineFilletOnSPolygon();

      case 32: //CM_FAIRCURVEREPARAM_CV : // Репараметризовать кривую.
        Test_ReParamCurve( createWire );
        break;
      case 33: //CM_FAIRCURVEONNOISYDATA : // Репараметризовать кривую.
        Test_Smoothing_Noisy_Data( createWire );
        break;
    }
  } // try
  catch ( ... ) { // Обработка аппаратного исключения.
    MessageBoxEx( _T( "*** НЕКОРРЕКТНЫЕ ОБЪЕКТ / ПАРАМЕТРЫ!" ), IDS_TITLE_ERR );
    // Закрытие отладочных файлов.
  }

  // Закрытие отчетных файлов.
  CloseFcm();
}


//////////////////////////////////////////////////////////////////
//
//  Методы для команд приложения
//
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
// Test_CreateFairBSplineCurveOnBasePolyline();
// ---
int Test_CreateFairBSplineCurveOnBasePolyline( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );
  
  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();
  int inType = 1; // опорная ломаная

  // TM Проверка не всегда работает верно!
  //if ( IsHermit( pllne ) )
  //  inType = 2; // ГО Эрмита

  size_t kArray;  // Количество опорных точек
  //Проверить количество точек в pllne.
  if ( inType == 1 )
    kArray = n;
  else
    kArray = n / 5;

  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts ); // cvrts - вершины ломаной.

  // Начальная инициализация параметров значениями по умолчанию.
  double smoothClearance = 0.002;
  int clearanceNoisyIteration = 10;
  MbeFairSmoothing fairing = fairSmooth_Yes;
  size_t degreeBSpline = 8;
  MbeFairApprox approx = fairApprox_IsoSpline;
  MbeFairSubdivision subdivision = fairSubdiv_Single;
  size_t nFixPnt = 0;
  size_t nFixTng = 0;
  int fixFirst = 0;
  int fixLast = 0;
  MbeFairVector accountInflexVector = fairVector_Tangent;
  bool tangentCorrectBspline = true;
  bool smoothTorsion = false;
  bool arrange = false;
  double realAccuracyBSpl = METRIC_ACCURACY * 0.01; //как в fair_curve_class.h
  int numberOfIterationsBSpl = 1000; //как в fair_curve_class.h

  // Чистка параметров сглаживания.
  fData.fixPntTng = fixPntTng_NotFix; // Признак фиксации касательных. // TODO

  //c3d::SpaceCurveSPtr geomPllnePrt; // Для удаления полилинии из GeomPreProcessor.
  //MbCurve3D * tmpPllne = nullptr;
  //GeomPreProcessor( inType, true, pllne, n, nSites, nStrSites, nMinPntSite, tmpPllne );

  //int no = 0;
  //if ( inType == 1 ) { // Включить фильтрацию ломаной.
  //  if ( GetInt( _T( "Включить фильтр: 0 - нет, 1 - да" ), no ) ) {
  //    if ( no > 0 ) {
  //      GeomPreProcessor( inType, true, tmpPllne, n, nSites, nStrSites, nMinPntSite, tmpPllne );  // Для точного nMinPntSite
  //      pllne = tmpPllne;
  //      geomPllnePrt = tmpPllne;
  //    }
  //  }
  //}

  fairing = (MbeFairSmoothing)GetIntValue( 1, 0, 5, gFairingMsg6 );

  c3d::SpaceCurveSPtr geomPllnePrt; // Для удаления полилинии из GeomPreProcessor.
  if ( fairing >= fairSmooth_Noisy ) {
    if ( GetDouble( _T( "Коридор сглаживания: " ), smoothClearance ) ) {
      if ( smoothClearance < 0. )
        smoothClearance = 0.;
    }
    clearanceNoisyIteration = GetIntValue( 10, 2, 1000, _T( "Количество итераций сглаживания: " ) );

    // Может изменить количество точек. Сбивает нумерацию при фиксации точек и касательных.
    // Рекомендуется предварительно отработать регуляризацию командой Utilites.
    // Здесь только для определения nSites, nStrSites, nMinPntSite.
    if ( inType == 1 ) {
      // !!!!!!!!!!!!!!!
      // TM В тестовом приложении не должно быть подмены входных данных (в данном случае полилинии)!!!
      // Вся обработка должна быть внутри функции CreateFairBSplineCurveOnBasePolyline.
      // !!!!!!!!!!!!!!!!
      /*if ( fairing >= fairSmooth_Noisy ) {
        c3d::SpaceCurveSPtr geomPllnePrt2; // Для удаления полилинии из GeomPreProcessor.
        MbCurve3D * tmpPllne = nullptr;
        GeomPreProcessor( inType, true, pllne, n, nSites, nStrSites, nMinPntSite, tmpPllne );
        geomPllnePrt2 = tmpPllne;
        GeomPreProcessor( inType, true, tmpPllne, n, nSites, nStrSites, nMinPntSite, tmpPllne );  // Для точного nMinPntSite
        geomPllnePrt = tmpPllne;
        pllne = tmpPllne;
      }*/
      kArray = n;
    }
#ifdef FAIR_CURVE_ADVANCED
    else if ( inType == 2 ) {
      //ГО - отфильтрованная ломаная без совпадающих и выпадаюшщих точек, без прямолинейных участков после GeomPreProcessor для сглаживания.
      // Отсортировать исходную ломаную. Удаляются совпадающие точки.
      size_t nSites = 0;        // количество выпуклых участков;
      size_t nStrSites = 0;     // количество прямолинейных участков;
      size_t nMinPntSite = 0;   // минимальное количество точек на участке.
      // Проверить ГО Эрмита на минимальный участок.
      kArray = n/5;
      MbCurve3D * pllne2 = nullptr;
      { // Преобразовать в ломаную.
        pllne->GetBasisPoints( cvrts ); // cvrts - вершины ломаной.
        SArray<MbCartPoint3D> polyLine; // Массив точек ломаной;
        for ( size_t ir = 0; ir < kArray; ir++ ) {
          polyLine.Add( cvrts[ir*5] );
        }
        closed = false;
        ::SplineCurve( polyLine, closed, st_Polyline3D, pllne2 );
      }
     
      GeomPreProcessor( 1, true, pllne2, kArray, nSites, nStrSites, nMinPntSite, pllne2 );
      //GeomPreProcessor( inType, true, pllne, n, nSites, nStrSites, nMinPntSite, pllne );  // Для точного nMinPntSite
      //ГО - отфильтрованная ломаная без совпадающих и выпадаюшщих точек, без прямолинейных участков после GeomPreProcessor для сглаживания.
      
      if ( nMinPntSite < 8 ) {
        // Слишком мало точек для сглаживания по коридору
        string_t message( IDS_TITLE_ERR );
        message = MessageError( 4 );                  // Формирует текст сообщения по номеру ошибки.
        MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
        outError = 4;
        return outError;
      }
    }
#endif
  }

  if ( approx == 0 )
    subdivision = fairSubdiv_No;

  // Точная фиксация зашумленных точек
  // Фиксация касательных.
  int no = 2;

  MbeFixPntTng fixPntTng = fixPntTng_NotFix; // Флаг включения фикс касательных

  // После ввода данных все массивы имеют одинаковый размер.
  IndexVectorPtrArray fixConstraints;
  SArray<MbCartPoint3D> arrayFixPnts; // Фиксированные точки.

  if ( fairing == 3 ) {  // Фиксация точек  и касательных.
    for ( size_t ir = 0; ir < n; ir++ ) {
      if ( GetInt( _T( "Укажите по возрастанию номер опорной точки для точной фиксации" ), no ) == false )
        break;
      MbCartPoint3D basePnt = cvrts[no];
      if ( inType == 1 ) {
        fixConstraints.push_back( std::pair<size_t, MbVector3D *>( (size_t)no, nullptr ) ); // Номер
      }
      else if ( inType == 2 ) {
        basePnt = cvrts[no * 5];
        fixConstraints.push_back( std::pair<size_t, MbVector3D *>( (size_t)( no * 5 ), nullptr ) ); // Номер
      }
      arrayFixPnts.Add( basePnt );
      nFixPnt++;

      // Фиксировать касательную в этой точке?
      MbVector3D * tangVec = nullptr;
      if ( GetBool( _T( "Фиксировать вектор касательной?" ), false ) ) {
        MbCartPoint3D  cursorPnt;
        MbVector3D  cursorVector;
        if ( inType == 1 ) {
          cursorVector = cvrts[no + 1] - cvrts[no - 1];
          cursorVector.Normalize();
          cursorPnt.Init( cursorVector.x, cursorVector.y, cursorVector.z );
          GetExactPoint( cursorPnt );
          basePnt = cvrts[no];
        }
        else if ( inType == 2 ) {
          cursorVector = cvrts[no * 5 + 1] - cvrts[no * 5];
          cursorVector.Normalize();
          cursorPnt.Init( cursorVector.x, cursorVector.y, cursorVector.z );
          basePnt = cvrts[no * 5];
        }

        // Берется из ГО Эрмита. Можно менять направление касательной на ГО Эрмита.
        cursorVector.Init( cursorPnt.x, cursorPnt.y, cursorPnt.z );
        cursorVector.Normalize();
        if ( inType == 1 ) {
          // Проверить корректность направления касательной
          MbVector3D v1, v2, vOrt1, vOrt2;
          v1 = cvrts[no] - cvrts[no - 1];
          v2 = cvrts[no + 1] - cvrts[no];
          vOrt1 = v1 | cursorVector;
          vOrt2 = cursorVector | v2;
          vOrt1.Normalize();
          vOrt2.Normalize();
          if ( false /*vOrt1 * vOrt1 < 0.001*/ ) { // Отключила.
            string_t message = _T( "" );

            MessageBoxEx( _T( "Некорректное направление касательной" ), IDS_TITLE_ERR );
            continue;
          }
          else {
            tangVec = new MbVector3D( cursorVector );
            fixPntTng = fixPntTng_Fix;
          }
        }
      }
      fixConstraints.back().second = tangVec;
      nFixTng++;

      no = no + 2; // Во избежание взаимного гашения фиксированные точки должны отстоять друг от друга достаточно далеко.
      if ( no > ( int )( kArray - 2 ) )
        break;
    }
  }
  // Изменить ГО Эрмита
  if ( nFixTng > 0 && inType == 2 ) { // TM В тестовом приложении не должно быть замены входных данных!!!
    bool updatePoly = false;
    for ( size_t ir = 0; ir < fixConstraints.size(); ir++ ) {
      size_t ind = fixConstraints[ir].first;
      if ( fixConstraints[ir].second != nullptr ) {
        updatePoly = true;
        cvrts[ind * 5 + 1] = cvrts[ind * 5] + *fixConstraints[ir].second;
      }
    }
    if ( updatePoly ) {
      SArray<MbCartPoint3D> polyLine; // Массив точек ломаной;
      for ( size_t ir = 0; ir < n; ir++ ) {
        polyLine.Add( cvrts[ir] );
      }
      ::SplineCurve( polyLine, false, st_Polyline3D, pllne );
    }
  }

  if ( fairing > fairSmooth_Noisy ) {
    if ( GetInt( _T( "Фиксировать начальную точку (0 - нет, 1 - да)" ), fixFirst ) ) {
      if ( fixFirst < 0 )
        fixFirst = 0;
    }
    if ( GetInt( _T( "Фиксировать конечную точку (0 - нет, 1 - да)" ), fixLast ) ) {
      if ( fixLast < 0 )
        fixLast = 0;
    }
  }
  subdivision = fairSubdiv_No; //fairSubdiv_Single; //DEBUG20230110
  if ( approx > 0 && fairing < fairSmooth_Noisy ) {
    int typeApprox = 0;
    degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg ); // Компас может прислать значение degree = 2
    if ( degreeBSpline != ( degreeBSpline / 2 ) * 2 ) {  // Нечетная степень.
     typeApprox = (MbeFairApprox)GetIntValue( 0, 0, 1, gApproxMsg );
    }
    if ( typeApprox == 0 ) {  // Четная степень.
      subdivision = (MbeFairSubdivision)GetIntValue( subdivision, 0, 2, gSubdivisionMsg );
      accountInflexVector = (MbeFairVector)GetIntValue( accountInflexVector, 0, 2, gInflexMsg );
      tangentCorrectBspline = GetIntValue( tangentCorrectBspline, 0, 1, gTangentCorrMsg ) > 0;

      int arrangeI = arrange;
      if ( accountInflexVector == 0 ) {
        arrangeI = GetIntValue( arrangeI, 0, 1, gArrangeMsg );
      }
      arrange = arrangeI > 0;
      smoothTorsion = GetIntValue( smoothTorsion, 0, 1, gTorsionMsg ) > 0;
      //==
      if ( GetIntValue( 0, 0, 1, gAccurChangeMsg ) > 0 ) {
        //Точность построения
        double delta = realAccuracyBSpl;
        if ( GetDouble( _T( "Точность построения B-кривой: " ), delta ) ) {
          if ( delta < 0. )
            delta = 0.;
        }

        realAccuracyBSpl = delta;

        if ( GetInt( _T( "Ограничение количества итераций при подгонке В-сплайном:" ), numberOfIterationsBSpl ) ) {
          if ( numberOfIterationsBSpl < 0 )
            numberOfIterationsBSpl = 10;
        }
      }
    }
  }
  // Параметры аппроксимации
  int s_numThreshold1 = 999;  // Гармоничное распределение.
  int s_numThreshold2 = 9999; // Распределение по узлам.
  int outSegments = 0;
  int typeSegm = 0; 
  double scaleLength = 4.;
  if ( approx > 0 && fairing > fairSmooth_Noisy ) {  //BSpline аппроксимация
    // Репараметризация по узлам исходной ломаной или по заданному количеству сегментов
    typeSegm = GetIntValue( 0, 0, 2, gApproxMsg3 );
    if ( typeSegm == 0 )
      outSegments = s_numThreshold2;   // Не  = 0;. Используется как параметр. Сегментация по узлам.
    if ( typeSegm == 2 )
      outSegments = s_numThreshold1;   // Не менять. Используется как параметр. Сегментация гармоничная.
    if ( typeSegm == 1 ) {  // Репараметризация с количеством сегментов.
      GetInt( _T( "Укажите количество сегментов >= 6:" ), outSegments );
      if ( outSegments < 6 )
        outSegments = 6;

      GetDouble( _T( "Укажите масштаб влияния длины (> 1.0):" ), scaleLength );
      if ( scaleLength < 1. )
        scaleLength = 1.;
    }
    degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg );
    if ( (int)degreeBSpline > outSegments + 2 ) //DEBUG20220927 уточнение степени по количеству сегментов.
      degreeBSpline = outSegments - 2;
  }
  if ( approx > 0 && fixPntTng == fixPntTng_Fix ) {  //BSpline аппроксимация по узлам
    outSegments = s_numThreshold2;
    degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg );
  }

  MbeFairWarning warning( fwarn_Success );

  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._fairing = fairing;
    parameters._degree = degreeBSpline;
    parameters._approx = approx;
    parameters._subdivision = subdivision;
    parameters._accountVectorInflect = accountInflexVector;
    parameters._tangentCorrect = tangentCorrectBspline;
    parameters._arrange = arrange;
    parameters._smoothTorsion = smoothTorsion;
    parameters.SetPrecision( realAccuracyBSpl );
    parameters._iterationsNumber = numberOfIterationsBSpl;
    parameters._noisyDeviation = smoothClearance;
    parameters._noisyIterations = (size_t)clearanceNoisyIteration;
    parameters._fixFirstPointNoisy = fixFirst > 0;
    parameters._fixLastPointNoisy = fixLast > 0;
#ifdef C3D_DEBUG_FAIR_CURVES
    //parameters.prt = prt;
#endif

    // Создать новые.
    if ( nFixPnt > 0 ) // Фиксация точек и касательных.
      parameters.InitFixConstraints( fixConstraints );

    MbFairCurveMethod method( fairCrvType_BSpline, fairData_Polyline );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams(*polyPtr, snMaker), method, parameters,
                                    resCurve );

    if ( IsSuccess(res) ) { // Успешное завершение.
      c3d::SpaceCurveSPtr curve = resCurve.ExtractCurve();
      // Отобразить кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, curve );
      PrintApproxErr( pllne, curve, resCurve.GetApproxError(), res ); // Сообщение о точности.
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );                  // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.fairing = fairing;
    fData.degreeBSpline = degreeBSpline;
    fData.approx = approx;
    fData.subdivision = subdivision;
    fData.accountInflexVector = accountInflexVector;
    fData.tangentCorrectBspline = tangentCorrectBspline;
    fData.arrange = arrange;
    fData.smoothTorsion = smoothTorsion;
    fData.realAccuracyBSpl = realAccuracyBSpl;
    fData.numberOfIterationsBSpl = numberOfIterationsBSpl;
    fData.clearanceNoisy = smoothClearance;
    fData.clearanceNoisyIteration = (size_t)clearanceNoisyIteration;
    fData.switchFixFirstPointAtNoisy = fixFirst != 0;
    fData.switchFixLastPointAtNoisy = fixLast != 0;

    // Фиксация точек и касательных.
    if ( nFixPnt > 0 ) {
      fData.InitFixConstraints( fixConstraints );
      fData.fixPntTng = fixPntTng;
    }

    // Репараметризация
    fData.outFormat = fairFormat_Close;
    fData.nSegments = outSegments;
    fData.scaleParam = scaleLength;

#ifdef C3D_DEBUG_FAIR_CURVES
    fData.prt = prt;
#endif
    
    MbCurve3D * resCurve = nullptr;
    ::CreateFairBSplineCurveOnBasePolyline( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Отобразить кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
      PrintApproxErr( pllne, resCurve, fData.errInfo, fData.error ); // Сообщение о точности.
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error );          // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }

    warning = fData.warning;
    outError = fData.error;
  }

  for ( auto v : fixConstraints )
    delete v.second;
  fixConstraints.clear();

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning );      // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBezierCurveOnBasePolyline();
// ---
int Test_CreateFairBezierCurveOnBasePolyline( bool createWire )
{
  bool closed;
  size_t n = 0;
  int outError = 0;

  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();

  // Задать параметры для построения кривой Bezier на базовой ломаной.
  // Начальная инициализация значениями по умолчанию.
  MbeFairSmoothing fairing = fairSmooth_Yes;
  bool arrange = false;
  MbeFairSubdivision subdivision = fairSubdiv_Single;
  MbeFairVector accountInflexVector = fairVector_Tangent;
  double smoothClearance = 0.002;
  int clearanceNoisyIteration = 200;

  if ( n > 3 ) {
    fairing = (MbeFairSmoothing)GetIntValue( 1, 0, 3, gFairingMsg2 );
    // Сглаживание зашумленных точек
    if ( fairing == fairSmooth_Noisy ) {
      if ( GetDouble( _T( "Коридор сглаживания: " ), smoothClearance ) ) {
        if ( smoothClearance < 0. )
          smoothClearance = 0.;
      }
      clearanceNoisyIteration = GetIntValue( 200, 2, 1000, _T( "Количество итераций сглаживания: " ) );
    }
    else { // fairing != fairSmooth_Noisy
      subdivision = (MbeFairSubdivision)GetIntValue( subdivision, 0, 2, gSubdivisionMsg );
      accountInflexVector = (MbeFairVector)GetIntValue( accountInflexVector, 0, 2, gInflexMsg );

      if ( accountInflexVector == 0 && fairing > 0 )
        arrange = GetIntValue( arrange, 0, 1, gArrangeMsg ) > 0;
    }
  }

  MbeFairWarning warning( fwarn_Success );
  
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._fairing = fairing;
    parameters._arrange = arrange;
    parameters._subdivision = subdivision;
    parameters._accountVectorInflect = accountInflexVector;
    parameters._noisyDeviation = smoothClearance;
    parameters._noisyIterations = (size_t)clearanceNoisyIteration;

    MbFairCurveMethod method( fairCrvType_Bezier, fairData_Polyline );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams(*polyPtr, snMaker), method, parameters, resCurve );

    if ( IsSuccess(res) ) { // Успешное завершение.
      c3d::SpaceCurveSPtr curve = resCurve.ExtractCurve();
      // Отобразить кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, curve );
      PrintApproxErr( pllne, curve, resCurve.GetApproxError(), res ); // Сообщение о точности.
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );                  // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.fairing = fairing;
    fData.arrange = arrange;
    fData.subdivision = subdivision;
    fData.accountInflexVector = accountInflexVector;
    fData.clearanceNoisyIteration = (size_t)clearanceNoisyIteration;
    fData.clearanceNoisy = smoothClearance;

    MbCurve3D * resCurve = nullptr;
    ::CreateFairBezierCurveOnBasePolyline( pllne, fData, resCurve );


    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Отобразить кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
      PrintApproxErr( pllne, resCurve, fData.errInfo, fData.error ); // Сообщение о точности.
    }

    if ( fData.error > 0 ) { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error );          // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning );      // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//-----------------------------------------------------------------------------
// Ввод параметров для односегментной кривой Безье
// ---
int GetOneSegmParams( const SArray<MbCartPoint3D> & polyLine,
                      bool bSpline,
                      int & typeCreation,
                      bool & iFirstTangent, bool & iLastTangent, // Касательная по умолчанию.
                      bool & iFirstCvt, bool & iLastCvt,     // Кривизна по умолчанию.
                      SArray<MbVector3D> & tangents, // Векторы касательных
                      SArray <double> & curvatures ) // Кривизны.
{
  typeCreation = 0;
  bool iBound = false; //DEBUG20211219 Признак задания граничных условий.

  if ( !bSpline ) { // Ввод параметров для Безье.
    size_t n = polyLine.size();
    MbCartPoint3D firstPoint = polyLine[0], lastPoint = polyLine[n - 1], point;
    MbVector3D firstTangent, lastTangent;
    double x( 0. ), y( 0. ), z( 0. );
    double cvt( 0. );

    MbItem * obj = nullptr;
    iFirstTangent = false, iLastTangent = false; // Касательная по умолчанию.
    iFirstCvt = false, iLastCvt = false; // Кривизна по умолчанию.

    iBound = GetIntValue( 0, 0, 1, gBoundCond ) > 0; //DEBUG20211219 Признак задания граничных условий
    if ( iBound ) { // Задаются граничные параметры
      iBound = false;
      // Граничные условия.
      // Касательная в первой точке
      int typeTangent = 0;

      if ( GetInt( gTangentType1, typeTangent ) ) {
        if ( typeTangent < 0 )
          typeTangent = 0;
        if ( typeTangent > 2 )
          typeTangent = 2;

        iBound = true;

        switch ( typeTangent ) {
        case 0: // Координаты концевой точки вектора касательной
          GetDouble( gCoordX, x );
          GetDouble( gCoordY, y );
          GetDouble( gCoordZ, z );
          firstTangent[0] = x;
          firstTangent[1] = y;
          firstTangent[2] = z;
          firstTangent.Normalize();
          iFirstTangent = true;
          tangents[0] = firstTangent;
          break;

        case 1: // Указать точку
          if ( GetPoint( gPointMsg, point ) ) {
            firstTangent.Init( firstPoint, point );
            firstTangent.Normalize();
            iFirstTangent = true;
            tangents[0] = firstTangent;
          }
          break;

        case 2: // Указать объект
          // Укажите первую кривую 
          if ( GetGObj( gCurveMsg, st_Curve3D, &obj ) && obj->IsA() == st_SpaceInstance ) {
            MbNurbs3D * curve = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
            if ( curve != nullptr && ( curve->IsA() == st_Nurbs3D || curve->IsA() == st_Bezier3D ||
                                       curve->IsA() == st_Hermit3D ) ) {
              double t;
              curve->NearPointProjection( firstPoint, t, false );

#ifdef C3D_DEBUG_FAIR_CURVES
              //double FirstCvt = curve->Curvature(t);
              //fprintf(prt, "\nfirstCvt = %f", FirstCvt);
#endif
              curve->Tangent( t, firstTangent );
              // Уточнение направления касательной.
              MbVector3D v01; v01.Init( polyLine[0], polyLine[1] );
              if ( v01 * firstTangent < 0. )
                firstTangent = -firstTangent;
              iFirstTangent = true;
              tangents[0] = firstTangent;
            }
            else { // Не выбрана кривая 
              outErrors = 2;
              return outErrors;
            }
          }
          else { // Кривая не NURBS
            outErrors = 2;
            return outErrors;
          }
          break;

        } //switch (typeTangent) 
      } // Касательная в первой точке.

      if ( n == 2 && iFirstTangent ) {  // Задана касательная в первой точке.
        // Изменить кривизну в конечной точке по умолчанию. 
        // Как к окружности.
        outErrors = TangentOnSecondPoint( polyLine, tangents, cvt );
        if ( outErrors > 0 ) { // Конфигурация плохая.
          return outErrors;
        }

        curvatures[0] = 0.8 * cvt;
        curvatures[1] = 0.8 * cvt;

#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\n n =  2 TangentOnSecondPoint default\n");
        //for (size_t ir = 0; ir < n; ++ir) {
        //
        //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f",
        //    ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
        //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f",
        //    ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
        //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f",
        //    ir, curvatures[ir]);
        //
        //}
#endif
      }

      if ( GetInt( gTangentType2, typeTangent ) ) {
        if ( typeTangent < 0 )
          typeTangent = 0;
        if ( typeTangent > 2 )
          typeTangent = 2;
        iBound = true;
        switch ( typeTangent ) {
        case 0: // Координаты концевой точки вектора касательной
          GetDouble( gCoordX, x );
          GetDouble( gCoordY, y );
          GetDouble( gCoordZ, z );
          lastTangent[0] = x;
          lastTangent[1] = y;
          lastTangent[2] = z;
          lastTangent.Normalize();
          iLastTangent = true;
          tangents[n - 1] = lastTangent;
          break;

        case 1: // Указать точку
          if ( GetPoint( gPointMsg, point ) ) {
            lastTangent.Init( lastPoint, point );
            lastTangent.Normalize();
            iLastTangent = true;
            tangents[n - 1] = lastTangent;
          }
          break;

        case 2: // Указать объект
          // Укажите вторую кривую 
          if ( GetGObj( gCurveMsg, st_Curve3D, &obj ) && obj->IsA() == st_SpaceInstance ) {
            MbNurbs3D * curve = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
            if ( curve != nullptr && ( curve->IsA() == st_Nurbs3D || curve->IsA() == st_Bezier3D ||
                                       curve->IsA() == st_Hermit3D ) ) {
              double t;
              curve->NearPointProjection( lastPoint, t, false );
#ifdef C3D_DEBUG_FAIR_CURVES
              //double LastCvt = curve->Curvature(t);
              //fprintf(prt, "\nLastCvt = %f", LastCvt);
#endif
              curve->Tangent( t, lastTangent );
              iLastTangent = true;
              // Уточнение направления касатеьной.
              MbVector3D v01; v01.Init( polyLine[n - 2], polyLine[n - 1] );
              if ( v01 * lastTangent < 0. )
                lastTangent = -lastTangent;
              tangents[n - 1] = lastTangent;
            }
            else { // Не выбрана кривая 
              outErrors = 2;
              return outErrors;
            }
          }
          else { // Кривая не NURBS
            outErrors = 2;
            return outErrors;
          }
          break;
        }
      } // Задается касательная во второй точке

      if ( n == 2 && iLastTangent ) {  // Задана касательная в первой точке.
        // Изменить кривизну в конечной точке по умолчанию. 
        // Как к окружности.
        SArray<MbCartPoint3D> polyLine_r;
        polyLine_r.resize( 10 );
        SArray<MbVector3D> tangents_r; // Векторы касательных
        tangents_r.resize( 10 );

        // Как к окружности (первая точка, касательная, вторая точка).
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nт = 2 Установка Кривизна по умолчанию в последней точке\n");
#endif

        polyLine_r[0] = polyLine[1];
        tangents_r[0] = tangents[1];
        polyLine_r[1] = polyLine[0];
        tangents_r[1] = tangents[0];
        TangentOnSecondPoint( polyLine_r, tangents_r, cvt );
        if ( outErrors > 0 ) { // Конфигурация плохая.
          return outErrors;
        }

        curvatures[1] = 0.8 * cvt; //curvatures[1] = cvt;

#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\n n = 3 new cvt [1]\n");
        //for (size_t ir = 0; ir < n; ++ir) {
        //
        //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f",
        //    ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
        //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f",
        //    ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
        //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f",
        //    ir, curvatures[ir]);
        //}
#endif

        polyLine_r.HardFlush();
        tangents_r.HardFlush();
      }

      // Значения кривизны
      // В первой точке

      int typeCurvature = 0;
      double firstCvt = curvatures[0];
      if ( GetInt( gCurvatureType1, typeCurvature ) ) {
        if ( typeCurvature < 0 )
          typeCurvature = 0;
        if ( typeCurvature > 3 )
          typeCurvature = 3;
        iBound = true;

        switch ( typeCurvature ) {
        case 0: // Координаты концевой точки вектора касательной
          if ( GetDouble( gCurvatureVal, firstCvt ) ) {
            iFirstCvt = true;
            curvatures[0] = firstCvt;
          }
          break;

        case 1: // Указать точку вектора кривизны

          if ( GetPoint( gCurvatureVec, point ) ) {
            MbVector3D firstD2r;
            firstD2r.Init( firstPoint, point );
            firstCvt = firstD2r.Length();
            curvatures[0] = firstCvt;
            iFirstCvt = true;
          }
        case 2: // Указать точку вектора радиуса

          if ( GetPoint( gRadiusVec, point ) ) {
            MbVector3D firstD2r;
            firstD2r.Init( firstPoint, point );
            if ( firstD2r.Length() > METRIC_ACCURACY )
              firstCvt = 1. / firstD2r.Length();
            else firstCvt = 0.;
            curvatures[0] = firstCvt;
            iFirstCvt = true;
          }
          break;

        case 3: // Указать объект
          // Укажите первую кривую 
          obj = nullptr;
          if ( GetGObj( gCurveMsg, st_Curve3D, &obj ) && obj->IsA() == st_SpaceInstance ) {
            MbNurbs3D * curve = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
            if ( curve != nullptr && ( curve->IsA() == st_Nurbs3D || curve->IsA() == st_Bezier3D ||
                                       curve->IsA() == st_Hermit3D ) ) {
              double t;
              curve->NearPointProjection( firstPoint, t, false );
              firstCvt = curve->Curvature( t );
#ifdef C3D_DEBUG_FAIR_CURVES
              //fprintf(prt, "\nFirstCvt = %f", firstCvt);
#endif
              curve->Tangent( t, firstTangent );
              MbVector3D f;

              curvatures[0] = firstCvt;
              iFirstCvt = true;
            }
            else { // Не выбрана кривая 
              outErrors = 2;
              return outErrors;
            }
          }
          else { // Кривая не NURBS
            outErrors = 2;
            return outErrors;
          }
          break;

        } //switch (typeTangent) 

        //outError = TangentsOnCirle(polyLine, tangents, &cvt);
      } // Задать кривизну в первой точке

      // Кривизна в последней точке
      double lastCvt = curvatures[n - 1];
      if ( GetInt( gCurvatureType2, typeCurvature ) ) {
        if ( typeCurvature < 0 )
          typeCurvature = 0;
        if ( typeCurvature > 3 )
          typeCurvature = 3;
        iBound = true;
        switch ( typeCurvature ) {
        case 0: // Координаты концевой точки вектора касательной
          if ( GetDouble( gCurvatureVal, lastCvt ) ) {
            curvatures[n - 1] = lastCvt;
            iLastCvt = true;
          }
          break;

        case 1: // Указать точку вектора кривизны
          if ( GetPoint( gCurvatureVec, point ) ) {
            MbVector3D LastD2r;
            LastD2r.Init( lastPoint, point );
            lastCvt = LastD2r.Length();
            iLastCvt = true;
            curvatures[n - 1] = lastCvt;
          }

        case 2: // Указать точку вектора радиуса
          if ( GetPoint( gRadiusVec, point ) ) {
            MbVector3D LastD2r;
            LastD2r.Init( lastPoint, point );
            if ( LastD2r.Length() > METRIC_ACCURACY )
              lastCvt = 1. / LastD2r.Length();
            else lastCvt = 0.;
            iLastCvt = true;
            curvatures[n - 1] = lastCvt;
            iBound = true;
          }
          break;

        case 3: // Указать объект
          // Укажите вторую кривую 
          if ( GetGObj( gCurveMsg, st_Curve3D, &obj ) && obj->IsA() == st_SpaceInstance ) {
            MbNurbs3D * curve = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
            if ( curve != nullptr && ( curve->IsA() == st_Nurbs3D || curve->IsA() == st_Bezier3D ||
                                       curve->IsA() == st_Hermit3D ) ) {
              double t;
              iBound = true;
              curve->NearPointProjection( lastPoint, t, false );
              lastCvt = curve->Curvature( t );
#ifdef C3D_DEBUG_FAIR_CURVES
              //fprintf(prt, "\nLastCvt = %f", lastCvt);
#endif
              curve->Tangent( t, lastTangent );

              iLastCvt = true;
              curvatures[n - 1] = lastCvt;
            }
            else { // Не выбрана кривая 
              outErrors = 2;
              return outErrors;
            }
          }
          else { // Кривая не NURBS
            outErrors = 2;
            return outErrors;
          }
          break;

        } //switch (typeTangent) 

      } // Задать кривизну в последней точке

#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nГО Эрмита после установки граничных параметров\n");
      //fprintf(prt, "\niFirstTangent = %d\n", iFirstTangent);
      //fprintf(prt, "\niLastTangent = %d\n", iLastTangent);
      //fprintf(prt, "\niFirstCvt = %d\n", iFirstCvt);
      //fprintf(prt, "\niLastCvt = %d\n", iLastCvt);

      //for (size_t ir = 0; ir < n; ++ir) {
      //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
      //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
      //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures[ir]);
      //}
#endif

      //DEBUG20220222
      // Не сплайн.
      if ( !iBound ) { // Граничные условия не задавались. Объект - Дуга оружности / Коника.
        if ( GetInt( gConstructType, typeCreation ) ) {
          if ( typeCreation < 0 )
            typeCreation = 0;
          if ( typeCreation > 1 )
            typeCreation = 1;
        }
      }
      else {
        typeCreation = 2;
      }  // Другая кривая
    } // Задаются граничные параметры
  } // !bSpline

  if ( typeCreation == 1 ) { // Не сплайн.
    double d = 0.5;
    GetDouble( gDiscrimMsg, d );
    // Построение коники. Средняя точка на медиане с дискриминантом
    // Сброс граничных параметров.
    iFirstTangent = false; //Касательная по умолчанию.
    iLastTangent = false;  //Касательная по умолчанию.
    iFirstCvt = false;     //Кривизна по умолчанию.
    iLastCvt = false;      //Кривизна по умолчанию.
    // Определяем соприкасающийся треугольник. 
    MbVector3D v01;
    v01.Init( polyLine[0], polyLine[2] );
    MbVector3D vMiddle = 0.5 * v01;
    double sMiddle = vMiddle.Length();
    vMiddle.Normalize();
    MbCartPoint3D pMiddle;
    pMiddle.Set( polyLine[0], vMiddle, sMiddle );
    MbVector3D vMedian;
    vMedian.Init( pMiddle, polyLine[1] );
    double sMedian = vMedian.Length();
    vMedian.Normalize();
    // sMedian/x = d; x = sMedian / d;
    MbCartPoint3D pVertex;
    pVertex.Set( pMiddle, vMedian, sMedian / d );
    tangents[0].Init( polyLine[0], pVertex );
    tangents[0].Normalize();
    tangents[1] = vMiddle;
    tangents[2].Init( pVertex, polyLine[2] );
    tangents[2].Normalize();
  }

  return 0;
}


//---------------------------------------------------------------------------------
// Построение односегментной кривой на опорной ломаной
// TODO: перенести код в библиотеку. Перенесено построение с параметрами по умолчанию (построение дуги).
// ---
int CreateOneSegmentOnBasePolyline( c3d::SpaceCurveSPtr & initPoly,
                                    bool bSpline, bool createWire )
{
  bool useDefaultParams = true;
  if ( useDefaultParams ) { // Просто построить дугу окружности.
    MbResultType res = rt_Success;
    MbeFairWarning warning( fwarn_Success );
    if ( createWire ) { // Создавать проволочный каркас.
      MbFairCreateData parameters;
      MbFairCurveMethod method( bSpline ? fairCrvType_BSpline : fairCrvType_Bezier, fairData_Polyline );
      MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
      MbFairCurveResult resCurve;
      res = ::FairCurve( MbFairCurveParams( *initPoly, snMaker ), method, parameters, resCurve );
      if ( IsSuccess( res ) ) { // Успешное завершение.
        c3d::SpaceCurveSPtr curve = resCurve.ExtractCurve();
        TestVariables::viewManager->AddObject( TestVariables::BSPLINE_Style, curve );
        PrintApproxErr( initPoly, curve, resCurve.GetApproxError(), res ); // Сообщение о точности.
      }
      warning = resCurve._warning;
    }
    else {
      MbCurve3D * resCurve = nullptr;
      res = bSpline ? ::CreateFairBSplineCurveOnBasePolyline( initPoly, fData, resCurve ) :
                      ::CreateFairBezierCurveOnBasePolyline( initPoly, fData, resCurve );
      if ( IsSuccess( fData.error ) ) { // Успешное завершение.
        TestVariables::viewManager->AddObject( TestVariables::BSPLINE_Style, resCurve );
        PrintApproxErr( initPoly, resCurve, fData.errInfo, fData.error ); // Сообщение о точности.
      }
      warning = fData.warning;
    }

    if ( !IsSuccess(res) ) {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    if ( warning > 0 ) { // Регистрация предупреждения.
      string_t message( IDS_TITLE_WRN );
      message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
    }
    outErrors = res;
    return outErrors;
  } // Просто построить дугу окружности.

  MbCurve3D * pllne = initPoly.get();

  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts );
  size_t n = cvrts.Count();

  double cvt( 0. );
  MbCartPoint3D point;

  bool iFirstTangent = false, iLastTangent = false; //Касательная по умолчанию.
  bool iFirstCvt = false, iLastCvt = false; //Кривизна по умолчанию.

  // Задание граничных условий. // Резервирование массивов.
  // Построение на трех точках
  SArray<MbCartPoint3D> polyLine;
  polyLine.resize( 10 );
  for ( size_t ir = 0; ir < n; ir++ ) {
    polyLine[ir] = cvrts[ir];
  }

  //DEBUG20211217
  //Для уплотненных точек
  SArray<MbCartPoint3D> polyLineRezult;
  polyLineRezult.resize( 10 );
  //DEBUG20211217 END
  SArray<MbVector3D> tangents; // Векторы касательных
  tangents.resize( 10 );
  SArray <double> curvatures; // Векторы касательных
  curvatures.resize( 10 );

  // Предварительная установка параметров
  if ( n == 2 ) {
    // Устанавливаем векторы по отрезку. Нулевые кривизны.
    MbVector3D v1;
    v1.Init( polyLine[0], polyLine[1] );
    v1.Normalize();
    tangents[0] = tangents[1] = v1;
    curvatures[0] = curvatures[1] = 0.;
  }
  else if ( n == 3 ) {
    // Устанавливаем векторы касательных и кривизны по дуге окружности
    outErrors = TangentsOnCirle( polyLine, tangents, cvt );
    curvatures[0] = curvatures[1] = curvatures[2] = cvt;

#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(prt, "\n TangentsOnCirle \n");
    //for (size_t ir = 0; ir < n; ++ir) {
    //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
    //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
    //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures[ir]);
    //}
#endif
  }

  int typeCreation = 0;

  // Ввод параметров. Весь ввод данных пользователя здесь.
  outErrors = GetOneSegmParams( polyLine, bSpline, typeCreation, iFirstTangent, iLastTangent, iFirstCvt, iLastCvt, tangents, curvatures );
  if ( outErrors > 0 )
    return outErrors;

  //DEBUG20222202
  bool closed = pllne->IsClosed();
  //DEBUG20222202

  if ( n == 3 ) {  // Построение по трем точкам
#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(prt, "\nПостроение по 3 точкам\n");
#endif

    //DEBUG20220222
    switch ( typeCreation ) {
      case 0: // Дуга окружности
        // Определить касательные векторы в точках ломаной
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nДуга окружности\n");
#endif
        //DEBUG20211217
        outErrors = TangentsOnCirleTest( polyLine, tangents, polyLineRezult, cvt );
        if ( outErrors > 0 ) { // Конфигурация плохая.
          return outErrors;
        }
        curvatures[0] = curvatures[1] = curvatures[2] = curvatures[3] = curvatures[4] = cvt;
        //DEBUG20211217 END

        // Формируем ГО Эрмита первого порядка фиксации.
        //DEBUG20220222 begin
        n = 5;
        if ( closed ) {
          curvatures[5] = curvatures[6] = cvt;
           n = 7;
        }
        //DEBUG20220222 end
        CreateHermiteGDOnStructure( polyLineRezult, tangents, curvatures, n, pllne );
        //DEBUG20211217
        break;

      case 1: // Conica
        if ( typeCreation == 1 ) { // Обмануть ошибку 'is scipped'
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\nДуга конической кривой\n");
#endif
        }
        // Формируем ГО Эрмита первого порядка фиксации.
        CreateHermiteGDOnStructure( polyLine, tangents, curvatures, n, pllne );
        break;

      case 2: // NURBzS
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nNURBzS кривая\n");
#endif
      // Проверяем конфигурацию. 
        MbVector3D v02;
        v02.Init( polyLine[0], polyLine[n - 1] );
        MbVector3D vOrt1, vOrt2;
        vOrt1 = tangents[0] | v02;
        vOrt2 = v02 | tangents[n - 1];

        if ( vOrt1.Length() < METRIC_ACCURACY || vOrt2.Length() < METRIC_ACCURACY ) {
#ifdef C3D_DEBUG_FAIR_CURVES
          //fprintf(prt, "\nНедопустимая конфигурация\n");
#endif
          outErrors = 2;
          return outErrors; // Недопустимая конфигурация
        }

        if ( (vOrt1 * vOrt2) > METRIC_ACCURACY ) {
          // Локально выпуклый участок.
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\nЛокально выпуклый участок\n");
#endif
        // Проверка положения средней точки.
          MbVector3D v01;
          v01.Init( polyLine[0], polyLine[1] );
          MbVector3D v12;
          v12.Init( polyLine[1], polyLine[2] );
          MbVector3D vOrt1_, vOrt2_;
          vOrt1_ = tangents[0] | v01;
          vOrt2_ = v12 | tangents[2];
          if ( vOrt1_.Length() < METRIC_ACCURACY || vOrt2_.Length() < METRIC_ACCURACY || (vOrt1_ * vOrt2_) < METRIC_ACCURACY ) {
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\nНедопустимая конфигурация\n");
#endif
            outErrors = 2;
            return outErrors; // Недопустимая конфигурация
          }

          // Выпуклая конфигурация. 
#ifdef C3D_DEBUG_FAIR_CURVES
          //fprintf(prt, "\nКорректная выпуклая конфигурация\n");
#endif
          // Задаем кривизны по умолчанию
          if ( iFirstTangent && !iFirstCvt ) {  // Задана касательная в первой точке.
            // Не задаем.
            // Устанавливаем параметры для построения на опорной ломаной.
          }

          if ( iLastTangent && !iLastCvt ) {
#ifdef C3D_DEBUG_FAIR_CURVES
            // fprintf(prt, "\nКривизна по умолчанию в последней точке\n");
#endif
          // Не задаем
          // Устанавливаем параметры для построения на опорной ломаной.
          }

          CreateHermiteGDOnStructure( polyLine, tangents, curvatures, n, pllne );
        } // if ((vOrt1 * vOrt2) > METRIC_ACCURACY) Выпуклый участок
        else if ( (vOrt1 * vOrt2) < 0. ) {
          // S-образный участок. Формируем участок кривой Безье программой Bezier2Curve.
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\nS-образный участок. Определяем точку перегиба.\n");
#endif
          PointVector3D  pv1, pv2;
          MbCartPoint3D pntFlex;
          MbVector3D dirFlex;
          pv1.first = polyLine[0];
          pv1.second = tangents[0];
          pv2.first = polyLine[n - 1];
          pv2.second = tangents[n - 1];
          outErrors = BezierCurveInflection( polyLine[0], tangents[0], polyLine[n - 1], tangents[n - 1],
                                             curvatures[0], curvatures[n - 1], pntFlex, dirFlex );
#ifdef C3D_DEBUG_FAIR_CURVES
          //fprintf(prt, "\nPost BezierCurveInflection\n");
#endif
          if ( outErrors > 0 ) {
            return outErrors; // Недопустимая конфигурация
          }

#ifdef C3D_DEBUG_FAIR_CURVES
          //fprintf(prt, "\npntFlex  = %f %f %f", pntFlex[0], pntFlex[1], pntFlex[2]);
          //fprintf(prt, "\ndirFlex  = %f %f %f", dirFlex[0], dirFlex[1], dirFlex[2]);
#endif
        // Перенос касательной в точке перегиба в среднюю точку. 
          tangents[1] = dirFlex;
          curvatures[1] = 0.;
          // Проверка конфигурации.
          MbVector3D v01;
          v01.Init( polyLine[0], polyLine[1] );
          MbVector3D v12;
          v12.Init( polyLine[1], polyLine[2] );
          MbVector3D vOrt1_, vOrt2_;
          vOrt1_ = tangents[0] | v01;
          vOrt2_ = v01 | tangents[1];
          if ( vOrt1_.Length() < METRIC_ACCURACY || vOrt2_.Length() < METRIC_ACCURACY || (vOrt1_ * vOrt2_) < METRIC_ACCURACY ) {
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\nНедопустимая конфигурация\n");
#endif
            outErrors = 2;
            return outErrors; // Недопустимая конфигурация
          }

          vOrt1_ = tangents[1] | v12;
          vOrt2_ = v12 | tangents[2];
          if ( vOrt1_.Length() < METRIC_ACCURACY || vOrt2_.Length() < METRIC_ACCURACY || (vOrt1_ * vOrt2_) < METRIC_ACCURACY ) {
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\nНедопустимая конфигурация\n");
#endif
            outErrors = 2;
            return outErrors; // Недопустимая конфигурация
          }

          // Конфигурация S-образная.
#ifdef C3D_DEBUG_FAIR_CURVES
          //fprintf(prt, "\nНормальная S-образная конфигурация\n");
#endif
          // Кривизны по умолчанию.
          if ( !iFirstCvt ) {
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\nНачальную кривизну по умолчанию\n");
#endif
            // Изменить кривизну в первой иочке по умолчанию. 
            // Скорректировать кривизну по умолчанию во последней точке.
            SArray<MbCartPoint3D> polyLine_r;
            polyLine_r.resize( 10 );
            SArray<MbVector3D> tangents_r; // Векторы касательных
            tangents_r.resize( 10 );

            // Как к окружности (первая точка, касательная, вторая точка).
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\nКривизна по умолчанию в первой точке\n");
#endif
            MbVector3D vOld = tangents[1];
            polyLine_r[0] = polyLine[0];
            tangents_r[0] = tangents[0];
            polyLine_r[1] = pntFlex;
            tangents_r[1] = dirFlex;
            TangentOnSecondPoint( polyLine, tangents, cvt );
            if ( outErrors > 0 ) { // Конфигурация плохая.
              return outErrors;
            }
            curvatures[0] = 0.8 * cvt; //curvatures[1] = cvt;
            curvatures[1] = 0.;
            vOld = tangents[1];
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\n n = 3 new cvt [0]\n");
            //for (size_t ir = 0; ir < n; ++ir) {
            //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
            //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
            //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures[ir]);
            //}
#endif
            polyLine_r.HardFlush();
            tangents_r.HardFlush();
          }

          if ( !iLastCvt ) {
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\nКонечную кривизну по умолчанию\n");
            //fprintf(prt, "\nКривизна по умолчанию в последней точке\n");
#endif
            // Скорректировать кривизну по умолчанию во последней точке.
            SArray<MbCartPoint3D> polyLine_r;
            polyLine_r.resize( 10 );
            SArray<MbVector3D> tangents_r; // Векторы касательных
            tangents_r.resize( 10 );

            polyLine_r[0] = polyLine[n - 1];
            tangents_r[0] = -tangents[n - 1];
            polyLine_r[1] = pntFlex;
            tangents_r[1] = -dirFlex;
            curvatures[1] = 0.;

#ifdef C3D_DEBUG_FAIR_CURVES
            fprintf(prt, "\n n = 3 Reversed params for TangentOnSecondPoint\n");
            for (size_t ir = 0; ir < 2; ++ir) {
              fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine_r[ir][0], polyLine_r[ir][1], polyLine_r[ir][2]);
              fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents_r[ir][0], tangents_r[ir][1], tangents_r[ir][2]);
            }
#endif

            TangentOnSecondPoint( polyLine_r, tangents_r, cvt );
            if ( outErrors > 0 ) { // Конфигурация плохая.
              return outErrors;
            }
            curvatures[n - 1] = 0.8 * cvt;

#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf(prt, "\n n = 3 New params post TangentOnSecondPoint cvt = %f\n", cvt);
            //for (size_t ir = 0; ir < n; ++ir) {
            //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
            //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
            //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures[ir]);
            //}
#endif

          // Сoздать на структуре ГО Эрмита
            CreateHermiteGDOnStructure( polyLine, tangents, curvatures, n, pllne );
            outErrors = fData.error;

            // Удалить рабочие массивы
            polyLine_r.HardFlush();
            tangents_r.HardFlush();
          }
        } // if ((vOrt1 * vOrt2) < -METRIC_ACCURACY) 
        break;
    } // switch (typeCreation)

    int iDraw = 0;
    if ( GetInt(gDrawMsg, iDraw) ) {
      if ( iDraw != 0 )
        iDraw = 1;
    }

    if ( iDraw == 1 ) { // Построение ГО Эрмита
      if ( outErrors == 0 )
        TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, pllne );
    }
    else if ( iDraw == 0 && outErrors == 0 ) { // Построение кривой.
      MbeFairCurvature accountCurvature = fairCur_No;
      bool switchEndCurvature = false;

      if ( typeCreation > 1 ) {
        accountCurvature = (MbeFairCurvature)3;
        switchEndCurvature = true;
      }
      else {
        accountCurvature = (MbeFairCurvature)0;
        switchEndCurvature = false;
      }

      MbResultType result = rt_Success;
      MbeFairWarning warnings = fwarn_Success;
      //DEBUG20220222 Begin
      if ( !bSpline ) { // Создание Безье.
        if ( createWire ) {
          MbFairCreateData parameters;
          parameters._subdivision = fairSubdiv_No;
          parameters._accountCurvature = accountCurvature;

          MbFairCurveMethod method( fairCrvType_Bezier, fairData_GDHPoly );
          MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
          MbFairCurveResult resCurve;

          result = ::FairCurve( MbFairCurveParams(*pllne, snMaker), method, parameters, resCurve );
          if ( IsSuccess(result) ) { // Успешное завершение.
            c3d::SpaceCurveSPtr curve = resCurve.ExtractCurve();
            // Нарисовать кривую.
            Style style = TestVariables::BSPLINE_Style;
            TestVariables::viewManager->AddObject( style, curve );
            PrintApproxErr( initPoly, curve, resCurve.GetApproxError(), result ); // Сообщение о точности.
          }
        }
        else {
          fData.accountCurvature = accountCurvature;
          fData.switchEndCurvature = switchEndCurvature;
          fData.subdivision = fairSubdiv_No;

          MbCurve3D * resCurve = nullptr;
          result = ::CreateFairBezierCurveOnBasePolylineOfHermiteGD( pllne, fData, resCurve );
          warnings = fData.warning;
          if ( IsSuccess(result) ) { // Успешное завершение.
            // Нарисовать кривую.
            Style style = TestVariables::BSPLINE_Style;
            TestVariables::viewManager->AddObject( style, resCurve );
            PrintApproxErr( initPoly, resCurve, fData.errInfo, fData.error ); // Сообщение о точности.
          }
        }
      }
      else {  // Создание B-сплайн.
        if ( createWire ) {
          MbFairCreateData parameters;
          parameters._subdivision = fairSubdiv_Single;
          parameters._accountCurvature = accountCurvature;
          parameters._degree = 8;

          MbFairCurveMethod method( fairCrvType_BSpline, fairData_GDHPoly );
          MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
          MbFairCurveResult resCurve;

          result = ::FairCurve( MbFairCurveParams(*pllne, snMaker), method, parameters, resCurve );
          if ( IsSuccess(result) ) { // Успешное завершение.
            c3d::SpaceCurveSPtr curve = resCurve.ExtractCurve();
            // Нарисовать кривую.
            Style style = TestVariables::BSPLINE_Style;
            TestVariables::viewManager->AddObject( style, curve );
            PrintApproxErr( initPoly, curve, resCurve.GetApproxError(), result ); // Сообщение о точности.
          }
        }
        else {
          fData.accountCurvature = accountCurvature;
          fData.switchEndCurvature = switchEndCurvature;
          fData.subdivision = fairSubdiv_Single; // fairSubdiv_No;
          fData.degreeBSpline = 8;

          MbCurve3D * resCurve = nullptr;
          result =  ::CreateFairBSplineCurveOnBasePolylineOfHermiteGD( pllne, fData, resCurve );
          warnings = fData.warning;
          if ( IsSuccess(fData.error) ) { // Успешное завершение.
            // Нарисовать кривую.
            Style style = TestVariables::BSPLINE_Style;
            TestVariables::viewManager->AddObject( style, resCurve );
            PrintApproxErr( initPoly, resCurve, fData.errInfo, fData.error ); // Сообщение о точности.
          }
        }
      }
      if ( pllne != initPoly.get() )
        DeleteItem( pllne );

      //DEBUG20220222 End

      outErrors = result;
      if( !IsSuccess(result) ) {
        string_t message( IDS_TITLE_ERR );
        message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
        MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
      }
      if ( warnings > 0 ) { // Регистрация предупреждения.
        string_t message( IDS_TITLE_WRN );
        message = MessageWarning( fData.warning ); // Формирует текст сообщения по номеру ошибки.
        MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
      }
    } // Нарисовать кривую.

    polyLine.HardFlush();
    polyLineRezult.HardFlush();
    tangents.HardFlush();
    curvatures.HardFlush();
  } //if (n == 3) {  // Построение на трех точках
  else if ( n == 2 ) {  // Построение на двух точках
#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(prt, "\nПостроение по 2 точкам\n");
#endif
    // Проверяем конфигурацию. 
    MbVector3D v01;
    v01.Init( polyLine[0], polyLine[1] );
    MbVector3D vOrt1, vOrt2;
    vOrt1 = tangents[0] | v01;
    vOrt2 = v01 | tangents[1];

    if ( vOrt1.Length() < METRIC_ACCURACY || vOrt2.Length() < METRIC_ACCURACY ) {
      outErrors = 2;
      return outErrors; // Плохая конфигурация
    }
    if ( (vOrt1 * vOrt2) > METRIC_ACCURACY ) {
        // Локально выпуклый участок.
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\nЛокально выпуклый участок\n");
#endif
      if ( iFirstTangent && !iFirstCvt ) {  // Уточнить кривизну в первой точке.
        // Как к окружности.
        TangentOnSecondPoint( polyLine, tangents, cvt );
        if ( outErrors > 0 ) { // Конфигурация плохая.
          return outErrors;
        }

        curvatures[0] = cvt; curvatures[1] = cvt;
#ifdef C3D_DEBUG_FAIR_CURVES
        // fprintf(prt, "\n n = 2 New TangentOnSecondPoint\n");
        // for (size_t ir = 0; ir < n; ++ir) {
        //   fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
        //   fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
        //   fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures[ir]);
        // }
#endif
      } // Уточнить кривизну в первой точке.

      if ( iLastTangent && !iLastCvt ) {  // Уточнить кривизну в последней точке
        // Скорректировать кривизну по умолчанию во второй точке.
        SArray<MbCartPoint3D> polyLine_r;
        polyLine_r.resize( 10 );
        SArray<MbVector3D> tangents_r; // Векторы касательных
        tangents_r.resize( 10 );

        for ( size_t ir = 0; ir < 2; ir++ ) {
          polyLine_r[ir] = polyLine[n - 1 - ir];
          tangents_r[ir] = -tangents[n - 1 - ir];
        }

#ifdef C3D_DEBUG_FAIR_CURVES
        SArray <double> curvatures_r; // Векторы касательных
        curvatures_r.resize( 10 );
        for ( size_t ir = 0; ir < 2; ir++ ) {
          curvatures_r[ir] = curvatures[n - 1 - ir];
        }
        //fprintf(prt, "\n n = 2  Reversed params\n");
       // for (size_t ir = 0; ir < n; ++ir) {
       //   fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine_r[ir][0], polyLine_r[ir][1], polyLine_r[ir][2]);
       //   fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents_r[ir][0], tangents_r[ir][1], tangents_r[ir][2]);
       //   fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures_r[ir]);
       // }
#endif

        TangentOnSecondPoint( polyLine_r, tangents_r, cvt );
        if ( outErrors > 0 ) { // Конфигурация плохая.
          return outErrors;
        }

        curvatures[1] = cvt;

#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\n n = 2 TangentOnSecondPoint cvt = %f\n", cvt);
        //for (size_t ir = 0; ir < n; ++ir) {
        //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
        //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
        //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures[ir]);
        //}
#endif

        polyLine_r.HardFlush();
        tangents_r.HardFlush();
      } // Уточнить кривизну в последней точке

      CreateHermiteGDOnStructure( polyLine, tangents, curvatures, n, pllne );
      outErrors = fData.error;

    } // Выпуклый участок

    if ( (vOrt1 * vOrt2) < -METRIC_ACCURACY ) {
      // S-образный участок. Формируем участок кривой Безье программой Bezier2Curve.
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nS-образный участок\n");
#endif     

      PointVector3D  pv1, pv2;
      MbCartPoint3D pntFlex;
      MbVector3D dirFlex;
      pv1.first = polyLine[0];
      pv1.second = tangents[0];
      pv2.first = polyLine[n - 1];
      pv2.second = tangents[n - 1];

      outErrors = BezierCurveInflection( polyLine[0], tangents[0], polyLine[n - 1], tangents[n - 1],
                                         curvatures[0], curvatures[n - 1], pntFlex, dirFlex );
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nPost BezierCurveInflection\n");
#endif     
      if ( outErrors > 0 ) {
        return outErrors; // Недопустимая конфигурация
      }

#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\npntFlex  = %f %f %f", pntFlex[0], pntFlex[1], pntFlex[2]);
      //fprintf(prt, "\ndirFlex  = %f %f %f", dirFlex[0], dirFlex[1], dirFlex[2]);
#endif     

      if ( !iFirstCvt ) {
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\nНачальную кривизну по умолчанию\n");
#endif     
        // Изменить кривизну в первой иочке по умолчанию. 
        // Скорректировать кривизну по умолчанию во последней точке.
        SArray<MbCartPoint3D> polyLine_r;
        polyLine_r.resize( 10 );
        SArray<MbVector3D> tangents_r; // Векторы касательных
        tangents_r.resize( 10 );

        // Как к окружности (первая точка, касательная, вторая точка).
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\nКривизна по умолчанию в первой точке\n");
#endif     
        MbVector3D vOld = tangents[1];
        polyLine_r[0] = polyLine[0];
        tangents_r[0] = tangents[0];
        polyLine_r[1] = pntFlex;
        tangents_r[1] = dirFlex;
        TangentOnSecondPoint( polyLine_r, tangents_r, cvt );
        if ( outErrors > 0 ) { // Конфигурация плохая.
          return outErrors;
        }

        curvatures[0] = 0.8 * cvt; //curvatures[1] = cvt;
        vOld = tangents[1];
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\n n = 3 new cvt [0] = \n", cvt);
        //for (size_t ir = 0; ir < n; ++ir) {
        //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
        //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
        //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f",ir, curvatures[ir]);
        //}
#endif
        polyLine_r.HardFlush();
        tangents_r.HardFlush();
      }
      if ( !iLastCvt ) {
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\nКонечную кривизну по умолчанию\n");
        //fprintf(prt, "\nКривизна по умолчанию в последней точке\n");
#endif
        // Скорректировать кривизну по умолчанию во последней точке.
        SArray<MbCartPoint3D> polyLine_r;
        polyLine_r.resize( 10 );
        SArray<MbVector3D> tangents_r; // Векторы касательных
        tangents_r.resize( 10 );

        polyLine_r[0] = polyLine[n - 1];
        tangents_r[0] = -tangents[n - 1];
        polyLine_r[1] = pntFlex;
        tangents_r[1] = -dirFlex;

#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\n n = 3 Reversed params for TangentOnSecondPoint\n");
        //for (size_t ir = 0; ir < 2; ++ir) {
        //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine_r[ir][0], polyLine_r[ir][1], polyLine_r[ir][2]);
        //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
        //}
#endif
        TangentOnSecondPoint( polyLine_r, tangents_r, cvt );
        if ( outErrors > 0 ) { // Конфигурация плохая.
          return outErrors;
        }

        curvatures[n - 1] = 0.8 * cvt;

#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(prt, "\n n = 3 New params post TangentOnSecondPoint cvt = %f\n", cvt);
        //for (size_t ir = 0; ir < n; ++ir) {
        //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f", ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
        //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f", ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
        //  fprintf(prt, "\ncurvatures ir = %d Cvt = %f", ir, curvatures[ir]);
        //}
#endif

        // Сoздать на структуре ГО Эрмита
        n = 3;
        polyLine[n - 1] = polyLine[1];
        tangents[n - 1] = tangents[1];
        curvatures[n - 1] = curvatures[1];
        polyLine[1] = pntFlex;
        tangents[1] = dirFlex;
        curvatures[1] = 0.;

        polyLine_r.HardFlush();
        tangents_r.HardFlush();
      }  //S-образный участок

      CreateHermiteGDOnStructure( polyLine, tangents, curvatures, n, pllne );
      outErrors = fData.error;
    } // S-образный участок

    if ( outErrors == 0 ) { // Нарисовать
      int iDraw = 0;
      if ( GetInt(gDrawMsg, iDraw) ) {
        if ( iDraw != 0 )
          iDraw = 1;
      }

      if ( iDraw == 1 ) { // Построение ГО Эрмита
        TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, pllne );
      } // Нарисовать ГО Эрмита
      else {  // Построение кривой.
        MbResultType result = rt_Success;
        MbeFairWarning warnings = fwarn_Success;
        if ( createWire ) {
          MbFairCreateData parameters;
          parameters._subdivision = fairSubdiv_No;
          parameters._arrange = false;

          MbFairCurveMethod method( fairCrvType_Bezier, fairData_GDHPoly );
          MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
          MbFairCurveResult resCurve;

          result = ::FairCurve( MbFairCurveParams( *pllne, snMaker ), method, parameters, resCurve );
          if ( IsSuccess(result) ) { // Успешное завершение.
            c3d::SpaceCurveSPtr curve = resCurve.ExtractCurve();
            // Нарисовать кривую.
            Style style = TestVariables::BSPLINE_Style;
            TestVariables::viewManager->AddObject( style, curve );
            PrintApproxErr( initPoly, curve, resCurve.GetApproxError(), result ); // Сообщение о точности.
          }
        }
        else {
          fData.subdivision = fairSubdiv_No;
          fData.arrange = false;

          MbCurve3D * resCurve = nullptr;
          result =  ::CreateBezierCurveOnHermiteGD( pllne, fData, resCurve );
          warnings = fData.warning;
          if ( IsSuccess(fData.error) ) { // Успешное завершение.
            // Нарисовать кривую.
            Style style = TestVariables::BSPLINE_Style;
            TestVariables::viewManager->AddObject( style, resCurve );
            PrintApproxErr( initPoly, resCurve, fData.errInfo, fData.error ); // Сообщение о точности.
          }
        }
        outErrors = result;
      } // Нарисовать кривую

    } // Нарисовать
    polyLine.HardFlush();
    tangents.HardFlush();
    curvatures.HardFlush();
  } //if (n == 2) 
 
  return outErrors;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBezierCurveOnTangentPolyline();
// 
int Test_CreateFairBezierCurveOnTangentPolyline( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();

  // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
  MbeFairSmoothing fairing = (MbeFairSmoothing)GetIntValue( 1, 0, 1, gFairingMsg1 );
  MbeFairSubdivision subdivision = (MbeFairSubdivision)0;
  MbeFairVector accountInflexVector = (MbeFairVector)0;
  bool arrange = false;
  if ( fairing > 0 ) {
    subdivision = (MbeFairSubdivision)GetIntValue( subdivision, 0, 2, gSubdivisionMsg );
    accountInflexVector = (MbeFairVector)GetIntValue( accountInflexVector, 0, 2, gInflexMsg );
  }

  MbeFairWarning warning( fwarn_Success );

  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._fairing = fairing;
    parameters._subdivision = subdivision;
    parameters._accountVectorInflect = accountInflexVector;
    parameters._arrange = arrange;

    MbFairCurveMethod method( fairCrvType_Bezier, fairData_Tangent );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams(*polyPtr, snMaker), method, parameters, resCurve );

    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    //fData.prt = prt;
    fData.fairing = fairing;
    fData.subdivision = subdivision;
    fData.accountInflexVector = accountInflexVector;
    fData.arrange = arrange;
    
    MbCurve3D * resCurve = nullptr;
    ::CreateFairBezierCurveOnTangentPolyline( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateHermiteGDOnBezierCurve()
// Создать ГО на NURBzS кривой (ломаную, касательную ломаную, ГО Эрмита.
// ---
int Test_CreateHermiteGDOnBezierCurve( bool createWire )
{
  int outError = 0;
  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbeFairWarning warning( fwarn_Success );

      // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
      int typeGD = GetIntValue( 2, 0, 2, gTypeGDMsg );
      MbeFairCurveType curveType = fairCrvType_HermiteGD; // typeGD = 2
      if ( typeGD == 1 )
        curveType = fairCrvType_PolyTangent;
      else if ( typeGD == 0 )
        curveType = fairCrvType_Poly;

      if ( createWire ) {
        MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
        MbFairCurveMethod method( curveType );
        MbFairCurveResult resCurve;

        MbResultType res = ::FairCurve( MbFairCurveParams(*curvePtr, snMaker), method, MbFairCreateData(),
                                        resCurve );
        if ( IsSuccess(res) ) {
          TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, resCurve.GetCurve() );
        }
        else {
          string_t message( IDS_TITLE_ERR );
          message = MessageError( res ); // Формирует текст сообщения по номеру ошибки.
          MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
        }
        outError = res;
        warning = resCurve._warning;
      }
      else {
        MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
        MbCurve3D * pllne = nullptr;
        KernelCreateHermiteGDOnBezierCurve( curve, curveType, fData, pllne );
        if ( IsSuccess(fData.error) ) { // Успешное завершение.
          // Нарисовать кривую.
          TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, pllne );
        }
        else { // Регистрация ошибки.
          string_t message( IDS_TITLE_ERR );
          message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
          MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
        }
        outError = fData.error;
        warning = fData.warning;
      }
      if ( warning > 0 ) { // Регистрация предупреждения.
        string_t message( IDS_TITLE_WRN );
        message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
        MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
      }
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }
  return outError;
}


#ifdef FAIR_CURVE_ADVANCED
//---------------------------------------------------------------------------------
// Создать графики кривизны или эволюты на кривой
// ---
int Test_CreateEvoluteCurvatureOnCurve(  )
{
  int outError = 0;
  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbeFairWarning warning( fwarn_Success );

      // Построение графиков кривизны или эволюты в виде полилиний.
      int typeGraph = 0;
      typeGraph = GetIntValue( 0, 0, 1, gTypeGraphMsg ); // "Отобразить: график кривизны = 0, эволюты = 1"
      float graphScale = (float)-0.1;
      int typeDrawCvt = 0;
      if ( typeGraph == 0 ) {
        graphScale = GetFloatValue( (float)-0.1, (float)-1.0, (float)-0.001, gGraphScaleMsg ); // "Значение масштаба отображения  ( cvtMax / lengthCurve):"
        typeDrawCvt = GetIntValue( 0, 0, 1, gTypeDrawCvtMsg ); // "Вид отображения кривизны: над кривой = 0, вида F(x) = 1"
      }
      int numSegms = 10;
      double hStep = 0.1;
      numSegms = GetIntValue( 10, 1, 10, gGraphNumSegmsMsg ); // "Количество сегментов на дуге кривой:"
      hStep = 1. / numSegms;
      int typeDraw = 0;
      typeDraw = GetIntValue( 0, 0, 1, gTypeDrawMsg ); // "Вид отображения: линия = 0, "ежик" = 1"
      
      {
        MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
        MbCurve3D * pllneHGD = nullptr;
        fData.tParam = hStep; // Шаг интерполяции.
        InterpolateNurbs( curve, fData, pllneHGD );
        MbControlData3D cvrts;
        pllneHGD->GetBasisPoints( cvrts );
        size_t n = cvrts.Count();
        size_t nCurve = n / 5;
        size_t nGraph = n / 5;
        if ( typeDraw == 1 )
          nGraph = nCurve * 3; // Тройной обход точки
        bool closed = false;
        //bool closed = pllneHGD->IsClosed();
        if ( IsSuccess( fData.error ) ) { // Успешное завершение.
          // Нарисовать кривую.
          SArray<MbCartPoint3D>  rezCurvePnts; // Точки кривой.
          rezCurvePnts.resize( nCurve + 10);
          SArray<MbCartPoint3D>  rezGraphPnts;  //  Точки графика.
          rezGraphPnts.resize( nGraph + 10);
          //TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, pllneHGD );
          // Нарисовать графики кривизны или эволюты.
          DrawGraphs( pllneHGD,   // "Полилиния интерполированных точек а аиде ГО Эрмита"
                      typeGraph,  // "Отобразить: график кривизны = 0, эволюты = 1"            
                      graphScale, // "Значение масштаба отображения  ( cvtMax / lengthCurve):"
                      typeDrawCvt,// "Вид отображения кривизны: над кривой = 0, вида F(x) = 1"
                      typeDraw,   // "Вид отображения: линия = 0, "ежик" = 1"
                      rezCurvePnts, // Точки кривой
                      rezGraphPnts // Точки графика
          );
#ifdef C3D_DEBUG_FAIR_CURVES
          //fprintf( prt, "\nrezGraphPnts n = %d\n", n );
          //for ( size_t ir = 0; ir < nGraph; ++ir ) {
          //  fprintf( prt, "ir = %d  rezGraphPnts = %f %f %f\n", ir, rezGraphPnts[ir][0], rezGraphPnts[ir][1], rezGraphPnts[ir][2] );
          //}
#endif

          MbCartPoint3D pnt = rezCurvePnts[0];  //DEBUG only
          MbCurve3D * pllneOnCurve = nullptr;
          MbCurve3D * pllneOnGraph = nullptr;
          SArray<MbCartPoint3D>  drawCrv;
          SArray<MbCartPoint3D>  drawPnts;
          for ( size_t ir = 0; ir < nCurve; ++ir ) {
            drawCrv.Add( rezGraphPnts[ir] );
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf( prt, "ir = %d  rezGraphPnts = %f %f %f\n", ir, rezGraphPnts[ir][0], rezGraphPnts[ir][1], rezGraphPnts[ir][2] );
#endif
          }
          for ( size_t ir = 0; ir < nGraph; ++ir ) {
            drawPnts.Add( rezGraphPnts[ir] );
#ifdef C3D_DEBUG_FAIR_CURVES
            //fprintf( prt, "ir = %d  rezGraphPnts = %f %f %f\n", ir, rezGraphPnts[ir][0], rezGraphPnts[ir][1], rezGraphPnts[ir][2] );
#endif
          }

          outErrors = ::SplineCurve( drawCrv, closed, st_Polyline3D, pllneOnCurve );
          TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, pllneOnCurve );
          //outErrors = ::SplineCurve( rezGraphPnts, closed, st_Polyline3D, pllneOnGraph );
          outErrors = ::SplineCurve( drawPnts, closed, st_Polyline3D, pllneOnGraph );
          TestVariables::viewManager->AddObject( TestVariables::WIRE_Style, pllneOnGraph );
          drawPnts.clear();
          drawCrv.clear();
          rezGraphPnts.Flush();
          rezCurvePnts.Flush();
        }
        else { // Регистрация ошибки.
          string_t message( IDS_TITLE_ERR );
          message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
          MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
        }
        outError = fData.error;
        warning = fData.warning;
      }
      if ( warning > 0 ) { // Регистрация предупреждения.
        string_t message( IDS_TITLE_WRN );
        message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
        MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
      }
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }
  return outError;
}
#endif


//---------------------------------------------------------------------------------
// Test_CreateBezierCurveOnHermiteGD
// ---
int Test_CreateBezierCurveOnHermiteGD( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();

  // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
  MbeFairSubdivision subdivision = fairSubdiv_No; // = 0
  bool arrange = false;

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._subdivision = subdivision;
    parameters._arrange = arrange;

    MbFairCurveMethod method( fairCrvType_Bezier, fairData_GDHermite );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType result = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(result) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( result );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = result;
    warning = resCurve._warning;
  }
  else {
    //fData.prt = prt;
    fData.subdivision = subdivision;
    fData.arrange = arrange;

    MbCurve3D * resCurve = nullptr;
    ::CreateBezierCurveOnHermiteGD( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateBSplineCurveOnHermiteGD()
// ---
int Test_CreateBSplineCurveOnHermiteGD( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();

  // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
  size_t degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg );

  // Установить учет кривизны при построении B-сплайновой кривой.
  MbeFairCurvature accountCurvature = fairCur_No;
  int select = 0;
  if ( GetInt( _T( "Учитывать кривизну в начальной точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
  }
  accountCurvature = (MbeFairCurvature)select;
  select = 0;
  if ( GetInt( _T( "Учитывать кривизну в последней точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
    if ( select == 1 ) {
      select = 2;
    }
  }
  accountCurvature = (MbeFairCurvature)( accountCurvature + select );
  MbeFairSubdivision subdivision = (MbeFairSubdivision)0;
  bool arrange = false;

  //==
  select = 0;
  double realAccuracyBSpl = METRIC_ACCURACY * 0.01;
  size_t numberOfIterationsBSpl = 1000;
  if ( GetInt(_T("Изменить параметры точности: 0 - не изменять, 1- изменить"), select) ) {
    if ( select > 0 ) {
      //Точность построения
      double delta = realAccuracyBSpl;
      if ( GetDouble(_T("Точность построения: "), delta) ) {
        if ( delta < 0. )
          delta = 0.;
      }
     
      realAccuracyBSpl = delta;

      int iterations = (int)numberOfIterationsBSpl;
      if ( GetInt(_T("Ограничение количества итераций при подгонке В-сплайном:"), iterations) ) {
        if ( iterations < 0 )
          iterations = 2;
      }
      numberOfIterationsBSpl = iterations;
    }
  }
  //==

  // Фиксация касательных.
  int No = (int)degreeBSpline / 2 + 1;
  IndexVectorPtrArray fixConstraints;
  size_t n_fix = 0;
  size_t np = n / 5; // Количество точек опорной ломаной.
  for ( size_t ir = 0; np; ir++ ) {
    if ( GetInt( _T( "Укажите по возрастанию номер точки для фиксации касательной / точки " ), No ) == false )
      break;
    fixConstraints.push_back( std::pair<size_t, MbVector3D *>( (size_t)No, nullptr ) );
    n_fix++;
    No = No + (int)degreeBSpline / 2 + 2;
    if ( No > (int)(np - (degreeBSpline / 2 + 2)) )
      break;
  }
  MbeFixPntTng fixPntTng = fixPntTng_NotFix;
  if ( n_fix > 0 )
    fixPntTng = fixPntTng_Fix;
  else
    fixPntTng = fixPntTng_NotFix;

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._degree = degreeBSpline;
    parameters._accountCurvature = accountCurvature;
    parameters._subdivision = subdivision;
    parameters._arrange = arrange;
    parameters.SetPrecision( realAccuracyBSpl );
    parameters._iterationsNumber = numberOfIterationsBSpl;
    parameters.InitFixConstraints( fixConstraints );

    MbFairCurveMethod method( fairCrvType_BSpline, fairData_GDHermite );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    //fData.prt = prt;
    fData.degreeBSpline = degreeBSpline;
    fData.accountCurvature = accountCurvature;
    fData.subdivision = subdivision;
    fData.arrange = arrange;
    fData.realAccuracyBSpl = realAccuracyBSpl;
    fData.numberOfIterationsBSpl = numberOfIterationsBSpl;
    fData.fixPntTng = fixPntTng;
    fData.InitFixConstraints( fixConstraints );

    MbCurve3D * resCurve = nullptr;
    ::CreateBSplineCurveOnHermiteGD( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }

#ifdef C3D_DEBUG_FAIR_CURVES
    if ( fData.error == 0 ) { // Печать отчета.
      fprintf( prt, "\n Фактическая точность аппроксимации = %15.12f", fData.realAccuracyBSpl );
      fprintf( prt, "\n Количество итераций = %zd", fData.numberOfIterationsBSpl );
    }
#endif
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBezierCurveOnBasePolylineOfHermiteGD()
// ---
int Test_CreateFairBezierCurveOnBasePolylineOfHermiteGD( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();
  if ( !IsHermit(pllne) ) {
    outErrors = rt_ObjectNotHermiteGD; // Объект не ГО Эрмита. rt_Success) 
    string_t message(IDS_TITLE_ERR);
    message = MessageError(outErrors); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx(message.c_str(), IDS_TITLE_ERR); // Выводит сообщение.
    return outErrors;
  }

  int select = 0;
  if ( GetInt(_T("Учитывать кривизну в начальной точке: 0 - не учитывать, 1- учитывать"), select) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
  }
  MbeFairCurvature accountCurvature = (MbeFairCurvature)select;
  select = 0;
  if ( GetInt(_T("Учитывать кривизну в последней точке: 0 - не учитывать, 1- учитывать"), select) ) {
    if (select < 0)
      select = 0;
    if (select > 1)
      select = 1;
    if (select == 1) {
      select = 2;
    }
  }

  // Уплотнение и гармонизация
  MbeFairSubdivision subdivision = fairSubdiv_No;
  bool arrange = false;

  MbeFixPntTng fixPntTng = fixPntTng_NotFix;
  IndexVectorPtrArray fixConstraints;
  // Сглаживание зашумленных точек
  MbeFairSmoothing fairing = fairSmooth_Yes;
  int smoothing = GetIntValue( 0, 0, 1, gFairingMsg4 );
  double smoothClearance = 0.002;
  int clearanceNoisyIteration = 200;
  if  ( smoothing > 0 ) {
    fairing = fairSmooth_Noisy;
    if ( GetDouble(_T("Коридор сглаживания: "), smoothClearance) ) {
      if ( smoothClearance < 0. )
        smoothClearance = 0.;
    }
    clearanceNoisyIteration = GetIntValue( 200, 2, 1000, _T("Количество итераций сглаживания: ") );
  }
  else { // if ( smoothing == 0 )
    accountCurvature = (MbeFairCurvature)( accountCurvature + select );

    // Фиксация касательных.
    int num( 0 );
    for ( int ir = 2; n - 2; ir = ir + 2 ) {
      if ( GetInt(_T( "Укажите по возрастанию номер точки для фиксации касательной / точки "), num) == false )
        break;
      fixConstraints.push_back( std::pair<size_t, MbVector3D *>((size_t)num, nullptr ) );
      num = num + 2;
    }

    if ( num > 2 )
      fixPntTng = fixPntTng_Fix;
    else
      fixPntTng = fixPntTng_NotFix;

    if ( num == 2 ) { // Можно только без фиксации касательных.
    // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
      subdivision = (MbeFairSubdivision)GetIntValue( subdivision, 0, 2, gSubdivisionMsg );
      arrange = GetIntValue( 0, 0, 1, gArrangeMsg ) > 0;
    }
  }

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._accountCurvature = accountCurvature;
    parameters.InitFixConstraints( fixConstraints );
    parameters._fixPointTangent = fixPntTng;
    parameters._subdivision = subdivision;
    parameters._arrange = arrange;
    parameters._fairing = fairing;
    parameters._noisyDeviation = smoothClearance;
    parameters._noisyIterations = (size_t)clearanceNoisyIteration;

    MbFairCurveMethod method( fairCrvType_Bezier, fairData_GDHPoly );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.accountCurvature = accountCurvature;
    fData.fixPntTng = fixPntTng;
    fData.subdivision = subdivision;
    fData.arrange = arrange;
    fData.clearanceNoisyIteration = clearanceNoisyIteration;
    fData.clearanceNoisy = smoothClearance;
    fData.fairing = fairing;

    fData.InitFixConstraints( fixConstraints );
    MbCurve3D * resCurve = nullptr;
    ::CreateFairBezierCurveOnBasePolylineOfHermiteGD( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBezierCurveOnTangentsOfHermiteGD();
// ---
int Test_CreateFairBezierCurveOnTangentsOfHermiteGD( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();
  if ( !IsHermit(pllne) ) {
    outErrors = rt_ObjectNotHermiteGD; // Объект не ГО Эрмита. rt_Success) 
    string_t message(IDS_TITLE_ERR);
    message = MessageError(outErrors); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx(message.c_str(), IDS_TITLE_ERR); // Выводит сообщение.
    return outErrors;
  }

  // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
  int select = 0;
  if ( GetInt( _T( "Учитывать кривизну в начальной точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
  }
  MbeFairCurvature accountCurvature = (MbeFairCurvature)select;

  select = 0;
  if ( GetInt( _T( "Учитывать кривизну в последней точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
    if ( select == 1 ) {
      select = 2;
    }
  }
  accountCurvature = (MbeFairCurvature)( accountCurvature + select );
  bool arrange = false;

  // Фиксация касательных.
  int No = 2;
  IndexVectorPtrArray fixConstraints;
  for ( int ir = 2; n - 2; ir = ir + 2 ) {
    if ( GetInt( _T( "Укажите по возрастанию номер точки для фиксации касательной / точки " ), No ) == false )
      break;
    fixConstraints.push_back( std::pair<size_t, MbVector3D *>( (size_t)No, nullptr ) );
    No = No + 2;
  }
  MbeFixPntTng fixPntTng = ( No > 2 ) ? fixPntTng_Fix : fixPntTng_NotFix;

  // Уплотнение 
  MbeFairSubdivision subdivision = (MbeFairSubdivision)0;

  if ( No == 2 ) { // Можно только без фиксации касательных.
    // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
    subdivision = (MbeFairSubdivision)GetIntValue( subdivision, 0, 2, gSubdivisionMsg );
  }

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._accountCurvature = accountCurvature;
    parameters.InitFixConstraints( fixConstraints );
    parameters._fixPointTangent = fixPntTng;
    parameters._subdivision = subdivision;
    parameters._arrange = arrange;

    MbFairCurveMethod method( fairCrvType_Bezier, fairData_GDHTangent );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.accountCurvature = accountCurvature;
    fData.arrange = arrange;
    fData.fixPntTng = fixPntTng;
    fData.subdivision = subdivision;

    fData.InitFixConstraints( fixConstraints );

    MbCurve3D * resCurve = nullptr;
    ::CreateFairBezierCurveOnTangentsOfHermiteGD( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = fData.error;
    warning = fData.warning;
  }

    if ( warning > 0 ) { // Регистрация предупреждения.
      string_t message( IDS_TITLE_WRN );
      message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
    }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBSplineCurveOnBasePolylineOfHermiteGD()
// ---
int Test_CreateFairBSplineCurveOnBasePolylineOfHermiteGD( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();
  if ( !IsHermit(pllne) ) {
    outErrors = rt_ObjectNotHermiteGD; // Объект не ГО Эрмита. rt_Success) 
    string_t message( IDS_TITLE_ERR );
    message = MessageError( outErrors ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    return outErrors;
  }

  // Сглаживание зашумленных точек
  MbeFairSmoothing fairing = fairSmooth_Yes;
  int smoothing = GetIntValue( 1, 0, 2, gFairingMsg2 );
  
  double smoothClearance = 0.;
  int clearanceNoisyIteration = 0;
  int outSegments = 0;
  double scaleLength = 4.;

  outSegments = 10;
  //if ( smoothing > 2 ) {
  //  GetInt( _T( "Укажите количество сегментов (не репарам < 6 или репарам >= 6):" ), outSegments );
  //  if ( outSegments < 6 )
  //    outSegments = 0; // Без репараметризации
  //  if ( outSegments > 0 ) {
  //    GetDouble( _T( "Укажите масштаб влияния длины (> 1.0):" ), scaleLength );
  //    if ( scaleLength < 1. )
  //      scaleLength = 1.;
  //  }
  //}
  //if ( smoothing >= 3 ) {
  //  bool data = false;
  //  while (data == false) {
  //    if (GetDouble(_T("Коридор сглаживания: "), smoothClearance)) {
  //      if (smoothClearance < 0.)
  //        smoothClearance = 0.;
  //    }
  //    if (smoothClearance > 0.)
  //      clearanceNoisyIteration = GetIntValue(200, 2, 1000, _T("Количество итераций сглаживания: "));
  //    data = true;
  //    
  //    if (smoothing == 3 && (smoothClearance == 0. || clearanceNoisyIteration == 0)) {
  //      // Дайте реальные значения коридора 
  //      MessageBoxEx(_T("Задайте ненулевые значения"), IDS_TITLE_ERR);
  //      data = false;
  //    }
  //  }
  //
  //
  //}
  int fixFirst = 0;
  int fixLast = 0;
  if ( smoothing > 3 ) {
    if ( GetInt( _T( "Фиксировать начальную точку (0 - нет, 1 - да)" ), fixFirst ) ) {
      if ( fixFirst < 0 )
        fixFirst = 0;
    }
    if ( GetInt( _T( "Фиксировать конечную точку (0 - нет, 1 - да)" ), fixLast ) ) {
      if ( fixLast < 0 )
        fixLast = 0;
    }
  }
  fairing = (MbeFairSmoothing)smoothing;
  size_t degreeBSpline = 8;
  bool arrange = false;
  MbeFairCurvature accountCurvature = (MbeFairCurvature)0;
  MbeFairVector accountInflexVector = fairSmooth_Segment;
  MbeFairSubdivision subdivision = (MbeFairSubdivision)0;
  double realAccuracyBSpl = METRIC_ACCURACY * 0.01;
  size_t numberOfIterationsBSpl = 1000;

  int select = 0;
  if ( GetInt( _T( "Учитывать кривизну в начальной точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
  }
  accountCurvature = (MbeFairCurvature)select;
  select = 0;
  if ( GetInt( _T( "Учитывать кривизну в последней точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
    if ( select == 1 ) {
      select = 2;
    }
  }
  accountCurvature = (MbeFairCurvature)( accountCurvature + select );

  if ( smoothing < 3 ) { // Построение без сглаживания
    // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
    degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg );

    if ( smoothing > 0 )
      subdivision = (MbeFairSubdivision)GetIntValue( 1, 0, 2, gSubdivisionMsg );

    arrange = GetIntValue( 0, 0, 1, gArrangeMsg ) > 0;

    //==
    select = 0;
    if ( GetInt( _T( "Изменить параметры точности: 0 - не изменять, 1- изменить" ), select ) ) {
      if ( select > 0 ) {
        //Точность построения
        double delta = realAccuracyBSpl;
        if ( GetDouble( _T( "Точность построения: " ), delta ) ) {
          if ( delta < 0. )
            delta = 0.;
        }

        realAccuracyBSpl = delta;

        int iterations = (int)numberOfIterationsBSpl;
        if ( GetInt( _T( "Ограничение количества итераций при подгонке В-сплайном:" ), iterations ) ) {
          if ( iterations < 0 )
            iterations = 1;
        }
        numberOfIterationsBSpl = iterations;
      }
    }
  }
  else { // Построение со сглаживанием.
    degreeBSpline = outSegments - 2;
    if ( degreeBSpline < 6 )
      degreeBSpline = 4;
    else if ( degreeBSpline > 8 )
      degreeBSpline = 8;
    degreeBSpline = ( degreeBSpline / 2 ) * 2;
  }
  //==

  //Сброс активации параметров
  bool fixFirstPointAtNoisy = false;
  bool fixLastPointAtNoisy = false;

  if ( fixFirst > 0 )
    fixFirstPointAtNoisy = true;
  if ( fixLast > 0 )
    fixLastPointAtNoisy = true;

  MbeFairSplineFormat outFormat = fairFormat_Close;
  if ( smoothing >= 3 ) {
    outFormat = fairFormat_Close;
  }

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._fairing = fairing;
    parameters._accountCurvature = accountCurvature;
    parameters._degree = degreeBSpline;
    parameters._subdivision = subdivision;
    parameters._arrange = arrange;
    parameters.SetPrecision( realAccuracyBSpl );
    parameters._iterationsNumber = numberOfIterationsBSpl;
    parameters._accountVectorInflect = accountInflexVector;
    parameters._noisyDeviation = smoothClearance;
    parameters._noisyIterations = clearanceNoisyIteration;

    parameters._fixFirstPointNoisy = fixFirstPointAtNoisy;
    parameters._fixLastPointNoisy = fixLastPointAtNoisy;
    //fData.nSegments = outSegments;
    //fData.outFormat = outFormat;
    //fData.scaleParam = scaleLength;

    MbFairCurveMethod method( fairCrvType_BSpline, fairData_GDHPoly );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.fairing = fairing;
    fData.degreeBSpline = degreeBSpline;
    fData.subdivision = subdivision;
    fData.accountInflexVector = accountInflexVector;
    fData.arrange = arrange;
    fData.accountCurvature = accountCurvature;
    fData.realAccuracyBSpl = realAccuracyBSpl;
    fData.numberOfIterationsBSpl = numberOfIterationsBSpl;
    fData.clearanceNoisy = smoothClearance;
    fData.clearanceNoisyIteration = clearanceNoisyIteration;
    fData.nSegments = outSegments; // По умолчанию 0 для простого построения

    fData.switchFixFirstPointAtNoisy = fixFirstPointAtNoisy;
    fData.switchFixLastPointAtNoisy = fixLastPointAtNoisy;
    fData.outFormat = outFormat;
    fData.scaleParam = scaleLength;

    // Программа построения плавной B-кривой [со сглаживанием] на ГО Эрмита произвольной формы.
    MbCurve3D * resCurve = nullptr;
    ::CreateFairBSplineCurveOnBasePolylineOfHermiteGDInflex( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    if ( fData.error == 0 )
      TestAccuracyHermite( pllne, resCurve );
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBSplineCurveOnTangentPolyline()
// ---
int Test_CreateFairBSplineCurveOnTangentPolyline( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();

  // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
  size_t degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg );
  MbeFairSubdivision subdivision = (MbeFairSubdivision)GetIntValue( 1, 0, 2, gSubdivisionMsg );
  bool arrange = GetIntValue( 0, 0, 1, gArrangeMsg ) > 0;

  int smoothT = 0;
  GetInt( _T("Сглаживать кручение: нет = 0, да = 1"), smoothT );
  bool smoothTorsion = smoothT > 0;

  MbeFairVector accountInflexVector = (MbeFairVector)GetIntValue( 0, 0, 2, gInflexMsg );

  //==
  double realAccuracyBSpl = METRIC_ACCURACY * 0.01; //как в fair_curve_class.h
  int select = 0;
  size_t numberOfIterationsBSpl = 1000; //как в fair_curve_class.h
  if ( GetInt(_T("Изменить параметры точности: 0 - не изменять, 1- изменить"), select) ) {
    if ( select > 0 ) {
      //Точность построения
      double delta = realAccuracyBSpl;
      if ( GetDouble(_T("Точность построения: "), delta) ) {
        if ( delta < 0.)
          delta = 0.;
      }

      realAccuracyBSpl = delta;

      int iterations = (int)numberOfIterationsBSpl;
      if ( GetInt(_T("Ограничение количества итераций при подгонке В-сплайном:"), iterations) ) {
        if ( iterations < 0 )
          iterations = 1;
      }
      numberOfIterationsBSpl = iterations;
    }
  }
  //==

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._degree = degreeBSpline;
    parameters._subdivision = subdivision;
    parameters._accountVectorInflect = accountInflexVector;
    parameters._arrange = arrange;
    parameters._smoothTorsion = smoothTorsion;
    parameters.SetPrecision( realAccuracyBSpl );
    parameters._iterationsNumber = numberOfIterationsBSpl;

    MbFairCurveMethod method( fairCrvType_BSpline, fairData_Tangent );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.degreeBSpline = degreeBSpline;
    fData.subdivision = subdivision;
    fData.accountInflexVector = accountInflexVector;
    fData.arrange = arrange;
    fData.smoothTorsion = smoothTorsion;
    fData.realAccuracyBSpl = realAccuracyBSpl;
    fData.numberOfIterationsBSpl = numberOfIterationsBSpl;

    MbCurve3D * resCurve = nullptr;
    ::CreateFairBSplineCurveOnTangentPolyline( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }

    if ( fData.error > 0 ) { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBSplineCurveOnTangentsOfHermiteGD()
// ---
int Test_CreateFairBSplineCurveOnTangentsOfHermiteGD( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();
  if ( !IsHermit(pllne) ) {
    outErrors = rt_ObjectNotHermiteGD; // Объект не ГО Эрмита. rt_Success) 
    string_t message(IDS_TITLE_ERR);
    message = MessageError(outErrors); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx(message.c_str(), IDS_TITLE_ERR); // Выводит сообщение.
    return outErrors;
  }

  // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
  size_t degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg );

  MbeFairSubdivision subdivision = (MbeFairSubdivision)GetIntValue( 1, 0, 2, gSubdivisionMsg );

  int select = 0;
  if ( GetInt( _T( "Учитывать кривизну в начальной точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
  }
  MbeFairCurvature accountCurvature = (MbeFairCurvature)select;
  select = 0;
  if ( GetInt( _T( "Учитывать кривизну в последней точке: 0 - не учитывать, 1- учитывать" ), select ) ) {
    if ( select < 0 )
      select = 0;
    if ( select > 1 )
      select = 1;
    if ( select == 1 ) {
      select = 2;
    }
  }
  accountCurvature = (MbeFairCurvature)( accountCurvature + select );
  bool arrange = false;
  //accountInflexVector
  //int accountInflexVector = 1;
  //if ( GetInt(_T("Векторы в точках перегиба: направления звеньев S-полигона = 0, касательные = 1"), accountInflexVector) ) {
  //  if ( accountInflexVector < 0 )
  //    accountInflexVector = 0;
  //  if ( accountInflexVector > 1 )
  //    accountInflexVector = 1;
  //}
  MbeFairVector accountInflexVector = (MbeFairVector)1;

  //==
  select = 0;
  double realAccuracyBSpl = METRIC_ACCURACY * 0.01; //как в fair_curve_class.h
  size_t numberOfIterationsBSpl = 1000; //как в fair_curve_class.h
  if ( GetInt(_T("Изменить параметры точности: 0 - не изменять, 1- изменить"), select) ) {
    if ( select > 0 ) {
      //Точность построения
      double delta = realAccuracyBSpl;
      if ( GetDouble(_T("Точность построения: "), delta) ) {
        if ( delta < 0. )
          delta = 0.;
      }
      realAccuracyBSpl = delta;

      int iterations = (int)numberOfIterationsBSpl;
      if ( GetInt(_T("Ограничение количества итераций при подгонке В-сплайном:"), iterations) ) {
        if ( iterations < 0 )
          iterations = 1;
      }
      numberOfIterationsBSpl = iterations;
    }
  }
  //==

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairCreateData parameters;
    parameters._degree = degreeBSpline;
    parameters._subdivision = subdivision;
    parameters._accountCurvature = accountCurvature;
    parameters._arrange = arrange;
    parameters._accountVectorInflect = accountInflexVector;
    parameters.SetPrecision( realAccuracyBSpl );
    parameters._iterationsNumber = numberOfIterationsBSpl;

    MbFairCurveMethod method( fairCrvType_BSpline, fairData_GDHTangent );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.degreeBSpline = degreeBSpline;
    fData.subdivision = subdivision;
    fData.arrange = arrange;
    fData.accountCurvature = accountCurvature;
    fData.accountInflexVector = accountInflexVector;
    fData.realAccuracyBSpl = realAccuracyBSpl;
    fData.numberOfIterationsBSpl = numberOfIterationsBSpl;

    MbCurve3D * resCurve = nullptr;
    ::CreateFairBSplineCurveOnTangentsOfHermiteGD( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    if ( fData.error == 0 )
      TestAccuracyHermite( pllne, resCurve );
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.

  }
  return outError;
}


//---------------------------------------------------------------------------------
//
// ---
int Test_ElevateDegreeBezierCurve( bool createWire )
{
  int outError = 0;

  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( createWire ) { // Создавать проволочный каркас.
      MbFairCurveMethod method( fairChange_Elevate );
      MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
      c3d::WireFrameSPtr resCurve;

      outError = ::FairCurve( MbFairCurveParams(*curvePtr, snMaker), method, MbFairChangeData(), resCurve );
      if ( outError == 0 ) {
        Style style = TestVariables::BSPLINE_Style;
        TestVariables::viewManager->DeleteObject( obj );
        TestVariables::viewManager->AddObject( style, resCurve );
      }
    }
    else {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
      MbCurve3D * resCurve = nullptr;
      outError = ElevateDegreeNurbzs( curve, fData, resCurve );
      if ( outError == 0 ) {
        Style style = TestVariables::BSPLINE_Style;
        TestVariables::viewManager->DeleteObject( obj );
        TestVariables::viewManager->AddObject( style, resCurve );
      }
    }

    if ( outError == 0 )
      MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
    else
      MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
  }

  return outError;
}

//---------------------------------------------------------------------------------
//
// ---
int Test_ReParamCurve( bool createWire )
{
  int outError = 0;

  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
      int typeControlPolygon;
      int nSegments;
      size_t curDegree;

      if ( ( outError = AnalyzeNurbs( curve, typeControlPolygon, nSegments, curDegree ) ) > 0 ) {
        MessageBoxEx( _T( "Некорректная NURBS кривая" ), IDS_TITLE_ERR );
        return outError;
      }

      int outSegments = nSegments / 2;
      GetInt( _T( "Укажите количество сегментов (>= 6):" ), outSegments );
      if ( outSegments < 6 )
        outSegments = 6;

      double scaleLength = 4.;
      GetDouble( _T( "Укажите масштаб влияния длины (> 1.0):" ), scaleLength );
      if ( scaleLength < 1. )scaleLength = 1.;

      int mYesNoisy = 0;
      GetInt( _T( "Сглаживать кривую:" ), mYesNoisy );
      if ( mYesNoisy < 0 )
        mYesNoisy = 0;

      size_t nDegree = outSegments - 2;
        if ( nDegree < 6 )
          nDegree = 4;
        if ( nDegree > 8 )
          nDegree = 8;
        nDegree = ( nDegree / 2 ) * 2;

      if ( createWire ) {
        MbFairChangeData parameters;
        parameters._outFormat = fairFormat_Close;
        parameters._nSegments = outSegments;
        parameters._scaleParam = scaleLength;
        parameters._degree = nDegree;
        if ( mYesNoisy > 0 )
          parameters._fairing = fairSmooth_Noisy;

        MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
        MbFairCurveMethod method( fairChange_Reparam );
        c3d::WireFrameSPtr resCurve;

        if ( ::FairCurve( MbFairCurveParams(*curvePtr, snMaker), method, parameters, resCurve ) == rt_Success ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
      else {
        fData.outFormat = fairFormat_Close;
        fData.nSegments = outSegments;
        fData.scaleParam = scaleLength;
        fData.degreeBSpline = nDegree;
        if ( mYesNoisy > 0 )
          fData.fairing = fairSmooth_Noisy;

        MbCurve3D * resCurve = nullptr;
        if ( ::ReparamNurbs( curve, fData, resCurve ) == rt_Success ) { // Репараметризация
          // Нарисовать кривую.
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }

  return outError;
}

//---------------------------------------------------------------------------------
//
// ---
int Test_SubdivideNurbsCurve( bool createWire ) {

  int outError = 0;

  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
      if ( createWire ) {
        MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
        c3d::WireFrameSPtr resCurve;
        if ( ::FairCurve( MbFairCurveParams(*curvePtr, snMaker), MbFairCurveMethod(fairChange_Subdivide) ,
             MbFairChangeData(), resCurve ) == rt_Success ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_MSG );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
      else {
        MbCurve3D * resCurve = nullptr;
        if ( ::SubdivideNurbs( curve, fData, resCurve ) == 0 ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_MSG );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }

  return outError;
}


//---------------------------------------------------------------------------------
//
// ---
int Test_ExtractChangeFormatNurbsCurve( bool createWire )
{
  int outError = 0;

  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
      int typeControlPolygon;
      int nSegments;
      size_t curDegree;

      if ( ( outError = AnalyzeNurbs( curve, typeControlPolygon, nSegments, curDegree ) ) > 0 ) {
        MessageBoxEx( _T( "Некорректная NURBS кривая" ), IDS_TITLE_ERR );
        return outError;
      }

      int outFormat = typeControlPolygon;
      GetInt( _T( "Укажите выходной формат для кривой 2 - BSpline, 3 - NURBzS" ), outFormat );
      if ( outFormat < 2 )
        outFormat = 2;
      if ( outFormat > 3 )
        outFormat = 3;

      int startSegment = 0;
      GetInt( _T( "Укажите номер первого сегмента" ), startSegment );
      if ( startSegment < 0 )
        startSegment = 0;
      if ( startSegment > nSegments - 1 )
        startSegment = nSegments - 1;

      int outSegments = nSegments - startSegment;
      GetInt( _T( "Укажите количество сегментов" ), outSegments );
      if ( outSegments > nSegments - startSegment )
        outSegments = nSegments - startSegment;
      if ( outSegments < 1 )
        outSegments = 1;

      if ( createWire ) {
        MbFairChangeData parameters;
        parameters._outFormat = (MbeFairSplineFormat)outFormat;
        parameters._numSegment = startSegment;
        parameters._nSegments = outSegments;

        MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
        MbFairCurveMethod method( fairChange_Change );
        c3d::WireFrameSPtr resCurve;
        if ( ::FairCurve( MbFairCurveParams(*curvePtr, snMaker), method, parameters, resCurve ) == rt_Success ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
      else {
        fData.outFormat = (MbeFairSplineFormat)outFormat;
        fData.numSegment = startSegment;
        fData.nSegments = outSegments;

        MbCurve3D * resCurve = nullptr;
        if ( ::ExtractChangeNurbs( curve, fData, resCurve ) == 0 ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }

  return outError;
}


//---------------------------------------------------------------------------------
// 
// ---
int Test_AddKnotNurbsCurve( bool createWire )
{
  int outError = 0;

  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
      int typeControlPolygon;
      int nSegments;
      size_t curDegree;

      if ( ( outError = AnalyzeNurbs( curve, typeControlPolygon, nSegments, curDegree ) ) > 0 ) {
        MessageBoxEx( _T( "Некорректная NURBS кривая" ), IDS_TITLE_ERR );
        return outError;
      }
      int numSegment = nSegments - 1;
      double tParam = 0.5;
      GetInt( _T( "Укажите номер сегмента для вставки узла" ), numSegment );

      GetDouble( _T( "Укажите значение параметра t: (0.1<= t <= 0.9)" ), tParam );
      if ( numSegment < 0 )
        numSegment = 0;
      if ( numSegment > nSegments - 1 )
        numSegment = nSegments - 1;

      if ( tParam < 0.1 )
        tParam = 0.1;
      if ( tParam > 0.9 )
        tParam = 0.9;

      if ( createWire ) {
        MbFairChangeData parameters;
        parameters._numSegment = numSegment;
        parameters._tParam = tParam;

        MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
        MbFairCurveMethod method( fairChange_AddKnot );
        c3d::WireFrameSPtr resCurve;
        if ( ::FairCurve( MbFairCurveParams(*curvePtr, snMaker), method, parameters, resCurve ) == rt_Success ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
      else {
        fData.numSegment = numSegment;
        fData.tParam = tParam;

        MbCurve3D * resCurve = nullptr;
        if ( AddKnotNurbs( curve, fData, resCurve ) == 0 ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }

  return outError;
}


//---------------------------------------------------------------------------------
// 
// ---
int Test_ConvertBezierCurveToConicBSpline( bool createWire )
{
  int outError = 0;

  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );

      if ( createWire ) {
        MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
        MbFairCurveMethod method( fairChange_Conic );
        c3d::WireFrameSPtr resCurve;
        if ( ::FairCurve( MbFairCurveParams(*curvePtr, snMaker), method, MbFairChangeData(), resCurve ) == rt_Success ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
      else {
        MbCurve3D * resCurve = nullptr;
        if ( ConicNurbs( curve, fData, resCurve ) == 0 ) {
          Style style = TestVariables::BSPLINE_Style;
          TestVariables::viewManager->DeleteObject( obj );
          TestVariables::viewManager->AddObject( style, resCurve );
          MessageBoxEx( _T( "Кривая успешно преобразована" ), IDS_TITLE_ALL_RIGHT );
        }
        else {
          MessageBoxEx( _T( "Кривая не преобразована" ), IDS_TITLE_ERR );
        }
      }
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }

  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateClothod();
// ---
int Test_CreateClothoid( bool createWire )
{
  double clothoidRMin = 50;
  GetDouble( _T( "Specify dynamic params for curve. rMin:" ), clothoidRMin );

  double clothoidLMax = 200;
  GetDouble( _T( "Specify dynamic params for curve. lMax:" ), clothoidLMax );

  int nP = 10;
  GetInt( _T( "Enter a number of segms of NURBzS:" ), nP );
  size_t clothoidSegms = nP;
  MbResultType res = rt_Success;

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbClothoidParams parameters;
    parameters._radiusMin = clothoidRMin;
    parameters._lengthMax = clothoidLMax;
    parameters._segmsNum = clothoidSegms;

    c3d::WireFrameSPtr resCurve;
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );

    res = ::FairCurve( parameters, snMaker, resCurve );

    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
  }
  else {
    fData.clothoidLMax = clothoidLMax;
    fData.clothoidRMin = clothoidRMin;
    fData.clothoidSegms = clothoidSegms;

    MbCurve3D * resCurve = nullptr;
    KernelCreateClothoid( fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error );      // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );     // Выводит сообщение.
    }
    res = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning );    // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN );       // Выводит сообщение.
  }
  return res;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Программа построения кривой с монотонным изменением кривизны 
// в заданном соприкасающемся треугольнике.
// Метод Зиатдинова - Кима - Набиева.
// Rushan Ziatdinov, Tae-wan Kim, Rifkat Nabiyev. Two-point G1 Hermite interpolation in biangular coordinates /
// Journal of Computational and Applied Mathematics 287 (2015) 1-11
// Кривую можно использовать для профилирования лопаток турбин, компрессоров, 
// насосов и т.п. изделий. Для трассирования участков дороги. Для построения сопряжений кривых.
// В промдизайне в качестве эстетической кривой.
//////////////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
///< \ru Построить и аппроксимировать сектрису Маклорена. \en Create and approximate the Maclaurin sectrix.
// ---
int Test_CreateSectrix( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  if ( n != 3 ) {
    //::MessageBoxEx(_T("*** Нужен двухзвенник"), IDS_TITLE_ERR);
    outError = 28;  // Не выбран нужный объект
    return outError;
  }

  MbCurve3D * pllne = polyPtr.get();

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveMethod method( fairCrvType_Sectrix );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams(*polyPtr, snMaker), method, MbFairCreateData(), resCurve );

    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    MbCurve3D * resCurve = nullptr;
    KernelCreateSectrix( pllne, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }

    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Test_CreateFairBezierFilletOnBasePolyline();
// ---
int Test_CreateFairBezierFilletOnPolyline( bool createWire )
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();

  int TypeCreate = 0;
  GetInt( _T( "Тип ломаной: опорная = 0, касательная = 1" ), TypeCreate );
  if ( TypeCreate < 0 ) {
    TypeCreate = 0;
  }
  if ( TypeCreate > 2 ) {
    TypeCreate = 1;
  }
  MbeFairMethod create = (MbeFairMethod)( TypeCreate + 1 );
   
  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts );

  MbCartPoint3D  FirstPoint = cvrts[0];
  MbCartPoint3D  LastPoint = cvrts[cvrts.Count() - 1];

  MbVector3D FirstTangent, LastTangent, FirstBNormal, LastBNormal;
  MbVector3D FirstDr, FirstD2r, FirstD3r, LastDr, LastD2r, LastD3r;
  double FirstCvt, LastCvt, FirstRadius, LastRadius;

  // Укажите первую кривую 
  MbItem * obj( nullptr );
  c3d::SpaceCurveSPtr curvePtr( ReadNurbs( outError, obj ) );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
      double t;
      curve->NearPointProjection( FirstPoint, t, false );
      FirstCvt = curve->Curvature( t );
      curve->FirstDer( t, FirstDr );
      curve->BNormal( t, FirstBNormal );
      curve->Tangent( t, FirstTangent );
      curve->SecondDer( t, FirstD2r );
      MbVector3D f;
      curve->ThirdDer( t, FirstD3r );

      if ( FirstCvt != 0 )
        FirstRadius = 1.0 / FirstCvt;
      else
        FirstRadius = MB_MAXDOUBLE;
    }
    else { // Не выбрана кривая 
      outError = 2;
      return outError;
    }
  }
  else { // Кривая не NURBS
    outError = 2;
    return outError;
  }

  // Укажите вторую кривую
  curvePtr = ReadNurbs( outError, obj );
  if ( curvePtr != nullptr ) {
    if ( curvePtr->IsA() == st_Nurbs3D ) {
      MbNurbs3D * curve = static_cast<MbNurbs3D *>( curvePtr.get() );
      double t;
      curve->NearPointProjection( LastPoint, t, false );
      LastCvt = curve->Curvature( t );
      curve->FirstDer( t, LastDr );
      curve->BNormal( t, LastBNormal );
      curve->Tangent( t, LastTangent );
      curve->SecondDer( t, LastD2r );
      MbVector3D f;
      curve->ThirdDer( t, LastD3r );

      if ( LastCvt != 0 )
        LastRadius = 1.0 / LastCvt;
      else
        LastRadius = MB_MAXDOUBLE;
    }
    else { // Не выбрана кривая 
      outError = 2;
      return outError;
    }
  }
  else { // Кривая не NURBS
    outError = 2;
    return outError;
  }

  //
  bool switchEndTangents = true;

  int SwitchEndCvt = 0;
  GetInt( _T( "Учитывать кривизну: нет = 0, да = 1" ), SwitchEndCvt );
  if ( SwitchEndCvt < 0 ) {
    SwitchEndCvt = 0;
  }
  if ( SwitchEndCvt > 2 ) {
    SwitchEndCvt = 1;
  }
  bool switchEndCurvature = SwitchEndCvt > 0;

  // Построение v-кривой на ГО и аппроксимация посредством NURBS кривой.
  MbeFairSubdivision subdivision = (MbeFairSubdivision)GetIntValue( 1, 0, 2, gSubdivisionMsg );
  bool arrange = GetIntValue( 0, 0, 1, gArrangeMsg ) > 0;

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    MbFairFilletData parameters;
    parameters._tangent = create == fairMethod_Tangent;
    parameters._switchEndTangents = switchEndTangents;
    parameters._firstTangent= FirstTangent;
    parameters._lastTangent = LastTangent;
    parameters._firstCurvature = FirstCvt;
    parameters._lastCurvature = LastCvt;
    parameters._switchEndCurvature = switchEndCurvature;
    parameters._subdivision = subdivision;
    parameters._arrange = arrange;

    MbFairCurveResult resCurve;
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );

    MbResultType res = ::FairCurve( MbFairCurveParams(*polyPtr, snMaker), parameters, resCurve );

    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.create = create;
    fData.switchEndTangents = switchEndTangents;
    fData.firstTangent = FirstTangent;
    fData.lastTangent = LastTangent;
    fData.firstCurvature = FirstCvt;
    fData.lastCurvature = LastCvt;
    fData.switchEndCurvature = switchEndCurvature;
    fData.subdivision = subdivision;
    fData.arrange = arrange;

    MbCurve3D * resCurve = nullptr;
    ::CreateFairBezierFilletOnPolyline( pllne, fData, resCurve );

    if ( fData.error == 0 ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = BSPLINE_Style;
      viewManager->AddObject( style, resCurve );
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


//---------------------------------------------------------------------------------
// Построение B-сплайн сопряжения.
// TODO: перевести на новый интерфейс.
// ---
int Test_CreateFairBSplineFilletOnSPolygon()
{
  int outError = 0;

  SArray<double> resWts, resKnots; //Для нестандартного представления в C3D полуоткрытого S-полигона.
  SArray<double> resWts2, resKnots2;
  // Укажите первую кривую 
  MbNurbs3D * curve;
  MbCurve3D * oneSpolyCurve;
  MbControlData3D cvrts;
  size_t bspDegree = 8;

  // Объекты для удаления после построения сопряжения по запросу
  MbItem * curveObj = nullptr;
  MbItem * obj = nullptr;
  if ( GetGObj( _T( "Specify a first onesegmented curve for fillet " ), st_Curve3D, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    curve = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( curve != nullptr && ( curve->IsA() == st_Nurbs3D || curve->IsA() == st_Bezier3D ||
         curve->IsA() == st_Hermit3D ) ) {
      // Запомнить параметры S-полигона

      //curve->GetBasisPoints(cvrts);
      //curve->GetKnots(knots);
      //curve->GetWeights(wts);
      bspDegree = curve->GetDegree() - 1;

      // Нарисовать полуоткрытый S-полигон на первом сегменте
      //size_t nResWts = ( bspDegree + 2 ) * 2; // Уплотнение
      // size_t nResKnots = ( bspDegree + 2 * bspDegree + 2 ) * 2; // Уплотнение

      //resWts.resize(nResWts); // Для весов 
      //resKnots.resize(nResKnots); // Для узлов 

      MbNurbs3D * oneSegmNurbs = dynamic_cast<MbNurbs3D *>( curve );
      fData.numSegment = 0;
      fData.outFormat = (MbeFairSplineFormat)0; // Преобразовать в полуоткрытый S-полигон
      fData.nSegments = 1;
      if ( ExtractChangeNurbsParams( oneSegmNurbs, fData, resWts, resKnots, oneSpolyCurve ) == 0 ) {
        //double r = resWts[0];
        Style style = BSPLINE_Style;
        //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
        curveObj = TestVariables::viewManager->AddObject( style, oneSpolyCurve );
        //MessageBoxEx( _T( "S-полигон кривой нарисован" ), IDS_TITLE_ALL_RIGHT );
        curveObj->GetBasisPoints( cvrts );
      }
      else {
        MessageBoxEx( _T( "S-полигон кривой не нарисован" ), IDS_TITLE_ERR );
        return false;
      }
    }
    else { // Не выбрана кривая 
      outError = 2;
      return outError;
    }
  }
  else { // Кривая не NURBS
    outError = 2;
    return outError;
  }

  // Укажите вторую кривую 
  MbNurbs3D * curve2;
  MbCurve3D * oneSpolyCurve2;
  MbControlData3D cvrts2;

  // Объекты для удаления после построения сопряжения по запросу
  MbItem * curveObj2 = nullptr;
  MbItem * obj2 = nullptr;
  if ( GetGObj( _T( "Specify a second onesegmented curve for fillet" ), st_Curve3D, &obj2 ) && ( obj2->IsA() == st_SpaceInstance ) ) {
    curve2 = (MbNurbs3D *)( (MbSpaceInstance *)obj2 )->GetSpaceItem();
    if ( curve2 != nullptr && ( curve2->IsA() == st_Nurbs3D || curve2->IsA() == st_Bezier3D ||
         curve2->IsA() == st_Hermit3D ) ) {
      //curve2->GetBasisPoints(cvrts2);
      //curve2->GetKnots(knots2);
      //curve2->GetWeights(wts2);
      if ( bspDegree != curve2->GetDegree() - 1 ) {
        MessageBoxEx( _T( "Несовпадение степеней" ), IDS_TITLE_ERR );
        return false;
      }
    }
    else { // Не выбрана кривая 
      outError = 2;
      return outError;
    }

    // Нарисовать полуоткрытый S-полигон на втором сегменте
    /// size_t nResWts2 = ( bspDegree + 2 ) * 2; // Уплотнение
    /// size_t nResKnots2 = ( bspDegree + 2 * bspDegree + 2 ) * 2; // Уплотнение

    //resWts2.resize(nResWts2); // Для весов 
    //resKnots2.resize(nResKnots2); // Для узлов 

    MbNurbs3D * oneSegmNurbs2 = dynamic_cast<MbNurbs3D *>( curve2 );

    fData.numSegment = 0;
    fData.outFormat = (MbeFairSplineFormat)0; // Преобразовать в полуоткрытый S-полигон
    fData.nSegments = 1;
    fData.degreeBSpline = bspDegree;
    if ( ExtractChangeNurbsParams( oneSegmNurbs2, fData, resWts2, resKnots2, oneSpolyCurve2 ) == 0 ) {
      //double r = resWts2[0];
      Style style = BSPLINE_Style;
      //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
      curveObj2 = TestVariables::viewManager->AddObject( style, oneSpolyCurve2 );
      //MessageBoxEx( _T( "S-полигон второй кривой нарисован" ), IDS_TITLE_ALL_RIGHT );
      curveObj2->GetBasisPoints( cvrts2 );
    }
    else {
      MessageBoxEx( _T( "S-полигон второй кривой не нарисован" ), IDS_TITLE_ERR );
      return false;
    }
  }
  else { // Кривая не NURBS
    outError = 2;
    return outError;
  }

  // Укажите / Нарисуйте S-полигон сопряжения
  MbControlData3D plcvs;
  MbCurve3D * pllne = nullptr;
  bool closed;
  size_t n = 0;
  int toDo = 1;

  size_t n_Pllne = 0;
  MbCartPoint3D  FirstPoint;
  MbCartPoint3D  LastPoint;

  if ( toDo == 0 ) { //Нарисовать
  }

  if ( toDo == 1 ) { //Указать
    if ( ( outError = ReadPllneFillet( pllne, closed, n ) ) > 0 )
      return outError;

    pllne->GetBasisPoints( plcvs );
    n_Pllne = plcvs.Count();

    FirstPoint = plcvs[0];
    LastPoint = plcvs[n_Pllne - 1];
#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf( prt, "\n FirstPoint = %f %f %f", FirstPoint.x, FirstPoint.y, FirstPoint.z );
    //fprintf( prt, "\n LastPoint = %f %f %f", LastPoint.x, LastPoint.y, LastPoint.z );
#endif
  }
  // Параметры начала кривой сопряжения
#ifdef C3D_DEBUG_FAIR_CURVES
  size_t nResKnots = 2 * bspDegree + 2;
  //for ( size_t ir = 0; ir < nResKnots; ir++ ) {
  //  fprintf( prt, "\n ir = %zd knots = %f", ir, resKnots[ir] );
  //}
#endif

  double t;
  double tsp;
  oneSpolyCurve->NearPointProjection( FirstPoint, tsp, false );
  curve->NearPointProjection( FirstPoint, t, false );
#ifdef C3D_DEBUG_FAIR_CURVES
  // fprintf( prt, "\n t = %f", t );
  //fprintf( prt, "\n tsp2 = %f", tsp );

  // Параметры конца кривой сопряжения
  //for ( size_t ir = 0; ir < nResKnots; ir++ ) {
  //  fprintf( prt, "\n ir = %zd knots2 = %f", ir, resKnots2[ir] );
  //}
#endif

  double tsp2;
  double t2;
  curve2->NearPointProjection( LastPoint, t2, false );
  oneSpolyCurve2->NearPointProjection( LastPoint, tsp2, false );
#ifdef C3D_DEBUG_FAIR_CURVES
  //fprintf( prt, "\n t2 = %f", t2 );
  //fprintf( prt, "\n tsp2 = %f", tsp2 );
#endif

  // Создать параметры сопряжения

  SArray<MbCartPoint3D> fvrtxList;     ///< \ru Множество вершин сплайнового полигона. \en Set of vertices of the spline polygon.
  SArray<double>        fweights;      ///< \ru Множество весов контрольных точек. \en Set of m_weights of the control vertices. 
  SArray<double>        fknots;        ///< \ru Узловой вектор сплайна. \en Knot vector of the spline.
  size_t fDegree = bspDegree;
  bool fclosed = false;
  MbCurve3D * fresCurve;
  // Создать NurbsCurve
  size_t n1 = (size_t)( tsp + 0.25 ) + 1;
  for ( size_t ir = 0; ir < n1; ir++ ) {
    fvrtxList.Add( cvrts[ir] );
    fweights.Add( resWts[ir] );
    //fweights.Add(1.0);
  }
  for ( size_t ir = 1; ir < n_Pllne - 1; ir++ ) {
    fvrtxList.Add( plcvs[ir] );
    fweights.Add( 1. );
  }
  size_t n2 = (size_t)( tsp2 + 0.25 ) + 1;
  for ( size_t ir = 0; ir < n2; ir++ ) {
    fvrtxList.Add( cvrts2[n2 - ir - 1] );
    //fweights.Add(1.0);
    fweights.Add( resWts2[n2 - ir - 1] );
  }
  // 
  size_t nDots = fvrtxList.Count() - fDegree + 1;
  for ( size_t ir = 0; ir < fDegree; ir++ ) {
    fknots.Add( 0.0 );
  }
  for ( size_t ir = 0; ir < nDots; ir++ ) {
    fknots.Add( (double)ir );
  }
  for ( size_t ir = 0; ir < fDegree; ir++ ) {
    fknots.Add( (double)( nDots - 1 ) );
  }
#ifdef C3D_DEBUG_FAIR_CURVES
  //for ( size_t ir = 0; ir < fknots.Count(); ir++ ) {
  //  fprintf( prt, "\n ir = %zd fknots = %f", ir, fknots[ir] );
  //}
  //for ( size_t ir = 0; ir < fweights.Count(); ir++ ) {
  //  fprintf( prt, "\n ir = %zd fweights = %f", ir, fweights[ir] );
  //}
#endif
  //fknots.clear();

  ::NurbsCurve( fvrtxList, fweights, fDegree + 1, fknots, fclosed, fresCurve );
  MbNurbs3D * nurbsCurve = dynamic_cast<MbNurbs3D *>( fresCurve );
  MbItem * curveObjF = nullptr;
  if ( nurbsCurve != nullptr ) {
    nurbsCurve->SetFormType( ncf_FairCurveForm );

    //MbItem * curveObj2 = nullptr;
    Style style = BSPLINE_Style;
    //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
    curveObjF = TestVariables::viewManager->AddObject( style, fresCurve );
  }

  //esWts.HardFlush();
  //esKnots.HardFlush();
  //esWts2.HardFlush();
  //esKnots2.HardFlush();

  return outError;
}


//---------------------------------------------------------------------------------
// Извлечь участок сегмента (экстраполяцию сегмента) в виде B-сплайновой кривой с отдельным S-полигоном.
// ---
int Test_ExtractExtrapolOneSegment()
{
  int outError = 0;
  MbControlData3D cvrts;
  // MbCurve3D * pllne = nullptr;
  // bool closed;
  // size_t n = 0;

  MbCartPoint3D  FirstPoint;

  MbVector3D FirstTangent;
  MbVector3D LastTangent;
  double FirstCvt;
  // double LastCvt;
  double FirstRadius;
  //double LastRadius;
  MbVector3D FirstBNormal;
  MbVector3D LastBNormal;

  MbVector3D FirstDr;
  MbVector3D FirstD2r;
  MbVector3D FirstD3r;

  MbVector3D LastDr;
  MbVector3D LastD2r;
  MbVector3D LastD3r;

  // Укажите первую кривую 
  MbNurbs3D * curve;

  MbItem * obj = nullptr;
  if ( GetGObj( _T( "Specify a curve" ), st_Curve3D, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    curve = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( curve != nullptr && ( curve->IsA() == st_Nurbs3D || curve->IsA() == st_Bezier3D ||
         curve->IsA() == st_Hermit3D ) ) {
      if ( GetPoint( IDS_SELECT_POINT, FirstPoint ) ) {
      }
      else { // Не выбрана точка 
        outError = 2;
        return outError;
      }

      double t;
      curve->NearPointProjection( FirstPoint, t, false );
      FirstCvt = curve->Curvature( t );
      curve->FirstDer( t, FirstDr );
      curve->BNormal( t, FirstBNormal );
      curve->Tangent( t, FirstTangent );
      curve->SecondDer( t, FirstD2r );
      MbVector3D f;
      curve->ThirdDer( t, FirstD3r );

      if ( FirstCvt != 0 )
        FirstRadius = 1.0 / FirstCvt;
      else
        FirstRadius = MB_MAXDOUBLE;
    }
    else { // Не выбрана кривая 
      outError = 2;
      return outError;
    }
  }
  else { // Кривая не NURBS
    outError = 2;
    return outError;
  }

  int spDegree = 8;
  GetInt( _T( "Степень B-сплайн кривой сопряжения 6 <= m <= 10" ), spDegree );
  if ( spDegree < 6 ) {
    spDegree = 6;
  }
  if ( spDegree > 10 ) {
    spDegree = 10;
  }
  fData.degreeBSpline = spDegree;
  int cvDirection = 0;
  GetInt( _T( "Направление сегмента: 0 - по кривой, 1 - против кривой" ), cvDirection );

  int typeSPoly = 0;
  GetInt( _T( "Вид S-полигона: 0 - полуоткрытый, 1 - открытый" ), typeSPoly );
  if ( typeSPoly < 0 ) {
    typeSPoly = 0;
  }
  if ( typeSPoly > 1 ) {
    typeSPoly = 1;
  }

  fData.outFormat = (MbeFairSplineFormat)typeSPoly;
  MbCurve3D * sPolyCurve = nullptr;
  CurveToSPolygon( curve, FirstPoint, cvDirection, fData, sPolyCurve );

  return outError;
}


///////////////////////////////////////////////////////////////////////////////
//                    Функции, использующиеся в командах.
///////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Проверка точности приближения.
// ---
int TestAccuracyHermite( MbCurve3D * pllne, MbCurve3D * curve )
{
  if ( prt != nullptr ) {
    bool closed;
    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );
    MbCartPoint3D pnt;
    for ( size_t ir = 0, number = cvrts.Count(); ir < number; ir = ir + 5 ) {
      double distanceR = curve->DistanceToPoint( pnt );
      fprintf( prt, "\nDistance = %15.12f", distanceR );
    }
    closed = pllne->IsClosed();
  }

  return 0;
}


//------------------------------------------------------------------------------
///< \ru Анализ NURBS кривой. \en Analyze the NURBS curve.
// ---
int AnalyzeNurbs( MbNurbs3D * curv,
                  int & typeControlPolygon,
                  int & nSegments,
                  size_t & curDegree )
{
  MbControlData3D cvrts;
  SArray<double> knots;
  SArray<double> wts;
  curv->GetBasisPoints( cvrts );
  curv->GetKnots( knots );
  curv->GetWeights( wts );

  //bool closed = curv->IsClosed();
  //if ( closed )
  //  return 24; // Некорректная структура данных

  size_t n = cvrts.Count();
  size_t kArray2 = n;
  size_t order = curv->GetDegree();
  curDegree = order - 1;

  // Проверка формата NURBzS
  typeControlPolygon = 2;

  if ( knots[curDegree + 2] == knots[curDegree + 3] )
    typeControlPolygon = 3;

  size_t kArray = 0;
  if ( typeControlPolygon == 3 )
    kArray = ( kArray2 - 1 ) / curDegree + 1;
  if ( typeControlPolygon == 2 )
    kArray = ( knots.Count() - 2 *curDegree );

  nSegments = (int)kArray - 1;

  return 0;
}


/// prototype: файл Sourse > Test > Make > test_computition.cpp > bool CurveRadius()
//------------------------------------------------------------------------------
// Вычислить тангенс и радиус кривизны кривой
// ---
bool CurveTangentRadius()
{
  MbItem * obj = nullptr;
  SPtr<MbCurve3D> curve;

  if ( GetGObj( IDS_SELECT_CURVE, st_Curve3D, &obj ) && GetSpaceCurve( *obj, true, curve ) ) {
    TestVariables::viewManager->ToggleObject( obj );
    MbCartPoint3D point;

    if ( GetPoint( IDS_SELECT_POINT, point ) ) {
      double t;
      //curve->NearPointProjection(point, t, false);
      t = 0.0;
      double d, k = curve->Curvature( t );
      MbVector3D b;
      b = curve->FirstDer( t );
      MbVector3D c;
      curve->BNormal( t, c );
      MbVector3D Tangent;
      curve->Tangent( t, Tangent );
      MbVector3D e;
      e = curve->SecondDer( t );
      MbVector3D f;
      f = curve->ThirdDer( t );

      if ( k != 0 )
        d = 1.0 / k;
      else
        d = MB_MAXDOUBLE;

      TestVariables::viewManager->ToggleObject( obj );

      TCHAR s[110];
      _sntprintf( s, 110, RT_CURVATURE_OF_CURVE, d, k );
      MessageBoxEx( s, IDS_RESULTS );

      double X = Tangent.x;
      double Y = Tangent.y;
      double Z = Tangent.z;
      _sntprintf( s, 110, _T( "curve tangent \n  X =  %22.15e\n  Y =  %22.15e\n  Z =  %22.15e\n" ), X, Y, Z );
      MessageBoxEx( s, IDS_RESULTS );

#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf( prt, "\nFirstDer  = %f %f %f", b.x, b.y, b.z );
      //fprintf( prt, "\nSecondDer = %f %f %f", e.x, e.y, e.z );
      //fprintf( prt, "\nThirdDer = %f %f %f", f.x, f.y, f.z );
      //
      //fprintf( prt, "\nTangent  = %f %f %f", Tangent.x, Tangent.y, Tangent.z );
      //fprintf( prt, "\nBNormal = %f %f %f", c.x, c.y, c.z );
      //fprintf( prt, "\ndiameter = %f curvature =  %f", d, k );
#endif

      return true;
    }
  }

  return false;
}


/// prototype: файл Sourse > Test > Make > test_computition.cpp > bool CurveRadius()
//------------------------------------------------------------------------------
// Выделить участок сегмента кривой / экстраполяции сегмента с S-полигоном
// ---
bool CurveToSPolygon( MbNurbs3D * curve, MbCartPoint3D point, int dir, MbFairCurveData & data, MbCurve3D *& sPolyCurve )
{
  // int outError = 0;
  //MbItem * obj = nullptr;

  // Params of curve
  int typeControlPolygon;
  int nSegments;
  size_t curDegree;

  int typeSPoly = data.outFormat;
  MbControlData3D cvrts;
  SArray<double> knots;
  SArray<double> wts;

  // Params of point
  // MbCartPoint3D point;
  double t;

  // Params of segment
  size_t i_Segm( 0 );
  double t0( 0.0 ), tk( 0.0 ), tl( 0.0 );

  // Extracted segment
  MbCurve3D * resCurve = nullptr;

  // Subdivided segment
  MbCurve3D * subCurve = nullptr;

  // Extracted 0th segment
  //MbCurve3D * extructCurve = nullptr;

  // Onesegmented curve
  MbCurve3D * oneSegmCurve = nullptr;

  // Кривая - носитель откытого S-полигона
  MbCurve3D * oneSpolyCurve = nullptr;

  // Кривая - носитель откытого S-полигона (на чертеж выводится в виде ломаной)
  // bCurve3D * sPolyCurve = nullptr;

  // Объект для удаления и повторного построения в виде S-полигона
  MbItem * curveObj = nullptr;
  // Вытащить параметры NURBS
  if ( dir != 0 ) curve->Inverse();
  curve->GetBasisPoints( cvrts );
  curve->GetKnots( knots );
  curve->GetWeights( wts );
  size_t nKnots = knots.Count();

  bool closed = curve->IsClosed();
  if ( closed )
    false; // return 24; // Некорректная структура данных

  size_t n = cvrts.Count();
  size_t kArray2 = n;
  size_t order = curve->GetDegree();
  curDegree = order - 1;

  // Проверка формата NURBzS
  typeControlPolygon = 2;

  if ( knots[curDegree + 2] == knots[curDegree + 3] )
    typeControlPolygon = 3;

  size_t kArray = 0;
  if ( typeControlPolygon == 3 )
    kArray = ( kArray2 - 1 ) / curDegree + 1;
  if ( typeControlPolygon == 2 )
    kArray = ( kArray2 - curDegree + 1 );

  nSegments = (int)kArray - 1;

#ifdef C3D_DEBUG_FAIR_CURVES
  // Печать параметров
  //if ( prt != nullptr ) {
  //  //fprintf( prt, "\n kArray2  =  %zd", kArray2 );
  //  //fprintf( prt, "\n kArray  =  %zd", kArray );
  //  //fprintf( prt, "\n nKnots  =  %zd", nKnots );
  //  //fprintf( prt, "\n nSegments  =  %d", nSegments );
  //  //fprintf( prt, "\n curDegree  =  %zd", curDegree );
  //  //fprintf( prt, "\n typeControlPolygon  =  %d", typeControlPolygon );
  //  //
  //  //for ( int ir = 0; ir < nKnots; ++ir ) {
  //  //  fprintf( prt, "\nir  = %d knots = %f ", ir, knots[ir] );
  //  //}
  //}
#endif

  // Нарисовать ломаную по узловым точкам
  //if (GetPoint(IDS_SELECT_POINT, point)) {

  // Параметры кривой при t фиксированном
  curve->NearPointProjection( point, t, false );
  //t = 0.0;
  double d, k = curve->Curvature( t );
  MbVector3D b;
  curve->FirstDer( t, b );
  MbVector3D c;
  curve->BNormal( t, c );
  MbVector3D Tangent;
  curve->Tangent( t, Tangent );
  MbVector3D e;
  curve->SecondDer( t, e );
  MbVector3D f;
  curve->ThirdDer( t, f );

  // Уточнение точки
  curve->PointOn( t, point ); //curve->GetPoint( t, point );

  if ( k != 0 )
    d = 1.0 / k;
  else
    d = MB_MAXDOUBLE;

#ifdef C3D_DEBUG_FAIR_CURVES
  //if ( prt != nullptr ) {
  //  //fprintf( prt, "\nt  = %f ", t );
  //  //fprintf( prt, "\nFirstDer  = %f %f %f", b.x, b.y, b.z );
  //  //fprintf( prt, "\nSecondDer = %f %f %f", e.x, e.y, e.z );
  //  //fprintf( prt, "\nThirdDer = %f %f %f", f.x, f.y, f.z );
  //  //
  //  //fprintf( prt, "\nTangent  = %f %f %f", Tangent.x, Tangent.y, Tangent.z );
  //  //fprintf( prt, "\nBNormal = %f %f %f", c.x, c.y, c.z );
  //  //fprintf( prt, "\ndiameter = %f curvature =  %f", d, k );
  //} //
#endif

  //TestVariables::viewManager->ToggleObject(obj);

  // Вытащить парамeтры сегмента, содержащего t
  if ( typeControlPolygon == 3 ) {
    size_t ir;
    for ( ir = 0; ir < nKnots - curDegree; ir = ir + curDegree ) {
      if ( t >= knots[ir + curDegree] && t <= knots[ir + curDegree + 1] ) {
        i_Segm = ir / curDegree;
        t0 = knots[ir + curDegree];
        tk = knots[ir + curDegree + 1];
        tl = t - t0;
        break;
      }
    }

#ifdef C3D_DEBUG_FAIR_CURVES
    //if ( prt != nullptr ) {
    //  fprintf( prt, "\nNo segment i_Segm = %zd", i_Segm );
    //  fprintf( prt, "\nt0 = %f", t0 );
    //  fprintf( prt, "\ntk = %f", tk );
    //  fprintf( prt, "\ntl = %f", tl );
    //}
#endif
  }

  if ( typeControlPolygon == 2 ) {
    size_t ir;
    for ( ir = 0; ir < nKnots - curDegree; ir++ ) {
      if ( t >= knots[ir + curDegree] && t <= knots[ir + curDegree + 1] ) {
        i_Segm = ir;
        t0 = knots[ir + curDegree];
        tk = knots[ir + curDegree + 1];
        tl = t - t0;
        break;
      }
    }

#ifdef C3D_DEBUG_FAIR_CURVES
    //if ( prt != nullptr ) {
    //  fprintf( prt, "\nNo segment i_Segm = %zd", i_Segm );
    //  fprintf( prt, "\nt0 = %f", t0 );
    //  fprintf( prt, "\ntk = %f", tk );
    //  fprintf( prt, "\ntl = %f", tl );
    //}
#endif
  }

  // Вытащить один сегмент из многогесментной кривой в виде NURBzS
  data.numSegment = i_Segm;
  data.outFormat = (MbeFairSplineFormat)3; // typeControlPolygon;
  //data.outFormat = (MbeFairSplineFormat)typeControlPolygon;
  data.nSegments = 1;

  if ( ExtractChangeNurbs( curve, data, resCurve ) == 0 ) {
    Style style = BSPLINE_Style;

  }
  else {
    MessageBoxEx( _T( "Не выделен сегмент с точкой сопряжения" ), IDS_TITLE_ERR );
    return false;
  }

  // Вставить узел в один сегмент
  double tParam = tl / ( tk - t0 ); // 

  data.numSegment = 0;
  data.tParam = tParam;

  //if (dir != 0) {
  //  resCurve->Inverse();
  //  tParam = 1. - tParam;
  //}

  MbNurbs3D * nurbsCurve = dynamic_cast<MbNurbs3D *>( resCurve );

  if ( AddKnotNurbs( nurbsCurve, data, subCurve ) == 0 ) {
    Style style = BSPLINE_Style;
    ////viewManager->AddObject(style, subCurve); // на чертеж не выводится.
    //MessageBoxEx( _T( "Сегмент сопряжения разбит на два сегмента" ), IDS_TITLE_ALL_RIGHT );
  }
  else {
    MessageBoxEx( _T( "Сегмент сопряжения не разбит на два сегмента" ), IDS_TITLE_ERR );
    return false;
  }

  // Анализ на соотношение параметров.
  if ( tl < 0.5 ) { // Берем второй сегмент.
    // Вытащить 1-ой сегмент в формате NURBzS  
    MbNurbs3D * extractNurbs = dynamic_cast<MbNurbs3D *>( subCurve );
    data.numSegment = 1;
    data.outFormat = (MbeFairSplineFormat)3; // typeControlPolygon;
    //data.outFormat = (MbeFairSplineFormat)typeControlPolygon;
    data.nSegments = 1;
    if ( ExtractChangeNurbs( extractNurbs, data, oneSegmCurve ) == 0 ) {
      Style style = BSPLINE_Style;
      //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
      ////viewManager->AddObject(style, extructCurve); //Не рисуется
      //MessageBoxEx( _T( "Выделен первый (1th) подсегмент" ), IDS_TITLE_ALL_RIGHT );
    }
    else {
      MessageBoxEx( _T( "Не выделен первый (1th)подсегмент" ), IDS_TITLE_ERR );
      return false;
    }
  }

  if ( tl > 0.5 ) { // Берем первый сегмент. И отображаем в направлении параметра от точки сопряжения.
    // Вытащить 0-ой сегмент в формате NURBzS  
    MbNurbs3D * extractNurbs = dynamic_cast<MbNurbs3D *>( subCurve );
    data.numSegment = 0;
    data.outFormat = (MbeFairSplineFormat)3; // typeControlPolygon;
    //data.outFormat = (MbeFairSplineFormat)typeControlPolygon;
    data.nSegments = 1;
    if ( ExtractChangeNurbs( extractNurbs, data, oneSegmCurve ) == 0 ) {
      Style style = BSPLINE_Style;
      //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
      ////viewManager->AddObject(style, extructCurve); //Не рисуется
      //MessageBoxEx( _T( "Выделен нулевой (0th) подсегмент" ), IDS_TITLE_ALL_RIGHT );
    }
    else {
      MessageBoxEx( _T( "Не выделен нулевой (0th)подсегмент" ), IDS_TITLE_ERR );
      return false;
    }

    // Отображаем в направлении параметра от точки сопряжения.
    MbNurbs3D * oneSegmNurbs = dynamic_cast<MbNurbs3D *>( oneSegmCurve );
    data.tParam = 2.; // Экстраполируем
    data.numSegment = 1;  // Выбираем второй (1th) сегмент 
    data.outFormat = (MbeFairSplineFormat)3; // в NURBzS;
    data.nSegments = 1;

    // Делит и сразу вытаскивает numSegment-ный сегмент NURBzS кривой. Может экстраполировать при tParam > 1.
    if ( ExtractExtrapolFromSegment( oneSegmNurbs, data, oneSegmCurve ) == 0 ) {
      Style style = BSPLINE_Style;
      //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
      ////viewManager->AddObject(style, oneSegmCurve); // Не рисуется
      //MessageBoxEx( _T( "Сегмент экстраполирован" ), IDS_TITLE_ALL_RIGHT );
    }
    else {
      MessageBoxEx( _T( "Сегмент не экстраполирован" ), IDS_TITLE_ERR );
      return false;
    }
    // Инвертировать кривую 
    oneSegmCurve->Inverse();
  }

  // Поднять степень до заданной
  size_t mDegree = data.degreeBSpline; // Принимаем заданную степень
  // data.degreeBSpline >= 6;
  size_t d_up = mDegree - curDegree;
  if ( curDegree == 3 )
    d_up = mDegree - curDegree - 2; // С 3 сразу до 6

  for ( size_t ir = 0; ir < d_up; ir++ ) {
    MbNurbs3D * oneSegmNurbs = dynamic_cast<MbNurbs3D *>( oneSegmCurve );

    if ( ElevateDegreeNurbzs( oneSegmNurbs, data, oneSegmCurve ) == 0 ) {
      Style style = BSPLINE_Style;
      ////viewManager->AddObject(style, resCurve); // Не рисовать.
      //MessageBoxEx( _T( "Кривая с повышением степени" ), IDS_TITLE_ALL_RIGHT );
    }
    else {
      MessageBoxEx( _T( "Ошибка при повышении степени" ), IDS_TITLE_ERR );
    }
  }

  // Преобразовать сегмент в кривую-ломаную в виде полуоткрытого S-полигона
  MbNurbs3D * oneSegmNurbs = dynamic_cast<MbNurbs3D *>( oneSegmCurve );
  data.numSegment = 0;
  data.outFormat = (MbeFairSplineFormat)0; // typeControlPolygon;
  //data.outFormat = (MbeFairSplineFormat)typeControlPolygon;
  data.nSegments = 1;
  if ( ExtractChangeNurbs( oneSegmNurbs, data, sPolyCurve ) == 0 ) {
    Style style = BSPLINE_Style;
    //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
    //viewManager->AddObject(style, extructCurve);
    curveObj = TestVariables::viewManager->AddObject( style, sPolyCurve );  // S-полигон рисуется на чертеже
    MessageBoxEx( _T( "Создан S-полигон нулевого сегмента" ), IDS_TITLE_ALL_RIGHT );
  }
  else {
    MessageBoxEx( _T( "Не создан S-полигон нулевого сегмента" ), IDS_TITLE_ERR );
    return false;
  }

  //Циклы масштабирования S-полигона. Подгонка под сопряжение.
  tParam = 0.5;
  oneSegmNurbs = dynamic_cast<MbNurbs3D *>( oneSegmCurve );

  for ( size_t ir = 0; ; ir++ ) {
    if ( GetDouble( _T( "Укажите значение параметра t: (0.1 <= t < ...)" ), tParam ) ) {  // от размера текущего S-полигона 
      // при t < 1 уменьшает, при t > 1 увеличивает.
      TestVariables::viewManager->DeleteObject( curveObj ); // Стереть рисунок S-полигона

      data.tParam = tParam;
      data.numSegment = 0;
      data.outFormat = (MbeFairSplineFormat)3; // в NURBzS;
      data.nSegments = 1;
      // Делит и сразу вытаскивает numSegment-ный сегмент NURBzS кривой. Может экстраполировать при tParam > 1.
      if ( ExtractExtrapolFromSegment( oneSegmNurbs, data, oneSegmCurve ) == 0 ) {
        Style style = BSPLINE_Style;
        //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
        ////viewManager->AddObject(style, oneSegmCurve); // Не рисуется
        //MessageBoxEx( _T( "Сегмент масштабирован" ), IDS_TITLE_ALL_RIGHT );
      }
      else {
        MessageBoxEx( _T( "Сегмент не масштабирован" ), IDS_TITLE_ERR );
        return false;
      }

      // Определить S-полигон на сегменте
      oneSegmNurbs = dynamic_cast<MbNurbs3D *>( oneSegmCurve );
      data.numSegment = 0;
      data.outFormat = (MbeFairSplineFormat)typeSPoly; // Преобразовать в заданный S-полигон
      data.nSegments = 1;

      if ( ExtractChangeNurbs( oneSegmNurbs, data, oneSpolyCurve ) == 0 ) {
        Style style = BSPLINE_Style;
        //if (data.outFormat == (MbeFairSplineFormat)1)style = st_Polyline3D;
        curveObj = TestVariables::viewManager->AddObject( style, oneSpolyCurve );
        //MessageBoxEx( _T( "S-полигон кривой нарисован" ), IDS_TITLE_ALL_RIGHT );
      }
      else {
        MessageBoxEx( _T( "S-полигон кривой не нарисован" ), IDS_TITLE_ERR );
        return false;
      }
    }
    else
      break;  // Рисунок S-полигона остается 
  }

  sPolyCurve = oneSegmCurve;  // Кривая со всеми параметрами
  Style style = BSPLINE_Style;
  viewManager->AddObject( style, oneSegmCurve );
  return true;
}


//---------------------------------------------------------------------------------
// Определить касательные к точкам на окружности.
// ---
int TangentsOnCirle( SArray<MbCartPoint3D> & polyLine,
                     SArray<MbVector3D> & tangents,
                     double & cvt )
{
#ifdef C3D_DEBUG_FAIR_CURVES
  //fprintf(prt, "\n Begin TangentsOnCirle \n");
  //for (size_t ir = 0; ir < 3; ++ir) {
  //
  //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f",
  //    ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
  //}
#endif

  int error = 0;
  MbCartPoint3D pm01; // = (polyLine[0] + polyLine[1]) / 2.;
  MbCartPoint3D pm12; // = (polyLine[1] + polyLine[1]) / 2.;
  for ( size_t mr = 0; mr < 3; ++mr ) {
    pm01[mr] = ( polyLine[0][mr] + polyLine[1][mr] ) / 2.;
    pm12[mr] = ( polyLine[1][mr] + polyLine[2][mr] ) / 2.;
  }

  MbVector3D v01; v01.Init( polyLine[0], polyLine[1] );
  v01.Normalize();
  MbVector3D v12; v12.Init( polyLine[1], polyLine[2] );
  v12.Normalize();
  MbVector3D ort = v01 | v12;
  if ( fabs( ort.Length() ) <= METRIC_ACCURACY ) {
    // Точки на прямой
    tangents[0] = v01;
    tangents[1] = v01;
    tangents[2] = v01;
    cvt = 0.;
    return error;
  }
  // Нормальная конфигурация точек.
  ort.Normalize();
  MbVector3D tng1;
  tng1 = ort | v01;
  MbVector3D tng2;
  tng2 = ort | v12;
  MbCartPoint3D pCentre;
  tng2 = -tng2;
  //DEBUG20211217 BEGIN
  //error = IntersectLines( pm01, pm12, tng1, tng2, pCentre );
  error = LineIntersectIn3DTest(pm01,  tng1, pm12, tng2, pCentre);
  //DEBUG20211217 END
  if ( error > 0 )return error;
  MbVector3D vRadius; vRadius.Init( pCentre, polyLine[0] );
  double radius = vRadius.Length();
  cvt = 1. / radius;
  tangents[0] = ort | vRadius;
  tangents[0].Normalize();
  vRadius.Init( pCentre, polyLine[1] );
  tangents[1] = ort | vRadius;
  tangents[1].Normalize();
  vRadius.Init( pCentre, polyLine[2] );
  tangents[2] = ort | vRadius;
  tangents[2].Normalize();

#ifdef C3D_DEBUG_FAIR_CURVES
  //fprintf(prt, "\n End TangentsOnCirle \n");
  //for (size_t ir = 0; ir < 3; ++ir) {
  //
  //  fprintf(prt, "\npolyLine ir = %d XYZ = %f %f %f",
  //    ir, polyLine[ir][0], polyLine[ir][1], polyLine[ir][2]);
  //  fprintf(prt, "\ntangents ir = %d XYZ = %f %f %f",
  //    ir, tangents[ir][0], tangents[ir][1], tangents[ir][2]);
  // 
  //
  //}
#endif
  return error;

}


//---------------------------------------------------------------------------------
// Определить касательную ко второй точке на окружности.
// Касательная в первой точке задана.
// ---

int TangentOnSecondPoint( const SArray<MbCartPoint3D> & polyLine,
                          SArray<MbVector3D> & tangents,
                          double & cvt )
{
  MbCartPoint3D pm0( polyLine[0] ), pm01;
  // pm01 = ( polyLine[0] + polyLine[1] ) / 2
  pm01.Set( polyLine[0], 0.5, polyLine[1], 0.5 ); // pm01.x = v1.x * 0.5 + v2.x * 0.5; pm01.y = v1.y * 0.5 + v2.y * 0.5; pm01.z = v1.z * 0.5 + v2.z * 0.5; 

  MbVector3D v0; v0 = tangents[0];
  v0.Normalize();
  MbVector3D v01; v01.Init( polyLine[0], polyLine[1] );
  v01.Normalize();
  MbVector3D ort = v0 | v01;
  if ( ort.Length() <= METRIC_ACCURACY ) {
    // Точки на прямой
    tangents[0] = v01;
    tangents[1] = v01;
    //tangents[2] = v01;
    cvt = 0.;
    return outErrors;
  }
  // Нормальная конфигурация точек.
  ort.Normalize();
  MbVector3D tng1;
  tng1 = ort | v0;
  MbVector3D tng2;
  tng2 = ort | v01;
  MbCartPoint3D pCentre;

  MbVector3D ort1 = tng1 | v01;
  MbVector3D ort2 = v01 | tng2;
  if ( ort1 * ort2 < 0. )  tng2 = -tng2;

  outErrors = IntersectLines( pm0, pm01, tng1, tng2, pCentre );
  if ( outErrors > 0 )return outErrors;
  MbVector3D vRadius; vRadius.Init( pCentre, polyLine[0] );
  double radius = vRadius.Length();
  cvt = 1. / radius;
  //tangents[0] = ort | vRadius;
  //tangents[0].Normalize();
  vRadius.Init( pCentre, polyLine[1] );
  tangents[1] = ort | vRadius;
  tangents[1].Normalize();
  //vRadius.Init(pCentre, polyLine[2]);
  //tangents[2] = ort | vRadius;
  //tangents[2].Normalize();
  return outErrors;

}


//------------------------------------------------------------------------------
/** \brief \ru Создать ГО Эрмита по массивам точек, касательных, кривизн.
\en Create Hermite GD on arrays of points, tangents, curvftures. \~
*/
//---
int CreateHermiteGDOnStructure(  SArray <MbCartPoint3D> & polyLine,
                                 SArray <MbVector3D> & tangents,
                                 SArray <double> & curvatures,
                                 size_t kArray,
                                 MbCurve3D *& pllne ) // Результат
{
  SArray <int>  arrayInfectionSign;
  SArray <int>  arrayStraightSign;
  SArray <MbVector3D>  arrayVectorCurvature;

  arrayInfectionSign.resize(kArray);
  arrayStraightSign.resize(kArray);
  arrayVectorCurvature.resize(kArray);

  // Структуризация
  StructToGDH( polyLine, tangents, curvatures, kArray, arrayInfectionSign, arrayStraightSign, arrayVectorCurvature );

  MbCartPoint3D pnt, ptng, pcvt;
  SArray<MbCartPoint3D> pnts;
  double rl = 0.;

  for ( size_t ir = 0; ir < kArray; ir++ ) {
    pnt = polyLine[ir];
    pnts.Add( pnt );

    if ( ir < kArray - 1 ) {
      rl = polyLine[ir + 1].DistanceToPoint( polyLine[ir] );
    }

    ptng = pnt + tangents[ir] * rl;
    pnts.Add( ptng );
    pnts.Add( pnt );

    // Установить точку вектора кривизны.
    arrayVectorCurvature[ir].Normalize();
    pcvt = pnt + arrayVectorCurvature[ir] * curvatures[ir];
    // Обмануть C3D по кратным точкам ломаной при нулевой кривизне. C3D не допускает трех кратных точек
    if ( curvatures[ir] < METRIC_EPSILON ) {
      arrayVectorCurvature[ir] = tangents[ir] * rl * 0.5;
      pcvt = pnt + arrayVectorCurvature[ir];
    }

    pnts.Add( pcvt );
    pnts.Add( pnt );
  }

#ifdef C3D_DEBUG_FAIR_CURVES
  int n = (int)pnts.size();
  //fprintf(prt, "\nPre SplineCurve(pnts, plClosed, st_Polyline3D, pllne) n =  %d\n", n);
  //for (int ir = 0; ir < n; ++ir) {
  //
  //  fprintf(prt, "ir = %d pnt = %f %f %f\n", ir, pnts[ir].x, pnts[ir].y, pnts[ir].z);
  //}
#endif
  // Создать ломаную 
  bool plClosed = false;
  ::SplineCurve( pnts, plClosed, st_Polyline3D, pllne );


  arrayInfectionSign.HardFlush();
  arrayStraightSign.HardFlush();
  arrayVectorCurvature.HardFlush();
  return outErrors;
}


//----------------------------------------------------------------------------------------
///< \ru Структурировать массивы pnts tngs cvts в виде. \en To structure a pnts tngs cvts.
// ---
bool StructToGDH( SArray <MbCartPoint3D> & polyLine,
                  SArray <MbVector3D> & tangents,
                  SArray <double> & curvatures,
                  size_t kArray,
                  // Результат
                  SArray <int> & arrayInfectionSign,
                  SArray <int> & arrayStraightSign,
                  SArray <MbVector3D> & vectorCurvature )
{
  /* Установить  векторы кривизны */
  for ( size_t i = 0; i < kArray - 1; ++i ) {
    // Дифференциальные характеристики

    MbVector3D v;
    v.Init( polyLine[i], polyLine[i + 1] );
    MbVector3D vOrt = tangents[i] | v;
    MbVector3D  vCvt = vOrt | tangents[i];

    vCvt.Normalize();
    if ( curvatures[i] > METRIC_EPSILON ) {
      vectorCurvature[i] = curvatures[i] * vCvt;
    }
    if ( curvatures[i] <= METRIC_EPSILON ) {
      vectorCurvature[i] = tangents[i];
    }
    if ( i == kArray - 2 ) { // Обработка последней точки
      vOrt = tangents[i + 1] | v;
      vCvt = vOrt | tangents[i + 1]; // v;
      vCvt.Normalize();
    }
    if ( curvatures[i + 1] > METRIC_EPSILON ) { // Ненулевая кривизна 
      vectorCurvature[i + 1] = curvatures[i + 1] * vCvt;
    }
    if ( curvatures[i + 1] <= METRIC_EPSILON ) { // Нулевая кривизна
      vectorCurvature[i + 1] = tangents[i + 1];
    }

  } // Обход ломаной

    // Сформировать признаки прямолинейности и перегиба

  for ( size_t ir = 0; ir < kArray; ir++ ) {
    arrayInfectionSign[ir] = 0;
    arrayStraightSign[ir] = 0;
  }

  for ( size_t ir = 0; ir < kArray - 1; ir++ ) {
    arrayInfectionSign[ir] = 0;
    if ( curvatures[ir] < METRIC_EPSILON && curvatures[ir + 1] > METRIC_EPSILON ) {
      arrayInfectionSign[ir] = 2;
      curvatures[ir] = 0.;
    }
    if ( curvatures[ir] < METRIC_EPSILON &&
         curvatures[ir + 1] < METRIC_EPSILON &&
         ir < kArray - 1 ) {
      arrayStraightSign[ir] = 1;
      arrayStraightSign[ir + 1] = 1;
      curvatures[ir] = 0.;
      curvatures[ir + 1] = 0.;
    }
  }

  return true;
}


//------------------------------------------------------------------------------
//
// ---
int TestAccuracyOld( MbCurve3D * pllne, MbCurve3D * curve )
{
  if ( prt != nullptr ) {
    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );
    size_t number = cvrts.Count();
    MbCartPoint3D pnt;
    for ( size_t ir = 0; ir < number; ir++ ) {
      double distanceR = curve->DistanceToPoint( pnt );
      //fprintf(data.prt, "\nDistance = %f", distance);
      fprintf( prt, "\nNo = %zd Distance =  %15.12f", ir, distanceR );
    }
  }

  return 0;
}


//------------------------------------------------------------------------------
//  Параметр accuracy:
//    погрешность в первой точке,
//    погрешность в последней точке,
//    максимальная погрешность
// ---
int TestAccuracy( const MbCurve3D * pllne, const MbCurve3D * curve, std::array<double, 3>* accuracies )
{
  if( curve != nullptr && pllne != nullptr
#ifdef C3D_DEBUG_FAIR_CURVES
    && prt != nullptr
#endif
      ) {
    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );
    size_t number = cvrts.Count();
    size_t step = 1;
    if ( ( number / 5 ) * 5 == number ) { // Кажется ГО Эрмита
      MbCartPoint3D pnt0 = cvrts[0];
      MbCartPoint3D pnt4 = cvrts[4];
      if ( fabs( pnt0.x - pnt4.x ) < 0.0000001 &&
           fabs( pnt0.y - pnt4.y ) < 0.0000001 &&
           fabs( pnt0.z - pnt4.z ) < 0.0000001 )
        step = 5; // Точно ГО Эрмита
    }
    MbCartPoint3D pnt;
    double t;  //внутренний параметр
    MbCartPoint3D point; // Точка на кривой.
    double sumDelta = 0.;  // Суммарная абсолютная  погрешность. 
    double sumvDelta = 0.; // Сумма векторов отеклонений.
    double maxDelta = 0.;  // Максимальное отеклонение.
    MbVector3D vOrtGlobal = { 0.,0.,1. };
    double maxAcc = MB_MINDOUBLE;
    for ( size_t ir = 0; ir < number; ir = ir + step ) {
      pnt = cvrts[ir];

      // Определить расстояние от точки до кривой.
      double distanceR = curve->DistanceToPoint( pnt );
      if ( accuracies != nullptr ) {
        if ( ir == 0 )
          (*accuracies )[0] = distanceR;
        else if ( ir == number - 1 )
          (*accuracies )[1] = distanceR;
      }
      if ( distanceR > maxAcc )
        maxAcc = distanceR;
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nNo = %d Distance =  %15.12fn", ir, distanceR);
#endif
      curve->NearPointProjection( pnt, t, false ); //Определить параметр

      // Параметры кривой при t фиксированном.

      //double d, curvature = curve->Curvature( t ); - // Не используются.
      MbVector3D firstDer;
      curve->FirstDer( t, firstDer );
      MbVector3D biNormal;
      curve->BNormal( t, biNormal );
      MbVector3D tangent;
      curve->Tangent( t, tangent );
      MbVector3D secondDer;
      curve->SecondDer( t, secondDer );
      MbVector3D thirdDer;
      curve->ThirdDer( t, thirdDer );

#ifdef C3D_DEBUG_FAIR_CURVES
//    if ( prt != nullptr ) {
//      fprintf( prt, "\nt  = %f ", t );
//      fprintf( prt, "\nFirstDer  = %f %f %f", firstDer.x, firstDer.y, firstDer.z );
//      fprintf( prt, "\nSecondDer = %f %f %f", secondDer.x, secondDer.y, secondDer.z );
//      fprintf( prt, "\nThirdDer = %f %f %f", thirdDer.x, thirdDer.y, thirdDer.z );
//        
//      fprintf( prt, "\nTangent  = %f %f %f", tangent.x, tangent.y, tangent.z );
//      fprintf( prt, "\nBNormal = %f %f %f", biNormal.x, biNormal.y, biNormal.z );
//      fprintf( prt, "\ncurvature =  %f",  curvature );
//    } 
#endif

      // Уточнение точки
      curve->PointOn( t, point ); //curve->GetPoint( t, point );
      // Вектор погрешности
      MbVector3D vDelta;
      vDelta.Init( pnt, point );
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nNo = %d pnt = %15.12f,  %15.12f,  %15.12f", ir, pnt[0], pnt[1], pnt[2]);
      //fprintf(prt, "\nNo = %d point = %15.12f,  %15.12f,  %15.12f", ir, point[0], point[1], point[2]);
      //fprintf(prt, "\nNo = %d vDelta = %15.12f,  %15.12f,  %15.12f", ir, vDelta[0], vDelta[1], vDelta[2]);
#endif

      // Знак отклонения
      MbVector3D vOrt;
      vOrt = firstDer | vDelta;
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(prt, "\nNo = %d vOrt = %15.12f,  %15.12f,  %15.12f", ir, vOrt[0], vOrt[1], vOrt[2]);
#endif
      if ( biNormal * vOrt < 0. ) distanceR = distanceR * -1.0;
#ifdef C3D_DEBUG_FAIR_CURVES
      firstDer.Normalize();
      fprintf( prt, "No = %d  oriented Distance =  %15.12f pnt = %f,%f,%f tng = %f,%f,%f \n", 
               ir, distanceR, pnt[0], pnt[1], pnt[2], firstDer[0], firstDer[1], firstDer[2]);
#endif
      sumDelta = sumDelta + fabs( distanceR );
      sumvDelta = sumvDelta + distanceR;
      if ( maxDelta < fabs( distanceR ) )maxDelta = fabs( distanceR );
    }
    if ( accuracies != nullptr )
      (*accuracies )[2] = maxAcc;

#ifdef C3D_DEBUG_FAIR_CURVES
    fprintf( prt, "\nsumDelta =  %15.12f\n", sumDelta );
    fprintf( prt, "\nsumvDelta =  %15.12f\n", sumvDelta );
    fprintf( prt, "\nmaxDelta =  %15.12f\n", maxDelta );
#endif
  }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//                    Вспомогательные функции.
///////////////////////////////////////////////////////////////////////////////


// Предварительные объявления.
size_t AnalizeCmps( char stroke[], char * p[], int max_p, size_t & fact );
int WrBSurfaceToGB();
int WriteSplgnToGB( MbCurve3D * pllne );
int WriteSplgnToCmpsGB( MbCurve3D * pllne, size_t j );
int RdNurbsSFromGB();
int ReadPllneFromGeomBuffer();
int WritePllneGB();
int ReadPllneGB();
#ifdef FAIR_CURVE_ADVANCED
int WriteNURBSGB();
#endif

//------------------------------------------------------------------------------
// Читать NURBS кривую. \en Read the NURBS curve.
// ---
int RdNurbs( MbNurbs3D *& curv )
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

      //double length = curv->GetParamLength();
      //bool closed = curv->IsClosed();
      //DEBUG20230125 Begin
      //if ( closed )
      //  return 24; // Некорректная структура данных;
      //DEBUG20230125 End
      size_t curDegree = curv->GetDegree() - 1;

      // Проверка формата NURBzS
      int typeControlPolygon = 2;

      if ( knots[curDegree + 2] == knots[curDegree + 3] )
        typeControlPolygon = 3;

#ifdef C3D_DEBUG
      size_t nDots;
      size_t n = cvrts.Count();
      if ( typeControlPolygon == 3 )
        nDots = ( n - 1 ) / curDegree + 1;
      if ( typeControlPolygon == 2 )
        nDots = ( n - curDegree + 1 );
#endif // C3D_DEBUG

      return 0;
    }
    else {
      MessageBoxEx( _T( "Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 24; // Некорректная структура данных.
    }
  }

  return 24; // Некорректная структура данных
}


//----------------------------------------------------------------------------------------
// ---
int Rd3DMeshFromCmps( MbFairCurveData & data )
{
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;
  SArray<MbCartPoint3D> array_pnts;
  Style style = TestVariables::BSPLINE_Style;
  bool closed = false; // Только незамкнутый формат выходе

  size_t nVertex;
  FILE * in;
  size_t alt, n, j, mr;

  char * p[20]; // указатели на числа в строке
  double xyz[3];
  //int outErrors = 0;

  char Path_File[] = "C:/FairCurveModeler_TEMP/Temp/3DMesh_cmps.txt";

  char stroke[128];

  size_t k_array = 0;
  int l_array = 0;
  int i = 0;
  int i_end = 0;
  int iMesh = true; // Проверка сети на корректность.

  if ( ( in = fopen( Path_File, "r" ) ) == nullptr ) {
    outErrors = 3;
    return outErrors;
  }
  array_pnts.clear(); // Хранилище всех точек.
  if ( fgets( stroke, 120, in ) == nullptr ) {
    outErrors = 2;  // Пустой файл;
    fclose( in );
    return outErrors;
  }
  alt = AnalizeCmps( stroke, p, 3, n );
  if ( n > 1 ) {  // не 3D сеть. Только одна переменная с номером ряда.
    fclose( in );
    outErrors = 2;
    return outErrors;
  }
  if ( fgets( stroke, 120, in ) == nullptr ) { // Считать реальную точку.
    outErrors = 2;  // Пустой файл;
    fclose( in );
    return outErrors;
  }
  for ( i = 0; ; ++i ) {
    pnts.clear();
    j = 0;

    for ( ; ;) { // Циклы по точкам ряда.
      alt = AnalizeCmps( stroke, p, 3, n );

      if ( n < 3 ) {
        break;  // Переход к реальной точке. Это номер ряда.
      }
      // запись чисел
      if ( n > 2 ) {
        for ( mr = 0; mr < 3; ++mr ) {
          xyz[mr] = atof( p[mr] );
        }
        pnt.x = xyz[0];
        pnt.y = xyz[1];
        pnt.z = xyz[2];

        pnts.Add( pnt );
        array_pnts.Add( pnt );
        ++j; // Новая точка ряда.

      }
      else {
        //outErrors = 6;
        //fclose(in);
        outErrors = 2;
        break;
      }

      if ( alt == 2 )
        break;

      if ( fgets( stroke, 120, in ) == nullptr ) { // Следующая строка с точкой.
        // Конец таблицы точек.
        i_end = 1;
        break;
      }
    } // Циклы по точкам ряда

    // Нарисовать ломаную.
    nVertex = j;
    if ( i == 0 ) k_array = j;
    if ( i > 0 && j != k_array ) iMesh = false;

    if ( nVertex > 1 && outErrors == 0 ) { // Ломаная считана
                                         // Нарисовать ломаную S-полигона
      MbCurve3D * result( nullptr );
      ::SplineCurve( pnts, closed, st_Polyline3D, result );
      TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
    }
    if ( i_end == 1 )
      break;
    if ( fgets( stroke, 120, in ) == nullptr ) { // Следующая строка с точкой.
                                              // Конец таблицы точек.
      i_end = 1;
      break;
    }

  } // Циклы по i

  l_array = i + 1;
  fclose( in );

  // Построить поверхность степеней 1 * 1.
   //---------------------------------------------------------------------------------
   // Прямое определение Nurbs поверности на точках сети.
   // ---
   //int PllnesToNurbsSurface11(
   //  MbFairCurveData & curveData,
   //  SArray<MbCartPoint3D> pnts,
   //  int kArray,
   //  int lArray)
  MbeFairSplineFormat initFormat = fairFormat_Open;
  if ( iMesh )
    PllnesToNurbsSurface11( data, array_pnts, k_array, l_array, initFormat );
  else
    MessageBoxEx( _T( "Каркас ломаных не сеть" ), IDS_TITLE_WRN ); // Выводит сообщение.

  return outErrors;
}


//---------------------------------------------------------------------------------
// Утилиты (utilities). Обмен данными с Геометрическим Буфером.
// Регуляризация ломаной. Редактирование ГО Эрмита. 
//---
#ifdef FAIR_CURVE_ADVANCED
int Utilites()
{
  MbFairCurveData curveData;
  int select = 1;
  GetInt( _T( "1- Обмен с ГБ, 2- Регуляризация ломаной, 3 - Редактировать ГО Эрмита, 4 - графики кривизны" ), select );
  switch ( select ) {
    case 1: // Обмен с Геометрическим  Буфером
      ExchangeGB();
      break;
    case 2: // Регуляризация ломаной.
      Test_GeomPreProcessor();
      break;

    case 3: // Редактировать ГО Эрмита.
      
      Test_EditHermiteGD();
      break;
    case 4: // Отображение графиков кривизны.
      Test_CreateEvoluteCurvatureOnCurve();
      break;
  }
  if ( outErrors == 0 )
    MessageBoxEx( _T( "Операция выполнена" ), IDS_TITLE_ALL_RIGHT );

  return 0;
}


//---------------------------------------------------------------------------------
// Утилиты (utilities). Обмен данными с Геометрическим Буфером.
//---
int ExchangeGB()
{
  MbFairCurveData curveData;
  int select = 1;
  int sel = 1;
  GetInt( _T( "1- читать ГО, 2- читать NURBS, 3 - записать ГО, 4 - записать NURBS, 5 - редактировать ГО Эрмита, 6 - графики кривизны" ), select );
  switch ( select ) {
    case 1: // Читать ломаную из Буфера
      //ReadPllneGB( );

      GetInt( _T( "1- читать ГО из таблицы WEB, 2- читать ГО из таблицы, 3 - читать 3D сеть из таблицы" ), sel );
      switch ( sel ) {
        case   1:
          ReadPllneFromGeomBuffer();
          break;
        case   2:
          ReadPllneFromGeomBufferCompas();
          break;

        case 3: // Читать сеть из таблицы
          Rd3DMeshFromCmps( curveData );
          break;
      }
      break;
    case 2: // Читать NURBS ГеомБуфера
      RdNurbsSFromGB();
      break;

    case 3: // Записать ломаную в ГеомБуфер
      GetInt( _T( "1- записать ломаную, 2- записать сеть" ), sel );
      switch ( sel ) {
        case   1:
          WritePllneGB();
          break;
        case   2:
          WrBSurfaceToGB();
          break;
      }
      break;
    case 4: // Читать NURBS ГеомБуфера
      WriteNURBSGB();
      break;
    case 5: // 5 - редактировать ГО Эрмита
      Test_EditHermiteGD();
      break;
    case 6: // 6 - отображение графиков кривизны.
#ifdef FAIR_CURVE_ADVANCED
      Test_CreateEvoluteCurvatureOnCurve();
#endif
      break;
  }
  if ( outErrors == 0 )
    MessageBoxEx( _T( "Операция выполнена" ), IDS_TITLE_ALL_RIGHT );

  return 0;
}
#endif


//---------------------------------------------------------------------------------- 
// Анализ строки с параметрами.
//---
size_t AnalizeCmps( char stroke[],
                    char * p[],
                    int max_p,
                    size_t & fact )
{
  size_t alt, n, i, mr, sw;

  char * c;    /* указатель на символ */
  mr = 0;

  n = strlen( stroke );
  sw = 0;
  alt = 0;

  // анализ строки 
  for ( i = 0; i < n; ++i ) {
    if ( (int)mr > max_p )
      break;

    c = &stroke[i];

    switch ( stroke[i] ) {
      case '(':
        /*  начало строки */
        stroke[i] = ' ';
        sw = 0;
        break;

      case ')':
        /* окончание строки  */
        stroke[i] = '\0';
        sw = 0;
        ++alt;
        break;

      case ' ':
        if ( sw == 1 ) {
          // окончание текста числа 
          stroke[i] = '\0';
          sw = 0;
        }
        break;

      case '\"':
        if ( sw == 1 ) {
          // окончание текста числа 
          stroke[i] = '\0';
          sw = 0;
        }
        else {
          stroke[i] = ' ';
          sw = 0;
        }
        break;

      case '\n':  // окончание строки 
        if ( sw == 1 ) {
          // забить пробелом 
          stroke[i] = ' ';
        }
        else {
          stroke[i] = '\0';
          sw = 0;
        }
        break;
      case '\t':  // знак табуляции - переход к числу 
                  // игнор
        break;

      default:  if ( sw == 0 ) {
        p[mr] = c;
        sw = 1;
        ++mr;
      }
             break;
    }
  }

  fact = mr;

  return alt;
}


//---------------------------------------------------------------------------------- 
// Анализ строки с параметрами.
//---
size_t AnalizeLine( char stroke[],
                    char * p[],
                    int max_p,
                    size_t & fact )
{
  size_t alt, n, i, mr, sw;

  char * c;    /* указатель на символ */
  mr = 0;

  n = strlen( stroke );
  sw = 0;
  alt = 0;

  // анализ строки 
  for ( i = 0; i < n; ++i ) {
    if ( (int)mr > max_p )
      break;

    c = &stroke[i];

    switch ( stroke[i] ) {
      case '(':
        /*  начало строки */
        stroke[i] = ' ';
        sw = 0;
        break;

      case ')':
        /* окончание строки  */
        stroke[i] = '\0';
        sw = 0;
        ++alt;
        break;

      case ' ':
        if ( sw == 1 ) {
          // окончание текста числа 
          stroke[i] = '\0';
          sw = 0;
        }
        break;

      case '\"':
        if ( sw == 1 ) {
          // окончание текста числа 
          stroke[i] = '\0';
          sw = 0;
        }
        else {
          stroke[i] = ' ';
          sw = 0;
        }
        break;

      case '\n':  // окончание строки 
        if ( sw == 1 ) {
          // забить пробелом 
          stroke[i] = ' ';
        }
        else {
          stroke[i] = '\0';
          sw = 0;
        }
        break;

      default:  if ( sw == 0 ) {
        p[mr] = c;
        sw = 1;
        ++mr;
      }
             break;
    }
  }

  fact = mr;

  return alt;
}

//---------------------------------------------------------------------------------
// Записать S-frame B-поверхности в файл pnt_name_rmps.txt
// ---
int WrBSurfaceToGB()
{
  MbSplineSurface * nurbsSurface = nullptr;
  MbControlData3D  cvrts;

  Array2<MbCartPoint3D> surfVrts;
  //MbCurve3D * resCurve = nullptr;
  SArray <double> uKnots;
  SArray <double> vKnots;

  int outError = 0;
  MbItem * obj( nullptr );

  size_t uOrder;
  size_t uDegree;
  size_t vOrder;
  size_t vDegree;
  size_t uCount;
  size_t vCount;
  //size_t nrr = 0;

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
                                                //int n = cvrts.Count();                      // Общее количество вершин
    uOrder = nurbsSurface->GetUDegree();        // Порядок по U
    uDegree = uOrder - 1;
    vOrder = nurbsSurface->GetVDegree();
    vDegree = vOrder - 1;

    uCount = nurbsSurface->GetPointsUCount();   // Количество вершин по U
    vCount = nurbsSurface->GetPointsVCount();
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не выбран" ), IDS_TITLE_ERR );
    outError = 25;
    return outError;
  }
  // Построение
  MbCurve3D * pllne( nullptr );
  if ( uDegree > 0 ) {  // Построение по U
    SArray<MbCartPoint3D> vrtx;

    //size_t nSegms = uCount - 1;
    //int outErrors = 0;
    char Path[128] = "C:/FairCurveModeler_TEMP/Temp/";
    char PathFile[128];

    strcpy( PathFile, Path );
    sf_out = fopen( strcat( PathFile, "pnt_name.pnt" ), "w" );
    if ( sf_out == nullptr ) {
      outErrors = 47;  // Неизвестная ошибка.
      return outErrors;
    }
    strcpy( PathFile, Path );
    sf_out_cmps = fopen( strcat( PathFile, "3DMesh_kmps.txt" ), "w" );
    if ( sf_out_cmps == nullptr ) {
      outErrors = 47;  // Неизвестная ошибка.
      return outErrors;
    }

    for ( size_t i = 0; i < vCount; i++ ) {  // Обход строк
      for ( size_t jr = 0; jr < uCount; jr++ ) {
        vrtx.Add( cvrts[i * uCount + jr] );
      }
      ::SplineCurve( vrtx, false, st_Polyline3D, pllne );

      vrtx.clear();
      WriteSplgnToGB( pllne );
      WriteSplgnToCmpsGB( pllne, i + 1 );
    }  // Обход строк

  } // Построение по U

  fclose( sf_out );
  fclose( sf_out_cmps );

  return 0;
}


//--------------------------------------------------------------------------------
// Читать 3D полилинию из Геометрического Буфера
// в формате таблицы
//---
int ReadPllneFromGeomBufferCompas()
{
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;

  Style style = TestVariables::BSPLINE_Style;
  bool closed = false; // Только незамкнутый формат выходе

  size_t nVertex, nVertexWeights, nKnots;
  FILE * in;
  size_t alt, n, j, mr;

  char * p[10]; // указатели на числа в строке
  double xyz[10];
  //int outErrors = 0;

  char Path_File[] = "C:/FairCurveModeler_TEMP/Temp/pllne_cmps.txt";

  if ( ( in = fopen( Path_File, "r" ) ) == nullptr ) {
    outErrors = 3;
    return outErrors;
  }
  for ( ; ; ) {  //DEBUG 2020 11 23

    char stroke[128];
    if ( fgets( stroke, 120, in ) == nullptr ) {
      nVertex = 0; nVertexWeights = 0; nKnots = 0;
      fclose( in );
      return 1;
    }
    pnts.clear(); //DEBUG 2020 11 23
#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(fData.prt, "\nВершины ");
#endif

    for ( j = 0; ; ++j ) {
      alt = AnalizeCmps( stroke, p, 3, n );

      // запись чисел
      if ( n > 2 ) {
        for ( mr = 0; mr < 3; ++mr ) {
          xyz[mr] = atof( p[mr] );
        }
        pnt.x = xyz[0];
        pnt.y = xyz[1];
        pnt.z = xyz[2];

        pnts.Add( pnt );
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(fData.prt, "\n j = %d x = %f y = %f z = %f", j, pnt.x, pnt.y, pnt.z);
#endif
      }
      else {
        //outErrors = 6;
        //fclose(in);
        outErrors = 2;
        break;
      }

      if ( alt == 2 )
        break;

      if ( fgets( stroke, 120, in ) == nullptr ) {
        // Конец таблицы точек.
        break;
      }

    }
    nVertex = j + 1;
    if ( nVertex > 1 && outErrors == 0 ) { // Ломаная рассчитана
      // Нарисовать ломаную S-полигона
      MbCurve3D * result( nullptr );
      ::SplineCurve( pnts, closed, st_Polyline3D, result );
      TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
    }
  }//DEBUG 2020 11 23
  fclose( in );

  return 0;
}


//-----------------------------------------------------------------------------
// Записать S-polygon в ГеомБуфер  
//---
int WriteSplgnToGB( MbCurve3D * pllne ) {

  //int outErrors = 0;
  // MbItem * obj = nullptr;
  //MbCurve3D * pllne(nullptr);
  //bool cl;

  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts );
  size_t n = cvrts.Count();
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;

  size_t nVertex = cvrts.Count();
  size_t i, ir, j;

  char stroke[128];
  char buffer[128];
  double xyz[3];

  n = nVertex;
  for ( i = 0; i < n; ++i ) {
    if ( i == 0 ) {
      stroke[0] = '(';
      stroke[1] = '(';
      stroke[2] = '\0';
    }
    else {
      stroke[0] = '(';
      stroke[1] = '\0';
    }

    xyz[0] = cvrts[i].x;
    xyz[1] = cvrts[i].y;
    xyz[2] = cvrts[i].z;

    for ( int mr = 0; mr < 3; ++mr ) {
      sprintf( buffer, "%.12f", xyz[mr] );

      for ( ir = 0; ir < 40; ++ir ) {
        if ( buffer[ir] == ' ' ) continue; break;
      }
      strcat( stroke, &buffer[ir] );
      j = strlen( stroke );
      stroke[j] = ' ';
      ++j;
      stroke[j] = '\0';
    }

    if ( i == n - 1 ) {
      stroke[j - 1] = ')';
      stroke[j] = ')';
      stroke[j + 1] = '\n';
      stroke[j + 2] = '\0';
    }
    else {
      stroke[j - 1] = ')';
      stroke[j] = '\n';
      stroke[j + 1] = '\0';
    }

#ifdef C3D_DEBUG_FAIR_CURVES
    if ( fprintf( sf_out, "%s", stroke ) == 0 ) {
      outErrors = 1;
      return outErrors;
    }
#endif
  }

  //fclose(out);

  return 0;
}


//-----------------------------------------------------------------------------
// Записать S-polygon в ГеомБуфер  
//---
int WriteSplgnToCmpsGB( MbCurve3D * pllne, size_t in_j ) {

  //int outErrors = 0;
  //MbItem * obj = nullptr;

  //bool cl;
  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts );
  size_t n = cvrts.Count();
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;
  size_t nVertex = cvrts.Count();
  size_t i; // , ir, j;

  //char stroke[128];
  char buffer[128];
  double xyz[3];

  sprintf( buffer, "%f\n", (float)in_j );

#ifdef C3D_DEBUG_FAIR_CURVES
  if ( fprintf( sf_out_cmps, "%s", buffer ) == 0 ) {
    outErrors = 1;
    return outErrors;
  }
#endif

  n = nVertex;

  // Не должно быть замыкания.
  if ( fabs( cvrts[0].x - cvrts[n - 1].x ) < METRIC_ACCURACY &&
       fabs( cvrts[0].y - cvrts[n - 1].y ) < METRIC_ACCURACY &&
       fabs( cvrts[0].z - cvrts[n - 1].z ) < METRIC_ACCURACY )
    n = n - 1;

  for ( i = 0; i < n; ++i ) {
    xyz[0] = cvrts[i].x;
    xyz[1] = cvrts[i].y;
    xyz[2] = cvrts[i].z;

    sprintf( buffer, "%.12f \t%.12f \t%.12f \t\n", xyz[0], xyz[1], xyz[2] );

#ifdef C3D_DEBUG_FAIR_CURVES
    if ( fprintf( sf_out_cmps, "%s", buffer ) == 0 ) {
      outErrors = 1;
      return outErrors;
    }
#endif
  }
  return 0;
}


//------------------------------------------------------------------------------
// Открыть файл для отладочной печати.
// ---
int InitFcm()
{
  int errors = 0;
  char Path[128] = "C:/FairCurveModeler_TEMP/Temp/";
  char PathFile[128];
  strcpy( PathFile, Path );
  prt = fopen( strcat( PathFile, "$fair_curve_report.txt" ), "w" );
  if ( prt == nullptr ) {
    errors = 47;  // Неизвестная ошибка.
    return errors;
  }
#ifdef C3D_DEBUG_FAIR_CURVES
  /*DEBUG*/ fData.prt = prt;
#endif

  return errors;
}


//------------------------------------------------------------------------------
// Закрыть файл для отладочной печати.
// ---
int CloseFcm()
{
  if ( prt )
    fclose( prt );
  return 0;

}


//-----------------------------------------------------------------------------
// Оценка точности построения кривой на ломаной. 
// Возвращает максимальную ошибку (максиманое расстояние между точками ломаной и сплайном).
//---
double EstimateAccuracy()
{
  bool closed;
  size_t n = 0;
  MbCurve3D * pllne = nullptr;
  MbNurbs3D * curve = nullptr;
  // Указывается ломаная.
  if ( ReadPllne( pllne, closed, n ) > 0 )
    return MB_MAXDOUBLE;
  // Указывается построенная кривая на ломаной.
  if ( RdNurbs( curve ) > 0 )
    return MB_MAXDOUBLE;

  std::array<double, 3> maxAccuracy = {MB_MAXDOUBLE, MB_MAXDOUBLE, MB_MAXDOUBLE};
  TestAccuracy( pllne, curve, &maxAccuracy );

//  // Определяются отклонения, которые записываются в файл отчета
//  // C:\FairCurveModeler_TEMP\Temp\$fair_curve_report.txt
//  MbControlData3D cvrts;
//  pllne->GetBasisPoints( cvrts );
//  MbCartPoint3D pnt;
//  for ( size_t ir = 0, number = cvrts.Count(); ir < number; ir++ ) {
//    pnt.x = cvrts[ir].x;
//    pnt.y = cvrts[ir].y;
//    pnt.z = cvrts[ir].z;
//
////#ifdef C3D_DEBUG_FAIR_CURVES
//    double r_distance = curve->DistanceToPoint( pnt );
//    fprintf( prt, "\nDistance = %15.12f", r_distance );
//
////#endif
//  }
  closed = pllne->IsClosed();
  if (outErrors == 0)
    MessageBoxEx(_T("Операция выполнена"), IDS_TITLE_ALL_RIGHT);

  return maxAccuracy[2];
}


//-----------------------------------------------------------------------------  
// Прочитать NURBS кривую из Геометрического Буфера.
//---
int RdNurbsSFromGB()
{
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;
  SArray<double> knots;
  SArray<double> wts;

  Style style = TestVariables::BSPLINE_Style;
  size_t nbs_degree;  // Порядок = степень + 1;
  bool closed = false; // Только незамкнутый формат выходе

  size_t nVertex, nVertexWeights, nKnots;

  FILE * in;

  size_t alt, n, j, i, mr;
  char * p[10]; // указатели на числа в строке
  double xyz[3];
  // int outErrors = 0;
  char Path_File[] = "C:/FairCurveModeler_TEMP/Temp/spl_name.spl";

  if ( ( in = fopen( Path_File, "r" ) ) == nullptr ) {
    outErrors = 3;
    return outErrors;
  }

  char stroke[128];
  if ( fgets( stroke, 120, in ) == nullptr ) {
    nVertex = 0; nVertexWeights = 0; nKnots = 0;
    fclose( in );
    return 1;
  }

#ifdef C3D_DEBUG_FAIR_CURVES
  //fprintf(fData.prt, "\nВершины ");
#endif
  for ( j = 0; ; ++j ) {
    alt = AnalizeLine( stroke, p, 3, n );

    if ( n == 3 ) {
      for ( mr = 0; mr < n; ++mr ) {
        xyz[mr] = atof( p[mr] );
      }
      pnt.x = xyz[0];
      pnt.y = xyz[1];
      pnt.z = xyz[2];

      pnts.Add( pnt );
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(fData.prt, "\n j = %d x = %f y = %f z = %f", j, pnt.x, pnt.y, pnt.z);
#endif
    }
    else {
      outErrors = 6;
      fclose( in );
      return outErrors;
    }

    if ( alt == 2 )
      break;
    if ( fgets( stroke, 120, in ) == nullptr ) {
      // Неправильная структура файла. Отсутствуют '))'
      nVertex = 0;
      nVertex = 0;
      nKnots = 0;
      // Message_Error( ach_msg_error[6] );
      outErrors = 17; // Структура данных
      fclose( in );
      return outErrors;
    }
  }

  nVertex = j + 1;
  // Читать веса
  if ( fgets( stroke, 120, in ) == nullptr ) {
    nVertexWeights = 0; nKnots = 0; // Веса не заданы
    fclose( in );
    return 0;
  }

  for ( j = 0; ; ++j ) {
    alt = AnalizeLine( stroke, p, 3, n );

    if ( n == 3 ) {
      double w = atof( p[0] );
      wts.Add( w );
#ifdef C3D_DEBUG_FAIR_CURVES
      //fprintf(fData.prt, "\n j = %d w = %f", j, w);
#endif
    }
    else {
      outErrors = 6;
      fclose( in );
      return outErrors;
    }

    if ( alt == 2 )
      break;

    if ( fgets( stroke, 120, in ) == nullptr ) {
      // Неправильная структура файла. Отсутствуют '))'
      nVertex = 0;
      nKnots = 0;
      //Message_Error( ach_msg_error[6] );
      outErrors = 6;
      fclose( in );
      return outErrors;
    }
  }

  nVertexWeights = j + 1;
  // Читать узлы
  size_t iq2_d;
  size_t n_dots;
  size_t curDegree;
  if ( fgets( stroke, 120, in ) == nullptr ) {
    // Вектор узлов не задан.
    // Запрашиваем тип и степень. По умолчанию степень 3 тип NURBzS.
    curDegree = 3;
    int select = 3;
    // GetInt(_T("Введите тип кривой; 1- NURBzS, 2 - BSpline "), select);
    GetInt( _T( "Введите степень NURBzS кривой" ), select );
    if ( select < 2 || select > 10 ) {
      // А та та
      return 24;
    }
    curDegree = select;

    //GetInt( IDS_ENTER_SPLINE_ORDER, curDegree )

    n_dots = ( nVertex - 1 ) / curDegree + 1;
    iq2_d = n_dots * curDegree + 2;

    j = 0;
    for ( size_t ir = 0; ir < curDegree + 1; ++ir ) {
      knots.Add( 0. );
    }

    j = j + curDegree + 1;
    for ( i = 1; i < n_dots - 1; ++i ) {
      for ( size_t ir = 0; ir < curDegree; ++ir ) {
        knots.Add( (double)( i ) );
      }

      j = j + curDegree;
    }
    for ( size_t ir = 0; ir < curDegree + 1; ++ir ) {
      knots.Add( (double)( n_dots - 1 ) );
    }
  }
  else {
    for ( j = 0; ; ++j ) {
      alt = AnalizeLine( stroke, p, 3, n );
#ifdef C3D_DEBUG_FAIR_CURVES
      // // fprintf( prt, "\n RdLnPntsN j = %d alt = %d, stroke = %s\n",
      // j, alt, stroke);
#endif

      // запись чисел
      if ( n == 3 ) {
        double u = atof( p[0] );
        knots.Add( u );
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(fData.prt, "\n j = %d u = %f", j, u);
#endif
      }
      else {
#ifdef C3D_DEBUG_FAIR_CURVES
        // fprintf( out_err,"*** количество координат точки\n");
#endif
        //Message_Error( ach_msg_error[6] );
        outErrors = 24;
        fclose( in );
        return 0;
      }

      if ( alt == 2 )
        break;

      if ( fgets( stroke, 120, in ) == nullptr )
        break;
    }
  }

  // Узнать степень
  size_t ir = 0;
  for ( ir = 0; ir < nVertex; ir++ ) {
    //double a = knots[ir]; double b = knots[ir + 1];
    if ( knots[ir] != knots[ir + 1] )
      break;
  }

  nbs_degree = ir + 1;

  // Нарисовать кривую
  MbCurve3D * resCurve = nullptr;
  ::NurbsCurve( pnts, wts, nbs_degree, knots, closed, resCurve );

  TestVariables::viewManager->AddObject( style, resCurve );

  fclose( in );

  return 0;
}


//--------------------------------------------------------------------------------
// Читать 3D полилинию их Геометрического Буфера.
//---
int ReadPllneFromGeomBuffer()
{
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;

  Style style = TestVariables::BSPLINE_Style;
  bool closed = false; // Только незамкнутый формат выходе

  size_t nVertex, nVertexWeights, nKnots;
  FILE * in;
  size_t alt, n, j, mr;

  char * p[10]; // указатели на числа в строке
  double xyz[10];
  //int outErrors = 0;

  char Path_File[] = "C:/FairCurveModeler_TEMP/Temp/pnt_name.pnt";

  if ( ( in = fopen( Path_File, "r" ) ) == nullptr ) {
    outErrors = 3;
    return outErrors;
  }
  for ( ; ; ) {  //DEBUG 2020 11 23
    char stroke[128];
    if ( fgets( stroke, 120, in ) == nullptr ) {
      nVertex = 0; nVertexWeights = 0; nKnots = 0;
      fclose( in );
      return 1;
    }
    pnts.clear();//DEBUG 2020 11 23
#ifdef C3D_DEBUG_FAIR_CURVES
    //fprintf(fData.prt, "\nВершины ");
#endif
    for ( j = 0; ; ++j ) {

      alt = AnalizeLine( stroke, p, 3, n );

      // запись чисел
      if ( n > 2 ) {
        for ( mr = 0; mr < 3; ++mr ) {
          xyz[mr] = atof( p[mr] );
        }
        pnt.x = xyz[0];
        pnt.y = xyz[1];
        pnt.z = xyz[2];

        pnts.Add( pnt );
#ifdef C3D_DEBUG_FAIR_CURVES
        //fprintf(fData.prt, "\n j = %d x = %f y = %f z = %f", j, pnt.x, pnt.y, pnt.z);
#endif
      }
      else {
        outErrors = 6;
        fclose( in );
        return outErrors;
      }

      if ( alt == 2 )
        break;

      if ( fgets( stroke, 120, in ) == nullptr ) {
        // Неправильная структура файла. Отсутствуют '))'
        nVertex = 0;
        nVertex = 0;
        nKnots = 0;
        // Message_Error( ach_msg_error[6] );
        outErrors = 17; // Структура данных
        fclose( in );
        return outErrors;
      }
    }

    nVertex = j + 1;

    // Нарисовать ломаную S-полигона
    MbCurve3D * result( nullptr );
    ::SplineCurve( pnts, closed, st_Polyline3D, result );
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );
  }//DEBUG 2020 11 23

  //fclose( in );
  //
  //return 0;
}


//-----------------------------------------------------------------------------
// Записать ломаную в ГеомБуфер  
//---
int WritePllneGB() {

  //int outErrors = 0;
  MbItem * obj = nullptr;
  MbCurve3D * pllne( nullptr );
  bool cl;

  GetGObj( IDS_SELECT_CURVE, st_Curve3D, &obj ) && ( obj->IsA() == st_SpaceInstance );
  pllne = (MbCurve3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
  if (  // curve->IsA() == st_Nurbs3D   ||
       pllne->IsA() == st_Polyline3D ) {
    cl = pllne->IsClosed();
    TestVariables::viewManager->RefreshObjectModel( obj ); // Погасить кривую. // Hide a curve.
  }
  else {
    ::MessageBoxEx( _T( "*** Объект не 3D Ломаная" ), IDS_TITLE_ERR );
    outErrors = 28;
    return false;
  }

  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts );
  size_t n = cvrts.Count();
  MbCartPoint3D pnt;
  SArray<MbCartPoint3D> pnts;

  size_t nVertex = cvrts.Count();

  if ( cl ) {  // Замкнутая ломаная
    pnt = cvrts[0];
    cvrts.AddShare( pnt );
    nVertex++;
  }

  FILE * out;
  size_t i, ir, j;

  char stroke[128];
  char buffer[128];
  double xyz[3];

  if ( ( out = fopen( "C:/FairCurveModeler_TEMP/Temp/pnt_name.pnt", "w" ) ) == nullptr ) {
    //fprintf( out_err, "*** входной командный файл ");
    ++outErrors;
    return outErrors;
  }
  n = nVertex;
  for ( i = 0; i < n; ++i ) {
    if ( i == 0 ) {
      stroke[0] = '(';
      stroke[1] = '(';
      stroke[2] = '\0';
    }
    else {
      stroke[0] = '(';
      stroke[1] = '\0';
    }

    xyz[0] = cvrts[i].x;
    xyz[1] = cvrts[i].y;
    xyz[2] = cvrts[i].z;

    for ( int mr = 0; mr < 3; ++mr ) {
      sprintf( buffer, "%.12f", xyz[mr] );

      for ( ir = 0; ir < 40; ++ir ) {
        if ( buffer[ir] == ' ' ) continue; break;
      }
      strcat( stroke, &buffer[ir] );
      j = strlen( stroke );
      stroke[j] = ' ';
      ++j;
      stroke[j] = '\0';
    }

    if ( i == n - 1 ) {
      stroke[j - 1] = ')';
      stroke[j] = ')';
      stroke[j + 1] = '\n';
      stroke[j + 2] = '\0';
    }
    else {
      stroke[j - 1] = ')';
      stroke[j] = '\n';
      stroke[j + 1] = '\0';
    }

    if ( fprintf( out, "%s", stroke ) == 0 ) {
      outErrors = 1;
      return outErrors;
    }
  }

  fclose( out );
  if ( ( out = fopen( "C:/FairCurveModeler_TEMP/Temp/pllne_cmps.txt", "w" ) ) == nullptr ) {
    //fprintf( out_err, "*** входной командный файл ");
    outErrors = 2;
    return outErrors;

  }
  n = nVertex;

  // Не должно быть замыкания.
  if ( fabs( cvrts[0].x - cvrts[n - 1].x ) < 0.0001 &&
       fabs( cvrts[0].y - cvrts[n - 1].y ) < 0.0001 &&
       fabs( cvrts[0].z - cvrts[n - 1].z ) < 0.0001 )
    n = n - 1;

  for ( i = 0; i < n; ++i ) {
    xyz[0] = cvrts[i].x;
    xyz[1] = cvrts[i].y;
    xyz[2] = cvrts[i].z;

    sprintf( buffer, "%.12f \t%.12f \t%.12f \t\n", xyz[0], xyz[1], xyz[2] );

    if ( fprintf( out, "%s", buffer ) == 0 ) {
      outErrors = 2;
      return outErrors;
    }
  }

  fclose( out );
  return 0;
}


//-----------------------------------------------------------------------------
// Читать ломаную из Геометрического Буфера.
//---
int ReadPllneGB()
{
  ReadPllneFromGeomBuffer();
  return 0;
}


#ifdef FAIR_CURVE_ADVANCED
//-----------------------------------------------------------------------------         
// Записать NURBS в Геометрический Буфер.
//---
int  WriteNURBSGB()
{
  //int outErrors = 0;

  MbItem * obj = nullptr;

  MbNurbs3D * curv;

  MbControlData3D cvrts;
  SArray<double> knots;
  SArray<double> wts;

  double length;
  int u_segm;
  bool closed = false;

  if ( GetGObj( IDS_SELECT_CURVE, st_Curve3D, &obj ) && ( obj->IsA() == st_SpaceInstance ) ) {
    curv = (MbNurbs3D *)( (MbSpaceInstance *)obj )->GetSpaceItem();
    if ( curv != nullptr && ( curv->IsA() == st_Nurbs3D || curv->IsA() == st_Bezier3D || curv->IsA() == st_Hermit3D ) ) {

      closed = curv->IsClosed();

      if ( closed ) {
        //outErrors = 27;
        //return outErrors;
        curv->UnClamped( false );
      }

      curv->GetBasisPoints( cvrts );
      curv->GetKnots( knots );
      curv->GetWeights( wts );

      length = curv->GetParamLength();
      u_segm = (int)( length + 0.25 );


      
    }
    else {
      ::MessageBoxEx( _T( "*** Объект не NURBS кривая" ), IDS_TITLE_ERR );
      return 28;
    }

    size_t nVertex = cvrts.Count();
    size_t curDegree = curv->GetDegree() - 1;
    size_t nVertexWeights = wts.Count();
    size_t nKnots = knots.Count();

    // Проверка формата NURBzS
    int u_cntr_plg;
    size_t k_array;
    int n_dots = u_segm + 1; //(fCurve.kArray2-1)/fCurve.uDegree + 1;
    if ( nKnots == n_dots * curDegree + 2 )
      u_cntr_plg = 3;

    // Проверка формата BSpline

    if ( nKnots == n_dots + 2 * curDegree )
      u_cntr_plg = 2;

    u_cntr_plg = 2;

    if ( knots[curDegree + 2] == knots[curDegree + 3] )
      u_cntr_plg = 3;

    if ( u_cntr_plg != 2 && u_cntr_plg != 3 ) {
      outErrors = 24;
      return 24;
    }

    if ( u_cntr_plg == 3 )
      k_array = nVertex / curDegree + 1;
    if ( u_cntr_plg == 2 )
      k_array = nVertex - curDegree + 1;

    FILE * out;
    size_t i, ir, j;

    char stroke[128];
    char buffer[128];
    double xyz[3];

    if ( ( out = fopen( "C:/FairCurveModeler_TEMP/Temp/spl_name.spl", "w" ) ) == nullptr ) {
      //fprintf( out_err, "*** входной командный файл ");
      outErrors = 29;
      return outErrors;
    }
    size_t n = nVertex;
    for ( i = 0; i < n; ++i ) {
      if ( i == 0 ) {
        stroke[0] = '(';
        stroke[1] = '(';
        stroke[2] = '\0';
      }
      else {
        stroke[0] = '(';
        stroke[1] = '\0';
      }

      xyz[0] = cvrts[i].x;
      xyz[1] = cvrts[i].y;
      xyz[2] = cvrts[i].z;

      for ( int mr = 0; mr < 3; ++mr ) {
        sprintf( buffer, "%.12f", xyz[mr] );

        for ( ir = 0; ir < 40; ++ir ) {
          if ( buffer[ir] == ' ' ) continue; break;
        }
        strcat( stroke, &buffer[ir] );
        j = strlen( stroke );
        stroke[j] = ' ';
        ++j;
        stroke[j] = '\0';
      }

      if ( i == n - 1 ) {
        stroke[j - 1] = ')';
        stroke[j] = ')';
        stroke[j + 1] = '\n';
        stroke[j + 2] = '\0';
      }
      else {
        stroke[j - 1] = ')';
        stroke[j] = '\n';
        stroke[j + 1] = '\0';
      }

      if ( fprintf( out, "%s", stroke ) == 0 ) {
        // fprintf( out_err, "*** внешняя память\n");
        //Message_Error( ach_msg_error[1] );
        outErrors = 1;
        return outErrors;
      }
    }

    n = nVertexWeights;
    xyz[1] = 0.;
    xyz[2] = 0.;
    for ( i = 0; i < n; ++i ) {
      if ( i == 0 ) {
        stroke[0] = '(';
        stroke[1] = '(';
        stroke[2] = '\0';
      }
      else {
        stroke[0] = '(';
        stroke[1] = '\0';
      }

      xyz[0] = wts[i];
      for ( int mr = 0; mr < 3; ++mr ) {
        sprintf( buffer, "%.12f", xyz[mr] );

        for ( ir = 0; ir < 40; ++ir ) {
          if ( buffer[ir] == ' ' ) continue;
          break;
        }
        strcat( stroke, &buffer[ir] );
        j = strlen( stroke );
        stroke[j] = ' ';
        ++j;
        stroke[j] = '\0';
      }

      if ( i == n - 1 ) {
        stroke[j - 1] = ')';
        stroke[j] = ')';
        stroke[j + 1] = '\n';
        stroke[j + 2] = '\0';
      }
      else {
        stroke[j - 1] = ')';
        stroke[j] = '\n';
        stroke[j + 1] = '\0';
      }

      if ( fprintf( out, "%s", stroke ) == 0 ) {
        // *** внешняя память
        outErrors = 19;
        return outErrors;
      }
    }

    n = nKnots;
    for ( i = 0; i < n; ++i ) {
      if ( i == 0 ) {
        stroke[0] = '(';
        stroke[1] = '(';
        stroke[2] = '\0';
      }
      else {
        stroke[0] = '(';
        stroke[1] = '\0';
      }

      xyz[0] = knots[i];
      for ( int mr = 0; mr < 3; ++mr ) {
        sprintf( buffer, "%.12f", xyz[mr] );

        for ( ir = 0; ir < 40; ++ir ) {
          if ( buffer[ir] == ' ' ) continue;
          break;
        }
        strcat( stroke, &buffer[ir] );
        j = strlen( stroke );
        stroke[j] = ' ';
        ++j;
        stroke[j] = '\0';
      }

      if ( i == n - 1 ) {
        stroke[j - 1] = ')';
        stroke[j] = ')';
        stroke[j + 1] = '\n';
        stroke[j + 2] = '\0';
      }
      else {
        stroke[j - 1] = ')';
        stroke[j] = '\n';
        stroke[j + 1] = '\0';
      }

      if ( fprintf( out, "%s", stroke ) == 0 ) {
        // fprintf( out_err, "*** внешняя память\n");
        //Message_Error( ach_msg_error[1] );
        outErrors = 19;
        return outErrors;
      }
    }

    fclose( out );
  }

  return 0;
}
#endif


bool IsHermit( MbCurve3D * pllne ) {
  // Проверка ГО Эрмита на корректность.
  MbControlData3D cvrts;
  pllne->GetBasisPoints(cvrts);
  size_t n = cvrts.Count();
  size_t kArray = n / 5;

  if (kArray * 5 != n) {
    return false; // Объект не ГО Эрмита.
  }
  else { // Более тонкая проверка
    if ( cvrts[0].x != cvrts[4].x || cvrts[0].y != cvrts[4].y || cvrts[0].z != cvrts[4].z )
      return false; // Объект не ГО Эрмита.
  }
  
  return true;
}


// -------------------------------------------------------------------------------- -
// Редактирование ГО Эрмита.
// ---
int Test_EditHermiteGD()
{
  int outError = 0;
  bool closed;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();
  if ( !IsHermit( pllne ) ) {
    outErrors = rt_ObjectNotHermiteGD; // Объект не ГО Эрмита. rt_Success) 
    string_t message( IDS_TITLE_ERR );
    message = MessageError( outErrors ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    return outErrors;

  }

  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts );
  MbCartPoint3D pnt, ptng, pcvt;
  SArray<MbCartPoint3D> pnts;

  for ( size_t ir = 0; ir < n; ir++ ) {
    pnt = cvrts[ir];
    pnts.Add( pnt );
  }

  // Фиксация касательных.
  int num = 0;
  for ( int ir = 0; ; ++ir ) {
    if ( GetInt( _T( "Укажите номер точки опорной ломаной " ), num ) == false )
      break;
    bool yes = false;

    if ( GetBool( _T( "Будете уточнять координаты точки? " ), yes ) == true ) {
      double x = pnts[num * 5].x;
      double y = pnts[num * 5].y;
      double z = pnts[num * 5].z;
      GetDouble( _T( "X = " ), x );
      GetDouble( _T( "Y = " ), y );
      GetDouble( _T( "Z = " ), z );
      pnts[num * 5].x = x;
      pnts[num * 5].y = y;
      pnts[num * 5].z = z;
      pnts[num * 5 + 2].x = x;
      pnts[num * 5 + 2].y = y;
      pnts[num * 5 + 2].z = z;
      pnts[num * 5 + 4].x = x;
      pnts[num * 5 + 4].y = y;
      pnts[num * 5 + 4].z = z;
    }

    if ( GetBool( _T( "Будете уточнять координаты касательного вектора? " ), yes ) == true ) {
      double dX = pnts[num * 5 + 1].x - pnts[num * 5].x;
      double dY = pnts[num * 5 + 1].y - pnts[num * 5].y;
      double dZ = pnts[num * 5 + 1].z - pnts[num * 5].z;
      GetDouble( _T( "dX = " ), dX );
      GetDouble( _T( "dY = " ), dY );
      GetDouble( _T( "dZ = " ), dZ );
      pnts[num * 5 + 1].x = pnts[num * 5].x + dX;
      pnts[num * 5 + 1].y = pnts[num * 5].y + dY;
      pnts[num * 5 + 1].z = pnts[num * 5].z + dZ;
    }

    //if (GetBool(_T("Будете уточнять значение кривизны? "), yes) == true) {
    //
    //  double dX = pnts[num * 5 + 3].x - pnts[num * 5].x;
    //  double dY = pnts[num * 5 + 3].y - pnts[num * 5].y;
    //  double dZ = pnts[num * 5 + 3].z - pnts[num * 5].z;
    //  MbVector3D vCurvature(dX, dY, dZ);
    //  double curvature = vCurvature.Length();
    //  GetDouble(_T("(! > 0.0)curvature = "), curvature);
    //  MbVector3D vr;
    //  vCurvature.Normalize();
    //  vr = vCurvature * curvature;
    //
    //  pnts[num * 5 + 3] = pnts[num * 5] + vr;
    //
    //}
  }

  //Если изменение, то переопределить pllne
  // Создать ломаную 

  // Нарисовать ломаную S-полигона
  MbCurve3D * result( nullptr );
  if ( ::SplineCurve( pnts, closed, st_Polyline3D, result ) == rt_Success )
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, result );

  return outError;
}


//-------------------------------------
// Проверка срока лицензии
//---
int Expired()
{
  int outError = 0;
#ifdef C3D_WINDOWS // Верно для Windows.
  char stroke[12];
  _strdate( stroke );
#ifdef C3D_DEBUG_FAIR_CURVES
  fprintf( prt, "OS date:%s\n", stroke );
  fprintf( prt, "Expired date:%s\n", "08/30/22" );
#endif
  stroke[2] = '\0';
  stroke[5] = '\0';
  stroke[8] = '\0';
  //int day = atoi( &stroke[3] );
  int mounth = atoi( &stroke[0] );
  int year = atoi( &stroke[6] );
  if ( 365 * year + mounth * 31 > 365 * 22 + 9 * 31 ) {
    ++outError; // защита по сроку (действует 2 месяца со дня установки)
    // activation code has expired
  }
#endif

  return outError;
}


//---------------------------------------------------------------------------------
// Построение сглаживанием
// ---
int Test_Smoothing_Noisy_Data( bool createWire )
{
  int outError = 0;
  bool closed = false;
  size_t n = 0;
  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();
  MbCurve3D * initPllne = nullptr;
  initPllne = pllne;  // Исходная ломаная для оценки точности
  int inType = 1; // опорная ломаная
  if ( IsHermit( pllne ) )
    inType = 2; // ГО Эрмита
  size_t kArray;  // Количество опорных точек
  //Проверить количество точек в pllne.
  if ( inType == 1 )
    kArray = n;
  else
    kArray = n / 5;


    // Фильтрация может изменить количество точек. Сбивает нумерацию при фиксации точек и касательных.
    // Рекомендуется предварительно отработать регуляризацию командой Utilites.
    // Здесь только для определения nSites, nStrSites, nMinPntSite.
    // Отсортировать исходную ломаную. Удаляются совпадающие точки.
#ifdef FAIR_CURVE_ADVANCED
  size_t nSites = 0;        // количество выпуклых участков;
  size_t nStrSites = 0;     // количество прямолинейных участков;
  size_t nMinPntSite = 0;   // минимальное количество точек на участке.

  if ( inType == 1 ) {
    GeomPreProcessor( inType, true, pllne, n, nSites, nStrSites, nMinPntSite, pllne );
    GeomPreProcessor( inType, true, pllne, n, nSites, nStrSites, nMinPntSite, pllne ); // Дл уточнения nMinPntSite
    //ГО - отфильтрованная ломаная без совпадающих и выпадаюшщих точек, без прямолинейных участков после GeomPreProcessor для сглаживания.
    kArray = n;
  }
#endif

  if ( inType == 2 && closed ) {
    // ГО Эрмита замкнутым по стандарту C3D быть не должен.
    // Выход по ошибке.
 
    outError = 1;
    MessageBoxEx( _T( "ГО Эрмита не может быть замкнутым!" ), IDS_TITLE_ERR );
    return outError;
  }

  MbeFairSubdivision subdivision = (MbeFairSubdivision)0;
  //DEBUG20220915
  //if (inType == 1) {
  //  subdivision = (MbeFairSubdivision)GetIntValue(subdivision, 0, 2, gSubdivisionMsg);
  //}
  //  ) {
  //  outErrors = rt_ObjectNotHermiteGD; // Объект не ГО Эрмита. rt_Success) 
  //  string_t message(IDS_TITLE_ERR);
  //  message = MessageError(outErrors); // Формирует текст сообщения по номеру ошибки.
  //  MessageBoxEx(message.c_str(), IDS_TITLE_ERR); // Выводит сообщение.
  //  return outErrors;
  //}

  // Сглаживание зашумленных точек
  double smoothClearance = 0.0;  // Не менять. Используется как параметр.
  int clearanceNoisyIteration = 0;  // Не менять. Используется как параметр.
  int outSegments = 10;
  double scaleLength = 4.;
  int typeApprx = 1; // Тип аппроксимации  0 = NURBzS 1 -BSpline
  int typeSegm = 0;  // 0 - сегментация по узлам ломаной, 1 - задается произвольное число сегментов
  
  //==
  size_t degreeBSpline = 8;
  bool arrange = false;
  MbeFairCurvature accountCurvature = (MbeFairCurvature)0;
  MbeFairVector accountInflexVector = fairVector_Tangent;
  //==
  double realAccuracyBSpl = METRIC_ACCURACY * 0.1;
  size_t numberOfIterationsBSpl = 1000;
  //==
  int fixFirst = 0; //Фиксация точных координат первой точки ломаной в аппр кривой
  int fixLast = 0;
  //===
  bool data = false; //вспомогательный параметр

  // Метод сглаживания
  // Тип аппроксимации
  int smoothing = GetIntValue( 3, 3, 5, gFairingMsg5 );
  // Фиксация касательных на ГО Эрмита
  MbeFixPntTng fixPntTng = fixPntTng_NotFix;

  // Сглаживание по коридору отклонений
  if ( smoothing == 3 || smoothing == 5 ) {
    smoothClearance = 0.002;
    clearanceNoisyIteration = 200;
    while ( data == false ) {
      if ( GetDouble( _T( "Коридор сглаживания: " ), smoothClearance ) ) {
        if ( smoothClearance < 0. )
          smoothClearance = 0.;
      }
      if ( smoothClearance > 0. )
        clearanceNoisyIteration = GetIntValue( 10, 2, 1000, _T( "Количество итераций сглаживания: " ) );
      data = true;

      if ( smoothClearance == 0. || clearanceNoisyIteration == 0 ) {
        // Дайте реальные значения коридора 
        MessageBoxEx( _T( "Задайте ненулевые значения" ), IDS_TITLE_ERR );
        data = false;
      }
    }
    // Тип аппроксимации
    typeApprx = 1; // BSpline
    //if ( smoothing == 3 ) {
    //  typeApprx = GetIntValue( 0, 0, 1, gApproxMsg2 );
    //  if ( typeApprx == 0 )
    //    outSegments = 0;  // Не менять. Параметр. Будет NURBzS кривая.
    //}
  } // Метод по коридору
  

  // Точная фиксация зашумленных точек. Фиксация касательных.
  IndexVectorPtrArray fixConstraints;

  int no = 2; 
  size_t n_fix = 0;  // R

  if ( smoothing == 3 || smoothing == 5  ) {  // Фиксация точек
    MbControlData3D cvrts;
    pllne->GetBasisPoints( cvrts );

    for ( size_t ir = 0; ir < n; ir++ ) {
      if ( GetInt( _T( "Укажите по возрастанию номер опорной точки для фиксации" ), no ) == false )
        break;
      if( inType == 1 )
        fixConstraints.push_back( std::pair<size_t, MbVector3D *>( (size_t)no, nullptr ) );
      else
        fixConstraints.push_back( std::pair<size_t, MbVector3D *>( (size_t)(no * 5), nullptr ) ); // Номер
      n_fix++;

      if ( inType == 2 && smoothing == 3) {  // ГО Эрмита
        // Фиксация касательных.
        MbVector3D * tangVec = nullptr;
        if ( GetBool( _T( "Фиксировать касательную?" ), false ) ) {
          tangVec = new MbVector3D();
          fixPntTng = fixPntTng_Fix;
        }
        fixConstraints.back().second = tangVec;
      }

      no = no + 2;  // Во избежание взаимного гашения фиксированные точки должны 
      // отстоять друг от друга достаточно далеко.
      if ( no > (int)(kArray - 2) )
        break;
    }
  }

  if ( smoothing > 3 ){
    MbeFixPntTng fixNoisyPnt = fixPntTng_NotFix;
    if ( n_fix > 0 )
      fixNoisyPnt = fixPntTng_Fix;

    if ( GetInt( _T( "Фиксировать начальную точку = 1" ), fixFirst ) == true ) {
      fixFirst = 1;
    }
    if ( GetInt( _T( "Фиксировать конечную точку = 1" ), fixLast ) == true ) {
      fixLast = 1;
    }
  }
  //======================================

  // Параметры аппроксимации
  //int s_numThreshold1 = 999;  // Гармоничное распределение.
  int s_numThreshold2 = 9999; // Распределение по узлам.

  if ( typeApprx > 0 && smoothing > 3 ) {  //BSpline аппроксимация
    // Репараметризация по узлам исходной ломаной или по заданному количеству сегментов
    typeSegm = GetIntValue( 0, 0, 2, gApproxMsg3 );
    if ( typeSegm == 0 )
      outSegments = s_numThreshold2;   // Не менять. Используется как параметр. Сегментация по узлам.
    if ( typeSegm == 2 )
      outSegments = s_numThreshold2;   // Не менять. Используется как параметр. Сегментация гармоничная.
    if ( typeSegm == 1 ) {  // Репараметризация с количеством сегментов.
      GetInt( _T( "Укажите количество сегментов >= 6:" ), outSegments );
      if ( outSegments < 6 )
        outSegments = 6;
   
      GetDouble( _T( "Укажите масштаб влияния длины (> 1.0):" ), scaleLength );
      if ( scaleLength < 1. )
        scaleLength = 1.;
    }
    degreeBSpline = GetIntValue( 8, 2, 10, gDegreeMsg );
    if ( (int)degreeBSpline > outSegments + 2 ) //DEBUG20220927 уточнение степени по количеству сегментов.
      degreeBSpline = outSegments - 2;
  }
  if ( typeApprx > 0 && fixPntTng == fixPntTng_Fix ) {  //BSpline аппроксимация по узлам
    outSegments = 0;
    degreeBSpline = GetIntValue( 4, 2, 10, gDegreeMsg );
  }

  //======================================

  MbeFairWarning warning( fwarn_Success );
  if ( createWire ) { // Создавать проволочный каркас.
    // TODO
    MessageBoxEx( _T("Not supported so far!"), IDS_TITLE_ERR );
    return 1;

    MbFairCreateData parameters;
    parameters._fairing = (MbeFairSmoothing)smoothing;
    parameters._accountCurvature = accountCurvature;
    parameters._degree = degreeBSpline;
    parameters._subdivision = subdivision;
    parameters._arrange = arrange;
    parameters.SetPrecision( realAccuracyBSpl );
    parameters._iterationsNumber = numberOfIterationsBSpl;
    parameters._accountVectorInflect = accountInflexVector;
    parameters._noisyDeviation = smoothClearance;
    parameters._noisyIterations = clearanceNoisyIteration;

    MbFairCurveMethod method( fairCrvType_BSpline, fairData_GDHPoly );
    MbSNameMaker snMaker( ct_Curve3DCreator, MbSNameMaker::i_SideNone, 0 );
    MbFairCurveResult resCurve;

    MbResultType res = ::FairCurve( MbFairCurveParams( *polyPtr, snMaker ), method, parameters, resCurve );
    if ( IsSuccess(res) ) { // Успешное завершение.
      // Нарисовать кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve.GetCurve() );
    }
    else {
      string_t message( IDS_TITLE_ERR );
      message = MessageError( res );        // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR );       // Выводит сообщение.
    }
    outError = res;
    warning = resCurve._warning;
  }
  else {
    fData.fairing = (MbeFairSmoothing)smoothing;
    fData.degreeBSpline = degreeBSpline;
    fData.subdivision = subdivision;
    fData.accountInflexVector = accountInflexVector;
    fData.arrange = arrange;
    fData.accountCurvature = accountCurvature;
    fData.realAccuracyBSpl = realAccuracyBSpl;
    fData.numberOfIterationsBSpl = numberOfIterationsBSpl;
    fData.clearanceNoisy = smoothClearance;
    fData.clearanceNoisyIteration = clearanceNoisyIteration;
    fData.nSegments = outSegments; // По умолчанию 0 для простого построения
    //Сброс активации параметров
    fData.switchFixFirstPointAtNoisy = false;
    fData.switchFixLastPointAtNoisy = false;
    fData.switchEndTangents = false; //Сброс касательных

    fData.tangentCorrectBspline = false;  // Не использовать касательные в точках перегиба от исходной кривой.
    // Создать новые.
    if ( n_fix > 0 ) {
      fData.InitFixConstraints( fixConstraints ); // Перенос номеров фиксированных зашумленных точек 
    }
    if ( fixFirst > 0 )
      fData.switchFixFirstPointAtNoisy = true;
    if ( fixLast > 0 )
      fData.switchFixLastPointAtNoisy = true;
    if ( fixFirst > 0 && fixLast > 0 )
      fData.switchEndTangents = true;

    {
      fData.outFormat = fairFormat_Close;
      fData.nSegments = outSegments;
      fData.scaleParam = scaleLength;
    }
    // Фиксация касательных.
    fData.fixPntTng = fixPntTng;

    //fairApprox_IsoSpline
    //fairApprox_IsoNurbs
    if ( typeApprx == 0 )fData.approx = fairApprox_IsoNurbs;
    if ( typeApprx == 1 )fData.approx = fairApprox_IsoSpline;

    // Программа построения плавной B-кривой [со сглаживанием] на ГО Эрмита произвольной формы.
    MbCurve3D * resCurve = nullptr;
    fData.error = ::CreateFairBSplineCurveOnNoisyData( pllne, inType, fData, resCurve );

    if ( IsSuccess(fData.error) ) { // Успешное завершение.
      std::array<double, 3> acc = { MB_MAXDOUBLE, MB_MAXDOUBLE, MB_MAXDOUBLE };
      // Отобразить кривую.
      Style style = TestVariables::BSPLINE_Style;
      TestVariables::viewManager->AddObject( style, resCurve );
      PrintApproxErr( initPllne, resCurve, fData.errInfo, fData.error ); // Сообщение о точности.
    }
    else { // Регистрация ошибки.
      string_t message( IDS_TITLE_ERR );
      message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
      MessageBoxEx( message.c_str(), IDS_TITLE_ERR ); // Выводит сообщение.
    }
    if ( fData.error == 0 )
      TestAccuracyHermite( initPllne, resCurve );
    outError = fData.error;
    warning = fData.warning;
  }

  if ( warning > 0 ) { // Регистрация предупреждения.
    string_t message( IDS_TITLE_WRN );
    message = MessageWarning( warning ); // Формирует текст сообщения по номеру ошибки.
    MessageBoxEx( message.c_str(), IDS_TITLE_WRN ); // Выводит сообщение.
  }
  return outError;
}


#ifdef FAIR_CURVE_ADVANCED
//---------------------------------------------------------------------------------
// Сформировать точки графиков кривизны или эволюты.
// ---
int DrawGraphs( MbCurve3D * pllneHGD,   // "Полилиния интерполированных точек а аиде ГО Эрмита"
                int typeGraph,          // "Отобразить: график кривизны = 0, эволюты = 1"            
                float graphScale,       // "Значение масштаба отображения  ( cvtMax / lengthCurve):"
                int typeDrawCvt,        // "Вид отображения кривизны: над кривой = 0, вида F(x) = 1"
                int typeDrawGraph,      // "Вид отображения: линия = 0, "ежик" = 1"
                SArray<MbCartPoint3D> & rezCurvePnts,   // Точки кривой.
                SArray<MbCartPoint3D> & rezGraphPnts )  //  Точки графика.
{
  MbControlData3D vrts;
  pllneHGD->GetBasisPoints( vrts );
  size_t n = vrts.Count();

  // Сформировать отображения графиков кривизны и эволюты
  // Отображать график кривизны: 
  // 1) в виде линии над кривой. 
  // 2) в виде "ежика"
  // Коэффициент масштабирования равен отношению максимальной кривизны к длине кривой.
  // Задается пользователем.
  // Отображать график эволюты:
  // 1) в виде линии под кривой. 
  // 2) в виде "ежика"

  SArray<MbCartPoint3D> curvePnts, cvtPnts, cvtPozs, tngPnts, evolutePnts;
  //SArray<MbControlData3D> curvePnts, cvtPnts, tngPnts, evolutepnts;
  SArray<double> cvtValue;

  // Записать точки кривой
  double lengthCurve = 0.;
  size_t nCurve = n / 5;
  MbCartPoint3D pnt;
  for ( size_t ir = 0; ir < nCurve; ++ir ) {
    //pnt = vrts[ir * 5];
    curvePnts.Add( vrts[ir * 5] );
    cvtPnts.Add( vrts[ir * 5 + 3] );
    cvtPozs.Add( vrts[ir * 5 + 3] ); 
    tngPnts.Add( vrts[ir * 5 + 1] );
  }

#ifdef C3D_DEBUG_FAIR_CURVES
  //fprintf( prt, "\nBegin DrawGraphs n = %d\n", n );
  //for ( size_t ir = 0; ir < nCurve; ++ir ) {
  //  fprintf( prt, "ir = %d cvtPnts = %f %f %f\n", ir, cvtPnts[ir][0], cvtPnts[ir][1], cvtPnts[ir][2] );
  //}
#endif
  
  MbVector3D vr;
  MbVector3D vOrt, vTng;

  if ( typeGraph == 1 ) { // Эволюта
    // Сформировать график эволюты

    MbVector3D vEv;
    double cvt;
    for ( size_t ir = 0; ir < nCurve; ++ir ) {
      vr = cvtPnts[ir] - curvePnts[ir];
      cvt = vr.Length();
      vTng = tngPnts[ir] - curvePnts[ir];
      vOrt = vr | vTng;
      if ( vOrt.Length() > 0. ) {
        vEv = vOrt | vTng;
        vEv.Normalize();
        vEv = -vEv;
        pnt = curvePnts[ir] + vEv * ( 1. / cvt );
      }
      else {
        cvtPnts[ir] = curvePnts[ir];
      }
      evolutePnts.Add( pnt );
    }
  } // Эволюта
  
  // Вычислить длину кривой и максимальную кривизну.
  lengthCurve = 0.;

  // Коэффициент масштабирования кривизны.
  double scale = 1.;

  if ( typeGraph == 0 ) { // Кривизна
    for ( size_t ir = 1; ir < nCurve; ++ir ) {
      vr = curvePnts[ir] - curvePnts[ir - 1];
      lengthCurve = lengthCurve + vr.Length();
    }

    // Вычислить длину кривой и максимальную кривизну.
    double maxCvt = 0.;
    for ( size_t ir = 0; ir < nCurve; ++ir ) {
      vr = cvtPnts[ir] - curvePnts[ir];
      vTng = tngPnts[ir] - curvePnts[ir];
      vOrt = vr | vTng;
      if ( maxCvt < vr.Length() && vOrt.Length() > 0. )
        maxCvt = vr.Length();
    }
    
    //maxCvt * scale / lengthCurve = srf_scale;
    scale = graphScale * lengthCurve / maxCvt;

    // Перестроить точки кривизны
    for ( size_t ir = 0; ir < nCurve; ++ir ) {
      vr = cvtPnts[ir] - curvePnts[ir];
      vTng = tngPnts[ir] - curvePnts[ir];
      vOrt = vr | vTng;
      if ( vOrt.Length() > 0. ) {
        cvtPozs[ir] = cvtPnts[ir] + vr * scale;
      }
      else {
        cvtPozs[ir] = curvePnts[ir];
      }
      //rezGraphPnts.Add( cvtPnts[ir] );
    }
    
  } // Кривизна

#ifdef C3D_DEBUG_FAIR_CURVES
  //fprintf( prt, "\ncvtPozs nCurve = %d\n", nCurve );
  //for ( size_t ir = 0; ir < nCurve; ++ir ) {
  //  fprintf( prt, "ir = %d cvtPozs = %f %f %f\n", ir, cvtPozs[ir][0], cvtPozs[ir][1], cvtPozs[ir][2] );
  //}
#endif

  // Нарисовать точки кривой и график кривизны
  for ( size_t ir = 0; ir < nCurve; ++ir ) {
    rezCurvePnts[ir] = curvePnts[ir];
  }

#ifdef C3D_DEBUG_FAIR_CURVES
  //strcpy( Path_3, Old_Path );
  //FILE * out;
  //out = fopen( strcat( Path_3, "int_name.int" ), "w" );
  // Записать точки кривой.
  //WrArrPnts( out, 1, curvePnts, nCurve );
#endif

  // Записать точки кривизны.
  size_t nCvt = 0;
  nCvt = cvtPnts.Count();
  if ( typeGraph == 0 ) { // Кривизна 
    if ( typeDrawCvt == 0 ) { // Над кривой
      // Нарисовать точки графика кривизны
      if ( typeDrawGraph == 0 ) { // Линия
        for ( size_t ir = 0; ir < nCvt; ++ir ) {
          rezGraphPnts[ir] = cvtPozs[ir];
        }
      }
      else { // Ежик
        for ( size_t ir = 0; ir < nCvt; ++ir ) {
          rezGraphPnts[ir * 3] = curvePnts[ir];
          rezGraphPnts[ir * 3 + 1] = cvtPozs[ir];
          rezGraphPnts[ir * 3 + 2] = curvePnts[ir];
        }
      }
    }
    else { // Вида F(x)
      MbVector3D vr2;
      double s = 0.;
      double h = 0.;

      if ( typeDrawGraph == 0 ) { // Линия
        vr = cvtPnts[0] - curvePnts[0];
        pnt.Init( 0., vr.Length() * scale, 0. );
        rezGraphPnts[0] = pnt;

        for ( size_t ir = 1; ir < nCvt; ++ir ) {
          //==
          vr2 = cvtPnts[ir] - curvePnts[ir];
          vTng = tngPnts[ir] - curvePnts[ir];
          vOrt = vr2 | vTng;

          vr = curvePnts[ir] - curvePnts[ir - 1];
          s = s + vr.Length();
          if ( vOrt.Length() > 0. ) {
            //==
            vr = cvtPnts[ir] - curvePnts[ir];
            h = vr.Length();
            pnt.Init( s, vr.Length() * scale, 0. );
            rezGraphPnts[ir] = pnt;
          }
          else {
            pnt.Init( s, 0., 0. );
            rezGraphPnts[ir] = pnt;
          }
        }
      } // Линия
      else { // Ежик
        vr = cvtPnts[0] - curvePnts[0];
        pnt.Init( 0., vr.Length() * scale, 0. );
        rezGraphPnts[0] = pnt;

        for ( size_t ir = 1; ir < nCvt; ++ir ) {
          //==
          vr2 = cvtPnts[ir] - curvePnts[ir];
          vTng = tngPnts[ir] - curvePnts[ir];
          vOrt = vr2 | vTng;

          vr = curvePnts[ir] - curvePnts[ir - 1];
          s = s + vr.Length();
          if ( vOrt.Length() > 0. ) {
            //==
            vr = cvtPnts[ir] - curvePnts[ir];
            h = vr.Length();
            pnt.Init( s, 0., 0. );
            rezGraphPnts[ir * 3] = pnt;
            pnt.Init( s, vr.Length() * scale, 0. );
            rezGraphPnts[ir * 3 + 1] = pnt;
            pnt.Init( s, 0., 0. );
            rezGraphPnts[ir * 3 + 2] = pnt;
          } //  if ( vOrt.Length() > 0. ) 
          else { // Нулевая кривизна
            pnt.Init( s, 0., 0. );
            rezGraphPnts[ir * 3] = pnt;
            pnt.Init( s, 0.0001, 0. );
            rezGraphPnts[ir * 3 + 1] = pnt;
            pnt.Init( s, 0., 0. );
            rezGraphPnts[ir * 3 + 2] = pnt;
          } // Нулевая кривизна
        } //for ( size_t ir = 1; ir < nCvt; ++ir ) {
      } //  Ежик
    } // Вида F(x)
#ifdef C3D_DEBUG_FAIR_CURVES
      //WrArrPnts( out, 2, cvtPnts, nCurve );
#endif
  } // Кривизна 

  // Записать график эволюты
  if ( typeGraph == 1 ) { // Эволюта
    size_t nEvolute = 0;
    nEvolute = evolutePnts.Count();
    // Нарисовать точки графика эволюты
    if ( typeDrawGraph == 0 ) { // Линия
      for ( size_t ir = 0; ir < nEvolute; ++ir ) {
        rezGraphPnts[ir] = evolutePnts[ir];
      }
    }
    else { // Ежик
      for ( size_t ir = 0; ir < nEvolute; ++ir ) {
        rezGraphPnts[ir * 3] = curvePnts[ir];
        rezGraphPnts[ir * 3 + 1] = evolutePnts[ir];
        rezGraphPnts[ir * 3 + 2] = curvePnts[ir];
      }
    }
#ifdef C3D_DEBUG_FAIR_CURVES
    //WrArrPnts( out, 3, evolutePnts, nEvolute );
#endif
  }

#ifdef C3D_DEBUG_FAIR_CURVES
  //fclose( out );
#endif

  return 0;
}


//------------------------------------------------------------------------------
// Препроцессор. Анализ конфигурации ломаной.
// Фильтрует совпадающие, выпадающие, находит количество локально выпуклых участков,
// минимальное количество точек на участке.
//===
int GeomPreProcessor( size_t inType,          // тип ГО (1 - ломаная, 2 - ГО Эрмита
                      bool smooth,            // режим сглаживания;
                      const MbCurve3D * pllne, // исходная ломаная;
                      size_t & n,             // количество точек рез ломаной (после сортировки);
                      size_t & nSites,        // количество выпуклых участков;
                      size_t & nStrSites,     // количество прямолинейных участков;
                      size_t & nMinPntSite,   // минимальное количество точек на участке.
                      MbCurve3D *& rezPllne ) // результирующая ломаная;
{
  nSites = 0; // количество выпуклых участков;
  nStrSites = 0; // количество прямолинейных участков;
  nMinPntSite = 0; // минимальное количество точек на участке.
  MbControlData3D cvrts;
  pllne->GetBasisPoints( cvrts );
  n = cvrts.Count();
  bool closed = pllne->IsClosed();
  SArray<MbCartPoint3D> polyLine; // Массив точек ломаной;

                                  //Перенести точки в polyline
  if ( inType == 1 ) {
    for ( size_t ir = 0; ir < n; ir++ ) {
      polyLine.Add( cvrts[ir] );
    }
  }
  if ( inType == 2 ) {
    for ( size_t ir = 0; ir < n; ir = ir + 5 ) {
      polyLine.Add( cvrts[ir] );
    }
    n = n / 5;
  }

  //Сортировка совпадающих точек.
  SArray<MbCartPoint3D> polyLineRezult;
  //polyLineRezult.resize(int(n));

  //Фильтрация совпадающих точек
  //DEBUG20220928 
  size_t j = 1;
  polyLineRezult.Add( polyLine[0] );
  for ( size_t i = 1; i < n; ++i ) {
    MbVector3D vr;
    vr = polyLine[i] - polyLine[i - 1];
    if ( vr.Length() < METRIC_ACCURACY )
      continue;
    polyLineRezult.Add( polyLine[i] );
    j++;

  }
  //m_arrayPoint[j] = m_arrayPoint[m_kArray - 1];
  n = j++;
  ::SplineCurve( polyLineRezult, closed, st_Polyline3D, rezPllne );

  // Проверка конфигурации
  SArray<MbCartPoint3D>  t;
  t.resize( (int)n );
  SArray<double>  s;
  s.resize( (int)n );
  SArray<int>  mpr;
  mpr.resize( (int)n );
  size_t i;
  double sr, ab, znak = 1.;
  for ( size_t ir = 0; ir < n; ++ir ) {
    t[ir] = polyLineRezult[ir];
  }
#ifdef C3D_DEBUG_FAIR_CURVES
  /*~*///      fprintf(prt, "\nBegin Struct3DPllneBissect\n");
       //  /*~*/      for (int ir = 0; ir < n; ++ir) {
       //    /*~*/         fprintf(prt, "ir = %d t =   %f,%f,%f\n",
       //      /*~*/            ir,
       //      /*~*/            t[ir][0], t[ir][1], t[ir][2]);
       //    /*~*/
       //  }
#endif

       //
       //  бл.2 первоначальное задание аппроксимаций главных кривизн (со знаками)
       //       в точках 1 ... N-2
       //
  size_t i0;
  MbVector3D vp, v, dv, v2, v3;
  MbVector3D vr0, vr1, vrOrt1, vrOrt2;
  MbCartPoint3D t1, t2;
  MbCartPoint3D bp[4];
  //==
  double delta_prm = METRIC_ACCURACY; // точность прямолинейности        
  if ( closed ) {
    DescreteCurvature( t[n - 2], t[0], t[1], delta_prm, vp, sr );
    s[0] = sr;
    i0 = 1;
    znak = 1.;
  }
  else {
    DescreteCurvature( t[0], t[1], t[2], delta_prm, vp, sr );
    s[0] = sr, s[1] = sr;
    i0 = 2;
    znak = 1.;
  }

  for ( i = i0; i < n - 1; ++i ) {
    DescreteCurvature( t[i - 1], t[i], t[i + 1], delta_prm, v, sr );
    ab = v * vp;
    if ( ab < 0. && sr != 0. ) {
      znak = znak * ( -1. );
    }
    s[i] = sr * znak;

    if ( s[i] != 0 ) {
      vp = v;
    }
  }

  // Без учета замкнутости.
  if ( !closed ) {
    s[n - 1] = s[n - 2];
  }
  else {
    DescreteCurvature( t[n - 2], t[n - 1], t[1], delta_prm, v, sr );
    ab = v * vp;
    if ( ab < -METRIC_ACCURACY ) {
      znak = znak * -1.;
    }

    s[n - 1] = sr * znak;
  }

#ifdef C3D_DEBUG_FAIR_CURVES
  fprintf( prt, "\nЗначения s[ir]\n" );
  for ( int ir = 0; ir < n; ++ir ) {
    fprintf( prt, "ir = %d s =  %f mpr = %d\n",
             ir, s[ir], mpr[ir] );
  }
#endif

  // Анализ прямолинейных участков
  //for (i = 1; i < n - 1; ++i) {
  //  if (s[i] == 0. ) { // Нет фиксированных касательных и перегибов.
  //    if (s[i - 1] * s[i + 1] < 0.0)
  //      continue; // Будет точка перегиба
  //                // Прямолинейный участок 
  //    nStrSites++; // Факт наличия прямолинейного участка.
  //  }
  //}
  SArray<MbCartPoint3D> polyLineNotStr;
  polyLineNotStr.clear();
  if ( smooth ) {
    //Удаление точек на прямых.
    j = 1;
    polyLineNotStr.Add( polyLineRezult[0] );

    for ( i = 1; i < n - 1; ++i ) {
      MbVector3D vr;
      if ( fabs( s[i] ) < METRIC_EPSILON )
        continue;
      polyLineNotStr.Add( polyLineRezult[i] );
      s[j] = s[i];
      j++;
    }
    polyLineNotStr.Add( polyLineRezult[n - 1] );
    s[j] = s[n - 1];
    n = j++;
    delete rezPllne;
    ::SplineCurve( polyLineNotStr, closed, st_Polyline3D, rezPllne );
  }

  //Проверка минимального количества точек на выпуклых участках
  SArray <size_t> deleteNumPnt;
  size_t max = n;
  size_t iMax = 0;
  size_t ir0 = 0;

  nSites = 1;
  nMinPntSite = n;
  for ( size_t ir = 0; ir < n - 2; ++ir ) {
    if ( s[ir] * s[ir + 1] < 0.0 ) {
      nSites++;
      if ( s[ir + 1] * s[ir + 2] < 0.0 ) {
        deleteNumPnt.Add( ir + 1 );
      }
      //iPrgb = ir - ir0 + 1;
      //if ( iPrgb < max ) {
      //  max = iPrgb;
      //  iMax = ir;
      //  //if ( max < 3 && ir > 1 && ir + 1 < n - 1)deleteNumPnt.Add( iMax );
      //  if ( max < 3 && ir > 1 && ir + 1 < n )deleteNumPnt.Add( iMax );
      //}
      ir0 = ir;
    }
  }

  if ( n - 1 - ir0 + 1 < max ) {
    max = n - 1 - ir0 + 1;
    iMax = ir0;
  }

  if ( max < 5 ) {  //DEBUG20220914
#ifdef C3D_DEBUG_FAIR_CURVES
    fprintf( prt, "***  количество точек на выпуклом участке = %d  номер = %d меньше минимального 5, включая концевые точки участка ", max, iMax );
#endif
}

  nMinPntSite = max;

  SArray<MbCartPoint3D> polyLineDel;
  polyLineDel.clear();
  size_t j0 = 0;
  size_t jk;
  size_t nk = polyLineNotStr.size();
  if ( deleteNumPnt.size() > 0 ) {
    for ( size_t ir = 0; ir < deleteNumPnt.size(); ++ir ) {
      jk = deleteNumPnt[ir];
      for ( size_t jr = j0; jr < jk; ++jr ) {
        polyLineDel.Add( polyLineNotStr[jr] );
      }
      j0 = jk + 1;
    }

    // Добрать до конца
    j0 = jk + 1;

    for ( size_t jr = j0; jr < nk; ++jr ) {
      polyLineDel.Add( polyLineNotStr[jr] );
    }
    // 
    delete rezPllne;
    ::SplineCurve( polyLineDel, closed, st_Polyline3D, rezPllne );
  }
  MbControlData3D rezCvrts;
  rezPllne->GetBasisPoints( rezCvrts );
  n = rezCvrts.Count();

#ifdef C3D_DEBUG_FAIR_CURVES
  fprintf( prt, "\nnSites = %d  nMinPntSite = %d\n", nSites, nMinPntSite );
#endif
  return 0;
}
#endif


#ifdef FAIR_CURVE_ADVANCED
//------------------------------------------------------------------------------
// Дискретная аппроксимация кривизны.
// ---
void DescreteCurvature( const MbCartPoint3D & t1,
                        const MbCartPoint3D & t2,
                        const MbCartPoint3D & t3,
                        double delta_prm,
                        MbVector3D & v,
                        double & s )
{
  MbVector3D v1, v2, dv, d2v, v1Ed, v2Ed;
  double a, b, c, d, r, sr;

  v1.Init( t1, t2 );
  v2.Init( t2, t3 );
  v = v1 | v2;

  dv = ( t3 - t1 ) * 0.5;
  d2v = ( t1 + t3 ) * 0.5 - t2;

  a = v.Length();

  v1Ed = v1;
  v2Ed = v2;
  v2Ed.Normalize();
  v1Ed.Normalize();
  s = ( v2Ed - v1Ed ).Length();

  if ( s < delta_prm * delta_prm ) {
    s = 0.;
    return;
  }

  if ( a != 0. ) {
    v /= a;
  }

  b = d2v[0] * dv[1] - dv[0] * d2v[1];
  c = d2v[1] * dv[2] - dv[1] * d2v[2];
  d = d2v[2] * dv[0] - dv[2] * d2v[0];

  a = b * b + c * c + d * d;

  r = dv.Length2();
  sr = a / ( r * r * r );
  s = ::sqrt( sr );

  r = ::fabs( s ) * ::sqrt( r );
  if ( r * r < delta_prm * delta_prm )
    s = 0.;

  // pегуляpизация условий пpямолинейности

}


//------------------------------------------------------------------------------
// ---
int Test_GeomPreProcessor() {
  bool closed;
  size_t n = 0;
  int outError = 0;

  c3d::SpaceCurveSPtr polyPtr = ReadPolyline( closed, n, outError );

  if ( polyPtr == nullptr )
    return outError;

  MbCurve3D * pllne = polyPtr.get();

  // Препроцессор. Анализ конфигурации ломаной.
  // Фильтрует совпадающие, находит количество локально выпуклых участков,
  // минимальное количество точек на участке.
  size_t nSites, nStrSites, nMinPntSite = 0;
  //MbeFairSmoothing inFairing, fairing = data.fairing;
  bool smooth = false;
  int res = 0;
  c3d::SpaceCurveSPtr newPolyPtr; // Для автоматического удаления полилинии, созданной препроцессором.
  {
    smooth = true;
    res = GeomPreProcessor( 1, smooth, pllne, n, nSites, nStrSites, nMinPntSite, pllne );
    newPolyPtr = pllne; // ГО - отфильтрованная ломаная без прямолинейных участков после GeomPreProcessor для сглаживания.
    // Без совпадающих и выпадающих точек.
  }

  if ( res == 0 ) { // Успешное завершение.
    // Нарисовать кривую.
    TestVariables::viewManager->AddObject( TestVariables::CURVE_Style, pllne );
  }
  else { // Регистрация ошибки.
    string_t message( IDS_TITLE_ERR );
    message = MessageError( fData.error ); // Формирует текст сообщения по номеру ошибки.
    ::MessageBoxEx( _T( "*** Объект не 3D Ломаная" ), IDS_TITLE_ERR );
  }
  outError = res;
  
  return outError;
}
#endif

