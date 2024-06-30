////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Геометрический атрибут.
         \en Geometric attribute. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __ATTR_GEOMETRIC_ATTRIBUTE_H
#define __ATTR_GEOMETRIC_ATTRIBUTE_H


#include <attr_common_attribute.h>
#include <attr_registry.h>
#include <math_define.h>
#include <creator.h>


class  MATH_CLASS MbSpaceItem;
class  MATH_CLASS MbProperty;
class  MATH_CLASS MbProperties;
class             MbRegTransform;
class             MbRegDuplicate;


//------------------------------------------------------------------------------
/** \brief \ru Геометрический атрибут.
           \en Geometric attribute. \~
  \details \ru Геометрический атрибут. \n
           \en Geometric attribute. \n \~
  \ingroup Model_Attributes
 */
class MATH_CLASS MbGeomAttribute : public MbCommonAttribute {
protected :
  MbSpaceItem *  spaceItem; ///< \ru Геометрический объект. \en A geometric object. 
  MbeCreatorType type;      ///< \ru Тип операции. \en Operation type. 
  bool           keepItem;  ///< \ru Сохранять исходный объект при копировании. \en Save the initial object when copying. 

private:
  // \ru Конструктор копирования. \en Copy constructor. 
  MbGeomAttribute( const MbGeomAttribute & init, MbRegDuplicate * iReg );
public :
  /// \ru Конструктор. \en Constructor. 
  MbGeomAttribute( const MbSpaceItem & item, MbeCreatorType t, bool keepItem ); 
  /// \ru Конструктор. \en Constructor. 
  MbGeomAttribute( const MbSpaceItem & item, MbeCreatorType t, bool keepItem, const c3d::string_t & itemPrompt ); 
  /// \ru Деструктор. \en Destructor. 
  virtual ~MbGeomAttribute();

public:
  // \ru Выдать подтип атрибута. \en Get subtype of an attribute. 
  MbeAttributeType AttributeType() const override;
  // \ru Сделать копию элемента. \en Create a copy of the element. 
  MbAttribute & Duplicate( MbRegDuplicate * = nullptr ) const override;
  // \ru Определить, являются ли объекты равными. \en Determine whether objects are equal. 
  bool    IsSame( const MbAttribute &, double accuracy ) const override;
  // \ru Инициализировать данные по присланным. \en Initialize data. 
  bool    Init( const MbAttribute & ) override; 
  // \ru Выполнить действия при трансформировании владельца. \en Perform actions when transforming the owner. 
  void    OnTransformOwner( const MbAttributeContainer & owner, const MbMatrix3D &, MbRegTransform * = nullptr ) override;
  // \ru Выполнить действия при перемещении владельца. \en Perform actions when moving the owner. 
  void    OnMoveOwner( const MbAttributeContainer & owner, const MbVector3D &, MbRegTransform * = nullptr ) override;
  // \ru Выполнить действия при вращении владельца. \en Perform actions when rotating the owner. 
  void    OnRotateOwner( const MbAttributeContainer & owner, const MbAxis3D &, double angle, MbRegTransform * = nullptr ) override;
  // \ru Выполнить действия при копировании владельца. \en Perform actions when copying the owner. 
  void    OnCopyOwner( const MbAttributeContainer & owner, MbAttributeContainer & other, MbRegDuplicate * = nullptr ) override;
  // \ru Выполнить действия при объединении владельца. \en Perform actions when merging the owner. 
  void    OnMergeOwner( const MbAttributeContainer & owner, MbAttributeContainer & other ) override; 

  void    GetCharValue( TCHAR * v ) const override; // \ru Выдать строковое значение свойства. \en Get a string value of the property. 
  void    GetProperties( MbProperties & ) override; // \ru Выдать свойства объекта. \en Get properties of the object. 
  size_t  SetProperties( const MbProperties & ) override; // \ru Установить свойства объекта. \en Set properties of object. 
  MbePrompt GetPropertyName() override; // \ru Выдать заголовок свойства объекта. \en Get a name of object property. 
          /// \ru Дать геометрический объект. \en Get geometric object. 
  const   MbSpaceItem * GetSpaceItem() const { return spaceItem; } 
          MbSpaceItem * SetSpaceItem() { return spaceItem; } 
          /// \ru Заменить геометрический объект. \en Replace geometric object. 
          void    ChangeSpaceItem( MbSpaceItem & init );
          /// \ru Дать тип операции. \en Get operation type. 
          MbeCreatorType GetOperationType() const { return type; }

DECLARE_PERSISTENT_CLASS_NEW_DEL( MbGeomAttribute ) 
OBVIOUS_PRIVATE_COPY( MbGeomAttribute )
}; 

IMPL_PERSISTENT_OPS( MbGeomAttribute )

#endif // __ATTR_GEOMETRIC_ATTRIBUTE_H
