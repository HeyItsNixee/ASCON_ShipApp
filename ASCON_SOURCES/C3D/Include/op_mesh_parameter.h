////////////////////////////////////////////////////////////////////////////////
/**
  \file
  \brief \ru Параметры операций над полигональными сетками.
         \en Parameters of operations on the polygonal meshes. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __OP_MESH_PARAMETERS_H
#define __OP_MESH_PARAMETERS_H


#include <math_define.h>
#include <mesh_plane_grid.h>
#include <mesh_float_point.h>
#include <model_item.h>
#include <templ_sptr.h>
#include <cur_contour.h>

//------------------------------------------------------------------------------
/** \brief \ru Параметры операции упрощения триангуляции.
           \en Triangulation simplification parameters. \~
    \details \ru Параметры операции упрощения триангуляции.
                 Упрощение может производиться до достижения заданного количества треугольников,
                 либо до превышения заданной точности.
             \en Triangulation simplification parameters.
                 Simplification can be carried out until target triangles count reaching or until
                 exceeding of specified tolerance. \~
    \ingroup Shell_Building_Parameters
*/
// ---
class MATH_CLASS MbMeshSimplificationParams
{
private:
  c3d::DoubleVector  _toleranceLevels;     ///< \ru Уровни точностей для упрощения. \en Tolerance levels for simplification. \~
  c3d::IndicesVector _triangleCountLevels; ///< \ru Уровни количества треугольников для упрощения. \en Triangle count levels for simplification. \~

public:
  /// \ru Конструктор для задания целевй точности. \en Constructor for tolerance target. \~
  MbMeshSimplificationParams( const c3d::DoubleVector & toleranceLevels )
    : _toleranceLevels( toleranceLevels )
    , _triangleCountLevels()
  {}

  /// \ru Конструктор для задания целевого количества треугольников. \en Constructor for triangles count target. \~
  MbMeshSimplificationParams( const c3d::IndicesVector & triangleCountLevels )
    : _toleranceLevels()
    , _triangleCountLevels( triangleCountLevels )
  {}

  /// \ru Деструктор. \en Destructor. \~
  ~MbMeshSimplificationParams() {}

  ///< \ru Делать ли упрощение до достижения целевого количества треугольников. \en Is target triangles count. \~
  bool IsTriangleCountTarget() const { return !_triangleCountLevels.empty(); }

  ///< \ru Делать ли упрощение до достижения целевой точности. \en Is target tolerance. \~
  bool IsToleranceTarget() const { return !_toleranceLevels.empty(); }

  ///< \ru Получить уровни упрощения для точности. \en Get simplification levels for tolerance. \~
  const c3d::DoubleVector & ToleranceLevels() const { return _toleranceLevels; }

  ///< \ru Получить уровни упрощения для количества треугольников. \en Get simplification levels for triangles count. \~
  const c3d::IndicesVector & TrianglesCountLevels() const { return _triangleCountLevels; }

  ///< \ru Валидны ли параметры? \en Is parameters valid? \~
  bool IsValid() const
  {
    bool isValid = true;
    if ( IsTriangleCountTarget() )
    {
      if ( _triangleCountLevels.size() > 1 )
      {
        auto valPrev = _triangleCountLevels.front();
        for ( size_t i = 1, n = _triangleCountLevels.size(); i < n; ++i )
        {
          const auto val = _triangleCountLevels[i];
          if ( val >= valPrev )
          {
            isValid = false;
            break;
          }
          valPrev = val;
        }
      }
    }
    else if ( IsToleranceTarget() )
    {
      if ( _toleranceLevels.size() > 1 )
      {
        auto valPrev = _toleranceLevels.front();
        for ( size_t i = 1, n = _toleranceLevels.size(); i < n; ++i )
        {
          const auto val = _toleranceLevels[i];
          if ( val <= valPrev )
          {
            isValid = false;
            break;
          }
          valPrev = val;
        }
      }
    }
    return isValid;
  }

  OBVIOUS_PRIVATE_COPY( MbMeshSimplificationParams )
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры операции создания выпуклого многоугольника в 2D.
           \en Parameters for convex hull 2D creation. \~
    \details \ru Параметры операции создания выпуклого многоугольника в 2D.
             \en Parameters for convex hull 2D creation. \~
    \ingroup Shell_Building_Parameters
*/
// ---
class MATH_CLASS MbConvexHullParams
{
private:
  bool                   _makeGrid;     ///< \ru Создавать ли двумерную триангуляцию. \en Should planar grid be created. \~
  bool                   _makeDelaunay; ///< \ru Создавать ли Делоне триангуляцию. \en Should planar grid be Delaunay triangulation. \~
  bool                   _makeContour;  ///< \ru Создавать ли двумерный контур. \en Should planar contour be created. \~
  double                 _tolerance;    ///< \ru Точность. \en Tolerance. \~
  c3d::ParamPointsVector _points;       ///< \ru Точки. \en Points. \~

public:
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams( const c3d::ParamPointsVector & points, double tolerance, bool makeContour, bool makeGrid, bool makeDelaunay )
    : _points      ( points       )
    , _tolerance   ( tolerance    )
    , _makeContour ( makeContour  )
    , _makeGrid    ( makeGrid     )
    , _makeDelaunay( makeDelaunay )
  {}
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams( const std::vector<MbFloatPoint> & points, double tolerance, bool makeContour, bool makeGrid, bool makeDelaunay )
    : _tolerance   ( tolerance    )
    , _makeContour ( makeContour  )
    , _makeGrid    ( makeGrid     )
    , _makeDelaunay( makeDelaunay )
  {
    _points.clear();
    _points.reserve( points.size() );
    for ( const auto & pt : points )
      _points.push_back( MbCartPoint( pt ) );
  }
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams( const SArray<MbFloatPoint> & points, double tolerance, bool makeContour, bool makeGrid, bool makeDelaunay )
    : _tolerance   ( tolerance    )
    , _makeContour ( makeContour  )
    , _makeGrid    ( makeGrid     )
    , _makeDelaunay( makeDelaunay )
  {
    _points.clear();
    _points.reserve( points.size() );
    for ( const auto & pt : points )
      _points.push_back( MbCartPoint( pt ) );
  }
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams( const SArray<MbCartPoint> & points, double tolerance, bool makeContour, bool makeGrid, bool makeDelaunay )
    : _tolerance   ( tolerance    )
    , _makeContour ( makeContour  )
    , _makeGrid    ( makeGrid     )
    , _makeDelaunay( makeDelaunay )
  {
    _points.clear();
    _points.reserve( points.size() );
    for ( const auto & pt : points )
      _points.push_back( pt );
  }
  
  /// \ru Деструктор. \en Destructor. \~
  ~MbConvexHullParams() {}

  ///< \ru Создавать ли двумерную триангуляцию. \en Should planar grid be created. \~
  bool   ShouldMakeGrid()     const { return _makeGrid; }
  ///< \ru Создавать ли Делоне триангуляцию. \en Should planar grid be Delaunay triangulation. \~
  bool   ShouldMakeDelaunay() const { return _makeDelaunay; }
  ///< \ru Создавать ли двумерный контур. \en Should planar contour be created. \~
  bool   ShouldMakeContour()  const { return _makeContour; }
  ///< \ru Получить точность. \en Get tolerance. \~
  double GetTolerance()       const { return _tolerance; }
  ///< \ru Получить точки. \en Get points. \~
  const c3d::ParamPointsVector & GetPoints() const { return _points; }

  OBVIOUS_PRIVATE_COPY( MbConvexHullParams )
};


//------------------------------------------------------------------------------
/** \brief \ru Результаты создания выпуклого многоугольника в 2D.
           \en Results of convex hull 2D creating. \~
  \details \ru Результаты создания выпуклого многоугольника в 2D.
           \en Results of convex hull 2D creating. \~
  \ingroup Shell_Building_Parameters
*/
// ---
struct MATH_CLASS MbConvexHullResults
{
public:
  c3d::IndicesVector     _hullIndices; ///< \ru Индексы точек выпуклого многоугольника. \en Indices of convex hull points. \~
  c3d::ParamPointsVector _hull;        ///< \ru Выпуклый многоугольник. \en Convex hull. \~
  SPtr<MbPlanarGrid>     _grid;        ///< \ru Точки. \en Points. \~
  SPtr<MbContour>        _contour;     ///< \ru Контур. \en Contour. \~

  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullResults()
    : _hullIndices()
    , _hull()
    , _grid()
    , _contour()
  {}
  /// \ru Деструктор. \en Destructor. \~
  ~MbConvexHullResults() {}

  OBVIOUS_PRIVATE_COPY( MbConvexHullResults )
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры операции создания выпуклой оболочки в 3D.
           \en Parameters for convex hull 3D creation. \~
    \details \ru Параметры операции создания выпуклой оболочки в 3D.
             \en Parameters for convex hull 3D creation. \~
    \ingroup Shell_Building_Parameters
*/
// ---
class MATH_CLASS MbConvexHullParams3D
{
private:
  double                 _tolerance;  ///< \ru Точность. \en Tolerance. \~
  c3d::SpacePointsVector _points;     ///< \ru Точки. \en Points. \~

public:
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams3D( const c3d::SpacePointsVector & points, double tolerance )
    : _points   ( points    )
    , _tolerance( tolerance )
  {}
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams3D( const std::vector<MbFloatPoint3D> & points, double tolerance )
    : _tolerance( tolerance )
  {
    _points.clear();
    _points.reserve( points.size() );
    for ( const auto & pt : points )
      _points.push_back( MbCartPoint3D( pt ) );
  }
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams3D( const SArray<MbFloatPoint3D> & points, double tolerance )
    : _tolerance( tolerance )
  {
    _points.clear();
    _points.reserve( points.size() );
    for ( const auto & pt : points )
      _points.push_back( MbCartPoint3D( pt ) );
  }
  /// \ru Конструктор.. \en Constructor. \~
  MbConvexHullParams3D( const SArray<MbCartPoint3D> & points, double tolerance )
    : _tolerance( tolerance )
  {
    _points.clear();
    _points.reserve( points.size() );
    for ( const auto & pt : points )
      _points.push_back( pt );
  }

  /// \ru Деструктор. \en Destructor. \~
  ~MbConvexHullParams3D() {}

  ///< \ru Получить точность. \en Get tolerance. \~
  double GetTolerance() const { return _tolerance; }
  ///< \ru Получить точки. \en Get points. \~
  const c3d::SpacePointsVector & GetPoints() const { return _points; }

  OBVIOUS_PRIVATE_COPY( MbConvexHullParams3D )
};


//------------------------------------------------------------------------------
/** \brief   \ru Параметры для поиска и замены повторяющихся полигональных объектов (#MbMesh) на вставки #MbInstance.
             \en Parameters for the replacement of repetative meshes (#MbMesh) into #MbInstance. \~
    \details \ru Параметры для поиска и замены одинаковых полигональных объектов (#MbMesh) на вставки #MbInstance.
                 Возможно переключение между абсолютной/относительной точностями сравнения точек. Под относительной
                 точностью имеется в виду ограничение сверху по каждой координате на abs( (c1 - c2) / (c1 + c2) ). \n
             \en Parameters for the replacement of repetative meshes (#MbMesh) into #MbInstance.
                 It is possible to switch between absolute/relative accuracy for the points comparison. The relative
                 accuracy is the upper bound of the expression abs( (c1 - c2) / (c1 + c2) ) for each coordinate. \n \~
    \ingroup Polygonal_Objects
*/
// ---
class MATH_CLASS MbMeshToInstanceParams : public MbPrecision
{
private:
  /// \ru Использовать относительную или абсолютную точность при сравнении точек.\en Use absolute or relative tolerance for points comparison.\~
  bool                   _isAbsoluteAccuracy;

public:
  /// \ru Конструктор по-умолчанию. \en Default constructor.\~
  MbMeshToInstanceParams ()
    : MbPrecision        (      )
    , _isAbsoluteAccuracy( true )
  {}
  /// \ru Конструктор. \en Constructor. \~
  MbMeshToInstanceParams ( double precision, bool isAbsoluteAccuracy )
    : MbPrecision        ( precision, ANGLE_REGION )
    , _isAbsoluteAccuracy( isAbsoluteAccuracy      )
  {}
  /// \ru Деструктор. \en Destructor. \~
  ~MbMeshToInstanceParams() {}

  /// \ru Переключатель между абсолютной и относительной точностью. \en Switch between absolute and relative accuracy. \~
  void   SetAbsoluteAccuracy( bool isAbs )       { _isAbsoluteAccuracy = isAbs; }
  /// \ru Установлена абсолютная или относительная точность. \en Whether the accuracy is set to absolute or relative. \~
  bool   IsAbsoluteAccuracy ()             const { return _isAbsoluteAccuracy; }

  OBVIOUS_PRIVATE_COPY( MbMeshToInstanceParams );
};


//------------------------------------------------------------------------------
/** \brief   \ru Результаты поиска и замены повторяющихся полигональных объектов (#MbMesh) на вставки #MbInstance.
             \en Results of the replacement of repetative meshes (#MbMesh) into #MbInstance. \~
    \details \ru Результаты поиска и замены одинаковых полигональных объектов (#MbMesh) на вставки #MbInstance.
                 Содержат присланный массив, где на месте повторяющихся полигональных объектов расположены вставки. \n
             \en Results of the replacement of repetative meshes (#MbMesh) into #MbInstance. 
                 Contain the initial array, where instances replace repeatative meshes.\n \~
    \ingroup Polygonal_Objects
*/
// ---
class MATH_CLASS MbMeshToInstanceResults
{
public:
  c3d::ItemsSPtrVector _meshContainer; ///< \ru Выходной массив. \en Output array \~
  size_t               _nReplaced;     ///< \ru Число заменённых объектов. \en Number of replaced objects. \~

public:
  /// \ru Конструктор. \en Constructor. \~
  MbMeshToInstanceResults()
    : _nReplaced ( 0 )
  {}
  /// \ru Деструктор. \en Destructor. \~
  ~MbMeshToInstanceResults() {}

  /// \ru Обнулить. \en Reset. \~
  void Reset () { _nReplaced = 0; _meshContainer.clear(); }

  OBVIOUS_PRIVATE_COPY( MbMeshToInstanceResults );
};
#endif // __OP_MESH_PARAMETERS_H
