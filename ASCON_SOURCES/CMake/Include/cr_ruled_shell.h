////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Построить линейчатую оболочку.
         \en Construct a ruled shell. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __CR_RULED_SHELL_H
#define __CR_RULED_SHELL_H


#include <math_define.h>
#include <mb_operation_result.h>
#include <name_item.h>
#include <creator.h>
#include <op_shell_parameter.h>


class  MATH_CLASS MbFaceShell;
class  MATH_CLASS MbCurveEdge;
class  MATH_CLASS MbOrientedEdge;
class  MATH_CLASS MbLoop;
struct MATH_CLASS RuledSurfaceValues; 


//------------------------------------------------------------------------------
/** \brief \ru Строитель линейчатой оболочки.
           \en Constructor of a ruled shell. \~
  \details \ru Строитель линейчатой оболочки по двум кривым. \n
           \en Constructor of a ruled shell from two curves. \n \~ 
  \ingroup Model_Creators
*/
//---
class MATH_CLASS MbRuledShell : public MbCreator {

private : 
  RuledSurfaceValues parameters; ///< \ru Параметры построения. \en Construction parameters. 
private:
  MbRuledShell( const MbRuledShell & obj, MbRegDuplicate * ireg );
public:
  /// \ru Конструктор по параметрам операции и именователю. \en Constructor by operation parameters and name-maker. 
  MbRuledShell( const RuledSurfaceValues & pars, const MbSNameMaker & n );
  virtual ~MbRuledShell();

public: // \ru Общие функции математического объекта \en Common functions of the mathematical object 
  MbeCreatorType  IsA() const override; ///< \ru Тип элемента \en Element type 
  MbCreator & Duplicate( MbRegDuplicate * = nullptr ) const override; ///< \ru Сделать копию \en Make a copy
  bool        IsSame( const MbCreator &, double accuracy ) const override; // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool        SetEqual ( const MbCreator & ) override; ///< \ru Сделать равным \en Make equal 
  void        Transform( const MbMatrix3D &, MbRegTransform * = nullptr ) override; ///< \ru Преобразовать по матрице \en Transform according to the matrix
  void        Move     ( const MbVector3D &, MbRegTransform * = nullptr ) override; ///< \ru Сдвиг по вектору \en Translation by the vector
  void        Rotate   ( const MbAxis3D &, double angle, MbRegTransform * = nullptr ) override; ///< \ru Поворот вокруг оси \en Rotation about an axis

  void        GetProperties( MbProperties & properties ) override; ///< \ru Выдать свойства объекта \en Get properties of the object 
  void        SetProperties( const MbProperties & properties ) override; ///< \ru Записать свойства объекта \en Write properties of the object 
  MbePrompt   GetPropertyName() override; ///< \ru Выдать заголовок свойства объекта \en Get name of object property 
  void        GetBasisItems ( RPArray<MbSpaceItem> & ) override; // \ru Дать базовые объекты \en Get the basis objects 
  void        GetBasisPoints( MbControlData3D & ) const override; // \ru Выдать контрольные точки объекта. \en Get control points of object.
  void        SetBasisPoints( const MbControlData3D & ) override; // \ru Изменить объект по контрольным точкам. \en Change the object by control points.

public: 
  /// \ru Построение оболочки \en Creation of a shell 
  bool        CreateShell( MbFaceShell *& shell, MbeCopyMode sameShell, 
                           RPArray<MbSpaceItem> * items = nullptr ) override;

  // \ru Дать параметры. \en Get the parameters. 
  void        GetParameters(       RuledSurfaceValues & params ) const;
  // \ru Установить параметры. \en Set the parameters. 
  void        SetParameters( const RuledSurfaceValues & params );

  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbRuledShell )
  OBVIOUS_PRIVATE_COPY( MbRuledShell )
};  // MbRuledShell

IMPL_PERSISTENT_OPS( MbRuledShell )


//------------------------------------------------------------------------------
/** \brief \ru Построить линейчатую оболочку.
           \en Construct a ruled shell. \~
  \details \ru Построить линейчатую оболочку по двум кривым.
               Кривые могут быть составными. 
               Одновременно с построением оболочки функция создаёт её строитель.\n
           \en Construct a ruled shell from two curves
               Curves can be composite. 
               The function simultaneously creates the shell and its constructor.\n \~ 
  \param[in,out]  parameters - \ru Параметры операции.
                           \en The operation parameters. \~
  \param[in]  operNames - \ru Именователь.
                          \en An object for naming the new objects. \~
  \param[in]  isPhantom - \ru Режим создания фантома.
                          \en Create in the phantom mode. \~
  \param[out] res - \ru Код результата операции.
                    \en Operation result code. \~
  \param[out] shell - \ru Построенная оболочка.
                      \en The resultant shell. \~
  \result \ru Возвращает строитель оболочки.
          \en Returns the shell constructor. \~
  \deprecated \ru Функция устарела, взамен использовать #CreateRuledShell с набором параметров #MbRuledShellParams.
              \en The function is deprecated, instead use #CreateRuledShell with the parameter list #MbRuledShellParams. \~
  \ingroup Model_Creators
*/
// ---
//DEPRECATE_DECLARE_REPLACE( CreateRuledShell with MbRuledShellParams )
MATH_FUNC (MbCreator *) CreateRuledShell(       RuledSurfaceValues & parameters, 
                                          const MbSNameMaker       & operNames, 
                                                bool                 isPhantom, 
                                                MbResultType       & res, 
                                                MbFaceShell       *& shell );


//------------------------------------------------------------------------------
/** \brief \ru Построить линейчатую оболочку.
           \en Construct a ruled shell. \~
  \details \ru Построить линейчатую оболочку по двум кривым.
               Кривые могут быть составными. 
               Одновременно с построением оболочки функция создаёт её строитель.\n
           \en Construct a ruled shell from two curves
               Curves can be composite. 
               The function simultaneously creates the shell and its constructor.\n \~ 
  \param[in] ruledParams - \ru Параметры для построения линейчатого незамкнутого тела.
                           \en The operation parameters for the creation an open ruled solid. \~
  \param[out]  outParams - \ru Выходные параметры операции.
                            \en Output operation parameters. \~
  \param[out] res - \ru Код результата операции.
                    \en Operation result code. \~
  \param[out] shell - \ru Результирующая оболочка.
                      \en Result shell. \~
  \result \ru Возвращает строитель оболочки.
          \en Returns the shell constructor. \~
  \ingroup Model_Creators
*/
// ---
MATH_FUNC (MbCreator *) CreateRuledShell( const MbRuledShellParams & ruledParams,
                                                MbRuledShellResult & outParams,
                                                MbResultType &       res,
                                                c3d::ShellSPtr &     shell );


#endif // __CR_RULED_SHELL_H
