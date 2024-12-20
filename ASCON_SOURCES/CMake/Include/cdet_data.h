﻿//////////////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Типы данных утилиты обнаружения столкновений.
         \en Data types of collision detection. \~
*/
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CDET_DATA_H
#define __CDET_DATA_H

#include <templ_sptr.h>
#include <mb_cart_point.h>
#include <mb_operation_result.h>
#include <mb_matrix3d.h>
#include <set>

class MbHRepSolid;

/**
  \addtogroup Collision_Detection
  \{
*/

//----------------------------------------------------------------------------------------
/// \ru Объект из набора контроля столкновений. \en Object from the set of collision detection.
//---
typedef const MbHRepSolid * cdet_item;
typedef MbResultType        cdet_result; ///< \ru Код результата контроля столкновений. \en Result code of collision queries.

//----------------------------------------------------------------------------------------
// \ru Код результата контроля столкновений. \en Codes of collision detection.
//---
const cdet_result CDET_RESULT_Intersected    = rt_Intersect;
const cdet_result CDET_RESULT_NoIntersection = rt_NoIntersect;
const cdet_result CDET_RESULT_Ok             = rt_Success;
const cdet_result CDET_RESULT_None           = rt_None;
const cdet_result CDET_RESULT_Error          = rt_Error;

//----------------------------------------------------------------------------------------
// \ru Геометрический объект пользователя. \en User geometric item.
//---
typedef const void * cdet_app_item;

//----------------------------------------------------------------------------------------
// Constants
//---
const cdet_item     CDET_NULL = nullptr;     ///< \ru Пустой объект набора для контроля столкновений. \en Empty object of the collision query set.
const cdet_app_item CDET_APP_NULL = nullptr; ///< \ru "Нулевой" объект модели приложения. \en "Null object" of the client app.

//----------------------------------------------------------------------------------------
/** \brief  \ru Структура данных и обратных вызовов для запроса на поиск соударений.
            \en Data structure and callbacks for the collision search request.
    \details
    \ru Базовый класс, предназначенный для реализации на стороне приложения структуры 
  запроса к алгоритму поиска соударений. С помощью обратных вызовов приложение
  управляет поиском соударений и получает сведения o найденных пересечениях.
    \en The base class intended to implement on the application side the structure of a query 
  to the collision search algorithm. The application uses a callback to control the collision 
  search and obtain details of detected interferencies.
            
*/
//---
struct MATH_CLASS cdet_query
{
  enum cback_res  ///< Result code of the callback function
  {
      CBACK_VOID    
    , CBACK_SUFFICIENT  ///< This code means that an app stops collision query for given pair of lumps
    , CBACK_SKIP        ///< Skip testing a given pair of the lumps
    , CBACK_NEED        ///< Enable testing a given pair of the lumps
    , CBACK_BREAK       ///< Break search of all collisions of the set
    , CBACK_SEARCH_MORE = CBACK_VOID  ///< This code notifies a collision detector to continue working at cases CDET_INTERSECTED, CDET_TOUCHED.
  };

  enum message  ///< Code of notification
  {
      CDET_QUERY_STARTED  // The collision query is started for the all solids
    , CDET_STARTED        // The collision query is started for the given pair
    , CDET_FINISHED       // Collision detector complete searching a collisions for the given pair of lumps.
    , CDET_INTERSECTED    // The collided pair of objects founded.
    , CDET_TOUCHED        // Touched faces has been founded with no penetration of the solids.
    , CDET_INCLUDED       // The included pair of objects founded.
  };

  struct geom_element ///< Structure representing a collision detection geometry.
  {
    cdet_app_item       appItem;
    const MbRefItem *   refItem;
    const MbMatrix3D *  wMatrix;
    geom_element()
      : appItem( nullptr )
      , refItem( nullptr )
      , wMatrix( &MbMatrix3D::identity ) {}
  };

  struct cback_data ///< Data structure that notifies an app about collision detection event.
  {    
    geom_element first, second; ///< Pair of geometric objects
    cback_data(): first(), second() {}
  };

  // If message is CDET_INCLUDED, then cback_data::first is in cback_data::second.
  cback_res operator() ( message code, cback_data & cData ) { return func( this, code, cData ); }  

protected:
  typedef cback_res (*cback_func)( cdet_query *, message, cback_data & );

  cdet_query( cback_func _func ) : func(_func) {}
  cdet_query( const cdet_query & cQuery ) : func(cQuery.func) {}
 ~cdet_query() {}

  cdet_query & operator = ( const cdet_query & cQuery ) { func = cQuery.func; return *this; }

private:
  cback_func func;
};

//----------------------------------------------------------------------------------------
// Simple collision query data to search for the first detected interference.
//---
struct MATH_CLASS cdet_query_result: public cdet_query
{  
  cdet_result result;

  cdet_query_result() 
    : cdet_query( QueryFunc )
    , result( CDET_RESULT_NoIntersection )
  {}

  cdet_query_result( const cdet_query_result & cQuery ) 
    : cdet_query( cQuery )
    , result( cQuery.result )
  {}


private:  
  static cback_res QueryFunc( cdet_query * query, message code, cback_data & )
  { 
    C3D_ASSERT( nullptr != query );
    cdet_query_result * q = static_cast<cdet_query_result*>( query );
    switch( code )
    {      
    case CDET_QUERY_STARTED: // The collision query is started for all solids of the scene.
      {
        q->result = CDET_RESULT_NoIntersection;
        return CBACK_VOID;
      }
    case CDET_INTERSECTED: // First intersection is founded.
      {
        q->result = CDET_RESULT_Intersected;
        return CBACK_SUFFICIENT;
      }            
    case CDET_FINISHED: // A pair of solids is finished.
      return (q->result == CDET_RESULT_Intersected) ? CBACK_BREAK : CBACK_VOID;

    default:
      return CBACK_VOID;
    }
  }
};

//----------------------------------------------------------------------------------------
// The structure queries first founded collision faces.
//---
struct MATH_CLASS cdet_first_collided: public cdet_query
{ 
  SPtr<const MbRefItem> first, second;  // collided faces

  cdet_first_collided() 
    : cdet_query( QueryFunc )
    , first()
    , second()
  {}

private:
  static cback_res QueryFunc( cdet_query * query, message code, cback_data & cData )
  {
    if ( cdet_first_collided * q = static_cast<cdet_first_collided*>(query) )
    {
      switch( code )
      {      
      case CDET_QUERY_STARTED: // The collision query is started for all solids of the set
        {
          q->first = q->second = nullptr;
          return CBACK_VOID;
        }
      case CDET_FINISHED: // A pair of solids is finished.
        return (q->first && q->second) ? CBACK_BREAK : CBACK_SEARCH_MORE;

      case CDET_INTERSECTED: // First intersection is founded.
        {
          q->first = cData.first.refItem;
          q->second = cData.second.refItem;
          return (q->first && q->second) ? CBACK_SUFFICIENT : CBACK_SEARCH_MORE;
        }            
      default:
        return CBACK_VOID;
      }
    }
    return CBACK_VOID;
  }
  OBVIOUS_PRIVATE_COPY( cdet_first_collided );
};

//----------------------------------------------------------------------------------------
/** \brief  \ru Структура запроса для поиска граней столкновения.
            \en The structure of the query to find collision faces.
*/
//---
struct MATH_CLASS cdet_collided_faces: public cdet_query
{ 
  typedef std::pair<cdet_app_item,const MbRefItem*> item_face;  // represents a face of app item
  typedef std::set<item_face> collided_faces;
  collided_faces                        faces;
  std::map<cdet_app_item,cdet_app_item> groups;
  cdet_app_item                         excluded; // a member of excluded group

public:
  cdet_collided_faces()
    : cdet_query( _QueryFunc )
    , faces()
    , groups()
    , excluded( CDET_APP_NULL )
  {}

  /** \brief  \ru Объединить пару геометрических объектов в группу.
              \en Unite a pair of geometric items to the group.
    \details  \ru Функция объединяет в группу два отдельных объекта или присоединяет 
                  первый объект к группе, которой принадлежит второй. Если оба объекта уже 
                  принадлежат каждый своей группе, то обе группы сливаются в одну общую.
              \en The function unites to group two separate objects or the first object 
                  attaches to the group, which owns the second. If both objects already 
                  belong to each of their group, the two groups merged into a single.
  */
  void Group( cdet_app_item fst, cdet_app_item snd )
  {
    fst = _Parent( fst );
    snd = _Parent( snd );
    if ( fst < snd )
    {
      std::swap( fst, snd );      
    }
    groups[fst] = snd;
  }

  /** \brief  \ru Исключить из контроля на столкновения тела группы.
              \en Exclude from the collision control solids of the group.
      \param[in] member - \ru Любой участник группы, элементы которой исключаются.
                          \en Any member of the group whose elements are excluded. \~ 
  */
  void ExludeGroup( cdet_app_item member )
  {
    if ( excluded == CDET_APP_NULL )
      excluded = _Parent( member );
    else
      Group( excluded, member );
  }
  /** \brief  \ru Отменить результаты работы функций Group() и ExludeGroup().
              \en Cancel the results of the functions Group() and ExludeGroup().
  */
  void Reset()
  {
    faces.clear();
    groups.clear();
    excluded = CDET_APP_NULL;
  }

private:
  static cback_res _QueryFunc( cdet_query * query, message code, cback_data & cData )
  {
    if ( cdet_collided_faces * q = static_cast<cdet_collided_faces*>(query) )
    {
      switch( code )
      {      
      case CDET_QUERY_STARTED:
        {
          q->faces.clear();
          return CBACK_VOID;
        }
      case CDET_STARTED:
        {
          if ( q->_SameGroups(cData.first.appItem,cData.second.appItem) )
          {
            return CBACK_SKIP;
          }
          return CBACK_VOID;
        }

      case CDET_FINISHED: // a pair of solids was finished.
        return CBACK_SEARCH_MORE;

      case CDET_INTERSECTED:
        {
          if ( cData.first.refItem )
          {
            q->faces.insert( item_face(cData.first.appItem,cData.first.refItem) );
          }
          if ( cData.second.refItem )
          {
            q->faces.insert( item_face(cData.second.appItem,cData.second.refItem) );
          }
          return CBACK_SEARCH_MORE;
        }            
      default:
        return CBACK_VOID;
      }
    }
    return CBACK_VOID;
  }

  cdet_app_item _Parent( cdet_app_item appItem ) const
  {
    std::map<cdet_app_item,cdet_app_item>::const_iterator iter = groups.find( appItem );
    if ( iter == groups.end() || (iter->second == iter->first) )
    {
      return appItem;
    }
    C3D_ASSERT( iter->second < iter->first );

    return _Parent( iter->second );
  }

  bool  _SameGroups( cdet_app_item fst, cdet_app_item snd ) const
  {    
    return _Parent( fst ) == _Parent( snd );
  }

  OBVIOUS_PRIVATE_COPY( cdet_collided_faces );
};

//----------------------------------------------------------------------------------------
//
//---
typedef enum
{
    CDET_EXAM_EnableComponentsOnly  // Test collisions between components only.
  , CDET_EXAM_Enabled   // Enable the pair to test collisions.
  , CDET_EXAM_Disabled  // Reject the collision test of the pair.  
} CDET_exam_status;

//----------------------------------------------------------------------------------------
//
//---
struct MATH_CLASS CDET_item_data
{
  cdet_item comp; // Descriptor of the component owning the inctance.
  cdet_item inst; // Descriptor of the instance.
  cdet_app_item appItem; // Application pointer for the instance or the component.
  CDET_item_data()
  {
    comp = inst = CDET_NULL;
    appItem = CDET_APP_NULL;
  }
};

//----------------------------------------------------------------------------------------
//
//---
typedef CDET_exam_status (*CDET_exam_func)( cdet_query *, const CDET_item_data &, const CDET_item_data & );

/** \} */ // Collision_Detection

class TapeBase;
class MbFace;

//----------------------------------------------------------------------------------------
/* \brief \ru Грань столкновения.  \en A face of collision. \~
*/
// ---
class MATH_CLASS MbCollisionFace
{
  cdet_item      item;      // The instance to witch the face belongs.
  const MbFace * mathFace;  // The topological face identified in the collision detection or proximity query.
  TapeBase     * partFace;  // The face of the application representation.

public:
  MbCollisionFace( const MbFace & f ) 
    : item( CDET_NULL )
    , mathFace( &f )
    , partFace( nullptr ) 
  {}

  const MbFace & Face() const { return *mathFace; }
  cdet_item      Item() const { return item;  }
  const MbFace & GetMathFace() const { return *mathFace; }

  // \ru Установка объекта модели. \en Setting an object of model.
  void SetCollisionFaceObject( TapeBase * _partFace ) { partFace = _partFace; }
  
  // \ru Выдача объекта модели. \en Getting an object of model.
  TapeBase * GetCollisionFaceObject() const { return partFace; }

  // \ru Задать грань и компонент-вставку, которой принадлежит. \en Set a face and its component-instance.
  MbCollisionFace & SetFace( const MbFace * f, cdet_item inst )
  {
    mathFace = f;
    item = inst;
    return *this;
  }

  MbCollisionFace & operator = ( const MbCollisionFace & other )
  { 
    item     = other.item;
    mathFace = other.mathFace;
    partFace = other.partFace;
    return *this;
  }
  bool operator >   ( const MbCollisionFace & other ) const { return mathFace >  other.mathFace; }
  bool operator <   ( const MbCollisionFace & other ) const { return mathFace <  other.mathFace; }
  bool operator ==  ( const MbCollisionFace & other ) const { return mathFace == other.mathFace; }
  bool operator !=  ( const MbCollisionFace & other ) const { return !(*this == other); }

private:
  MbCollisionFace( const MbCollisionFace & ); // not implemented
};

//----------------------------------------------------------------------------------------
// \ru Параметры (характеристика) близости двух объектов. \en Parameters (characteristic) of proximity of two objects.
// ---
class MATH_CLASS MbProximityParameters
{
  MbCollisionFace *   theFace1;
  MbCollisionFace *   theFace2;
  SPtr<const MbFace>  plane;

public:
  MbCartPoint3D fstPnt, sndPnt;   // \ru Пара точек близости, принадлежащие триангуляционным сеткам. \en The points of the proximity belonging to the triangulation grids.
  MbCartPoint thePar1, thePar2;   // \ru Пара точек близости, заданная в поверхностных координатах граней. \en The points of the proximity specified in the surface coordinates of the faces.
  double      theDistance;        // \ru Расстояние. \en Distance. 
  double      upperDist;          // \ru Верхняя оценка для поиска минимальной дистанции. \en The upper bound of the minimal distance estimation.

public:
  MbProximityParameters();
 ~MbProximityParameters();

protected:
  MbProximityParameters(  const MbFace & topoFace1
                        , const MbFace & topoFace2
                        , MbCartPoint  & par1
                        , MbCartPoint  & par2
                        , double        dist );

public:
  const MbCollisionFace & FaceOne() const { return *theFace1; }
  const MbCollisionFace & FaceTwo() const { return *theFace2; }

  void  SetFacePair( const MbFace &, const MbFace & );
  void  SetFacePair( const MbFace *, cdet_item, const MbFace *, cdet_item );

private:
  MbProximityParameters( const MbProximityParameters & );                 // not implemented
  MbProximityParameters & operator =  ( const MbProximityParameters & );  // not implemented
};




#endif // __CDET_DATA_H

// eof
