//////////////////////////////////////////////////////////////////////////////////////////
//
//    The file contains source codes demonstrating samples how to create closed corners.
//
//    WARNING: This code is only an example. The real application can require more 
//             functionality and data types for implementation.
// 
//////////////////////////////////////////////////////////////////////////////////////////

#include <test_samples.h>
#include <test_variables.h>
#include <test_manager.h>
#include <test_service.h>
#include <templ_sptr.h>
#include <cur_line_segment.h>
#include <cur_arc.h>
#include <action_sheet.h>
#include <alg_draw.h>
#include <math_namespace.h>
#include <last.h>

using namespace c3d;


//------------------------------------------------------------------------------
// \ru Замыкание угла. \en A closed corner creation.
// ---
void SampleCloseCorner() {
  MbResultType res = rt_Error;
  MbSolid * resultSolid = nullptr;

  RPArray<MbSMBendNames> resultBends( 0, 1 );

  SetWaitCursor( true );

  { // \ru Создаём квадратную пластину. \en Create a square plate.
    MbPlacement3D placement;
    SPtr<MbContour> contour( new MbContour() );
    contour->AddSegment( new MbLineSegment(-50.0/*u1*/, -50.0/*v1*/, 50.0/*u2*/, -50.0/*v2*/) );
    contour->AddSegment( new MbLineSegment(50.0/*u1*/, -50.0/*v1*/, 50.0/*u2*/, 50.0/*v2*/) );
    contour->AddSegment( new MbLineSegment(50.0/*u1*/, 50.0/*v1*/, -50.0/*u2*/, 50.0/*v2*/) );
    contour->AddSegment( new MbLineSegment(-50.0/*u1*/, 50.0/*v1*/, -50.0/*u2*/, -50.0/*v2*/) );
    RPArray<MbContour> contours( 1, 0 );
    contours.Add( contour );

    MbSheetMetalValues parameters;
    parameters.side1.scalarValue = 1.0; // thickness
    parameters.k = 0.5; // K-factor value

    RPArray<MbSNameMaker> nameMakers( 0, 1 );
    nameMakers.Add( new MbSNameMaker(0/*mainName*/, MbSNameMaker::i_SideNone/*sideAdd*/, 0/*buttAdd*/) );

    RPArray<MbSMBendNames> operation0Bends( 0, 1 ); // no bends are going to be created
    res = CreateSheetSolid( placement, contours, false/*unbended*/, parameters, &nameMakers, operation0Bends, resultSolid );
    delete nameMakers[0];
  }

  if ( resultSolid != nullptr ) { // \ru Создаём сгибы по периметру. \en Create bends around the perimeter.
    RPArray<MbCurveEdge> edgesForFlange( 4, 1 );
    MbFace * sheetFace = nullptr;
    for ( size_t faceIndex = resultSolid->GetFacesCount(); faceIndex--; ) {
      MbFace * face = resultSolid->GetFace( faceIndex );
      if ( (face != nullptr) && face->GetName().IsSheet() ) {
        sheetFace = face;
        break;
      }
    }

    if ( sheetFace != nullptr ) {
      const MbLoop * outerLoop = sheetFace->GetLoop( 0 );
      if ( outerLoop != nullptr ) {
        for ( size_t edgeIndex = outerLoop->GetEdgesCount(); edgeIndex--; ) {
          MbOrientedEdge * orEdge = outerLoop->_GetOrientedEdge( edgeIndex );
          if ( orEdge != nullptr )
            edgesForFlange.Add( &orEdge->GetCurveEdge() );
        }
      }
    }

    if ( edgesForFlange.Count() == 4 ) {
      MbSNameMaker nameMaker( 1/*mainName*/, MbSNameMaker::i_SideNone/*sideAdd*/, 0/*buttAdd*/ );
      MbBendByEdgeValues params( 0.5/*k*/, 1.0/*radius*/, M_PI_2/*bendAngle*/, 0.0/*coneAngle*/, 30.0/*length*/, 0.0/*deepness*/, rt_No );
      params.SideLeftInit( 0.0/*distance*/, 0.0 * Math::DEGRAD/*angle*/, 0.0 * Math::DEGRAD/*deviation*/, 0.0/*widening*/ );
      params.SideRightInit( 0.0/*distance*/, 0.0 * Math::DEGRAD/*angle*/, 0.0 * Math::DEGRAD/*deviation*/, 0.0/*widening*/ );
      params.SlotInit( 0.0/*width*/, 0.0/*depth*/, 0.0/*radius*/ );
      params.miterMiddle.allow = false; // don't close corner
      //params.miterMiddle.cornerWay = MbClosedCornerValues::ccTight;
      //params.miterMiddle.gap = 0.5;
      //params.miterMiddle.bendsWay.push_back( MbClosedCornerValues::cbEdge );
      RPArray<MbSMBendNames> operation1Bends( 4, 1 ); // individual parameters of four bends to be created
      operation1Bends.Add( new MbSMBendNames(0.5/*k*/, 1.0/*radius*/, M_PI_2/*angle*/, SimpleName(SIMPLENAME_MAX)/*SimpleName1*/, SimpleName(SIMPLENAME_MAX)/*SimpleName2*/) );
      operation1Bends.Add( new MbSMBendNames(0.5/*k*/, 1.0/*radius*/, M_PI_2/*angle*/, SimpleName(SIMPLENAME_MAX)/*SimpleName1*/, SimpleName(SIMPLENAME_MAX)/*SimpleName2*/) );
      operation1Bends.Add( new MbSMBendNames(0.5/*k*/, 1.0/*radius*/, M_PI_2/*angle*/, SimpleName(SIMPLENAME_MAX)/*SimpleName1*/, SimpleName(SIMPLENAME_MAX)/*SimpleName2*/) );
      operation1Bends.Add( new MbSMBendNames(0.5/*k*/, 1.0/*radius*/, M_PI_2/*angle*/, SimpleName(SIMPLENAME_MAX)/*SimpleName1*/, SimpleName(SIMPLENAME_MAX)/*SimpleName2*/) );
      MbSolid * newSolid = nullptr;
      res = BendSheetSolidByEdges( *resultSolid, cm_Copy, edgesForFlange, false/*unbended*/, params, nameMaker, operation1Bends, newSolid );
      if ( newSolid != nullptr ) {
        ::DeleteItem( resultSolid );
        resultSolid = newSolid;

        resultBends.AddArray( operation1Bends ); // save created bends data in the common array
      }
      else {
        for ( size_t bendIndex = operation1Bends.Count(); bendIndex--; ) {
          delete operation1Bends[bendIndex];
        }
      }
    }
  }

  // \ru Создаём два сгиба на продолжении предыдущих. \en Create two bends on the extension of the previous ones.
  if ( (resultSolid != nullptr) && (res == rt_Success) && (resultBends.Count() > 1) ) {
    MbSMBendNames * bend0 = resultBends[0];
    MbSMBendNames * bend1 = resultBends[1];
    if ( (bend0 != nullptr) && (bend1 != nullptr) ) {
      const MbFace * innerBendFace0 = resultSolid->FindFaceByName( bend0->innerFaceName );
      const MbFace * innerBendFace1 = resultSolid->FindFaceByName( bend1->innerFaceName );
      if ( (innerBendFace0 != nullptr) && (innerBendFace1 != nullptr) ) {
        RPArray<MbCurveEdge> edgesForFlange( 2, 1 );
        {
          const MbLoop * loop = innerBendFace0->GetLoop( 0 );
          if ( (loop != nullptr) && (loop->GetEdgesCount() == 4) ) {
            for ( size_t edgeIndex = 4; edgeIndex--; ) {
              const MbOrientedEdge * orEdge = loop->_GetOrientedEdge( edgeIndex );
              if ( orEdge != nullptr ) {
                const MbFace * neighborFace = orEdge->GetFaceMinus();
                if ( (neighborFace != nullptr) && (neighborFace->GetName().GetMainName() == 0) ) {
                  const MbOrientedEdge * oppositeOrEdge = loop->_GetOrientedEdge( (edgeIndex + 2) % 4 );
                  if ( oppositeOrEdge != nullptr ) {
                    const MbFace * extensionFace = oppositeOrEdge->GetFaceMinus();
                    if ( extensionFace != nullptr ) {
                      const MbLoop * extensionLoop = extensionFace->GetLoop( 0 );
                      if ( (extensionLoop != nullptr) && (extensionLoop->GetEdgesCount() == 4) ) {
                        for ( size_t extEdgeIndex = 4; extEdgeIndex--; ) {
                          const MbOrientedEdge * extOrEdge = extensionLoop->_GetOrientedEdge( extEdgeIndex );
                          if ( (extOrEdge != nullptr) && (&extOrEdge->GetCurveEdge() == &oppositeOrEdge->GetCurveEdge()) ) {
                            extOrEdge = extensionLoop->GetOrientedEdge( (extEdgeIndex + 2) % 4 );
                            edgesForFlange.Add( &extOrEdge->GetCurveEdge() );
                            edgeIndex = extEdgeIndex = 0;
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

        {
          const MbLoop * loop = innerBendFace1->GetLoop( 0 );
          if ( (loop != nullptr) && (loop->GetEdgesCount() == 4) ) {
            for ( size_t edgeIndex = 4; edgeIndex--; ) {
              const MbOrientedEdge * orEdge = loop->_GetOrientedEdge( edgeIndex );
              if ( orEdge != nullptr ) {
                const MbFace * neighborFace = orEdge->GetFaceMinus();
                if ( (neighborFace != nullptr) && (neighborFace->GetName().GetMainName() == 0) ) {
                  MbOrientedEdge * oppositeOrEdge = loop->_GetOrientedEdge( (edgeIndex + 2) % 4 );
                  if ( oppositeOrEdge != nullptr ) {
                    const MbFace * extensionFace = oppositeOrEdge->GetFaceMinus();
                    if ( extensionFace != nullptr ) {
                      const MbLoop * extensionLoop = extensionFace->GetLoop( 0 );
                      if ( (extensionLoop != nullptr) && (extensionLoop->GetEdgesCount() == 4) ) {
                        for ( size_t extEdgeIndex = 4; extEdgeIndex--; ) {
                          const MbOrientedEdge * extOrEdge = extensionLoop->_GetOrientedEdge( extEdgeIndex );
                          if ( (extOrEdge != nullptr) && (&extOrEdge->GetCurveEdge() == &oppositeOrEdge->GetCurveEdge()) ) {
                            extOrEdge = extensionLoop->GetOrientedEdge( (extEdgeIndex + 2) % 4 );
                            edgesForFlange.Add( &extOrEdge->GetCurveEdge() );
                            edgeIndex = extEdgeIndex = 0;
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

        if ( edgesForFlange.Count() == 2 ) {
          MbSNameMaker nameMaker( 2/*mainName*/, MbSNameMaker::i_SideNone/*sideAdd*/, 0/*buttAdd*/ );
          MbBendByEdgeValues params( 0.5/*k*/, 1.0/*radius*/, M_PI_2/*bendAngle*/, 0.0/*coneAngle*/, 30.0/*length*/, 0.0/*deepness*/, rt_No );
          params.SideLeftInit( 0.0, 0.0 * Math::DEGRAD, 0.0 * Math::DEGRAD, 0.0 );
          params.SideRightInit( 0.0, 0.0 * Math::DEGRAD, 0.0 * Math::DEGRAD, 0.0 );
          params.SlotInit( 0.0/*width*/, 0.0/*depth*/, 0.0/*radius*/ );
          params.miterMiddle.allow = false;
          //params.miterMiddle.cornerWay = MbClosedCornerValues::ccTight;
          //params.miterMiddle.gap = 0.5;
          //params.miterMiddle.bendsWay.push_back( MbClosedCornerValues::cbEdge );
          RPArray<MbSMBendNames> operation2Bends( 2, 1 ); // individual parameters for two more bends
          operation2Bends.Add( new MbSMBendNames(0.5/*k*/, 1.0/*radius*/, M_PI_2/*angle*/, SimpleName(SIMPLENAME_MAX)/*SimpleName1*/, SimpleName(SIMPLENAME_MAX)/*SimpleName2*/) );
          operation2Bends.Add( new MbSMBendNames(0.5/*k*/, 1.0/*radius*/, M_PI_2/*angle*/, SimpleName(SIMPLENAME_MAX)/*SimpleName1*/, SimpleName(SIMPLENAME_MAX)/*SimpleName2*/) );
          MbSolid * newSolid = nullptr;
          res = BendSheetSolidByEdges( *resultSolid, cm_Copy, edgesForFlange, false/*unbended*/, params, nameMaker, operation2Bends, newSolid );
          if ( newSolid != nullptr ) {
            ::DeleteItem( resultSolid );
            resultSolid = newSolid;

            resultBends.AddArray( operation2Bends );
          }
          else {
            for ( size_t bendIndex = operation2Bends.Count(); bendIndex--; ) {
              delete operation2Bends[bendIndex];
            }
          }
        }
      }
    }
  }

  // \ru Создаём замыкание угла. \en Close corner.
  if ( (resultSolid != nullptr) && (res == rt_Success) && (resultBends.Count() > 1) ) {
    MbCloseCornerResults cornerResult;
    {
      MbSMBendNames * bendPlus = resultBends[resultBends.Count() - 1];
      MbSMBendNames * bendMinus = resultBends[resultBends.Count() - 2];
      if ( (bendPlus != nullptr) && (bendMinus != nullptr) ) {
        c3d::FaceSPtr farBendFacePlus( resultSolid->FindFaceByName( bendPlus->innerFaceName ) );
        c3d::FaceSPtr farBendFaceMinus( resultSolid->FindFaceByName( bendMinus->innerFaceName ) );
        if ( (farBendFacePlus != nullptr) && (farBendFaceMinus != nullptr) ) {
          c3d::FacesSPtrVector facesPlus, facesMinus;
          facesPlus.push_back( farBendFacePlus );
          facesMinus.push_back( farBendFaceMinus );
          MbCloseCornerParams cornerParams( facesPlus, facesMinus );
          // We may use GetParamsForCloseCorner variant with faces arrays like in this example
          // facesPlus array must contain either side face/faces that move when the corner is closing, or a sheet face neighbor to one of them
          // facesMinus array must contain either same faces as in facesPlus array (in this case the corner will be defined automatically)
          // or similar faces from the other side of the corner if automatic selection is not possible, like in this example,
          // because two corners is suitable for closing in this example.
          // 
          // There is also another variant of GetParamsForCloseCorner that takes two edges instead of faces.
          // The edges must either be the ones moving when closing the corner or the elongating ones.
          // If the plus edge is equal to the minus edge, the choice of the corner is automatic.
          // In order to clarify one of the two possible corners we must select minus edge from the other side of the corner.
          ::GetParamsForCloseCorner( cornerParams, cornerResult );
        }
      }
    }

    if ( ( cornerResult._edgePlus != nullptr) && ( cornerResult._edgeMinus != nullptr) ) {
      MbSNameMaker nameMaker( 3/*mainName*/, MbSNameMaker::i_SideNone/*sideAdd*/, 0/*buttAdd*/ );
      MbSolid * newSolid = nullptr;
      MbClosedCornerValues params( MbClosedCornerValues::ccTight,
                                   MbClosedCornerValues::cbEdge,
                                   MbClosedCornerValues::cpBend,
                                   1.0/*gap*/,
                                   0.0/*diameter*/,
                                   0.0/*shift*/,
                                   0.5/*kPlus*/,
                                   0.5/*kMinus*/,
                                   M_PI_4/*angle*/,
                                   true/*plus*/,
                                   true/*prolong*/,
                                   false/*acrossBend*/ );
      res = ::CloseCorner( *resultSolid, cm_Copy, cornerResult._edgePlus, cornerResult._edgeMinus,
                           params, nameMaker, newSolid );
      if ( newSolid != nullptr ) {
        ::DeleteItem( resultSolid );
        resultSolid = newSolid;
      }
    }
  }

  for ( size_t bendIndex = resultBends.Count(); bendIndex--; ) {
    delete resultBends[bendIndex];
  }

  SetWaitCursor( false );

  if ( resultSolid )
    TestVariables::viewManager->AddObject( TestVariables::SOLID_Style, resultSolid );
  else
    ::PutErrorMessage( res, IDS_TITLE_ERR );
}

