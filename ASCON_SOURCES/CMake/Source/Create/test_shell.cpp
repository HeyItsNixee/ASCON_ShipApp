////////////////////////////////////////////////////////////////////////////////
//
// Примеры работы функций создания оболочек.
// Examples of creation various face shells.
//
////////////////////////////////////////////////////////////////////////////////

#include <test.h>
#include <test_manager.h>
#include <test_service.h>
#include <mb_enum.h>
#include <cur_polycurve.h>
#include <cur_line_segment3d.h>
#include <cur_surface_intersection.h>
#include <surface.h>
#include <creator.h>
#include <op_shell_parameter.h>
#include <action_shell.h>
#include <action_curve3d.h>
#include <action_surface_curve.h>
#include <action_surface.h>
#include <action_phantom.h>
#include <action_general.h>
#include <solid.h>
#include <func_const_function.h>
#include <func_line_function.h>
#include <func_cubic_function.h>
#include <position_data.h>
#include <contour_graph.h>
#include <region.h>
#include <plane_instance.h>
#include <space_instance.h>
#include <math_namespace.h>
#include <cur_polyline3d.h>
#include <test_draw.h>
#include <last.h>

using namespace c3d;


// Включатели отрисовки во время отладки.
// State of debug drawing.
namespace DrawTestShell {
  bool drawItems = false;
  bool checkPhantom = false;
}


//------------------------------------------------------------------------------
// Создание эквидистантной оболочки на указанных гранях тела
// Create an offset shell with selected solid body's faces
// ---
void CreateOffsetShells()
{
  MbItem  * obj = nullptr;
  MbTopItem * faceTopItem = nullptr;
  RPArray<MbFace> faces( 0, 1 );

  if ( GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj, &faceTopItem ) ) {
    TestVariables::viewManager->ToggleObject(obj);
//#ifdef _DRAWGI
    TestVariables::drawImp->DrawItem( faceTopItem, TRGB_WHITE );
//#endif
    MbFace * face = (MbFace *)faceTopItem;

    faces.Add( face );

    MbItem * obj0 = nullptr;
    while ( GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj0, &faceTopItem ) ) {
      if ( obj == obj0 ) {
//#ifdef _DRAWGI
        TestVariables::drawImp->DrawItem( faceTopItem, TRGB_WHITE );
//#endif
        face = (MbFace *)faceTopItem;

        size_t foundInd = faces.FindIt( face );
        if ( foundInd == SYS_MAX_T )
          faces.Add( face );
        else {
//#ifdef _DRAWGI
          TestVariables::drawImp->DrawItem( faceTopItem, TRGB_LIGHTGREEN );
//#endif
          faces.DetachInd( foundInd );
        }
      }
    }
    TestVariables::viewManager->ToggleObject(obj);
  }
  else if ( GetGObj( IDS_SELECT_SOLID, st_Solid, &obj ) && (obj != nullptr) ) {
    // Выбор всех граней для создания эквидистантной оболочки
    // Selection of all faces for creation of the offset shell
    const MbSolid * solid = static_cast<const MbSolid *>(obj);
    solid->GetFaces( faces );
  }

  if ( faces.Count() > 0 ) {
    if ( GetDouble( IDS_ENTER_THICKNESS, TestVariables::thickness )) {
      SetWaitCursor( true );
      
      MbSNameMaker snMaker( (SimpleName)ct_ShellSolid, MbSNameMaker::i_SideNone/*sideAdd*/, 0/*buttAdd*/ );
      SweptValues param;
      param.shellClosed = false;

      param.SetMergingFaces( TestVariables::mergeFaces );
      param.SetMergingEdges( TestVariables::mergeEdges );

      if ( TestVariables::thickness<0 ) {
        param.thickness1 = 0;
        param.thickness2 = -TestVariables::thickness;
      }
      else {
        param.thickness1 = TestVariables::thickness;
        param.thickness2 = 0;
      }
      MbSolid * resSolid = nullptr;
      bool copyAttrs = TestVariables::copyAttributesOnDemand;
      MbResultType res = ::OffsetShell( *static_cast<MbSolid *>(obj), cm_Copy, faces, true, param, 
                                        snMaker, copyAttrs,
                                        resSolid );
      if ( resSolid == nullptr )
        ::PutErrorMessage( res, IDS_TITLE_ERR );

      SetWaitCursor( false );

      if (resSolid != nullptr)
        TestVariables::viewManager->AddObject( TestVariables::SURFACE_Style, resSolid );
    }
  }
}


//------------------------------------------------------------------------------
// Создание Lofted оболочки по точкам
// Creation of a lofted shell by points
// ---
void CreateLoftedShellByPoints() {
  MbCartPoint3D p;
  PArray <SArray <MbCartPoint3D> > points(2,2,true);

  while ( GetPoint( IDS_SELECT_POINT, p ) ) {
    SArray <MbCartPoint3D> *pList = new SArray <MbCartPoint3D> (4,2);    //-V112
    do {
//#ifdef _DRAWGI
      TestVariables::drawImp->DrawPoint( &p, TRGB_LIGHTGREEN );
//#endif // _DRAWGI
        pList->Add( p );
    } while ( GetPoint( IDS_SELECT_POINT, p ) );
    if ( pList->Count() > 1 )
      points.Add(pList);
    else
      delete pList;
  } 

  if ( points.Count() > 1 ) {
    MbSNameMaker names;
    SimpleName name(0); 
    SolidSPtr result;

    MbLoftedPointsShellParams params( points, names, name );
    ::LoftedShell( params, result );

    if ( result!=nullptr )
      TestVariables::viewManager->AddObject( TestVariables::SOLID_Style, result );
  }
}


//------------------------------------------------------------------------------
// Создание Lofted оболочки по кривым
// Creation of a lofted shell by curves
// ---
void CreateLoftedShellByCurves() {
  PArray <MbCurve3D> curves( 4, 4, false ); // Множество не владеет объектами    //-V112
  MbItem *nearest;

  while ( GetGObj( IDS_SELECT_CURVE, st_Curve3D, &nearest ) && (nearest->IsA() == st_SpaceInstance) ) {
    TestVariables::viewManager->ToggleObject(nearest);
    MbCurve3D *curv = (MbCurve3D *)(((MbSpaceInstance *)nearest)->GetSpaceItem());
    curves.Add( curv );
    TestVariables::viewManager->ToggleObject(nearest);
  }

  if ( curves.Count() > 1 ) {
    MbSNameMaker names;
    SimpleName name(0); 
    SolidSPtr result;

    MbLoftedCurvesShellParams params( curves, names, name );
    ::LoftedShell( params, result ); 

    for ( ptrdiff_t i = 0; i < (ptrdiff_t)curves.Count(); i++ )
      DeleteItem( curves[i] );

    if ( result!=nullptr )
      TestVariables::viewManager->AddObject( TestVariables::SOLID_Style, result );
  }
}


//------------------------------------------------------------------------------
// Создание заплатки
// A patch creation
// ---
void CreatePatchShell( int mode )
{
  SPtr<MbSolid> solid;

  MbItem * obj = nullptr;
  RPArray<MbPatchCurve> patchCurves( 0, 1 );
  RPArray<MbCurve3D>    spaceCurves( 0, 1 );
  const MbMatrix3D mtr;

  PatchValues pValues;

  switch ( mode ) {

    case 1 : {
      RPArray<MbCurveEdge> edges( 0, 1 );
      std::vector<DPtr< MbPatchMating>> matings;
      MbTopItem * item = nullptr;

      { // Будем ли задавать сопряжения.
        bool resetToFalse = true;
        bool setMates = false;
        ::GetBoolean( RT_SET_MATES, setMates, resetToFalse );
        if ( setMates ) {
          pValues.SetType() = PatchValues::ts_byCurves;
          pValues.SetInternalMatingType( pmt_SmoothG2 );
        }
      }

      while ( GetTObj( IDS_SELECT_EDGE, st_Solid, tt_CurveEdge, &obj, &item ) && item->IsAnEdge() ) {
        MbCurveEdge * edge = static_cast<MbCurveEdge *>(item);
        size_t ind = edges.FindIt( edge );

        if ( ind != SYS_MAX_T ) {
          TestVariables::drawImp->DrawItem( item, TRGB_WHITE );
          TestVariables::viewManager->UnselectObject( obj );
          edges.DetachInd( ind );
          patchCurves.DetachInd( ind );
        }
        else {
          TestVariables::drawImp->DrawItem( item, TRGB_AZURE );
          TestVariables::viewManager->SelectObject( obj );

          // Задать сопряжение.
          const MbSurface * surface = nullptr;
          int mateType = 0;
          if ( pValues.GetType() == PatchValues::ts_byCurves ) {
            ::GetInt( RT_SET_CURVE_MATE_TYPE, mateType );
            if ( ( MbePatchMatingType ) mateType != pmt_None ) {
              // Нужна поверхность
              MbTopItem * faceItem = nullptr;
              if ( GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj, &faceItem ) && faceItem != nullptr ) {
                TestVariables::viewManager->ToggleObject( obj );
                MbFace * face = ( MbFace * ) faceItem;
                surface = &face->GetSurface();
              }
              else {
                MbItem * surfItem = nullptr;
                if ( GetGObj( IDS_SELECT_SURFACE, st_Surface, &surfItem ) && surfItem != nullptr ) {
                  surface = static_cast< const MbSurface * >( ( static_cast< MbSpaceInstance * >( surfItem ) )->GetSpaceItem() );
                }
              }
            }
          }

          edges.Add( edge );
          MbPatchCurve * patchCurve = new MbPatchCurve( *edge, mtr );
          if ( surface != nullptr )
            patchCurve->SetMating( ( MbePatchMatingType ) mateType, surface );
          patchCurves.Add( patchCurve );
        }
        for ( size_t m = 0, mcnt = edges.Count(); m < mcnt; m++ )
          TestVariables::drawImp->DrawItem( edges[m], TRGB_AZURE );
      }
    } break;

    case 2 : {
      while ( GetGObj ( IDS_SELECT_GENER_CURVE, st_Curve3D, &obj ) ) {
        if ( obj->IsA() == st_SpaceInstance ) {
          MbCurve3D * spaceCurve = static_cast<MbCurve3D *>( static_cast<MbSpaceInstance *>(obj)->SetSpaceItem() );
          size_t ind = spaceCurves.FindIt( spaceCurve );

          if ( ind != SYS_MAX_T ) {
            TestVariables::drawImp->DrawItem( spaceCurve, TRGB_WHITE );
            TestVariables::viewManager->UnselectObject( obj );
            ::DeleteItem( spaceCurves[ind] );
            spaceCurves.DetachInd( ind );
          }
          else {
            TestVariables::drawImp->DrawItem( spaceCurve, TRGB_AZURE );
            TestVariables::viewManager->SelectObject( obj );
            spaceCurves.Add( spaceCurve );
          }
          for ( size_t m = 0, mcnt = spaceCurves.Count(); m < mcnt; m++ )
            TestVariables::drawImp->DrawItem( spaceCurves[m], TRGB_AZURE );
        }
        else if ( obj->IsA() == st_PlaneInstance ) { // SD#7116293 
          MbPlaneInstance * planeInst = static_cast<MbPlaneInstance *>( obj );
          MbPlaneItem * planeItem = planeInst->SetPlaneItem();

          if ( planeItem->Family() == pt_Curve ) {
            SPtr<MbCurve3D> spaceCurve;
            if ( ::GetSpaceCurve( *obj, true, spaceCurve ) && (spaceCurve != nullptr) ) {
              size_t k, ind = SYS_MAX_T;
              for ( k = spaceCurves.size(); k--; ) {
                if ( spaceCurves[k]->IsSame( *spaceCurve ) ) {
                  ind = k;
                  break;
                }
              }
              if ( ind != SYS_MAX_T ) {
                TestVariables::drawImp->DrawItem( spaceCurve, TRGB_WHITE );
                TestVariables::viewManager->UnselectObject( obj );
                ::DeleteItem( spaceCurves[ind] );
                spaceCurves.DetachInd( ind );
              }
              else {
                TestVariables::drawImp->DrawItem( spaceCurve, TRGB_AZURE );
                TestVariables::viewManager->SelectObject( obj );
                spaceCurves.Add( ::DetachItem( spaceCurve ) );
              }
            }
          }
          else if ( planeItem->Family() == pt_Region ) { // SD#7116293 
            SetWaitCursor( true );

            const MbRegion & region = static_cast<const MbRegion &>( *planeItem );
            const MbPlacement3D & place = planeInst->GetPlacement();

            MbSurface * surface = nullptr;
            ::BoundedSurface( place, region, surface );

            if ( surface != nullptr ) {
              TestVariables::viewManager->SelectObject( obj );
              MbSNameMaker n( ct_SimpleCreator, MbSNameMaker::i_SideNone, 0 );
              MbSolid * surfSolid = nullptr;
              ::SurfaceShell( *surface, n, surfSolid );
              solid = surfSolid;
            }
            ::DeleteItem( surface );

            SetWaitCursor( false );
            break;
          }
        }
      }
    } break;
  }

  Style ns = TestVariables::SHELL_Style;

  if ( solid == nullptr ) {
    SetWaitCursor( true );
    MbSNameMaker n( ct_PatchSetCreator, MbSNameMaker::i_SideNone, 0 );
    MbResultType res = rt_Error;

    if ( patchCurves.Count() > 0 ) {
      ::AddRefItems( patchCurves );
      MbPatchShellParams patchParams( patchCurves, pValues, n );
      res = ::PatchShell( patchParams, solid );
      ::ReleaseItems( patchCurves );
    }
    else
    if ( spaceCurves.Count() > 0) {
      ::AddRefItems( spaceCurves );
      MbPatchShellParams patchParams( spaceCurves, pValues, n );
      res = ::PatchShell( patchParams, solid );
      ::ReleaseItems( spaceCurves );
    }

    if ( solid != nullptr ) {
      TestVariables::viewManager->AddObject( ns, solid );
      if ( res != rt_Success )
        ::PutErrorMessage( res, IDS_TITLE_WRN );
    }
    else
      ::PutErrorMessage( res, IDS_TITLE_ERR );

    SetWaitCursor( false );
  }
  else { // SD#7116293 
    TestVariables::viewManager->AddObject( ns, solid );
  }
}


//------------------------------------------------------------------------------
// Создание оболочки по сетке кривых
// Create shell by mesh of curves
// ---
void CreateMeshShell()
{
  RPArray<MbCurve3D> curvesU( 0, 1 ), curvesV( 0, 1 );
  RPArray<MbPolyline3D> chainsU( 0, 1 ), chainsV( 0, 1 );
  bool uCls = false;
  bool vCls = false;
  c3d::ConstSurfacesVector matSurf[4];
  MbeMatingType types[4];
  const size_t strLen = 256;
  TCHAR buf[strLen];
  bool tess = false, smooth = false;
  ptrdiff_t dig = 0;

#if defined ( __NATIVE_LANGUAGE__ )
  _sntprintf( buf, strLen, _T( "Строить ли дополнительные сечения?" ) );
#else
  _sntprintf( buf, strLen, _T( "Should you build additional sections?" ) );
#endif
  GetInt( buf, dig );
  tess = dig != 0;

#if defined ( __NATIVE_LANGUAGE__ )
  _sntprintf( buf, strLen, _T( "Требуется ли g2 гладкость граней?" ) );
#else
  _sntprintf( buf, strLen, _T( "Are the faces required to be g2 smooth?" ) );
#endif
  GetInt( buf, dig );
  smooth = dig != 0;

  for ( size_t k = 0; k < 2; k++ ) {
    RPArray<MbCurve3D> & curves = k == 0 ? curvesU : curvesV;
    bool & cls = k == 0 ? uCls : vCls;
#if defined ( __NATIVE_LANGUAGE__ )
    _sntprintf( buf, strLen, _T("Укажите %c кривую (задано %d)"), k == 0 ? _T( 'U' ) : _T( 'V' ), (int)curves.Count() );
#else
    _sntprintf( buf, strLen, _T( "Specify %c curve (set %d)" ), k == 0 ? _T( 'U' ) : _T( 'V' ), (int)curves.Count() );

#endif
    MbItem * nearest = nullptr;
    while ( GetAnyCurve( buf, &nearest ) ) {
      TestVariables::viewManager->ToggleObject( nearest );
      MbCurve3D * curv = nullptr;
      if ( nearest->IsA() == st_SpaceInstance ) {
        const MbSpaceItem * spaceItem = ( (MbSpaceInstance *)nearest )->GetSpaceItem(); // Дать трёхмерный геометрический объект. Get a spatial geometric object
        if ( ( spaceItem != nullptr ) && ( spaceItem->Family() == st_Curve3D ) ) {
          curv = (MbCurve3D *)spaceItem;
        }
      }
      if ( nearest->IsA() == st_PlaneInstance ) {
        const MbPlaneItem * planeItem = ( (const MbPlaneInstance *)nearest )->GetPlaneItem(); // Дать двумерный геометрический объект. Get a spatial geometric object.
        if ( planeItem != nullptr && ( planeItem->Family() == pt_Curve ) ) {
          const MbCurve * pCurve = static_cast<const MbCurve *>( planeItem );
          MbPlacement3D place;
          if ( pCurve != nullptr && ( (const MbPlaneInstance *)nearest )->GetPlacement( place ) ) {
            pCurve = static_cast<MbCurve *>( &pCurve->Duplicate() );
            ::PlaneCurve( place, *pCurve, curv );
            ::DeleteItem( pCurve );
          }
        }
      }
      if ( curv != nullptr )
        curves.Add( curv );
      TestVariables::viewManager->ToggleObject( nearest );

#if defined ( __NATIVE_LANGUAGE__ )
      _sntprintf( buf, strLen, _T("Укажите %c кривую (задано %d)"), k == 0 ? _T( 'U' ) : _T( 'V' ), (int)curves.Count() );
#else
      _sntprintf( buf, strLen, _T( "Specify %c curve (set %d)" ), k == 0 ? _T( 'U' ) : _T( 'V' ), (int)curves.Count() );
#endif
    }
    if ( curves.size() > 0 ) {
#if defined ( __NATIVE_LANGUAGE__ )
      _sntprintf( buf, strLen, _T("Укажите замкнутость по %с (0 - нет, 1 - да)"), k == 0 ? _T( 'U' ) : _T( 'V' ) );
#else
      _sntprintf( buf, strLen, _T( "Specify closeness by %c (0 - no, 1 - yes)" ), k == 0 ? _T( 'U' ) : _T( 'V' ) );
#endif
      GetInt( buf, dig );
      cls = ( dig > 0 );
    }
  }

  for ( size_t k = 0; k < 4; k++ ) {
    if( (k % 2 == 0 && (curvesU.empty() || vCls)) || (k % 2 == 1 && (curvesV.empty() || uCls)) ) {
      types[k] = trt_Position;
      continue;
    }
#if defined ( __NATIVE_LANGUAGE__ )
    _sntprintf( buf, strLen, _T("Укажите поверхность сопряжения для %s (задано %d)"), k % 2 == 0 ? ( k == 0 ? _T( "cu[0]" ) : _T( "cu[nu]" ) ) : ( k == 1 ? _T( "cv[0]" ) : _T( "cv[nv]" ) ), (int)matSurf[k].size() );
#else
    _sntprintf( buf, strLen, _T( "Specify mating surface for %s (set %d)" ), k % 2 == 0 ? ( k == 0 ? _T( "cu[0]" ) : _T( "cu[nu]" ) ) : ( k == 1 ? _T( "cv[0]" ) : _T( "cv[nv]" ) ), (int)matSurf[k].size() );
#endif

    MbItem * obj = nullptr;
    while ( GetGObj( buf, st_Surface, &obj ) ) {
      if ( obj != nullptr ) {
        const MbSpaceItem * spaceItem = ( (MbSpaceInstance *)obj )->GetSpaceItem();
        if ( spaceItem != nullptr && ( spaceItem->Family() == st_Surface ) ) {
          matSurf[k].push_back( &((const MbSurface *)( spaceItem ))->GetBasisSurface() );
        }
      }
#if defined ( __NATIVE_LANGUAGE__ )
      _sntprintf( buf, strLen, _T("Укажите поверхность сопряжения для %s (задано %d)"), k % 2 == 0 ? ( k == 0 ? _T( "cu[0]" ) : _T( "cu[nu]" ) ) : ( k == 1 ? _T( "cv[0]" ) : _T( "cv[nv]" ) ), (int)matSurf[k].size() );
#else
      _sntprintf( buf, strLen, _T( "Specify mating surface for %s (set %d)" ), k % 2 == 0 ? ( k == 0 ? _T( "cu[0]" ) : _T( "cu[nu]" ) ) : ( k == 1 ? _T( "cv[0]" ) : _T( "cv[nv]" ) ), (int)matSurf[k].size() );
#endif
    }
    if ( !matSurf[k].empty() ) {
#if defined ( __NATIVE_LANGUAGE__ )
      _sntprintf( buf, strLen, _T("Укажите тип сопряжения к %s (1-касательное, 2-нормальное, 3-гладкое)"), k % 2 == 0 ? ( k == 0 ? _T( "cu[0]" ) : _T( "cu[nu]" ) ) : ( k == 1 ? _T( "cv[0]" ) : _T( "cv[nv]" ) ) );
#else
      _sntprintf( buf, strLen, _T( "Specify mating type for %s (1-tangent, 2-normal, 3-smooth)" ), k % 2 == 0 ? ( k == 0 ? _T( "cu[0]" ) : _T( "cu[nu]" ) ) : ( k == 1 ? _T( "cv[0]" ) : _T( "cv[nv]" ) ) );
#endif
      GetInt( buf, dig );
      switch ( dig ) {
      case 1:
        types[k] = trt_Tangent;
        break;
      case 2:
        types[k] = trt_Normal;
        break;
      case 3:
        types[k] = trt_SmoothG2;
        break;
      default:
        types[k] = trt_Position;
      }
    }
    else
      types[k] = trt_Position;
  }
  // Блок задания цепочек.
  for ( size_t k = 0; k < 2; k++ ) {
    RPArray<MbPolyline3D> & chains = k == 0 ? chainsU : chainsV;
#if defined ( __NATIVE_LANGUAGE__ )
    _sntprintf( buf, strLen, _T("Укажите %c цепочку (задано %d)"), k == 0 ? _T('U') : _T('V'), (int)chains.Count() );
#else
    _sntprintf( buf, strLen, _T("Specify %c chain (set %d)"), k == 0 ? _T('U') : _T('V'), (int)chains.Count() );
    
#endif
    MbItem * nearest = nullptr;
    while ( GetAnyCurve(buf, &nearest) ) {
      TestVariables::viewManager->ToggleObject( nearest );
      MbPolyline3D * chain = nullptr;
      if ( nearest->IsA() == st_SpaceInstance ) {
        const MbSpaceItem * spaceItem = ( (MbSpaceInstance *)nearest )->GetSpaceItem(); // Дать трёхмерный геометрический объект. Get a spatial geometric object
        if ( (spaceItem != nullptr) && (spaceItem->Family() == st_Curve3D) ) {
          MbCurve3D * curv = (MbCurve3D *)spaceItem;
          if ( curv->IsA() == st_Contour3D ) {
            MbContour3D * cont = static_cast<MbContour3D*>( curv );
            std::vector<MbCartPoint3D> points( cont->GetSegmentsCount() + (cont->IsClosed() ? 0 : 1) );
            for ( size_t i = 0, icount = cont->GetSegmentsCount(); i < icount; i++ )
              points[i].Init( cont->GetSegment(i)->GetLimitPoint(1) );
            if( !cont->IsClosed() )
              points.back().Init( cont->GetSegment( cont->GetSegmentsCount() - 1 )->GetLimitPoint(2) );
            chain = new MbPolyline3D( points, cont->IsClosed() );
          }
          else if( !curv->IsClosed())
            chain = new MbPolyline3D( curv->GetLimitPoint(1), curv->GetLimitPoint(2) );
        }
      }
      if ( chain != nullptr )
        chains.Add( chain );
      TestVariables::viewManager->ToggleObject( nearest );
#if defined ( __NATIVE_LANGUAGE__ )
      _sntprintf( buf, strLen, _T("Укажите %c цепочку (задано %d)"), k == 0 ? _T('U') : _T('V'), (int)chains.Count() );
#else
      _sntprintf( buf, strLen, _T( "Specify %c chain (set %d)" ), k == 0 ? _T('U') : _T('V'), (int)chains.Count() );
#endif
    }
  }

/*  // Блок автоматической вставки полюсов (раскомментировать при отладке, если нужно).
  if ( curvesU.Count() > 1 ) {
    MbCartPoint3D beg1, end1, beg2, end2;
    curvesU[0]->_PointOn( curvesU[0]->GetTMin(), beg1 );
    curvesU[0]->_PointOn( curvesU[0]->GetTMax(), end1 );
    curvesU[1]->_PointOn( curvesU[1]->GetTMin(), beg2 );
    curvesU[1]->_PointOn( curvesU[1]->GetTMax(), end2 );
    if ( c3d::EqualPoints( beg1, beg2, Math::metricEpsilon ) || c3d::EqualPoints( beg1, end2, Math::metricEpsilon ) ) {
      if ( !curvesV.empty() && beg1.DistanceToPoint( curvesV.front()->GetLimitPoint( 1 ) ) < beg1.DistanceToPoint( curvesV.back()->GetLimitPoint( 1 ) ) )
        curvesV.Insert( 0, new MbLineSegment3D( beg1, beg1 ) );
      else
        curvesV.Add( new MbLineSegment3D( beg1, beg1 ) );
    }
    if ( c3d::EqualPoints( end1, beg2, Math::metricEpsilon ) || c3d::EqualPoints( end1, end2, Math::metricEpsilon ) ) {
      if ( !curvesV.empty() && end1.DistanceToPoint( curvesV.front()->GetLimitPoint( 1 ) ) < end1.DistanceToPoint( curvesV.back()->GetLimitPoint( 1 ) ) )
        curvesV.Insert( 0, new MbLineSegment3D( end1, end1 ) );
      else
        curvesV.Add( new MbLineSegment3D( end1, end1 ) );
    }
  }
  if ( curvesV.Count() > 1 ) {
    MbCartPoint3D beg1, end1, beg2, end2;
    curvesV[0]->_PointOn( curvesV[0]->GetTMin(), beg1 );
    curvesV[0]->_PointOn( curvesV[0]->GetTMax(), end1 );
    curvesV[1]->_PointOn( curvesV[1]->GetTMin(), beg2 );
    curvesV[1]->_PointOn( curvesV[1]->GetTMax(), end2 );
    if ( c3d::EqualPoints( beg1, beg2, Math::metricEpsilon ) || c3d::EqualPoints( beg1, end2, Math::metricEpsilon ) ) {
      if ( !curvesU.empty() && beg1.DistanceToPoint( curvesU.front()->GetLimitPoint( 1 ) ) < beg1.DistanceToPoint( curvesU.back()->GetLimitPoint( 1 ) ) )
        curvesU.Insert( 0, new MbLineSegment3D( beg1, beg1 ) );
      else
        curvesU.Add( new MbLineSegment3D( beg1, beg1 ) );
    }
    if ( c3d::EqualPoints( end1, beg2, Math::metricEpsilon ) || c3d::EqualPoints( end1, end2, Math::metricEpsilon ) ) {
      if ( !curvesU.empty() && end1.DistanceToPoint( curvesU.front()->GetLimitPoint( 1 ) ) < end1.DistanceToPoint( curvesU.back()->GetLimitPoint( 1 ) ) )
        curvesU.Insert( 0, new MbLineSegment3D( end1, end1 ) );
      else
        curvesU.Add( new MbLineSegment3D( end1, end1 ) );
    }
  }
*/

  ::AddRefItems( curvesU );
  ::AddRefItems( curvesV );
  ::AddRefItems( chainsU );
  ::AddRefItems( chainsV );

  if ( curvesU.Count() > 1 ) {

    MeshSurfaceValues params;
    bool defDir[4] = { true, true, true, true };
    const c3d::ConstSurfacesVector * surfaces[4] = { &matSurf[0], &matSurf[1], &matSurf[2], &matSurf[3] };
    params.Init( curvesU, curvesV, uCls, vCls, types, surfaces, defDir, false, tess, smooth, &chainsU, &chainsV, nullptr, true );

    MbSolid * solid = nullptr;
    MbSNameMaker n; 
    SolidSPtr solSPtr;
    MbResultType res = ::MeshShell( MbMeshShellParameters(params, n, false), solSPtr );
    solid = solSPtr.detach();
    if ( solid != nullptr && res == rt_Success ) {
      TestVariables::viewManager->AddObject( TestVariables::SHELL_Style, solid );
    }
    else
    ::PutErrorMessage( res, IDS_TITLE_ERR );

  }

  ::ReleaseItems( curvesU );
  ::ReleaseItems( curvesV );
  ::ReleaseItems( chainsU );
  ::ReleaseItems( chainsV );
}


//------------------------------------------------------------------------------
// Создание линейчатой оболочки по двум кривым.
// Create a ruled shell by two curves.
// ---
void CreateRuledShell()
{
  SPtr<MbCurve3D> curve0;
  SPtr<MbCurve3D> curve1;

  MbItem * nearest0 = nullptr;
  MbItem * nearest1 = nullptr;
  
  if ( GetGObj(IDS_SELECT_CURVE, st_Curve3D, &nearest0 ) ) {
    TestVariables::viewManager->ToggleObject( nearest0 );
    curve0 = ::GetCurve3D( nearest0, false );

    if ( GetGObj(IDS_SELECT_CURVE, st_Curve3D, &nearest1) ) {
      TestVariables::viewManager->ToggleObject( nearest1 );
      curve1 = ::GetCurve3D( nearest1, false );
    }
  }
  
  if ( curve0 != nullptr && curve1 != nullptr ) {
    RuledSurfaceValues params;
    SArray<double> breaks0( 0, 1 ), 
                   breaks1( 0, 1 );
    params.Init( *curve0, *curve1, breaks0, breaks1 );
    ptrdiff_t iByVerts = 0;
    GetInt( IDS_ENTER_JOIN_BY_VERTICES, iByVerts );
    params.SetJoinByVertices( (iByVerts != 0) );
    ptrdiff_t prop = 0;
    GetInt( IDS_SET_PROPORTIONAL_PARAMS, prop );
    params.SetProportional( (prop != 0) ); // KOMPAS-46442
    ptrdiff_t split = 1;
    GetInt( IDS_SET_DIVIDE_SHELL, split );
    params.SetSegmentSplit( (split != 0) ); // KOMPAS-46442
    double gap = METRIC_ACCURACY;
    GetDouble( IDS_SET_PERMISSIBLE_MISMATCH, gap );
    params.SetMismatch( gap ); // KOMPAS-46442
    SolidSPtr solid;
    MbSNameMaker n;
    MbRuledShellResult outParams;
    MbResultType res = ::RuledShell( MbRuledShellParams(params, n, false), outParams );
    solid = outParams.SetSolid();

    if ( solid != nullptr && res == rt_Success )
      TestVariables::viewManager->AddObject( TestVariables::SHELL_Style, solid );
    else
      ::PutErrorMessage( res, IDS_TITLE_ERR );
  }

  TestVariables::viewManager->ToggleObject( nearest0 );
  TestVariables::viewManager->ToggleObject( nearest1 );
}


//------------------------------------------------------------------------------
// Продлить грань вдоль указанных рёбер
// Extend a face along selected edges
// ---
void CreateExtensionShell()
{
  MbItem * obj = nullptr, * obj0 = nullptr;
  MbTopItem * curve = nullptr;
  MbFace * face = nullptr;
  Style ns;
  RPArray<MbCurveEdge> initCurves(4,4);    //-V112
  
  ptrdiff_t i0;

  if ( GetTObj( IDS_SELECT_EDGE, st_Solid, tt_CurveEdge, &obj, &curve ) ) {
    MbCurveEdge * edge = (MbCurveEdge *)curve;
    if ( edge->IsBoundaryFace() ) {
      face = edge->GetFacePlus();
      if ( face == nullptr )
        face = edge->GetFaceMinus();
      initCurves.Add(edge);
//#ifdef _DRAWGI
      TestVariables::drawImp->DrawItem( face, TRGB_GOLD );
      TestVariables::drawImp->DrawItem( curve, TRGB_AVIATION );
//#endif
      while ( GetTObj( IDS_SELECT_EDGE, st_Solid, tt_CurveEdge, &obj0, &curve ) ) {
        if (obj==obj0) {
//#ifdef _DRAWGI
          TestVariables::drawImp->DrawItem( curve, TRGB_AVIATION );
//#endif
          edge = (MbCurveEdge *)curve;
          i0 = initCurves.FindIt(edge);
          if (i0 != SYS_MAX_T) {
//#ifdef _DRAWGI
            TestVariables::drawImp->DrawItem( curve, TRGB_LIGHTGREEN );
//#endif
            initCurves.DetachInd(i0);
          }
          else
            initCurves.Add(edge);
        }
      }
    }
  }

  if ( (face != nullptr) && (initCurves.Count() > 0) ) {
    int type_ = TestVariables::type;
    int way_  = TestVariables::way ;
    int kind_ = TestVariables::kind;
    MbVector3D v;

    if ( GetInt( IDS_ENTER_EXTENSIONTYPE, type_ ) &&
         GetInt( IDS_ENTER_EXTENSIONWAY,  way_  ) && 
         GetInt( IDS_ENTER_EXTENSIONKIND, kind_ ) ) {
      ExtensionValues param;
      TestVariables::type = type_;
      TestVariables::way  = way_;
      TestVariables::kind = kind_;

      switch ( way_ ) {
        case 1 : {
          MbTopItem * vertex;
          MbCartPoint3D p;
          if ( GetTObj( IDS_SELECT_VERTEX, st_Solid, tt_Vertex, &obj0, &vertex ) ) 
            ((MbVertex *)vertex)->GetCartPoint( p );
          param.InitByVertex( (ExtensionValues::ExtensionType)type_, (ExtensionValues::LateralKind)kind_, p );
        } break;
        case 2 : {
          MbTopItem * face0;
          if ( GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj0, &face0 ) ) 
            param.InitByShell( (ExtensionValues::ExtensionType)type_, (ExtensionValues::LateralKind)kind_, 
                                 (const MbFace *)face0, (const MbSolid *)obj0 );
        } break;
        default : {
          double d = 50;
          GetDouble( IDS_ENTER_SPAN, d );
          param.InitByDistance( (ExtensionValues::ExtensionType)type_, (ExtensionValues::LateralKind)kind_, MbVector3D(), d );
        }
      }

      if ( type_ == ExtensionValues::et_direction ) {
        double x = 1.0, y = 1.0, z = 1.0;
        GetDouble( IDS_ENTER_X_COORDINATE, x );
        GetDouble( IDS_ENTER_Y_COORDINATE, y );
        GetDouble( IDS_ENTER_Z_COORDINATE, z );
        v.Init( x, y, z );
        v.Normalize();
        param.direction = v;
      }

      SetWaitCursor( true );

      ns.Init( obj->GetWidth(), obj->GetColor() );
      MbSNameMaker names( ct_FilletSolid, MbSNameMaker::i_SideNone, 0 );
      SolidSPtr solid = nullptr; 
      MbExtensionShellParams extShellPar( param, names );
      extShellPar.Add( initCurves );
      MbResultType res = ::ExtensionShell( static_cast<MbSolid &>(*obj), cm_Copy, extShellPar, solid );
    
      if ( solid != nullptr ) {
        TestVariables::viewManager->DeleteObject( obj );
        TestVariables::viewManager->AddObject( ns, solid );
      }
      else 
        ::PutErrorMessage( res, IDS_TITLE_ERR );

      SetWaitCursor( false );
    }
  }
}


//------------------------------------------------------------------------------
// Создание оболочки скругления между двумя гранями
// Create a fillet shell of two faces
// ---
void CreateFacesFillet ( int form )
{
  MbItem * obj1 = nullptr;
  MbItem * obj2 = nullptr;
  MbTopItem * top1 = nullptr;
  MbTopItem * top2 = nullptr;

  bool get1 = GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj1, &top1 );
  TestVariables::drawImp->DrawItem( top1, TRGB_WHITE );
  bool get2 = GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj2, &top2 );
  TestVariables::drawImp->DrawItem( top2, TRGB_WHITE );

  if ( get1 && get2 ) {
    bool res = false;
    TestVariables::conic = _ARC_;

    if ( GetDouble(IDS_ENTER_RADIUS, TestVariables::radius1) ) {
      TestVariables::radius2 = TestVariables::radius1;
      if ( form == st_Span ) // Скругление с заданной хордой. A fillet with given horda.
        res = true;
      else
        res = GetDouble( IDS_ENTER_RADIUS, TestVariables::radius2 );
    }
    if ( res && 
         GetDouble(IDS_SELECT_CONIC, TestVariables::conic) &&
         GetInt(IDS_GENERAL_PROLONG, TestVariables::prolong) ) {
      SetWaitCursor( true );

      Style ns( 1, obj1->GetColor() );
      MbSNameMaker n( ct_FilletShell, MbSNameMaker::i_SideNone, 0 );
      SmoothValues::CornerForm cForm = SmoothValues::ec_uniform; 
      SmoothValues params( TestVariables::radius1, TestVariables::radius2, st_Fillet, TestVariables::conic, TestVariables::prolong != 0,
                           cForm, TestVariables::keepKerb == 0, TestVariables::keepKerb > 0, true, false/*equable*/ );
      if ( form == st_Span ) // Скругление с заданной хордой. A fillet with given horda.
        params.form = st_Span;

      ConstSolidSPtr solid1( static_cast<MbSolid *> (obj1) );
      ConstSolidSPtr solid2( static_cast<MbSolid *> (obj2) );
      ConstFaceSPtr face1( static_cast<MbFace *> (top1) );
      ConstFaceSPtr face2( static_cast<MbFace *> (top2) );

      if ( (solid1 != nullptr) && (solid2 != nullptr) && (face1 != nullptr) && (face2 != nullptr) ) {
        c3d::SolidSPtr resSolid;
        MbResultType res_ = ::FacesFillet( MbFacesFilletParams(solid1, face1, solid2, face2, params, n), resSolid );

        if ( resSolid != nullptr ) {
          if ( obj1 == obj2 && resSolid->IsClosed() )
            TestVariables::viewManager->DeleteObject( obj1 );
          TestVariables::viewManager->AddObject( ns, resSolid );
        }
        else {
          ::PutErrorMessage( res_, IDS_TITLE_ERR );
        }
      }

      SetWaitCursor( false );
    }
  }
}


//------------------------------------------------------------------------------
// Создание новой оболочки сшивкой
// Stitch faces to a shell
// ---
void CreateShellSew()
{
  MbItem * item = nullptr;
  RPArray<MbItem> items( 2, 1 );
  //RPArray<const MbSolid> solids( 2, 1 );
  c3d::SolidsSPtrVector solids;
  solids.reserve( 4 );

  TestVariables::viewManager->GetSelectedObjects( items );
  if ( items.size() > 0 ) {
    size_t selectedCnt = items.size();
    for ( size_t k = 0; k < selectedCnt; k++ ) {
      item = items[k];
      if ( (item == nullptr) || (item->IsA() != st_Solid) ) {
        items.clear();
        solids.clear();
        break;
      }
      else {
        SolidSPtr solid( static_cast<MbSolid *>(item) );
        if ( solid != nullptr ) {
          if ( std::find( solids.begin(), solids.end(), solid ) == solids.end() )
            solids.push_back( solid );
        }
      }
    }
  }

  if ( items.size() < 1 ) {
    while ( ::GetGObj( IDS_SELECT_SOLID, st_Solid, &item ) && (item != nullptr) && (item->IsA() == st_Solid ) ) {
      TestVariables::viewManager->ToggleObject( item );
      SolidSPtr solid( static_cast<MbSolid *>(item) );
      if ( solid != nullptr ) {
        if ( std::find( items.begin(), items.end(), item ) == items.end() ) {
          solids.push_back( solid );
          items.push_back( item );
        }
      }
    }
  }

  if ( solids.size() > 0 ) {
    double accuracy = TestVariables::fidelity; //DELTA_MOD;

    if ( ::GetDouble( IDS_ENTER_ACCURACY, accuracy )) {
      TestVariables::fidelity = accuracy;
      SetWaitCursor( true );

      c3d::SolidSPtr resultSolid;
      MbSNameMaker operNames( ct_StitchedSolid, MbSNameMaker::i_SideNone, 0 );
      bool formBody = false; // In this function we want to create "surfaces" set (open shell)

      MbShellStitchParams stitchParams( operNames, formBody, accuracy, TestVariables::mergeEdges );

      MbeStitchResType res = ::StitchShells( solids, stitchParams, resultSolid );

      if ( resultSolid != nullptr ) {
        Style ns( items[0]->GetWidth(), items[0]->GetColor() );
        for ( size_t i = items.size(); i--; )
          TestVariables::viewManager->DeleteObject( items[i] );
        TestVariables::viewManager->AddObject( ns, resultSolid );
      }
      else {
        ::StitchErrorMessage( res, IDS_TITLE_ERR );
      }

      SetWaitCursor( false );
    }
  }
}


//------------------------------------------------------------------------------
// Создание нового тела сшивкой.
// Stitch solids to a solid.
// ---
void CreateSolidSew()
{
  MbItem * item = nullptr;
  RPArray<MbItem> items( 2, 1 );
  RPArray<MbSolid> solids( 2, 1 );
  SArray<MbMatrix3D> matrs( 2, 1 );

  TestVariables::viewManager->GetObjects( st_Solid, items, matrs ); // Дать все объекты указанного типа.
  if ( items.size() > 0 ) {
    size_t itemsCnt = items.size();
    for ( size_t k = 0; k < itemsCnt; k++ ) {
      item = items[k];
      if ( (item == nullptr) || (item->IsA() != st_Solid) ) {
        items.clear();
        solids.clear();
        break;
      }
      else {
        MbSolid * solid = static_cast<MbSolid *>(item);
        if ( solid != nullptr && solids.FindIt( solid ) == SYS_MAX_T ) {
          if ( k < matrs.size() && !matrs[k].IsSingle() )
            solid->Transform( matrs[k] );
          solids.push_back( solid );
        }
      }
    }
  }

  if ( items.size() < 1 ) {
    while ( ::GetGObj( IDS_SELECT_SOLID, st_Solid, &item ) && (item != nullptr) && (item->IsA() == st_Solid ) ) {
      TestVariables::viewManager->ToggleObject( item );
      const MbSolid * solid = static_cast<const MbSolid *>(item);
      if ( solid != nullptr && items.FindIt( item ) == SYS_MAX_T ) {
        solids.push_back( solid );
        items.push_back( item );
      }
    }
  }

  if ( solids.size() > 0 ) {
    double accuracy = TestVariables::fidelity;

    if ( ::GetDouble( IDS_ENTER_ACCURACY, accuracy )) {
      TestVariables::fidelity = accuracy;
      SetWaitCursor( true );

      c3d::SolidSPtr resultSolid;
      MbSNameMaker operNames( ct_StitchedSolid, MbSNameMaker::i_SideNone, 0 );
      bool formBody = true; // In this function we want to create solid (closed shell)

      c3d::SolidsSPtrVector initialSolids; 
      MbShellStitchParams stitchParams( operNames, formBody, accuracy, TestVariables::mergeEdges );
      initialSolids.reserve( solids.size() );
      for ( size_t j = 0, jCount = solids.size(); j < jCount; j++ )
        initialSolids.push_back( SolidSPtr(solids[j]) );

      MbeStitchResType res = ::StitchShells( initialSolids, stitchParams, resultSolid );

      if ( resultSolid != nullptr ) {
        Style ns( items[0]->GetWidth(), items[0]->GetColor() );
        for ( size_t i = items.size(); i--; )
          TestVariables::viewManager->DeleteObject( items[i] );
        TestVariables::viewManager->AddObject( ns, resultSolid );
      }
      else
        ::StitchErrorMessage( res, IDS_TITLE_ERR );

      SetWaitCursor( false );
    }
  }
}


//------------------------------------------------------------------------------
// Построение силуэтного контура для заданного направления взгляда и разрезка им тела
// Create silhouette contour for the given direction and split the solid body along it
// ---
bool MakeSolidsSilhouetteContour()
{
  MbItem * obj = nullptr;

  if ( GetGObj( IDS_SELECT_SOLID, st_Solid, &obj ) ) {
    TestVariables::viewManager->ToggleObject(obj);

    MbSolid * solid = static_cast<MbSolid *>(obj);

    if ( solid != nullptr ) {
      SpaceCurvesSPtrVector outlineCurves;
      outlineCurves.reserve( 4 );

      RPArray<MbSolid> solids( 2, 1 );

      MbVector3D z( (TestVariables::viewManager->GetActiveWindow())->GetAxisZ() );
      Style ns( obj->GetWidth(), obj->GetColor() );

      int iEnterViewDir = 0;
      GetInt( IDS_ENTER_USE_OR_NOT, iEnterViewDir );
      if ( iEnterViewDir != 0 )
        GetExactPoint( (MbCartPoint3D &)z );

      MbSNameMaker n;
      ::CutShellSilhouetteContour( *solid, cm_Copy, z, n, outlineCurves, solids );

      TestVariables::viewManager->ToggleObject(obj);

      for ( size_t i = 0; i < outlineCurves.size(); i++)
        TestVariables::viewManager->AddObject( TestVariables::CONTOUR_Style, outlineCurves[i] );

      for ( size_t j = 0; j < solids.Count(); j++) {
        TestVariables::viewManager->AddObject( ns, solids[j] );
        ns.SetColor( ns.GetColor() / 2 );
      }
      TestVariables::viewManager->DeleteObject( obj );

      return true;
    }

    TestVariables::viewManager->ToggleObject(obj);
  }

  return false;
}


//------------------------------------------------------------------------------
// Построение элементов токарного сечения и оси вращения.
// Create lathe elements and rotation axis
// ---
bool MakeLatheElements( MbPlacement3D & position )
{
  MbItem * obj = nullptr;
  MbTopItem * top( nullptr );
  PathMatrix pathFrom;
  bool res( false );

  bool res0 = GetTObj( IDS_SELECT_SOLID, st_Solid, tt_TopItem, &obj, &top, pathFrom.second );
  if ( res0 || GetGObj( IDS_SELECT_SOLID, st_Solid, &obj, pathFrom ) ) {
    TestVariables::viewManager->ToggleObject(obj);

    if ( obj->IsA() == st_Solid ) {
      const MbSolid * solid = (const MbSolid *)obj;
      if ( solid != nullptr ) {
        MbAxis3D axis;
        double angle( M_PI_4 ); 
        bool cylinderAxis( false );

        if ( top != nullptr && top->RefType() == rt_TopItem ) {
          if ( top->IsA() == tt_Face ) {
            cylinderAxis = ((const MbFace *)top)->GetCylinderAxis( axis ); 
          }
        }

        //RPArray<MbCurve> curves( 2, 1 );
        //MbResultType resul = ::LatheCurves( *solid, cylinderAxis ? (&axis) : nullptr, angle, position, curves );

        MbLatheCurvesParams params( *solid, cylinderAxis ? (&axis) : nullptr, angle, Math::deviateSag );
        MbLatheCurvesResult result;

        MbResultType resul = ::LatheCurves( params, result );

        position = result.GetPosition();
        //result.DetachCurves( curves );
        std::vector<SPtr<MbCurve> > & curves = result.SetCurves();

        if ( resul == rt_Success ) {
          MbRect rect; // Для отрисовки оси вращения. For rotation axis painting.
          for ( size_t i = 0; i < curves.size(); i++ ) {
            TestVariables::viewManager->AddObject( Style(2,TestVariables::LIGHTMAGENTA), curves[i], &position );
            curves[i]->AddYourGabaritTo( rect ); // Для отрисовки оси вращения. For rotation axis painting.
          }
          const MbVector3D & axisX = position.GetAxisX();
          MbCartPoint3D p1( position.GetOrigin() ), p2( position.GetOrigin() );
          if ( rect.IsEmpty() )
            rect.Set( -10, -10, 10, 10 );
          p1.Add( axisX, rect.left );
          p2.Add( axisX, rect.right );
          MbLineSegment3D * axis_ = new MbLineSegment3D( p1, p2 ); // Для отрисовки оси вращения. For rotation axis_ painting.
          TestVariables::viewManager->AddObject( Style(2,TestVariables::LIGHTBLUE), axis_ );

          //viewManager->DeleteObject( obj );
          res = true;
        }
      }
    }
    TestVariables::viewManager->ToggleObject(obj);
  }

  return res;
}



//------------------------------------------------------------------------------
// Построение следа от вращения ребер тела.
// Create curves bounding rotating edges of a solid body.
// ---
bool MakeLatheElement( const MbPlacement3D & position, VERSION version )
{
  MbItem * obj = nullptr;
  MbTopItem * top( nullptr );
  MbMatrix3D matr;
  bool res( false );

  if ( GetTObj( IDS_SELECT_TOPO_OBJECT, st_Solid, tt_TopItem, &obj, &top, matr ) ) {
    TestVariables::viewManager->ToggleObject(obj);

    if ( top != nullptr && top->RefType() == rt_TopItem ) {
      if ( top->IsA() == tt_CurveEdge ) {
        MbCurveEdge * edge = (MbCurveEdge *)top;
        MbEdgeSequence edges( *edge, true, edge->IsClosed() );
        edges.CollectEdges( Math::deltaMin ); // Собираем все гладко стыкующиеся с edge рёбра. Collect all edges smoothly connected with the edge.
        RPArray<MbCurve> curves( 2*edges.Count(), 1 );
        MbResultType result( rt_Error );

        for ( size_t i = 0, iCount = edges.Count(); i < iCount; i++ ) { 
          MbResultType res_ = ::LatheCurve( edges.Edge(i)->GetCurve(), position, curves );
          if ( res_ == rt_Success ) 
            result = rt_Success;
        }
        if ( result == rt_Success ) {
          for ( size_t i = 0; i < curves.Count(); i++ ) {
            TestVariables::viewManager->AddObject( Style(2,TestVariables::MAGENTA), curves[i], &position );
            res = true;
          }
        }
      }
      if ( top->IsA() == tt_Face ) {
        MbFace * face = (MbFace *)top;
        MbAxis3D axis( position.GetOrigin(), position.GetAxisX() );
        RPArray<MbCurve3D> silhouettes( 5, 1 ); // Силуэтные линии грани. Silhouette curves of a face.

        ::SilhouetteCurve( *face, axis, silhouettes, version);

        RPArray<MbCurve> curves( 2*silhouettes.Count(), 1 );
        MbResultType result( rt_Error );
        size_t i( 0 ), iCount;

        for ( i = 0, iCount = silhouettes.Count(); i < iCount; i++ ) { 
          TestVariables::viewManager->AddObject( Style(1,TestVariables::LIGHTBLUE), silhouettes[i] );
        }

        for ( i = 0, iCount = silhouettes.Count(); i < iCount; i++ ) { 
          MbResultType res_ = ::LatheCurve( *(silhouettes[i]), position, curves );
          if ( res_ == rt_Success ) 
            result = rt_Success;
        }
        if ( result == rt_Success ) {
          for ( i = 0; i < curves.Count(); i++ ) {
            TestVariables::viewManager->AddObject( Style(2,TestVariables::MAGENTA), curves[i], &position );
            res = true;
          }
        }
      }
    }
    TestVariables::viewManager->ToggleObject(obj);
  }

  return res;
}


//------------------------------------------------------------------------------
// Построение контура по элементам токарного сечения.
// Create a contour from lathe elements.
// ---
bool MakeLatheContour( const MbPlacement3D & position )
{
  MbItem * obj = nullptr;
  MbCartPoint3D points[1];
  //MbTopItem * curve( nullptr );
  //MbMatrix3D matr;
  RPArray<MbItem> objs( 0, 1 );
  PArray<MbCurve> curveList( 20, 1, false );
  bool res( false );
  MbPlacement3D place;

  if ( GetPoint(IDS_SELECT_OBJ_ON_PLANE, points[0]) &&
       FindNearestObj(points[0], pt_Curve, &obj) ) {
    obj->GetPlacement( place ); // Запоминаем ЛСК. Save the placement.
    if ( position.Complanar(place) ) 
      TestVariables::viewManager->CreateCurveArray( obj, curveList, true );
  }

  if ( curveList.Count() > 0 ) {
    ::AddRefItems( curveList );

    double accuracy = TestVariables::accuracy; //DELTA_MIN;
    GetDouble( IDS_ENTER_ACCURACY, accuracy );
    TestVariables::accuracy = accuracy;

    PArray <MbCurve> usedCurves( 5, 1, false);
    PArray<MbContour> contours( 5, 1, false );
    bool strict( false );
    VERSION version = Math::DefaultMathVersion();

    MpGraph * graph = ::OuterContoursBuilder( curveList, contours, accuracy, strict, version, nullptr );

    if ( contours.Count() > 0 ) {
      MbVector3D to( place.GetAxisZ() );
      to *= -50;
      place.Move( to );
      for ( size_t i = 0; i < contours.Count(); i++ ) {
        TestVariables::viewManager->AddObject( Style(1,TestVariables::LIGHTRED), contours[i], &place );
        res = true;
      }
    }
    delete graph;
    ::ReleaseItems( curveList );
  }

  return res;
}


//------------------------------------------------------------------------------
// Создание срединной оболочки между парами граней тела.
// Creation the median shell between face pairs of solid.
// ---
void CreateMedianShell()
{
  SetWaitCursor( true );

  MbItem * obj = nullptr;

  // Выбираем тело.
  if ( GetGObj( IDS_SELECT_SOLID, st_Solid, &obj, TestVariables::itemPathFrom ) ) {
    TCHAR msgTitleInfo[] = IDS_TITLE_MSG;
    TCHAR msgTitleError[] = IDS_TITLE_ERR;

    // Выбираем грани между которыми будет построена срединная поверхность.
    TCHAR msgSelectFaces[] = RT_SELECT_MEDIAN_FACES;
    MessageBoxEx( msgSelectFaces, msgTitleInfo );

    MbSolid * solidInit = ( MbSolid * )obj;
    //MbFaceShell * shellInit = solidInit->GetShell();

    PArray<MbFace> selectedFaces( 10, 10, false );
    std::vector< IndicesPair > medianFacesInd;
    MbItem * objCur = nullptr;
    MbTopItem * item1 = nullptr, * item2 = nullptr;

    //size_t count = 0;
    while ( GetTObj( IDS_SELECT_FIRST_FACE, st_Solid, tt_Face, &objCur, &item1, TestVariables::itemPathFrom.second ) && 
            objCur == obj ) {
      MbFace * firstFace = ( MbFace * )item1;
      size_t ind1 = selectedFaces.FindIt( firstFace );

      if ( ind1 == SYS_MAX_T ) {
        selectedFaces.Add( firstFace );
        TestVariables::drawImp->DrawClearMap();
        for ( size_t i = 0, icnt = selectedFaces.Count(); i < icnt; i++ )
          TestVariables::drawImp->DrawItem( selectedFaces[i], 0, 65, 255 );
        if ( GetTObj( IDS_SELECT_SECOND_FACE, st_Solid, tt_Face, &objCur, &item2, TestVariables::itemPathFrom.second ) && 
            objCur == obj )
        {
          MbFace * secondFace = ( MbFace * )item2;
          size_t ind2 = selectedFaces.FindIt( secondFace );
          if ( ind2 == SYS_MAX_T ) {
            selectedFaces.Add( secondFace );
            medianFacesInd.push_back( IndicesPair( solidInit->GetFaceIndex(*firstFace),
                                                  solidInit->GetFaceIndex(*secondFace)) );
            TestVariables::drawImp->DrawClearMap();
            for ( size_t i = 0, icnt = selectedFaces.Count(); i < icnt; i++ )
              TestVariables::drawImp->DrawItem( selectedFaces[i], 0, 65, 255 );
          }
        }
        else if ( objCur == obj ) {
           medianFacesInd.push_back( IndicesPair(solidInit->GetFaceIndex(*firstFace), SYS_MAX_T) );
        }
      }
      else {
        selectedFaces.DetachInd( ind1 );
        TestVariables::drawImp->DrawItem( firstFace, TRGB_LIGHTGREEN );
      }
    }

    double dmin = 0.0, dmax = 0.0;
    if ( 0 == selectedFaces.Count() ) { // Грани не выбраны, запрос толщины для автоматического поиска.
      GetDouble( IDS_ENTER_LOWERLIMIT, dmin );
      GetDouble( IDS_ENTER_UPPERLIMIT, dmax );
    }

    MbSNameMaker snameMaker( ( SimpleName )ct_MedianShell, MbSNameMaker::i_SideNone, 0 );

    MedianShellValues param;
    param.dmin = dmin;
    param.dmax = dmax;
    param.filletType = MedianShellValues::FilletType::tf_average;
    MbMedianShellParams params( medianFacesInd, param, snameMaker );
    MbMedianShellResults results;
    MbResultType res = ::MedianShell( *solidInit, cm_Copy, params, results );

    if ( results._solid == nullptr )
      ::PutErrorMessage( res, msgTitleError );
    else {
      Style objStyle( obj->GetWidth(), obj->GetColor() );
      TestVariables::viewManager->AddObject( objStyle, results._solid );
    }
    if ( obj == nullptr ) {
      TCHAR msgNoSelectedSolid[] = RT_NOT_SELECTED_BODY;
      ::ShowString( msgNoSelectedSolid );
    }

    SetWaitCursor( false );
  }
}


//------------------------------------------------------------------------------
// Создать решетчатую оболочку.
// Create a lattice shell.
// ---
void CreateLattice() 
{
  MbCartPoint3D p[3];

  if ( GetPoint( IDS_SELECT_POINT, p[0] ) ) {
    TestVariables::drawImp->DrawPoint( &p[0], TRGB_LIGHTGREEN );
    if ( GetPoint( IDS_SELECT_POINT, p[1] ) ) {
      TestVariables::drawImp->DrawPoint( &p[1], TRGB_LIGHTGREEN );
      if ( GetPoint( IDS_SELECT_POINT, p[2] ) ) {
        TestVariables::drawImp->DrawPoint( &p[2], TRGB_LIGHTGREEN );
        MbSNameMaker names( ct_ElementarySolid, MbSNameMaker::i_SideNone, 0 );
        MbSolid * solid = nullptr;

        double xRadius, yRadius, zRadius;

        xRadius = p[0].DistanceToPoint( p[1] );
        if ( GetDouble( IDS_ENTER_FIRST_SRF_DST, xRadius ) ) {
           yRadius = xRadius;
          if ( GetDouble( IDS_ENTER_SECOND_SRF_DST, yRadius ) ) {
            zRadius = yRadius;
            if ( GetDouble( IDS_ENTER_DISTANCE, zRadius ) ) {
              ptrdiff_t xCount(5), yCount, zCount; 
              if ( GetInt( IDS_ENTER_U_POINT_NUMBER, xCount ) ) {
                yCount = xCount;
                if ( GetInt( IDS_ENTER_V_POINT_NUMBER, yCount ) ) {
                  zCount = yCount;
                  if ( GetInt( IDS_ENTER_QUANTITY, zCount ) ) {
                    SolidSPtr solSPtr;
                    ::OctaLattice( MbOctaLatticeParams(p[0], p[1], p[2], xRadius, yRadius, zRadius, xCount, yCount, zCount, names), solSPtr );
                    solid = ::DetachItem( solSPtr );
                    if ( solid != nullptr )
                      TestVariables::viewManager->AddObject( TestVariables::ELEMENTARY_Style, solid );
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
// Разделить оболочку на части по заданому набору ребер.
// Divide shell into parts using a group of edges.
// ---
void CreateDividedShell()
{
  MbItem * obj = nullptr;
  MbSolid * solid = nullptr;

  if ( GetGObj(IDS_SELECT_SOLID, st_Solid, &obj, TestVariables::itemPathFrom) ) {
    solid = static_cast<MbSolid *>(obj);

    RPArray<MbCurveEdge> edges( 0, 1 );
    MbTopItem * item = nullptr;

    while ( GetTObj(IDS_SELECT_EDGE, st_Solid, tt_CurveEdge, &obj, &item) && item->IsAnEdge() ) {
      MbCurveEdge * edge = static_cast<MbCurveEdge *>(item);
      size_t ind = edges.FindIt( edge );

      if ( ind != SYS_MAX_T ) {
        TestVariables::drawImp->DrawItem( item, TRGB_GREEN );
        edges.DetachInd( ind );
      }
      else {
        TestVariables::drawImp->DrawItem( item, TRGB_AZURE );
        edges.Add( edge );
      }
      for ( size_t m = 0, mcnt = edges.Count(); m < mcnt; m++ )
        TestVariables::drawImp->DrawItem( edges[m], TRGB_AZURE );
    }
  
    if ( edges.Count() > 0 ) {
      MbSNameMaker operNames( ct_DividedShell, MbSNameMaker::i_SideNone, 0 );
      c3d::SolidSPtr resSolid;

      size_t partsCnt0 = 1;

      if ( solid->IsShellBuild() ) {
        ShellSPtr shell( new MbFaceShell( *solid->GetShell() ) );
        c3d::ShellsSPtrVector parts;
        ::DetachShells( *shell, parts, false );
        partsCnt0 += parts.size();
      }

      ::DivideShell( *solid, MbeCopyMode::cm_Copy, MbDivideShellParams(edges, operNames), resSolid );

      if ( resSolid != nullptr ) {
        size_t partsCnt = 1;

        if ( resSolid->IsShellBuild() ) {
          ShellSPtr shell( new MbFaceShell( *resSolid->GetShell() ) );
          c3d::ShellsSPtrVector parts;
          ::DetachShells( *shell, parts, false );
          partsCnt += parts.size();

          if ( partsCnt <= partsCnt0 ) {
            ::MessageBoxEx( RT_DONE_IN_PART, IDS_TITLE_WRN );
          }
        }

        Style ns = TestVariables::SHELL_Style;
        TestVariables::viewManager->DeleteObject( solid );
        TestVariables::viewManager->AddObject( ns, resSolid );
      }
      else {
        ::MessageBoxEx( RT_PARAMETER_ERROR, IDS_TITLE_ERR );
      }
    }
  }
}


//------------------------------------------------------------------------------
// Дать кривую на поверхности.
// Get curve on surface.
// ---
static
bool FindSide( MbCurveEdge & edge, const MbVector3D & axisZ )
{
  MbFace * faceP = edge.GetFacePlus();
  MbFace * faceM = edge.GetFaceMinus();

  if ( faceP != nullptr && faceM == nullptr )
    return true;
  if ( faceM != nullptr && faceP == nullptr )
    return false;

  const MbSurfaceIntersectionCurve & intCurve = edge.GetIntersectionCurve();
  MbVector3D vec1, vec2;
  double t = intCurve.GetTMid();

  if ( faceP != nullptr && faceM != nullptr ) {
    if ( edge.FaceNormal( t, vec1, true ) &&
         edge.FaceNormal( t, vec2, false ) ) {
      int fn = ( ::fabs(vec1 * axisZ) > ::fabs(vec2 * axisZ) - EPSILON ) ? 1 : 0;
      GetInt( IDS_FACE_PLUS_MINUS, fn );
      return ( fn > 0 ) ? true : false;
    }
  }

  const MbSurfaceCurve & curve1 = intCurve.GetCurveOne();
  const MbSurfaceCurve & curve2 = intCurve.GetCurveTwo();

  curve1.SurfaceNormal( t, vec1 );
  curve2.SurfaceNormal( t, vec2 );
  if ( ::fabs(vec1 * axisZ) > ::fabs(vec2 * axisZ) - EPSILON )
    return true;
  else
    return false;
}


//------------------------------------------------------------------------------
// Наполнить контейнеры опорными объектами. Fill the rail with reference objects.
// ---
static
void FillSectionRail( MbSectionRail & rail, int form )
{
  MbItem * obj = nullptr;
  MbTopItem * topoItem = nullptr;
  bool res = false;
  MbVector3D axisZ( TestVariables::cursorAxis.GetAxisZ() );

  obj = nullptr;
  if ( form != 0 ) { // Линейчатая поверхность.
    while ( GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj, &topoItem ) ) { // Вводим грани. Add faces.
      TestVariables::viewManager->ToggleObject(obj);
      MbFace * face = (MbFace *)topoItem;
      if ( face != nullptr ) {
        int side = 1;
        ::GetInt( IDS_ENTER_SURFACE_NORMAL, side );
        MbSolid * solid( nullptr );
        if ( obj != nullptr && (obj->IsA() == st_Solid) )
          solid = static_cast<MbSolid *>( obj );
        rail.AddFace( *face, side == 1, solid );
        res = true;
        TestVariables::viewManager->ToggleObject(obj);
      }
    }
  }

  if ( !res || (form == 1) ) {
    if ( res && rail.GetFacesCount() > 0 )
      rail.SolidsClear();  // Очистить контейр тел. Solid conteiner clear.
    while ( GetTObj( IDS_SELECT_GUIDE_EDGE, st_Solid, tt_CurveEdge, &obj, &topoItem ) ) { // Вводим рёбра. Add edges.
      TestVariables::viewManager->ToggleObject(obj);
      MbCurveEdge * edge = (MbCurveEdge *)topoItem;
      if ( edge != nullptr ) {
        bool side = ::FindSide( *edge, axisZ );
        MbSolid * solid( nullptr );
        if ( obj != nullptr && (obj->IsA() == st_Solid) )
          solid = static_cast<MbSolid *>( obj );
        rail.AddEdge( *edge, side, solid );
        res = true;
        TestVariables::viewManager->ToggleObject(obj);
      }
    }
  }

  if ( !res || (form == 1 && rail.GetEdgesCount() == 0) ) { // KOMPAS-48068
    while ( GetGObj( IDS_SELECT_GUIDE_CURVE, st_Curve3D, &obj ) ) { // Если рёбра не введены, то вводим кривые. Add curves.
      if ( obj->IsA() == st_SpaceInstance ) {
          const MbSpaceInstance * sInstance = static_cast<const MbSpaceInstance *>( obj );
        if ( sInstance != nullptr && sInstance->GetSpaceItem()->Family() == st_Curve3D ) {
          MbCurve3D * curve = (MbCurve3D *)sInstance->GetSpaceItem();
          if ( curve != nullptr ) {
//#ifdef _DRAWGI
            TestVariables::drawImp->DrawItem( curve, TRGB_GOLD );
//#endif
            rail.AddCurve( *curve );
//#ifdef _DRAWGI
            TestVariables::drawImp->DrawItem( curve, TRGB_AVIATION );
//#endif
            res = true;
          }
        }
      }
    }
  }

  if ( res || (form == 1 && rail.GetFacesCount() == 0) ) { // Вводим функцию. Add angle function.
    MbFunction * function( nullptr );
    size_t eCount = rail.GetEdgesCount();
    size_t cCount = rail.GetCurvesCount();
    size_t fCount = rail.GetFacesCount();
    double d1 = 0.2;  // Угол в начале.
    double d2 = 0.15; // Угол на конце.
    if ( cCount > 0 ) {
      d1 = 0.7; // Угол в начале.
      d2 = 1.0; // Угол на конце.
    }
    if ( fCount > 0 ) {
      if ( form != 1 ) {
        d1 = 10.0; // Радиус в начале.
        d2 = 15.0; // Радиус на конце.
      }
      if ( form == 1 ) {
        d1 = 25; // Длина в начале.
        d2 = 35; // Длина на конце.
      }
    }
    double d0 = d1; // Запоминаем. Memorize.
    bool b = GetDouble( IDS_ENTER_ANGLE, d1 );
    if ( b ) {
      if ( ::fabs(d0 - d1) > EPSILON )
        d2 = d1; // Было изменено, делаем одинаковыми. It was changed, we make it the same.
      b = GetDouble( IDS_ENTER_ANGLE, d2 );
    }
    if ( b ) {
      int ft = 0;
      if ( ::fabs(d1 - d2) > EPSILON ) {
        ft = 2;
        GetInt( IDS_FUNCTION_TYPE, ft );
      }
      if ( ft == 0 )
        function = new MbConstFunction( d1 ); 
      else {
        if ( ft == 1 )
          function = new MbLineFunction( d1, d2, 0.0, 1.0 ); 
        else 
          function = new MbCubicFunction( d1, 0.0, 0.0, d2, 0.0, 1.0 ); 
      }
    }
    else {
      int nc = 1;
      if ( GetGObj( IDS_SELECT_CURVE, st_Curve3D, &obj ) ) {
        const MbSpaceItem * sItem = static_cast<MbSpaceInstance *>( obj )->GetSpaceItem();
        if ( sItem != nullptr && sItem->Family() == st_Curve3D ) {
          const MbCurve3D * fCurve = static_cast<const MbCurve3D *>( sItem );
          if ( GetInt( IDS_ENTER_COORD_NUMBER, nc ) )
            function = ::CreateFunction( *fCurve, MbPlacement3D::global, nc );
          else
            rail.SetTrack( *((MbCurve3D *)fCurve) );
        }
      }
    }
    if ( function != nullptr ) { 
      int ns = ( rail.GetEdgesCount() > 0 ) ? 1 : 0;
      if ( (eCount > 0) || (cCount > 0) ) 
        ::GetInt( IDS_ENTER_STATE_NUMBER, ns ); // Как откладывать угол. How to use an angle function.
      ThreeStates ts = ts_neutral;
      if ( ns > 0 )
        ts = ts_positive;
      else
      if ( ns < 0 )
        ts = ts_negative;
      else
        ts = ts_neutral;
      rail.SetAngle( *function, ts );
    }
  }
}


//------------------------------------------------------------------------------
// Построить оболочку переменного сечения. Mutable section shell construction.
// ---
void CreateSectionShell()
{
  MbItem * objCurve = nullptr;
  PathMatrix pathFrom0, pathFrom;

  if ( GetGObj( IDS_SELECT_REFER_CURVE, st_Curve3D, &objCurve ) ) {
    MbCurve3D * reference( nullptr );
    if ( objCurve!= nullptr && objCurve->IsA() == st_SpaceInstance ) {
       MbSpaceItem * sItem = static_cast<MbSpaceInstance *>( objCurve )->SetSpaceItem();
      if ( sItem != nullptr && sItem->Family() == st_Curve3D )
        reference = static_cast<MbCurve3D *>( sItem );
    }

    if ( reference != nullptr) {
      ::GetInt( IDS_SPECIFY_SURFACE_SECTION, TestVariables::form );

      MbSectionRail rail1, rail2; // A guide rails (edges, or curves with function, or surfaces).
      MbSectionRule descript;

      ::FillSectionRail( rail1, TestVariables::form );
      if ( TestVariables::form != 0 )
        ::FillSectionRail( rail2, TestVariables::form );

      bool br1 = ( rail1.GetEdgesCount() > 0 || rail1.GetFacesCount() > 0 || rail1.GetCurvesCount() > 0 );
      bool br2 = ( rail2.GetEdgesCount() > 0 || rail2.GetFacesCount() > 0 || rail2.GetCurvesCount() > 0 );

      if ( ( br1 && br2  && (TestVariables::form >  1)) || 
           ((br1 || br2) && (TestVariables::form <= 1)) ) {

        double uBeg = 0.0; 
        double uEnd = ( TestVariables::form == 0 ) ? M_PI2 : 1.0; 
        if ( TestVariables::form != 4 )
          GetDouble( IDS_ENTER_FINAL_PRM, uEnd );
        double d1 = 0.4142, d2 = 0.4142;
        if ( TestVariables::form == 0 ) {
          double len = reference->GetMetricLength();
          d1 = len * DELTA_MOD;
          d2 = d1;
        }
        MbCurve3D * apex( nullptr );
        MbPolyCurve * pattern( nullptr );
        if ( TestVariables::form == 4 ) {
          MbItem * pObj  = nullptr;
          if ( GetMObj( IDS_SELECT_2D_CURVE, pt_Curve, &pObj ) ) {
            const MbPlaneItem * pItem = static_cast<MbPlaneInstance *>( pObj )->GetPlaneItem( 0 );
            if ( pItem != nullptr && pItem->Type() == pt_PolyCurve ) {
              pattern = (MbPolyCurve *)&( pItem->Duplicate() );
              uBeg = pattern->GetTMin(); 
              uEnd = pattern->GetTMax();
            }
          }
        }
        else {
          if ( TestVariables::form != 1 ) {
            bool b = false;
            // Дискриминант: функция, или кривая, или поверхность.  A discriminant, or curve, or surface.
            double d0 = d1; // Запоминаем. Memorize.
            if ( TestVariables::form == 0 ) {
              b = GetDouble( IDS_ENTER_RADIUS, d1 ); 
              if ( b && ( ::fabs(d0 - d1) > EPSILON ) )
                d2 = d1; // Было изменено, делаем одинаковыми. It was changed, we make it the same.
              b = b && GetDouble( IDS_ENTER_RADIUS, d2 );
            }
            else {
              b = GetDouble( IDS_ENTER_DISCR, d1 );
              if ( b && ( ::fabs(d0 - d1) > EPSILON ) )
                d2 = d1; // Было изменено, делаем одинаковыми. It was changed, we make it the same.
              b = b && GetDouble( IDS_ENTER_DISCR, d2 );
            }
            if ( b ) { // Функция дискриминанта. A discriminant function.
              double st1 = reference->GetTMin(), st2 = reference->GetTMax();
              if ( d1 > EPSILON && d2 > EPSILON ) {
                MbFunction * discr( nullptr );
                int ft = 0;
                if ( ::fabs(d1 - d2) > EPSILON ) {
                  ft = 2;
                  GetInt( IDS_FUNCTION_TYPE, ft );
                }
                if ( ft == 0 )
                  discr = new MbConstFunction( d1 ); 
                else {
                  if ( ft == 1 )
                    discr = new MbLineFunction( d1, d2, st1, st2 );
                  else 
                    discr = new MbCubicFunction( d1, 0.0, st1, d2, 0.0, st2 ); 
                }
              }
            }
            else {
              int nc = 1;
              if ( GetGObj( IDS_DISCRIMINANT_CURVE, st_Curve3D, &objCurve ) ) {
                const MbSpaceItem * sItem = static_cast<MbSpaceInstance *>( objCurve )->GetSpaceItem();
                if ( sItem != nullptr && sItem->Family() == st_Curve3D ) {
                  const MbCurve3D * fCurve = static_cast<const MbCurve3D *>( sItem );
                  if ( GetInt( IDS_ENTER_COORD_NUMBER, nc ) ) { // Функция дискриминанта. A discriminant function.
                    MbFunction * discr = ::CreateFunction( *fCurve, MbPlacement3D::global, nc );
                    if ( discr != nullptr )
                      descript.SetFunction( *discr );
                  }
                  else {
                    if ( fCurve != nullptr ) { // Кривая дискриминанта. A curve that must pass through.
                      MbCurve3D * cCurve = const_cast<MbCurve3D *>( fCurve );
                      descript.SetCurve( *cCurve );
                    }
                  }
                }
              }
              else {
                MbItem * objSurf = nullptr; // Поверхность дискриминанта. A surface that must touch.
                if ( GetGObj( IDS_DISCRIMINANT_SURFACE, st_Surface, &objSurf ) ) {
                  MbSpaceItem * sItem = static_cast<MbSpaceInstance *>( objSurf )->SetSpaceItem();
                  if ( sItem != nullptr && sItem->Family() == st_Surface ) {
                    MbSurface * fSurface = static_cast<MbSurface *>( sItem );
                    if ( fSurface != nullptr )
                      descript.SetSurface( *fSurface );
                  }
                }
                else {
                  MbTopItem * topoItem = nullptr;
                  if ( GetTObj( IDS_DISCRIMINANT_FACE, st_Solid, tt_Face, &objSurf, &topoItem ) ) {
                    MbFace * face = (MbFace *)topoItem;
                    if ( face != nullptr ) {
                      MbSurface & fSurface = face->SetSurface();
                      descript.SetSurface( fSurface );
                    }
                  }
                }
              }
            }
          }
        }

        if ( (TestVariables::form > 1) && // Кривая вершин. An apex curve
             ((rail1.GetEdgesCount() == 0 && rail1.GetFacesCount() == 0 && rail1.GetCurvesCount() > 0 && rail1.GetAngle() == nullptr) ||
              (rail2.GetEdgesCount() == 0 && rail2.GetFacesCount() == 0 && rail2.GetCurvesCount() > 0 && rail2.GetAngle() == nullptr)) ) {
          if ( GetGObj(IDS_SELECT_APEX_CURVE, st_Curve3D, &objCurve) ) {
            MbSpaceItem* sItem = static_cast<MbSpaceInstance*>(objCurve)->SetSpaceItem();
            if (sItem != nullptr && sItem->Family() == st_Curve3D) {
              apex = static_cast<MbCurve3D*>(sItem);
            }
          }
        }

        MbeCopyMode sameShell = cm_KeepHistory;//cm_KeepSurface;
        MbSNameMaker names( ct_SectionShell, MbSNameMaker::i_SideNone, 0 );
        //names.SetMathVersion( MATH_21_VERSION ); // KOMPAS-58277
        MbSectionData data( *reference, (MbeSectionShape)(TestVariables::form), 
                            rail1, rail2, apex, descript, pattern, names );
        data.SetDirection( MbVector3D(0.0,0.0,1.0) );
        data.SetUParams( uBeg, uEnd );

        int nb = 1;
        GetInt( IDS_ENTER_SIDE_SHAPE, nb ); // Установить форму обрезки боков поверхности. Set the shape of cropping the sides of the surface. \~
        data.SetSideShape( (MbeSideShape)(nb) );
        nb = 0;
        GetInt( IDS_ENTER_FACE_HANDLING, nb ); // Установить обработку исходных опорных граней. Set the processing of initial reference faces. \~
        data.SetFaceHandling( (MbeFaceHandling)(nb) );
        nb = 0;
        GetInt( IDS_ENTER_FACE_DIVIDE, nb ); // Установить деление оболочки на грани по сегментам направляющих кривых. Set division the shell into faces by segments of guides.
        data.SetFaceSplit( (nb > 0) ? true : false );

        data.SetCheck( false );

        int pf = DrawTestShell::checkPhantom ? 1 : 0;
        GetInt( IDS_BUILD_PFANTOM, pf );
        if ( pf > 0 )
          DrawTestShell::checkPhantom = true;
        if ( DrawTestShell::checkPhantom ) { // Вычисление данных фантома для торцев поверхности переменного сечения.
          MbPlacement3D begPlace, endPlace;
          MbCartPoint3D begGuideP1, begGuideP2, begApex, endGuideP1, endGuideP2, endApex;
          MbVector3D begVector1, begVector2, endVector1, endVector2;

          MbSectionPhantom sectionPhantom;
          ::SectionPhantom( data, sectionPhantom );
          sectionPhantom.GetLayout( 1, begPlace, begGuideP1, begGuideP2, begVector1, begVector2, begApex );
          sectionPhantom.GetLayout( 2, endPlace, endGuideP1, endGuideP2, endVector1, endVector2, endApex );

          //::SectionPhantom( data, begPlace, begGuideP1, begGuideP2, begVector1, begVector2, begApex, 
          //                  endPlace, endGuideP1, endGuideP2, endVector1, endVector2, endApex );

//#ifdef _DRAWGI
          TestVariables::drawImp->DrawPoint( &begGuideP1, TRGB_OLIVE );
          TestVariables::drawImp->DrawPoint( &begGuideP2, TRGB_AZURE );
          TestVariables::drawImp->DrawLine( begGuideP1, begGuideP1+begVector1, TRGB_OLIVE );
          TestVariables::drawImp->DrawLine( begGuideP2, begGuideP2+begVector2, TRGB_AZURE );
          TestVariables::drawImp->DrawPoint( &begApex, TRGB_CERISE );
          TestVariables::drawImp->DrawPoint( &endGuideP1, TRGB_OLIVE );
          TestVariables::drawImp->DrawPoint( &endGuideP2, TRGB_LIGHTCYAN );
          TestVariables::drawImp->DrawLine( endGuideP1, endGuideP1+endVector1, TRGB_OLIVE );
          TestVariables::drawImp->DrawLine( endGuideP2, endGuideP2+endVector2, TRGB_LIGHTCYAN );
          TestVariables::drawImp->DrawPoint( &endApex, TRGB_CERISE );
//#endif // _DRAWGI
        }
        c3d::SolidSPtr srcSolid;
        std::vector<MbSolid *> solids;
        data.GetUniqueSolids ( solids );

        if ( solids.size() == 1 ) {
          int ns = 1;
          GetInt( IDS_ENTER_MERGING, ns );
          if ( ns != 0 )
            srcSolid = solids[0];
        }

        SetWaitCursor( true );

        c3d::SolidSPtr result;
        //MbSolid * result( nullptr );

        MbResultType res = ::SectionShell( srcSolid, sameShell, data, result );
        //MbResultType res = ::SectionShell( solid, sameShell, data, names, result );

        if ( res != rt_Success )
          ::PutErrorMessage( res, IDS_TITLE_ERR );
        if ( result != nullptr ) {
          if ( srcSolid != nullptr )
            TestVariables::viewManager->DeleteObject( srcSolid );
          else {
            if ( data.GetFaceHandling() == face_Cut ) {
              for ( size_t i = solids.size(); i--; ) 
                TestVariables::viewManager->RedrawObject( solids[i] );
            }
            else
            if ( data.GetFaceHandling() == face_Sew ) {
              for ( size_t i = solids.size(); i--; ) 
                TestVariables::viewManager->DeleteObject( solids[i] );
            }
          }
          TestVariables::viewManager->AddObject( TestVariables::SHELL_Style, result );
        }
        ::DeleteItem( pattern );
        SetWaitCursor( false );
      }
    }
  }
}

