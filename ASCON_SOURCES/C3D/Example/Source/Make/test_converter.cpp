#include <test_converter.h>
#include <test_manager.h>
#include <test_service.h>
#include <test_computation.h>
#include <test.h>
#include <test_variables.h>
#include <system_atomic.h>
#include <action_solid.h>
#include <action_shell.h>
#include <assembly.h>
#include <instance_item.h>
#include <space_instance.h>
#include <plane_instance.h>
#include <reference_item.h>
#include <surface.h>
#include <solid.h>
#include <wire_frame.h>
#include <point_frame.h>
#include <cur_polyline.h>
#include <conv_model_exchange.h>
#include <conv_exchange_settings.h>
#include <conv_model_document.h>
#include <conv_requestor.h>
#include <attr_color.h>
#include <attr_common_attribute.h>
#include <list>
#include <sstream>
#include <stdio.h>
#include <math_namespace.h>
#include <last.h>

using namespace c3d;

//------------------------------------------------------------------------------
// \ru Раскомментируйте этот define, чтобы установить нужную локаль при экспорте
// \en Uncomment this define to specify locale for export
// ---

//#define SET_LOCALE_OBVIOUSLY

//------------------------------------------------------------------------------
// \ru Эта локаль бутед явно установлена при экспорте при определённом SET_LOCALE_OBVIOUSLY
//     Например: "russian"
// \en This locale will be used for export if SET_LOCALE_OBVIOUSLY is defined
//     Example: "russian"
// ---
#define SPECIFIC_LOCALE "C"



//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
StrSpyCommon::~StrSpyCommon() {
}

//------------------------------------------------------------------------------
// Посетитель, извлекающий строку
// Visitor extracting the string
// ---
void StrSpyCommon::Visit( MbeProgBarId_Common & strId )
{
  switch ( strId ) {
    case pbarId_Read_Data     : Data().assign( IDS_PBAR_READ_DATA );    break;
    case pbarId_Prepare_Data  : Data().assign( IDS_PBAR_PREPARE_DATA ); break;
    case pbarId_Process_Data  : Data().assign( IDS_PBAR_PROCESS_DATA ); break;
    case pbarId_Finish_Data   : Data().assign( IDS_PBAR_FINISH_DATA );  break;
    case pbarId_Draw_Data     : Data().assign( IDS_PBAR_DRAW_DATA );    break;
    case pbarId_Write_Data    : Data().assign( IDS_PBAR_WRITE_DATA );   break;
    default: break;
  }
}


//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
StrSpyPointsSurface::~StrSpyPointsSurface() {
}


//------------------------------------------------------------------------------
// Посетитель, извлекающий строку
// Visitor extracting the string
// ---
void StrSpyPointsSurface::Visit( MbeProgBarId_PointsSurface & strId )
{
  switch ( strId ) {
    case pbarId_Solve_LinearEquationsSystem : Data().assign(IDS_PBAR_SOLVE_LINEAR_EQUATIONS_SYSTEM); break;
    case pbarId_Remove_RedundantPoints      : Data().assign(IDS_PBAR_REMOVE_REDUNDANT_POINTS);       break;
    case pbarId_Build_ShellByPointsMesh     : Data().assign(IDS_PBAR_BUILD_SHELL_BY_POINTS_MESH);    break;
    case pbarId_Build_PointsCloudMesh       : Data().assign(IDS_PBAR_BUILD_POINTS_CLOUD_MESH);       break;
    case pbarId_Build_TriangleFaces         : Data().assign(IDS_PBAR_BUILD_TRIANGLE_FACES);          break;
    case pbarId_Find_AdjacentEdges          : Data().assign(IDS_PBAR_FIND_ADJACENT_EDGES);           break;
    case pbarId_Build_TrianglesShell        : Data().assign(IDS_PBAR_BUILD_TRIANGLES_SHELL);         break;
    case pbarId_Prepare_SurfaceData         : Data().assign(IDS_PBAR_PREPARE_SURFACE_DATA);          break;
    case pbarId_Check_Surface               : Data().assign(IDS_PBAR_CHECK_SURF);                    break;
    default: break;
  }
}


//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
StrSpyConverters::~StrSpyConverters() {
}


//------------------------------------------------------------------------------
// Посетитель, извлекающий строку
// Visitor extracting the string
// ---
void StrSpyConverters::Visit( MbeProgBarId_Converters & strId )
{
  switch ( strId ) {
    case pbarId_Cnv_Parse_Data         : Data().assign(IDS_PBAR_CNV_PARSE_DATA);          break;
    case pbarId_Cnv_Create_Objects     : Data().assign(IDS_PBAR_CNV_CREATE_OBJECTS);      break;
    case pbarId_Cnv_Process_Surfaces   : Data().assign(IDS_PBAR_CNV_PROCESS_SURFACES);    break;
    case pbarId_Cnv_Process_Annotation : Data().assign(IDS_PBAR_CNV_PROCESS_ANNOTATION);  break;
    case pbarId_Cnv_Create_Model       : Data().assign(IDS_PBAR_CNV_CREATE_MODEL);        break;
    case pbarId_Cnv_Write_Model        : Data().assign(IDS_PBAR_CNV_WRITE_MODEL);         break;
    default: break;
  }
}


//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
StrSpyTriangulation::~StrSpyTriangulation() {
}


//------------------------------------------------------------------------------
// Посетитель, извлекающий строку
// Visitor extracting the string
// ---
void StrSpyTriangulation::Visit( MbeProgBarId_Triangulation & strId )
{
  switch ( strId ) {
    case pbarId_Calc_Triangulation : Data().assign( IDS_PBAR_CALC_TRIANGULATION ); break;
    default: break;
  }
}


//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
StrSpyMIP::~StrSpyMIP() {
}


//------------------------------------------------------------------------------
// Посетитель, извлекающий строку
// Visitor extracting the string
// ---
void StrSpyMIP::Visit( MbeProgBarId_MassInertiaProperties & strId )
{
  switch ( strId ) {
    case pbarId_Calc_MassInertiaProperties : Data().assign( IDS_PBAR_CALC_MASS_INERTIA_PROPERTIES ); break;
    default: break;
  }
}


//------------------------------------------------------------------------------
// Реализация получения строки
// Implementation of the string getter
// ---
class GetMsgImp {
protected:
  BaseStrVisitor * strSpy;
public:
  GetMsgImp() : strSpy( new BaseStrVisitor ) {}
  virtual ~GetMsgImp();
public:
  virtual const TCHAR * Msg( IStrData & ) const;
          void          SetStrSpy( BaseStrVisitor * );
};


//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
GetMsgImp::~GetMsgImp()
{
  if ( strSpy != nullptr ) {
    delete strSpy;
    strSpy = nullptr;
  }
}


//------------------------------------------------------------------------------
// Получение строки
// Get message string
// ---
const TCHAR * GetMsgImp::Msg( IStrData & strData ) const {
  return strData.Accept( *strSpy ) ? strSpy->GetString() : nullptr;
}


//------------------------------------------------------------------------------
// Установка посетителя
// Set the visitor
// ---
void GetMsgImp::SetStrSpy( BaseStrVisitor * newStrSpy )
{
  if ( newStrSpy != nullptr && strSpy != newStrSpy ) {
    delete strSpy;
    strSpy = newStrSpy;
  }
}


//------------------------------------------------------------------------------
// Реализация индикатора прогресса выполнения
// Implementation of the process and feedback indicator
// ---
class ProgressIndicatorImp : public IProgressIndicator, public GetMsgImp, public MbSyncItem {
private:
  serial_type count;
  string_t    resMsg;
  string_t    curMsg;
  atomic_bool cancel;
public:
  ProgressIndicatorImp() : count( 0 ), resMsg(), curMsg(), cancel( false ) {}
  virtual ~ProgressIndicatorImp();

public:
  virtual bool          Initialize( size_t range, size_t delta, IStrData & msg ); // Установка диапазона индикации, сброс состояния
  virtual bool          Progress  ( size_t n );       // Обработать прогресс на n у.е. Вернет false - пора останавливаться
  virtual void          Success   ();                 // Ликвидация ошибок округления дорастим прогресс бар до 100%
  virtual bool          IsCancel  ();                 // Проверка не пора ли остановиться
  virtual void          SetCancel ( bool c );         // Скажем, что пора остановиться
  virtual void          Stop      ();                 // Команда пора остановиться
  virtual const TCHAR * Msg( IStrData & msg ) const;  // Получить строку

private:
  ProgressIndicatorImp( class ProgressIndicatorImp & );
  void operator =  ( class ProgressIndicatorImp & );
};


//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
ProgressIndicatorImp::~ProgressIndicatorImp() {
}


//------------------------------------------------------------------------------
// Получить строку
// Get a string to indicate a state
// ---
const TCHAR * ProgressIndicatorImp::Msg( IStrData & strData ) const {
  return GetMsgImp::Msg( strData );
}

//------------------------------------------------------------------------------
// Послать строку в виде сообщения (потокобезопасная версия)
// Send a string as a message (thread-safe version)
// ---
void ShowStringSafe( const TCHAR * str )
{
  if ( !omp_in_parallel() || omp_get_thread_num() == 0 /*главный поток*/ )
    ::ShowString( str );
}

//------------------------------------------------------------------------------
// Установка диапазона индикации, сбросить состояние в 0
//
// Initialize range of the indicator and set it to the beginning
// ---
bool ProgressIndicatorImp::Initialize( size_t, size_t, IStrData & strData )
{
  Lock();
  count = 0;
  cancel = false;
  const TCHAR* rawMsg = Msg( strData );
  if ( nullptr != rawMsg )
    curMsg.assign( rawMsg );
  if ( curMsg.length() > 0 ) {
    TCHAR buff[20];
    resMsg = curMsg;
    resMsg += _T(" : ");
    _sntprintf( buff, 20, F_TD, count.load() );
    resMsg += string_t( buff );
  }
  Unlock();
  if ( curMsg.length() > 0 )
    ::ShowStringSafe( (const TCHAR *)resMsg.c_str() );
  return true;
}


//------------------------------------------------------------------------------
// Обработать прогресс на n единиц. Вернет false - пора останавливаться
//
// Indicate progress by n units. If the returned value is false, converter 
// will stop the process
// ---
bool ProgressIndicatorImp::Progress( size_t n )
{
  Lock();
  count += n;
  TCHAR buff[20];
  resMsg = curMsg;
  resMsg += _T(" : ");
  _sntprintf( buff, 20, F_TD, count.load() );
  resMsg += string_t( buff );
  Unlock();
  ::ShowStringSafe( (const TCHAR *)resMsg.c_str() );

  return !IsCancel();
}


//------------------------------------------------------------------------------
// Ликвидация ошибок округления дорастим прогресс бар до 100%
// Set the indicator to the "complete" state
// ---
void ProgressIndicatorImp::Success()
{
  Lock();
  count = 0;
  resMsg = curMsg;
  resMsg += string_t( RT_IS_FINISIED );
  Unlock();
  ::ShowStringSafe( (const TCHAR *)resMsg.c_str() );
}


//------------------------------------------------------------------------------
// Команда пора остановиться
// Здесь очистка строки состояния
// Set the indicator to the "stop" state
// ---
void ProgressIndicatorImp::Stop()
{
  if ( cancel )
    return;
  string_t emptyMsg;
  size_t strLen = resMsg.length();
  for ( size_t k = 0; k <= strLen; k++ )
    emptyMsg += _T(" ");

  ::ShowStringSafe( emptyMsg.c_str() );
}


//------------------------------------------------------------------------------
// Проверка не пора ли остановиться
// Is it necessary to cancel the operation
// ---
bool ProgressIndicatorImp::IsCancel()
{
  if ( !cancel ) {
    if ( PeekEscape() )
      SetCancel( true );
  }
  return cancel;
}


//------------------------------------------------------------------------------
// Скажем, что пора остановиться.
// Set the indicator to the "stop" state
// ---
void ProgressIndicatorImp::SetCancel( bool c ) {
  cancel = c;
}


//------------------------------------------------------------------------------
// Создать индикатор прогресса выполнения
// Create the progress indicatior
// ---
IProgressIndicator & CreateProgressIndicator( BaseStrVisitor * strSpy ) {
  ProgressIndicatorImp * progInd = new ProgressIndicatorImp;
  progInd->SetStrSpy( strSpy );
  return *progInd;
}


////////////////////////////////////////////////////////////////////////////////
//
// Интерфейс запроса масштаба
//
// Scale requestor
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Интерфейс запроса масштаба
// Scale requestor
// ---
class ScaleRequestorImp : public IScaleRequestor {
public:
  ScaleRequestorImp()  { }
  virtual double        ScaleRequest   () { return 1.0; }
};


////////////////////////////////////////////////////////////////////////////////
//
// WrConvertor3D
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
//
// ---
class LogReporter {
  string_t fileName;  ///< \ru Имя файла. \en File name. \~
  t_ofstream errOut;  ///< \ru Рабочий поток. \en Stream to operate with. \~
  std::map< eMsgDetail, string_t > errorMessages; ///< \ru Строки сообщений трансляции. \en Translation mesages strings. \~

public:

  // Конструктор
  LogReporter( const string_t &ofn );
  ~LogReporter();
  bool InitErrOut();
  void LogReport( ptrdiff_t id, eMsgType msgType, eMsgDetail msgText );

  void LogReport( std::vector<ConvConvertorProperty3D::LogRecord> records );
};


//------------------------------------------------------------------------------
// Конструктор
// Constructor
// ---
LogReporter::LogReporter( const string_t &ofn ) 
  : fileName( ofn )
  , errOut()
  , errorMessages()
{
  // Общие сообщения
  // Common messages
  errorMessages[emd_Title] =                          RT_EMD_TITLE;
  errorMessages[emd_HEADError] =                      RT_EMD_HEAD_ERROR;
  errorMessages[emd_HEADWarinig] =                    RT_EMD_HEAD_WARNING;
  errorMessages[emd_HEADInfo] =                       RT_EMD_HEAD_INFO;
  errorMessages[emd_HEADDefaultMsg] =                 RT_EMD_HEAD_DEFAULT_MSG;

  // Прекращение импорта
  // Errors which cause immediate stop of the import process
  errorMessages[emd_STOPFileOpenError] =              RT_EMD_STOP_FILE_OPEN_ERROR;
  errorMessages[emd_STOPFileOpenErrorOrEmpty] =       RT_EMD_STOP_FILE_OPEN_ERROR_OR_EMPTY;
  errorMessages[emd_STOPHeaderReadError] =            RT_EMD_STOP_HEADER_READ_ERROR;
  errorMessages[emd_STOPNoOrBadData] =                RT_EMD_STOP_NO_OR_BAD_DATA;
  errorMessages[emd_STOPIncorrectStructure] =         RT_EMD_STOP_INCORRECT_STRUCTURE;
  errorMessages[emd_STOPAddressConflict] =            RT_EMD_STOP_ADDRESS_CONFLICT;

  // Общеконвертерные ошибки
  // Common errors which do not cause immediate stop of the import process
  errorMessages[emd_ErrorNoRootObject] =              RT_EMD_ERROR_NO_ROOT_OBJECT;
  errorMessages[emd_ErrorSyntaxIncorrectFormFloat] =  RT_EMD_ERROR_SYNTAX_INCORRECT_FORM_FLOAT;
  errorMessages[emd_ErrorEmptyLoop] =                 RT_EMD_ERROR_EMPTY_LOOP;
  errorMessages[emd_ErrorEmptyQueriesList] =          RT_EMD_ERROR_EMPTY_QUERIES_LIST;
  errorMessages[emd_ErrorEmptyObjectsList] =          RT_EMD_ERROR_EMPTY_OBJECTS_LIST;
  errorMessages[emd_ErrorEmptyGeomObjectsList] =      RT_EMD_ERROR_EMPTY_GEOM_OBJECTS_LIST;
  errorMessages[emd_ErrorEmptyShellsList] =           RT_EMD_ERROR_EMPTY_SHELLS_LIST;
  errorMessages[emd_ErrorEmptyCurveCompositesList] =  RT_EMD_ERROR_EMPTY_CURVE_COMPOSITIES_LIST;
  errorMessages[emd_ErrorEmptyBoundCurvesList] =      RT_EMD_ERROR_EMPTY_BOUND_CURVES_LIST;
  errorMessages[emd_ErrorEmptyEdgeList] =             RT_EMD_ERROR_EMPTY_EDGE_LIST;
  errorMessages[emd_ErrorEmptyFacesList] =            RT_EMD_ERROR_EMPTY_FACES_LIST;
  errorMessages[emd_ErrorEmptyReferencesList] =       RT_EMD_ERROR_EMPTY_REFERENCES_LIST;
  errorMessages[emd_ErrorEmptyListOfWrieframes] =     RT_EMD_ERROR_EMPTY_LIST_OF_WIREFRAMES;
  errorMessages[emd_ErrorEmptyOrMore2ReferencesList] =RT_EMD_ERROR_EMPTY_OR_MORE_2_REFERENCES_LIST;
  /*  errorMessages[emd_ErrorUndefinedRef] =              RT_EMD_ERROR_UNDIFINED_REF;*/
  errorMessages[emd_ErrorUndefinedFaceSurfaceRef] =   RT_EMD_ERROR_UNDIFINED_FACE_SURFACE_REF;
  errorMessages[emd_ErrorUndefinedBaseCurveRef] =     RT_EMD_ERROR_UNDIFINED_BASE_CURVE_REF;
  errorMessages[emd_ErrorRadiusTooCloseToZero] =      RT_EMD_ERROR_RADIUS_TOO_CLOSE_TO_ZERO;
  errorMessages[emd_ErrorRadiusValueNegative] =       RT_EMD_ERROR_RADIUS_VALUE_NEGATIVE;
  errorMessages[emd_ErrorEllipseAxisTooCloseToZero] = RT_EMD_ERROR_ELLIPSE_AXIS_TOO_CLOSE_TO_ZERO;
  errorMessages[emd_ErrorEllipseAxisNegative] =       RT_EMD_ERROR_ELLIPSE_AXIS_NEGATIVE;
  errorMessages[emd_ErrorNegativeDegree] =            RT_EMD_ERROR_NEGATIVE_DEGREE;
  errorMessages[emd_ErrorNegativeUDegree] =           RT_EMD_ERROR_NEGATIVE_U_DEGREE;
  errorMessages[emd_ErrorNegativeVDegree] =           RT_EMD_ERROR_NEGATIVE_V_DEGREE;
  errorMessages[emd_ErrorDegreeFixImpossible] =       RT_EMD_ERROR_DEGREE_FIX_IMPOSIBLE;
  errorMessages[emd_ErrorPolylinePointListLess2] =    RT_EMD_ERROR_POLYLINE_POINT_LIST_LESS_2;
  errorMessages[emd_ErrorPointListLess2] =            RT_EMD_ERROR_POINT_LIST_LESS_2;
  errorMessages[emd_ErrorKnotsListLess2] =            RT_EMD_ERROR_KNOTS_LIST_LESS_2;
  errorMessages[emd_ErrorWeightsListLess2] =          RT_EMD_ERROR_WEIGHTS_LIST_LESS2;
  errorMessages[emd_ErrorUPointListLess2] =           RT_EMD_ERROR_U_POINTS_LIST_LESS_2;
  errorMessages[emd_ErrorUKnotsListLess2] =           RT_EMD_ERROR_U_KNOTS_LEST_LESS_2;
  errorMessages[emd_ErrorUWeightsListLess2] =         RT_EMD_ERROR_U_WEIGTHS_LIST_LESS_2;
  errorMessages[emd_ErrorVPointListLess2] =           RT_EMD_ERROR_V_POINTS_LIST_LESS_2;
  errorMessages[emd_ErrorVKnotsListLess2] =           RT_EMD_ERROR_V_KNOTS_LEST_LESS_2;
  errorMessages[emd_ErrorVWeightsListLess2] =         RT_EMD_ERROR_V_WEIGTHS_LIST_LESS_2;
  errorMessages[emd_ErrorListsSizeMismatch] =         RT_EMD_ERROR_LISTS_SIZE_MISMATCH;
  errorMessages[emd_ErrorKnotsWeightsListsOrderMismatch] =               RT_EMD_ERROR_KNOTS_WEIGHTS_LISTS_ORDER_MISMATCH;
  errorMessages[emd_ErrorKnotsWeightsListsSizeMismatch] =                RT_EMD_ERROR_KNOTS_WEIGHTS_LISTS_SIZE_MISMATCH;
  errorMessages[emd_ErrorUKnotsWeightsListsSizeMismatch] =               RT_EMD_ERROR_U_KNOTS_WEIGHTS_LISTS_SIZE_MISMATCH;
  errorMessages[emd_ErrorVKnotsWeightsListsSizeMismatch] =               RT_EMD_ERROR_V_KNOTS_WEIGHTS_LISTS_SIZE_MISMATCH;
  errorMessages[emd_ErrorSplineCurveNotCreatedUndefinedKnotsVector] =    RT_EMD_ERROR_SPLINE_CURVE_NOTCREATED_UNDIFINED_KNOTS_VECTOR;
  errorMessages[emd_ErrorSplineSurfaceNotCreatedUndefinedKnotsVectors] = RT_EMD_ERROR_SPLINE_SURFACE_NOT_CREATED_UNDIFINED_KNOTSVECTORS;
  errorMessages[emd_ErrorInCorrectSplineSurfaceData] =                   RT_EMD_ERROR_INCORRECT_SPLINE_SURFACE_DATA;  

  // Общеконвертерные предупреждения
  // Common warning messages
  errorMessages[emd_WarningNoSectionTerminator] =             RT_EMD_WARNING_NO_SECTION_TERMINATOR;
  errorMessages[emd_WarningSyntaxMultipleDotInFloat] =        RT_EMD_WARNING_SYNTAX_MULTIPLE_DOT_IN_FLOAT;
  errorMessages[emd_WarningSyntaxMultipleEInFloat] =          RT_EMD_WARNING_SYNTAX_MULTIPLE_E_IN_FLOAT;
  errorMessages[emd_WarningLoopNotClosed] =                   RT_EMD_WARNING_LOOP_NOT_CLOSED;
  errorMessages[emd_WarningContourNotClosed] =                RT_EMD_WARNING_CONTOUR_NOT_CLOSED;  
  errorMessages[emd_WarningUndefinedRef] =                    RT_EMD_WARNING_UNDIFINED_REF;
  errorMessages[emd_WarningToroidalSurfaceDegenerated] =      RT_EMD_WARNING_TOROIDAL_SURFACE_DEGENERATED;
  errorMessages[emd_WarningUndefinedBasisCurve] =             RT_EMD_WARNING_UNDIFINED_BASIS_CURVE;
  errorMessages[emd_WarningUndefinedSweptCurve] =             RT_EMD_WARNING_UNDIFINED_SWEPT_CURVE;
  errorMessages[emd_WarningUndefinedExtrusionDirection] =     RT_EMD_WARNING_UNDIFINED_EXTURSION_DIRECTION;
  errorMessages[emd_WarningUndefinedAxis] =                   RT_EMD_WARNING_UNDIFINED_AXIS;
  errorMessages[emd_WarningUndefinedAxisOfRevolution] =       RT_EMD_WARNING_UNDIFINED_AXIS_OF_REVOLUTION;
  errorMessages[emd_WarningUndefinedBasisSurface] =           RT_EMD_WARNING_UNDIFINED_BASIS_SURFACE;
  errorMessages[emd_WarningUndefinedRepresentation] =         RT_EMD_WARNING_UNDIFINED_REPRESENTATIOV;
  errorMessages[emd_WarningUndefinedTransformationOperator] = RT_EMD_WARNING_UNDIFINED_TRANSFORMATION_OPERATOR;
  errorMessages[emd_WarningUndefinedObjectTransformBy] =      RT_EMD_WARNING_UNDIFINED_OBJECT_TRANSFORM_BY;
  errorMessages[emd_WarningUndefinedObjectToTransform] =      RT_EMD_WARNING_UNDIFINED_OBJECT_TO_TRANSFORM;
  errorMessages[emd_WarningUndefinedCurve] =                  RT_EMD_WARNING_UNDIFINED_CURVE;
  errorMessages[emd_WarningUndefinedCompositeSegment] =       RT_EMD_WARNING_UNDIFINED_COMPOSITE_SEGMENT;
  errorMessages[emd_WarningUndefinedDirection] =              RT_EMD_WARNING_UNDIFINED_DIRECTION;
  errorMessages[emd_WarningUndefinedAxisDirection] =          RT_EMD_WARNING_UNDIFINED_AXIS_DIRECTION;
  errorMessages[emd_MessageWeightsFilled] =                   RT_EMD_MESSAGE_WEIGHTS_FILLED;
  errorMessages[emd_WarningFloatParceFailureDefaultUsed] =    RT_EMD_MESSAGE_FLOAT_PARSE_FAILURE_REPLACE_DEFAULT;
  errorMessages[emd_WarningIncorrectFaceWasNotAddedToShell] = RT_EMD_WARNING_INCORRECT_FACE_WAS_NOT_ADDED_TO_SHELL;
  errorMessages[emd_WarningSameShapeEdgeTwiceInLoop] =        RT_EMD_WARNING_SAME_EDGES_TWICE_IN_LOOP;
  errorMessages[emd_WarningVRMLGridDuplicatesInMeshes] =      RT_EMD_WARNING_VRML_GRID_DUPLICATES_IN_MESHES;
  errorMessages[emd_WarningACISLawIntCurveIsNotCreated] =     RT_EMD_WARNING_ACIS_LAW_INT_CURVE_IS_NOT_CREATED;
  errorMessages[emd_WarningIntCurveWasReplacedBySegment] = RT_EMD_WARNING_INT_CURVE_WAS_REPLACED_BY_SEGMENT;
  errorMessages[emd_WarningCreateEdgeBadInput] = RT_EMD_WARNING_INTERSECTION_CURVE_BAD_UNPUT;

  // Сообщения конвертера STEP
  // Common info messages
  errorMessages[emd_ErrorEdgeCurveFlagTSingleRedefinition] =  RT_EMD_ERROR_EDGE_CUVE_FLAG_T_SINGLE_REDEFINITION;
  errorMessages[emd_ErrorEdgeCurveFlagFSingleRedefinition] =  RT_EMD_ERROR_EDGE_CUVE_FLAG_F_SINGLE_REDEFINITION;
  errorMessages[emd_ErrorSTEPEdgeCurveFlagTMultipleRedefinition] =RT_EMD_ERROR_STEP_EDGE_CUVE_FLAG_T_MULTIPLE_REDEFINITION;
  errorMessages[emd_ErrorSTEPEdgeCurveFlagFMultipleRedefinition] =RT_EMD_ERROR_STEP_EDGE_CUVE_FLAG_F_MULTIPLE_REDEFINITION;
  errorMessages[emd_ErrorSTEPUndefinedFaceGeometry] =             RT_EMD_ERROR_STEPUNDIFINED_FACE_GEOMETRY;
  errorMessages[emd_ErrorSTEPSyntaxMultipleDotInEnum] =           RT_EMD_ERROR_STEP_SYNTAX_MULTIPLE_DOT_IN_ENUM;
  errorMessages[emd_WarningSTEPPointCorrection] =                 RT_EMD_WARNING_STEP_POINT_CORRECTION;
  errorMessages[emd_WarningSTEPEdgeCurveByVertices] =             RT_EMD_WARNING_STEP_EDGE_CERVE_BY_VERTICES;
  errorMessages[emd_MessageSTEPFlagChangedToF] =                  RT_EMD_MESSAGE_STEP_FLAG_CHANGED_TO_F;
  errorMessages[emd_MessageSTEPFlagChangedToT] =                  RT_EMD_MESSAGE_STEP_FLAG_CHANGED_TO_T;

  // Сообщения конвертера SAT
  // SAT converter messages
  errorMessages[emd_WarningACISUnsupportedInterpoleCurveType] =   RT_EMD_WARNING_ACIS_UNSUPPORTED_INTERPOLE_CURVE_TYPE;
  errorMessages[emd_WarningACISUnsupportedParametricCurveType] =  RT_EMD_WARNING_ACIS_UNSUPPORTED_PARAMETRIC_CURVE_TYPE;
  errorMessages[emd_ErrorACISUnsupportedVersion] =                RT_EMD_ERROR_ACIS_UNSUPPORTED_VERSION;
  errorMessages[emd_WarningACISCannotImportEntityId] =            RT_EMD_WARNING_ACIS_CANNOT_IMPORT_ENTITY_ID;
  errorMessages[emd_WarningACISIncorrectIntAttribute] =           RT_EMD_WARNING_ACIS_INCORRECT_INT_ATTRIBUTE;
  
  // Сообщения конвертера IGES
  // IGES converter messages
  errorMessages[ emd_ErrorIGESIncorrectExternalReference ] =      RT_EMD_ERROR_IGES_INCORRECT_EXTERNAL_REFERENCE;

  // Сообщения конвертера XT
  // XT converter messages
  errorMessages[ emd_ErrorXTUnsupportedVersion ] =                RT_EMD_ERROR_XT_UNSUPPORTED_VERSION;

  // Сообщения конвертера JT
  // JT converter messages
  errorMessages[emd_ErrorJTDuplicatedTriangles] =                 RT_EMD_ERROR_JT_DUPLICATED_TRIANGLES;
  
}


//------------------------------------------------------------------------------
// Деструктор
// Destructor
// ---
LogReporter::~LogReporter() {
  errOut.close();
}


//------------------------------------------------------------------------------
// Инициализировать поток сообщений
// Initialise stream of the log
// ---
bool LogReporter::InitErrOut() {
  if ( !errOut.good() )
    return false;

  if ( errOut.rdbuf()->is_open() )
    return true;

  //OV_Lnx errOut.imbue( std::locale( "Russian" ) );
  //OV_Lnx std::wstring errName ( GetFileName().get_wcs() ), fName( errName );

  string_t errName ( fileName ), fName( errName );

  if ( errName.length() == 0 )
    return false;

  ptrdiff_t pointPos = errName.find_last_of( _T(".") );
  errName.erase( errName.begin()+pointPos, errName.end() );
  errName += _T(".log");

  errOut.open( c3d::C3DToPathstring(errName).c_str(), std::ios::out ); // Открыть файл на запись

  if ( !errOut.good() )
    return false;

  errOut << std::endl << std::endl;
  errOut << errorMessages[emd_Title].c_str() << fName.c_str() << _T(".");
  errOut << std::endl << std::endl;

  return errOut.good();
}


//------------------------------------------------------------------------------
// Сделать запись в журнал
// Log the message
// ---
void LogReporter::LogReport( ptrdiff_t id, eMsgType msgType, eMsgDetail msgText )
{
  if ( !!InitErrOut() ) {
    switch ( msgType ) {
    case emt_ErrorNoId:
      errOut << errorMessages[emd_HEADError].c_str();
      break;
    case emt_TextOnly:
      ;
      break;
    case emt_Info:
      errOut << _T("#") << id << _T(" ") << errorMessages[emd_HEADInfo].c_str();
      break;
    case emt_Warning:
      errOut << _T("#") << id << _T(" ") << errorMessages[emd_HEADWarinig].c_str();
      break;
    case emt_Error:
      errOut << _T("#") << id << _T(" ") << errorMessages[emd_HEADError].c_str();
      break;
    default:  // на случай внесения дополнительных изменений в enum;
              // In case of the enum extension
      errOut << _T("#") << id << _T(" ") << errorMessages[emd_HEADDefaultMsg].c_str();
        }
        errOut<< _T("") << errorMessages[msgText].c_str() << _T(".") << std::endl;
      }
}


//------------------------------------------------------------------------------
// Сделать запись в журнал
// Log all the messages
// ---
void LogReporter::LogReport( std::vector<ConvConvertorProperty3D::LogRecord> records ) {
  for( size_t i = 0, sz = records.size(); i < sz; i++ )
    LogReport( records[i].id, records[i].msgType, records[i].msgText );
}


////////////////////////////////////////////////////////////////////////////////
//
//                 Методы общения конвертеров и менеджера.
//
//                      Model management utilities
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Передача конвертированной модели менеджеру.
// Transfer the converted model to manager.
// ---
static
void AddContentToManager( MbItem * content, bool adjustView )
{
  if ( content != nullptr ) {
    //if ( /*replaceItems*/true )
    TestVariables::viewManager->DeleteModel();

    if ( adjustView ) {
      MbCube gab;
      content->AddYourGabaritTo( gab );
      // подстроить активное окно и стрелку прогиба для размеров объекта.
      // Adjust the scale of the model to display the entire model
      if ( !gab.IsEmpty() )
        TestVariables::viewManager->SetWindowScale( gab );
    }
    const MbColor * colorAttr= static_cast<const MbColor *>(content->GetSimpleAttribute(at_Color));
    TestVariables::viewManager->AddItem( *content, Style ( 1, ((colorAttr != nullptr) ? colorAttr->Color() : MB_C3DCOLOR) ) );
  }
}


//------------------------------------------------------------------------------
// Сформировать контент на основе модели
// Build the model for export
// ---
static MbItem* ModelToContent( MbModel& model ) {
  MbItem * content = nullptr;
  RPArray<MbItem> objs( 2, 1 );
  model.GetItems( objs );
  size_t objSz = objs.Count();
  if ( objSz > 0 ) {
    if ( objSz > 1 ) {
      std::vector< MbItem* > itemsOfContent;
      for( size_t  i = 0; i < objSz; i++ ) {
        MbItem* curIt = objs[i];
        if ( curIt == nullptr )
          continue;
        if ( curIt->IsA() == st_Instance ) {
          MbInstance& curInst = static_cast<MbInstance&>( *curIt );
          if ( curInst.GetItem() != nullptr ) {
            MbInstance& asInst = *( new MbInstance( *curInst.SetItem(), curInst.GetPlacement() ) );// static_cast<MbInstance&>( curIt->Duplicate() );
            curIt->AttributesCopy( asInst );
            itemsOfContent.push_back( &asInst );
          }
        } else
          //itemsOfContent.push_back( new MbInstance( *curIt, MbPlacement3D::global ) );
          itemsOfContent.push_back( curIt );
      }
      content = new MbAssembly( itemsOfContent );
    }
    else {
      content = objs[0];
    }
  }
  if ( content != nullptr )
    model.AttributesCopy( *content );

  return content;
}


//------------------------------------------------------------------------------
// Распознать булeво значение и обновить в соответствие с нотацией
// ---
bool UpdateBool( const c3d::string_t& key, bool& value )
{
  const c3d::string_t trueBoolean( CONFIG_CONVERTER_DEBUG_SETTING_TRUE );
  const c3d::string_t falseBoolean( CONFIG_CONVERTER_DEBUG_SETTING_FALSE );
  bool result(false);
  if ( trueBoolean == key )
  {
    value = true;
    result = true;
  }
  else if ( falseBoolean == key )
  {
    value = false;
    result = true;
  }
  return  result;
}

//------------------------------------------------------------------------------
// Получить пользовательские настройки конвертера
// Файл c3d_test.config должен лежать рядом с файлом лицензии.
// Возвращает true, если хотя бы одна опция была задана.
// ---
bool UpdateConverterSettings( ConvConvertorProperty3D & convProperties, bool& bPmiToItems )
{
  bool result (false);
  bPmiToItems = false;
  const c3d::string_t debugSettingPrefix (CONFIG_CONV_SETTINGS_DEBUG);
  const c3d::string_t enableCERRout( CONFIG_CONVERTER_DEBUG_SETTING_ENABLE_CERR_OUT );
  const c3d::string_t cerrOutUserTreeTraverse( CONFIG_CONVERTER_DEBUG_SETTING_USER_TREE );
  const c3d::string_t cerrOutIntermediateTreeTraverse( CONFIG_CONVERTER_DEBUG_SETTING_IM_TREE );
  const c3d::string_t cerrOutImportStatistic( CONFIG_CONVERTER_DEBUG_SETTING_OUT_STATISTICS );
  const c3d::string_t saveModelTwin( CONFIG_CONVERTER_DEBUG_SETTING_SAVE_TWIN );
  const c3d::string_t elementIdFineDebug( CONFIG_CONVERTER_DEBUG_SETTING_ID_FINE_DEBUG );
  const c3d::string_t attachHitsIdAttr( CONFIG_CONVERTER_DEBUG_SETTING_ATTHACH_THISID_ATTRIBUTE );
  const c3d::string_t pathFineDebug( CONFIG_CONVERTER_DEBUG_SETTING_OUT_FINE_DEBUG );

  const c3d::string_t cnvSettingPrefix( CONFIG_CONV_SETTINGS );
  const c3d::string_t stitchOn( CONFIG_CONVERTER_SETTING_STITCH_ON );
  const c3d::string_t stitchPrec( CONFIG_CONVERTER_SETTING_STITCH_PRECISION );
  const c3d::string_t joinSimilar( CONFIG_CONVERTER_SETTING_JOIN_SIMILAR_FACES );
  const c3d::string_t pmiToItems( CONFIG_CONVERTER_SETTING_PMI_TO_ITEM );

  for ( const std::pair<c3d::string_t, c3d::string_t>& curSettingPair :  ::TestVariables::userProp )
  {
    if ( debugSettingPrefix != curSettingPair.first.substr( 0, debugSettingPrefix.length() ) )
    {
      if ( cnvSettingPrefix != curSettingPair.first.substr( 0, cnvSettingPrefix.length() ) )
        continue;
      const string_t debugSettingKey( curSettingPair.first.substr( cnvSettingPrefix.length() ) );
      if ( debugSettingKey == stitchOn )
        result = UpdateBool( curSettingPair.second, convProperties.enableAutostitch ) || result;
      else if ( debugSettingKey == joinSimilar )
        result = UpdateBool( curSettingPair.second, convProperties.joinSimilarFaces ) || result;
      else if ( debugSettingKey == stitchPrec )
        result = sscanf( c3d::ToSTDstring( curSettingPair.second ).c_str(), "%lf", &convProperties.autostitchPrecision ) == 1 || result;
      else if ( debugSettingKey == pmiToItems)
        result = UpdateBool( curSettingPair.second, bPmiToItems ) || result;
      else
        continue;
    }
    const string_t debugSettingKey( curSettingPair.first.substr( debugSettingPrefix.length() ) );
    if ( debugSettingKey == enableCERRout )
      result = UpdateBool( curSettingPair.second, convProperties.debugSettings.enableCERRout ) || result;
    else if ( debugSettingKey == cerrOutUserTreeTraverse )
      result = UpdateBool( curSettingPair.second, convProperties.debugSettings.cerrOutUserTreeTraverse ) || result;
    else if ( debugSettingKey == cerrOutIntermediateTreeTraverse)
      result = UpdateBool( curSettingPair.second, convProperties.debugSettings.cerrOutIntermediateTreeTraverse ) || result;
    else if ( debugSettingKey == cerrOutImportStatistic )
      result = UpdateBool( curSettingPair.second, convProperties.debugSettings.cerrOutImportStatistic ) || result;
    else if ( debugSettingKey == saveModelTwin)
      result = UpdateBool( curSettingPair.second, convProperties.debugSettings.saveModelTwin ) || result;
    else if ( debugSettingKey == attachHitsIdAttr )
      result = UpdateBool( curSettingPair.second, convProperties.debugSettings.attachThisIdAttribute ) || result;
    else if ( debugSettingKey == elementIdFineDebug )
    {
      ptrdiff_t id( 0 );
      if ( 1 == sscanf( c3d::ToSTDstring( curSettingPair.second ).c_str(), "%td", &id ) )
      {
        convProperties.debugSettings.elementIdFineDebug = id;
        result = true;
      }
    }
    else if ( debugSettingKey == pathFineDebug  )
    {
      convProperties.debugSettings.pathFineDebug = curSettingPair.second;
      result = true;
    }    
  }
  return  result;
}


//------------------------------------------------------------------------------
// получить единицы измерения длины при экспорте
// Get the length units for export
// ---
//static MbeModelLengthUnits GetLengthUnits() {
//  MbeModelLengthUnits result(mlu_Mm);
//  int lengthUnits( 0 );
//  ::GetInt( IDS_UNIT_OF_MEASURE, lengthUnits );
//  switch( lengthUnits ) {
//    case 1:
//      result = mlu_Cm;
//      break;
//    case 2:
//      result = mlu_Dm;
//      break;
//    case 3:
//      result = mlu_Meter;
//      break;
//    case 4:
//      result = mlu_Inch;
//      break;
//    default:
//      result = mlu_Mm;
//      break;
//  }
//  return result;
//}


//------------------------------------------------------------------------------
// Получить параметры автосшивки
// Get the parameters for automatic stitch
// ---
bool GetAutostitchData( double & precision )
{
  int sType = 1;

  if ( !TestVariables::checkReadTime ) // не спрашиваем при замере времени
    ::GetInt( IDS_ENTER_STITCH_SURFACES, sType );

  bool enableAutostitch = sType != 0;
  if ( enableAutostitch ) {
    double autostitchVal( DELTA_MID );
    if ( !TestVariables::checkReadTime ) // не спрашиваем при замере времени
      ::GetDouble( IDS_ENTER_STITCH_PRECISION, autostitchVal );
    if ( autostitchVal > METRIC_REGION )
      precision = autostitchVal;
    else
      enableAutostitch = false;
  }
  return enableAutostitch;
}


//------------------------------------------------------------------------------
// Получить флаг слияния подобных граней
// Get the join similar faces flag
// ---
bool GetJoinSimilarFacesFlag()
{
  int sType = 1;
  if ( !TestVariables::checkReadTime ) // не спрашиваем при замере времени
    ::GetInt( IDS_ENTER_JOIN_SIMILAR_FACES, sType );

  return ( sType != 0 );
}


//------------------------------------------------------------------------------
// Получить флаг добавления удаленных граней в качестве оболочек
// Get the join similar faces flag
// ---
bool GetAddRemovedFacesAsShellsFlag()
{
  int sType = 0;
  if ( !TestVariables::checkReadTime ) // не спрашиваем при замере времени
    ::GetInt( IDS_ENTER_ADD_REMOVED_FACES_AS_SHELLS, sType );

  return ( sType != 0 );
}


////------------------------------------------------------------------------------
//// Поменять расширение имени файла
//// ---
//static
//void ChangeFileNameExt( string & name, TCHAR * ext )
//{
//  size_t len = name.length();
//
//  if ( len ) {
//    size_t pointPos = name.find_last_of( _T(".") );
//
//    if ( pointPos != NPOS )
//      name.remove( pointPos );
//
//    name += _T("."); 
//    name += string( ext );
//  }
//}


//------------------------------------------------------------------------------
// Установка имени модели
// Set the name of the model based on the name of the imported one
// ---
bool WrConvertor3D::SetModelName( const TCHAR * fileName )
{
  bool changed = false;

  if ( TestVariables::viewManager != nullptr ) {
    WorkWindow * wnd = TestVariables::viewManager->GetActiveWindow();
    if ( wnd != nullptr && wnd->IsModelNameEmpty() ) {
      wnd->SetModelName( fileName );
      changed = true;
    }
  }
  return changed;
}



////////////////////////////////////////////////////////////////////////////////
//
//                               SAT конвертер
//
//                               SAT Converter
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Запись SAT формата 
// Export model to SAT
// ---
bool WrConvertor3D::SATWrite( const TCHAR * fileName )
{
  bool ok = false;
  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, SAT_EXT ) && !::SameExtension( fileName, SAB_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, SAT_EXT_INFO, SAT_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;      
      convProperties.fileName.assign( c3d::C3DToPathstring(fileName) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      RegularModelDocument modelDoc( bPmiToItems ? SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) : SPtr<C3DPmiToItem>() );
      modelDoc.SetContent( content );
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetDouble( IDS_UNIT_OF_MEASURE, convProperties.lengthUnitsFactor );

      SetWaitCursor( true );

#ifdef SET_LOCALE_OBVIOUSLY
      const char* prevLocale = ::setlocale( LC_CTYPE, nullptr );
      ::setlocale( LC_CTYPE, SPECIFIC_LOCALE );  
#endif // SET_LOCALE_OBVIOUSLY

      ok = ( ::SATWrite(convProperties, modelDoc, progInd) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );
#ifdef SET_LOCALE_OBVIOUSLY
      ::setlocale( LC_CTYPE, prevLocale );  
#endif // SET_LOCALE_OBVIOUSLY

      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Чтение SAT формата 
// Import model from SAT
// ---
bool WrConvertor3D::SATRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  Math::SetAssertNotify( c3d::avn_CERR );

  // Получить конвертер
  // Get the converter interface
  IConvertor3D * convertor = ::GetConvertor3D();
  if ( convertor != nullptr )
  {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, SAT_EXT ) && !::SameExtension( fileName, SAB_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, SAT_EXT_INFO, SAT_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {
      SetWaitCursor( true );

      c3d::string_t strFileName( fileName );
      SPtr<IScaleRequestor> requestor ( new ScaleRequestorImp() );
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      convProperties.enableAutostitch = false;//::GetAutostitchData( convProperties.autostitchPrecision );
      convProperties.joinSimilarFaces = ::GetJoinSimilarFacesFlag();
      convProperties.addRemovedFacesAsShells = ::GetAddRemovedFacesAsShellsFlag();
      RegularModelDocument contentDoc( bPmiToItems ? SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) : SPtr<C3DPmiToItem>() );

      // Использование потока данных.
      // Use stream obviously
      
      std::fstream wkStream;
      wkStream.open( c3d::C3DToPathstring(strFileName).c_str(), (std::ios::in | std::ios::ate | ( static_cast<std::ios_base::openmode>(0))) );
      size_t dotPos = strFileName.find_last_of( _T(".") ), slashPos = strFileName.find_last_of( _T("\\") );
      if (slashPos == SYS_MAX_T )
        slashPos = strFileName.find_last_of( _T("/") );
      if (slashPos == SYS_MAX_T )
        slashPos = 0;
      else {
        slashPos++;
        dotPos -= slashPos;
      }
      if ( (slashPos != SYS_MAX_T) && (dotPos != SYS_MAX_T) )
        strFileName = strFileName.substr( slashPos, dotPos );

      ok = ( convertor->SATRead( convProperties, contentDoc, &wkStream, progInd, requestor) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

      delete convertor;

      SetWaitCursor( false );

      if ( ok ) {
        content = contentDoc.GetContent();
        SetModelName( TestVariables::nameBuffer );
      }
      ::AddRefItem( content );
    }
    else
      delete convertor;
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


////////////////////////////////////////////////////////////////////////////////
//
//                               IGES конвертер
//
//                               IGES Converter
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Запись IGES формата 
// Export model to IGES
// ---
bool WrConvertor3D::IGESWrite( const TCHAR * fileName )
{
  bool ok = false;
  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, IGS_EXT ) && !::SameExtension( fileName, IGES_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, IGS_EXT_INFO, IGS_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      ConvConvertorProperty3D convProperties;
      convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      int exportType = 0;
      ::GetInt( IDS_SAVE_TOPOLIGY, exportType );
      convProperties.exportIGESTopology = ( exportType != 0 );
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      RegularModelDocument modelDoc;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetDouble( IDS_UNIT_OF_MEASURE, convProperties.lengthUnitsFactor );

      modelDoc.SetContent( content );

      SetWaitCursor( true );

#ifdef SET_LOCALE_OBVIOUSLY
      const char* prevLocale = ::setlocale( LC_CTYPE, nullptr );
      ::setlocale( LC_CTYPE, SPECIFIC_LOCALE );  
#endif // SET_LOCALE_OBVIOUSLY

      ok = ( ::IGSWrite(convProperties, modelDoc, progInd) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );
#ifdef SET_LOCALE_OBVIOUSLY
      ::setlocale( LC_CTYPE, prevLocale );  
#endif // SET_LOCALE_OBVIOUSLY


      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Чтение IGES формата 
// Import moel from IGES
// ---
bool WrConvertor3D::IGESRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, IGS_EXT ) && !::SameExtension( fileName, IGES_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, IGS_EXT_INFO, IGS_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    SetWaitCursor( true );

    string_t fName( TestVariables::nameBuffer );
    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ));
    ConvConvertorProperty3D convProperties;
    bool bPmiToItems( false );
    UpdateConverterSettings( convProperties, bPmiToItems );
    convProperties.enableAutostitch = ::GetAutostitchData( convProperties.autostitchPrecision );
    convProperties.joinSimilarFaces = ::GetJoinSimilarFacesFlag();
    convProperties.addRemovedFacesAsShells = ::GetAddRemovedFacesAsShellsFlag();
    convProperties.fileName.assign( c3d::C3DToPathstring(fileName) );
    Math::SetAssertNotify( c3d::avn_CERR );
    RegularModelDocument contentDoc( bPmiToItems ? SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) : SPtr<C3DPmiToItem>() );

    ok = ( ::IGSRead(convProperties, contentDoc, progInd ) == cnv_Success );
    LogReporter(fileName).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content= contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


////////////////////////////////////////////////////////////////////////////////
//
//                             PARASOLID конвертер
//
//                             Parasolid Converter
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Запись X_T формата 
// Export model to X_T or X_B
// ---
bool WrConvertor3D::XTWrite( const TCHAR * fileName )
{
  bool ok = false;

  SetWaitCursor( true );

  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, X_T_EXT ) && !::SameExtension( fileName, X_B_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, X_T_EXT_INFO, X_T_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      SetWaitCursor( true );
      ConvConvertorProperty3D convProperties;
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      convProperties.fileName.assign( c3d::C3DToPathstring(fileName) );
      Math::SetAssertNotify( c3d::avn_CERR );
      int sType = 0; 
      GetInt( IDS_ENTER_WRITE_FILETYPE, sType ); 
      convProperties.fileASCII = sType == 0;
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      RegularModelDocument modelDoc;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetDouble( IDS_UNIT_OF_MEASURE, convProperties.lengthUnitsFactor );

      modelDoc.SetContent( content );

#ifdef SET_LOCALE_OBVIOUSLY
      const char* prevLocale = ::setlocale( LC_CTYPE, nullptr );
      ::setlocale( LC_CTYPE, SPECIFIC_LOCALE );  
#endif // SET_LOCALE_OBVIOUSLY

      ok = ( ::XTWrite(convProperties, modelDoc, progInd) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );
#ifdef SET_LOCALE_OBVIOUSLY
      ::setlocale( LC_CTYPE, prevLocale );  
#endif // SET_LOCALE_OBVIOUSLY


      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Чтение X_T формата 
// Import model from X_T or X_B
// ---
bool WrConvertor3D::XTRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, X_T_EXT ) && !::SameExtension( fileName, X_B_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, X_T_EXT_INFO, X_T_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    SetWaitCursor( true );

    Math::SetAssertNotify( c3d::avn_CERR );

    ConvConvertorProperty3D convProperties;
    bool bPmiToItems( false );
    UpdateConverterSettings( convProperties, bPmiToItems );
    convProperties.fileName.assign( c3d::C3DToPathstring(fileName) );
    convProperties.enableAutostitch = ::GetAutostitchData( convProperties.autostitchPrecision );
    convProperties.joinSimilarFaces = ::GetJoinSimilarFacesFlag();
    convProperties.addRemovedFacesAsShells = ::GetAddRemovedFacesAsShellsFlag();
    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
    RegularModelDocument contentDoc( bPmiToItems ? SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) : SPtr<C3DPmiToItem>() );

    ok = ( ::XTRead( convProperties, contentDoc, progInd) == cnv_Success );
    LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content = contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


////////////////////////////////////////////////////////////////////////////////
//
//                             STEP конвертер
//
//                             STEP converter
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Запись STEP формата 
// Export model to STEP
// ---
bool WrConvertor3D::STEPWrite( const TCHAR * fileName )
{
  bool ok = false;

  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, STP_EXT ) && !::SameExtension( fileName, STEP_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, STP_EXT_INFO, STP_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;
      convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      RegularModelDocument modelDoc;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetDouble( IDS_UNIT_OF_MEASURE, convProperties.lengthUnitsFactor );

      SetWaitCursor( true );
      modelDoc.SetContent( content );

#ifdef SET_LOCALE_OBVIOUSLY
      const char* prevLocale = ::setlocale( LC_CTYPE, nullptr );
      ::setlocale( LC_CTYPE, SPECIFIC_LOCALE );  
#endif // SET_LOCALE_OBVIOUSLY

      ok = ( ::STEPWrite(convProperties, modelDoc, progInd) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );
#ifdef SET_LOCALE_OBVIOUSLY
      ::setlocale( LC_CTYPE, prevLocale );  
#endif // SET_LOCALE_OBVIOUSLY


      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Чтение STEP формата 
// Import model from STEP
// ---
bool WrConvertor3D::STEPRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  // Получить конвертор и заставить его работать;
  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, STP_EXT ) && !::SameExtension( fileName, STEP_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, STP_EXT_INFO, STP_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    SetWaitCursor( true );

    ConvConvertorProperty3D convProperties;
    convProperties.fileName.assign( c3d::C3DToPathstring(fileName) );
    bool bPmiToItems( false ); //в данном случае было решено отображать pmi всегда
    UpdateConverterSettings( convProperties, bPmiToItems );
    convProperties.enableAutostitch = ::GetAutostitchData( convProperties.autostitchPrecision );
    convProperties.joinSimilarFaces = ::GetJoinSimilarFacesFlag();
    convProperties.addRemovedFacesAsShells = ::GetAddRemovedFacesAsShellsFlag();

    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
    C3dModelDocument contentDoc( SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) );
    Math::SetAssertNotify( c3d::avn_CERR );
    ok = ( ::STEPRead( convProperties, contentDoc, progInd ) == cnv_Success );
    LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content = contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


////////////////////////////////////////////////////////////////////////////////
//
//                             JT конвертер
//
//                             JT converter
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Чтение JT формата 
// Import model from JT
// ---
bool WrConvertor3D::JTRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  // Получить конвертор и заставить его работать;
  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, STP_EXT ) && !::SameExtension( fileName, JT_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, JT_EXT_INFO, JT_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    SetWaitCursor( true );
    Math::SetAssertNotify( c3d::avn_CERR );
    ConvConvertorProperty3D convProperties;
    convProperties.fileName.assign( c3d::C3DToPathstring(fileName) );
    bool bPmiToItems( false );//в данном случае было решено отображать pmi всегда
    UpdateConverterSettings( convProperties, bPmiToItems );
    int importBrep(0);
    GetInt( IDS_ENTER_IMPORT_BREP, importBrep );
    convProperties.ioPermissions[ iop_rBRep ] = (importBrep != 0);
    if ( convProperties.ioPermissions[ iop_rBRep ] ) {
      convProperties.enableAutostitch = ::GetAutostitchData( convProperties.autostitchPrecision );
      convProperties.joinSimilarFaces = ::GetJoinSimilarFacesFlag();
      convProperties.addRemovedFacesAsShells = ::GetAddRemovedFacesAsShellsFlag();
    }    

    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
    C3dModelDocument contentDoc( SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) );

    ok = ::JTRead( convProperties, contentDoc, progInd ) == cnv_Success;
    
    LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content = contentDoc.GetContent();

      if ( content != nullptr ) {
        content->AddRef();
        SetModelName( TestVariables::nameBuffer );
      }
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


//------------------------------------------------------------------------------
// Запись JT формата 
// Export model to JT
// ---
bool WrConvertor3D::JTWrite( const TCHAR * fileName )
{
  bool ok = false;

  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, JT_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, JT_EXT_INFO, JT_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;
      convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      RegularModelDocument modelDoc;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetDouble( IDS_UNIT_OF_MEASURE, convProperties.lengthUnitsFactor );
      int mode = TestVariables::operateType;
      ::InitStepData( convProperties.tesseleationStepData, false, mode );

      SetWaitCursor( true );
      modelDoc.SetContent( content );

#ifdef SET_LOCALE_OBVIOUSLY
      const char* prevLocale = ::setlocale( LC_CTYPE, nullptr );
      ::setlocale( LC_CTYPE, SPECIFIC_LOCALE );  
#endif // SET_LOCALE_OBVIOUSLY
      IConvertor3D* conv3d = GetConvertor3D();
      if ( conv3d != nullptr ) {
        ok = ( conv3d->JTWrite(convProperties, modelDoc, progInd) == cnv_Success );
        LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );
        delete conv3d;
      }
#ifdef SET_LOCALE_OBVIOUSLY
      ::setlocale( LC_CTYPE, prevLocale );  
#endif // SET_LOCALE_OBVIOUSLY


      SetWaitCursor( false );
    }
  }

  return ok;
}


////////////////////////////////////////////////////////////////////////////////
//
//                             STL конвертер
//
//                             STL converter
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Запись STL формата 
// Export model to STL
// ---
bool WrConvertor3D::STLWrite( const TCHAR * fileName )
{
  bool ok = false;

  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, STL_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, STL_EXT_INFO, STL_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;
      convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      RegularModelDocument modelDoc;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetDouble( IDS_UNIT_OF_MEASURE, convProperties.lengthUnitsFactor );
      int sType = 0; 
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetInt( IDS_ENTER_WRITE_FILETYPE, sType ); 
      convProperties.fileASCII = sType == 0;
      int mode = TestVariables::operateType;
      ::InitStepData( convProperties.tesseleationStepData, false, mode );
      bool dualSeams = convProperties.dualSeams;
      int ttt = dualSeams ? 1 : 0;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetInt( IDS_ENTER_DUAL_SEAMS, ttt );
      dualSeams = ( ttt != 0 );
      convProperties.dualSeams = dualSeams;

      SetWaitCursor( true );

      modelDoc.SetContent( content );

      ok = ( ::STLWrite(convProperties, modelDoc, progInd ) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Чтение STL формата 
// Import model from STL
// ---
bool WrConvertor3D::STLRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, STL_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, STL_EXT_INFO, STL_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
    ConvConvertorProperty3D convProperties;
    convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
    bool bPmiToItems( false );
    UpdateConverterSettings( convProperties, bPmiToItems );
    convProperties.SetIoPermission( iop_rSolid, false ); // false - не удалять избыточные точки.
    RegularModelDocument contentDoc( bPmiToItems ? SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) : SPtr<C3DPmiToItem>() );

    SetWaitCursor( true );

    ok = ( ::STLRead(convProperties, contentDoc, progInd) == cnv_Success );
    if ( ok ) {
      C3D_ASSERT ( !contentDoc.IsEmpty() );
    }
    LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content = contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


////////////////////////////////////////////////////////////////////////////////
//
//                             VRML конвертер
//
//                             VRML converter
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Запись VRML формата
// Export model to VRML
// ---
bool WrConvertor3D::VRMLWrite( const TCHAR * fileName )
{
  bool ok = false;

  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, VRML_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, VRML_EXT_INFO, VRML_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;
      convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      RegularModelDocument modelDoc;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetDouble( IDS_UNIT_OF_MEASURE, convProperties.lengthUnitsFactor );
      int mode = TestVariables::operateType;
      ::InitStepData( convProperties.tesseleationStepData, false, mode );
      bool dualSeams = convProperties.dualSeams;
      int ttt = dualSeams ? 1 : 0;
      if ( !TestVariables::checkWriteTime ) // не спрашиваем при замере времени
        ::GetInt( IDS_ENTER_DUAL_SEAMS, ttt );
      dualSeams = ( ttt != 0 );
      convProperties.dualSeams = dualSeams;

      SetWaitCursor( true );

      modelDoc.SetContent( content );

      ok = ( ::VRMLWrite(convProperties, modelDoc, progInd ) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Чтение VRML формата 
// Import model from VRML
// ---
bool WrConvertor3D::VRMLRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, VRML_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, VRML_EXT_INFO, VRML_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    SetWaitCursor( true );

    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
    ConvConvertorProperty3D convProperties;
    convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
    bool bPmiToItems( false );
    UpdateConverterSettings( convProperties, bPmiToItems );
    RegularModelDocument contentDoc;
    Math::SetAssertNotify( c3d::avn_CERR );

    ok = ( ::VRMLRead(convProperties, contentDoc, progInd) == cnv_Success );
    LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content= contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}

//------------------------------------------------------------------------------
// Чтение OBJ формата 
// Import model from OBJ
// ---
bool WrConvertor3D::OBJRead( const TCHAR* fileName )
{
  bool ok = false;
  MbItem* content = nullptr;

  if ( fileName != nullptr )
  {
    if ( !::SameExtension( fileName, OBJ_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr )
  {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, OBJ_EXT_INFO, OBJ_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    SetWaitCursor( true );

    DPtr<IProgressIndicator> progInd( &::CreateProgressIndicator( new StrSpyConverters ) );
    ConvConvertorProperty3D convProperties;
    convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
    bool bPmiToItems( false );
    UpdateConverterSettings( convProperties, bPmiToItems );
    RegularModelDocument contentDoc;
    Math::SetAssertNotify( c3d::avn_CERR );

    ok = ( ::OBJRead( convProperties, contentDoc, progInd ) == cnv_Success );
    LogReporter( c3d::string_t( fileName ) ).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok )
    {
      content = contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; 
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


//------------------------------------------------------------------------------
// Чтение GRDECL формата 
// Import model from GRDECL
// ---
bool WrConvertor3D::GRDECLRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, GRDECL_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, GRDECL_EXT_INFO, GRDECL_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    SetWaitCursor( true );

    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
    ConvConvertorProperty3D convProperties;
    convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
    bool bPmiToItems( false );
    UpdateConverterSettings( convProperties, bPmiToItems );
    RegularModelDocument contentDoc;

    ok = ( ::GRDECLRead(convProperties, contentDoc, progInd) == cnv_Success );
    LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content= contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
   bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


//------------------------------------------------------------------------------
// Запись Grdecl формата 
// Export model to Grdecl
// ---
bool WrConvertor3D::GRDECLWrite( const TCHAR * fileName )
{
  bool ok = false;

  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, GRDECL_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, GRDECL_EXT_INFO, GRDECL_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;
      convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      RegularModelDocument modelDoc;     

      SetWaitCursor( true );

      modelDoc.SetContent( content );

      ok = ( ::GRDECLWrite(convProperties, modelDoc, progInd ) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Чтение облака точек из файла ASCII ( .asc, .txt, .xyz ) 
// Import model from ASCII Point Cloud ( .asc, .txt, .xyz ) 
// ---
bool WrConvertor3D::ASCIIPointCloudRead( const TCHAR * fileName )
{
  bool ok = false;
  MbItem * content = nullptr;

  if ( fileName != nullptr ) {
    if ( !::SameExtension( fileName, TXT_EXT ) && !::SameExtension( fileName, ASC_EXT ) &&
         !::SameExtension( fileName, XYZ_EXT ) )
      fileName = nullptr;
  }
  if ( fileName == nullptr ) {
    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, ASCIIPC_EXT_INFO, TXT_EXT ) )
      fileName = TestVariables::nameBuffer;
  }
  if ( fileName != nullptr )
  {
    DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
    ConvConvertorProperty3D convProperties;
    convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
    bool bPmiToItems( false );
    UpdateConverterSettings( convProperties, bPmiToItems );
    RegularModelDocument contentDoc;

    SetWaitCursor( true );

    ok = ( ::ASCIIPointCloudRead(convProperties, contentDoc, progInd) == cnv_Success );
    LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

    SetWaitCursor( false );

    if ( ok ) {
      content = contentDoc.GetContent();
      SetModelName( TestVariables::nameBuffer );
    }
    ::AddRefItem( content );
  }
  bool calcScale = true; // SD#739011 = false;
  ::AddContentToManager( content, calcScale );
  ::ReleaseItem( content );

  return ok;
}


//------------------------------------------------------------------------------
// Запись облака точек в файл ASCII ( .asc, .txt, .xyz ) 
// Export model to ASCII Point Cloud ( .asc, .txt, .xyz ) 
// ---
bool WrConvertor3D::ASCIIPointCloudWrite( const TCHAR * fileName )
{
  bool ok = false;

  SPtr<MbItem> content( ::ModelToContent( TestVariables::viewManager->GetActiveWindow()->SetGeomModel() ) );

  if ( content != nullptr ) {
    if ( fileName != nullptr ) {
      if ( !::SameExtension( fileName, TXT_EXT ) && !::SameExtension( fileName, ASC_EXT ) &&
           !::SameExtension( fileName, XYZ_EXT ) )
        fileName = nullptr;
    }
    if ( fileName == nullptr ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, WRITE_FILE_INFO, ASCIIPC_EXT_INFO, TXT_EXT ) )
        fileName = TestVariables::nameBuffer;
    }
    if ( fileName != nullptr )
    {    
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      ConvConvertorProperty3D convProperties;
      convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
      bool bPmiToItems( false );
      UpdateConverterSettings( convProperties, bPmiToItems );
      RegularModelDocument modelDoc;     

      SetWaitCursor( true );

      modelDoc.SetContent( content );

      ok = ( ::ASCIIPointCloudWrite(convProperties, modelDoc, progInd ) == cnv_Success );
      LogReporter(c3d::string_t( fileName )).LogReport( convProperties.logRecords );

      SetWaitCursor( false );
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
// Функтор получения пользовательской настройки плагина
// пример настройки: converter.plugin.settings.C3D_CAPVIDIA_TRACE_OUT=C3D_CAPVIDIA_BOOLEAN_TRUE
//---
struct PluginPropGetter
{
  optionNameValuePairs_t & pluginSpecificSettings;

public:
  PluginPropGetter( optionNameValuePairs_t & _pluginSpecificSettings ) : pluginSpecificSettings( _pluginSpecificSettings ) {}

public:
  void operator() ( const std::pair<c3d::string_t, c3d::string_t> & prop )
  {
    if ( prop.first.find( CONFIG_CONV_PLUGIN_SETTINGS_PREFIX ) == 0 )
    {
      c3d::string_t keyPlagin = prop.first.substr( string_t(CONFIG_CONV_PLUGIN_SETTINGS_PREFIX).length() );
      pluginSpecificSettings[keyPlagin] = prop.second;
    }
  }
};


//------------------------------------------------------------------------------
// Получить пользовательские настройки плагина
// Файл c3d_test.config должен лежать рядом с файлом лицензии.
// ---
static bool GetPluginSettings( c3d::path_string & fullPathToPlugin, optionNameValuePairs_t & pluginSpecificSettings )
{
  std::map<c3d::string_t, c3d::string_t>::iterator userPropIter = ::TestVariables::userProp.find( CONFIG_CONV_PLUGIN_FULL_PATH );
  if ( userPropIter != ::TestVariables::userProp.end() )
  {
    fullPathToPlugin = c3d::C3DToPathstring( userPropIter->second );

    //специфичные настройки плагина
    std::for_each( ::TestVariables::userProp.begin(), ::TestVariables::userProp.end(), PluginPropGetter( pluginSpecificSettings ) );
    return true;
  }
  return false;
}


//------------------------------------------------------------------------------
// Импортировать с использованием плагина
// Import using plugin
// ---
bool WrConvertor3D::PluginRead( const TCHAR* fileName ) {
  bool result(false);
  if ( nullptr == fileName )
    return result;
  IConvertor3D * conv3d = ::GetConvertor3D();
  optionNameValuePairs_t pluginSpecificSettings;
  c3d::path_string fullPathToPlugin;
  if ( nullptr != conv3d && GetPluginSettings( fullPathToPlugin, pluginSpecificSettings ) ) {
 
    if ( cnv_Success == conv3d->LoadForeignReader( fullPathToPlugin, pluginSpecificSettings ) ) {
      ConvConvertorProperty3D ccp3d;
      ccp3d.fileName = C3DToPathstring( fileName );
      bool bPmiToItems( false );
      UpdateConverterSettings( ccp3d, bPmiToItems );
      C3dModelDocument mDoc( bPmiToItems ? SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) : SPtr<C3DPmiToItem>() );
      DPtr<IProgressIndicator> progInd ( &::CreateProgressIndicator( new StrSpyConverters ) );
      const MbeConvResType readResult = conv3d->ImportForeign( C3DToPathstring( fileName ), mDoc, &ccp3d, progInd );
      LogReporter(c3d::string_t( fileName )).LogReport( ccp3d.logRecords );
      result = (cnv_Success == readResult);
      if ( result ) {
        MbItem* content = mDoc.GetContent();
        if ( nullptr != content ) {
          ::AddRefItem( content );
          AddContentToManager( content, true );
          ::ReleaseItem( content );
        } else
          result = false;
      }
    }
    conv3d->ReleaseForeignReader();
    ::ReleaseConvertor3D( conv3d );
  }
  return result;
}
