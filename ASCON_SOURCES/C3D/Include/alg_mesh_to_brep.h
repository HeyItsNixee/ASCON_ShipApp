////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief Функции преобразования полигональной модели в граничное представление.

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __ALG_MESH_TO_BREP_H
#define __ALG_MESH_TO_BREP_H

#include <math_define.h>
#include <mb_variables.h>
#include <mb_operation_result.h>
#include <mesh_triangle.h>
#include <templ_sptr.h>
#include <topology.h>
#include <vector>
#include <utility>


class  MATH_CLASS MbCartPoint3D;
class  MATH_CLASS MbVector3D;
class  MATH_CLASS MbFaceShell;
class  MATH_CLASS MbMesh;
class  MATH_CLASS MbGrid;
class  MATH_CLASS MbCollection;
class  MATH_CLASS MbSNameMaker;
class  MATH_CLASS IProgressIndicator;
class  MATH_CLASS ProgressBarWrapper;
class  MATH_CLASS MbGridShellParams;


//------------------------------------------------------------------------------
// Удалить дублирующие с заданной точностью друг друга точки.
// ---
bool RemoveRedundantPoints( c3d::SpacePointsVector &   points, 
                            c3d::MeshTrianglesVector & triangles,
                            double                     epsilon,
                            ProgressBarWrapper *       baseProgBar );

//------------------------------------------------------------------------------
// Удалить дублирующие с заданной точностью друг друга точки.
// ---
bool RemoveRedundantPoints( std::vector<c3d::SpacePointVector> & pointNormals, 
                            c3d::MeshTrianglesVector &           triangles,
                            double                               epsilon,
                            ProgressBarWrapper *                 baseProgBar );

//------------------------------------------------------------------------------
// Удалить дублирующие с заданной точностью друг друга точки.
// ---
bool RemoveRedundantPoints( c3d::SpacePointsVector & points, 
                            c3d::UintVector &        indexes,
                            double                   epsilon );


//------------------------------------------------------------------------------
// Удалить дублирующие с заданной точностью друг друга точки с помощью кд-дерева.
// ---
void MergeCoincidentPoints( c3d::SpacePointsVector &   points,
                            c3d::MeshTrianglesVector & triangles,
                            double                     epsilon );


//------------------------------------------------------------------------------
// Удалить дублирующие с заданной точностью друг друга точки с помощью кд-дерева.
// ---
void MergeCoincidentPoints( const c3d::SpacePointsVector &   points,
                                  c3d::SpacePointsVector &   pointsMerged,
                                  c3d::IndicesVector &       mapVertex,
                                  double                     epsilon );


//------------------------------------------------------------------------------
// Объединить ребра двух смежных плоских граней с полигональной границей
// (возвращает общее после сшивки ребро)
// ---
MbCurveEdge * StitchAdjacentGridsEdges( MbFace & face1, MbOrientedEdge & edge1,
                                        MbFace & face2, MbLoop & loop2, size_t e2Ind );

//---------------------------------------------------------s---------------------
// Обеспечить связность треугольных граней
// ---
bool ConnectTriangleFaces( const c3d::FacesSPtrVector &               faces,
                           const std::vector<c3d::IndicesPairsPair> & edgesPairs,
                                 std::vector<c3d::IndicesPairsPair> * combinedPairs,
                                 ProgressBarWrapper *                 baseProgBar );

//------------------------------------------------------------------------------
// Преобразовать триангуляцию в оболочку.
// ---
MbFaceShell * ConvertGridToShell( const MbGrid &             grid, 
                                  const MbGridShellParams &  params,
                                        MbResultType &       res, 
                                        IProgressIndicator * progBar = nullptr );


//------------------------------------------------------------------------------
// Преобразовать полигональную модель в оболочку.
// ---
MbFaceShell * ConvertMeshToShell( const MbMesh &             mesh, 
                                  const MbGridShellParams &  params,
                                        MbResultType &       res, 
                                        IProgressIndicator * progBar = nullptr );


//------------------------------------------------------------------------------
// Преобразовать триангуляцию в оболочку.
// ---
MbFaceShell * ConvertCollectionToShell( const MbCollection &       grid, 
                                              bool                 mergeFaces,
                                        const MbSNameMaker &       snMaker, 
                                              MbResultType &       res, 
                                              IProgressIndicator * progIndicator );


#endif // __ALG_UTILITES_H
