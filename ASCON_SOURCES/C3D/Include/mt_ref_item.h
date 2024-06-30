//////////////////////////////////////////////////////////////////////////////////////////
/** 
  \file 
  \brief  \ru Надкласс для объектов, время жизни которых автоматически регулируется счетчиком ссылок.
          \en Superclass for objects their lifetime is automatically regulated by reference counter.
*/
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MT_REF_ITEM_H
#define __MT_REF_ITEM_H

#include <math_define.h>

#include <system_atomic.h>
#include <tool_memory_leaks_check.h>

#if defined (C3D_WINDOWS ) && !defined(ALL_WARNINGS)  //_MSC_VER // Set warnings level
#pragma warning(disable: 4275)  //AP non dll-interface class '1' used as base for dll-interface class '2' (deriving exported from non-exported)
#endif


//////////////////////////////////////////////////////////////////////////////////////////
/**
  \brief  \ru Базовый класс для объектов с подсчетом ссылок.
          \en Base class for objects with reference counting. \~
  \ingroup Base_Items
  \sa #MbRefItem, #SPtr
*/
//////////////////////////////////////////////////////////////////////////////////////////

class MtRefItem //: public c3d::MemoryLeaksVerifiable
{
  //typedef  use_count_type _use_count_type;
  typedef  refcount_t     _use_count_type; // Эксперимент с потокобезопасным счетчиком ссылок (07/07/2021).
  mutable _use_count_type useCount;


protected:
           MtRefItem() : useCount(0) {}
  virtual ~MtRefItem() {} // Деструктор всех наследников должен быть закрыт. Экземпляры данного класса хранить в SPtr.

public: 
  /// \ru Добавить одну ссылку на объект. \en Adds a reference to this object. 
  refcount_t  AddRef()  const { return ++useCount; }
  /// \ru Освободить одну ссылку на объект. \en Releases a reference to this object.
  refcount_t  Release() const;

public:
  /// \ru Вернуть количество объектов, ссылающихся на данный. \en Returns a number of objects referring to this.
  refcount_t  GetUseCount() const { return useCount; }

private:
  MtRefItem( const MtRefItem & );
  MtRefItem & operator = ( const MtRefItem & );
};

//----------------------------------------------------------------------------------------
//
//---
inline refcount_t MtRefItem::Release() const 
{          
  if ( !useCount || (--useCount == 0) )
  {
    // \ru Вызов виртуального деструктора. \en Call of virtual destructor.
    delete this; // SKIP_SA
    return 0;
  }

  return useCount;
}                                        

#endif // __MT_REF_ITEM_H

// eof