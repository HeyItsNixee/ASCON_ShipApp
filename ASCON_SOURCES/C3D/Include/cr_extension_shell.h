﻿////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Построение удлинённой грани оболочки.
         \en Construction of an extended face of a shell. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __CR_EXTENSION_SHELL_H
#define __CR_EXTENSION_SHELL_H


#include <creator.h>
#include <op_shell_parameter.h>
#include <math_define.h>


//------------------------------------------------------------------------------
/** \brief \ru Строитель удлинённой грани оболочки.
           \en Constructor of an extended face of a shell. \~
  \details \ru Строитель удлинённой грани оболочки. Удлинение может быть выполнено следующими способами.
    Может быть удлинена на заданное расстояние указанная грань.
    К указанной грани может быть добавлена гладко стыкующаяся с ней грань.
    К указанной грани может быть добавлена грань, полученная выдавливанием крайнего ребра в заданном направлении.
           \en Constructor of an extended face of a shell. Extension can be performed in the following ways:
    The specified faces can be extended on the given distance.
    A smoothly connected face can be added to the given face.
    A face obtained by extrusion of boundary edge in the given direction can be added to the specified face. \~
  \ingroup Model_Creators
*/
// ---
class MATH_CLASS MbExtensionShell : public MbCreator {
protected :
  std::vector<MbExtendedIndex> facesIndex; ///< \ru Идентификаторы удлиняемых граней в оболочке. \en Identifier of a shell faces to extend. 
  ExtensionValues              parameters; ///< \ru Параметры построения удлинённой оболочки. \en Parameters of the extended shell construction. 

public :
  MbExtensionShell( const std::vector<MbExtendedIndex> & fInd,
                    const ExtensionValues & p, const MbSNameMaker & n );
private :
  MbExtensionShell( const MbExtensionShell &, MbRegDuplicate * ireg ); 
public :
  virtual ~MbExtensionShell();

  // \ru Общие функции математического объекта \en Common functions of the mathematical object 

  MbeCreatorType  IsA() const override; // \ru Тип элемента \en Type of element 
  MbCreator & Duplicate( MbRegDuplicate * = nullptr ) const override; // \ru Сделать копию \en Create a copy
  void        Transform( const MbMatrix3D & matr, MbRegTransform * = nullptr ) override; // \ru Преобразовать элемент согласно матрице \en Transform element according to the matrix
  void        Move( const MbVector3D & to, MbRegTransform * = nullptr ) override;                 // \ru Сдвиг \en Translation
  void        Rotate( const MbAxis3D & axis, double angle, MbRegTransform * = nullptr ) override; // \ru Повернуть вокруг оси \en Rotate about an axis

  void        GetProperties( MbProperties & properties ) override; // \ru Выдать свойства объекта \en Get properties of the object 
  void        SetProperties( const MbProperties & properties ) override; // \ru Записать свойства объекта \en Set properties of the object 
  MbePrompt   GetPropertyName() override; // \ru Выдать заголовок свойства объекта \en Get a name of object property 
  
  bool        IsSame( const MbCreator &, double accuracy ) const override; // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool        IsSimilar( const MbCreator & ) const override; // \ru Являются ли объекты подобными \en Whether the objects are similar 
  bool        SetEqual ( const MbCreator & ) override;       // \ru Сделать равным \en Make equal 

  // \ru Общие функции твердого тела \en Common functions of solid solid 

  bool        CreateShell( MbFaceShell *& shell, MbeCopyMode sameShell, 
                           RPArray<MbSpaceItem> * items = nullptr ) override; // \ru Построение \en Construction

  // \ru Дать параметры. \en Get the parameters. 
  void        GetParameters(       ExtensionValues & params ) const { params = parameters; }
  // \ru Установить параметры. \en Set the parameters. 
  void        SetParameters( const ExtensionValues & params )       { parameters = params; }

  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbExtensionShell )
  OBVIOUS_PRIVATE_COPY( MbExtensionShell )
}; // MbExtensionShell

IMPL_PERSISTENT_OPS( MbExtensionShell )


//------------------------------------------------------------------------------
/** \brief \ru Построить удлинённую грань оболочки.
           \en Construct the extended face of a shell. \~
  \details \ru Построить удлинённую грань оболочки. Удлинение может быть выполнено следующими способами.
               Может быть удлинена на заданное расстояние указанная грань.
               К указанной грани может быть добавлена гладко стыкующаяся с ней грань.
               К указанной грани может быть добавлена грань, полученная выдавливанием крайнего ребра в заданном направлении.
               Одновременно с построением оболочки функция создаёт её строитель.\n
           \en Construct the extended face of a shell. An extension can be performed in the following ways:
               The specified faces can be extended on the given distance.
               A smoothly connected face can be added to the given face.
               A face obtained by extrusion of a boundary edge in the given direction can be added to the specified face.
               The function simultaneously creates the shell and its constructor.\n \~ 
  \param[in]  solid - \ru Исходная оболочка.
                      \en The initial shell. \~
  \param[in]  sameShell  - \ru Режим копирования исходной оболочки.
                           \en Mode of copying the initial shell. \~
  \param[in]  edges - \ru Множество краевых ребер, через которые выполняется продление.
                      \en An array of boundary edges through which to extend the face. \~
  \param[in]  parameters - \ru Параметры построения.
                           \en Parameters of a shell creation. \~
  \param[in]  operNames - \ru Именователь граней.
                          \en An object for naming faces. \~
  \param[out] res - \ru Код результата операции.
                    \en Operation result code. \~
  \param[out] shell - \ru Построенная оболочка.
                      \en The resultant shell. \~
  \result \ru Возвращает строитель оболочки.
          \en Returns the shell constructor. \~
  \deprecated \ru Функция устарела, взамен использовать #CreateExtensionShell с набором параметров #MbExtensionShellParams.
              \en The function is deprecated, instead use #ExtensionShellShell with the parameter list #MbExtensionShellParams. \~
  \ingroup Model_Creators
*/
// ---
//DEPRECATE_DECLARE_REPLACE( ExtensionShellShell with MbExtensionShellParams )
MATH_FUNC (MbCreator *) CreateExtensionShell(       MbFaceShell *          solid, 
                                                    MbeCopyMode            sameShell, 
                                              const RPArray<MbCurveEdge> & edges, 
                                              const ExtensionValues &      parameters, 
                                              const MbSNameMaker &         operNames, 
                                                    MbResultType &         res, 
                                                    MbFaceShell *&         shell );


//------------------------------------------------------------------------------
/** \brief \ru Построить удлинённую грань оболочки.
           \en Construct the extended face of a shell. \~
  \details \ru Построить удлинённую грань оболочки. Удлинение может быть выполнено следующими способами.
               Может быть удлинена на заданное расстояние указанная грань.
               К указанной грани может быть добавлена гладко стыкующаяся с ней грань.
               К указанной грани может быть добавлена грань, полученная выдавливанием крайнего ребра в заданном направлении.
               Одновременно с построением оболочки функция создаёт её строитель.\n
           \en Construct the extended face of a shell. An extension can be performed in the following ways:
               The specified faces can be extended on the given distance.
               A smoothly connected face can be added to the given face.
               A face obtained by extrusion of a boundary edge in the given direction can be added to the specified face.
              The function simultaneously creates the shell and its constructor.\n \~ 
  \param[in]  solid - \ru Исходная оболочка.
                      \en The initial shell. \~
  \param[in]  sameShell  - \ru Режим копирования исходной оболочки.
                           \en Mode of copying the initial shell. \~
  \param[in]  parameters - \ru Параметры построения.
                           \en Parameters of a shell creation. \~
  \param[out] res - \ru Код результата операции.
                    \en Operation result code. \~
  \param[out] shell - \ru Построенная оболочка.
                      \en The resultant shell. \~
  \result \ru Возвращает строитель оболочки.
          \en Returns the shell constructor. \~
\ingroup Model_Creators
*/
// ---
MATH_FUNC (MbCreator *) CreateExtensionShell(       c3d::ShellSPtr  &        solid, 
                                                    MbeCopyMode              sameShell,
                                              const MbExtensionShellParams & parameters, 
                                                    MbResultType &           res, 
                                                    c3d::ShellSPtr  &        shell );


#endif // __CR_EXTENSION_SHELL_H
