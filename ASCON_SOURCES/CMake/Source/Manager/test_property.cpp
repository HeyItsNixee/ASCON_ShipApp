#include <test_property.h>
#include <test_manager.h>
#include <test_service.h>
#include <test.h>
#include <test_property_title.h>
#include <mb_property.h>
#include <mb_placement.h>
#include <mb_placement3d.h>
#include <mb_point_mating.h>
#include <function.h>
#include <curve.h>
#include <curve3d.h>
#include <surface.h>
#include <solid.h>
#include <instance_item.h>
#include <assembly.h>
#include <assisting_item.h>
#include <collection.h>
#include <mesh.h>
#include <point_frame.h>
#include <wire_frame.h>
#include <point3d.h>
#include <marker.h>
#include <mb_symbol.h>
#include <mb_thread.h>
#include <plane_instance.h>
#include <space_instance.h>
#include <attribute_item.h>
#include <cr_attribute_provider.h>
#include <region.h>
#include <generic_utility.h>
#include <math_namespace.h>

#include <last.h>

using namespace c3d;


#define PROMPT_LENGTH 82 ///< \ru Длина подсказки \en Length of hint

#ifdef C3D_WINDOWS
  #define PROPERTY_WINDOW_TITLE_FORMAT _T("%s")
  #define PROPERTY_NAME_SPACE_FORMAT _T("%s \t%s")
  #define PROPERTY_NAME_FORMAT _T("%s \t%s")
#else
  #ifdef _UNICODE
    #define PROPERTY_WINDOW_TITLE_FORMAT _T("%S")
    #define PROPERTY_NAME_SPACE_FORMAT _T("%S \t%S")
    #define PROPERTY_NAME_FORMAT _T("%S \t%S")
  #else
    #define PROPERTY_WINDOW_TITLE_FORMAT _T("%s")
    #define PROPERTY_NAME_SPACE_FORMAT _T("%s \t%s")
    #define PROPERTY_NAME_FORMAT _T("%s \t%s")
  #endif
#endif

//----------------------------------------------------------------------------------------
// Конструктор
// ---
PropertyWindow::PropertyWindow( MbProperty & rootProp, PropertyWindow * initParent ) 
  : MbRefItem()
  , propTool( nullptr )
  , parent( initParent )
  , parentProp( &rootProp )
  , child( nullptr )
  , obj( nullptr )
  , type( rootProp.IsA() )
  , properties()
  , changeable( rootProp.IsChangeable() && (initParent == nullptr || initParent->IsChangeable()) )
  , changed( false )
{
  rootProp.GET_PROPERTY_VALUE( obj );

  PRECONDITION( initParent != nullptr );
  if ( parent != nullptr ) // Блокируем родительское окно
  { 
    parent->child = this;
    parent->AddRef();
  }

  if ( obj != nullptr )
  {
    switch ( type ) 
    {
      case pt_ModelProp: // Отдельно рассмотрен случай
        _GetProperties( (MbModel*)obj, properties );
        break;

      case pt_TransactionsProp: // Отдельно рассмотрен случай
        _GetProperties( static_cast<MbTransactions*>(obj), properties ); // -->MbTransactions
        break;

      default: {
        // свойства грани не редактируемые, но атрибуты при этом можно редактировать
        if ( rootProp.IsChangeable() )
          changeable = true;
        rootProp.GetProperties( properties );
        break;
      }
    }
  }
  else {
    PRECONDITION( false ); // Нет объекта
  }

  CreatePropertyWindow(); // Собственно создание оконной части
}


//------------------------------------------------------------------------------
// Деструктор
// ---
PropertyWindow::~PropertyWindow()
{
  if ( parent != nullptr ) { // Снимаем флаг блокировки родительского окна
    if ( this == parent->child )
      parent->child = nullptr;
    parent->Release();
  }

  if ( changed )
    SetOwnProperties();

  delete propTool;
}


//------------------------------------------------------------------------------
// Установить признак измененности
// ---
void PropertyWindow::SetChanged()
{
  if ( parent != nullptr && type != pt_AttributeContainerProp )
    parent->SetChanged();
  changed = true;
}


//------------------------------------------------------------------------------
// Записать новые свойства в объект
// ---
void PropertyWindow::SetOwnProperties()
{
  if ( changeable && obj != nullptr )
  {
    if ( child != nullptr )
      child->SetOwnProperties();

    // MA 2015.06 (!) Есть возможность устранить этот switch. Использовать виртуальность MbProperty.
    switch ( type )
    {
      // Комплексные свойства двумерных объектов 
      case pt_CartPointProp :   // Точка-свойство
        ((MbCartPoint *)obj)->SetProperties( properties );
        break;
      case pt_VectorProp :      // Вектор-свойство
        ((MbVector *)obj)->SetProperties( properties );
        break;
      case pt_DirectionProp :   // Вектор-свойство
        ((MbDirection *)obj)->SetProperties( properties );
        break;
      case pt_PlacementProp :   // Система-свойство
        ((MbPlacement *)obj)->SetProperties( properties );
        break;
      case pt_MatrixProp :      // Матрица-свойство
        ((MbMatrix *)obj)->SetProperties( properties );
        break;
      case pt_CurveProp :  // Свойство плоской кривой
        static_cast<MbCurve *>(obj)->SetProperties( properties ); 
        break;

      // Комплексные свойства трёхмерных объектов 
      case pt_CartPoint3DProp :     // Точка-свойство
        ((MbCartPoint3D *)obj)->SetProperties( properties );
        break;
      case pt_Vector3DProp :    // Вектор-свойство
        ((MbVector3D *)obj)->SetProperties( properties );
        break;
      case pt_Placement3DProp : // Система-свойство
        ((MbPlacement3D *)obj)->SetProperties( properties );
        break;
      case pt_Matrix3DProp :    // Матрица-свойство
        ((MbMatrix3D *)obj)->SetProperties( properties );
        break;
      case pt_FloatPointProp : // Параметр-свойство
        ((MbFloatPoint *)obj)->SetProperties( properties );
        break;
      case pt_FloatPoint3DProp : // Вершина-свойство
        ((MbFloatPoint3D *)obj)->SetProperties( properties );
        break;
      case pt_FloatVector3DProp : // Нормаль-свойство
        ((MbFloatVector3D *)obj)->SetProperties( properties );
        break;
      case pt_TriangleProp : // Треугольник-свойство
        ((MbTriangle *)obj)->SetProperties( properties );
        break;
      case pt_QuadrangleProp : // Четырехугольник-свойство
        ((MbQuadrangle *)obj)->SetProperties( properties );
        break;
      case pt_ElementProp : // Элемент-свойство
        ((MbElement *)obj)->SetProperties( properties );
        break;
      case pt_GridProp : // Триангуляция-свойство
        ((MbGrid *)obj)->SetProperties( properties );
        break;

      // Комплексные свойства геометрических объектов 
      case pt_FunctionProp :   // Свойство функции
        ((MbFunction *)obj)->SetProperties( properties );
        break;
      case pt_Curve3DProp:       // Свойство кривой
        ((MbCurve3D *)obj)->SetProperties( properties );
        break;
      case pt_SurfaceProp :    // Свойство поверхности
        ((MbSurface *)obj)->SetProperties( properties );
        break;
      case pt_Point3DProp:        // Свойство точки
        ((MbPoint3D *)obj)->SetProperties( properties );
        break;
      case pt_MarkerProp:          // Cвойство маркера ("точка присоединения").
        ((MbMarker *)obj)->SetProperties( properties );
        break;
      case pt_SymbolProp:          // Cвойство условного обозначения.
        ((MbSymbol *)obj)->SetProperties( properties );
        break;
      case pt_ThreadProp:          // Cвойство резьбы.
        ((MbThread *)obj)->SetProperties( properties );
        break;

      // Комплексные свойства топологических объектов 
      case pt_VertexProp:      // Свойство вершины
        ((MbVertex *)obj)->SetProperties( properties );
        break;
      case pt_CurveEdgeProp:    // Свойство ребра
        ((MbCurveEdge *)obj)->SetProperties( properties );
        break;
      case pt_EdgeProp:        // Свойство ребра
        ((MbEdge *)obj)->SetProperties( properties );
        break;
      case pt_OrientedEdgeProp: // Свойство ребра
        ((MbOrientedEdge *)obj)->SetProperties( properties );
        break;
      case pt_LoopProp:         // Свойство цикла
        ((MbLoop *)obj)->SetProperties( properties );
        break;
      case pt_FaceProp:         // Свойство грани
        ((MbFace *)obj)->SetProperties( properties );
        break;
      case pt_FaceShellProp:    // Обоболочк-свойство
        ((MbFaceShell *)obj)->SetProperties( properties );
        break;

      // Комплексные свойства строителей объектов 
      case pt_CreatorProp:        // Свойство оболочки
        ((MbCreator *)obj)->SetProperties( properties );
        break;
      case pt_NameProp:         // Свойство имени
        ((MbName *)obj)->SetProperties( properties );
        break;

      // Комплексные свойства объектов геометрической модели 
      case pt_ItemProp :  // Свойство объекта
        if ( changed )
          ((MbItem *)obj)->SetProperties( properties );
        break;
      case pt_SolidProp:        // Свойство тела
        if ( changed )
          ((MbSolid *)obj)->SetProperties( properties );
        break;
      case pt_InstanceProp:        // Свойство вставки
        if ( changed )
          ((MbInstance *)obj)->SetProperties( properties );
        break;
      case pt_AssemblyProp:        // Свойство тела
        if ( changed )
          ((MbAssembly *)obj)->SetProperties( properties );
        break;
      case pt_WireFrameProp:    // Свойство каркаса
        if ( changed )
          ((MbWireFrame *)obj)->SetProperties( properties );
        break;
      case pt_CollectionProp:   // Свойство тела
        if ( changed )
          ((MbCollection *)obj)->SetProperties( properties );
        break;
      case pt_MeshProp:        // Свойство полигональной модели
        if ( changed )
          ((MbMesh *)obj)->SetProperties( properties );
        break;

      // Комплексные свойства объектов 
      case pt_ModelProp:
        ((MbModel*)obj)->SetProperties( properties );
        break;
      case pt_TransactionsProp:
        ((MbTransactions*)obj)->SetProperties( properties );
        break;
      case pt_AttributeContainerProp:
        ((MbAttributeContainer*)obj)->SetProperties( properties );
        break;
      case pt_AttributeProp:
        ((MbAttribute*)obj)->SetProperties( properties );
        break;
      case pt_NamedAttributeContainerProp:
        ((MbNamedAttributeContainer*)obj)->SetProperties( properties );
        break;
      case pt_AttributeActionProp:
        ((MbAttributeAction*)obj)->SetProperties( properties );
        break;

      // Комплексные свойства геометрических ограничений
      case pt_PntMatingProp :
        ((PntMatingData2D *)obj)->SetProperties( properties );
        break;
      case pt_Pnt3DMatingProp :
        ((PntMatingData3D *)obj)->SetProperties( properties );
        break;

      default :
        if ( parentProp )
        {
          parentProp->SetProperties( properties );
          break;
        }       
        C3D_ASSERT_UNCONDITIONAL( "The parent property is unknown." );
        break;
    }

    if ( parent != nullptr && type != pt_AttributeContainerProp )
      parent->SetChanged();

    changed = false;
  }
}


//------------------------------------------------------------------------------
// Редактировать свойство
// ---
PropertyWindow * PropertyWindow::CreateProperty( size_t index )
{
  MbProperty * indexProp = properties[index];
  PRECONDITION( indexProp != nullptr );
  return new PropertyWindow( *indexProp, this );
}


//------------------------------------------------------------------------------
// Создать окно свойств
// ---
void PropertyWindow::CreatePropertyWindow()
{
  const int npcMax = 17; // Максимальное количество строк в окне свойств
  TCHAR title[STRINGLENGTH];
  int npc = min_of( (int)(GetProperties().Count() + 2), npcMax );

  TCHAR name[PROMPT_LENGTH]; 
  ::GetPromptValue( const_cast<MbProperties &>(GetProperties()).SetName(), name ); // Получить строку по её значению в перечислении.
  _sntprintf( title, STRINGLENGTH, PROPERTY_WINDOW_TITLE_FORMAT, name ); // Имя объекта

#ifndef __USE_QT__ 
  HWND hprop = 0;
  HWND hlist = 0;
  HWND hedit = 0;
  if ( CreatePropertyClient( hprop, hlist, hedit, title, npc ) )
    propTool = CreatePropertyTool( hprop, hlist, hedit );
#else // __USE_QT__ 
  QWidget * hprop = nullptr;
  if ( CreatePropertyClient( hprop, title, npc ) )
    propTool = CreatePropertyTool( hprop );
#endif // __USE_QT__

  TCHAR s[STRINGLENGTH];
  // Окно для изменения численного значения
  if ( GetPropCount() > 0 ) 
  {
    Properties()[0]->GetCharValue( s );
    GetPropTool().SetEditText( s );
  }

  // Окно с перечнем свойств
  PArray<string_t> a;
  a.Reserve( GetPropCount() ); // C3D-289
  for ( size_t i = 0; i < GetPropCount(); i++ ) {
    TCHAR ss[STRINGLENGTH];
    TCHAR prompt[PROMPT_LENGTH]; 
    ::GetPromptValue( Properties()[i]->SetPrompt(), (TCHAR*)prompt ); // Получить строку по её значению в перечислении.
    Properties()[i]->GetCharValue( ss );
    _sntprintf( s, STRINGLENGTH, PROPERTY_NAME_SPACE_FORMAT, (TCHAR*)prompt, (TCHAR*)ss );
    a.Add( new string_t(s) );
  }

  GetPropTool().SetListTexts( a );
  GetPropTool().SetCurSelList( 0 );
  GetPropTool().Layout();
  GetPropTool().SetFocusList();
}


//------------------------------------------------------------------------------
// Записать новое свойство по индексу в списке
// ---
void PropertyWindow::SetString( TCHAR * s )
{
  TCHAR * p = _tcsstr( s, _T("\r\n") );
  if ( !p )
    p = _tcsstr( s, _T("\n\r") );
  if ( p )
    *p = 0;

  size_t currpos = GetPropTool().GetCurSelList();

  MbProperties & mp = Properties();
  mp[currpos]->SetPropertyValue( s );
  mp[currpos]->GetCharValue( s );

  TCHAR ss[STRINGLENGTH];
  TCHAR prompt[PROMPT_LENGTH]; 
  ::GetPromptValue( (GetProperties())[currpos]->SetPrompt(), (TCHAR*)prompt ); 
  _sntprintf( ss, STRINGLENGTH, PROPERTY_NAME_FORMAT, (TCHAR*)prompt, (TCHAR*)s );

  GetPropTool().SetEditText( s );
  GetPropTool().SetListText( currpos, ss );  
  GetPropTool().SetCurSelList( currpos );
  GetPropTool().SetFocusList();
}


//------------------------------------------------------------------------------
// Записать новое свойство по индексу в списке
// ---
void PropertyWindow::SetBoolean( bool & b )
{
  TCHAR s[STRINGLENGTH];

  size_t currpos = GetPropTool().GetCurSelList();

  MbProperties & mp = Properties();
  TCHAR bc = b;
  mp[currpos]->SetPropertyValue( &bc );
  mp[currpos]->GetCharValue( s );

  TCHAR ss[STRINGLENGTH];
  TCHAR prompt[PROMPT_LENGTH]; 
  ::GetPromptValue( (GetProperties())[currpos]->SetPrompt(), (TCHAR*)prompt ); 
  _sntprintf( ss, STRINGLENGTH, PROPERTY_NAME_FORMAT, (TCHAR*)prompt, (TCHAR*)s );

  GetPropTool().SetEditText( s );
  GetPropTool().SetListText( currpos, ss );  
  GetPropTool().SetCurSelList( currpos );
  GetPropTool().SetFocusList();
}


//------------------------------------------------------------------------------
// Записать новые свойства в объект
// ---
void PropertyWindow::SetProperties()
{
  TCHAR s[STRINGLENGTH];

  SetWaitCursor( true );
  TestVariables::viewManager->SetProperty( this );
  SetWaitCursor( false );

  // Переустановить свойства родителя
  if ( (parent != nullptr) && TestVariables::viewManager->IsMyProperty( parent ) )
  {
    size_t currpos = parent->GetPropTool().GetCurSelList();

    PArray<string_t> a;
    for ( size_t i = 0, icnt = parent->GetPropCount(); i < icnt; i++ ) {
      TCHAR ss[STRINGLENGTH];
      TCHAR prompt[PROMPT_LENGTH]; 
      ::GetPromptValue( parent->Properties()[i]->SetPrompt(), (TCHAR*)prompt ); 
      parent->Properties()[i]->GetCharValue( ss );
      _sntprintf( s, STRINGLENGTH, PROPERTY_NAME_FORMAT, (TCHAR*)prompt, (TCHAR*)ss );
      a.Add( new string_t(s) );
    }
    parent->GetPropTool().SetListTexts( a );
    parent->GetPropTool().SetCurSelList( currpos );
  }
}


//------------------------------------------------------------------------------
// Сменить текст в редакторе при изменении позиции в листе
// ---
void PropertyWindow::ChangeProperty()
{
  size_t currpos = GetPropTool().GetCurSelList();
  if ( currpos < Properties().Count() ) {
    MbProperty * prop = Properties()[currpos];
    TCHAR s[STRINGLENGTH];
    prop->GetCharValue( s );
    GetPropTool().SetEditText( s );
  }
}


//------------------------------------------------------------------------------
// Редактировать свойство
// ---
void PropertyWindow::EditProperty()
{
  size_t currpos = GetPropTool().GetCurSelList();
  if ( currpos < Properties().Count() )
  {
    switch ( Properties()[currpos]->IsA() ) 
    {
      case pt_BoolProp: // Логическое значение
      {
        if ( IsChangeable() ) 
        {
          bool value = false;
          Properties()[currpos]->GET_PROPERTY_VALUE( value );
          value = !value;
          SetBoolean( value );
        }
      } break;
      case pt_IntProp:      // Целое значение
      case pt_UIntProp:     // Целое значение
      case pt_DoubleProp:   // Действительное значение
      case pt_NDoubleProp:  // Действительное значение
      case pt_CharProp:     // Строковое значение
      case pt_StringProp:   // Строковое значение
      case pt_VersionProp:  // Версия
      {
        if ( IsChangeable() ) 
        {
          GetPropTool().SelectEditText();
          GetPropTool().SetFocusEdit();
        }
      } break;

      default:
      {        
        if ( PropertyWindow * np = CreateProperty(currpos) )
          TestVariables::viewManager->AddProperty( np );
      }
      break;
    }
  }
}

//------------------------------------------------------------------------------
//
// ---
bool PropertyWindow::PropertyCommand( uint command, uint subCommand, bool isReturn )
{
  switch ( command ) {
    case BUTTON_SET :       // Установка свойств в объект
    case BUTTON_SET_EXIT :  // Установка свойств в объект и выход
    { 
      SetProperties(); // Записать новые свойства в объект
      if ( command == BUTTON_SET_EXIT ) 
      {      
        GetPropTool().ClosePropertyWindow();
      }
    } break;

    case BUTTON_EXIT : // Выход без записи
    {
      GetPropTool().ClosePropertyWindow();
    } break;

    case H_LIST : {
      switch ( subCommand ) {
        case LBN_SELCHANGE : {
          ChangeProperty();
        } break;

        case LBN_DBLCLK : {
          if ( !IsBlocked() )
            EditProperty(); // Редактировать свойство
        } break;
      }
    } break;

    case H_EDIT : {
      if ( subCommand == EN_UPDATE && isReturn ) {
        TCHAR s[STRINGLENGTH];
        _tcscpy( s, _T("") );
        GetPropTool().GetEditText( s, STRINGLENGTH );
        SetString( s ); // Записать новое свойство
      }
    } break;

    default : 
      return false; // Не обработано
  }

  return true;
}

//------------------------------------------------------------------------------
//
// ---
bool PropertyWindow::KeyboardCommand( uint command, bool shift, bool ctrl )
{
  switch ( command ) {
    case VK_RETURN : {
      if ( ctrl || shift ) {
        SetProperties(); // Записать новые свойства в объект
        GetPropTool().ClosePropertyWindow();
      }
      else {
        if ( !IsBlocked() )
          EditProperty(); // Редактировать свойство
      }
    } break;

    case VK_ESCAPE : { 
      GetPropTool().ClosePropertyWindow();
    } break;

    default :
      return false; // Не обработанно
  }

  return true;
}


//------------------------------------------------------------------------------
//
// ---
void PropertyWindow::PreDestroyCommand( bool redraw )
{
  if ( child != nullptr ) {
    child->GetPropTool().ClosePropertyWindow();
    child->PreDestroyCommand( false );
  }

  if ( (parent != nullptr) && TestVariables::viewManager->IsMyProperty( parent ) )
    parent->GetPropTool().SetFocusList();

  TestVariables::viewManager->RemoveProperty( this, redraw );
}
