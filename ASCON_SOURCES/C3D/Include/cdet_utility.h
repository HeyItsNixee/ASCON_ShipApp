//////////////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Утилита оценки столкновений и параметров близости тел.
         \en Utility of collision detection and proximity queries.
*/
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CDET_UTILITY_H
#define __CDET_UTILITY_H

#include <cdet_data.h>
#include <mt_ref_item.h>

class  MtRefItem;
class  MbItem;
class  MbSolid;
class  MbMesh;
class  MbAssembly;
struct MbLumpAndFaces;
class  MbCollisionDetector;

/**
  \addtogroup Collision_Detection   
  \{
*/

//----------------------------------------------------------------------------------------
/** \brief \ru Утилита расчета параметров пересечения и близости тел.
           \en Utility for calculation of intersection and proximity parameters of solids. \~
  \details \ru Предоставляет функциональность Collision Detection для взаимодействия 
               с приложением САПР.
           \en Provides facilities of The Collision Detection to interact with the CAD 
               application. \~
  \attention  \ru Для гарантированно правильной работы детектора необходимо, чтобы объект
    типа MbLumpAndFaces, добавляемый в рассмотрение посредством функции AddSolid, имел 
    правильную матрицу преобразования в мир в настоящем его положении, т.е. с самого начала.
              \en For the ensure proper functionality of the detector it is necessary that 
    an object of type MbLumpAndFaces to be added in consideration by function AddSolid will 
    have a correct matrix of transformation to the world coordinate system in its current 
    state, i.e. from the beginning. \~
*/
// ---
class MATH_CLASS MbCollisionDetectionUtility 
{
  MbCollisionDetector & detector;

public:
    MbCollisionDetectionUtility();
   ~MbCollisionDetectionUtility();

public:
  /**
    \brief \ru Добавить твердое тело с заданным положением в набор для контроля столкновений.
           \en Add a solid with given placement to the collision detection set. \~
    \return \ru Дескриптор объекта для контроля столкновений. \en Descriptor of object for collision detection. \~
  */  
  cdet_item AddItem( const MbSolid & solid, const MbPlacement3D & place, cdet_app_item appItem = CDET_APP_NULL );
  /**
    \brief \ru Добавить луч по вершине и направлению с заданным положением в набор для контроля столкновений.
           \en Add a ray with given vertex, direction and placement to the collision detection set. \~
    \return \ru Дескриптор объекта для контроля столкновений. \en Descriptor of object for collision detection. \~
  */
  cdet_item AddRay( const MbCartPoint3D & vertex, const MbVector3D & direction, const MbPlacement3D & place, cdet_app_item appItem = CDET_APP_NULL );
  /**
    \brief \ru Добавить полигональный объект с заданным положением в набор для контроля столкновений.
           \en Add a poligonal object with given placement to the collision detection set. \~
    \return \ru Дескриптор объекта для контроля столкновений. \en Descriptor of object for collision detection. \~
  */  
  cdet_item AddMesh( const MbMesh & mesh, const MbPlacement3D & place, cdet_app_item appItem = CDET_APP_NULL );
  /**
    \brief \ru Добавить новый компонент контроля соударений и параметров близости.
           \en Add a new component to track collisions and proximity parameters. \~
    \details \ru Компонент позволяет объединять тела в геометрически-жесткие множества.
             \en Component is able to unite solids into the rigid geometric sets.
  */
  cdet_item AddComponent( cdet_app_item );
  /**
    \brief \ru Добавить новый экземпляр тела в компонент контроля соударений.
           \en Add a new instance of a reused solid into the component. \~
    \param[in] compItem - \ru Компонент, которому будет принадлежать экземпляр.
                          \en A component to witch the instance will belong.
    \param[in] solidItem - \ru Оригинальное тело, добавленное методом #AddSolid, по которому изготавливается экземпляр.
                           \en An original solid added by the method #AddSolid by witch the instance is made.
    \param[in] place - \ru Положение, которое занимает тело экземпляра в глобальной СК.
                       \en The placement that the instance solid takes in global space.
    \return \ru Новый экземпляр тела, зарегистрированный в аппарате контроля соударений.
            \en The new solid instance registered in the detector.
    \note   \ru Значение compItem может быть нулевым. Значит просто вставка не будет 
                принадлежать ни одному компоненту.
            \en The value compItem can be CDET_NULL. This just means that the 
                instance does not belong to any component.
  */
  cdet_item AddInstance( cdet_item compItem, cdet_item solidItem, const MbPlacement3D & place );  
  /**
    \brief \ru Удалить геометрический объект из набора для контроля столкновений.
           \en Remove a geometric object from the set of collision detection. \~
  */
  void      RemoveItem( cdet_item cdItem );
  /**
    \brief  \ru Поменять текущее положение геометрического объекта в наборе.
            \en Change current position of a geometric object. \~
  */
  void      Reposition( cdet_item, const MbPlacement3D & );
  /**
    \brief  \ru Проверить соударения между геометрическими объектами набора.
            \en Check collisions between geometric objects of the set. \~
    \return \ru Функция вернет CDET_RESULT_Intersected при обранужении хотя бы одной коллизии.
            \en The function will return CDET_RESULT_Intersected if it detects at least one collision.
  */
  cdet_result CheckCollisions( cdet_query & );
  /**
    \brief  \ru Проверить соударения между геометрическими объектами набора.
            \en Check collisions between geometric objects of the set. \~
    \return \ru Функция вернет CDET_RESULT_Intersected при обранужении хотя бы одной коллизии.
            \en The function will return CDET_RESULT_Intersected if it detects at least one collision.    
  */
  cdet_result CheckCollisions();

  /**
    \brief  \ru Выдать дескриптор клиентского приложения по дескриптору контрольного набора столкновений.  
            \en Get an application pointer by descriptor of the collision detection set.
  */
  cdet_app_item AppItem( cdet_item cdItem ) const;


public: // the functions below can be deprecated in future version.
  /*
    \brief \ru Добавить модель тела, как набор граней и решеток. 
           \en Add a solid data as a set of faces and the grids. \~
    \return \ru Внутренняя структура данных представляющая добавленную модель. \en Internal data structure representing added solid data. \~
  */
  cdet_item     AddSolid( const MbLumpAndFaces & );
  /// \ru Добавить тело с заданным положением. \en Add a solid with a given placement.
  cdet_item     AddSolid( const MbSolid &, const MbPlacement3D &, cdet_app_item = CDET_APP_NULL );
  /// \ru Удалить твердотельную модель или луч из детектора столкновений. \en Remove a solid model or ray from a collision detector.
  void          RemoveSolid( cdet_item );
  /// \ru Выдать количество добавленных твердотельных моделей или лучей. \en Get number of added solid models or rays.
  size_t        Count() const;
  /// \ru Номер твердотельной модели или луча, зарегистрированной в детекторе. \en An index of solid model or ray registered in the detector.
  size_t        SolidIndex( cdet_item cItem ) const;
  /// \ru Вычисление минимального расстояния между объектами (см.функцию #SetDistanceTracking) \en Calculation of minimal distance between objects (see the function #SetDistanceTracking)
  cdet_result   DistanceQuery( MbProximityParameters & ) const;
  /// \ru Вычисление минимального расстояния между объектами
  cdet_result   DistanceQuery( cdet_item, cdet_item, MbProximityParameters & ) const;
  /// \ru Выключить из рассмотрения все модели. \en Exclude all models from consideration.
  void          FlushSolids();
  /// \ru Выдать иерархическое представление тела (CDET_NULL = отсутствие такового в списке). \en Get the hierarchical representation of the solid (CDET_NULL means that the solid is not in the list).
  cdet_item     GetHRepSolid ( const MbLumpAndFaces & ) const;
  /// \ru Задать барьер для отличия касания от пересечения. \en Set the barrier for the difference between the touch and the intersection.
  void          SetTouchTolerance( double lTol );
  /// \ru Вкл./выкл. приближенного вычисления пересечений тел \en On/off approximated calculation of intersections of solids 
  void          SetApproxCollisionQuery( bool ff );
  /// \ru Вкл./выкл. приближенного вычисления параметров близости - по триангуляции \en On/off approximated calculation of proximity parameters - by triangulation 
  void          SetApproxDistanceComputation ( bool ff );
  /// \ru Назначить объекты для отслеживания между ними расстояния. \en Assign the pair to track the distance between them.
  void          SetDistanceTracking( const MbLumpAndFaces &, const MbLumpAndFaces & );
  /// \ru Обновить текущее положение тела или луча с индексом solIdx. \en Update current placement of solid or ray with index solIdx.
  void          SetPlacement( size_t solIdx, const MbPlacement3D & );

  // \ru Объявление конструктора копирования и оператора присваивания без реализации, чтобы не было присваивания по умолчанию. \en The declaration without implementation of the copy-constructor and assignment operator to prevent an assignment by default. 
  OBVIOUS_PRIVATE_COPY( MbCollisionDetectionUtility );

public:  /*
            Deprecated and testing functions
         */
  void        SetCallback( CDET_exam_func );
  // Use AppItem() insead this
  cdet_app_item Component( size_t solIdx ) const;
  // The func is deprecated. Instead, use CheckCollisions
  cdet_result InterferenceDetect( void * formalPar = nullptr ) const;
  // The func is deprecated. Use SetDistanceTracking instead.
  void        SetDistanceComputationObjects( const MbLumpAndFaces &, const MbLumpAndFaces & );
  // The func is deprecated. Use AddSolid/AddItem instead.
  size_t      AddLump( const MbLumpAndFaces & );
  // For testing purposes
  bool        IsEmpty( cdet_item ) const;
  // For testing purposes
  const MtRefItem * _ComputeBVTree( cdet_item );

private:

  /*
    \brief \ru Добавить объект геометрической модели в набор для контроля столкновений.
           \en Add an object of geometric model to the set of collision detection control. \~
    \return \ru Объект в наборе для контроля столкновений. \en Object of the set of collision detection. \~
  */  
  cdet_item   AddItem( const MbItem & );
  // Set an assembly to detect collisions between its elements
  void        SetAssembly( const MbAssembly & );
  void        UpdateGeometry();  
};

//----------------------------------------------------------------------------------------
// Default implemention of the call CheckCollisions.
//---
inline cdet_result MbCollisionDetectionUtility::CheckCollisions()
{  
  cdet_query_result defaultQuery;
  return CheckCollisions( defaultQuery );
}

/// \ru Узел дерева объемов. \en A node of the bounding volume tree.
typedef const MtRefItem * cdet_bvt_node;
/// \ru Пустое дерево объемов. \en An empty bounding volume tree.
const cdet_bvt_node CDET_BVT_NULL = nullptr;
/// \ru Пара ветвей поддерева объемов. \en A pair of branches of the bounding volume subtree.
typedef std::pair<cdet_bvt_node,cdet_bvt_node> cdet_bvt_pair;  

//---------------------------------------------------------------------------------------
/** 
  \brief  \ru Получить левую и правую ветви поддерева объемов.
          \en Get the left and the right branches of the bounding volume subtree.
*/
//---
MATH_FUNC(cdet_bvt_pair) BvtSubNodes( cdet_item, cdet_bvt_node );

//---------------------------------------------------------------------------------------
/** \brief  \ru Матрица получения ограничивающего параллелепипеда из единичного куба. 
            \en Transformation matrix yielding bounding parallelepiped from the unit cube.
*/
//---
MATH_FUNC(void) GetOrientedBox( cdet_item, cdet_bvt_node, MbMatrix3D & );

#endif // __CDET_UTILITY_H

/** \} */

// eof
