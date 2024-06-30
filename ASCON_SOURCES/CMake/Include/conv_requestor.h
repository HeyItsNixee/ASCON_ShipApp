////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Интерфейс запроса масштаба. Интерфейс запроса сшивки.
         \en Interface of scale request. Interface of stitching request. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __CONV_REQUESTOR_H
#define __CONV_REQUESTOR_H


#include <reference_item.h>
#include <tool_cstring.h>


//------------------------------------------------------------------------------
/** 
\brief \ru Интерфейс выбора конфигурации. 
       \en Interface of configuration selection. \~
\details  \ru Вызывается при импорте однократно, если импортируемоя модель содержит более одной конфигурации.
          \en Called on import once if the model contains contains more than one configurations. \~
 \note \ru ЭКСПЕРИМЕНТАЛЬНАЯ.
       \en EXPEREIMENTAL. \~
*/
// ---
class IConfigurationSelector : public MbRefItem
{
public:
  virtual void    AddConfiguration        ( const c3d::string_t& configurationName )  = 0;
  virtual void    SetActiveConfiguration  ( const size_t index )                      = 0;
  virtual size_t  GetConfiguration        () const                                    = 0;
};


//------------------------------------------------------------------------------
/**
\brief \ru Интерфейс для сохранения списка имен атрибутов модели.
       \en Interface for collecting model attribute names. \~
\details  \ru Вызывается во время импорта предварительных данных модели
        для задания списка имен атрибутов модели.
          \en Called during import of preliminary model data to specify
        a list of model attribute names .  \~
 \note \ru ЭКСПЕРИМЕНТАЛЬНАЯ.
       \en EXPEREIMENTAL. \~
*/
// ---
class IAttributeNamesCollector : public MbRefItem
{
public:
  virtual void AddAttributeName( const c3d::string_t& configurationName ) = 0;
};



//------------------------------------------------------------------------------
///  
/** 
\brief \ru Интерфейс запроса масштаба.
       \en Interface of scale request. \~
\details  \ru Рекомендуется использовать методы интерфейса IConvertorProperty3D.
          \en Using methods of the IConvertorProperty3D interface recommended. \~
 \note \ru Рекомендуется использовать методы интерфейса IConvertorProperty3D.
       \en Using methods of the IConvertorProperty3D interface recommended. \~
*/
// ---
class IScaleRequestor : public MbRefItem
{
public:
  virtual double ScaleRequest() = 0;
};


//------------------------------------------------------------------------------
/** 
\brief \ru Интерфейс запроса сшивки. 
      \en Interface of stitching request \~
\details  \ru Рекомендуется использовать методы интерфейса IConvertorProperty3D.
          \en Using methods of the IConvertorProperty3D interface recommended. \~
 \note \ru Рекомендуется использовать методы интерфейса IConvertorProperty3D.
       \en Using methods of the IConvertorProperty3D interface recommended. \~
*/
// ---
class IStitchRequestor : public MbRefItem
{
public:
  virtual bool StitchRequest() = 0;
};


#endif // __CONV_REQUESTOR_H
