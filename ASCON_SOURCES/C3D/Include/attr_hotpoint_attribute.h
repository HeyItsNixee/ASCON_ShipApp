////////////////////////////////////////////////////////////////////////////////
/**
  \file
  \brief \ru Атрибут хот-точки.
         \en Attribute of hot-point. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __ATTR_HOTPOINT_ATTRIBUTE_H
#define __ATTR_HOTPOINT_ATTRIBUTE_H


#include <attr_common_attribute.h>
#include <topology.h>



//------------------------------------------------------------------------------
/** \brief \ru Атрибут хот-точки.
           \en Attribute of hot-point. \~
  \details \ru Атрибут параметров хот-точки содержит трехмерную точку
               и трехмерную локальную систему координат.
           \en Attribute of parameters of hot-point contains three-dimensional point and
               three-dimensional coordinate system. \n \~
  \ingroup Model_Attributes
  \warning \ru В разработке.
           \en Under development. \~
 */
class MATH_CLASS MbHotPointAttribute : public MbCommonAttribute
{
protected:
  MbPlacement3D _place;    ///< \ru Трехмерная локальная система координат. \en Three-dimensional coordinate system.
  MbCartPoint3D _hotPoint; ///< \ru Хот-точка. \en Hot-point.
public:
         /** \brief \ru Конструктор атрибута хот-точки.
                    \en Constructor of аttribute of hot-point. \~
           \details \ru Создается атрибут хот-точки содержащий трехмерную точку и трехмерную локальную систему координат.
                    \en Created attribute of hot-point contains three-dimensional point and three-dimensional coordinate system. \~
           \param[in] place - \ru Локальная система координат хот-точки.
                              \en The local coordinate system of hot-point. \~
           \param[in] hotPoint - \ru Хот-точка.
                                 \en The hot-point. \~
         */
         MbHotPointAttribute( const MbPlacement3D & place, 
                              const MbCartPoint3D & hotPoint, 
                              const c3d::string_t & itemPrompt,
                                    bool            changeable );
         // \ru Выдать подтип атрибута. \en Get subtype of an attribute. 
         MbeAttributeType AttributeType() const override;
         // \ru Сделать копию элемента. \en Create a copy of the element. 
         MbAttribute &    Duplicate( MbRegDuplicate * = nullptr ) const override;
         // \ru Инициализировать данные по присланным. \en Initialize data. 
         bool             Init( const MbAttribute & ) override;
         // \ru Выполнить действия при трансформировании владельца. \en Perform actions when transforming the owner. 
         void             OnTransformOwner( const MbAttributeContainer & owner, 
                                            const MbMatrix3D &,
                                                  MbRegTransform * = nullptr ) override;
         // \ru Выполнить действия при перемещении владельца. \en Perform actions when moving the owner. 
         void             OnMoveOwner( const MbAttributeContainer & owner, 
                                       const MbVector3D &, 
                                             MbRegTransform * = nullptr) override;
         // \ru Выполнить действия при вращении владельца. \en Perform actions when rotating the owner. 
         void             OnRotateOwner( const MbAttributeContainer & owner, 
                                         const MbAxis3D &, 
                                              double                  angle, 
                                              MbRegTransform * = nullptr) override;
         // \ru Выполнить действия при копировании владельца. \en Perform actions when copying the owner. 
         void             OnCopyOwner( const MbAttributeContainer & owner, 
                                             MbAttributeContainer & other, 
                                             MbRegDuplicate * = nullptr ) override;
         // \ru Выполнить действия при объединении владельца. \en Perform actions when merging the owner. 
         void             OnMergeOwner( const MbAttributeContainer & owner, 
                                              MbAttributeContainer & other ) override;
         // \ru Определить, являются ли объекты равными. \en Determine whether objects are equal. 
         bool             IsSame( const MbAttribute &, 
                                        double       accuracy ) const override;
         // \ru Выдать строковое значение свойства. \en Get a string value of the property.
         void             GetCharValue( TCHAR * v ) const override; 
         // \ru Выдать свойства объекта. \en Get properties of the object.
         void             GetProperties( MbProperties & ) override; 
         // \ru Установить свойства объекта. \en Set properties of object.
         size_t           SetProperties( const MbProperties & ) override; 
         // \ru Выдать заголовок свойства объекта. \en Get a name of object property.
         MbePrompt        GetPropertyName() override; 
  /// \ru Дать систему координат. \en Get placement.
  const  MbPlacement3D & GetPlacement() const { return _place; }
  /// \ru Дать хот точку. \en Get hot - point.
  const  MbCartPoint3D & GetHotPoint() const { return _hotPoint; }
  /// \ru Деструктор. \en Destructor. 
  virtual ~MbHotPointAttribute();

private:
  // \ru Конструктор копирования. \en Copy constructor. 
  MbHotPointAttribute( const MbHotPointAttribute & init, 
                             MbRegDuplicate *      iReg );

  DECLARE_PERSISTENT_CLASS( MbHotPointAttribute )
  OBVIOUS_PRIVATE_COPY( MbHotPointAttribute )
};

IMPL_PERSISTENT_OPS( MbHotPointAttribute )

#endif // __ATTR_HOTPOINT_ATTRIBUTE_H

