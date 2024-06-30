////////////////////////////////////////////////////////////////////////////////
/** 
  \file
  \brief \ru Элемент аннотации.
         \en Annotation item.

*/
////////////////////////////////////////////////////////////////////////////////


#ifndef __MB_PMI_H
#define __MB_PMI_H

#include <legend.h>
#include <model_item.h>
#include <mb_placement.h>

//----------------------------------------------------------------------------------------
/** \brief \ru Элемент текста объекта аннотации.
           \en Reference-counted object.  \~
  \details \ru Базовый класс текстовых элементов - составного текста или несущего содержание. \n
           \en Base class for text items - composite or readable. \n\~

  \note \ru Текст размещается в плоскости элемента аннотации и может быть либо группой текстовых
            элементов, либо строкой либо специальным символом.
        \en Text is located in the plane of annotation and can be either formatter or unformatted
            text or symbol or a group of text elements. \~
  \ingroup Legend
*/
// ---
class MATH_CLASS MbTextItem : public TapeBase, public MbRefItem
{
public:
  /// \ru Создать копию объекта. \en Create a copy of an object.
  virtual SPtr<MbTextItem> Clone( MbRegDuplicate * = nullptr ) const = 0;
  /// \ru Являются ли объекты равными. \en Are the objects equal. 
  virtual bool IsSame( const MbTextItem & to, double accuracy ) const = 0;

  DECLARE_PERSISTENT_CLASS( MbTextItem );
protected:
  MbTextItem();
  OBVIOUS_PRIVATE_COPY( MbTextItem )
};


IMPL_PERSISTENT_OPS( MbTextItem )


//----------------------------------------------------------------------------------------
/** \brief \ru Интерпретация текста.
           \en Text content type.  \~
  \details \ru Текст или специальный символ. \n
           \en Object counting number of its owners. \n\~
  \ingroup Legend
*/
// ---
enum MbeTextLiteralForm
{ 
  tlf_UnformattedString = 0,  ///< \ru Текст без кодов форматирования. \en Text as is.
  tlf_PreDefinedCode    = 1,  ///< \ru Код специального символа. \en Text is a code of a symbol.
  tlf_FormattedString   = 2   ///< \ru Текст с элементами форматирования форматирования. \en Formatted text.
};


//----------------------------------------------------------------------------------------
/** \brief \ru Текстовый элемент, несущий содержание.
           \en Text element which has content.  \~
  \details \ru Текст или специальный символ. \n
           \en Either text or pre-defined simbol. \n\~
  \ingroup Legend
*/
// ---
class MATH_CLASS MbTextLiteral : public MbTextItem
{
  c3d::string_t       m_text; ///< \ru Текст или код символа. \en Text or symbol's code.
  c3d::string_t       m_font; ///< \ru Шрифт. \en Font.
    /** \brief \ru Положение в плоскости аннотации.
             \en Location in annotation plane. \~
    \details \ru Начало системы координат соответствует левому нижнему углу.
             \en The origin of the location is position of the left-bottom corner. \~
  */
  MbPlacement         m_location;
  double              m_width;    ///< \ru Ширина. \en Width.
  double              m_height;   ///< \ru Высота. \en Height.
  MbeTextLiteralForm  m_textForm; ///< \ru Способ интерпретации текста. \en Meaning of the text field..
public:
  
  explicit MbTextLiteral( const c3d::string_t & initText, const MbPlacement & place, MbeTextLiteralForm preDefinedCode );
  
  /// \ru Получить текст. \en Get text. 
  c3d::string_t GetText() const;
  /// \ru Получить положение в плоскости аннотации. \en Get location in annotation plane. 
  MbPlacement   GetLocation() const;
  /// \ru Получить ширину. \en Get width. 
  double        GetWidth() const;
  /// \ru Получить высоту. \en Get height. 
  double        SetHeight() const;
  /// \ru Задать ширину. \en Set width. 
  bool          SetWidth( double w );
  /// \ru Задать высоту. \en Set height.
  bool          SetHeight( double h );
  /// \ru Получить тип текста. \en Get text form. 
  MbeTextLiteralForm GetTextForm() const;
  /// \ru Получить шрифт. \en Get font. 
  c3d::string_t GetFont() const;
  /// \ru Задать шрифт. \en Set font. 
  void SetFont( const c3d::string_t & font );
  
  /// \ru Создать копию объекта. \en Create a copy of an object.
  SPtr<MbTextItem> Clone( MbRegDuplicate * = nullptr ) const override;
  /// \ru Являются ли объекты равными. \en Are the objects equal. 
  bool IsSame( const MbTextItem & to, double accuracy ) const override;

  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbTextLiteral )
  OBVIOUS_PRIVATE_COPY( MbTextLiteral )
};

IMPL_PERSISTENT_OPS( MbTextLiteral )


//----------------------------------------------------------------------------------------
/** \brief \ru Составной текст.
           \en Composite text.  \~
  \details \ru Группа из более, чем одного текстового элемента. \n
           \en Group of two or more text items. \n\~
  \ingroup Legend
*/
// ---
class MATH_CLASS MbCompositeText : public MbTextItem
{
  /// \ru Элементы группы текстовых элементов. \en Grouped items.
  std::vector<SPtr<MbTextItem>> m_textItems;
public:
/** \brief  \ru Создать составной текстовый элемент.
            \en Create composite text element. \~
    \details \ru Элемент создаётся только в том случае, если в исходных данных будет два или более ненулевых элемента.
             \en New element is created if two or more text items found in the source array. \~
  */
  static SPtr<MbCompositeText> Create( const std::vector<SPtr<MbTextItem>> & textItems );
private:
  /// \ru Конструктор. \en Constructor. 
  explicit MbCompositeText( std::vector<SPtr<MbTextItem>> && textItems );
  /// \ru Создать копию объекта. \en Create a copy of an object.
  SPtr<MbTextItem> Clone( MbRegDuplicate * = nullptr ) const override;
  /// \ru Являются ли объекты равными. \en Are the objects equal. 
  bool IsSame( const MbTextItem & to, double accuracy ) const override;
  
  DECLARE_PERSISTENT_CLASS_NEW_DEL( MbCompositeText )
  OBVIOUS_PRIVATE_COPY( MbCompositeText )
};


IMPL_PERSISTENT_OPS( MbCompositeText )


//----------------------------------------------------------------------------------------
/** \brief \ru Элемент аннотации.
           \en Reference-counted object.  \~
  \ingroup Legend
*/
// ---
class MATH_CLASS MbPMI : public MbLegend
{
  /// \ru Плоскость для отображения текста. \en The plane text is shown in.
  MbPlacement3D                 m_planeTextDisplay;
  /// \ru Заголовок элемента аннотации. \en The title of the annotation item.
  c3d::string_t                 m_title;
  /// \ru Элементы для непосредственного отображения. \en The elements to be displaied directly.
  c3d::ItemsSPtrVector          m_nonTextItems;
  /// \ru Текстовые элементы. \en The text items of the annotation.
  std::vector<SPtr<MbTextItem>> m_textItems;
public:

/** \brief \ru Создать элемент аннотации.
           \en Create annotation item. \~
              \param[in] plane -            \ru Плоскость для отображения плоских элементов,
                                            \en Plane for planar elements transformation into space, \~
              \param[in] pmiName -          \ru Название элемента аннотации,
                                            \en Captrion of the annotation element, \~
              \param[in] pmiVisual -        \ru Геометрические компоненты элемента аннотации,
                                            \en Geometric items ot the annotation element, \~
              \param[in] pmiText            \ru Текстовые компоненты элемента аннотации.
                                            \en Text items ot the annotation element. \~
              \return \ru Возвращает указатель на элемент аннотации, если передаётся хотя бы один
                         ненулевой текстовый или геометрический элемент, иначе нулевой указатель.
                      \en Returns pointer to new annotation element if only at least one text or geometric 
                         element is given, otherwise null pointer. \~
          */
  static SPtr<MbPMI> Create ( const MbPlacement3D & plane   = MbPlacement3D::global,
                              const c3d::string_t & pmiName = c3d::string_t(),
                              const c3d::ItemsSPtrVector &  pmiVisual = c3d::ItemsSPtrVector(),
                              const std::vector<SPtr<MbTextItem>> & pmiText = std::vector<SPtr<MbTextItem>>() );

protected:
/** \brief \ru Конструктор.
           \en Constructor. \~
              \param[in] plane -            \ru Плоскость для отображения плоских элементов,
                                            \en Plane for planar elements transformation into space, \~
              \param[in] pmiName -          \ru Название элемента аннотации,
                                            \en Captrion of the annotation element, \~
              \param[in] pmiVisual -        \ru Геометрические компоненты элемента аннотации,
                                            \en Geometric items ot the annotation element, \~
              \param[in] pmiText            \ru Текстовые компоненты элемента аннотации.
                                            \en Text items ot the annotation element. \~
          */
  MbPMI( const MbPlacement3D & plane, const c3d::string_t & pmiName, c3d::ItemsSPtrVector &&, std::vector<SPtr<MbTextItem>> && pmiText );
public:

  /// \ru Получить плоскость для отображения плоских элементов. \en Get plane for planar elements transformation into space.
  MbPlacement3D GetLocation() const;

  /// \ru Получить название элемента аннотации. \en Get captrion of the annotation element.
  c3d::string_t GetTitle() const;

  /// \ru Получить количество геометрических элементов. \en Get count of geometric items.
  size_t GeometricElementsCount() const;

  /** \brief \ru Получить геометрический элемент с указанным индексом.
             \en Get geometric item at the specified index. \~
              \param[in] elementIndex -     \ru Индекс элемента,
                                            \en Element's index, \~
              \return \ru Элемент по указанному индексу, если индекс допустим, иначе нулевой указатель.
                      \en Element if index is valid, null pointer otherwise. \~
          */
  c3d::ItemSPtr GetGeometricElement( size_t elementIndex ) const;

  /// \ru Получить количество текстовых элементов. \en Get count of text items.
  size_t TextElementsCount() const;

  /** \brief \ru Получить текстовый элемент с указанным индексом.
             \en Get text item at the specified index. \~
              \param[in] elementIndex -     \ru Индекс элемента,
                                            \en Element's index, \~
              \return \ru Элемент по указанному индексу, если индекс допустим, иначе нулевой указатель.
                      \en Element if index is valid, null pointer otherwise. \~
          */
  SPtr<MbTextItem> GetTextElement( size_t elementIndex ) const;
  
  MbeSpaceType    IsA      () const override; // \ru Тип объекта. \en A type of an object.
  MbeSpaceType    Type     () const override; // \ru Групповой тип объекта. \en Group type of object. 
  MbSpaceItem &   Duplicate( MbRegDuplicate * = nullptr ) const override; // \ru Создать копию. \en Create a copy.
  bool            IsSame   ( const MbSpaceItem & other, double accuracy = LENGTH_EPSILON ) const override; // \ru Являются ли объекты равными? \en Are the objects equal? 
  bool            SetEqual ( const MbSpaceItem & ) override; // \ru Сделать объекты равным. \en Make the objects equal. 
  
  void    Transform( const MbMatrix3D &, MbRegTransform * = nullptr ) override; // \ru Преобразовать согласно матрице. \en Transform according to the matrix.
  void    Move     ( const MbVector3D &, MbRegTransform * = nullptr ) override; // \ru Сдвинуть вдоль вектора. \en Translate along a vector.
  void    Rotate   ( const MbAxis3D   &, double angle, MbRegTransform * = nullptr ) override; // \ru Повернуть вокруг оси. \en Rotate around an axis.
  bool    IsSimilar( const MbSpaceItem & init ) const override; // \ru Являются ли объекты подобными? \en Determine whether the objects are similar.
  double  DistanceToPoint ( const MbCartPoint3D & ) const override; // \ru Вычислить расстояние до точки. \en Calculate the distance to a point.
  void    AddYourGabaritTo( MbCube & r ) const override; // \ru Добавь свой габарит в куб. \en Add bounding box into a cube.
  
  void    CalculateMesh( const MbStepData & stepData, const MbFormNote & note, MbMesh & mesh ) const override; // \ru Построить полигональную копию mesh. \en Build polygonal copy mesh.

  MbProperty & CreateProperty( MbePrompt n ) const override;     // \ru Создать собственное свойство. \en Create a custom property.
  void    GetProperties( MbProperties & properties ) override; // \ru Выдать свойства объекта. \en Get properties of the object.
  void    SetProperties( const MbProperties & properties ) override; // \ru Установить свойства объекта. \en Set properties of the object.
  
  DECLARE_PERSISTENT_CLASS( MbPMI )
private:
  MbPMI( const MbPMI& );
  const MbPMI& operator=( const MbPMI& ) = delete;
};

IMPL_PERSISTENT_OPS( MbPMI )

#endif /* __MB_PMI_H */
