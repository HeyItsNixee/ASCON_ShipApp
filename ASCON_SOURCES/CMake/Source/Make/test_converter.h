///////////////////////////////////////////////////////
//
// Классы, функции и другие объекты обмена данными
//
// Classes, functions et.c. for data exchange
//
///////////////////////////////////////////////////////


#ifndef __CONV3DWR_H
#define __CONV3DWR_H


#include <system_types.h>
#include <alg_indicator.h>
#include <conv_predefined.h>
#include <conv_exchange_settings.h>
#include <iterator>


// \ru Cообщения индикатора прогресса выполнения 
// \en Execution progress indicator messages.
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CALC_MASS_INERTIA_PROPERTIES  _T("Расчет масс-инерционных характеристик")
#else
#define IDS_PBAR_CALC_MASS_INERTIA_PROPERTIES  _T("Mass-inertial properties calculation")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CALC_TRIANGULATION            _T("Расчет триангуляции")
#else
#define IDS_PBAR_CALC_TRIANGULATION            _T("Calculation of triangulation")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CNV_PARSE_DATA                _T("Синтаксический анализ...")
#else
#define IDS_PBAR_CNV_PARSE_DATA                _T("Syntactic analysis...")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CNV_CREATE_OBJECTS            _T("Создание объектов...")
#else
#define IDS_PBAR_CNV_CREATE_OBJECTS            _T("Objects creation...")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CNV_PROCESS_SURFACES          _T("Обработка поверхностей...")
#else
#define IDS_PBAR_CNV_PROCESS_SURFACES          _T("Surfaces processing...")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CNV_PROCESS_ANNOTATION        _T("Обработка аннотаций...")
#else
#define IDS_PBAR_CNV_PROCESS_ANNOTATION        _T("Annotation processing...")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CNV_CREATE_MODEL              _T("Создание модели...")
#else
#define IDS_PBAR_CNV_CREATE_MODEL              _T("Model creation...")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CNV_WRITE_MODEL               _T("Запись модели...")
#else
#define IDS_PBAR_CNV_WRITE_MODEL               _T("Writing a model...")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_READ_DATA                     _T("Чтение данных")
#else
#define IDS_PBAR_READ_DATA                     _T("Data reading")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_PREPARE_DATA                  _T("Подготовка данных")
#else
#define IDS_PBAR_PREPARE_DATA                  _T("Data preparation")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_PROCESS_DATA                  _T("Обработка данных")
#else
#define IDS_PBAR_PROCESS_DATA                  _T("Data processing")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_FINISH_DATA                   _T("Завершение обработки данных")
#else
#define IDS_PBAR_FINISH_DATA                   _T("Finish of data processing")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_DRAW_DATA                     _T("Отображение данных")
#else
#define IDS_PBAR_DRAW_DATA                     _T("Data mapping")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_WRITE_DATA                    _T("Запись данных")
#else
#define IDS_PBAR_WRITE_DATA                    _T("Data writing")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_SOLVE_LINEAR_EQUATIONS_SYSTEM _T("Решение системы линейных уравнений")
#else
#define IDS_PBAR_SOLVE_LINEAR_EQUATIONS_SYSTEM _T("Solving a system of linear equations")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_BUILD_SHELL_BY_POINTS_MESH     _T("Построение оболочки по сети точек")
#else
#define IDS_PBAR_BUILD_SHELL_BY_POINTS_MESH     _T("Building a shell from a mesh of points")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_BUILD_POINTS_CLOUD_MESH       _T("Построение сети точек по пласту точек")
#else
#define IDS_PBAR_BUILD_POINTS_CLOUD_MESH       _T("Building a mesh of points from a cloud of points")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_REMOVE_REDUNDANT_POINTS       _T("Удаление избыточных точек")
#else
#define IDS_PBAR_REMOVE_REDUNDANT_POINTS       _T("Removal of redundant points")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_BUILD_TRIANGLE_FACES          _T("Построение треугольных граней")
#else
#define IDS_PBAR_BUILD_TRIANGLE_FACES          _T("Building triangular faces")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_FIND_ADJACENT_EDGES           _T("Поиск смежных ребер")
#else
#define IDS_PBAR_FIND_ADJACENT_EDGES           _T("Search adjacent edges")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_BUILD_TRIANGLES_SHELL         _T("Построение оболочки из треугольных граней")
#else
#define IDS_PBAR_BUILD_TRIANGLES_SHELL         _T("Building a shell from triangular faces")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_PREPARE_SURFACE_DATA          _T("Подготовка данных для построения поверхности")
#else
#define IDS_PBAR_PREPARE_SURFACE_DATA          _T("Preparing data for surface building")
#endif
#if defined ( __NATIVE_LANGUAGE__ )
#define IDS_PBAR_CHECK_SURF                    _T("Проверка правильности построения поверхности")
#else
#define IDS_PBAR_CHECK_SURF                    _T("Check correctness of surface building")
#endif


//------------------------------------------------------------------------------
// \ru proxy (заместитель) для конвертора 
// \en proxy for converter
// ---
class WrConvertor3D {
public:
  static bool      SATWrite ( const TCHAR * fileName );
  static bool      SATRead  ( const TCHAR * fileName );
  static bool      IGESWrite( const TCHAR * fileName );
  static bool      IGESRead ( const TCHAR * fileName );
  static bool      XTWrite  ( const TCHAR * fileName );
  static bool      XTRead   ( const TCHAR * fileName );
  static bool      STEPWrite( const TCHAR * fileName );
  static bool      STEPRead ( const TCHAR * fileName ); 
  static bool      JTRead   ( const TCHAR * fileName ); 
  static bool      JTWrite  ( const TCHAR * fileName ); 
  static bool      STLWrite ( const TCHAR * fileName );
  static bool      STLRead  ( const TCHAR * fileName );
  static bool      VRMLWrite( const TCHAR * fileName );
  static bool      VRMLRead ( const TCHAR * fileName );
  static bool      OBJRead  ( const TCHAR* fileName );
  static bool      GRDECLRead ( const TCHAR * fileName );
  static bool      GRDECLWrite ( const TCHAR * fileName );
  static bool      ASCIIPointCloudRead ( const TCHAR * fileName );
  static bool      ASCIIPointCloudWrite ( const TCHAR * fileName );
  static bool      PluginRead( const TCHAR* fileName );
  static bool      SetModelName( const TCHAR * fileName );

OBVIOUS_PRIVATE_COPY( WrConvertor3D )
};

//------------------------------------------------------------------------------
// \ru Посетитель, извлекающий строку 
// \en Visitor extracting a string
// ---
class StrSpyCommon : public BaseStrVisitor, public VisitorImpl<MbeProgBarId_Common> {
public:
  StrSpyCommon() : BaseStrVisitor() {}
  virtual ~StrSpyCommon();
  virtual void Visit( MbeProgBarId_Common & strId );
};


//------------------------------------------------------------------------------
// \ru Посетитель, извлекающий строку 
// \en Visitor extracting a string
// ---
class StrSpyPointsSurface : public BaseStrVisitor, public VisitorImpl<MbeProgBarId_PointsSurface> {
public:
  StrSpyPointsSurface() : BaseStrVisitor() {}
  virtual ~StrSpyPointsSurface();
  virtual void Visit( MbeProgBarId_PointsSurface & strId );
};


//------------------------------------------------------------------------------
// \ru Посетитель, извлекающий строку 
// \en Visitor extracting a string
// ---
class StrSpyConverters : public BaseStrVisitor, public VisitorImpl<MbeProgBarId_Converters> {
public:
  StrSpyConverters() : BaseStrVisitor() {}
  virtual ~StrSpyConverters();
  virtual void Visit( MbeProgBarId_Converters & strId );
};


//------------------------------------------------------------------------------
// \ru Посетитель, извлекающий строку
// \en Visitor extracting a string
// ---
class StrSpyTriangulation : public BaseStrVisitor, public VisitorImpl<MbeProgBarId_Triangulation> {
public:
  StrSpyTriangulation() : BaseStrVisitor() {}
  virtual ~StrSpyTriangulation();
  virtual void Visit( MbeProgBarId_Triangulation & strId );
};


//------------------------------------------------------------------------------
// \ru Посетитель, извлекающий строку 
// \en Visitor extracting a string
// ---
class StrSpyMIP : public BaseStrVisitor, public VisitorImpl<MbeProgBarId_MassInertiaProperties> {
public:
  StrSpyMIP() : BaseStrVisitor() {}
  virtual ~StrSpyMIP();
  virtual void Visit( MbeProgBarId_MassInertiaProperties & strId );
};


//------------------------------------------------------------------------------
// \ru создать индикатор прогресса выполнения 
// \en create execution progress indicator
// ---
IProgressIndicator & CreateProgressIndicator( BaseStrVisitor * );

//------------------------------------------------------------------------------
// Распознать булeво значение и обновить в соответствие с нотацией
// ---
bool UpdateBool( const c3d::string_t & key, bool & value );

//------------------------------------------------------------------------------
// Получить пользовательские настройки конвертера
// Файл c3d_test.config должен лежать рядом с файлом лицензии.
// ---
bool UpdateConverterSettings( ConvConvertorProperty3D & convProperties, bool& bPmiToItems );

#endif

