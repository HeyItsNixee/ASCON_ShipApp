////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Строитель продления кривой.
         \en Constructor of curve extension.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __CR_EXTENDING_CURVE_H
#define __CR_EXTENDING_CURVE_H


#include <creator.h>
#include <curve3d.h>
#include <wire_frame.h>
#include <op_curve_parameter.h>

//------------------------------------------------------------------------------
/** \brief \ru Строитель продления кривой.
           \en Constructor of curve extension. \~
  \details \ru Строитель продления кривой.
           \en Constructor of curve extension. \~
  \ingroup Model_Creators
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
class MATH_CLASS MbExtendCurveCreator : public MbCreator
{
private:
  c3d::SpaceCurveSPtr          _sourceCurve; // \ru Исходная (расширяемая) кривая. Всегда не ноль. \en Source (extended) curve. It is not nullptr. \~
  MbCurveExtensionParameters3D _parameters;  // \ru Параметры продления. \en Extension parameters. \~

protected:
  /// \ru Конструктор копирования. \en Copy-constructor. 
  MbExtendCurveCreator( const MbExtendCurveCreator &, MbRegDuplicate * iReg );
private:
  MbExtendCurveCreator( const MbExtendCurveCreator & ); // \ru Не реализовано \en Not implemented 
  MbExtendCurveCreator(); // \ru Не реализовано. \en Not implemented.

public:
  /// \ru Конструктор по параметрам. \en Constructor by parameters.
  MbExtendCurveCreator( const MbCurve3D & curve, const MbCurveExtensionParameters3D & extendParams );

  /// \ru Деструктор. \en Destructor.
  ~MbExtendCurveCreator() override;

  // \ru Общие функции строителя. \en The common functions of the creator.
  MbeCreatorType IsA() const override { return ct_ExtensionCurveCreator; };   // \ru Тип элемента. \en A type of element.
  MbCreator & Duplicate( MbRegDuplicate * iReg = nullptr ) const override; // \ru Сделать копию. \en Create a copy.

  bool        IsSame( const MbCreator &, double accuracy ) const override; // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool        IsSimilar( const MbCreator & ) const override; // \ru Являются ли объекты подобными? \en Whether the objects are similar?
  bool        SetEqual( const MbCreator & ) override; // \ru Сделать равным. \en Make equal. 

  void        Transform( const MbMatrix3D &, MbRegTransform * = nullptr ) override; // \ru Преобразовать элемент согласно матрице. \en Transform element according to the matrix.
  void        Move( const MbVector3D &, MbRegTransform * = nullptr ) override; // \ru Сдвиг. \en Translation.
  void        Rotate( const MbAxis3D &, double angle, MbRegTransform * = nullptr ) override; // \ru Повернуть вокруг оси. \en Rotate about the axis.

  MbePrompt   GetPropertyName() override; // \ru Дать имя свойства объекта. \en Get the object property name.
  void        GetProperties( MbProperties & ) override; // \ru Выдать свойства объекта. \en Get properties of the object.
  void        SetProperties( const MbProperties & ) override; // \ru Записать свойства объекта. \en Set properties of the object. 

  /** \} */

  //DEPRECATE_DECLARE_REPLACE( CreateWireFrame with 'c3d::WireFrameSPtr' argument )
  bool        CreateWireFrame( MbWireFrame *&, MbeCopyMode, RPArray<MbSpaceItem> * ) override;  // \ru Построить кривую по журналу построения. \en Create a curve from the history tree. \~
  bool        CreateWireFrame( c3d::WireFrameSPtr & result );  // \ru Построить кривую по журналу построения. \en Create a curve from the history tree. \~

private:
  // \ru Объявление оператора присваивания без реализации, чтобы не было присваивания по умолчанию. \en The declaration of the assignment operator without implementation, to prevent an assignment by default. 
  void        operator = ( const MbExtendCurveCreator & );

  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbExtendCurveCreator )
}; // MbExtendCurveCreator

IMPL_PERSISTENT_OPS( MbExtendCurveCreator )


//------------------------------------------------------------------------------
/** \brief \ru Создание строителя продления кривой.
           \en Create a constructor of extending curve. \~
  \details \ru Создание строителя продления кривой.\n
           \en Create a constructor of extending curve.\n \~
  \param[in] sourceCurve  - \ru Исходная (удлиняемая) кривая.
                            \en Source (extended) curve. \~
  \param[in] parameters   - \ru Параметры операции.
                            \en Parameters of the operation. \~
  \param[out] res         - \ru Код результата операции.
                            \en Operation result code. \~
  \param[out] resCurve    - \ru Возвращаемая (удлиненная) кривая. Может быть nullptr.
                            \en The curve returned (extended). Can be nullptr. \~
  \result \ru Возвращает строитель.
          \en Returns the constructor. \~
  \ingroup Curve3D_Modeling
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
MATH_FUNC( c3d::CreatorSPtr ) CreateExtendedCurve( const MbCurve3D &                    sourceCurve,
                                                   const MbCurveExtensionParameters3D & parameters, 
                                                         MbResultType &                 res,
                                                         c3d::SpaceCurveSPtr &          resCurve );


#endif // __CR_EXTENDING_CURVE_H