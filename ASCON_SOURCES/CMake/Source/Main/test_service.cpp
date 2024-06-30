#include <test_service.h>
#include <test_manager.h>
#include <test.h>

#ifdef C3D_WINDOWS // _MSC_VER
  #include <windows.h>
#endif // _WIN32

#include <iostream>
#include <mb_operation_result.h>
#include <gcm_res_code.h>
#include <reference_item.h>
#include <item_registrator.h>
#include <cur_contour3d.h>
#include <surface.h>
#include <surf_plane.h>
#include <space_item.h>
#include <space_instance.h>
#include <solid.h>
#include <wire_frame.h>
#include <model_entity.h>
#include <op_swept_parameter.h>
#include <action_curve3d.h>
#include <conv_model_exchange.h>
#include <conv_model_document.h>
#include <conv_exchange_settings.h>
#include <mesh_primitive.h>
#include <math_namespace.h>
#include <test_converter.h>
#include <templ_dptr.h>
#include <locale.h>
#include <last.h>

using namespace c3d;


static bool debugMessages = false; // Выдавать отладочные сообщения

// В диалоге открыть/сохранить файл будут одновременно отображаться файлы всех поддерживаемых типов.
// В противном случае тип файла нужно выбирать из выпадающего списка.
// In the file open/save dialog files of all supported extensions will be shown.
// Otherwise the file type should be chosen in a combo box.
#ifndef __USE_QT__
static bool allSupportedFormats = true;
#else
static bool allSupportedFormats = false;
#endif


//----------------------------------------------------------------------------------------
// Послать строку в виде сообщения
// ---
void ShowString( const TCHAR * str ) 
{
  size_t strLen = _tcslen(str);
  TestVariables::waitFlag = (strLen > 0) ? 1 : -1;
  { // Очистка поля вывода
    size_t clearLen = 2 * strLen;
    clearLen = std_max( (size_t)256, clearLen );
    string_t clearStr;
    for ( size_t k = 0; k < clearLen; k++ )
      clearStr.append( _T(" ") );

    SetStatusBarText( clearStr.c_str() );
  }

  SetStatusBarText( str );
}


//----------------------------------------------------------------------------------------
// Ввод точки
// ---
static void GetIntPoint( MbCartPoint3D & pnt )
{
  if ( ::fabs( TestVariables::delta_point ) > EXTENT_EQUAL ) {
    double g;
    int i;

    g = pnt.x / TestVariables::delta_point;
    if (pnt.x>=0)   i = (int)( g + 0.5 );
    else            i = (int)( g - 0.5 );
    pnt.x = i*TestVariables::delta_point;

    g = pnt.y / TestVariables::delta_point;
    if (pnt.y>=0)   i = (int)( g + 0.5 );
    else            i = (int)( g - 0.5 );
    pnt.y = i*TestVariables::delta_point;

    g = pnt.z / TestVariables::delta_point;
    if (pnt.z>=0)   i = (int)( g + 0.5 );
    else            i = (int)( g - 0.5 );
    pnt.z = i*TestVariables::delta_point;
  }
}


//----------------------------------------------------------------------------------------
// Ввод точки
// ---
bool GetPoint( const TCHAR * strResID, MbCartPoint3D & pnt )
{
  SyncActiveWindowIndex();

  TestVariables::getPointResult = PNT_RES_NONE;
  ShowString( strResID ); //ShowResString( strResID );

  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    wind->ExecuteGetPointProc(); // Выполнить процесс обработки сообщений ввода точки

    ShowString( _T("") );

    switch ( TestVariables::getPointResult )
    {
      case PNT_RES_USUAL : {
        pnt = TestVariables::cursorPoint;
        return true;
      }
      case PNT_RES_SHIFT : { // Привязка к ближайшей точке в проекции
        double d = 4*SENSE_RADIUS;//wind->ScaleDistance( SENSE_RADIUS );
        if ( !wind->FindNearestSpaceProject( st_SpaceItem, TestVariables::cursorAxis, std_min( TestVariables::delta_point, d ), TestVariables::cursorPoint, pnt ) ) {
          pnt = TestVariables::cursorPoint;
          GetIntPoint( pnt );
        }
        return true;
      }
      case PNT_RES_CTRL : { // Привязка к ближайшей точке объекта   //-V112
        double d = 4*SENSE_RADIUS;//wind->ScaleDistance( SENSE_RADIUS );
        if ( !wind->FindNearestSpacePoint( st_SpaceItem, TestVariables::cursorAxis, std_min(TestVariables::delta_point,d), TestVariables::cursorPoint, pnt ) ) {
          pnt = TestVariables::cursorPoint;
          GetIntPoint( pnt );
        }
        return true;
      }
      default: {
        return false;
      }
    }
  }
  return false;
} // GetPoint


//----------------------------------------------------------------------------------------
// Ввод точки
// ---
bool GetPoint( const TCHAR * nStr, MbCartPoint & pnt, MbPlacement3D * dstPlace )
{
  SyncActiveWindowIndex();

  TestVariables::getPointResult = PNT_RES_NONE;
  ShowString( nStr );  

  if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
  {
    double d = SENSE_RADIUS;//wind->ScaleDistance( SENSE_RADIUS );
    wind->ExecuteGetPointProc(); // Выполнить процесс обработки сообщений ввода точки

    ShowString( _T("") );

    switch ( TestVariables::getPointResult )
    {
      case PNT_RES_USUAL:
      {
        // return TestVariables::viewManager->GetCplanePoint( TestVariables::cursorPoint, pnt );
        // ниже основное содержимое GetCplanePoint

        bool b = false;

        MbPlacement3D pl( wind->GetOwnPlace() );

        if ( dstPlace != nullptr )
          b = dstPlace->DirectPointProjection( TestVariables::cursorPoint, wind->GetAxisZ(), pnt.x, pnt.y );
        else
          b = pl.DirectPointProjection( TestVariables::cursorPoint, wind->GetAxisZ(), pnt.x, pnt.y );

        return b;
      }

      case PNT_RES_SHIFT: 
      { // Привязка к ближайшей точке в проекции        
        bool b = wind->FindNearestPlaneProject( pt_PlaneItem, TestVariables::cursorAxis, std_min(TestVariables::delta_point,d), TestVariables::cursorPoint, pnt );
        if ( !b ) {
          MbCartPoint3D p3d;
          b = wind->FindNearestSpaceProject( st_SpaceItem, TestVariables::cursorAxis, std_min(TestVariables::delta_point,d), TestVariables::cursorPoint, p3d );
          if ( b ) {
            b = false;

            MbPlacement3D pl( wind->GetOwnPlace() );

            if ( dstPlace != nullptr )
              b = dstPlace->DirectPointProjection( TestVariables::cursorPoint, wind->GetAxisZ(), pnt.x, pnt.y );
            else
              b = pl.DirectPointProjection( TestVariables::cursorPoint, wind->GetAxisZ(), pnt.x, pnt.y );

            return b;
          }
        }
        return b;
      }

      case PNT_RES_CTRL: // Привязка к ближайшей точке объекта   //-V112
      {
        bool b = wind->FindNearestPlanePoint( pt_PlaneItem, TestVariables::cursorAxis, std_min(TestVariables::delta_point,d), TestVariables::cursorPoint, pnt );
        if ( !b ) {
          MbCartPoint3D p3d;
          b = wind->FindNearestSpacePoint( st_SpaceItem, TestVariables::cursorAxis, std_min(TestVariables::delta_point,d), TestVariables::cursorPoint, p3d );
          if ( b ) {
            b = false;

            MbPlacement3D pl( wind->GetOwnPlace() );

            if ( dstPlace != nullptr )
              b = dstPlace->DirectPointProjection( TestVariables::cursorPoint, wind->GetAxisZ(), pnt.x, pnt.y );
            else
              b = pl.DirectPointProjection( TestVariables::cursorPoint, wind->GetAxisZ(), pnt.x, pnt.y );

            return b;
          }
        }
        return b;
      }

      default:
        return false;
    } // switch
  }
  return false;
} // GetPoint


//----------------------------------------------------------------------------------------
// Дать объект
// ---
void CommonGetObj( const TCHAR * nStr )
{
  SyncActiveWindowIndex();

  TestVariables::getPointResult = PNT_RES_NONE;
  ShowString( nStr );

  if ( WorkWindow * activeWindow = TestVariables::viewManager->GetActiveWindow() )
    activeWindow->ExecuteGetPointProc();  // Выполнить процесс обработки сообщений ввода точки

  ShowString( _T("") );
}


//----------------------------------------------------------------------------------------
// Указание элемента модели
// ---
bool GetObj( const TCHAR * nStr, MbItem ** find, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 ) {
    MbRefItem * geom = nullptr;
    WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
    if ( wind != nullptr ) {
      if ( wind->FindGeometricItem( st_SpaceItem, tt_Undefined, pt_PlaneItem, TestVariables::cursorAxis, SENSE_RADIUS, find, &geom, pathFrom ) )
        return true;
    }
  }

  return false;
} // GetObj


//----------------------------------------------------------------------------------------
// Указание объекта модели или его элемента(части)
// ---
bool GetObj( const TCHAR * nStr, MbItem ** find, MbRefItem ** item, PathMatrix & pathFrom )
{
  bool result = false;

  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 || TestVariables::getPointResult == 3 ) {
    WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
    if ( wind != nullptr ) {
      if ( wind->FindGeometricItem( st_SpaceItem, tt_TopItem, pt_PlaneItem, TestVariables::cursorAxis, SENSE_RADIUS, find, item, pathFrom ) ) {
        if ( TestVariables::getPointResult == 1 )
          *item = nullptr;
        result = true;
      }
    }
  }

  return result;
}


//----------------------------------------------------------------------------------------
// Указание ближайшей к курсору кривой модели
// ---
bool GetAnyCurve( const TCHAR * nStr, MbItem ** find, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 || TestVariables::getPointResult == 3 )
  {
    MbRefItem * geom = nullptr;
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      if ( wind->FindGeometricItem( st_Curve3D, tt_Undefined, pt_Curve, TestVariables::cursorAxis, SENSE_RADIUS, find, &geom, pathFrom ) )
        return true;
    }
  }
  return false;
} // GetAnyCurve


//----------------------------------------------------------------------------------------
// Указание сборки или вставки.
// ---
bool GetRigidObj( const TCHAR * nStr, MbItem *& find, MbRefItem *& geom, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 ) {
    geom = nullptr;
    WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
    if ( wind != nullptr ) {
      // Ищем сборочную единицу. // Find assembly.
      if ( wind->FindGeometricItem(st_Assembly, tt_Undefined, pt_Undefined, TestVariables::cursorAxis, SENSE_RADIUS, &find, &geom, pathFrom ) ) {
        if ( find != nullptr && find->IsA() == st_Assembly )
          return true;
      }
      // Ищем вставку. // Find instance.
      if ( wind->FindGeometricItem(st_Instance, tt_Undefined, pt_Undefined, TestVariables::cursorAxis, SENSE_RADIUS, &find, &geom, pathFrom ) ) {
        if ( find != nullptr && find->IsA() == st_Instance )
          return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------
// Указание произвольного 3D-объекта
// ---
const MbRefItem * GetSObj( const TCHAR * nStr, const MbItem ** find, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == PNT_RES_USUAL
    || TestVariables::getPointResult == PNT_RES_SHIFT )
  {    
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      MbItem * item = nullptr;
      MbRefItem * refObj = nullptr;
      if ( wind->FindGeometricItem( st_SpaceItem, tt_TopItem, pt_Undefined, TestVariables::cursorAxis
                                  , SENSE_RADIUS, &item, &refObj, pathFrom ) )
      {
        *find = item;
        return refObj;
      }
    }
  }

  pathFrom.first.clear();
  pathFrom.second = MbMatrix3D::identity;
  *find = nullptr;
  return nullptr;
}

//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу
// ---
bool GetGObj( const TCHAR * nStr, MbeSpaceType sType, MbItem ** find, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == PNT_RES_USUAL )
  {
    MbRefItem * geom = nullptr;
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      if ( wind->FindGeometricItem( sType, tt_Undefined, pt_Undefined, TestVariables::cursorAxis, SENSE_RADIUS, find, &geom, pathFrom ) )
        return true;
    }
  }

  return false;
} // GetGObj

//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу
//---
SPtr<MbItem> GetGObj( const TCHAR* nStr, MbeSpaceType sType, PathMatrix & pathFrom )
{
  MbItem * find = nullptr;
  if ( GetGObj(nStr, sType, &find, pathFrom) )
  {
    return SPtr<MbItem>( find );
  }
  return SPtr<MbItem>();
}

//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу и типу полигона
// ---
bool GetGObj( const TCHAR * nStr, MbeSpaceType sType,
              MbItem ** find, MbSpaceItem ** geom, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 || TestVariables::getPointResult == 3 )
  {
    MbRefItem * item = nullptr;
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      if ( wind->FindGeometricItem( sType, tt_Undefined, pt_Undefined, TestVariables::cursorAxis, SENSE_RADIUS, find, &item, pathFrom ) )
      {
        if ( (item != nullptr) && (item->RefType() == rt_SpaceItem) )
        {
          *geom = static_cast<MbSpaceItem*>( item );
          return true;
        }
      }
    }
  }

  return false;
} // GetGObj


//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу и типу полигона
// ---
bool GetTObj( const TCHAR * nStr, MbeSpaceType sType, MbeTopologyType tType,
              MbItem ** find, MbTopItem ** item, MbMatrix3D & matr )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 || TestVariables::getPointResult == 3 )
  {    
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      if ( wind->FindNearestItem( sType, tType, TestVariables::cursorAxis, SENSE_RADIUS, find, item, matr ) )
        return true;
    }
  }

  return false;
} // GetGObj


//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу и типу полигона
// ---
bool GetTObj( const TCHAR * nStr, MbeSpaceType sType, MbeTopologyType tType,
              MbItem ** find, MbTopItem ** item, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 || TestVariables::getPointResult == 3 )
  {
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      if ( wind->FindNearestItem( sType, tType, TestVariables::cursorAxis, SENSE_RADIUS, find, item, pathFrom ) )
        return true;
    }
  }

  return false;
} // GetGObj


//----------------------------------------------------------------------------------------
// Указание кривой. Работает как с геометрическими объектами, так и с топологическими (ребра).
// На текущий момент значение параметра part определяется тем, нажал ли пользователь клавишу shift
// при выборе объекта.
// ---
bool GetCurveObject( const TCHAR * nStr, bool & part, const MbeSpaceType sGeomType, const MbeSpaceType sTopologyType, const MbeTopologyType tType,
                     MbItem ** find, MbRefItem ** item, PathMatrix & pathFrom, MbMatrix3D & matr )
{
  CommonGetObj( nStr );

  if ( (TestVariables::getPointResult == PNT_RES_USUAL) || (TestVariables::getPointResult == PNT_RES_SHIFT) ) {
    part = ( TestVariables::getPointResult == PNT_RES_SHIFT );
    WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
    if ( wind != nullptr ) {
      if ( wind->FindGeometricItem(sGeomType, tt_Undefined, pt_Undefined, TestVariables::cursorAxis, SENSE_RADIUS, find, item, pathFrom) )
        return true;

      if ( wind->FindNearestItem(sTopologyType, tType, TestVariables::cursorAxis, SENSE_RADIUS, find, (MbTopItem**)item, matr) )
        return true;
    }
  }

  return false;
}


//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу
// ---
bool GetMObj( const TCHAR * nStr, MbePlaneType pType, MbItem ** find, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 )
  {
    MbRefItem * item = nullptr;
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      if ( wind->FindGeometricItem( st_Undefined, tt_Undefined, pType, TestVariables::cursorAxis, SENSE_RADIUS, find, &item, pathFrom ) )
        return true;
    }
  }

  return false;
} // GetGObj


//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу и типу полигона
// ---
bool GetMObj( const TCHAR * nStr, MbePlaneType pType,
              MbItem ** find, MbPlaneItem ** geom, PathMatrix & pathFrom )
{
  CommonGetObj( nStr );

  if ( TestVariables::getPointResult == 1 || TestVariables::getPointResult == 3 )
  {
    MbRefItem * item = nullptr;
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      if ( wind->FindGeometricItem( st_Undefined, tt_Undefined, pType, TestVariables::cursorAxis, SENSE_RADIUS, find, &item, pathFrom ) )
      {
        if ( (item != nullptr) && (item->RefType() == rt_PlaneItem) ) {
          *geom = (MbPlaneItem *)item;
          return true;
        }
      }
    }
  }
  return false;
} // GetGObj

//----------------------------------------------------------------------------------------
// Выбрать на экране экземпляр геометрии с учетом повторно-используемых тел.
// Select a geom object, preferably an instance of the re-inserted solid of the model.
//---
MbItem * GetInstance( const TCHAR * nStr, PathMatrix & pathFrom )
{
  MbItem * obj = 0;
  if ( GetObj(nStr, &obj, pathFrom) )
  {
    if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
    {
      const MbModel & model = wind->GetGeomModel();
      MbPath reducedPath( pathFrom.first );
      reducedPath.pop_back();
      MbMatrix3D m;
      if ( const MbItem * item = model.GetItemByPath( reducedPath, m ) )
      {
        if ( item->IsA() == st_Instance )
        {
          pathFrom.first = reducedPath;
          pathFrom.second = m;
          return const_cast<MbItem*>( item );
        }
      }
    }
  }

  return obj;
}

//----------------------------------------------------------------------------------------
// Нахождение ближайшего элемента к заданной точке
// ---
bool FindNearestObj( MbCartPoint3D & from_p, MbItem ** find, PathMatrix & pathFrom )
{
  MbRefItem * item = nullptr;
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    MbFloatAxis3D cAxis( from_p, wind->GetAxisZ() );
    if ( wind->FindGeometricItem( st_SpaceItem, tt_Undefined, pt_PlaneItem, cAxis, SENSE_AREA, find, &item, pathFrom ) )
      return true;
  }
  return false;
} // FindNearestObj


//----------------------------------------------------------------------------------------
// Нахождение ближайшего элемента к заданной точке
// ---
bool FindNearestObj( MbCartPoint & from_p, MbePlaneType pType, MbItem ** find, PathMatrix & pathFrom )
{

  MbRefItem * item = nullptr;
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    // SD#776923: Был получена точка конструктивной плоскости bool GetPoint( const TCHAR * nStr, MbCartPoint & pnt )
    // POINT mp =  wind->GetDevicePoint( from_p );
    // MbCartPoint3D cPoint( wind->GetCartPoint( mp ) );
    // А код дает точку в ЛСК экрана
    MbCartPoint3D cPoint;
    MbPlacement3D cPlace( wind->GetOwnPlace() );
    cPlace.PointOn( from_p, cPoint );

    MbFloatAxis3D cAxis( cPoint, wind->GetAxisZ() );
    if ( wind->FindGeometricItem( st_Undefined, tt_Undefined, pType, cAxis, SENSE_AREA, find, &item, pathFrom ) )
      return true;
  }
  return false;
} // FindNearestObj


//----------------------------------------------------------------------------------------
// Нахождение ближайшего элемента к заданной точке
// ---
bool FindNearestObj( MbCartPoint3D & from_p, MbePlaneType pType, MbItem ** find, PathMatrix & pathFrom )
{
  MbRefItem * item = nullptr;
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    MbFloatAxis3D cAxis( from_p, wind->GetAxisZ() );
    if ( wind->FindGeometricItem( st_Undefined, tt_Undefined, pType, cAxis, SENSE_AREA, find, &item, pathFrom ) )
      return true;
  }
  return false;
} // FindNearestObj


//----------------------------------------------------------------------------------------
// Нахождение ближайшего элемента к заданной точке
// ---
bool FindNearestObj( MbCartPoint3D & from_p, MbeSpaceType sType, MbItem ** find, PathMatrix & pathFrom )
{
  MbRefItem * item = nullptr;
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    MbFloatAxis3D cAxis( from_p, wind->GetAxisZ() );
    if ( wind->FindGeometricItem( sType, tt_Undefined, pt_Undefined, cAxis, SENSE_AREA, find, &item, pathFrom ) )
      return true;
  }
  return false;
} // FindNearestObj


//----------------------------------------------------------------------------------------
// Указание элемента модели по его типу и типу полигона
// ---
bool FindNearestObj( MbCartPoint3D & from_p, MbeSpaceType sType, MbeTopologyType tType,
                     MbItem ** find, MbTopItem ** item, MbMatrix3D & matr )
{
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    MbFloatAxis3D cAxis( from_p, wind->GetAxisZ() );
    if ( wind->FindNearestItem( sType, tType, cAxis, SENSE_RADIUS, find, item, matr ) )
      return true;
  }
  return false;
} // FindNearestObj


//----------------------------------------------------------------------------------------
// Установка проекции
// ---
void SetProjection()
{
  SyncActiveWindowIndex();

  WorkWindow *currentView = TestVariables::viewManager->GetActiveWindow();

  MbPlacement3D placeTmp( currentView->GetOwnPlacement() );
  MbMatrix3D tmpMatrix = placeTmp.GetMatrixInto(); // Берем обратную матрицу
  TestVariables::setProj = false;

  ExecuteSetProjectionDlg( tmpMatrix );

  if ( TestVariables::setProj ) {
    MbCartPoint3D origin = currentView->GetOrigin();
    MbMatrix3D divMatr;
    tmpMatrix.Div( divMatr );

    MbVector3D x = divMatr.GetColumn( 0 );
    MbVector3D z = divMatr.GetColumn( 2 );

    // Очистить проекции модели в активном виде
    ptrdiff_t index = TestVariables::viewManager->GetActiveWindowIndex();
    TestVariables::viewManager->EraseWindow( index );
    TestVariables::viewManager->RefreshWindow( index );
    currentView->InitPlacement( origin, z, x );
    currentView->CalculateMatrix();

    TestVariables::viewManager->DrawContentWindow( index );
  }
} // SetProjection


//----------------------------------------------------------------------------------------
// Изменить глубину проекции
// ---
bool SetDepth()
{
  double depth = 0;

  if ( GetDouble( IDS_ENTER_DEPTH_INCREM, depth ) ) {
    SyncActiveWindowIndex();

    WorkWindow *currentView = TestVariables::viewManager->GetActiveWindow();
    if ( currentView ) {
      MbCartPoint3D origin = currentView->GetOrigin();
      origin.z += depth;
      currentView->SetOrigin( origin );
    }
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------------------
// Установить масштаб вида
// ---
bool SetScale()
{
  SyncActiveWindowIndex();

  WorkWindow * currentView = TestVariables::viewManager->GetActiveWindow();
  if ( currentView ) {
    double scale = currentView->GetScale(); // Выдать текущий масштаб вида

    if ( GetDouble( IDS_ENTER_MAP_SCALE, scale ) ) {
      currentView->SetScale( scale );
      ptrdiff_t index = TestVariables::viewManager->GetActiveWindowIndex();
      // Очистить проекции модели в активном виде
      TestVariables::viewManager->EraseWindow( index );
      TestVariables::viewManager->InvalidateWindow( index, true );

      return true;
    }
  }
  return false;
}


//----------------------------------------------------------------------------------------
// Ввод булевского значения
// ---
void GetBoolean( const TCHAR * nPrompt, bool & bVal, bool resetToFalse )
{
  if ( resetToFalse )
    bVal = false;

  bVal = GetBool( nPrompt, bVal );
}


//----------------------------------------------------------------------------------------
// Ввод булевского значения
// ---
bool GetBoolean( const TCHAR * nPrompt ) {
  return GetBool( nPrompt, true/*defVal*/ );
}


//----------------------------------------------------------------------------------------
// Найти первую '.' с конца строки
//---
static
const TCHAR * FindLastDot( const TCHAR * str )
{
  const TCHAR * c = nullptr;

  if ( str != nullptr ) {
    c = str;

    // Найти конец строки
    while ( *c != _T('\0') ) {
      c++;
    }
    // Найти первую '.' с конца строки
    const TCHAR * s = c;
    while ( *s != _T('.') && (s != str) ) {
      s--;
    }
    if ( *s == _T('.') )
      c = s;
  }

  return c;
}


//----------------------------------------------------------------------------------------
// Перевести в верхний регистр.
//---
static void ToUpper( c3d::string_t& s ) {
  std::use_facet<std::ctype<TCHAR>>(std::locale()).toupper( &s[0], &s[0] + s.size() );
}


//----------------------------------------------------------------------------------------
// Есть ли такое расширение в имени
//---
bool SameExtension( const TCHAR * fullname, const TCHAR * extname )
{
  bool res = false;

  if ( (fullname != nullptr) && (extname != nullptr) ) {
    const TCHAR * checkExtName = ::FindLastDot( fullname );
    if ( checkExtName != nullptr ) {
      if ( _tcscmp( checkExtName, extname ) == 0 )
        res = true;
      else {
        string_t EXTname( extname );
        ToUpper( EXTname );
        if ( _tcscmp( checkExtName, EXTname.c_str() ) == 0 )
          res = true;
      }
    }
  }

  return res;
}


//----------------------------------------------------------------------------------------
// Есть ли такое расширение в имени
//---
inline
const TCHAR * FindExtname( const TCHAR * fullname, const TCHAR * extname )
{
  const TCHAR * foundExtName = nullptr;

  if ( fullname != nullptr ) {
    if ( (extname != nullptr) && (_tcslen(extname) > 1) ) {
      const TCHAR * checkExtName = ::FindLastDot( fullname );

      if ( (checkExtName != nullptr) && (_tcslen(checkExtName) > 1) ) {
        if ( _tcscmp( fullname, ALL_EXT ) != 0 )
          foundExtName = _tcsstr( checkExtName, extname );

#ifdef C3D_WINDOWS // _MSC_VER
        if ( foundExtName == nullptr && (_tcscmp( fullname, ALL_EXT ) != 0) ) {
          string_t EXTname( extname );
          ToUpper( EXTname );
          foundExtName = _tcsstr( checkExtName, EXTname.c_str() );
        }
#endif
      }
    }
  }

  return foundExtName;
}


//----------------------------------------------------------------------------------------
// Перезаписать или добавить расширение имени файла типа (".ext\0")
//---
void RewriteFileExtension( TCHAR * fullname, const TCHAR * extname, bool forWriting )
{
  PRECONDITION( fullname != nullptr );

  if ( fullname != nullptr && extname != nullptr ) {
    if ( debugMessages )
      MessageBoxEx( fullname, IDS_FILE_NAME_BEFORE_CHANGING_EXT );

    ptrdiff_t count = STRINGLENGTH + 1;

    // Найти конец строки
    TCHAR * c = fullname;
    while ( count-- > 0 && *c != _T('\0') ) {
      c++;
    }

    ptrdiff_t fullNameLen = _tcslen( fullname );
    ptrdiff_t extNameLen = (extname != nullptr) ? _tcslen( extname ) : 0;

    // Осталось ли память буфера для добавления расширения ?
    if ( extNameLen < count ) {
      string_t finalExtName( extname );
      const TCHAR * foundExtName = ::FindExtname( fullname, extname );

      if ( foundExtName == nullptr ) {
        foundExtName = ::FindExtname( fullname, X_T_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, X_B_EXT );
        if ( foundExtName != nullptr ) {
          if ( _tcscmp( extname, X_T_EXT ) == 0 )
            finalExtName = foundExtName;
        }
      }
      if ( foundExtName == nullptr ) {
        foundExtName = ::FindExtname( fullname, STP_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, STEP_EXT );
        if ( foundExtName != nullptr ) {
          if ( _tcscmp( extname, STP_EXT ) == 0 )
            finalExtName = foundExtName;
        }
      }
      if ( foundExtName == nullptr ) {
        foundExtName = ::FindExtname( fullname, JT_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, JT_EXT );
        if ( foundExtName != nullptr ) {
          if ( _tcscmp( extname, JT_EXT ) == 0 )
            finalExtName = foundExtName;
        }
      }
      if ( foundExtName == nullptr ) {
        foundExtName = ::FindExtname( fullname, IGS_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, IGES_EXT );
        if ( foundExtName != nullptr ) {
          if ( _tcscmp( extname, IGS_EXT ) == 0 )
            finalExtName = foundExtName;
        }
      }
      if ( foundExtName == nullptr ) {
        foundExtName = ::FindExtname( fullname, SAT_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, SAB_EXT );
        if ( foundExtName != nullptr ) {
          if ( _tcscmp( extname, SAT_EXT ) == 0 )
            finalExtName = foundExtName;
          else if ( _tcscmp( extname, SAB_EXT ) == 0 )
            finalExtName = foundExtName;
        }
      }
      if ( foundExtName == nullptr ) {
        foundExtName = ::FindExtname( fullname, ASC_EXT );
        if ( foundExtName == nullptr ) {
          foundExtName = ::FindExtname( fullname, XYZ_EXT );
          if ( foundExtName == nullptr )
            foundExtName = ::FindExtname( fullname, TXT_EXT );
          }
        if( foundExtName != nullptr ) {
          if( _tcscmp( extname, ASC_EXT ) == 0 )
            finalExtName = foundExtName;
          else if( _tcscmp( extname, XYZ_EXT ) == 0 )
            finalExtName = foundExtName;
          else if( _tcscmp( extname, TXT_EXT ) == 0 )
            finalExtName = foundExtName;
        }
      }

      if ( foundExtName == nullptr ) {
        foundExtName = ::FindExtname( fullname, C3D_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, MAT_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, STL_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, VRML_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, OBJ_EXT );
        if ( foundExtName == nullptr )
          foundExtName = ::FindExtname( fullname, GRDECL_EXT );
      }

      if ( (foundExtName != nullptr) || ((fullNameLen + extNameLen) > STRINGLENGTH) ) {
        { // Найти первую '.' с конца строки
          TCHAR * s = c;
          while ( *s != _T('.') && (s != fullname) ) {
            s--;
          }
          if ( *s == _T('.') )
            c = s;
        }

        // Перебить расширение файла
#ifdef C3D_WINDOWS // _MSC_VER
;//        finalExtName.to_lower();
#endif
        if ( foundExtName == nullptr )
          _tcscpy( c, finalExtName.c_str() );
        else {
          if ( _tcscmp( finalExtName.c_str(), NON_EXT ) != 0 ) // если расширение известно
            _tcscpy( c, finalExtName.c_str() ); // устанавливаем расширение
          else { // если расширение неизвестно
            if ( forWriting ) // то при записи
              _tcscpy( c, NON_EXT ); // удаляем расширение
            else // а при чтении
              _tcscpy( c, foundExtName ); // сохраняем расширение
          }
        }
      }
      else { // Ничего не надо перебивать, т.к. наших расширений в имени нет, поэтому просто добавить расширение
        if ( forWriting )
          _tcscat( fullname, extname );
        else if ( _tcscmp( fullname, ALL_EXT ) != 0 )
          _tcscat( fullname, extname );
        else
          _tcscpy( fullname, NON_EXT ); // чтение с несколькими расширениям
      }
    }

    if ( debugMessages )
      MessageBoxEx( fullname, IDS_FILE_NAME_AFTER_CHANGING_EXT );
  }
}

//#define __DEBUG_MODEL_TREE__
//----------------------------------------------------------------------------------------
// Чтение модели из файла
// ---
void ReadModel( const TCHAR * path )
{
  const TCHAR * fileName( path );
#if defined C3D_WINDOWS && defined PLATFORM_32
  FatalErrorHandler::SetMemoryThreshold( 1024. ); // уменьшаем порог памяти, чтобы OpenGL выжил
#endif
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();

  if ( fileName == nullptr ) {
#ifndef __USE_QT__
      if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, OPEN_EXT_SUPPORTED_INFO, NON_EXT ) )
        fileName = TestVariables::nameBuffer;
#else
      if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, nullptr, nullptr ) )
        fileName = TestVariables::nameBuffer;
#endif
  }

  if ( wind == nullptr ) {
    wind = new WorkWindow( v_Isometry );
    wind->CreateWorkWindow( IDS_TITLE_VIEWISOMETRY, TestVariables::viewManager->GetBackgrColor(), TestVariables::viewManager->GetLightDirection()/*lightDirection*/ );
    wind->OrientateWindow( v_Isometry );

    ptrdiff_t newIndex = TestVariables::viewManager->AddWindow( wind );
    TestVariables::viewManager->SetFocus( newIndex, false );
  }

  if ( fileName != nullptr ) {
    bool isGoodRead = false;
    uint32 readFlag = 0;
    // Запомненный объект и его элемент
    TestVariables::ResetHLighted();
    const size_t bufLen = 1024;
    TCHAR buf[bufLen];
    size_t len = 0;
    if ( TestVariables::checkReadTime )
      len += (size_t)_sntprintf( buf+len, bufLen-len, RT_TEST_RESULTS );

    MbAccurateTimer ac;

    if ( ::SameExtension( fileName, C3D_EXT ) ) { // C3D
      std::unique_ptr<reader_ex> pl = reader_ex::CreateReaderEx( std::unique_ptr<iobuf_Seq>(&::createiobuf( fileName )), io::in );
      DPtr<IProgressIndicator> progBar( &::CreateProgressIndicator( new StrSpyCommon ) );
      pl->InitProgress( progBar );
      isGoodRead = pl->good();

      if ( isGoodRead ) {
        SetWaitCursor( true );

        ac.Begin();

        wind->ReadModel( *pl );

        ac.End();

        SetWaitCursor( false );

#ifdef __DEBUG_MODEL_TREE__
        const IModelTree* tree = pl.GetModelTree();
        if ( tree ) {
          TCHAR nameBuff[257];
          _tcscpy ( nameBuff, fileName );

          RewriteFileExtension ( nameBuff, _T("_read.xml") );

          t_ofstream treeFile(c3d::C3DToPathstring(nameBuff ? c3d::string_t(nameBuff) : c3d::string_t()).c_str());
          if ( treeFile.is_open() ) 
            treeFile << *tree;
        }
#endif
      }
      readFlag = pl->state();
      isGoodRead = pl->good();
      pl->FinishProgress();
      }
    else if ( ::SameExtension( fileName, MAT_EXT ) ) { // MAT (устаревший, только чтение)
      std::unique_ptr<reader> pl = reader::CreateReader( std::unique_ptr<iobuf_Seq>(&::createiobuf(fileName)), io::in );
      isGoodRead = pl->good();

      if ( isGoodRead ) {
        SetWaitCursor( true );

        ac.Begin();

        bool isRead = wind->ReadItem( *pl );

        ac.End();

        SetWaitCursor( false );

        if ( !isRead )
          MessageBoxEx( RT_ERROR_READING_PARTLY, IDS_TITLE_ERR );
      }
    }
    else { // импортируемые форматы
      SetWaitCursor( true );

      char * currentLocale = setlocale( LC_CTYPE, nullptr );  // иначе не читаются пути с кириллицей
      setlocale(LC_CTYPE, "");

      if ( allSupportedFormats ) {
        TestVariables::viewManager->DeleteModel();

        DPtr<IProgressIndicator> progBar( &::CreateProgressIndicator( new StrSpyConverters ) );

        ac.Begin(); 
        ConvConvertorProperty3D convProperties;
        convProperties.fileName.assign( c3d::C3DToPathstring( fileName ) );
        bool bPmiToItems( false );
        ::UpdateConverterSettings( convProperties, bPmiToItems );
        MbeConvResType readResult( cnv_Error );
        if ( bPmiToItems )
        {
          C3dModelDocument modelDoc( SPtr<C3DPmiToItem>( new C3DPmiToItem( SPtr<C3DSymbolToItem>( new C3DSymbolToItem() ) ) ) );
          readResult = ::ImportFromFile( modelDoc, C3DToPathstring( fileName ), &convProperties, progBar );
          if ( readResult == cnv_Success )
            wind->AddItem( *modelDoc.GetContent(), nullptr, true );
        }
        else
          readResult = ::ImportFromFile( wind->SetGeomModel(), C3DToPathstring( fileName ), &convProperties, progBar );

        isGoodRead = ( cnv_Success == readResult );
        if ( cnv_UnknownExtension == readResult ) {
          isGoodRead = WrConvertor3D::PluginRead( fileName );
        }
        ac.End();

        if ( isGoodRead )
          wind->RefreshModel();
        }
      else {
        ac.Begin();

        if ( ::SameExtension( fileName, SAT_EXT ) || ::SameExtension( fileName, SAB_EXT ) )
          isGoodRead = WrConvertor3D::SATRead( fileName );
        else if ( ::SameExtension( fileName, IGS_EXT ) || ::SameExtension( fileName, IGES_EXT ) )
          isGoodRead = WrConvertor3D::IGESRead( fileName );
        else if ( ::SameExtension( fileName, X_T_EXT ) || ::SameExtension( fileName, X_B_EXT ) )
          isGoodRead = WrConvertor3D::XTRead( fileName );
        else if ( ::SameExtension( fileName, STP_EXT ) || ::SameExtension( fileName, STEP_EXT ) )
          isGoodRead = WrConvertor3D::STEPRead( fileName );
        else if ( ::SameExtension( fileName, JT_EXT ) )
          isGoodRead = WrConvertor3D::JTRead( fileName );
        else if ( ::SameExtension( fileName, STL_EXT ) )
          isGoodRead = WrConvertor3D::STLRead( fileName );
        else if ( ::SameExtension( fileName, VRML_EXT ) )
          isGoodRead = WrConvertor3D::VRMLRead( fileName );
        else if ( ::SameExtension( fileName, OBJ_EXT ) )
          isGoodRead = WrConvertor3D::OBJRead( fileName );
        else if ( ::SameExtension( fileName, GRDECL_EXT ) )
          isGoodRead = WrConvertor3D::GRDECLRead( fileName );
        else if ( ::SameExtension( fileName, TXT_EXT ) || ::SameExtension( fileName, ASC_EXT ) ||
          ::SameExtension( fileName, XYZ_EXT ) )
          isGoodRead = WrConvertor3D::ASCIIPointCloudRead( fileName );
        else {
          isGoodRead = WrConvertor3D::PluginRead( fileName );
        }

        ac.End();
      }

      setlocale( LC_CTYPE, currentLocale );

      SetWaitCursor( false );
    }
    //TestVariables::checkReadTime = true;
    if ( TestVariables::checkReadTime ) {
      len += (size_t)_sntprintf( buf+len, bufLen-len, RT_TIME_SEC, ac.GetLast() );
      ::MessageBoxEx( buf, IDS_INFORMATION );
    }

    if ( isGoodRead ) {
      wind->SetModelName( fileName );
      if ( ( readFlag & io::skippedUnknAttr ) != 0 ) {
        MessageBoxEx( RT_FILE_WITH_UNKNOWN_ATTR, IDS_TITLE_WRN );
      }
      else if ( ( readFlag & io::skippedUnknown ) != 0 ) {
        MessageBoxEx( RT_FILE_WITH_UNKNOWN_OBJECTS, IDS_TITLE_WRN );
      }
      else if ( ( readFlag & io::verViolation ) != 0 ) {
        MessageBoxEx( RT_WAR_FILE_VERSION, IDS_TITLE_WRN );
      }
    }
    else {
      if ( (readFlag & io::cantReadCatalog) != 0 )
        MessageBoxEx( RT_FILE_FAIL_READ_CATALOG, IDS_TITLE_ERR );
      else if ( ( readFlag & io::badData ) != 0)
        MessageBoxEx( RT_FILE_BAD_FORMAT, IDS_TITLE_ERR );
      else if ( (readFlag & io::badClassId) != 0 )
        MessageBoxEx( RT_FILE_BAD_CLASSID, IDS_TITLE_ERR );
      else if ( ( readFlag & io::outOfMemory ) != 0 ) {
        MessageBoxEx( RT_FILE_OUT_OF_MEM, IDS_TITLE_ERR );
      }
      else if ( ( readFlag & io::emptyFile ) != 0 ) {
        MessageBoxEx( RT_NULL_FILE, IDS_TITLE_ERR );
      }
      else if ( ( readFlag & io::notFound ) != 0 ) {
        MessageBoxEx( RT_FILE_NOT_FOUND, IDS_TITLE_ERR );
      }
      else if ( ( readFlag & io::verViolation ) != 0 ) {
        MessageBoxEx( RT_ERR_FILE_VERSION, IDS_TITLE_ERR );
      }
      else
        MessageBoxEx( RT_FILE_NOT_OPEN, IDS_TITLE_ERR );
    }
  }
#if defined C3D_WINDOWS && defined PLATFORM_32
  FatalErrorHandler::ResetMemoryThreshold();
#endif
}

//----------------------------------------------------------------------------------------
// Запись модели в файл
// ---
void WriteModel( TCHAR * fileName0, VERSION dstVersion )
{  
  if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
  {
    bool nameIsReady = false;

    if ( fileName0 != nullptr && (_tcslen(fileName0) > 4) ) {
      if ( ::SameExtension( fileName0, C3D_EXT ) ) { // C3D
        _tcscpy( TestVariables::nameBuffer, fileName0 ); // Имя файла модели.
        nameIsReady = true;
      }
    }
    if ( !nameIsReady ) {
      if ( !wind->IsModelNameEmpty() )
        _tcscpy( TestVariables::nameBuffer, wind->GetModelName() ); // Имя файла модели.

      //if ( _GetSaveFileName( TestVariables::nameBuffer, RT_WRITTING_IN_MODEL_FILE, C3D_EXT_INFO, C3D_EXT ) ) {
      if ( _GetSaveFileName( TestVariables::nameBuffer, RT_WRITTING_IN_MODEL_FILE, nullptr, nullptr ) ) {
        nameIsReady = true;
      }
    }
    if ( nameIsReady ) {
      bool isGoodWrite = false;

      const size_t bufLen = 1024;
      TCHAR strBuf[bufLen];
      size_t len = 0;
      if ( TestVariables::checkWriteTime )
        len += (size_t)_sntprintf( strBuf+len, bufLen-len, RT_TEST_RESULTS );

      MbAccurateTimer ac;

      if ( ::SameExtension( TestVariables::nameBuffer, C3D_EXT ) ) { // C3D
        ac.Begin();

        membuf memBuf;           // Создаем membuf
        {
          bool canUseModelTree = false;

          if ( !::CanWriteToMathFileVersion( dstVersion, &canUseModelTree ) ) {
            C3D_ASSERT_UNCONDITIONAL( false );
            dstVersion = ::GetCurrentMathFileVersion();
          }

          bool saveToPrevVersion = (dstVersion < ::GetCurrentMathFileVersion());

          // \ru Создаем подходящий writer в зависимости от опции
          writer::writer_ptr out = ( TestVariables::useModelTree && canUseModelTree ) // \ru Если заказано сохранение в компактном формате, то используем его.
            ? writer::writer_ptr( writer_ex::CreateMemWriterEx(memBuf, 0).release() )
            : writer::writer_ptr( writer::CreateMemWriter(memBuf, 0) );

          isGoodWrite = out->good();

          if ( isGoodWrite && saveToPrevVersion ) { // C3D-1182
            VersionContainer verCont = out->GetVersionsContainer();
            verCont.SetVersion( 0, dstVersion );
            out->SetVersionsContainer( verCont );
            out->SetStorageVersion( dstVersion );
          }

          if ( isGoodWrite ) {
            SetWaitCursor( true );
            wind->WriteModel ( *out );
            memBuf.setMaxRegCount ( out->GetMaxRegisteredCount() ); // \ru Максимальное количество регистрируемых объектов. \en The maximum number of registered facilities. 

            if ( !out->good() )
              MessageBoxEx( RT_WRITTING_ERROR, IDS_TITLE_ERR );
            else {
              wind->SetModelName( TestVariables::nameBuffer );
            }

#ifdef __DEBUG_MODEL_TREE__
            const IModelTree * tree = out->GetModelTree();
            if ( tree ) {
              TCHAR nameBuff[257];
              _tcscpy ( nameBuff, TestVariables::nameBuffer );

              RewriteFileExtension ( nameBuff, _T("_write.xml") );

              t_ofstream treeFile(c3d::C3DToPathstring(nameBuff ? c3d::string_t(nameBuff) : c3d::string_t()).c_str());
              if ( treeFile.is_open() )
                treeFile << *tree;
            }
#endif

            SetWaitCursor( false );

            isGoodWrite = out->good();
          }
        }

        if ( isGoodWrite )
          isGoodWrite = ::writeiobuftodisk( TestVariables::nameBuffer, memBuf ); // Пишем его на диск

        ac.End();
      }
      else {
        const TCHAR * fileName = TestVariables::nameBuffer;

        ac.Begin();

        if ( ::SameExtension( fileName, SAT_EXT ) || ::SameExtension( fileName, SAB_EXT ) )
          isGoodWrite = WrConvertor3D::SATWrite( fileName );
        else if ( ::SameExtension( fileName, IGS_EXT ) || ::SameExtension( fileName, IGES_EXT ) )
          isGoodWrite = WrConvertor3D::IGESWrite( fileName );
        else if ( ::SameExtension( fileName, X_T_EXT ) || ::SameExtension( fileName, X_B_EXT ) )
          isGoodWrite = WrConvertor3D::XTWrite( fileName );
        else if ( ::SameExtension( fileName, STP_EXT ) || ::SameExtension( fileName, STEP_EXT ) )
          isGoodWrite = WrConvertor3D::STEPWrite( fileName );
        else if ( ::SameExtension( fileName, JT_EXT ) )
          isGoodWrite = WrConvertor3D::JTWrite( fileName );
        else if ( ::SameExtension( fileName, STL_EXT ) )
          isGoodWrite = WrConvertor3D::STLWrite( fileName );
        else if ( ::SameExtension( fileName, VRML_EXT ) )
          isGoodWrite = WrConvertor3D::VRMLWrite( fileName );
        else if ( ::SameExtension( fileName, GRDECL_EXT ) )
          isGoodWrite = WrConvertor3D::GRDECLWrite( fileName );
        else if ( ::SameExtension( fileName, TXT_EXT ) || ::SameExtension( fileName, ASC_EXT ) ||
                  ::SameExtension( fileName, XYZ_EXT ) )
          isGoodWrite = WrConvertor3D::ASCIIPointCloudWrite( fileName );

        ac.End();
      }

      if ( TestVariables::checkWriteTime ) {
        len += (size_t)_sntprintf( strBuf+len, bufLen-len, RT_TIME_SEC, ac.GetLast() );
        ::MessageBoxEx( strBuf, IDS_INFORMATION );
      }

      if ( !isGoodWrite )
        MessageBoxEx( RT_FILE_NOT_WRITTEN, IDS_TITLE_ERR );
    }
  }
}


//----------------------------------------------------------------------------------------
// Запись тела в файл
// ---
bool WriteSolid( const TCHAR * pathName, MbSolid & solid )
{  
  bool isGoodWrite = false;    
  
  if ( ::SameExtension( pathName, C3D_EXT ) ) { // C3D
    solid.AddRef();
    MbModel * geomModel = new MbModel();
    geomModel->AddItem( solid, 0 );

    membuf memBuf;           // Создаем membuf
    std::unique_ptr<writer> rc = writer::CreateMemWriter( memBuf, 0 );
    isGoodWrite = rc->good();

    if ( isGoodWrite ) {
      ::WriteModelItems( *rc.get(), *geomModel );
      memBuf.setMaxRegCount ( rc->GetMaxRegisteredCount() ); // \ru Максимальное количество регистрируемых объектов. \en The maximum number of registered facilities. 

      isGoodWrite = ::writeiobuftodisk( pathName, memBuf ); // Пишем его на диск
    }
    ::DeleteItem( geomModel );
    solid.DecRef();
  }
  return isGoodWrite;
}


//----------------------------------------------------------------------------------------
// Добавить в модель файл
// ---
void AddModel()
{
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    if ( !wind->IsModelNameEmpty() )
      _tcscpy( TestVariables::nameBuffer, wind->GetModelName() ); // Имя файла модели.

    string_t nameBufferBak( TestVariables::nameBuffer );
    bool isGoodRead = false;
    bool hasUnknownObjects = false;

    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, nullptr, nullptr ) ) {
      if ( ::SameExtension( TestVariables::nameBuffer, C3D_EXT ) ) { // C3D
        std::unique_ptr<reader_ex> pl = 
          reader_ex::CreateReaderEx( std::unique_ptr<iobuf_Seq>(&::createiobuf(TestVariables::nameBuffer)), io::in );
        isGoodRead = pl->good();

        if ( isGoodRead ) {
          SetWaitCursor( true );

          wind->ReadAddModel( *pl );
          hasUnknownObjects = (pl->state() & io::skippedUnknown) != 0;

          SetWaitCursor( false );
        }
      }
      else { // импортируемые форматы
        bool repl = TestVariables::replaceItems;
        TestVariables::replaceItems = false;
        char * currentLocale = setlocale( LC_CTYPE, nullptr );  // иначе не читаются пути с кириллицей
        setlocale(LC_CTYPE, "");

        if ( ::SameExtension( TestVariables::nameBuffer, SAT_EXT ) || ::SameExtension( TestVariables::nameBuffer, SAB_EXT ) )
          isGoodRead = WrConvertor3D::SATRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, IGS_EXT ) || ::SameExtension( TestVariables::nameBuffer, IGES_EXT ) )
          isGoodRead = WrConvertor3D::IGESRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, X_T_EXT ) || ::SameExtension( TestVariables::nameBuffer, X_B_EXT ) )
          isGoodRead = WrConvertor3D::XTRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, STP_EXT ) || ::SameExtension( TestVariables::nameBuffer, STEP_EXT ) )
          isGoodRead = WrConvertor3D::STEPRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, JT_EXT ) )
          isGoodRead = WrConvertor3D::JTRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, STL_EXT ) )
          isGoodRead = WrConvertor3D::STLRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, VRML_EXT ) )
          isGoodRead = WrConvertor3D::VRMLRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, OBJ_EXT ) )
          isGoodRead = WrConvertor3D::OBJRead( TestVariables::nameBuffer );
        else if ( ::SameExtension( TestVariables::nameBuffer, GRDECL_EXT ) )
          isGoodRead = WrConvertor3D::GRDECLRead( TestVariables::nameBuffer );

        setlocale( LC_CTYPE, currentLocale );
        TestVariables::replaceItems = repl;
      }
    }
    if ( !nameBufferBak.empty() )
      _tcscpy( TestVariables::nameBuffer, nameBufferBak.c_str() ); // вернуть имя файла, куда добавляли
    if ( hasUnknownObjects )
      MessageBoxEx( RT_FILE_WITH_UNKNOWN_OBJECTS, IDS_TITLE_WRN );
    else if ( !isGoodRead )
      MessageBoxEx( RT_FILE_NOT_OPEN, IDS_TITLE_ERR );
  }
}


//----------------------------------------------------------------------------------------
// Чтение геометрической модели из файла *.MAT
// Read the geometric model from *.MAT file
// ---
void ReadItem()
{
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();
  if ( wind != nullptr ) {
    if ( !wind->IsModelNameEmpty() )
      _tcscpy( TestVariables::nameBuffer, wind->GetModelName() ); // Имя файла модели.

    if ( _GetOpenFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, MAT_EXT_INFO, MAT_EXT ) ) {
      std::unique_ptr<reader> pl = 
        reader::CreateReader( std::unique_ptr<iobuf_Seq>(&::createiobuf(TestVariables::nameBuffer)), io::in );

      if ( pl->good() ) {
        SetWaitCursor( true );

        // Запомненный объект и его элемент
        TestVariables::ResetHLighted();
         

        bool isRead = wind->ReadItem( *pl );
        wind->SetModelName( TestVariables::nameBuffer );

        SetWaitCursor( false );

        if ( !isRead )
          MessageBoxEx( RT_ERROR_READING_PARTLY, IDS_TITLE_ERR );
      }
      else
        MessageBoxEx( RT_FILE_NOT_OPEN, IDS_TITLE_ERR );
    }
  }
}


//----------------------------------------------------------------------------------------
// Запись геометрической модели в формат *.MAT
// Write the geometric model into format *.MAT
// ---
void WriteItem()
{  
  if ( WorkWindow * wind = TestVariables::viewManager->GetActiveWindow() )
  {
    if ( !wind->IsModelNameEmpty() )
      _tcscpy( TestVariables::nameBuffer, wind->GetModelName() ); // Имя файла модели.

    if ( _GetSaveFileName( TestVariables::nameBuffer, SELECT_FILE_INFO, MAT_EXT_INFO, MAT_EXT ) )
    {
      membuf buf; // Создаем membuf
      {
        writer::writer_ptr rc = writer::CreateMemWriter( buf, 0 );
        if ( rc->good() ) {
          SetWaitCursor( true );
          wind->WriteItem( *rc );
          buf.setMaxRegCount( rc->GetMaxRegisteredCount() ); // \ru Максимальное количество регистрируемых объектов. \en The maximum number of registered facilities. 
          SetWaitCursor( false );
        }
        else
          MessageBoxEx( RT_FILE_NOT_WRITTEN, IDS_TITLE_ERR );
      }
      ::writeiobuftodisk( TestVariables::nameBuffer, buf ); // Пишем его на диск
    }
  }
}

//----------------------------------------------------------------------------------------
// AP
// ---
static
void SecondsToTimeString( int32 time, TCHAR* lpOut ) 
{
  if( lpOut == nullptr )
    return;

  // maximum: 99:59:59:999
  if( time > 359999999 ) {
    ::_tcscpy( lpOut, _T("infinite") );
    return;
  }

  int32 h, m, s, ms;

  if( time <= 0 ) {
    h = 0; m = 0; s = 0, ms = 0;
  }
  else {
    h = time / 3600000;
    time %= 3600000;
    m = time / 60000;
    time %= 60000;
    s = time / 1000;
    time %= 1000;
    ms = time;
  }

  _sntprintf( lpOut, 256, _T("%02u:%02u:%02u:%03u"), h, m, s, ms );
}


//----------------------------------------------------------------------------------------
// Выдать информацию о времени работы
// ---
void ShowTimeTestResult( TimeTest * testResult,
                         uint32 miliSec,
                         const TCHAR * sss )
{
  TCHAR buf0[128];
  TCHAR buf3[128];

  SecondsToTimeString( miliSec, buf0 );

  _sntprintf( buf3, 128, RT_TEST_TIME_MSEC, (TCHAR*)buf0, miliSec );

  const void * params[5] = { sss, nullptr, nullptr, buf3, testResult };

  ExecuteTimeTestResultDlg(&params);
}

//----------------------------------------------------------------------------------------
// Выдать информацию о времени работы
// ---
void ShowTimeTestResult( TimeTest * testResult,
                         size_t operationCount,
                         size_t facesCount,
                         uint32 miliSec,
                         bool isItemRebuild,
                         const TCHAR * sss )
{
  TCHAR buf0[128];
  TCHAR buf1[128];
  TCHAR buf2[128];
  TCHAR buf3[128];

  SecondsToTimeString( miliSec, buf0 );

  if ( isItemRebuild ) {
    _sntprintf( buf1, 128, RT_OPERATIONS_COUNT F_TD _T(" "), operationCount );           //-V111
    _sntprintf( buf2, 128, RT_FACES_COUNT      F_TD _T(" "), facesCount );               //-V111
  }
  else {
    _sntprintf( buf1, 128, RT_VISIBLE_LINES_COUNT F_TD _T(" "), operationCount );     //-V111
    _sntprintf( buf2, 128, RT_HIDE_LINES_COUNT    F_TD _T(" "), facesCount );         //-V111
  }

  _sntprintf( buf3, 128, RT_TEST_TIME_MSEC, (TCHAR*)buf0, miliSec );

  const void * params[5] = { sss, buf1, buf2, buf3, testResult };

  ExecuteTimeTestResultDlg(&params);
}

//----------------------------------------------------------------------------------------
// Выдать информацию о времени работы теста сопряжений.
// ---
void ShowMateTimeTestResult( TimeTest *     testResult
                           , size_t         mateCount
                           , size_t         bodyCount
                           , GCM_result result
                           , const TCHAR *  sss )
{
  TCHAR buf0[128];
  TCHAR buf1[128];
  TCHAR buf2[128];
  TCHAR buf3[128];

  SecondsToTimeString( result, buf0 );
  _sntprintf( buf1, 128, RT_MATES_COUNT F_TD _T(" "), mateCount );
  _sntprintf( buf2, 128, RT_BLOCKS_COUNT F_TD _T(" "), bodyCount );
  _sntprintf( buf3, 128, RT_SOLVING_CODE, result );

  const void * params[5] = { sss, buf1, buf2, buf3, testResult };
  ExecuteTimeTestResultDlg( &params );
}

//----------------------------------------------------------------------------------------
// Вернуть resId ошибки построения solida
// Перевод enuma MbResultType в идентификаторы main.rh
// ---
static const TCHAR* /*uint*/ GetSolidErrorResId( uint solidError )
{
  switch ( solidError )
  {
    case rt_Success:                     // Нормальная работа
      return RT_OK;
    case rt_Empty:
      return RT_EMPTY;              // Пустой
    case rt_ToManyAxis:
      return RT_TOMANYAXIS;         // Слишком много осей
    case rt_ToFewAxis:
      return RT_TOFEWAXIS;          // Не хватает осей
    case rt_ToManyContours:
      return RT_TOMANYCONTOURS;     // Слишком много контуров
    case rt_Stars:
      return RT_STARS;              // Есть "звезда"
    case rt_SelfIntersection:
      return RT_SELFINTERSECTION;   // Самопересечение
    case rt_Intersection:
      return RT_INTERSECTION;       // Пересечение контуров
    case rt_NoIntersectSolid:       // Контур не пересекает поверхность тела
      return RT_NO_INTERSECTION_SOLID;
    case rt_NoIntersectSection:     // Образующий контур не пересекает сечения (для операции построения тела по сечениям)
      return RT_NO_INTERSECT_SECTION;  // Образующий контур не пересекает сечения (для операции построения тела по сечениям)
    case rt_MustBeClosed:
      return RT_MUST_BE_CLOSED;     // Должен быть замкнут
    case rt_MustBeOpen:
      return RT_MUST_BE_OPEN;       // Должен быть разомкнут
    case rt_AxisIntersection:
      return RT_AXIS_INTERSECTION;  // Пересечение с осью
    case rt_InvalidType:
      return RT_INVALID_TYPE;       // Неподходящий тип кривой

    // Типы ошибок
    case rt_NoSequenceCurveAndSections:  // Не последовательное расположение сечений вдоль кривой (для операции построения тела по сечениям)
      return RT_NO_SEQUENCE_CURVE_AND_SECTIONS;  // Не последовательное расположение сечений вдоль кривой (для операции построения тела по сечениям)
    case rt_MultiSolid:             // Тело состоит из отдельных частей
      return RT_MULTISOLID;
    case rt_CurveError:             // Ошибочная кривая
      return RT_CURVEERROR;
    case rt_ContourError:           // Ошибочный контур
      return RT_CONTOURERROR;
    case rt_SurfaceError:           // Ошибочная поверхность
      return RT_SURFACEERROR;
    case rt_SolidError:             // Ошибочное тело
      return RT_SOLIDERROR;
    case rt_ParameterError:         // Ошибочный параметр
      return RT_PARAMETERERROR;
    case rt_ThicknessError:         // Неверно задана толщина
       return RT_THICKNESSERROR;
    case rt_SelfIntersect:          // Объект самопересекается
      return RT_SELFINTERSECT;
    case rt_SelfIntWhenExtended:    // Самопересечение в продолжении контура
      return RT_SELFINTWHENEXTENDED;
    case rt_Intersect:              // Объекты пересекаются
      return RT_INTERSECT;
    case rt_NoIntersect:            // Объекты не пересекаются
      return RT_NOINTERSECT;
    case rt_OffsetIntersectError:         // Объекты пересекается с ошибкой
      return RT_ERRORINTERSECT;
    case rt_BooleanError:           // Ошибка в булевой операции
      return RT_BOOLEANERROR;
    case rt_NoEdges:                // Ребра не найдены
      return RT_NOEDGES;
    case rt_PrepareError:           // Ошибка при подготовке операции
      return RT_PREPAREERROR;
    case rt_ChamferError:           // Ошибка при создании фаски ребра
      return RT_CHAMFERERROR;
    case rt_FilletError:            // Ошибка при скруглении ребра
      return RT_FILLETERROR;
    case rt_ChamferSurfaceError:    // Ошибка при создании поверхности фаски ребра
      return RT_CHAMFER_SURFACE_ERROR;
    case rt_FilletSurfaceError:     // Ошибка при создании поверхности скругления ребра
      return RT_FILLET_SURFACE_ERROR;
    case rt_TooLargeChamfer:        // Слишком большые катеты фаски
      return RT_TOO_LARGE_CHAMFER;
    case rt_TooLargeFillet:         // Слишком большой радиус скругления
      return RT_TOO_LARGE_FILLET;
    case rt_SemiChamfer:           // Фаски построены на для всех ребер
      return RT_SEMI_CHAMFER;
    case rt_SemiFillet:            // Скруглены не все ребра
      return RT_SEMI_FILLET;
    case rt_CuttingError :          // Ошибка резки поверхностью
      return RT_CUTTINGERROR;
    case rt_ThinError:              // Ошибка при создании тонкостенного тела
      return RT_THINERROR;
    case rt_RibError :              // Ошибка постановки ребра жесткости
      return RT_RIBERROR;
    case rt_DraftError:             // Неизвестная ошибка уклона граней тела
      return RT_DRAFTERROR;
    case rt_CutBySilhouetteError:   // Ошибка разреза силуэтной линией.
      return RT_CUTBYSILHOUETTE;
    case rt_SplitWireNotIntersectFace:  // Контур не пересекает ни одну из граней или совпадает с кромкой грани(MA 17.07.2001)
      return RT_SPLIT_WIRE_NOT_INTERSECT_FACE;

    case rt_SplitWireNotSplitFace:      // Контур не разбивает
      return RT_SPLIT_WIRE_NOT_SPLIT_FACE;
    case rt_NotAllContoursUsed: // Not all contours or curves were used
      return RT_NOT_ALL_ITEMS_WERE_USED;

    case rt_ProcessIsStopped:
      return RT_PROCESS_INTERRUPTED;

    case rt_Error:                      // Ошибка
    default :
      return RT_ERROR_;
  }
} // GetSolidErrorResId


//----------------------------------------------------------------------------------------
// Вернуть resId ошибки сшивки
// Перевод enuma MbStitchResType в идентификаторы main.rh
// ---
static const TCHAR* /*uint*/ GetStitchErrorResId( uint stitchError ) {

  switch ( stitchError ) {

    case stch_Success:                    // Нормальная работа
      return RT_OK;
    case stch_PrepareError:               // Ошибка при подготовке операции
      return RT_PREPAREERROR;
    case stch_CoorientFaceError:          // Невозможно выставить согласованную ориентацию граней
      return RT_COORIENTFACEERROR;
    case stch_SomeEdgesUnstitched:        // Некоторые рёбра остались не сшитыми
      return RT_SOMEEDGESUNSTITCHED;
    case stch_OutwardOrientError:         // Не удалось установить нормали граней наружу тела
      return RT_OUTWARDORIENTERROR;
    case stch_NoEdgeWasStitched:          // Не было сшито ни одного ребра
      return RT_NOEDGEWASSTITCHED;
    case stch_SeparatePartsResult:        // После сшивки остались несвязанные между собой куски
      return RT_SEPARATEPARTSRESULT;

    default :                             // Ошибка
      return RT_ERROR_;
  }
} // GetStitchErrorResId


//-----------------------------------------------------------------------------------------
// Функция записи параметров документа в файл
// ---
#ifdef C3D_DEBUG

#ifndef C3D_WINDOWS // _MSC_VER
  #define SIZE_WRITE  size_t
  #define HANDLE      FILE*
  #define CloseHandle fclose
  static void WriteFile( HANDLE      hFile,
                         const void *lpBuffer,
                         SIZE_WRITE  nNumberOfBytesToWrite,
                         SIZE_WRITE  *lpNumberOfBytesWritten,
                         const void *lpOverlapped )
  {
    size_t size = fwrite( lpBuffer, nNumberOfBytesToWrite, 1/*count*/,  hFile );
  }
#else // _WIN32
  #define SIZE_WRITE DWORD
#endif // _WIN32

static void WriteDocToFile( void * ptext )
{
  string_t fileName;
  fileName.assign( TestVariables::nameBuffer );
  if ( fileName.empty() ) {
    fileName.assign( _T("noname") );
    fileName.append( TXT_EXT );
  }
  else {
    string_t dotStr(_T("."));
    size_t pos = fileName.find_last_of( dotStr );
    if ( pos == string_t::npos )
      fileName.append( TXT_EXT );
    else {
      size_t len = fileName.length();
      if ( pos == len - 4 ) {
        dotStr.assign( TXT_EXT );
        fileName.replace( pos, 4, dotStr );
      }
      else {
        fileName.append( TXT_EXT );
      }
    }
  }

  HANDLE hFile = nullptr; // Имя файла
  if ( TestVariables::rebuildTimeToFile != 0 ) {
    //создаём файл
#ifdef C3D_WINDOWS // _MSC_VER
    hFile = CreateFile( fileName.c_str(),     // Имя файла
                        GENERIC_WRITE,        // Создание на запись
                        0,                    // Флаг доступа для другого процесса
                        nullptr,                 // Аттрибуты безопасности
                        CREATE_ALWAYS,        // Открываем файл
                        FILE_ATTRIBUTE_NORMAL,// Аттрибуты файла
                        nullptr);
#else // _WIN32
    hFile = fopen( c3d::C3DToPathstring(fileName).c_str(), "wt" );
#endif // _WIN32

    SIZE_WRITE numOfBytes;// Количество реально записанных байт
    uint sizeEOL = 4; // Длина записи конца строки //-V112

    // Вывод общих данных
    TCHAR buf[128];
    // Запись в файл, имя файла
    _tcscpy( buf, TestVariables::nameBuffer );
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile(hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки
    // Запись в файл, действие
    _tcscpy(buf,((TCHAR**)ptext)[0]);
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile( hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки
    // Запись в файл, число тел
    _tcscpy(buf,((TCHAR**)ptext)[1]);
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile( hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки
    // Запись в файл, общее число граней
    _tcscpy(buf,((TCHAR**)ptext)[2]);
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile( hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки
    // Запись в файл, число точек
    _tcscpy(buf,((TCHAR**)ptext)[3]);
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile( hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки
    // Запись в файл, число треугольников
    _tcscpy(buf,((TCHAR**)ptext)[4]);   //-V112
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile( hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки
    // Запись в файл, число четырехугольников
    _tcscpy(buf,((TCHAR**)ptext)[5]);
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile( hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки
    // Запись в файл, общее время выполнения
    _tcscpy(buf,((TCHAR**)ptext)[6]);
    WriteFile(hFile, buf, (SIZE_WRITE)LoUint32(_tcslen(buf)), &numOfBytes, nullptr);
    WriteFile( hFile, _T("\r\n\r\n"), sizeEOL, &numOfBytes, nullptr ); // Запись конца строки

    // Закрываем файл
    CloseHandle(hFile);
  }
}

#ifndef C3D_WINDOWS // _MSC_VER
  #undef SIZE_WRITE
  #undef HANDLE
  #undef CloseHandle
#else // _WIN32
  #undef SIZE_WRITE
#endif // _WIN32

#endif // C3D_DEBUG


//-----------------------------------------------------------------------------------------
// Выдать информацию о времени и размере триангуляции
// ---
void ShowTimeTestResult( TimeTest * testResult,
                         size_t solidsCount,
                         size_t facesCount,
                         size_t pointsCount,
                         size_t trianglesCount,
                         size_t quadranglesCount,
                         uint32 miliSec,
                         const TCHAR * sss )
{
  TCHAR info[9999];
  _tcscpy( info, _T("\r") );

  TCHAR buf0[128];
  TCHAR buf1[128];
  TCHAR buf2[128];
  TCHAR buf3[128];
  TCHAR buf4[128];
  TCHAR buf5[128];
  TCHAR buf6[128];

  SecondsToTimeString( miliSec, buf0 );

  _sntprintf( buf6, 128, RT_BODIES_COUNT      F_TD _T(" "), solidsCount      );  //-V111
  _sntprintf( buf1, 128, RT_TOTAL_FACES_COUNT F_TD _T(" "), facesCount       );  //-V111
  _sntprintf( buf2, 128, RT_POINTS_COUNT      F_TD _T(" "), pointsCount      );  //-V111
  _sntprintf( buf3, 128, RT_TRIANGLES_COUNT   F_TD _T(" "), trianglesCount   );  //-V111
  _sntprintf( buf4, 128, RT_QUADRANGLES_COUNT F_TD _T(" "), quadranglesCount );  //-V111

  _sntprintf( buf5, 128, RT_TEST_TIME_MSEC, (TCHAR*)buf0, miliSec );

  _tcscat( info, buf6 );
  _tcscat( info, _T("\n\r") );
  _tcscat( info, buf1 );
  _tcscat( info, _T("\n\r") );
  _tcscat( info, buf2 );
  _tcscat( info, _T("\n\r") );
  _tcscat( info, buf3 );
  _tcscat( info, _T("\n\r") );
  _tcscat( info, buf4 );
  _tcscat( info, _T("\n\r") );
  _tcscat( info, buf5 );

  MessageBoxEx( info, sss );

#ifdef C3D_DEBUG
  const void * params[8] = {sss, buf6, buf1, buf2, buf3, buf4, buf5, testResult };
  ::WriteDocToFile( params );
#else
  testResult = nullptr;
#endif
}


//----------------------------------------------------------------------------------------
// Выдаст сообщение о причине неудачи построения solid'а
//---
void PutErrorMessage( int solidError, const TCHAR * name  ) {
  MessageBoxEx( ::GetSolidErrorResId( solidError ), name );
}


//----------------------------------------------------------------------------------------
// Выдаст сообщение о причине неудачи построения solid'а
//---
void StitchErrorMessage( int stitchError, const TCHAR * name  ) {
  MessageBoxEx( ::GetStitchErrorResId( stitchError ), name );
}


//----------------------------------------------------------------------------------------
// Провести неоконные действия при инициализации главного окна задачи
// ---
void SystemInitFrame()
{
//OV_Lnx   _tcscpy(fontName,_T("Courier New"));
  _tcscpy( TestVariables::nameBuffer,_T(""));
  TestVariables::viewManager = new Manager();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// Неклассные функции конкретного окна.
//
///////////////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------------------
// Создать новое окно и модель.
// ---
bool CreateViewWindow( uint command )
{
  wProjections viewNumber = v_Isometry;  // Номер проекции
  const TCHAR * title = nullptr;
  switch (command)
  {
    case CM_VIEWFRONT : // Спереди
      title = IDS_TITLE_VIEWFRONT;
      viewNumber = v_Front;
      break;

    case CM_VIEWREAR  : // Сзади
      title = IDS_TITLE_VIEWREAR;
      viewNumber = v_Rear;
      break;

    case CM_VIEWUP    : // Сверху
      title = IDS_TITLE_VIEWUP;
      viewNumber = v_Up;
      break;

    case CM_VIEWDOWN  : // Снизу
      title = IDS_TITLE_VIEWDOWN;
      viewNumber = v_Down;
      break;

    case CM_VIEWLEFT  : // Слева
      title = IDS_TITLE_VIEWLEFT;
      viewNumber = v_Left;
      break;

    case CM_VIEWRIGHT : // Справа
      title = IDS_TITLE_VIEWRIGHT;
      viewNumber = v_Right;
      break;

    case CM_VIEWISO   : // Изометрия
      title = IDS_TITLE_VIEWISOMETRY;
      viewNumber = v_Isometry;
      break;
  }

  if ( TestVariables::viewManager )
  {
    WorkWindow * wind = new WorkWindow( viewNumber );
    wind->CreateWorkWindow( title, TestVariables::viewManager->GetBackgrColor(), TestVariables::viewManager->GetLightDirection()/*lightDirection*/ );
    wind->OrientateWindow( viewNumber );

    ptrdiff_t newIndex = TestVariables::viewManager->AddWindow( wind );
    TestVariables::viewManager->SetFocus( newIndex, false );
  }

  return true;
}


//----------------------------------------------------------------------------------------
// Расположить по вектору взгляда.
// ---
void SetViewVector()
{
  if ( TestVariables::viewManager != nullptr )
  {
    size_t wInd = TestVariables::viewManager->GetActiveWindowIndex();
    WorkWindow * window = TestVariables::viewManager->GetWindow( wInd ) ;

    if ( window != nullptr ) {
      // Запросить вектор взгляда.
      MbCartPoint3D p;
      if( GetExactPoint(p) ) {
        if ( p.MaxFactor() > METRIC_PRECISION ) {
          MbVector3D eye( p );
          eye.Invert();
          const MbCartPoint3D & origin = window->GetOrigin();
          MbPlacement3D place( origin, eye );
          window->OrientateWindow( place );
        }
      }
    }
  }
}


//----------------------------------------------------------------------------------------
// Установить ориентацию окна
// ---
void SetViewWindow( wProjections proj )
{
  if ( TestVariables::viewManager != nullptr )
  {
    size_t wInd = TestVariables::viewManager->GetActiveWindowIndex();
    WorkWindow * window = TestVariables::viewManager->GetWindow( wInd ) ;

    if ( window != nullptr ) {
      window->OrientateWindow( proj );
      window->SetWindowTitle();
    }
  }
}


//----------------------------------------------------------------------------------------
// Select a mesh
// ---
MbMesh * SelectMesh( MbMatrix3D * from )
{
  MbMesh * mesh = nullptr;
  {
    MbItem * obj = nullptr;
    PathMatrix pathFrom;
    if ( GetGObj( IDS_SELECT_MESH, st_Mesh, &obj, pathFrom ) && (obj->IsA() == st_Mesh) ) {
      mesh = static_cast<MbMesh *>(obj);
      if ( from != nullptr )
        from->Init( pathFrom.second );
    }
  }
  return mesh;
}


//----------------------------------------------------------------------------------------
// Select a solid
// ---
MbSolid * SelectSolid( MbMatrix3D * from )
{
  MbSolid * solid = nullptr;
  {
    MbItem * obj = nullptr;
    PathMatrix pathFrom;
    if ( GetGObj( IDS_SELECT_SOLID, st_Solid, &obj, pathFrom ) && (obj->IsA() == st_Solid) ) {
      solid = static_cast<MbSolid *>( obj );
      if ( from != nullptr )
        from->Init( pathFrom.second );
    }
  }
  return solid;
}


//----------------------------------------------------------------------------------------
// Select a face
// ---
MbFace * SelectFace( MbMatrix3D * from )
{
  MbFace * face = nullptr;
  {
    MbItem * obj = nullptr;
    MbTopItem * topItem = nullptr;

    MbMatrix3D matrFrom;
    if ( GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj, &topItem, matrFrom ) ) {
      if ( topItem->IsAFace() ) {
        face = static_cast<MbFace *>(topItem);
        if ( from != nullptr )
          from->Init( matrFrom );
      }
    }
  }
  return face;
}


//----------------------------------------------------------------------------------------
// Select a surface
// ---
MbSurface * SelectSurface( MbMatrix3D * from )
{
  MbSurface * surface = nullptr;
  {
    MbItem * obj = nullptr;
    PathMatrix pathFrom;
    if ( GetGObj( IDS_SELECT_SURFACE, st_Surface, &obj, pathFrom ) ) {
      MbSpaceInstance * spInst = static_cast<MbSpaceInstance *>(obj);
      if ( spInst->GetSpaceItem() != nullptr ) {
        MbSpaceItem * spItem = static_cast<MbSpaceItem *>(spInst->SetSpaceItem());
        if ( spItem->Family() == st_Surface ) {
          surface = static_cast<MbSurface *>(spItem);
          if ( from != nullptr ) {
            from->Init( pathFrom.second );
          }
        }
      }
    }
  }
  return surface;
}


//----------------------------------------------------------------------------------------
// Select a curve copy
// ---
SPtr<MbCurve3D> SelectAnyCurveCopy( PathMatrix & pathFrom, std::vector< SPtr<MbCurve3D> > * otherCurvesPtr, MbItem ** curveCarrier )
{
  SPtr<MbCurve3D> curve;

  MbItem * item = nullptr;

  if ( GetAnyCurve( IDS_SELECT_CURVE, &item, pathFrom ) ) {
    if ( ::GetSpaceCurve( *item, true, curve, otherCurvesPtr ) && (curve != nullptr) ) {
      if ( curveCarrier != nullptr )
        *curveCarrier = item;

      MbRegDuplicate * iReg = nullptr;
      MbAutoRegDuplicate autoReg( iReg );

      curve = ::DuplicateIfUsed( curve, iReg );
      if ( otherCurvesPtr != nullptr ) {
        std::vector< SPtr<MbCurve3D> > & otherCurves = *otherCurvesPtr;
        std::vector< SPtr<MbCurve3D> >::iterator it = otherCurves.begin();
        while ( it != otherCurves.end() ) {
          *it = ::DuplicateIfUsed( *it, iReg );
          ++it;
        }
      }
    }
  }

  return curve;
}


//----------------------------------------------------------------------------------------
// Select a point
// ---
bool SelectPoint( MbCartPoint3D & pnt )
{
  if ( GetPoint( IDS_SELECT_POINT, pnt ) ) {
    return true;
  }
  return false;
}


//------------------------------------------------------------------------------
// Выбор образующей для операции.
// ---
MbSweptData * SelectSweptData( bool bWireFrame )
{
  MbSweptData * resData = nullptr;

  if ( bWireFrame ) {
    MbItem * wireFrameItem = nullptr;

    if ( GetGObj( IDS_SELECT_WIREFRAME, st_SpaceItem, &wireFrameItem) && (wireFrameItem != nullptr) ) {
      if ( wireFrameItem->IsA() == st_WireFrame ) {
        MbWireFrame * wireFrame = static_cast<MbWireFrame *>( wireFrameItem );
        resData = new MbSweptData( *wireFrame );
      }
    }
  }
  else {
    MbItem * curCurveObj(0); // Образующая. Generatrix.
    MbItem * prevCurveObj(0);

    SpaceContoursSPtrVector contours3d;
    MbPlacement3D contoursPlace;
    SurfaceSPtr surface;
    PlaneContoursSPtrVector contours;

    while ( GetAnyCurve ( IDS_SELECT_GENER_CURVE, &curCurveObj ) ) {

      if ( curCurveObj != prevCurveObj ) {
        MbPlacement3D cntrPlace;
        MbContour * contour = ::GetContourOnPlace( curCurveObj, cntrPlace );
        if ( contour != nullptr ) {
          contours.push_back( PlaneContourSPtr( contour ) );
          if ( prevCurveObj != nullptr ) {
            TestVariables::viewManager->ToggleObject( prevCurveObj );
            if ( !contoursPlace.Colinear( cntrPlace ) )
              break;
            MbMatrix mtr;
            if ( cntrPlace.GetMatrixToPlace( contoursPlace, mtr ) && !mtr.IsSingle() )
              contour->Transform( mtr );
          }
          else {
            contoursPlace.Init( cntrPlace );
            surface = new MbPlane( contoursPlace );
          }
          TestVariables::viewManager->ToggleObject( curCurveObj );
          prevCurveObj = curCurveObj;
        }
      }

      if ( surface == nullptr && GetCurve3D( curCurveObj, true )  ) {
        SPtr<MbCurve3D> crv( ::GetCurve3D( curCurveObj, false ) );
        if ( crv != nullptr ) {
          MbContour3D * contour = nullptr;
          if ( crv->Type() == st_Contour3D )
            contour = &static_cast<MbContour3D &>(*crv);
          else {
            RPArray<MbCurve3D> curves;
            curves.Add( crv );
            contour = new MbContour3D( curves, false/*same*/, Math::DefaultMathVersion() );
          }
          contours3d.push_back( SpaceContourSPtr( contour ) );
          if ( prevCurveObj ) 
            TestVariables::viewManager->ToggleObject(prevCurveObj);
          TestVariables::viewManager->ToggleObject(curCurveObj);
          prevCurveObj = curCurveObj;
        }
      }
    }

    if ( prevCurveObj )
      TestVariables::viewManager->ToggleObject( prevCurveObj );

    if ( contours3d.size() > 0 )
      resData = new MbSweptData( contours3d );
    else if ( surface != nullptr && contours.size() > 0 ) 
      resData = new MbSweptData( *surface, contours );
  }

  return resData;
}


//----------------------------------------------------------------------------------------
// Чтение объектов модели из файла по фильтру
// ---
void ReadModelTree ( TCHAR * fileName, EObjectFilter filter )
{
  WorkWindow * wind = TestVariables::viewManager->GetActiveWindow();

  if ( fileName == nullptr ) {
#ifndef __USE_QT__
      if ( _GetOpenFileName ( TestVariables::nameBuffer, SELECT_FILE_INFO, OPEN_EXT_SUPPORTED_INFO, NON_EXT ) )
        fileName = TestVariables::nameBuffer;
#else
      if ( _GetOpenFileName ( TestVariables::nameBuffer, SELECT_FILE_INFO, nullptr, nullptr ) )
        fileName = TestVariables::nameBuffer;
#endif
  }

  if ( wind == nullptr ) {
    wind = new WorkWindow ( v_Isometry );
    wind->CreateWorkWindow ( IDS_TITLE_VIEWISOMETRY, TestVariables::viewManager->GetBackgrColor(), TestVariables::viewManager->GetLightDirection()/*lightDirection*/ );
    wind->OrientateWindow ( v_Isometry );

    ptrdiff_t newIndex = TestVariables::viewManager->AddWindow ( wind );
    TestVariables::viewManager->SetFocus( newIndex, false );
  }

  if ( fileName != nullptr ) {
    const size_t bufLen = 1024;
    TCHAR buf[bufLen];
    size_t len = 0;
    if ( TestVariables::checkReadTime )
      len += (size_t)_sntprintf ( buf+len, bufLen-len, RT_TEST_RESULTS );

    MbAccurateTimer ac;

    if ( ::SameExtension( fileName, C3D_EXT ) ) { // C3D
      reader_ex* pl = reader_ex::CreateReaderEx( std::unique_ptr<iobuf_Seq>(&::createiobuf(fileName)), io::in ).release();

      if ( pl->good() ) {
        SetWaitCursor ( true );

        ac.Begin();

        pl->ReadObjectCatalog();

        ac.End();

        SetWaitCursor ( false );

        if ( pl->good() && pl->GetModelTree() ) {
          switch (filter) {
            case of_selection: {
              TestVariables::viewManager->CreateTree ( pl ); // Now this dialog owns reader
            } break;
            case of_type: {
              TestVariables::viewManager->CreateTypeFilter ( pl ); // Now this dialog owns reader
            } break;
            case of_gabarit: {
              TestVariables::viewManager->CreateGabaritFilter ( pl ); // Now this dialog owns reader
            } break;
            case of_attribute: {
              TestVariables::viewManager->CreateAttributeFilter ( pl ); // Now this dialog owns reader
            } break;
            case of_embodiment: {
              TestVariables::viewManager->CreateEmbodimentTree( pl ); // Now this dialog owns reader
            } break;
          }
        }
      }
    }
  }
}

#if defined(C3D_WINDOWS)
  //----------------------------------------------------------------------------------------
  // Обработчик консоли.
  // Console event handler.
  // ---
  BOOL CtrlHandler( DWORD ctrlType )
  {
    switch( ctrlType )
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      FreeConsole();
      return TRUE;
    case CTRL_CLOSE_EVENT:
      return TRUE;

    default:
      return FALSE;
    }
  }

  //----------------------------------------------------------------------------------------
  // Запустить консоль.
  // Run the console.
  // ---
  void RunConsole()
  {
    BOOL res = AttachConsole( ATTACH_PARENT_PROCESS );
    if ( !res )
    {
      res = AllocConsole();
      SetConsoleCtrlHandler( (PHANDLER_ROUTINE)CtrlHandler, TRUE );

      freopen("CONIN$", "r", stdin);
      freopen("CONOUT$", "w", stdout);
      freopen("CONOUT$", "w", stderr);

      std::cin.sync_with_stdio();
      std::cout.sync_with_stdio();
      std::cerr.sync_with_stdio();
      std::clog.sync_with_stdio();
    }
  }
#else  // C3D_WINDOWS
  //----------------------------------------------------------------------------------------
  // Запустить консоль.
  // Run the console.
  // ---
  void RunConsole()
  {}
#endif  // C3D_WINDOWS
