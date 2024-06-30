////////////////////////////////////////////////////////////////////////////////
/**
  \file
  \brief \ru Параметры операций над кривыми.
         \en Parameters of operations on the curves. \~

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __OP_CURVE_PARAMETERS_H
#define __OP_CURVE_PARAMETERS_H

#include <math_version.h>
#include <math_define.h>
#include <templ_dptr.h>
#include <templ_sptr.h>
#include <mb_vector3d.h>
#include <mb_axis3d.h>
#include <curve.h>
#include <curve3d.h>
#include <topology.h>
#include <wire_frame.h>
#include <op_binding_data.h>
#include <mb_smooth_nurbs_fit_curve.h>

class MATH_CLASS MbSurfaceCurve;
class MATH_CLASS MbCurveExtensionParameters;

//------------------------------------------------------------------------------
/** \brief \ru Базовый класс для параметров создания кривой пересечения поверхностей.
           \en The base class for parameters of a surface intersection curve creation.\n \~
  \details \ru Базовый класс для параметров создания кривой пересечения поверхностей.
           \en The base class for parameters of a surface intersection curve creation.\n \~
  \ingroup Curve3D_Building_Parameters
*/ // ---
class MATH_CLASS MbIntCurveValues
{
protected:
  /// \ru Конструктор без параметров закрыт. \en The default constructor is protected.
  explicit MbIntCurveValues() {};
  OBVIOUS_PRIVATE_COPY(MbIntCurveValues)
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры создания кривой пересечения поверхностей по начальной точке.
           \en Parameters of a surface intersection curve creation by the initial point. \~
  \details \ru Параметры создания кривой пересечения поверхностей по начальной точке и вектору направления. \n
           \en Parameters of a surface intersection curve creation by the initial point and the direction vector. \n \~
  \ingroup Curve3D_Building_Parameters
*/ // ---
class MATH_CLASS MbIntCurveOnePointValues : public MbIntCurveValues
{
protected:
  MbRect2D _surface1Domain; ///< \ru Область ограничения кривой на первой поверхности. \en The curve bound area on the first surface. \~
  MbRect2D _surface2Domain; ///< \ru Область ограничения кривой на второй поверхности.\en The curve bound area on the second surface. \~
  MbCartPoint _uvInitOnSurface1; ///< \ru Начальная точка на первой поверхности. \en The curve starting point on the first surface. \~
  MbCartPoint _uvInitOnSurface2; ///< \ru Начальная точка на второй поверхности. \en The curve starting point on the second surface. \~
  MbVector3D _initDirection; ///< \ru Начальное направление создания линии пересечения. \en The initial direction for the intersection curve creation. \~

private:
  /// \ru Конструктор без параметров запрещён. \en The default constructor is forbidden.
  explicit MbIntCurveOnePointValues();

public:
  /** \brief \ru Конструктор.
             \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] surface1Domain - \ru Область ограничения кривой на первой поверхности.
                              \en The curve bound area on the first surface. \~
  \param[in] surface2Domain - \ru Область ограничения кривой на второй поверхности.
                              \en The curve bound area on the second surface. \~
  \param[in] uvInitOnSurface1 - \ru Начальная точка на первой поверхности.
                                \en The curve starting point on the first surface. \~
  \param[in] uvInitOnSurface2 - \ru Начальная точка на второй поверхности.
                                \en The curve starting point on the second surface. \~
  \param[in] initDirection - \ru Начальное направление создания линии пересечения.
                             \en The initial direction for the intersection curve creation. \~
    */
  MbIntCurveOnePointValues( const MbRect2D &    surface1Domain,
                            const MbRect2D &    surface2Domain,
                            const MbCartPoint & uvInitOnSurface1,
                            const MbCartPoint & uvInitOnSurface2,
                            const MbVector3D &  initDirection )
    : _surface1Domain   ( surface1Domain   )
    , _surface2Domain   ( surface2Domain   )
    , _uvInitOnSurface1 ( uvInitOnSurface1 )
    , _uvInitOnSurface2 ( uvInitOnSurface2 )
    , _initDirection    ( initDirection    )
  {}

  /// \ru Получить область ограничения кривой на первой поверхности. \en Get the curve bound area on the first surface. 
  const MbRect2D & GetSurf1Rect() const { return _surface1Domain; }
  /// \ru Получить область ограничения кривой на второй поверхности. \en Get the curve bound area on the second surface. 
  const MbRect2D & GetSurf2Rect() const { return _surface2Domain; }
  /// \ru Получить начальную точку кривой на первой поверхности. \en Get the curve starting point on the first surface. 
  const MbCartPoint & GetSurf1InitPoint() const { return _uvInitOnSurface1; }
  /// \ru Получить начальную точку кривой на второй поверхности. \en Get the curve starting point on the second surface. 
  const MbCartPoint & GetSurf2InitPoint() const { return _uvInitOnSurface2; }
  /// \ru Получить начальное направление создания линии пересечения. \en Get the initial direction for the intersection curve creation
  const MbVector3D & GetDirection() const { return _initDirection; }

OBVIOUS_PRIVATE_COPY( MbIntCurveOnePointValues )
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры создания кривой пересечения поверхностей по начальной и конечной точкам.
           \en Parameters of a surface intersection curve creation by the initial and final points. \~
  \details \ru Параметры создания кривой пересечения поверхностей по начальной точке и конечной точкам. \n
           \en Parameters of a surface intersection curve creation by the initial and final points. \n \~
  \ingroup Curve3D_Building_Parameters
*/ // ---
class MATH_CLASS MbIntCurveLimitPointsValues : public MbIntCurveValues
{
protected:
  MbCartPoint _uv1beg; ///< \ru Начальная точка на первой поверхности. \en The start point on the first surface. \~
  MbCartPoint _uv1end; ///< \ru Конечная точка на первой поверхности. \en The end point on the first surface. \~
  MbCartPoint _uv2beg; ///< \ru Начальная точка на второй поверхности. \en The start point on the second surface. \~
  MbCartPoint _uv2end; ///< \ru Конечная точка на второй поверхности. \en The end point on the second surface. \~
  MbVector3D _direction; ///< \ru Начальное направление создания линии пересечения. \en The start direction for intersection curve creation. \~
  const MbCurve3D * _guideCurve; ///< \ru Направляющая кривая, приближенно описывающая искомую кривую. \en The guide curve that approximates the desired curve. \~
  bool _ext1; ///< \ru На расширенной первой поверхности. \en Whether to create on the extended surface. \~
  bool _ext2; ///< \ru На расширенной второй поверхности. \en Whether to create on the extended second surface. \~
  bool _useRedetermination; ///< \ru Флаг, определяющий нужно ли уточнять шаг построения следующей точки по сравнению с функцией DeviationStep. \en The flag that determines whether it is necessary to specify the next point build step as compared to the DeviationStep function. \~
  bool _checkPoles; ///< \ru Флаг необходимости проверки и корректировки полюсных точек. \en The flag that determines whether it is necessary to check and correct pole points. \~

private:
  /// \ru Конструктор без параметров запрещён. \en The default constructor is forbidden.
  explicit MbIntCurveLimitPointsValues();

public:
/** \brief \ru Конструктор.
           \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] uv1beg - \ru Начальная точка на первой поверхности.
                      \en The start point on the first surface. \~
  \param[in] uv1end - \ru Конечная точка на первой поверхности.
                      \en The end point on the first surface. \~
  \param[in] ext1 - \ru На расширенной первой поверхности.
                    \en Whether to create on the extended surface. \~
  \param[in] uv2beg - \ru Начальная точка на второй поверхности.
                      \en The start point on the second surface. \~
  \param[in] uv2end - \ru Конечная точка на второй поверхности.
                      \en The end point on the second surface. \~
  \param[in] ext2 - \ru На расширенной второй поверхности.
                    \en Whether to create on the extended second surface. \~
  \param[in] dir - \ru Начальное направление создания линии пересечения.
                   \en The start direction for intersection curve creation. \~
*/
  MbIntCurveLimitPointsValues( const MbCartPoint & uv1beg,
                               const MbCartPoint & uv1end,
                                     bool          ext1,
                               const MbCartPoint & uv2beg,
                               const MbCartPoint & uv2end,
                                     bool          ext2, 
                               const MbVector3D &  dir )
    : _uv1beg ( uv1beg )
    , _uv1end ( uv1end )
    , _ext1   ( ext1   )
    , _uv2beg ( uv2beg )
    , _uv2end ( uv2end )
    , _ext2   ( ext2   )
    , _direction ( dir )
    , _guideCurve ( nullptr )
    , _useRedetermination ( false )
    , _checkPoles ( false )
  {}

/** \brief \ru Конструктор.
           \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] uv1beg - \ru Начальная точка на первой поверхности.
                      \en The start point on the first surface. \~
  \param[in] uv1end - \ru Конечная точка на первой поверхности.
                      \en The end point on the first surface. \~
  \param[in] ext1 - \ru На расширенной первой поверхности.
                    \en Whether to create on extended surface. \~
  \param[in] uv2beg - \ru Начальная точка на второй поверхности.
                      \en The start point on the second surface. \~
  \param[in] uv2end - \ru Конечная точка на второй поверхности.
                      \en The end point on the second surface. \~
  \param[in] ext2 - \ru На расширенной второй поверхности.
                    \en Whether to create on the extended second surface. \~
  \param[in] guideCurve - \ru Направляющая кривая, приближенно описывающая искомую кривую.
                          \en The guide curve that approximates the desired curve. \~
  \param[in] useRedetermination - \ru Флаг, определяющий нужно ли уточнять шаг построения следующей точки по сравнению с функцией DeviationStep.
                                  \en The flag that determines whether it is necessary to specify the next point build step as compared to the DeviationStep function. \~
  \param[in] checkPoles - \ru Флаг необходимости проверки и корректировки полюсных точек.
                          \en The flag that determines whether it is necessary to check and correct pole points. \~ 
*/
  MbIntCurveLimitPointsValues( const MbCartPoint & uv1beg,
                               const MbCartPoint & uv1end,
                                     bool          ext1,
                               const MbCartPoint & uv2beg,
                               const MbCartPoint & uv2end,
                                     bool          ext2, 
                               const MbCurve3D *   guideCurve,
                                     bool          useRedetermination,
                                     bool          checkPoles )
    : _uv1beg ( uv1beg )
    , _uv1end ( uv1end )
    , _ext1   ( ext1   )
    , _uv2beg ( uv2beg )
    , _uv2end ( uv2end )
    , _ext2   ( ext2   )
    , _guideCurve ( guideCurve )
    , _useRedetermination ( useRedetermination )
    , _checkPoles ( checkPoles )
  {}

  /// \ru Получить начало кривой на первой поверхности. \en Get the curve beginning on the first surface. 
  const MbCartPoint & GetSurf1Beg() const { return _uv1beg; }
  /// \ru Получить конец кривой на первой поверхности. \en Get the curve end on the first surface. 
  const MbCartPoint & GetSurf1End() const { return _uv1end; }
  /// \ru Получить начало кривой на второй поверхности. \en Get the curve beginning on the second surface. 
  const MbCartPoint & GetSurf2Beg() const { return _uv1beg; }
  /// \ru Получить конец кривой на второй поверхности. \en Get the curve end on the second surface. 
  const MbCartPoint & GetSurf2End() const { return _uv1end; }

  /// \ru Строить ли кривую на расширении первой поверхности. \en Whether to build the curve on the first surface extension.
  bool IfSurf1Ext () const { return _ext1; }
  /// \ru Строить ли кривую на расширении второй поверхности. \en Whether to build the curve on the second surface extension.
  bool IfSurf2Ext () const { return _ext2; }
  /// \ru Уточнять ли шаг построения следующей точки по сравнению с функцией DeviationStep. \en Whether to improve precision of the next point building step comparing to the DeviationStep function. \~
  bool UseRedetermination () const { return _useRedetermination; }
  /// \ru Нужно ли проверять и корректировать полюсные точки. \en Whether to check abd correct the pole points.
  bool CheckPoles () const { return _checkPoles; }

  /// \ru Получить начальное направление создания линии пересечения. \en Get the initial direction for the intersection curve creation
  const MbVector3D & GetDirection() const { return _direction; }
  /// \ru Получить направляющую кривую, приближенно описывающую искомую кривую. \en Get the guide curve that approximates the desired curve.
  const MbCurve3D * GetGuideCurve() const { return _guideCurve; }

OBVIOUS_PRIVATE_COPY( MbIntCurveLimitPointsValues )
};


//------------------------------------------------------------------------------
/** \brief \ru Тип параметров создания кривой пересечения поверхностей.
           \en The type of parameters for surface intersection curve creation. \~
  \details \ru Тип параметров создания кривой пересечения поверхностей.
           \en The type of parameters for surface intersection curve creation. \~
*/ // ---
enum MbeIntCurveBuildType {
    ict_General = 0,       ///< \ru Общий тип. \en General type. 
    ict_OnePoint = 1,      ///< \ru Пересечение по точке и направлению. \en Intersection by a point and a direction. 
    ict_TwoPoints = 2,     ///< \ru Пересечение по начальной и конечной точкам. \en Intersection by the initial and final points. 
    ict_TwoPointsGuide = 3 ///< \ru Пересечение по начальной и конечной точкам и направлению. \en Intersection by the initial and final points and a direction. 
  };


//------------------------------------------------------------------------------
/** \brief \ru Параметры кривой пересечения поверхностей.
           \en Parameters of an surface intersection curve. \~
  \details \ru Параметры эквидистантной кривой в пространстве по трехмерной кривой и вектору направления. \n
           \en Parameters of an offset curve in space from a three-dimensional curve and a direction vector. \n \~
  \ingroup Curve3D_Building_Parameters
*/ // ---
class MATH_CLASS MbIntCurveParams {
public:
  bool mergeCurves;  ///< \ru Объединять кривые, разрезанные швом. \en Merge curves cut by a surface seam.
  bool cutCurves;    ///< \ru Разрезать кривые в точках пересечения. \en Cut curves at intersection points.

protected:
  MbeIntCurveBuildType   _type;    ///< \ru Тип параметров создания кривой пересечения поверхностей. \en The type of parameters for surface intersection curve creation.
  SPtr<MbSNameMaker>     _snMaker; ///< \ru Именователь с версией операции. \en Names maker with operation version.
  DPtr<MbIntCurveValues> _values;  ///< \ru Параметры создания кривой пересечения поверхностей. \en Parameters for surface intersection curve creation.

public:
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор по параметрам.
             \en Constructor by parameters. \~
    \param[in] _mergeCurves - \ru Объединять кривые, разрезанные швом.
                              \en Merge curves cut by a surface seam. \~
    \param[in] _cutCurves   - \ru Разрезать кривые в точках пересечения.
                              \en Cut curves at intersection points. \~
    \param[in] _snMaker     - \ru Именователь с версией операции.
                              \en Names maker with operation version. \~
  */
  MbIntCurveParams( const MbSNameMaker & snMaker )
    : mergeCurves( true                 )
    , cutCurves  ( false                )
    , _snMaker   ( &snMaker.Duplicate() )
    , _type      ( ict_General          )
  {
    if ( snMaker.GetMathVersion() > MATH_19_VERSION ) // KOMPAS-39273 + KOMPAS-40408
      cutCurves = true;
  }
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор по параметрам.
             \en Constructor by parameters. \~
    \param[in] _cutCurves   - \ru Разрезать кривые в точках пересечения.
                              \en Cut curves at intersection points. \~
    \param[in] _snMaker     - \ru Именователь с версией операции.
                              \en Names maker with operation version. \~
  */
  MbIntCurveParams( bool _cutCurves, const MbSNameMaker & snMaker )
    : mergeCurves( true                 )
    , cutCurves  ( _cutCurves           )
    , _snMaker   ( &snMaker.Duplicate() )
    , _type      ( ict_General          )
  {}
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор по параметрам.
             \en Constructor by parameters. \~
    \param[in] _mergeCurves - \ru Объединять кривые, разрезанные швом.
                              \en Merge curves cut by a surface seam. \~
    \param[in] _cutCurves   - \ru Разрезать кривые в точках пересечения.
                              \en Cut curves at intersection points. \~
    \param[in] _snMaker     - \ru Именователь с версией операции.
                              \en Names maker with operation version. \~
  */
  MbIntCurveParams( bool _mergeCurves, bool _cutCurves, const MbSNameMaker & snMaker )
    : mergeCurves( _mergeCurves         )
    , cutCurves  ( _cutCurves           )
    , _snMaker   ( &snMaker.Duplicate() )
    , _type      ( ict_General          )
  {}

  /** \brief \ru Конструктор.
             \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] nameMaker - \ru Именователь с версией операции.
                         \en Names maker with operation version. \~
  \param[in] surface1Domain - \ru Область ограничения кривой на первой поверхности.
                              \en The curve bound area on the first surface. \~
  \param[in] surface2Domain - \ru Область ограничения кривой на второй поверхности.
                              \en The curve bound area on the second surface. \~
  \param[in] uvInitOnSurface1 - \ru Начальная точка на первой поверхности.
                                \en The curve starting point on the first surface. \~
  \param[in] uvInitOnSurface2 - \ru Начальная точка на второй поверхности.
                                \en The curve starting point on the second surface. \~
  \param[in] initDirection - \ru Начальное направление создания линии пересечения.
                             \en The initial direction for the intersection curve creation. \~
    */
  MbIntCurveParams( const MbRect2D &     surface1Domain,
                    const MbRect2D &     surface2Domain,
                    const MbCartPoint &  uvInitOnSurface1,
                    const MbCartPoint &  uvInitOnSurface2,
                    const MbVector3D &   initDirection,
                    const MbSNameMaker & nameMaker )
    : _values    ( new MbIntCurveOnePointValues(
                   surface1Domain, surface2Domain, uvInitOnSurface1, uvInitOnSurface2, initDirection) )
    , _type      ( ict_OnePoint )
    , mergeCurves( true )
    , cutCurves  ( true )
    , _snMaker   ( &nameMaker.Duplicate() )
  {}

  /** \brief \ru Конструктор.
           \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] uv1beg - \ru Начальная точка на первой поверхности.
                      \en The start point on the first surface. \~
  \param[in] uv1end - \ru Конечная точка на первой поверхности.
                      \en The end point on the first surface. \~
  \param[in] ext1 - \ru На расширенной первой поверхности.
                    \en Whether to create on the extended surface. \~
  \param[in] uv2beg - \ru Начальная точка на второй поверхности.
                      \en The start point on the second surface. \~
  \param[in] uv2end - \ru Конечная точка на второй поверхности.
                      \en The end point on the second surface. \~
  \param[in] ext2 - \ru На расширенной второй поверхности.
                    \en Whether to create on the extended second surface. \~
  \param[in] dir - \ru Начальное направление создания линии пересечения.
                   \en The start direction for intersection curve creation. \~
  \param[in] nameMaker - \ru Именователь с версией операции.
                         \en Names maker with operation version. \~
*/
  MbIntCurveParams( const MbCartPoint &  uv1beg, 
                    const MbCartPoint &  uv1end,
                          bool           ext1, 
                    const MbCartPoint &  uv2beg,
                    const MbCartPoint &  uv2end,
                          bool           ext2, 
                    const MbVector3D &   dir,
                    const MbSNameMaker & nameMaker )
    : _values    ( new MbIntCurveLimitPointsValues(uv1beg, uv1end, ext1, uv2beg, uv2end, ext2, dir) )
    , _type      ( ict_TwoPoints )
    , mergeCurves( true )
    , cutCurves  ( true )
    , _snMaker   ( &nameMaker.Duplicate() )
  {}


/** \brief \ru Конструктор.
           \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] uv1beg - \ru Начальная точка на первой поверхности.
                      \en The start point on the first surface. \~
  \param[in] uv1end - \ru Конечная точка на первой поверхности.
                      \en The end point on the first surface. \~
  \param[in] ext1 - \ru На расширенной первой поверхности.
                    \en Whether to create on extended surface. \~
  \param[in] uv2beg - \ru Начальная точка на второй поверхности.
                      \en The start point on the second surface. \~
  \param[in] uv2end - \ru Конечная точка на второй поверхности.
                      \en The end point on the second surface. \~
  \param[in] ext2 - \ru На расширенной второй поверхности.
                    \en Whether to create on the extended second surface. \~
  \param[in] guideCurve - \ru Направляющая кривая, приближенно описывающая искомую кривую.
                          \en The guide curve that approximates the desired curve. \~
  \param[in] useRedetermination - \ru Флаг, определяющий нужно ли уточнять шаг построения следующей точки по сравнению с функцией DeviationStep.
                                  \en The flag that determines whether it is necessary to specify the next point build step as compared to the DeviationStep function. \~
  \param[in] checkPoles - \ru Флаг необходимости проверки и корректировки полюсных точек.
                          \en The flag that determines whether it is necessary to check and correct pole points. \~ 
  \param[in] nameMaker - \ru Именователь с версией операции.
                         \en Names maker with operation version. \~
*/
  MbIntCurveParams( const MbCartPoint &  uv1beg,
                    const MbCartPoint &  uv1end,
                          bool           ext1, 
                    const MbCartPoint &  uv2beg,
                    const MbCartPoint &  uv2end,
                          bool           ext2, 
                    const MbCurve3D *    guideCurve,
                          bool           useRedetermination,
                          bool           checkPoles,
                    const MbSNameMaker & nameMaker )
    : _values    ( new MbIntCurveLimitPointsValues(
                   uv1beg, uv1end, ext1, uv2beg, uv2end, ext2, guideCurve, useRedetermination, checkPoles) )
    , _type      ( ict_TwoPointsGuide )
    , mergeCurves( true )
    , cutCurves  ( true )
    , _snMaker   ( &nameMaker.Duplicate() )
  {}

  /// \ru Получить параметры создания кривой по начальной точке. \en Get the parameters for the curve creation by the initial point. 
  const MbIntCurveValues * GetValues() const { return _values; }

  /// \ru Получить тип создания кривой пересечения. \en Get the type of the intesection curve creation. 
  MbeIntCurveBuildType GetType() const { return _type; }
  /// \ru Получить именователь. \en Get names maker. 
  const MbSNameMaker & GetNameMaker() const { return *_snMaker; }

OBVIOUS_PRIVATE_COPY( MbIntCurveParams )
};


//------------------------------------------------------------------------------
/** \brief \ru Результаты кривой пересечения поверхностей.
           \en Results of the surface intersection curve creation. \~
  \details \ru Результаты кривой пересечения поверхностей.
           \en Results of the surface intersection curve creation. \~
  \ingroup Curve3D_Building_Parameters
*/
// ---
struct MATH_CLASS MbIntCurveResults {
public:
  SPtr<MbCurve>     _curve1;    ///< \ru Кривая на первой поверхности. \en The curve on the first surface.
  SPtr<MbCurve>     _curve2;    ///< \ru Кривая на второй поверхности. \en The curve on the second surface.
  MbeCurveBuildType _label;     ///< \ru Тип полученной кривой пересечения. \en The type of the resulting intersection curve.
  SPtr<MbWireFrame> _wireFrame; ///< \ru Каркас с построенными кривыми. \en The frame with the constructed curves.

  /// \ru Конструктор. \en Constructor.
  MbIntCurveResults() : _label( cbt_Ordinary ) {}
OBVIOUS_PRIVATE_COPY( MbIntCurveResults )
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры эквидистантной кривой в пространстве.
           \en Parameters of an offset curve in space. \~
  \details \ru Параметры эквидистантной кривой в пространстве по трехмерной кривой и вектору направления. \n
           \en Parameters of an offset curve in space from a three-dimensional curve and a direction vector. \n \~
  \ingroup Curve3D_Building_Parameters
*/ // ---
struct MATH_CLASS MbSpatialOffsetCurveParams {
public:
  MbVector3D            offsetVect;       ///< \ru Вектор, задающий смещение в точке кривой. \en The displacement vector at a point of the curve.
  bool                  useFillet;        ///< \ru Если true, то разрывы заполнять скруглением, иначе продолженными кривыми. \en If 'true', the gaps are to be filled with fillet, otherwise with the extended curves.
  bool                  keepRadius;       ///< \ru Если true, то в существующих скруглениях сохранять радиусы. \en If 'true', the existent fillet radii are to be kept.
  bool                  bluntAngle;       ///< \ru Если true, то в притуплять острые углы. \en If 'true', sharp corners are to be blunt.
  bool                  fromBeg;          ///< \ru Вектор смещения привязан к началу (если true). \en The translation vector is associated with the beginning (if true).
protected:
  bool                  useSurfaceNormal; ///< \ru Эквидистанта согласована с нормалью к поверхности. \en Offset point is moved according to surface normal. 
  c3d::ConstSurfaceSPtr surface;          ///< \ru Поверхность кривой или подобная ей. \en Curve's surface or similar to such surface. 
  const MbSNameMaker &  snMaker;          ///< \ru Именователь кривых каркаса. \en An object defining the frame curves names.

public:
  /// \ru Конструктор по параметрам. \en Constructor by parameters.
  MbSpatialOffsetCurveParams( const MbVector3D & v, const MbSNameMaker & nm )
    : offsetVect      ( v        )
    , useFillet       ( false    )
    , keepRadius      ( false    )
    , bluntAngle      ( false    )
    , fromBeg         ( true     )
    , useSurfaceNormal( false    )
    , surface         ( nullptr )
    , snMaker         ( nm       )
  {}
public:
        /// \ru Установка режима по поверхности, переносится ли эквидистантная точка согласованно с нормалью к поверхности. \en Setting the mode whether offset point is moved according to surface normal or not. 
        void SetBySurfaceNormal( bool set, c3d::ConstSurfaceSPtr * s = nullptr )
        {
          if ( set ) {
            useSurfaceNormal = set;
            if ( s != nullptr )
              surface = *s;
          }
          else {
            useSurfaceNormal = false;
            surface = nullptr;
          }
        }
        /// \ru Получить поверхность. \en Get surface. 
        bool                    BySurfaceNormal() const { return useSurfaceNormal;  }
        /// \ru Получить поверхность. \en Get surface. 
  const c3d::ConstSurfaceSPtr & GetSurface()      const { return surface; }
        /// \ru Получить ссылку на именователь. \en Get names maker reference. 
  const MbSNameMaker &          GetNameMaker()    const { return snMaker; }

OBVIOUS_PRIVATE_COPY( MbSpatialOffsetCurveParams )
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры эквидистантной кривой на поверхности.
           \en Parameters of an offset curve on a surface. \~
  \details \ru Параметры эквидистантной кривой на поверхности по поверхностной кривой и значению смещения. \n
           \en Parameters of an offset curve on a surface from a curve on the surface and a shift value. \n \~
\ingroup Curve3D_Building_Parameters
*/ // ---
struct MATH_CLASS MbSurfaceOffsetCurveParams {
public:
  c3d::ConstFaceSPtr    face;     ///< \ru Грань, на которой строится эквидистанта. \en The face on which to build the offset curve.
  MbAxis3D              dirAxis;  ///< \ru Направление смещения с точкой приложения. \en The offset direction with a reference point.
  double                dist;     ///< \ru Величина смещения. \en The offset distance.
protected:
  const MbSNameMaker &  snMaker;  ///< \ru Именователь кривых каркаса. \en An object defining the frame curves names.

public:
  /// \ru Конструктор по параметрам. \en Constructor by parameters.
  MbSurfaceOffsetCurveParams( const MbFace & f, const MbAxis3D & a, double d, const MbSNameMaker & nm )
    : face   ( &f )
    , dirAxis( a  )
    , dist   ( d  )
    , snMaker( nm )
  {}
public:
  /// \ru Получить ссылку на именователь. \en Get names maker reference. 
  const MbSNameMaker & GetNameMaker() const { return snMaker; }

OBVIOUS_PRIVATE_COPY( MbSurfaceOffsetCurveParams )
};


//-------------------------------------------------------------------------------
/** \brief \ru Параметры продления одного конца кривой.
           \en Parameters of extension of the one curve end. \~
  \details \ru Параметры продления одного конца кривой.
           \en Parameters of extension of the one curve end. \~
  \ingroup Data_Structures
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
class MATH_CLASS MbCurveExtensionEnds : public MbPrecision
{
private:
  friend class MbExtendCurveCreator;
  friend class MbCurveExtensionParameters;

  MbeCurveExtensionWays _extensionWay;  ///< \ru Способ продления. \en The way to extend. \~
  double                _extensionLength;  ///< \ru Длина продления (в метрическом диапазоне). Всегда неотрицательна \en The length to extend (in metric range). It is always not negative. \~
  
public:
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор по умолчанию. Инициализирует параметрами, при которых само удлинение невозможно. Для инициализации корректными параметрами использовать конструкторы классов-наследников.
             \en Default constructor. It initializes by parameters, which any extension is impossible for. Use constructors of inherit classes in order to initialize by appropriate parameters.\~
  */
  MbCurveExtensionEnds();

protected:
//------------------------------------------------------------------------------
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор по параметрам.
             \en Constructor by parameters. \~
    \param[in] extWay - \ru способ продления кривой.
                        \en The way to extend the curve. \~
    \param[in] length - \ru Длина (в метрическом пространстве), на которую продлевается кривая.
                        \en The length (in metric space) the curve extended to. \~
  */
// ---
  MbCurveExtensionEnds( const MbeCurveExtensionWays extWay, const double length );

public:
  /// \ru Конструктор копирования. \en Copy constructor. \~
  MbCurveExtensionEnds( const MbCurveExtensionEnds & other );

  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор для чтения.
             \en Constructor for reading. \~
  */
  MbCurveExtensionEnds( TapeInit tapeInit );

  /// \ru Деструктор. \en Destructor. \~
  ~MbCurveExtensionEnds();

  /// \ru Минимально допустимая величина продления. \en Minimal extension length supported. \~
  static double MinLengthAllowed() { return Math::minLength; }

  /// \ru Максимально допустимая величина продления. \en Maximal extension length supported. \~
  static double MaxLengthAllowed() { return Math::maxLength; }

  /// \ru Текущий способ продления кривой. \en The Current way to extend the curve. \~
  MbeCurveExtensionWays GetWayToExtend() const { return _extensionWay; }

  /// \ru Длина (в метрическом пространстве), на которую продлевается кривая. \en The length (in metric space) the curve extended to. \~
  double GetExtensionLength() const { return _extensionLength; }

  /// \ru Проверка на равенство. \en Check if *this == other. \~
  bool IsEqual( const MbCurveExtensionEnds & other ) const;

  /// \ru Оператор присваивания. \en Assignment operator. \~
  MbCurveExtensionEnds & operator=( const MbCurveExtensionEnds & other );

KNOWN_OBJECTS_RW_REF_OPERATORS( MbCurveExtensionEnds ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class.
};

//-------------------------------------------------------------------------------
/** \brief \ru Параметры продления одного конца кривой по касательной.
           \en Parameters of extension of the one curve end by tangent. \~
  \details \ru Параметры продления одного конца кривой по линейному закону.
           \en Parameters of extension of the one curve end by linear way. \~
  \ingroup Data_Structures
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
class MATH_CLASS MbCurveExtensionLinear : public MbCurveExtensionEnds
{
public:
//------------------------------------------------------------------------------
/** \brief \ru Конструктор.
           \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] length - \ru Длина (в метрическом пространстве), на которую продлевается кривая.
                      \en The length (in metric space) the curve extended to. \~
*/
// ---
  MbCurveExtensionLinear( const double length );
};

//-------------------------------------------------------------------------------
/** \brief \ru Параметры продления одного конца кривой по соприкасающейся окружности.
           \en Parameters of extension of the one curve end by osculating circle. \~
  \details \ru Параметры продления одного конца кривой по соприкасающейся окружности.
           \en Parameters of extension of the one curve end by osculating circle. \~
  \ingroup Data_Structures
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
class MATH_CLASS MbCurveExtensionCircular : public MbCurveExtensionEnds
{
public:
//------------------------------------------------------------------------------
/** \brief \ru Конструктор.
           \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] length - \ru Длина (в метрическом пространстве), на которую продлевается кривая.
                      \en The length (in metric space) the curve extended to. \~
*/
// ---
  MbCurveExtensionCircular( const double length );
};

//-------------------------------------------------------------------------------
/** \brief \ru Параметры продления одного конца кривой по натуральному закону.
           \en Parameters of extension of the one curve end by natural way. \~
  \details \ru Входные параметры продления одного конца кривой по натуральному закону.
           \en Input parameters of extension of the one curve end by natural way. \~
  \ingroup Data_Structures
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
class MATH_CLASS MbCurveExtensionNatural : public MbCurveExtensionEnds
{
public:
//------------------------------------------------------------------------------
/** \brief \ru Конструктор.
           \en Constructor. \~
  \details \ru Конструктор по параметрам.
           \en Constructor by parameters. \~
  \param[in] length - \ru Длина (в метрическом пространстве), на которую продлевается кривая.
                      \en The length (in metric space) the curve extended to. \~
*/
// ---
  MbCurveExtensionNatural( const double length );
};

//-------------------------------------------------------------------------------
/** \brief \ru Параметры продления кривой.
           \en Parameters of the curve extension. \~
  \details \ru Параметры продления кривой.
           \en Parameters of the curve extension. \~
  \ingroup Data_Structures
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
class MATH_CLASS MbCurveExtensionParameters : public MbPrecision
{
protected:
  MbCurveExtensionEnds          _paramStart;  ///< \ru Параметры продления начала. \en The parameters to extend to the start point. \~
  MbCurveExtensionEnds          _paramEnd;  ///< \ru Параметры продления конца. \en The parameters to extend to the end point. \~
  c3d::SNameMakerSPtr           _operName;  ///< \ru Именователь операции. Всегда не ноль. \en An object defining names generation in the operation. It is always not nullptr. \~
  bool                          _allowClosure;  ///< \ru Разрешено ли замыкание. По умолчанию разрешено. \en Whether closure is allowed. It is allowed by default. \~
  
public:
  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор по параметрам.
             \en Constructor by parameters. \~
    \param[in] paramStart   - \ru Параметры продления начала кривой.
                              \en Parameters of extension to the start point. \~
    \param[in] paramEnd     - \ru Параметры продления конца кривой.
                              \en Parameters of extension from the end point. \~
    \param[in] allowClosure - \ru Допустимо ли замыкание удлиненной кривой.
                              \en Whether closure of the extended curve is allowed. \~
    \param[in] alongSurface - \ru Если true, то будет удлинена параметрическая (2D) кривая.
                                  Если false, будет удлинена пространственная (3D) кривая.
                              \en If that is true then the parametric (2D) curve will be extended.
                                  If that is false then the spatial (3D) curve will be extended.\~
    \param[in] operName     - \ru Именователь операции.
                              \en An object defining names generation in the operation. \~
  */
  // ---
  MbCurveExtensionParameters( const MbCurveExtensionEnds & paramStart,
                              const MbCurveExtensionEnds & paramEnd,
                              const bool                   allowClosure,
                              const MbSNameMaker &         operName );

  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор копирования.
             \en Copy constructor. \~
  */
  // ---
  MbCurveExtensionParameters( const MbCurveExtensionParameters & other );

  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор для чтения.
             \en Constructor for reading. \~
  */
  // ---
  MbCurveExtensionParameters( TapeInit tapeInit );

private:

  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор по умолчанию.
             \en Default constructor. \~
    \details \ru Конструктор по умолчанию (не реализован).
             \en Default constructor (not implemented). \~
  */
  // ---
  MbCurveExtensionParameters();

public:
  /// \ru Деструктор. \en Destructor. \~
  ~MbCurveExtensionParameters();

  /// \ru Вернуть true, если *this == other. \en Return true if *this == other. \~
  bool IsEqual( const MbCurveExtensionParameters & other ) const;

  /// \ru Поменять местами параметры концов кривой. \en Swap the parameters of curve's ends. \~
  void SwapEnds();

  //------------------------------------------------------------------------------
  /** \brief \ru Текущие параметры продления.
             \en The Current parameters to extend. \~
    \details \ru Текущие параметры продления на конце кривой.
             \en The Current parameters to extend at he curve end. \~
    \param[in] inStart - \ru true для начала кривой, false для ее конца.
                         \en It is true for start point of the curve and false for its end point. \~
  */
  // ---
  const MbCurveExtensionEnds & GetEndsParameters( const bool inStart ) const { return inStart? _paramStart : _paramEnd; }

  /// \ru Получить именователь операции. \en Get the object defining names generation in the operation. \~
  const MbSNameMaker & GetNameMaker() const { return *_operName; }

  /// \ru Получить информацию о разрешении замкнутости. \en Get the information about closure allowing. \~
  bool IsClosureAllowed() const { return _allowClosure; }

  /// \ru Версия операции. \en Version of the operation. \~
  VERSION GetVersion() const { return _operName->GetMathVersion(); }

  /// \ru Минимальная величина зазора (в параметрическом пространстве) для случая, когда запрещено создания замкнутых кривых. \en Minimal gap value (in parametric space) for case when closed result curves are forbidden. \~
  double GetMinUnclosedGap() const { return GetPrecision(); }

  /// \ru Оператор присваивания. \en Assignment operator. \~
  MbCurveExtensionParameters & operator=( const MbCurveExtensionParameters & other );
KNOWN_OBJECTS_RW_REF_OPERATORS( MbCurveExtensionParameters ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class.
};

//-------------------------------------------------------------------------------
/** \brief \ru Параметры продления кривой.
           \en Parameters of the curve extension. \~
  \details \ru Параметры продления кривой.
           \en Parameters of the curve extension. \~
  \ingroup Data_Structures
  \warning \ru В разработке.
           \en Under development. \~
*/
// ---
class MATH_CLASS MbCurveExtensionParameters3D : public MbCurveExtensionParameters
{
private:
  bool _extendAlongSurface;  ///< \ru Удлинять вдоль поверхности. Действует только на поверхностные кривые. \en Extend along the surface. It works only with curves on surface. \~
  
public:

  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор по параметрам.
             \en Constructor by parameters. \~
    \param[in] paramStart   - \ru Параметры продления начала кривой.
                              \en Parameters of extension to the start point. \~
    \param[in] paramEnd     - \ru Параметры продления конца кривой.
                              \en Parameters of extension from the end point. \~
    \param[in] allowClosure - \ru Допустимо ли замыкание удлиненной кривой.
                              \en Whether closure of the extended curve is allowed. \~
    \param[in] alongSurface - \ru Флаг, определяющий, продлевается ли поверхностная кривая, оставаясь лежать на поверхности.
                              \en The flag determining whether the curve on surface is extended, staying to lie in the surface.
    \param[in] operName     - \ru Именователь операции.
                              \en An object defining names generation in the operation. \~
  */
  // ---
  MbCurveExtensionParameters3D( const MbCurveExtensionEnds & paramStart,
                                const MbCurveExtensionEnds & paramEnd,
                                const bool                   allowClosure,
                                const bool                   alongSurface,
                                const MbSNameMaker &         operName );

  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор копирования.
             \en Copy constructor. \~
  */
  // ---
  MbCurveExtensionParameters3D( const MbCurveExtensionParameters3D & other );

  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор.
             \en Constructor. \~
    \details \ru Конструктор для чтения.
             \en Constructor for reading. \~
  */
  // ---
  MbCurveExtensionParameters3D( TapeInit tapeInit );

private:

  //------------------------------------------------------------------------------
  /** \brief \ru Конструктор по умолчанию.
             \en Default constructor. \~
    \details \ru Конструктор по умолчанию (не реализован).
             \en Default constructor (not implemented). \~
  */
  // ---
  MbCurveExtensionParameters3D();

public:

  /// \ru Деструктор. \en Destructor. \~
  ~MbCurveExtensionParameters3D();

  /// \ru Вернуть true, если *this == other. \en Return true if *this == other. \~
  bool IsEqual( const MbCurveExtensionParameters3D & other ) const;

  /// \ru Если true, то поверхностные кривые после удлинения останутся лежать на поверхности. В противном случае, удлиненная часть может покинуть поверхность. Действует только на поверхностные кривые. \en If it is true then curves on surface will lie in the surface after extension. Otherwise, extending part can leave the surface. It works only with curves on surface. \~
  bool IsExtensionAlongSurface() const { return _extendAlongSurface; }

  /// \ru Оператор присваивания. \en Assignment operator. \~
  MbCurveExtensionParameters3D & operator=( const MbCurveExtensionParameters3D & other );
KNOWN_OBJECTS_RW_REF_OPERATORS( MbCurveExtensionParameters3D ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class.
};


//-------------------------------------------------------------------------------
/** \brief \ru Параметры создания фаски.
           \en Parameters for the chamfer creation. \~
\details \ru Параметры создания фаски для пары соседних сегментов контура (или полилинии).
         \en Parameters for the chamfer creation between two adjacent contour (or polyline) segments. \n \~
\ingroup Data_Structures
*/
// ---
struct MATH_CLASS MbCornerChamferParams {
public:
  /// \ru Расстояние от пересечения фаски с первым сегментом до узла контура.  \en Distance between the chamfer intersection with the first segment to the contour joint.
  double distance;
  /** 
  \ru (type == true ) Угол между первым сегментом и фаской.
      (type == false) Расстояние от пересечения фаски со вторым сегментом до узла контура.
  \en (type == true ) Angle between the first segment and the chamfer.
      (type == false) Distance between the chamfer intersection with the second segment to the contour joint.
  */
  double distAngle;
  /// \ru Номер первого сегмента из пары \en The number of the first segment of the pair.
  size_t index;
  /** 
    \ru Тип создания фаски: true для типа расстояние-угол, false для типа расстояние-расстояние.
    \en The type of chamfer creation: true for the distance-angle type; false for the distance-distance type.
  */
  bool type;

  /// \ru Конструктор по параметрам. \en Constructor by parameters.
  MbCornerChamferParams( size_t _index, double _distance, double _distAngle, bool _type = false )
    : index    ( _index )
    , distance ( _distance  )
    , distAngle( _distAngle  )
    , type     ( _type )
  {}
  /// \ru Конструктор по умолчанию. \en Default constructor.
  MbCornerChamferParams()
    : index    ( -1    )
    , distance ( 0     )
    , distAngle( 0     )
    , type     ( false )
  {}
  /// \ru Конструктор копирования. \en Copy constructor.
  MbCornerChamferParams(const MbCornerChamferParams & other)
    : index    ( other.index     )
    , distance ( other.distance  )
    , distAngle( other.distAngle )
    , type     ( other.type      )
  {}
  /// \ru Оператор присваивания. \en Assignment operator. \~
  MbCornerChamferParams & operator=( const MbCornerChamferParams & other )
  {
    index     = other.index;
    distance  = other.distance;
    distAngle = other.distAngle;
    type      = other.type;
    return *this;
  }
};


//-------------------------------------------------------------------------------
/** \brief \ru Параметры создания фасок на 3д контуре.
           \en Parameters for the 3d-contour chamfers creation. \~
  \details \ru Параметры создания фасок на 3д контуре. \n
            Конструктор принимает на вход указатель на исходную кривую и/или вектор параметров <MbCornerChamferParams>.
            Возможно задать одинаковую фаску для всех узлов контура, в этом случае вместо вектора подаётся набор параметров без индекса.
            Исходная кривая может быть следующих типов: 3д-контур, 3д-полилиния, контур на поверхности, полилиния на поверхности.
            При прочих типах успешное создание фаски не гарантируется.
            Во время инициализации входная кривая преобразуется в контур.
           \en Parameters for the 3d-contour chamfers creation. \n
            Constructor takes in the pointer to the source curve and/or the vector of parameters <MbCornerChamferParams>.
            One can set the same chamfers for all the joints by passing the set of parameters without index instead of the parameter vector.
            The source curve can be one of the following types: 3d contour, 3d polyline, contour on a surface, polyline on a surface.
            For other types the successfull chamfer creation is not guaranteed.
            A polyline is transformed into a contour during the chamfering process.\~
  \ingroup Data_Structures
*/
// ---
class MATH_CLASS MbContourChamferParams {
protected:
  SPtr<MbCurve3D> curve;                     ///< \ru Кривая, на которой делается фаска. \en The curve to be chamfered.
  std::vector<MbCornerChamferParams> params; ///< \ru Набор параметров \en The set of parameters.
  size_t segmentCount;                       ///< \ru Число сегментов кривой \en Numver of curve segments

private:
  /// \ru Конструктор без параметров запрещён. \en The default constructor is forbidden.
  explicit MbContourChamferParams();

public:
  /// \ru Конструктор по параметрам. \en Constructor by parameters.
  MbContourChamferParams(const MbCurve3D & initCurve, const std::vector<MbCornerChamferParams> & _params);
  /// \ru Конструктор по кривой. \en Constructor by the curve.
  MbContourChamferParams(const MbCurve3D & initCurve);
  /// \ru Конструктор для создания контура на плоскости. \en Constructor for the creation of a plane contour.
  MbContourChamferParams(const MbCurve & initCurve, const MbPlacement3D & _placement, const std::vector<MbCornerChamferParams> & _params);
  /// \ru Конструктор для создания контура на плоскости. \en Constructor for the creation of a plane contour.
  MbContourChamferParams(const MbCurve & initCurve, const MbPlacement3D & _placement);
  /// \ru Конструктор по кривой с одинаковыми фасками во всех узлах. \en Constructor by the curve with same chamfers on each joint.
  MbContourChamferParams(const MbCurve3D & initCurve, double _distance, double _distAngle, bool _type = false);
  /// \ru Конструктор для создания контура на плоскости с одинаковыми фасками во всех узлах. \en Constructor by a plane contour with same chamfers on each joint.
  MbContourChamferParams(const MbCurve & initCurve, const MbPlacement3D & _placement, double _distance, double _distAngle, bool _type = false);

private:
  /// \ru Инициализируем трёхмерную кривую. \en 3d curve initialisation.
  void Init(const MbCurve3D & initCurve);
  /// \ru Инициализируем трёхмерную кривую. \en 3d curve initialisation.
  void Init(const MbCurve & initCurve, const MbPlacement3D & _placement);
  /// \ru Случай одинаковой фаски для всех узлов. \en Same chamfer for all the joints.
  void MakeChamfersSame(double _distance, double _distAngle, bool _type);

public:
  /// \ru Добавить в вектор параметров. \en Add to the vector of parameters.
  void Add(const MbCornerChamferParams & par);
  /// \ru Добавить в вектор параметров. \en Add to the vector of parameters.
  void Add(size_t _index, double _distance, double _distAngle, bool _type = false);
  /// \ru Очистить вектор параметров. \en Clear the vector of parameters.
  void Clear();

  /// \ru Получить число сегментов. \en Get the number of segments.
        size_t GetSegmentsCount()                           const { return segmentCount;}
  /// \ru Получить указатель на кривую. \en Get the curve pointer.
  const SPtr<MbCurve3D> & GetCurve()                         const { return curve;  }
  /// \ru Получить вектор параметров. \en Get the set of parameters.
  const std::vector<MbCornerChamferParams> & GetParameters() const { return params; }
  /// \ru Получить вектор параметров для изменения. \en Get the set of parameters for modification.
  std::vector<MbCornerChamferParams> & SetParameters()             { return params; }

  OBVIOUS_PRIVATE_COPY( MbContourChamferParams )
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры для переноса копий двумерных кривых на другой носитель.
           \en Parameters for transferring copies of two-dimensional curves on another medium. \~
    \details \ru Точка xy плоскости XY локальной системы координат должна совпадать с точкой uv 
                 параметрической области UV поверхности.
                 При параметрах angle = 0 и sense = true наложение плоскости на поверхность 
                 делается таким образом, что оси 'xy' плоскости и 'uv' поверхности соответственно сопадают.
                 При параметрах angle = 0 и sense = false наложение плоскости на поверхность делается таким образом,
                 что оси 'y' плоскости и 'v' поверхности сопадают, оси 'x' и 'u' направлены протиположно.
                 Далее, значение угла angle показывает, насколько нужно повернуть систему координат XY плоскости
                 относительно её оси Z. \n
             \en The point xy of the XY plane of the local coordinate system must coincide with the point uv of the
                 parametric region UV of the surface.
                 With parameters angle = 0 and sense = true the overlay of the plane on the surface matches 'x' and 'y' plane axes
                 to the 'u' and 'v' surface axes.
                 With parameters angle = 0 and sense = false the overlay of the plane on the surface matches 'y' plane axis
                 to the 'v' surface axis, with 'x' and 'u' axes directed oppositely.
                 Then the value of 'angle' shows how much the plane coordinate system XY is turned in respect to its Z axis. \n \~
    \ingroup Curve3D_Building_Parameters
*/ // ---
class MATH_CLASS MbCurvesWrappingParams : public MbPrecision {
private:
  c3d::ConstPlaneCurvesSPtrVector curves;  ///< \ru Двумерные кривые, копии которых будут перенесены на другой носитель. \en 2d curves, copies of which will be transferred to another medium. \~
  c3d::ConstSurfaceSPtr           surface; ///< \ru Поверхность. \en The surface. \~
  MbPlacement3D place;       ///< \ru Локальная система координат (ЛСК). \en The local coordinate system (LCS) of the curves. \~
  MbCartPoint   xy;          ///< \ru Точка привязки на плоскости XY локальной системы координат. \en The anchor point on the "XY" plane of the LCS that will be aligned with the uv point on the parametric plane of the surface. \~
  MbCartPoint   uv;          ///< \ru Точка привязки в параметрической плоскости "UV" параметров поверхности. \en The anchor point in the parametric plane "UV" of the surface parameters. \~
  double        angle;       ///< \ru Угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности. \en The angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
  bool          sense;       ///< \ru Совпадают ли направления оси "X" ЛСК и оси "U" поверхности? \en Whether the directions of the "X" axis of the LSC and the "U" axis of the surface coincide? \~
  bool          equals;      ///< \ru Должны ли длины кривых на другом носителе соответствовать оригиналам? \en Should the lengths of the curves on another medium match the originals? \~
  bool          uSeamStitch; ///< \ru Нужно ли сдвинуть кривые, попавшие краем на шов umax. \en Whether to shift curves with one end on umax seam. \~
  VERSION       version;     ///< \ru Версия алгоритма. \en Algorithm version. \~

private:
  /** \brief \ru Закрытый конструктор по-умолчанию.
             \en Default private constructor. \~
    \details \ru Закрытый конструктор по-умолчанию.
             \en Default private constructor. \~
  */
  MbCurvesWrappingParams();
  /** \brief \ru Конструктор копирования, копирует кривые и поверхности.
             \en Copy constructor, copies curves and surfaces.\~
    \details \ru Конструктор копирования, копирует кривые и поверхности.
             \en Copy constructor, copies curves and surfaces.\~
  */
  MbCurvesWrappingParams( const MbCurvesWrappingParams & other );
  /** \brief \ru Конструктор без кривых.
             \en Constructor with no curves.\~
    \details \ru Конструктор без кривых.
             \en Constructor  with no curves.\~
    \param[in] place_       - \ru Локальная система координат (ЛСК) плоскости.
                              \en The local coordinate system (LCS) of the plane. \~
    \param[in] xy_          - \ru Точка привязки на плоскости, которая будет привязана к uv-точке на поверхности.
                              \en The anchor point on the plane that will be aligned with the uv point on the parametric plane of the surface. \~
    \param[in] surface_     - \ru Поверхность для сворачивания/разворачивания.
                              \en The surface to wrap to/unwrap from. \~
    \param[in] uv_          - \ru Точка привязки на поверхности в параметрической плоскости "UV". Будет привязана к xy-точке на плоскости.
                              \en The anchor uv point on the parametric plane of the surface that will be aligned with the xy point on the plane.\~
    \param[in] angle_       - \ru Угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности.
                              \en The angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
    \param[in] sense_       - \ru Совпадают ли направления оси "X" ЛСК и оси "U" поверхности.
                              \en Whether the directions of the "X" axis of the LSC and the "U" axis of the surface coincide? \~
    \param[in] equals_      - \ru Должны ли длины кривых на другом носителе соответствовать оригиналам.
                              \en Should the lengths of the curves on another medium match the originals?\~
    \param[in] accuracy_    - \ru Точность построения.
                              \en The accuracy. \~
    \param[in] copyCurves_  - \ru Сохранить ли в этом классе параметров копии кривых.
                              \en Whether to save the curves copies in this parameter class. \~
    \param[in] copySurface_ - \ru Сохранить ли в классе параметров копию поверхности.
                              \en Whether to save the surface copy in this parameter class. \~
    \param[in] ver_         - \ru Версия алгоритма.
                              \en The version. \~
  */
  MbCurvesWrappingParams( const MbPlacement3D & place_,
                          const MbCartPoint &   xy_,
                          const MbSurface &     surface_,
                          const MbCartPoint &   uv_,
                                double  angle_,
                                bool    sense_,
                                bool    equals_,
                                double  accuracy_,
                                bool    ,
                                bool    copySurface_,
                                VERSION ver_ );

public:
  /** \brief \ru Конструктор.
             \en Constructor.\~
    \details \ru Конструктор.
             \en Constructor.\~
    \param[in] curves_      - \ru Двумерные кривые, копии которых будут свёрнуты/развёрнуты.
                              \en 2d curves, copies of which will be wrapped/unwrapped. \~
    \param[in] place_       - \ru Локальная система координат (ЛСК) плоскости.
                              \en The local coordinate system (LCS) of the plane. \~
    \param[in] xy_          - \ru Точка привязки на плоскости, которая будет привязана к uv-точке на поверхности.
                              \en The anchor point on the plane that will be aligned with the uv point on the parametric plane of the surface. \~
    \param[in] surface_     - \ru Поверхность для сворачивания/разворачивания.
                              \en The surface to wrap to/unwrap from. \~
    \param[in] uv_          - \ru Точка привязки на поверхности в параметрической плоскости "UV". Будет привязана к xy-точке на плоскости.
                              \en The anchor uv point on the parametric plane of the surface that will be aligned with the xy point on the plane.\~
    \param[in] angle_       - \ru Угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности.
                              \en The angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
    \param[in] sense_       - \ru Совпадают ли направления оси "X" ЛСК и оси "U" поверхности.
                              \en Whether the directions of the "X" axis of the LSC and the "U" axis of the surface coincide? \~
    \param[in] equals_      - \ru Должны ли длины кривых на другом носителе соответствовать оригиналам.
                              \en Should the lengths of the curves on another medium match the originals?\~
    \param[in] accuracy_    - \ru Точность построения.
                              \en The accuracy. \~
    \param[in] copyCurves_  - \ru Сохранить ли в этом классе параметров копии кривых.
                              \en Whether to save the curves copies in this parameter class. \~
    \param[in] copySurface_ - \ru Сохранить ли в классе параметров копию поверхности.
                              \en Whether to save the surface copy in this parameter class. \~
    \param[in] ver_         - \ru Версия алгоритма.
                              \en The version. \~
  */
  MbCurvesWrappingParams( const c3d::PlaneCurvesSPtrVector & curves_,
                          const MbPlacement3D & place_,
                          const MbCartPoint &   xy_,
                          const MbSurface &     surface_,
                          const MbCartPoint &   uv_,
                                double  angle_,
                                bool    sense_,
                                bool    equals_,
                                double  accuracy_,
                                bool    copyCurves_,
                                bool    copySurface_,
                                VERSION ver_ );


  /** \brief \ru Конструктор.
             \en Constructor.\~
    \details \ru Конструктор.
             \en Constructor.\~
    \param[in] curves_      - \ru 3d кривые, копии которых будут свёрнуты/развёрнуты. Ожидается, что внутри -- 2д кривые на поверхности.
                              \en 3d curves, copies of which will be wrapped/unwrapped. It is expected that there are 2d curves on surface inside.\~
    \param[in] place_       - \ru Локальная система координат (ЛСК) плоскости.
                              \en The local coordinate system (LCS) of the plane. \~
    \param[in] xy_          - \ru Точка привязки на плоскости, которая будет привязана к uv-точке на поверхности.
                              \en The anchor point on the plane that will be aligned with the uv point on the parametric plane of the surface. \~
    \param[in] surface_     - \ru Поверхность для сворачивания/разворачивания.
                              \en The surface to wrap to/unwrap from. \~
    \param[in] uv_          - \ru Точка привязки на поверхности в параметрической плоскости "UV". Будет привязана к xy-точке на плоскости.
                              \en The anchor uv point on the parametric plane of the surface that will be aligned with the xy point on the plane.\~
    \param[in] angle_       - \ru Угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности.
                              \en The angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
    \param[in] sense_       - \ru Совпадают ли направления оси "X" ЛСК и оси "U" поверхности.
                              \en Whether the directions of the "X" axis of the LSC and the "U" axis of the surface coincide? \~
    \param[in] equals_      - \ru Должны ли длины кривых на другом носителе соответствовать оригиналам.
                              \en Should the lengths of the curves on another medium match the originals?\~
    \param[in] accuracy_    - \ru Точность построения.
                              \en The accuracy. \~
    \param[in] copyCurves_  - \ru Сохранить ли в этом классе параметров копии кривых.
                              \en Whether to save the curves copies in this parameter class. \~
    \param[in] copySurface_ - \ru Сохранить ли в классе параметров копию поверхности.
                              \en Whether to save the surface copy in this parameter class. \~
    \param[in] ver_         - \ru Версия алгоритма.
                              \en The version. \~
  */
  MbCurvesWrappingParams( const c3d::ConstPlaneCurvesSPtrVector & curves_,
                          const MbPlacement3D & place_,
                          const MbCartPoint &   xy_,
                          const MbSurface &     surface_,
                          const MbCartPoint &   uv_,
                                double  angle_,
                                bool    sense_,
                                bool    equals_,
                                double  accuracy_,
                                bool    copyCurves_,
                                bool    copySurface_,
                                VERSION ver_ );


  /** \brief \ru Конструктор.
             \en Constructor.\~
    \details \ru Конструктор.
             \en Constructor.\~
    \param[in] curves_      - \ru 3д кривые, копии которых будут свёрнуты/развёрнуты. Ожидается, что внутри -- 2д кривые на поверхности.
                              \en 2d curves, copies of which will be wrapped/unwrapped. It is expected that there are 2d curves on surface inside.\~
    \param[in] place_       - \ru Локальная система координат (ЛСК) плоскости.
                              \en The local coordinate system (LCS) of the plane. \~
    \param[in] xy_          - \ru Точка привязки на плоскости, которая будет привязана к uv-точке на поверхности.
                              \en The anchor point on the plane that will be aligned with the uv point on the parametric plane of the surface. \~
    \param[in] surface_     - \ru Поверхность для сворачивания/разворачивания.
                              \en The surface to wrap to/unwrap from. \~
    \param[in] uv_          - \ru Точка привязки на поверхности в параметрической плоскости "UV". Будет привязана к xy-точке на плоскости.
                              \en The anchor uv point on the parametric plane of the surface that will be aligned with the xy point on the plane.\~
    \param[in] angle_       - \ru Угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности.
                              \en The angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
    \param[in] sense_       - \ru Совпадают ли направления оси "X" ЛСК и оси "U" поверхности.
                              \en Whether the directions of the "X" axis of the LSC and the "U" axis of the surface coincide? \~
    \param[in] equals_      - \ru Должны ли длины кривых на другом носителе соответствовать оригиналам.
                              \en Should the lengths of the curves on another medium match the originals?\~
    \param[in] accuracy_    - \ru Точность построения.
                              \en The accuracy. \~
    \param[in] copyCurves_  - \ru Сохранить ли в этом классе параметров копии кривых.
                              \en Whether to save the curves copies in this parameter class. \~
    \param[in] copySurface_ - \ru Сохранить ли в классе параметров копию поверхности.
                              \en Whether to save the surface copy in this parameter class. \~
    \param[in] ver_         - \ru Версия алгоритма.
                              \en The version. \~
  */
  MbCurvesWrappingParams( const c3d::ConstSpaceCurvesSPtrVector & curves_,
                          const MbPlacement3D & place_,
                          const MbCartPoint &   xy_,
                          const MbSurface &     surface_,
                          const MbCartPoint &   uv_,
                                double  angle_,
                                bool    sense_,
                                bool    equals_,
                                double  accuracy_,
                                bool    copyCurves_,
                                bool    copySurface_,
                                VERSION ver_ );


  /** \brief \ru Конструктор по 1 кривой.
             \en Constructor by 1 curve.\~
    \details \ru Конструктор по 1 кривой.
             \en Constructor by 1 curve.\~
    \param[in] curves_      - \ru 2д кривая, копия которой будет свёрнута/развёрнута.
                              \en 2d curve, which copy will be wrapped/unwrapped. \~
    \param[in] place_       - \ru Локальная система координат (ЛСК) плоскости.
                              \en The local coordinate system (LCS) of the plane. \~
    \param[in] xy_          - \ru Точка привязки на плоскости, которая будет привязана к uv-точке на поверхности.
                              \en The anchor point on the plane that will be aligned with the uv point on the parametric plane of the surface. \~
    \param[in] surface_     - \ru Поверхность для сворачивания/разворачивания.
                              \en The surface to wrap to/unwrap from. \~
    \param[in] uv_          - \ru Точка привязки на поверхности в параметрической плоскости "UV". Будет привязана к xy-точке на плоскости.
                              \en The anchor uv point on the parametric plane of the surface that will be aligned with the xy point on the plane.\~
    \param[in] angle_       - \ru Угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности.
                              \en The angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
    \param[in] sense_       - \ru Совпадают ли направления оси "X" ЛСК и оси "U" поверхности.
                              \en Whether the directions of the "X" axis of the LSC and the "U" axis of the surface coincide? \~
    \param[in] equals_      - \ru Должны ли длины кривых на другом носителе соответствовать оригиналам.
                              \en Should the lengths of the curves on another medium match the originals?\~
    \param[in] accuracy_    - \ru Точность построения.
                              \en The accuracy. \~
    \param[in] copyCurves_  - \ru Сохранить ли в этом классе параметров копии кривых.
                              \en Whether to save the curves copies in this parameter class. \~
    \param[in] copySurface_ - \ru Сохранить ли в классе параметров копию поверхности.
                              \en Whether to save the surface copy in this parameter class. \~
    \param[in] ver_         - \ru Версия алгоритма.
                              \en The version. \~
  */
  MbCurvesWrappingParams( const c3d::ConstPlaneCurveSPtr & curve_,
                          const MbPlacement3D & place_,
                          const MbCartPoint &   xy_,
                          const MbSurface &     surface_,
                          const MbCartPoint &   uv_,
                                double  angle_,
                                bool    sense_,
                                bool    equals_,
                                double  accuracy_,
                                bool    copyCurves_,
                                bool    copySurface_,
                                VERSION ver_ );


  /** \brief \ru Конструктор по 1 кривой.
             \en Constructor by 1 curve.\~
    \details \ru Конструктор по 1 кривой.
             \en Constructor by 1 curve.\~
    \param[in] curve_       - \ru 3д кривая, копия которой будет свёрнута/развёрнута. Ожидается, что внутри -- 2д кривая на поверхности.
                              \en 3d curve, which copy will be wrapped/unwrapped. It is expected that there is 2d curve on surface inside.\~
    \param[in] place_       - \ru Локальная система координат (ЛСК) плоскости.
                              \en The local coordinate system (LCS) of the plane. \~
    \param[in] xy_          - \ru Точка привязки на плоскости, которая будет привязана к uv-точке на поверхности.
                              \en The anchor point on the plane that will be aligned with the uv point on the parametric plane of the surface. \~
    \param[in] surface_     - \ru Поверхность для сворачивания/разворачивания.
                              \en The surface to wrap to/unwrap from. \~
    \param[in] uv_          - \ru Точка привязки на поверхности в параметрической плоскости "UV". Будет привязана к xy-точке на плоскости.
                              \en The anchor uv point on the parametric plane of the surface that will be aligned with the xy point on the plane.\~
    \param[in] angle_       - \ru Угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности.
                              \en The angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
    \param[in] sense_       - \ru Совпадают ли направления оси "X" ЛСК и оси "U" поверхности.
                              \en Whether the directions of the "X" axis of the LSC and the "U" axis of the surface coincide? \~
    \param[in] equals_      - \ru Должны ли длины кривых на другом носителе соответствовать оригиналам.
                              \en Should the lengths of the curves on another medium match the originals?\~
    \param[in] accuracy_    - \ru Точность построения.
                              \en The accuracy. \~
    \param[in] copyCurves_  - \ru Сохранить ли в этом классе параметров копии кривых.
                              \en Whether to save the curves copies in this parameter class. \~
    \param[in] copySurface_ - \ru Сохранить ли в классе параметров копию поверхности.
                              \en Whether to save the surface copy in this parameter class. \~
    \param[in] ver_         - \ru Версия алгоритма.
                              \en The version. \~
  */
  MbCurvesWrappingParams( const c3d::ConstSpaceCurveSPtr & curve_,
                          const MbPlacement3D & place_,
                          const MbCartPoint &   xy_,
                          const MbSurface &     surface_,
                          const MbCartPoint &   uv_,
                                double  angle_,
                                bool    sense_,
                                bool    equals_,
                                double  accuracy_,
                                bool    copyCurves_,
                                bool    copySurface_,
                                VERSION ver_ );


  /** \brief \ru Конструктор копирования.
             \en Copy constructor.\~
    \details \ru Конструктор копирования.
             \en Copy constructor.\~
    \param[in] copyCurves - \ru Создать копии кривых.
                            \en Create copies of the curves. \~
    \param[in] copySurface - \ru Создать копию поверхности.
                             \en Create a copy of the surface. \~
  */
  MbCurvesWrappingParams( const MbCurvesWrappingParams & other, bool copyCurves, bool copySurface, MbRegDuplicate * iReg = nullptr );
  /// \ru Деструктор. \ en Destructor.
  ~MbCurvesWrappingParams() {}

  // Закрытые методы добавления кривых.
private:
  /// \ru Добавить 2d или 3d кривые с проверкой на нули. \en 2d or 3d curves with nullptr check. \~
  template<class CurveVector>
  void AddCurves( const CurveVector & curves, bool copyCurves, MbRegDuplicate * iReg );
  /// \ru Добавить кривую без копирования с проверкой на nullptr. \en Add curve without copy and with nullptr check. \~ 
  void AddCurve( const MbCurve3D * curve );
  ///< \ru Добавить кривую без копирования с проверкой на nullptr. \en Add curve without copy and with nullptr check. \~ 
  void AddCurve( const MbCurve *   curve );
  ///< \ru Добавить кривую с проверкой на nullptr. \en Add curve with nullptr check. \~ 
  void AddCurve( const MbCurve3D * curve, bool copyCurve /*= false */, MbRegDuplicate * iReg );
  ///< \ru Добавить кривую с проверкой на nullptr. \en Add curve with nullptr check. \~ 
  void AddCurve( const MbCurve *   curve, bool copyCurve /*= false */, MbRegDuplicate * iReg );


public:
  /// \ru Дать двумерную кривую. \en Get two-dimensional curve by index. \~
  const c3d::ConstPlaneCurveSPtr & GetCurve( size_t i ) const { return curves[i]; }

  /// \ru Дать двумерные кривые. \en Get two-dimensional curves. \~
  const c3d::ConstPlaneCurvesSPtrVector & GetCurves() const { return curves; }

  /// \ru Дать двумерные кривые. \en Get two-dimensional curves. \~
        void                    GetCurves( c3d::ConstPlaneCurvesSPtrVector & retCurves ) const;
  /// \ru Дать количество кривых. \en Get two-dimensional curves count. \~
        size_t                  GetCurvesCount()                       const { return curves.size(); }
  /// \ru Заменить двумерную кривую. \en Set two-dimensional curve by index. \~
        void                    SetCurve( size_t i, const MbCurve & c )      { if ( i < curves.size() ) curves[i].assign( const_cast<MbCurve *>(&c)); }
  /// \ru Заменить двумерную кривую. \en Set two-dimensional curve by index. \~
        void                    SetCurve( size_t i, MbCurve * c )            { if ( i < curves.size() && c != nullptr ) curves[i].assign(c); }
  /// \ru Дать локальную систему координат. \en Get the local coordinate system. \~
  const MbPlacement3D &         GetPlacement()                         const { return place; }
  /// \ru Установить локальную систему координат. \en Set the local coordinate system. \~
        void                    SetPlacement( const MbPlacement3D & p )      { place = p; }
  /// \ru Дать точку на плоскости XY локальной системы координат. \en Get a point on the "XY" plane of the LCS that will be aligned with the uv point on the parametric plane of the surface. \~
  const MbCartPoint &           GetPlacePoint()                        const { return xy; }
  /// \ru Установить точку на плоскости XY локальной системы координат. \en Set a point on the "XY" plane of the LCS that will be aligned with the uv point on the parametric plane of the surface. \~
        void                    SetPlacePoint( const MbCartPoint & p )       { xy = p; }

  /// \ru Дать поверхность. \en Get the surface. \~
  const MbSurface &             GetSurface()    const;
  /// \ru Дать поверхность. \en Get the surface. \~
  const c3d::ConstSurfaceSPtr & GetSurfacePtr() const { return surface; }

  /// \ru Установить поверхность. \en Set the surface. \~
        void                    SetSurface   ( MbSurface & surf )                   { surface.assign( &surf ); }
  /// \ru Установить поверхность. \en Set the surface. \~
        void                    SetSurfacePtr( const c3d::ConstSurfaceSPtr & surf ) { if ( surf != nullptr ) surface = surf; }

  /// \ru Дать точку в области параметров поверхности. \en Get a point on the parametric plane "UV" of the surface corresponding to the point xy on the plane. \~
  const MbCartPoint &      GetSurfacePoint()                        const { return uv; }
  /// \ru Установить точку в области параметров поверхности. \en Set a point on the parametric plane "UV" of the surface corresponding to the point xy on the plane. \~
        void               SetSurfacePoint( const MbCartPoint & p )       { uv = p; }

  /// \ru Дать угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности. \en Get the angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
        double             GetAngle()                const { return angle; }
  /// \ru Установить угол поворота плоскости "XY" ЛСК и параметрической плоскости "UV" поверхности. \en Set the angle of rotation of the LSC "XY" plane and the parametric "UV" plane of the surface. \~
        void               SetAngle( double a )            { angle = a; }
  /// \ru Совпадают ли ориентации осей "X-Y" ЛСК и осей "U-V" поверхности. \en Whether the orientations of LCS "X-Y" axes and the surface "U-V" axes are same. \~
        bool               IsSense()                 const { return sense; }
  /// \ru Установить флаг совпадения направления оси "X" ЛСК и оси "U" поверхности. \en Set the coincidence of the directions of the "X" axis of the LSC and the "U" axis of the surface. \~
        void               SetSense( bool s )              { sense = s; }
  /// \ru Должна ли совпадать длина итоговых кривых оригинальным. \en Should the result curve length be equal to the original curve length? \~
        bool               IsEquals()                const { return equals; }
  /// \ru Установить требование совпадения длины кривых в плоскости и на поверхности? \en Set the curves length correspondence to the originals on the surface? \~
        void               SetEquals( bool e )             { equals = e; }
  /// \ru Дать точность построения. \en Get an accuracy. \~
        double             GetAccuracy()             const { return GetPrecision(); }
  /// \ru Установить точность построения. \en Set an accuracy. \~
        void               SetAccuracy( double acc )       { SetPrecision( acc ); }
  /// \ru Версия алгоритма. \en The version. \~
        VERSION            GetVersion()              const { return version; } 
  /// \ru Установить версию алгоритма. \en Set the algorithm version. \~
        void               SetVersion( VERSION ver )       { version = ver; } 

  /// \ru Проверка на валидность: поверхность не null и часть кривых не null. \en The validity check: whether surface is not null and some of the curves are not null. \~
        bool               IsValid  () const;

  /// \ru Сравнение параметров. \en The comparison with other parameters. \~
        bool               IsSame   ( const MbCurvesWrappingParams & other, double accuracy ) const;
  /// \ru Сравнение параметров. \en The comparison with other parameters. \~
        bool               IsSimilar( const MbCurvesWrappingParams & other )                  const;

  /** \brief \ru Нужно ли сдвинуть кривые, попавшие одним краем на шов Umax.
             \en Whether to shift curves with one end on Umax seam.\~
    \details \ru Нужно ли сдвинуть кривые, попавшие краем на шов Umax. Если кривая попадает одним краем на шов Umax,
                 а другим не попадает ни на Umax, ни на Umin, то сдвинуть эту кривую на Umax-Umin. Это упрощённая
                 стыковка кривых, которые в 3д соединяются краями на шве, но в 2д разнесены на период. При включении
                 этого флага на развёртке они также будут стыковаться.
             \en Whether to shift curves with one end on Umax seam. If one curve end is on the Umax seam and the other
                 is neither on Umax nor Umin, then shift the curve for Umax-Umin. This is a simplified stitching of
                 curves which have connected edges in 3d on the seam, but in 2d are spaced apart on a period.
                 If this flag is enabled, those unwrapped curves are alse connected. \~
  */
        bool               GetUSeamStitch()    const { return uSeamStitch; }
  /** \brief \ru Нужно ли сдвинуть кривые, попавшие одним краем на шов Umax.
             \en Whether to shift curves with one end on Umax seam.\~
    \details \ru Нужно ли сдвинуть кривые, попавшие краем на шов Umax. Если кривая попадает одним краем на шов Umax,
                 а другим не попадает ни на Umax, ни на Umin, то сдвинуть эту кривую на Umax-Umin. Это упрощённая
                 стыковка кривых, которые в 3д соединяются краями на шве, но в 2д разнесены на период. При включении
                 этого флага на развёртке они также будут стыковаться.
             \en Whether to shift curves with one end on Umax seam. If one curve end is on the Umax seam and the other
                 is neither on Umax nor Umin, then shift the curve for Umax-Umin. This is a simplified stitching of
                 curves which have connected edges in 3d on the seam, but in 2d are spaced apart on a period.
                 If this flag is enabled, those unwrapped curves are alse connected. \~
  */        
        void               SetUSeamStitch( bool s )  { uSeamStitch = s; }

  /** \brief \ru Создать копию текущих параметров.
             \en Make a copy of current parameters.\~
    \details \ru Создать копию текущих параметров.
             \en Make a copy of current parameters.\~
    \param[in] copyCurves - \ru Создать копии кривых.
                            \en Create copies of the curves. \~
    \param[in] copySurface - \ru Создать копию поверхности.
                             \en Create a copy of the surface. \~
  */
  MbCurvesWrappingParams * Duplicate( bool copyCurves, bool copySurface );

private:
  MbCurvesWrappingParams & operator = ( const MbCurvesWrappingParams & other ); // \ru Приравнивание. \en Equal operator.  \~
  friend class MATH_CLASS MbUnwrapCurveCreator;
  KNOWN_OBJECTS_RW_REF_OPERATORS( MbCurvesWrappingParams ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class. \~
};


//-------------------------------------------------------------------------------
/** \brief \ru Параметры создания клотоиды.
           \en Parameters for a clothoid creation. \~
\details \ru Параметры создания клотоиды.
         \en Parameters for a clothoid creation. \n \~
\ingroup Data_Structures
*/
// ---
struct MATH_CLASS MbClothoidParams
{
public:
  double _radiusMin;  ///< \ru Радиус кривизны на конце начального участка клотоиды. \en Curvature radius of the end of the initial part of Clothoid. \~
  double _lengthMax;  ///< \ru Максимальная длина начального участка клотоиды. \en Max length of the initial part of Clothoid. \~
  size_t _segmsNum;   ///< \ru Количество сегментов аппроксимирующей клотоиду кривой. \en Number of segments of a curve approximating Clothoid. \~

public:
  /// \ru Конструктор. \en Constructor. \~
  MbClothoidParams(double r, double l, size_t s) : _radiusMin( r ), _lengthMax( l ), _segmsNum( s ) {}
  /// \ru Конструктор. \en Constructor. \~
  MbClothoidParams() : _radiusMin( 50.0 ), _lengthMax( 200.0 ), _segmsNum( 10 ) {}

  // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool IsEqual( const MbClothoidParams & other, double accuracy ) const {
    return  ::fabs( _radiusMin - other._radiusMin ) < accuracy &&
            ::fabs( _lengthMax - other._lengthMax ) < accuracy &&
            _segmsNum == other._segmsNum;
  }


KNOWN_OBJECTS_RW_REF_OPERATORS( MbClothoidParams ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class.
};


//------------------------------------------------------------------------------
/** \brief \ru Параметры построения гладких кривых на базе трехмерной ломаной.
           \en The parameters of the construction of smooth curves based on a three-dimensional polyline. \~
  \details \ru Данные содержат параметры построения сплайнов с плавным изменением кривизны. \n
           \en The data contains parameters for constructing splines with smooth curvature changes. \n \~
  \ingroup Data_Structures
*/
// ---
struct MATH_CLASS MbFairCreateData : public MbPrecision
{
public:
  typedef std::vector<std::pair<size_t, MbVector3D> >  IndexVectorArray;

private:
  IndexVectorArray    _fixData; /// \ru Массив данных для фиксации точек и касательных. \en Data array for fixing points and tangents.
public:
  // \ru Общие параметры построения плавной кривой. \en The common parameters for a fire curve creation.
  MbeFairSmoothing    _fairing;               ///< \ru Сглаживание. \en Smoothing of curve. \~
  bool                _arrange;               ///< \ru Перераспределение точек по контуру. \en Redistribution of points. \~
  MbeFairSubdivision  _subdivision;           ///< \ru Коэффициент уплотнения кривой. \en Curve subdivision coefficient . \~
  MbeFairCurvature    _accountCurvature;      ///< \ru Учет кривизны в концевых точках. \en Accounting for curvature at end points. \~
  MbeFairVector       _accountVectorInflect;  ///< \ru Учет вектора в точке перегиба при построении NURBzS кривой. \en How to take into account the vector at the inflection point at creating NURBzS. \~
  MbeFixPntTng        _fixPointTangent;       ///< \ru Фиксировать точки на касательных / касательные в точках.       \en Fix the points on tangents / the tangents on points.

  // \ru Параметры построения B-сплайновой кривой. \en The Parameters for B-Spline curve creation.
  size_t              _degree;            ///< \ru Степень B-сплайновой кривой m ( 3 <= m <= 10). \en The degree m (3 <= m <= 10) of B-Spline curve. \~ 
  MbeFairApprox       _approx;            ///< \ru Метод аппроксимации. \en Approx method. \~
  bool                _tangentCorrect;    ///< \ru Сохранение вектора в точке перегиба при построении B-кривой. \en Save the vector at the inflection point at creating B-curve. \~
  bool                _smoothTorsion;     ///< \ru Флаг сглаживания кручения пространственной кривой (false - незамкнут, true - замкнут). \en Sign of smoothing of torsion of curve.
  size_t              _iterationsNumber;  ///< \ru Количество итераций построения B-сплайна (заданное и фактическое). \en The number of iterations for building the B-spline (given and actual).

  // \ru Параметры сглаживания зашумленных точек. \en Parameters for smoothing noisy points.
  double              _noisyDeviation;    ///< \ru Величина отклонения сглаживающей кривой от зашумленных точек.  \en Value of smoothing curve deviation from noisy points.
  size_t              _noisyIterations;   ///< \ru Количество итераций сглаживания зашумленных точек.  \en Number of iterations of smoothing noisy points.
  bool                _fixFirstPointNoisy; ///< \ru Флаг фиксации сплайна в начальной точке. \en Flag of fixing a spline at the first point.
  bool                _fixLastPointNoisy;  ///< \ru Флаг фиксации сплайна в конечной точке. \en Flag of fixing a spline at the last point.

  // \ru Диагностика. \en The diagnostics.
  MbeFairWarning      _warning; ///< \ru Предупреждение о работе. \en The operation warning. \~
  MbResultType        _error; ///< \ru Ошибка о работе. \en The operation error. \~

#ifdef C3D_DEBUG_FAIR_CURVES
  /*DEBUG*/  FILE * prt;
#endif

public:
  /// \ru Конструктор по умолчанию. \en Default constructor. 
  MbFairCreateData() : MbPrecision( METRIC_ACCURACY * 0.1, ANGLE_REGION ),
    _fairing( fairSmooth_Yes ), _arrange( false ), _subdivision( fairSubdiv_Single ),
    _accountCurvature( fairCur_No ), _accountVectorInflect( fairVector_Tangent ), _fixPointTangent( fixPntTng_NotFix ),
    _degree( 8 ), _approx( fairApprox_IsoSpline ), _tangentCorrect( true ),
    _smoothTorsion( false ),  _iterationsNumber( 1000 ),
    _noisyDeviation( 0.002 ), _noisyIterations( 200 ),
    _fixFirstPointNoisy( false ), _fixLastPointNoisy( false ),
#ifdef C3D_DEBUG_FAIR_CURVES
    prt( nullptr ),
#endif
    _warning( fwarn_Success ), _error( rt_Success ) {}

  ~MbFairCreateData() {}

  // \ru Инициализировать данные для фиксации точек и касательных. \en Initialize данные для фиксации точек и касательных..
  void InitFixConstraints( const std::vector<std::pair<size_t, MbVector3D *>> & constraints );

  //< \ru Получить данные для фиксации точек и касательных. \en Get данные для фиксации точек и касательных.
  const IndexVectorArray & GetFixConstraints() const { return _fixData; }


  /// \ru Оператор присваивания. \en Assignment operator. 
  MbFairCreateData & operator = ( const MbFairCreateData & other );

  // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool IsEqual( const MbFairCreateData &, double accuracy ) const;

  KNOWN_OBJECTS_RW_REF_OPERATORS_EX( MbFairCreateData, MATH_FUNC_EX ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class. 

}; // MbFairCreateData


//------------------------------------------------------------------------------
/** \brief \ru Параметры построения гладких кривых сопряжения на базе трехмерной ломаной.
           \en The parameters of the construction of smooth fillet curves based on a three-dimensional polyline. \~
  \details \ru Параметры построения гладких кривых сопряжения на базе трехмерной ломаной. \n
           \en The parameters of the construction of smooth fillet curves based on a three-dimensional polyline. \n \~
  \ingroup Data_Structures
*/
// ---
struct MATH_CLASS MbFairFilletData : public MbPrecision
{
public:
  bool                _arrange;      ///< \ru Перераспределение точек по контуру. \en Redistribution of points. \~
  MbeFairSubdivision  _subdivision;  ///< \ru Коэффициент уплотнения кривой. \en Curve subdivision coefficient . \~
  bool                _tangent;      ///< \ru Метод (на опорной ломаной или на касательных). \en Method (on base polyline or tangents).

  bool                _switchEndTangents;  ///< \ru Флаги учета значений концевых касательных. \en Flags accounting tangents values.
  MbVector3D          _firstTangent;       ///< \ru Касательная в начальной точке. \en Tangent in the first point.
  MbVector3D          _lastTangent;        ///< \ru Касательная в конечной точке. \en Tangent in the last point.

  bool                _switchEndCurvature; ///< \ru Флаги учета значений кривизны. \en Flags accounting curvature values.
  double              _firstCurvature;     ///< \ru Значение кривизны в начальной точке. \en Curvature in the first point.
  double              _lastCurvature;      ///< \ru Значение кривизны в конечной точке. \en Curvature in the last point.

  MbeFairWarning      _warning;      ///< \ru Предупреждение. \en The operation warning. \~
  MbResultType        _error;        ///< \ru Ошибка. \en The operation error. \~

#ifdef C3D_DEBUG_FAIR_CURVES
  /*DEBUG*/  FILE * prt;
#endif

public:
  /// \ru Конструктор по умолчанию. \en Default constructor. 
  MbFairFilletData() : MbPrecision(),
    _arrange( false ), _subdivision( fairSubdiv_Single ), _tangent( false ),
    _switchEndTangents( false ), _switchEndCurvature( false ),
    _firstCurvature( 0.0 ), _lastCurvature( 0.0 ),
#ifdef C3D_DEBUG_FAIR_CURVES
    prt( nullptr ),
#endif
    _warning( fwarn_Success ), _error( rt_Success ) {}

  ~MbFairFilletData() {}

  /// \ru Оператор присваивания. \en Assignment operator. 
  MbFairFilletData & operator = ( const MbFairFilletData & other );

  // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool IsEqual( const MbFairFilletData &, double accuracy ) const;

  KNOWN_OBJECTS_RW_REF_OPERATORS_EX( MbFairFilletData, MATH_FUNC_EX ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class. 

}; // MbFairFilletData


//------------------------------------------------------------------------------
/** \brief \ru Параметры изменения гладких кривых.
           \en The parameters for modifying smooth curves. \~
  \details \ru Данные содержат параметры изменения гладких кривых. \n
           \en The data contains parameters for modifying smooth curves. \n \~
  \ingroup Data_Structures
*/
// ---
struct MATH_CLASS MbFairChangeData : public MbPrecision
{
public:
  MbeFairSplineFormat _outFormat;  ///< \ru Выходной формат сплайна. \en Output format of spline. \~
  size_t              _nSegments;  ///< \ru Количество сегментов сплайна. \en Number of segments of spline. \~
  size_t              _numSegment; ///< \ru Номер сегмента. \en Number of segment. \~
  double              _tParam;     ///< \ru Внутренний параметр точки сегмента сплайна. \en Point internal param on segment of spline. \~

  MbeFairSmoothing    _fairing;    ///< \ru Сглаживание. \en Smoothing of curve. \~
  size_t              _degree;     ///< \ru Степень B-сплайновой кривой m ( 3 <= m <= 10). \en The degree m (3 <= m <= 10) of B-Spline curve. \~ 
  double              _scaleParam; ///< \ru Параметр репараметризации. \en Scaling parameter.    
  MbeFairCurvature    _accountCurvature;  ///< \ru Учет кривизны в концевых точках. \en Accounting for curvature at end points. \~

  bool                _fixFirstPointNoisy; ///< \ru Флаг фиксации сплайна в начальной точке. \en Flag of fixing a spline at the first point.
  bool                _fixLastPointNoisy;  ///< \ru Флаг фиксации сплайна в конечной точке. \en Flag of fixing a spline at the last point.

  SArray<size_t>        _arrayFixNoisyNum;   ///< \ru Номера точек точных значений зашумленных точек. \en Signs of exactly noisy points. 

  MbeFairWarning      _warning;    ///< \ru Предупреждение о работе. \en The operation warning. \~
  MbResultType        _error;      ///< \ru Ошибка о работе. \en The operation error. \~

#ifdef C3D_DEBUG_FAIR_CURVES
  /*DEBUG*/  FILE * prt;
#endif

public:
  /// \ru Пустой конструктор. \en Empty constructor. 
  MbFairChangeData() : MbPrecision(),
    _outFormat( fairFormat_Close ), _nSegments( 4 ), _numSegment( 0 ), _tParam( 0.5 ),
    _fairing( fairSmooth_Yes ), _degree ( 8 ), _scaleParam( 1.0 ), _accountCurvature( fairCur_No ),
    _fixFirstPointNoisy( false ), _fixLastPointNoisy( false ),
#ifdef C3D_DEBUG_FAIR_CURVES
    prt( nullptr ),
#endif
    _warning( fwarn_Success ), _error( rt_Success ) {}

  ~MbFairChangeData() {}

  /// \ru Оператор присваивания. \en Assignment operator. 
  MbFairChangeData & operator = ( const MbFairChangeData & other );

  // \ru Являются ли объекты равными? \en Determine whether an object is equal?
  bool IsEqual( const MbFairChangeData &, double accuracy ) const;

  KNOWN_OBJECTS_RW_REF_OPERATORS_EX( MbFairChangeData, MATH_FUNC_EX ) // \ru Для работы со ссылками и объектами класса. \en For working with references and objects of the class. 

}; // MbFairChangeData


//------------------------------------------------------------------------------
/** \brief \ru Параметры построения или изменения гладкой кривой.
           \en The parameters of creating or changing a fair curve. \~
  \details \ru Параметры построения или изменения гладкой кривой. \n
           \en The parameters of creating or changing a fair curve. \n \~
  \ingroup Data_Structures
*/
// ---
struct MATH_CLASS MbFairCurveParams
{
  c3d::WireFrameSPtr   _initCurve;  ///< \ru Исходная кривая. \en An initial curve.
  const MbSNameMaker & _snMaker;    ///< \ru Именователь кривых каркаса. \en An object defining the frame curves names.

public:
  /// \ru Конструктор по кривой. \en Constructor by a curve. 
  MbFairCurveParams( const MbCurve3D & crv, const MbSNameMaker & maker );
  /// \ru Конструктор по каркасу. \en Constructor by a frame. 
  MbFairCurveParams( const MbWireFrame & crv, const MbSNameMaker & maker );
  /// \ru Конструктор по каркасу. \en Constructor by a frame. 
  MbFairCurveParams( const c3d::WireFrameSPtr & crv, const MbSNameMaker & maker );

  /// \ru Деструктор. \en Destructor. 
  ~MbFairCurveParams();

  /// \ru Получить исходную кривую. \en Get the initial curve. 
  c3d::WireFrameSPtr & GetCurve() { return _initCurve; }

  /// \ru Получить исходную кривую. \en Get the initial curve. 
  const c3d::WireFrameSPtr & GetCurve() const { return _initCurve; }

  /// \ru Получить именователь. \en Get names maker. 
  const MbSNameMaker & GetNameMaker() const { return _snMaker; }

OBVIOUS_PRIVATE_COPY( MbFairCurveParams )

}; // MbFairCurveParams


//------------------------------------------------------------------------------
/** \brief \ru Результат построения или изменения гладкой кривой.
           \en The result of creating or changing a fair curve. \~
  \details \ru Результат построения или изменения гладкой кривой. \n
           \en The result of creating or changing a fair curve. \n \~
  \ingroup Data_Structures
*/
// ---
struct MATH_CLASS MbFairCurveResult : public MbApproxError
{
public:
  c3d::WireFrameSPtr _resultCurve;
  MbeFairWarning     _warning;

public:
  /// \ru Пустой конструктор. \en Empty constructor. 
  MbFairCurveResult() : MbApproxError(), _warning( fwarn_Success ) {}
  ~MbFairCurveResult() {}

  /// \ru Получить результирующую кривую. \en Get the result curve. 
  const c3d::WireFrameSPtr & GetCurve() const { return _resultCurve; }

  /// \ru Получить результирующую кривую. \en Get the result curve. 
  c3d::WireFrameSPtr & SetCurve() { return _resultCurve; }

  /// \ru Забрать кривую, удалив ее из каркаса. \en Extract a curve, removing it from wireframe.
  c3d::SpaceCurveSPtr ExtractCurve() {
    c3d::SpaceCurvesSPtrVector curves;
    ExtractCurves( _resultCurve, curves );
    return curves.size() ? curves[0] : nullptr;
  }

  /// \ru Сведения об ошибке аппроксимации. \en Approximation error information.
  const MbApproxError & GetApproxError() const { return *this; }

  /// \ru Сведения об ошибке аппроксимации. \en Approximation error information.
  MbApproxError & SetApproxError() { return *this; }

  /// \ru Сведения о предупреждении. \en Warning information.
  MbeFairWarning GetWarning() { return _warning; }

  /// \ru Сбросить данные. \en Reset data. 
  void Reset() {
    MbApproxError::Reset();
    _resultCurve.reset();
  }


OBVIOUS_PRIVATE_COPY( MbFairCurveResult )

}; // MbFairCurveResult


#endif // __OP_CURVE_PARAMETERS_H
