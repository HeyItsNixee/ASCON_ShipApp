////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Трехмерный проволочный каркас.
         \en Three-dimensional wire frame. \~ 

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __WIRE_FRAME_H
#define __WIRE_FRAME_H


#include <model_item.h>
#include <item_registrator.h>
#include <topology.h>
#include <curve3d.h>
#include <creator.h>
#include <math_define.h>
#include <vector>

class  MATH_CLASS  MbWireFrame;


namespace c3d // namespace C3D
{
typedef SPtr<MbWireFrame>                 WireFrameSPtr;
typedef SPtr<const MbWireFrame>           ConstWireFrameSPtr;

typedef std::vector<MbWireFrame *>        WireFramesVector;
typedef std::vector<const MbWireFrame *>  ConstWireFramesVector;

typedef std::vector<WireFrameSPtr>        WireFramesSPtrVector;
typedef std::vector<ConstWireFrameSPtr>   ConstWireFramesSPtrVector;
} // namespace C3D


//------------------------------------------------------------------------------
/** \brief \ru Трехмерный проволочный каркас.
           \en Three-dimensional wire frame. \~ 
  \details \ru Трехмерный проволочный каркас состоит из множества рёбер MbEdge. \n
    Каркас может состоять из нескольких связных частей.
    Связная часть может иметь топологию звезды, при которой в одной вершине стыкуется более двух рёбер.
    Каркас может быть разбит на отдельные связные части. Каждая связная часть обладает функциями составной кривой.
           \en Three-dimensional wire frame consists of a set of edges of a type MbEdge. \n
    A wire frame may consist of several connected parts.
    A connected part may have a topology of a star where one vertex is coincident with more than two edges
    A wire frame may be split into separate connected parts. Each connected part has functions of a composite curve. \~
  \ingroup Model_Items
*/
// ---
class MATH_CLASS MbWireFrame : public MbItem {
protected :
  c3d::WireEdgesVector  edges;       ///< \ru Множество рёбер каркаса. \en A set of edges of the frame. 
  size_t                partsCount;  ///< \ru Количество связных частей объекта. \en A number of connected parts of an object. 
  bool                  closed;      ///< \ru Замкнутость указывает на возможность получит множество замкнутых кривых. \en Closedness indicates to a possibility to get a set of closed curves. 
  mutable bool          normal;      ///< \ru Разложен ли каркас на связные части? \en Is a frame split into connected parts? 

private :
  /// \ru Конструктор копирования с регистратором. \en Copy-constructor with the registrator. 
  explicit MbWireFrame( const MbWireFrame &, MbRegDuplicate * );
public :
  /// \ru Конструктор без параметров. \en Constructor without parameters. 
  MbWireFrame();
  /// \ru Конструктор по кривой и строителю. \en Constructor by a curve and creator. 
  MbWireFrame( const MbCurve3D &,                  const MbCreator * = nullptr );
  /// \ru Конструктор по множеству кривых и строителю. \en Constructor by a set of curves and creator. 
  MbWireFrame( const RPArray<MbCurve3D> &,         const MbCreator * = nullptr );
  /// \ru Конструктор по множеству кривых и строителю. \en Constructor by a set of curves and creator. 
  MbWireFrame( const c3d::SpaceCurvesSPtrVector &, const MbCreator * = nullptr );
  /// \ru Конструктор по ребру и строителю. \en Constructor by an edge and creator. 
  MbWireFrame( const MbEdge &,                     const MbCreator * = nullptr, bool same = true );
  /// \ru Конструктор по множеству рёбер и строителю. \en Constructor by a set of edges and creator. 
  MbWireFrame( const RPArray<MbEdge> &,            const MbCreator * = nullptr, bool same = true );
  /// \ru Конструктор по множеству рёбер и строителю. \en Constructor by a set of edges and creator. 
  MbWireFrame( const c3d::WireEdgesSPtrVector &,   const MbCreator * = nullptr, bool same = true );
  /// \ru Деструктор. \en Destructor. 
  virtual ~MbWireFrame();

public :
  VISITING_CLASS( MbWireFrame ); 

  // \ru Общие функции геометрического объекта. \en Common functions of a geometric object. 

  MbeSpaceType  IsA()  const override; // \ru Тип объекта. \en An object type. 
  MbSpaceItem & Duplicate( MbRegDuplicate * iReg = nullptr ) const override; // \ru Создать копию. \en Create a copy.
  void    Transform( const MbMatrix3D &, MbRegTransform * = nullptr ) override; // \ru Преобразовать согласно матрице. \en Transform according to the matrix.
  void    Move     ( const MbVector3D &, MbRegTransform * = nullptr ) override; // \ru Сдвинуть вдоль вектора. \en Translate along a vector.
  void    Rotate   ( const MbAxis3D &, double angle, MbRegTransform * = nullptr ) override; // \ru Повернуть вокруг оси. \en Rotate about an axis.
  bool    IsSame   ( const MbSpaceItem &, double accuracy = LENGTH_EPSILON ) const override; // \ru Являются ли объекты равными? \en Are the objects equal? 
  bool    IsSimilar( const MbSpaceItem & ) const override; // \ru Являются ли объекты подобными? \en Are the objects similar? 
  bool    SetEqual ( const MbSpaceItem & ) override; // \ru Сделать объекты равными. \en Make the objects equal. 
  double  DistanceToPoint ( const MbCartPoint3D & ) const override; // \ru Вычислить расстояние до точки. \en Calculate distance to point. 
  void    AddYourGabaritTo( MbCube & ) const override; // \ru Добавь свой габарит в куб. \en Add bounding box into a cube. 
  void    CalculateLocalGabarit( const MbMatrix3D & into, MbCube & cube ) const override; // \ru Рассчитать габарит в локальной системы координат. \en Calculate the bounding box in a local coordinate system. 
  void    CalculateMesh( const MbStepData & stepData, const MbFormNote & note, MbMesh & mesh ) const override; // \ru Построить полигональную копию mesh. \en Build polygonal copy mesh.

  void    GetProperties( MbProperties & ) override; // \ru Выдать свойства объекта. \en Get properties of the object. 
  void    SetProperties( const MbProperties & ) override; // \ru Установить свойства объекта. \en Set properties of the object. 
  void    GetBasisItems ( RPArray<MbSpaceItem> & ) override; // \ru Дать базовые объекты. \en Get the basis objects. 
  void    GetBasisPoints( MbControlData3D & ) const override; // \ru Выдать контрольные точки объекта. \en Get control points of object.
  void    SetBasisPoints( const MbControlData3D & ) override; // \ru Изменить объект по контрольным точкам. \en Change the object by control points.
  bool    GetPlacement( MbPlacement3D & ) const override; // \ru Проинициализировать присланную локальную систему координат (совместить плоскость XY), если каркас плоский. \en Initialize the sent local coordinate system (combine the plane XY) if the frame is planar. 
  // \ru Перестроить объект по журналу построения. \en Reconstruct object according to the history tree. 
  bool    RebuildItem( MbeCopyMode sameShell, RPArray<MbSpaceItem> * items, IProgressIndicator * progInd ) override;

  // \ru Создать полигональный объект - упрощенную копию данного объекта. \en Create a polygonal object - a simplified copy of the given object. 
  MbItem * CreateMesh( const MbStepData & stepData, const MbFormNote & note, MbRegDuplicate * iReg ) const override; 

  /** \ru \name Общие функции каркаса.
      \en \name Common functions of a frame.
      \{ */

          /// \ru Выдать количество ребер каркаса. \en Get the number of edges of the frame. 
          size_t  GetEdgesCount() const { return edges.size(); } 
          /// \ru Выдать объект по индексу. \en Get the item by index. 
  const   MbEdge * GetEdge( size_t i ) const { return (i < edges.size()) ? edges[i] : nullptr; }
          /// \ru Выдать объект по индексу для возможного редактирования. \en Get the item by index for the possible editing. 
          MbEdge * SetEdge( size_t i )       { return (i < edges.size()) ? edges[i] : nullptr; }

          /// \ru Получить ребра. \en Get edges. 
          template <class EdgesVector>
          void    GetEdges( EdgesVector & dstEdges ) const {
            if ( !edges.empty() ) {
              size_t addCnt = edges.size();
              dstEdges.reserve( dstEdges.size() + addCnt );
              c3d::ConstWireEdgeSPtr edge;
              for ( size_t k = 0; k < addCnt; ++k ) {
                edge = edges[k];
                dstEdges.push_back( edge );
              }
            }
          }
          /// \ru Получить ребра для возможного редактирования. \en Get edges for the possible editing. 
          template <class EdgesVector>
          void    SetEdges( EdgesVector & dstEdges ) {
            if ( !edges.empty() ) {
              size_t addCnt = edges.size();
              dstEdges.reserve( dstEdges.size() + addCnt );
              c3d::WireEdgeSPtr edge;
              for ( size_t k = 0; k < addCnt; ++k ) {
                edge = edges[k];
                dstEdges.push_back( edge );
              }
            }
          }
          /// \ru Добавить ребро по кривой и ее ориентации в ребре. \en Add an edge by a curve and its orientation in relation to an edge. 
          void    AddEdge( const MbCurve3D &, bool sense = true );
          /// \ru Добавить ребро (оригинал, не копию). \en Add an edge (an original, not a copy). 
          void    AddEdge( const MbEdge &, bool same = true );
          /// \ru Добавить массив ребер (оригиналы, не копии). \en Add an array of edges (originals, not copies). 
          template <class WireEdgesVector>
          void    AddEdges( const WireEdgesVector &, bool same = true );
          /// \ru Вставить ребро по индексу (оригинал, не копию). \en Insert an edge by index (an original, not a copy). 
          void    InsertEdge( size_t index, const MbEdge & item, bool same = true );
          /// \ru Отцепить ребро по индексу. \en Detach an edge by index. 
          MbEdge * DetachEdge( size_t index );
          /// \ru Удалить все рёбра. \en Delete all edges. 
          void    DeleteEdges(); 
          /// \ru Удалить ребро по индексу. \en Delete an edge by index. 
          bool    DeleteEdge( size_t index ); 
          /// \ru Удалить ребро, если таковое имеется. \en Delete an edge if it already exists. 
          bool    DeleteEdge( MbEdge * );

          /// \ru Выдать массив вершин ребер каркаса. \en Get an array of frame edges vertices. 
          void    GetVerticesArray ( RPArray<MbVertex> & );
          /// \ru Выдать массив вершин ребер каркаса. \en Get an array of frame edges vertices. 
          void    GetVerticesArray ( RPArray<const MbVertex> & ) const;
          /// \ru выдать индекс вершины. \en Get vertex index.
          size_t  GetVertexIndex( const MbVertex & find ) const;
          /// \ru выдать вершину по индексу \en Get vertex by index.
          MbVertex * GetVertex( size_t index ) const;
          /// \ru выдать индекс ребра. \en Get edge index.
          size_t  GetEdgeIndex( const MbEdge & find ) const;
          /// \ru Получить вершины. \en Get vertices. 
          template <class VerticesSet>
          void    GetVerticesSet( VerticesSet & dstVertices ) const {
            if ( !edges.empty() ) {
              size_t addCnt = edges.size();
              c3d::ConstWireEdgeSPtr edge;
              c3d::ConstVertexSPtr vertex;
              for ( size_t k = 0; k < addCnt; ++k ) {
                edge = edges[k];
                vertex = &edge->GetBegVertex();
                dstVertices.insert( vertex );
                if ( &edge->GetEndVertex() != &edge->GetBegVertex() ) {
                  vertex = &edge->GetEndVertex();
                  dstVertices.insert( vertex );
                }
              }
            }
          }

          /// \ru Выдать вершину-начало каркаса. \en Get the start vertex of a frame. 
  const   MbVertex * GetBegVertex() const;
          /// \ru Выдать вершину-конец  каркаса. \en Get the end vertex of a frame. 
  const   MbVertex * GetEndVertex() const; 

          /// \ru Найти вершину по имени. \en Find vertex by name. 
  const   MbVertex * FindVertexByName( const MbName & ) const; 
          /// \ru Найти ребро по имени. \en Find edge by name. 
  const   MbEdge *   FindEdgeByName  ( const MbName & ) const; 

          /** \brief \ru Разбить ребро по параметрам его кривой на несколько его частей.
                     \en Split the edge using the curve parameters into several pieces. \~
          \details \ru . Если beginSafe == true  - ребро сохранит начальный участок, 
              Если beginSafe == false - ребро сохранит конечный  участок. 
              По параметру 'eps' отсеиваются значения в контейнере 'params', совпадающие друг с другом и с начальным и конечным параметрами кривой.
              Контейнер 'edges' содержит отрезанные части.
                     \en . If beginSafe == true then the edge saves its starting piece, 
              If beginSafe == false then the edge saves its ending piece. 
              According to the parameter 'eps' drop out value in the container 'params', coinciding with each other and with the initial and final parameters of the curve.
              The container 'edges' contains cut parts. \~ 
              \params[in, out] targetEdge - \ru Ребро для разрезания. Возвращается урезанный кусок с учетом флага beginSafe или nullptr,
                                                если параметр разрезания находится на расстоянии меньшим еps от соответствующего конца кривой,
                                            \en Edge for cutting. The return value of 'targetEdge' is the shortened edge according to 'beginSafe' flag
                                                or nullptr, if the cut param in 'params' lies at the distance less than 'eps' from the corresponding end of the curve,
              \param[in] params -           \ru Параметры кривой для разбиения ребра,
                                            \en Parameters of intersection curve of edge to split, \~
              \param[in] beginSafe -        \ru Ребро сохранит начальную часть (true) или ребро сохранит конечную часть (false),
                                            \en The edge will keep a beginning piece (true) or the edge will keep an end piece (false) \~
              \param[in] eps -              \ru Точность совпадения параметров разбиения,
                                            \en Precision matching options of parameters to split, \~
              \param[out] edges -           \ru Отрезанные части ребра.
                                            \en The container of cut parts. \~
              \return \ru Возвращает true, если ребро было разрезано.
                      \en Returns true, if the edge was cut. \~
          */
          bool    CuttingEdge( MbEdge *& targetEdge, SArray<double> & params, bool beginSafe, double eps, RPArray<MbEdge> & edges );

          /// \ru Замкнут ли каркас? \en Is frame closed? 
          bool    IsClosed(); 
          /// \ru Является ли каркас многосвязным? \en Is frame multiply connected? 
          bool    IsMultiWireFrame();
          /// \ru Количество связных частей каркаса. \en A number of connected parts of a frame. 
          size_t  GetPartsCount();

          /// \ru Является ли объект плоским? \en Is the object planar? 
          bool    IsPlanar() const; 
          /// \ru Дать плоскую кривую и ее систему координат, если пространственная кривая плоская (после использования вызывать DeleteItem на двумерные кривые). \en Get planar coordinate system if the space curve is planar (after the using call DeleteItem for two-dimensional curves) 
          bool    MakePlaneCurves( RPArray<MbCurve> & curves, MbPlacement3D & place ) const;
          /// \ru Дать кривую на поверхности, если пространственная кривая на поверхности (после использования вызывать DeleteItem на двумерные кривые). \en Get a surface curve if a space curve is on a surface (after the using call DeleteItem for two-dimensional curves) 
          bool    MakeSurfaceCurves( RPArray<MbCurve> & curves, MbSurface *& surface ) const;
          /** \brief \ru Построить контуры из копий кривых.
                     \en Construct contours of curves copies.\~
            \details \ru Построить контуры из копий кривых. Новые контуры прицепляются к массиву curves.\n
                     \en Construct contours of curves copies. New contours are added to the curves array.\n\~
            \param[out] curves - \ru Массив, к которуму добавляются построенные контуры.
                                 \en Array, to which created contours are added.\~
            \result \ru Возвращает true, если хоть 1 контур был создан.
                    \en Returns true if at least one contour is created. \~
          */
          bool    MakeCurves( RPArray<MbCurve3D> & curves ) const;
          /// \ru Положить в массив оригиналы кривых. \en Put originals of curves into an array. 
          template <class CurvesVector>
          void    GetCurves( CurvesVector & ) const;
          /// \ru Разложен ли каркас на связные части? \en Is a frame split into connected parts? 
          bool    IsNormalizeWire() const { return normal; } 
          /// \ru Переставить кривые и переориентировать ребра, создав связные цепочки с общими вершинами. \en Perform curves reposition and edges reorientation by creating connected chains with common vertices. 
          bool    NormalizeWire( double precision = METRIC_REGION );
          /** \brief \ru Отделение частей каркаса.
                     \en Detachment of frame parts \~
            \details \ru Отделение частей каркаса с сохранением исходного объекта.
              Если исходный каркас распадается на части, то все части складываются в parts. \n
                     \en Detachment of frame parts with saving an initial object.
              If the initial frame is decomposed, all the parts are put into array 'parts'. \n \~
            \param[out] parts - \ru Каркасы, полученные из frame.
                                \en Frames obtained from 'frame'. \~
            \result \ru Возвращает количество каркасов в parts.
                    \en Returns a number of frames in 'parts'. \~
          */
          size_t  CreateParts( RPArray<MbWireFrame> & parts ); 

          /** \brief \ru Создать связные контуры с учётом толерантностей в вершинах рёбер каркаса.
                     \en Create connected contours according to vertex tolerances of the wire frame edges. \~
            \details \ru Создать связные контуры с учётом толерантностей в вершинах рёбер каркаса
                         с сохранением исходного объекта. Если исходный каркас распадается на части,
                         то все части выдаются в качестве результата. Отличается от #MakeCurves тем, что
                         толерантности разные в вершинах рёбер.\n
                     \en Create connected contours according to vertex tolerances of the wire frame edges.
                         The initial object is preserved. If the initial frame is decomposed, all parts
                         will be given as a result. Difference from #MakeCurves is that tolerances are
                         different in each edge vertex instead of the global one.\n \~
            \param[in] onlySmoothConnected - \ru Сегменты контуров должны быть состыкованы гладко (по G1).
                                             \en Contours segments must be smoothly connected (by G1). \~
            \param[out] contours - \ru Каркасы, полученные из frame.
                                   \en Frames obtained from 'frame'. \~
            \result \ru Возвращает true, если размер массива контуров увеличился.
                    \en Returns true if contours array size is increased. \~
          */
          bool CreateContours ( c3d::SpaceCurvesSPtrVector & contours, bool onlySmoothConnected ) const;

          /** \brief \ru Создать связные контуры с учётом толерантностей в вершинах рёбер каркаса.
                     \en Create connected contours according to vertex tolerances of the wire frame edges. \~
            \details \ru Создать связные контуры с учётом толерантностей в вершинах рёбер каркаса
                         с сохранением исходного объекта. Если исходный каркас распадается на части,
                         то все части выдаются в качестве результата. Отличается от #MakeCurves тем, что
                         толерантности разные в вершинах рёбер.\n
                     \en Create connected contours according to vertex tolerances of the wire frame edges.
                         The initial object is preserved. If the initial frame is decomposed, all parts
                         will be given as a result. Difference from #MakeCurves is that tolerances are
                         different in each edge vertex instead of the global one.\n \~
            \param[in] onlySmoothConnected - \ru Сегменты контуров должны быть состыкованы гладко (по G1).
                                             \en Contours segments must be smoothly connected (by G1). \~
            \param[out] contours - \ru Каркасы, полученные из frame.
                                   \en Frames obtained from 'frame'. \~
            \result \ru Возвращает true, если размер массива контуров увеличился.
                    \en Returns true if contours array size is increased. \~
          */
          bool CreateContours ( c3d::WireFramesSPtrVector & contours, bool onlySmoothConnected ) const;

          /// \ru Установить заданный флаг измененности для всех рёбер и вершин. \en Set flag of changes for all edges and vertices. 
          void    SetOwnChangedThrough( MbeChangedType );

          /// Выдать центр тяжести каркаса. \en Get the frame's center of mass.
          void GetWeightCentre( MbCartPoint3D & pc ) const;

protected:
  /** \ru \name Функции для работы с проволочным каркасом как с кривой.
              Считается, что каркас имеет параметр, который изменяется от нуля до суммы параметрических длин всех рёбер.
              По параметру можно вычислить радиус-вектор точки ребра каркаса и производные радиуса-вектора по аналогии с кривой.
              При увеличении параметра вычисляемая точка каркаса пробегает последовательно по всем рёбрам в порядке их следования.
              Разрывы между рёбрами каркаса не учитываются. 
      \en \name Functions for working with the wire frame as with a curve.
              It is assumed that the frame has a parameter that varies from zero to the sum of the parametric lengths of all edges.
              The parameter can be used to calculate the radius vector of the edge point of the frame and the derivatives of the radius vector by analogy with the curve.
              When the parameter is increased, the calculated frame point runs sequentially through all the edges in the order of their sequence.
              Gaps between the edges of the wire frame are not taken into account.
      \{ */

          double  GetTMin() const; // \ru Минимальное значение параметра. \en Minimum value of the the wire frame parameter. 
          double  GetTMax() const; // \ru Максимальное значение параметра. \en Maximum value of the the wire frame parameter. 

          ptrdiff_t FindSegment( double & t, double & tSeg ) const; // Найти индекс ребра каркаса по параметру. \en Find an index of the wire frame edge by paramneter. \~

          void    PointOn  ( double & t, MbCartPoint3D & ) const; // \ru Точка на проволочном каркасе. \en Point on the wire frame.
          void    FirstDer ( double & t, MbVector3D & ) const;    // \ru Первая производная. \en First derivative. 
          void    SecondDer( double & t, MbVector3D & ) const;    // \ru Вторая производная. \en Second derivative. 
          void    ThirdDer ( double & t, MbVector3D & ) const;    // \ru Третья производная. \en Third derivative. 
          void    Normal   ( double  & t, MbVector3D & ) const;   // \ru Вектор главной нормали каркаса. \en Main normal vector. 

          double  Step         ( double t, double sag ) const; // \ru Вычисление шага по стрелке прогиба. \en Calculation of parameter step by the sag.
          double  DeviationStep( double t, double angle ) const; // \ru Вычисление шага по углу отклонения нормали. \en Calculation of parameter step by the deviation angle.   
          double  MetricStep   ( double t, double length ) const; // \ru Вычисление шага параметра по длине. \en Calculation of parameter step by the given length. 

  const   MbCurve3D * GetSegment( size_t ind ) const; // \ru Выдать сегмент контура по индексу. \en Get wire frame curve by the index.
          MbCurve3D * SetSegment( size_t ind );       // \ru Выдать сегмент контура по индексу. \en Get wire frame curve by the index. 
          bool    IsCurveSameSense( size_t ind ); // \ru Выдать направление кривой ребра по индексу. \en Get wire frame curve sense by the index. 

  /** \} */

private:
          /// \ru Связано ли ребро с каким-либо ребром каркаса. \en Whether an edge is connected with another edge of a frame. 
          bool    IsConnectedWith( const MbEdge & );
          /// \ru Нормализовать ребро (выставить общую вершину замкнутого ребра) \en Normalize an edge (set the common vertex af a closed edge) 
          void    NormalizeEdge( MbEdge & );

DECLARE_PERSISTENT_CLASS_NEW_DEL( MbWireFrame )
OBVIOUS_PRIVATE_COPY( MbWireFrame )
};

IMPL_PERSISTENT_OPS( MbWireFrame )


//------------------------------------------------------------------------------
// \ru Положить в массив оригиналы кривых. \en Put originals of curves into an array. 
// ---
template <class CurvesVector>
void MbWireFrame::GetCurves( CurvesVector & curves ) const
{
  size_t edgesCnt = edges.size();
  curves.reserve( curves.size() + edgesCnt );
  c3d::SpaceCurveSPtr curve;
  for ( size_t k = 0; k < edgesCnt; ++k ) {
    const MbEdge * edge = edges[k];
    if ( edge != nullptr ) {
      curve = const_cast<MbCurve3D *>( &edge->GetCurve() );
      curves.push_back( curve );
      ::DetachItem( curve );
    }
  }
}


//------------------------------------------------------------------------------
// \ru Добавить массив ребер (оригиналы, не копии). \en Add an array of edges (originals, not copies). 
// ---
template <class WireEdgesVector>
void MbWireFrame::AddEdges( const WireEdgesVector & items, bool same )
{
  bool add = false;
  MbRegDuplicate * iReg = nullptr;
  MbAutoRegDuplicate autoReg( iReg );

  size_t addCnt = items.size();
  edges.reserve( edges.size() + addCnt );

  c3d::WireEdgeSPtr edge;
  for ( size_t k = 0; k < addCnt; ++k ) {
    const MbEdge * item = items[k];

    if ( item != nullptr ) {
      edge = same ? const_cast<MbEdge *>(item) : item->DataDuplicate( iReg );
      edge->AddRef();
      edges.push_back( edge );
      add = true;
    }
  }
  if ( add ) {
    normal = false;
    AttributesChange();
  }
}


//------------------------------------------------------------------------------
/** \brief \ru Забрать кривые.
           \en Extract curves. \~
  \details \ru Забрать кривые, удалив их из каркаса. \n
           \en Extract curves, removing from wireframe. \n \~
  \param[in] wireFrame - \ru Каркас.
                         \en A frame. \~
  \param[out] curves   - \ru Кривые, полученные из каркаса.
                         \en Curves obtained from the frame. \~
  \ingroup Curve3D_Modeling
*/ // ---
template <class CurvesVector>
void ExtractCurves( c3d::WireFrameSPtr & wireFrame, CurvesVector & curves )
{
  if ( wireFrame != nullptr ) {
    c3d::SpaceCurvesSPtrVector wireCurves;
    wireFrame->GetCurves( wireCurves );
    wireFrame->DeleteEdges();

    size_t wireCurvesCnt = wireCurves.size();

    if ( wireCurvesCnt > 0 ) {
      curves.reserve( curves.size() + wireCurvesCnt );
      for ( size_t k = 0; k < wireCurvesCnt; ++k ) {
        if ( wireCurves[k] != nullptr ) {
          curves.push_back( wireCurves[k] );
          ::DetachItem( wireCurves[k] );
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
/** \brief \ru Забрать кривые и удалить каркас, если он не используется.
           \en Take curves and delete a frame if it is not used. \~
  \details \ru Забрать кривые и удалить каркас, если он не используется. \n
           \en Take curves and delete a frame if it is not used. \n \~
  \param[in] wireFrame - \ru Каркас, подлежащий удалению.
                         \en A frame to delete. \~
  \param[out] curves   - \ru Кривые, полученные из каркаса.
                         \en Curves obtained from the frame. \~
  \ingroup Curve3D_Modeling
*/ // ---
template <class CurvesVector>
void ExtractCurvesDeleteFrame( MbWireFrame *& wireFrame, CurvesVector & curves )
{
  if ( wireFrame != nullptr ) {
    c3d::SpaceCurvesSPtrVector wireCurves;
    wireFrame->GetCurves( wireCurves );
    ::DeleteItem( wireFrame );

    size_t wireCurvesCnt = wireCurves.size();

    if ( wireCurvesCnt > 0 ) {
      curves.reserve( curves.size() + wireCurvesCnt );
      for ( size_t k = 0; k < wireCurvesCnt; ++k ) {
        if ( wireCurves[k] != nullptr ) {
          curves.push_back( wireCurves[k] );
          ::DetachItem( wireCurves[k] );
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
/** \brief \ru Забрать первую кривую и удалить каркас, если он пуст и не используется.
           \en Take the first curve and delete a frame if it is empty and not used. \~
  \details \ru Забрать первую кривую и удалить каркас, если он пуст и не используется. \n
           \en Take the first curve and delete a frame if it is empty and not used. \n \~
  \param[in] wireFrame - \ru Каркас, подлежащий удалению.
                         \en A frame to delete. \~
  \param[out] curve    - \ru Кривая, полученная из каркаса.
                         \en A curve obtained from the frame. \~
  \ingroup Curve3D_Modeling
*/ // ---
inline
void ExtractCurveDeleteFrame( MbWireFrame *& wireFrame, MbCurve3D *& curve )
{
  if ( wireFrame != nullptr ) {
    c3d::WireEdgeSPtr edge( wireFrame->DetachEdge( 0 ) ); // \ru Отцепить объект \en Detach an object 
    ::DeleteItem( wireFrame );

    if ( edge != nullptr ) {
      curve = &edge->SetCurve();
      ::AddRefItem( curve );
      edge = nullptr;
      ::DecRefItem( curve );
    }
  }
}


//------------------------------------------------------------------------------
/** \brief \ru Создать каркас по множеству кривых.
           \en Create a frame by a set of curves. \~
  \details \ru Создать или обновить каркас по множеству кривых. \n
           \en Create or update a frame by a set of curves. \n \~
  \param[out] result - \ru Каркас, подлежащий замене или построению.
                       \en A frame to replace or construct. \~
  \param[in] curves  - \ru Кривые для построения каркаса.
                       \en Curves for the frame construction. \~
  \param[in] snMaker - \ru Именователь кривых каркаса.
                       \en An object defining the frame curves names. \~
  \param[in] creator - \ru Строитель каркаса.
                       \en A creator of a frame. \~
  \result \ru Возвращает true, если присланный каркас обновился, или был создан новый при отсутствии каркаса на входе.
          \en Returns true if the sent frame has been updated or if the new frame has been created without a frame in the input. \~
  \ingroup Curve3D_Modeling
*/ // ---
MATH_FUNC (bool) CreateWireFrame(       MbWireFrame *&       result, 
                                  const RPArray<MbCurve3D> & curves, 
                                  const MbSNameMaker &       snMaker,
                                  const MbCreator *          creator = nullptr );


//------------------------------------------------------------------------------
/** \brief \ru Создать каркас по кривой.
           \en Create a frame by a curve. \~
  \details \ru Создать или обновить каркас по кривой. \n
           \en Create or update a frame by a curve. \n \~
  \param[out] result - \ru Каркас, подлежащий замене или построению.
                       \en A frame to replace or construct. \~
  \param[in] curve   - \ru Кривая для построения каркаса.
                       \en A curve for the frame construction. \~
  \param[in] snMaker - \ru Именователь кривых каркаса.
                       \en An object defining the frame curves names. \~
  \param[in] creator - \ru Строитель каркаса.
                       \en A creator of a frame. \~
  \result \ru Возвращает true, если присланный каркас обновился, или был создан новый при отсутствии каркаса на входе.
          \en Returns true if the sent frame has been updated or if the new frame has been created without a frame in the input. \~
  \ingroup Curve3D_Modeling
*/ // ---
MATH_FUNC (bool) CreateWireFrame(       MbWireFrame *& result, 
                                  const MbCurve3D &    curve, 
                                  const MbSNameMaker & snMaker,
                                  const MbCreator *    creator = nullptr );


//------------------------------------------------------------------------------
/** \brief \ru Создать каркас по кривой.
           \en Create a frame by a curve. \~
  \details \ru Создать или обновить каркас по кривой. \n
           \en Create or update a frame by a curve. \n \~
  \param[in] curve   - \ru Кривая для построения каркаса.
                       \en A curve for the frame construction. \~
  \param[in] snMaker - \ru Именователь кривых каркаса.
                       \en An object defining the frame curves names. \~
  \param[out] result - \ru Каркас, подлежащий замене или построению.
                       \en A frame to replace or construct. \~
  \param[in] creator - \ru Строитель каркаса.
                       \en A creator of a frame. \~
  \result \ru Возвращает true, если присланный каркас обновился, или был создан новый при отсутствии каркаса на входе.
          \en Returns true if the sent frame has been updated or if the new frame has been created without a frame in the input. \~
  \ingroup Curve3D_Modeling
*/ // ---
MATH_FUNC (bool) CreateWireFrame( const MbCurve3D &          curve, 
                                  const MbSNameMaker &       snMaker,
                                        c3d::WireFrameSPtr & result, 
                                  const c3d::CreatorSPtr &   creator = nullptr );


//------------------------------------------------------------------------------
/** \brief \ru Создать каркас по множеству кривых.
           \en Create a frame by a set of curves. \~
  \details \ru Создать или обновить каркас по множеству кривых. \n
           \en Create or update a frame by a set of curves. \n \~
  \param[in] curves  - \ru Кривые для построения каркаса.
                       \en Curves for the frame construction. \~
  \param[in] snMaker - \ru Именователь кривых каркаса.
                       \en An object defining the frame curves names. \~
  \param[out] result - \ru Каркас, подлежащий замене или построению.
                       \en A frame to replace or construct. \~
  \param[in] creator - \ru Строитель каркаса.
                       \en A creator of a frame. \~
  \result \ru Возвращает true, если присланный каркас обновился, или был создан новый при отсутствии каркаса на входе.
          \en Returns true if the sent frame has been updated or if the new frame has been created without a frame in the input. \~
  \ingroup Curve3D_Modeling
*/ // ---
MATH_FUNC( bool ) CreateWireFrame( const c3d::SpaceCurvesSPtrVector & curves,
                                   const MbSNameMaker &               snMaker,
                                         c3d::WireFrameSPtr &         result,
                                   const c3d::CreatorSPtr &           creator = nullptr );

#endif // __WIRE_FRAME_H
