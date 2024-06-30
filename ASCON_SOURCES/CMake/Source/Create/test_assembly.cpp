﻿//////////////////////////////////////////////////////////////////////////////////////////
// Методы создания модельных объектов, команды подменю "Создать->Объект модели".
// Methods of creation of model items, sub-items of menu "Create->Model object".
//////////////////////////////////////////////////////////////////////////////////////////

#include <test.h>
#include <test_manager.h>
#include <test_service.h>
#include <test_computation.h>
#include <test_draw.h>

#include <space_instance.h>
#include <mb_class_traits.h>
#include <instance_item.h>
#include <assembly.h>
#include <mesh.h>
#include <action_general.h>
#include <action_solid.h>
#include <action_shell.h>
#include <action_mesh.h>
#include <tri_face.h>
#include <templ_dptr.h>
#include <tool_time_test.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <math_namespace.h>

#include <last.h>

using namespace c3d;
using namespace TestVariables;


//----------------------------------------------------------------------------------------
// Создать сборку.
// Create an assembly.
// ---
void CreateAssembly()
{  
  std::vector< SPtr<MbItem> > addItems;

  MbItem * item = nullptr;
  PathMatrix pathFrom;
  MbRefItem * geom = nullptr;

  while ( GetRigidObj(IDS_SELECT_GEOM_OBJECT, item, geom, pathFrom ) ) 
  {
    if ( item != nullptr && std::find(addItems.begin(), addItems.end(), item) == addItems.end() )
    {
      //if ( !pathFrom.second.IsSingle() ) {
      //  item = static_cast<MbItem *>( &item->Duplicate() );
      //  item->Transform( pathFrom.second );
      //}      
      addItems.push_back( SPtr<MbItem>(item) );
    }
  }
  
  if ( !addItems.empty() ) 
  {
    SetWaitCursor( true );

    Style ns = SHELL_Style;
    for ( size_t i = 0, iCount = addItems.size(); i < iCount; i++ )
    {
      viewManager->DeleteObject( addItems[i] );
      addItems[i]->SetItemName( UNDEFINED_SNAME );
    }
    
    SPtr<MbItem> result( new MbAssembly(addItems) );
    viewManager->AddObject( ns, result );

    SetWaitCursor( false );
  }
}

//----------------------------------------------------------------------------------------
// Создать вставку.
// Create the instance.
// ---
void CreateInstance() 
{
  MbItem * obj = nullptr;
  if ( GetGObj( IDS_SELECT_GEOM_OBJECT, st_SpaceItem, &obj ) ) 
  {
    SetWaitCursor( true );

    MbPlacement3D place;
    // Для тестирования MbPlacement3D place( viewManager->GetCplane() );

    MbItem * result = new MbInstance( *obj, place );

    Style ns = SHELL_Style;
    viewManager->DeleteObject( obj );
    viewManager->AddObject( ns, result );

    SetWaitCursor( false );
  }
}


//----------------------------------------------------------------------------------------
// Создать полигональный объект вместо указанного.
// Create polygonal object instead a pointed.
//---
void CreateMesh()
{
  MbItem * obj = nullptr;

  if ( GetGObj( IDS_SELECT_GEOM_OBJECT, st_SpaceItem, &obj ) ) 
  {
    MbStepData stepData;
    int mode = TestVariables::operateType;
    if ( ::InitStepData( stepData, false, mode ) ) 
    {
      int exact = 1;
      GetInt( IDS_DO_EXACT_OBJECT, exact );
      bool doExact = ( exact == 0 ) ? false : true; // \ru Выполнить построение полигональных объектов на числах double (true) на числах float (false). \en Polygonal objects will created on double data (true) on float data (false).

      int ttt = 1;
      GetInt( IDS_ENTER_DUAL_SEAMS, ttt );
      bool dualSeams = ( ttt == 0 ) ? false : true;

      SetWaitCursor( true );
      bool wire = false;
      bool grid = true;
      MbRegDuplicate * iReg = nullptr;

      MbItem * result = obj->CreateMesh( stepData, MbFormNote(wire, grid, dualSeams, doExact, false), iReg ); 

      if ( MbMesh * mesh = isa_cast<MbMesh*>(result) )
        mesh->SetMeshType( st_Mesh );
      Style ns = SHELL_Style;
      viewManager->DeleteObject( obj );
      viewManager->AddObject( ns, result );

      SetWaitCursor( false );
    }
  }
}


//----------------------------------------------------------------------------------------
// Создать полигональный объект по триангуляции грани тела. 
// Create polygonal object by solid face triangulation.
//---
void CreateGrid()
{
  MbStepData stepData;
  int mode = TestVariables::operateType;
  if ( ::InitStepData( stepData, false, mode ) ) 
  {
    int exact = 1;
    GetInt( IDS_DO_EXACT_OBJECT, exact );
    bool doExact = ( exact == 0 ) ? false : true; // \ru Выполнить построение полигональных объектов на числах double (true) на числах float (false). \en Polygonal objects will created on double data (true) on float data (false).
    MbItem * obj = nullptr;
    MbTopItem * topFace = nullptr;

    while ( GetTObj( IDS_SELECT_FACE, st_Solid, tt_Face, &obj, &topFace, TestVariables::itemPathFrom.second ) ) {
      TestVariables::viewManager->ToggleObject(obj);
      TestVariables::drawImp->DrawItem( topFace, TRGB_WHITE );

      MbFace * face = static_cast<MbFace *>(topFace);

      SetWaitCursor( true );

      MbMesh * mesh = new MbMesh( doExact );
      MbGrid * grid = mesh->AddGrid();
      if ( grid != nullptr ) {
        ::CalculateGrid( *face, stepData, *grid, true, false, true );
        face->AttributesConvert( *grid );
        grid->SetItem( face );
        grid->SetPrimitiveName( face->GetNameHash() );
        grid->SetPrimitiveType( rt_TopItem );
        grid->SetStepData( stepData );
        mesh->SetMeshType( st_Mesh );
      }

      if ( mesh->IsComplete() ) {
        Style ns = TestVariables::CONIC_Style;
        TestVariables::viewManager->AddObject( ns, mesh );
      }
      SetWaitCursor( false );
      TestVariables::viewManager->ToggleObject(obj);
    }
  }
}


//----------------------------------------------------------------------------------------
// Создать абрис объект
// Create sketch of a mesh object
// ---
void CreateConvexPolyhedron() 
{
  MbItem * obj = nullptr;

  if ( GetGObj(IDS_SELECT_GEOM_OBJECT, st_SpaceItem, &obj) ) 
  {
    double sag = Math::visualSag;
    MbeStepType stepType = ist_SpaceStep;
    GetDouble( IDS_ENTER_NUMBER, sag );

    SetWaitCursor( true );
    bool wire = false;
    bool grid = true;
    MbRegDuplicate * iReg = nullptr;

    MbItem * item = obj->CreateMesh( MbStepData(stepType, sag), MbFormNote(wire, grid, true, TestVariables::exactMesh), iReg ); 
    MbMesh * result = nullptr;

    if ( MbMesh * mesh = isa_cast<MbMesh*>(item) ) 
    {        
      if ( mesh->GridsCount() > 0 && mesh->GetMeshType() == st_Solid ) 
      {
        MbFloatPoint3D pnt;
        SArray<MbFloatPoint3D> points( 0, 1 );
        size_t j = 0, jCount = mesh->GridsCount();
        for ( j = 0; j < jCount; j++ ) {
          const MbGrid * jGrid = mesh->GetGrid( j );
          if ( jGrid != nullptr ) {
            for ( size_t k = 0, kCount = jGrid->PointsCount(); k < kCount; k++ ) {
              jGrid->GetPoint(k,pnt);
              points.Add( pnt );
            }
          }
        }
        ::CreateConvexPolyhedron( points, result );
      }
    }
    ::DeleteItem( item );

    if ( result != nullptr ) {
      Style ns = SHELL_Style;
      viewManager->DeleteObject( obj );
      viewManager->AddObject( ns, result );
    }
    ::DeleteItem( result );

    SetWaitCursor( false );
  }
}


//------------------------------------------------------------------------------
// Создать крепеж по точке.
// Create fastener using a point.
// ---
void CreateFastenerByPoint()
{
  RPArray<MbSolid> solids;  // тела
  RPArray<MbItem> selectedItems;
  MbCartPoint3D point;      // точка
  MbItem * obj( nullptr );

  while ( GetGObj( IDS_SELECT_SOLID, st_Solid, &obj ) ) {
    TestVariables::viewManager->ToggleObject( obj );
    selectedItems.Add( obj );
    solids.Add( static_cast<MbSolid *>(obj) );
  }

  for ( size_t i = 0; i < selectedItems.Count(); i++ )
    TestVariables::viewManager->ToggleObject( selectedItems[i] );

  if ( solids.Count() > 0 ) {
    if ( GetPoint( IDS_SELECT_POINT, point ) ) {
      TestVariables::drawImp->DrawPoint( &point, TRGB_LIGHTGREEN );
      int ftype = 0;
      if ( GetInt( IDS_ENTER_FASTENER_TYPE, ftype ) ) {
        FastenersValues::MbeFastenerType fastenerType = static_cast<FastenersValues::MbeFastenerType>( ftype );
        double diam = 1.0;
        if ( GetDouble( IDS_ENTER_DIAMETER, diam ) ) {
          ThreeStates rivetAndHole = ts_positive;
          int nh = 1;
          GetInt( IDS_ENTER_HOLES_ONLY, nh );
          if ( nh == 0 )
            rivetAndHole = ts_neutral;
          if ( nh < 0 )
            rivetAndHole = ts_negative;
          double angle = M_PI_2;                          // параметры фаски и параметров головки заклепки пока не запрашиваем у пользователя
          double depth = diam / 4.0;
          double hbd = 2.0 * diam;
          double hh = diam / 2.0;
          FastenersValues params( fastenerType, diam, angle, depth, hbd, hh, rivetAndHole );  
          MbSNameMaker names( ct_BooleanSolid, MbSNameMaker::i_SideNone, 0 );
          RPArray<MbSolid> results( solids.size(), 1 );
          MbResultType res = ::CreateFastener( solids, cm_Copy, point, params, names, results );

          if ( res == rt_Success ) {
            if ( rivetAndHole != ts_neutral ) {
              for ( size_t i = 0; i < selectedItems.Count(); i++ )
                TestVariables::viewManager->DeleteObject( selectedItems[i] );
            }
            for ( size_t i = 0; i < results.Count(); i++ )
              TestVariables::viewManager->AddObject( TestVariables::SOLID_Style, results[i] );
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
// Создать крепеж по кривой.
// Create fastener using a curve.
// ---
void CreateFastenerByCurve()
{
  RPArray<MbSolid> solids;  // тела
  RPArray<MbItem> selectedItems;
  MbCurve3D * curve( nullptr );        // кривая
  MbItem * obj( nullptr );
  MbItem * cobj( nullptr );

  while ( GetGObj( IDS_SELECT_SOLID, st_Solid, &obj ) ) {
    TestVariables::viewManager->ToggleObject( obj );
    selectedItems.Add( obj );
    solids.Add( static_cast<MbSolid *>(obj) );
  }

  for ( size_t i = 0; i < selectedItems.Count(); i++ )
    TestVariables::viewManager->ToggleObject( selectedItems[i] );

  if ( solids.Count() > 0 ) {
    if ( GetGObj( IDS_SELECT_CURVE, st_Curve3D, &cobj ) ) {
      if ( cobj->IsA() == st_SpaceInstance ) {
        MbSpaceItem * sitem = static_cast<MbSpaceInstance *>( cobj )->SetSpaceItem();
        TestVariables::viewManager->ToggleObject( cobj );
        if ( sitem != nullptr && (sitem->Family() == st_Curve3D) ) {
          curve = static_cast<MbCurve3D *>( sitem );
          int ftype = 0;
          if ( GetInt( IDS_ENTER_FASTENER_TYPE, ftype ) ) {
            FastenersValues::MbeFastenerType fastenerType = static_cast<FastenersValues::MbeFastenerType>( ftype );
            double diam = 1.0;
            if ( GetDouble( IDS_ENTER_DIAMETER, diam ) ) {
              int num = 5;
              if ( GetInt( IDS_ENTER_POINT_NUMBER, num ) ) {
                ThreeStates rivetAndHole = ts_positive;
                int nh = 1;
                GetInt( IDS_ENTER_HOLES_ONLY, nh );
                if ( nh == 0 )
                  rivetAndHole = ts_neutral;
                if ( nh < 0 )
                  rivetAndHole = ts_negative;
                double angle = M_PI_2;                          // параметры фаски и параметров головки заклепки пока не запрашиваем у пользователя
                double sideLength = diam / 4.0;
                double hbd = 2.0 * diam;
                double hh = diam / 2.0;
                FastenersValues params( fastenerType, diam, angle, sideLength, hbd, hh, rivetAndHole );
                MbSNameMaker names( ct_BooleanSolid, MbSNameMaker::i_SideNone, 0 );
                RPArray<MbSolid> results( solids.size(), 1 );

#if defined(C3D_DEBUG)
                SET_TIME_TEST(true);
#endif // C3D_DEBUG
                uint32 miliSec = ::GetTickCountEx();

                MbResultType res = ::CreateFasteners( solids, cm_Copy, *curve, num, params, names, results );

                miliSec = ::GetTickCountEx() - miliSec;

                TestVariables::viewManager->ToggleObject( cobj );

                if ( res == rt_Success ) {
                  TimeTest * testResult = nullptr;
#if defined(C3D_DEBUG)
                  testResult = ::GetTimeTestResults();
#endif // C3D_DEBUG
                  SArray<Style> sts( selectedItems.Count(), 1 );

                  if ( rivetAndHole != ts_neutral ) {
                    for ( size_t i = 0; i < selectedItems.Count(); i++ ) {
                      Style st( selectedItems[i]->GetWidth(), selectedItems[i]->GetColor() );
                      sts.Add( st );
                      TestVariables::viewManager->DeleteObject( selectedItems[i] );
                    }
                  }
                  else {
                    if ( solids.size() == 2 ) {
                      std::vector< std::pair<size_t, size_t> > facesNumbers;
                      ::FindTouchedFaces( *(solids[0]), *(solids[1]), METRIC_PRECISION, facesNumbers );
                      for ( std::vector< std::pair<size_t, size_t> >::iterator iter = facesNumbers.begin(); iter != facesNumbers.end(); ++iter ) {
                        size_t i1 = iter->first;
                        size_t i2 = iter->second;
                        MbFace * face1 = solids[0]->GetFace( i1 );
                        MbFace * face2 = solids[1]->GetFace( i2 );
                        TestVariables::drawImp->DrawItem( face1, TRGB_CERISE );
                        TestVariables::drawImp->DrawItem( face2, TRGB_OLIVE );
                      }
                    }
                  }
                  size_t stssize = sts.size();
                  for ( size_t i = 0; i < results.Count(); i++ ) {
                    Style st( TestVariables::GOLDEN_Style );
                    if ( i < stssize )
                      st = sts[i];
                    TestVariables::viewManager->AddObject( st, results[i] );
                  }
                  size_t operationCount = 0;
                  size_t facesCount = 0;
                  string_t rebuildMsg( RT_REBUILDING_BODY );
                  ShowTimeTestResult( testResult, operationCount, facesCount, miliSec, true, rebuildMsg.c_str() );
                }
#if defined(C3D_DEBUG)
                SET_TIME_TEST(false);
#endif // C3D_DEBUG
              }
            }
          }
        }
      }
    }
  }
}
