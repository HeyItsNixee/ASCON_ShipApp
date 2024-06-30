////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Строитель оболочки на поверхности переменного сечения.
         \en Constructor of shell on mutable section surface. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __CR_SECTION_SHELL_H
#define __CR_SECTION_SHELL_H


#include <creator.h>
#include <curve3d.h>
#include <name_item.h>
#include <op_swept_parameter.h>


class  MATH_CLASS  MbFaceShell;


//------------------------------------------------------------------------------
/** \brief \ru Строитель оболочки на поверхности переменного сечения.
           \en Constructor of the shell on mutable section surface. \~
  \details \ru Грань оболочки строится путём движения переменного сечения по опорной кривой. \n
           \en Constructor of face by moving generating curve along a reference spine curve. \n \~
  \ingroup Model_Creators
*/
// ---
class MATH_CLASS MbSectionShell : public MbCreator {
protected :
  MbSectionData sectionData; ///< \ru Данные о поверхности переменного сечения. \en Data about mutable section surface. 

  /** \brief \ru Конструктор.
             \en Constructor. \~
    \param[in] data  - \ru Данные о поверхности переменного сечения. 
                       \en Data about mutable section surface.  \~
    \param[in] names - \ru Именователь грани оболочки.
                       \en Generating face names. \~
  */
  MbSectionShell( const MbSectionData & data );
private :
  MbSectionShell( const MbSectionShell & init, MbRegDuplicate * ireg );
  // \ru Объявление конструктора копирования без реализации, чтобы не было копирования по умолчанию. \en Declaration without implementation of the copy-constructor to prevent copying by default. 
  MbSectionShell( const MbSectionShell & ); 
public :
  virtual ~MbSectionShell();

  /** \ru \name Общие функции математического объекта.
      \en \name Common functions of the mathematical object.
      \{ */
  MbeCreatorType  IsA() const override; // \ru Тип элемента \en A type of element 
  MbCreator & Duplicate( MbRegDuplicate * = nullptr ) const override; // \ru Сделать копию \en Create a copy
  void        Transform( const MbMatrix3D &, MbRegTransform * = nullptr ) override;             // \ru Преобразовать элемент согласно матрице \en Transform element according to the matrix
  void        Move     ( const MbVector3D &, MbRegTransform * = nullptr ) override;             // \ru Сдвиг \en Translation
  void        Rotate   ( const MbAxis3D &, double angle, MbRegTransform * = nullptr ) override; // \ru Повернуть вокруг оси \en Rotate around an axis

  void        GetProperties( MbProperties & ) override; // \ru Выдать свойства объекта \en Get properties of the object 
  void        SetProperties( const MbProperties & ) override; // \ru Записать свойства объекта \en Set properties of the object 
  MbePrompt   GetPropertyName() override; // \ru Выдать заголовок свойства объекта \en Get a name of object property 
  void        GetBasisItems ( RPArray<MbSpaceItem>   & s ) override; // \ru Дать базовые объекты \en Get the base objects 
  void        GetBasisPoints( MbControlData3D & ) const override; // \ru Выдать контрольные точки объекта. \en Get control points of object.
  void        SetBasisPoints( const MbControlData3D & ) override; // \ru Изменить объект по контрольным точкам. \en Change the object by control points.

  bool        IsSame( const MbCreator &, double accuracy ) const override; // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool        IsSimilar( const MbCreator & ) const override; // \ru Являются ли объекты подобными \en Whether the objects are similar 
  bool        SetEqual ( const MbCreator & ) override;       // \ru Сделать равным \en Make equal 
  /** \} */

  /** \ru \name Общие функции твердого тела (формообразующей операции).
      \en \name Common functions of the rigid solid (forming operations).
      \{ */
  bool        CreateShell( MbFaceShell *& shell, MbeCopyMode sameShell, 
                           RPArray<MbSpaceItem> * items = nullptr ) override; // \ru Построение \en Construction

  virtual void        SetYourVersion( VERSION version );
  /** \} */

  /** \ru \name Функции строителя оболочки на поверхности переменного сечения.
      \en \name Functions of creator of evolution solid shell.
      \{ */
          /// \ru Дать параметры. \en Get the parameters. 
  const MbSectionData & GetSectionData() { return sectionData; }
  /// \ru Установить параметры. \en Set the parameters. 
  void        SetSectionData( const MbSectionData & data ) { sectionData = data; }
  /** \} */

  /** \brief \ru Создать оболочку на поверхности переменного сечения.
             \en Create a shell on mutable section surface. \~
    \details \ru Построить оболочку путём движения образующей кривой по направляющей кривой
      и выполнить булеву операцию с оболочкой, если последняя задана. \n
      Одновременно с построением оболочки функция создаёт её строитель.\n
             \en Create a shell by moving the generating curve along the spine curve
      and perform the Boolean operation with the shell if it is specified. \n
      The function simultaneously creates the shell and its constructor.\n \~ 
    \param[in]  solid     - \ru Набор граней, к которым дополняется построение.
                            \en Face set the construction is complemented with respect to. \~
    \param[in]  sameShell - \ru Способ копирования граней.
                            \en The method of copying faces. \~
    \param[in] data       - \ru Данные о поверхности переменного сечения. 
                            \en Data about mutable section surface.  \~
    \param[in] names      - \ru Именователь грани оболочки.
                            \en Generating face names. \~
    \param[out] res       - \ru Код результата операции.
                            \en Operation result code. \~
    \param[out] shell     - \ru Код ошибки порстроения.
                            \en Result code of building. \~
    \result \ru Возвращает строитель.
            \en Returns the constructor of operation. \~
  */
  static  
  c3d::CreatorSPtr    Create(       c3d::ShellSPtr & solid,
                                    MbeCopyMode      sameShell,
                              const MbSectionData &  data,
                                    MbResultType &   res,
                                    c3d::ShellSPtr & shell );

private :
  // \ru Объявление оператора присваивания без реализации, чтобы не было присваивания по умолчанию. \en The declaration of the assignment operator without implementation to prevent an assignment by default. 
  void        operator = ( const MbSectionShell & ); 

  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbSectionShell )

}; // MbSectionShell

IMPL_PERSISTENT_OPS( MbSectionShell )


#endif // __CR_SECTION_SHELL_H
