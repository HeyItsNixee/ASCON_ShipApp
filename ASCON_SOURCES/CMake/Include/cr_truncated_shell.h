////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Построение усеченной оболочки.
         \en Construction of a truncated shell. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __TRUNCATED_SHELL_H
#define __TRUNCATED_SHELL_H


#include <creator.h>
#include <cr_split_data.h>
#include <math_define.h>
#include <mb_enum.h>
#include <mb_operation_result.h>


class  MATH_CLASS  MbCurve;
class  MATH_CLASS  MbAxis3D;
class  MATH_CLASS  MbSpaceItem;
class  MATH_CLASS  MbPlacement3D;
class  MATH_CLASS  MbCurve3D;
class  MATH_CLASS  MbFace;
class  MATH_CLASS  MbSolid;
class  MATH_CLASS  MbSNameMaker;
struct MATH_CLASS  MbMergingFlags;


//------------------------------------------------------------------------------
/** \brief \ru Строитель усеченной оболочки.
           \en Constructor of a truncated shell. \~
  \details \ru Строитель усеченной оболочки режет исходную оболочку на части указанными элементами, 
    которыми могут служить двумерные кривые в локальной системе координат, трёхмерные кривые, поверхности и оболочки. \n
           \en Constructor of a truncated shell cuts the initial shell into parts by the specified elements 
    which can be two-dimensional curves in the local coordinate system, three-dimensional curves, surfaces and shells. \n \~ 
  \ingroup Model_Creators
*/
// ---
class MATH_CLASS MbTruncatedShell : public MbCreator {
private :
  std::vector<MbItemIndex>  selIndices; ///< \ru Идентификаторы выбранных граней усекаемой оболочки. \en Identifiers of selected faces of the shell being truncated. 
  MbSplitData               splitItems; ///< \ru Усекающие элементы c ориентациями. \en Truncating elements with orientations. 
  c3d::BoolVector           orients;    ///< \ru Ориентация усекающих элементов. \en Orientation of truncating elements. 
  bool                      mergeFaces; ///< \ru Сливать подобные грани (true). \en Whether to merge similar faces (true). 
  bool                      mergeEdges; ///< \ru Сливать подобные ребра (true). \en Whether to merge similar edges (true). 

public:
  /// \ru Конструктор по двумерным кривым. \en Constructor by two-dimensional curves. 
  MbTruncatedShell( const MbPlacement3D &, const RPArray<MbContour> &, bool same, 
                    const c3d::BoolVector & orients, const MbMergingFlags &, const MbSNameMaker & );
  /// \ru Конструктор по трехмерным кривым. \en Constructor by three-dimensional curves. 
  MbTruncatedShell( const RPArray<MbCurve3D> &, bool same, 
                    const c3d::BoolVector & orients, const MbMergingFlags &, const MbSNameMaker & );
  /// \ru Конструктор по поверхностям. \en Constructor by surfaces. 
  MbTruncatedShell( const RPArray<MbSurface> &, bool same, 
                    const c3d::BoolVector & orients, const MbMergingFlags &, const MbSNameMaker & );
  /// \ru Конструктор по строителям тела. \en Constructor by solid creators. 
  MbTruncatedShell( const MbSolid &, bool same, bool keepShell,
                    bool orient, const MbMergingFlags &, const MbSNameMaker & );

  virtual ~MbTruncatedShell();

private:
  MbTruncatedShell( const MbTruncatedShell &, MbRegDuplicate * );

public:
  MbeCreatorType  IsA()  const override; // \ru Тип элемента \en Type of element 
  MbCreator & Duplicate( MbRegDuplicate * = nullptr ) const override; // \ru Сделать копию \en Create a copy

  void        Transform( const MbMatrix3D &, MbRegTransform * = nullptr ) override; // \ru Преобразовать элемент согласно матрице \en Transform element according to the matrix
  void        Move     ( const MbVector3D &, MbRegTransform * = nullptr ) override; // \ru Сдвиг \en Translation
  void        Rotate   ( const MbAxis3D &, double angle, MbRegTransform * = nullptr ) override; // \ru Повернуть вокруг оси \en Rotate about an axis

  void        GetProperties( MbProperties & properties ) override; // \ru Выдать свойства объекта \en Get properties of the object 
  void        SetProperties( const MbProperties & properties ) override; // \ru Записать свойства объекта \en Set properties of the object 
  MbePrompt   GetPropertyName() override;                        // \ru Выдать заголовок свойства объекта \en Get a name of object property 
  void        GetBasisItems ( RPArray<MbSpaceItem>   & ) override; // \ru Дать базовые объекты \en Get the base objects
  void        GetBasisPoints( MbControlData3D & ) const override; // \ru Выдать контрольные точки объекта. \en Get control points of object.
  void        SetBasisPoints( const MbControlData3D & ) override; // \ru Изменить объект по контрольным точкам. \en Change the object by control points.
  size_t      GetCreatorsCount( MbeCreatorType ct ) const override; // \ru Посчитать внутренние построители по типу. \en Count internal creators by type.
  bool        GetInternalCreators( MbeCreatorType, c3d::ConstCreatorsSPtrVector & ) const override; // \ru Получить внутренние построители по типу. \en Get internal creators by type.
  bool        SetInternalCreators( MbeCreatorType, c3d::CreatorsSPtrVector & ) override;            // \ru Получить внутренние построители по типу. \en Get internal creators by type.

  bool        IsSame( const MbCreator &, double accuracy ) const override; // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool        IsSimilar ( const MbCreator & ) const override; // \ru Являются ли объекты подобными \en Whether the objects are similar
  bool        SetEqual( const MbCreator & ) override;         // \ru Сделать равным \en Make equal

  // \ru Построение оболочки по исходным данным \en Construction of a shell from the given data 
  bool        CreateShell( MbFaceShell *& shell, MbeCopyMode sameShell, 
                           RPArray<MbSpaceItem> * items = nullptr ) override;

  // \ru Установить номера выбраных граней усекаемого тела \en Set indices of selected faces of the solid being truncated. 
  void        SetSelIndices( const std::vector<MbItemIndex> & selInds );

  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbTruncatedShell )
  OBVIOUS_PRIVATE_COPY( MbTruncatedShell )
}; // MbTruncatedShell

IMPL_PERSISTENT_OPS( MbTruncatedShell )


//------------------------------------------------------------------------------
/** \brief \ru Построить усечённую оболочку.
           \en Build a truncated shell. \~
  \details \ru Построить усечённую оболочку резкой исходного тела на части указанными элементами, 
    которыми могут служить двумерные кривые в локальной системе координат, трёхмерные кривые, поверхности и оболочки.
    Одновременно с построением оболочки функция создаёт её строитель.\n
           \en Construct a truncated shell by cutting the initial solid into parts by the specified elements 
    which can be two-dimensional curves in the local coordinate system, three-dimensional curves, surfaces and shells.
    The function simultaneously creates the shell and its constructor.\n \~ 
  \param[in]  initSolid - \ru Исходное тело.
                          \en The initial solid. \~
  \param[in]  selIndices - \ru Идентификаторы выбранныых граней, приотсутствии - всё тело.
                           \en Identifiers of selected faces, if not specified - the whole solid. \~
  \param[in]  sameShell  - \ru Режим копирования исходной оболочки.
                           \en Mode of copying the initial shell. \~
  \param[in]  operNames - \ru Именователь граней.
                          \en An object for naming faces. \~
  \param[in]  items - \ru Усекающие объекты.
                      \en Truncating objects. \~
  \param[in]  orients - \ru Ориентация усекающих объектов.
                        \en The truncating objects orientation. \~
  \param[in]  curvesSplitMode - \ru Кривые используются как линии разъема.
                                \en The curves are used as parting lines. \~
  \param[in]  solidsCopyMode - \ru Режим копирования усекающих объектов.
                               \en Mode of copying the truncating objects. \~
  \param[in] mergeFlags - \ru Флаги слияния элементов оболочки.
                          \en Control flags of shell items merging. \~
  \param[out] res - \ru Код результата операции.
                    \en Operation result code. \~
  \param[out] resShell - \ru Построенная усеченная оболочка.
                         \en Constructed truncated shell. \~
  \param[out] resDir - \ru Направление фантома усечения.
                       \en Direction of truncation phantom. \~
  \result \ru Возвращает строитель оболочки.
          \en Returns the shell constructor. \~
  \ingroup Model_Creators
*/
// ---
MATH_FUNC (MbCreator *) TruncateSurfacesSol(       MbSolid &              initSolid,
                                                   SArray<size_t> &       selIndices,
                                                   MbeCopyMode            sameShell,
                                             const MbSNameMaker &         operNames,
                                                   RPArray<MbSpaceItem> & items,
                                                   c3d::BoolVector &      orients,
                                                   bool                   curvesSplitMode,
                                                   MbeCopyMode            solidsCopyMode,
                                             const MbMergingFlags &       mergeFlags,       // флаги слияния граней и ребер
                                                   MbResultType &         res,
                                                   MbFaceShell *&         resShell,
                                                   MbPlacement3D *&       resDir );


#endif // __TRUNCATED_SHELL_H
