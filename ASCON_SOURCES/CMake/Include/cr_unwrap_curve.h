////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Строитель развёрнутой кривой.
         \en Unwrapped curve creator.
*/
////////////////////////////////////////////////////////////////////////////////
  
#ifndef __CR_UNWRAP_CURVE_H
#define __CR_UNWRAP_CURVE_H

#include <creator.h>
#include <op_curve_parameter.h>

//------------------------------------------------------------------------------
/** \brief \ru Строитель развёрнутой кривой.
           \en Unwrapped curve creator. \~
  \details \ru Строитель развёрнутой кривой.\n
           \en Unwrapped curve creator.\n \~
  \ingroup Model_Creators
*/
// ---
class MATH_CLASS MbUnwrapCurveCreator : public MbCreator {
  /// \ru Параметры разворачивания. \en Parameters of the unwrapping.
  MbCurvesWrappingParams _params;

protected:
  /// \ru Конструктор копирования. \en Copy-constructor.
  MbUnwrapCurveCreator( const MbUnwrapCurveCreator & other, MbRegDuplicate * iReg );

public:
  /// \ru Конструктор по параметрам. \en Constructor by parameters.
  MbUnwrapCurveCreator( const MbCurvesWrappingParams & params );

public:
  virtual ~MbUnwrapCurveCreator() {};

  // \ru Общие функции строителя. \en The common functions of the creator.
  MbeCreatorType  IsA()                                       const override; // \ru Тип элемента \en A type of element 
  MbCreator & Duplicate( MbRegDuplicate * iReg = nullptr )    const override; // \ru Сделать копию \en Create a copy

  bool        IsSame   ( const MbCreator &, double accuracy ) const override; // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool        IsSimilar( const MbCreator & )                  const override; // \ru Являются ли объекты подобными \en Whether the objects are similar 
  bool        SetEqual ( const MbCreator & )                        override; // \ru Сделать равным \en Make equal

  void        Transform( const MbMatrix3D &, MbRegTransform * = nullptr ) override; // \ru Преобразовать элемент согласно матрице \en Transform element according to the matrix
  void        Move     ( const MbVector3D &, MbRegTransform * = nullptr ) override; // \ru Сдвиг \en Translation
  void        Rotate   ( const MbAxis3D &, double angle, MbRegTransform * = nullptr ) override; // \ru Повернуть вокруг оси \en Rotate about an axis

  MbePrompt   GetPropertyName()                          override; // \ru Дать имя свойства объекта \en Get the object property name  
  void        GetProperties  ( MbProperties & )          override; // \ru Выдать свойства объекта \en Get properties of the object 
  void        SetProperties  ( const MbProperties & )    override; // \ru Записать свойства объекта \en Set properties of the object 

  // \ru Построить кривую по журналу построения \en Create a curve from the history tree 
  bool        CreateWireFrame( MbWireFrame *&, MbeCopyMode, RPArray<MbSpaceItem> * items = nullptr ) override;

private:
  MbUnwrapCurveCreator(); // \ru Не реализовано \en Not implemented 
  OBVIOUS_PRIVATE_COPY ( MbUnwrapCurveCreator );
  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbUnwrapCurveCreator );
}; // MbUnwrapCurveCreator

IMPL_PERSISTENT_OPS( MbUnwrapCurveCreator )


////////////////////////////////////////////////////////////////////////////////
//
//                            Неклассные функции
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
/** \brief \ru Создание строителя развёрнутой кривой.
           \en Create a constructor of unwrapped curve. \~
  \details \ru Создание строителя развёрнутой кривой. Ожидется, что на вход будет подана одна кривая, или набор связных кривых.
               Иначе корректное разворачивание не гарантируется. \n
           \en Create a constructor of unwrapped curve. It is expected to receive as input 1 curve or an array of connected curves.
               Otherwise the correct unwrapping is not guaranteed. \n \~
  \param[in] parameters - \ru Параметры операции.
                          \en Parameters of the operation. \~
  \param[out] res       - \ru Код результата операции.
                          \en Operation result code. \~
  \param[out] resWire   - \ru Возвращаемая развёрнутая кривая. Может быть nullptr.
                          \en The unwrapped curve. Can be nullptr. \~
  \result \ru Возвращает строитель.
          \en Returns the creator. \~
  \ingroup Curve3D_Modeling
*/
// ---
MATH_FUNC( c3d::CreatorSPtr ) CreateUnwrappedCurve( const MbCurvesWrappingParams & parameters,
                                                          MbResultType &           res,
                                                          c3d::WireFrameSPtr &     resWire );

#endif // __CR_UNWRAP_CURVE_H
