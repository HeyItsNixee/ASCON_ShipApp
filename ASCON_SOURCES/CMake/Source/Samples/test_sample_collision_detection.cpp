#include <test_samples.h>
//
#include <test_manager.h>
#include <action_solid.h>
#include <solid.h>
#include <instance_item.h>
#include <mb_class_traits.h>
#include <cdet_utility.h>
#include <math_namespace.h>
#include <last.h>

using namespace c3d;


#ifndef _UNUSED_VAR
#define _UNUSED_VAR( var ) (void*)(&var)
#endif

#ifndef NULL_PTR
const void * NULL_PTR = 0; // Перенести в math_define.h ?
#endif

//----------------------------------------------------------------------------------------
// The func simulates a semantic environment of google unit testing
//---
template<class _Type1,class _Type2>
static void EXPECT_EQ( const _Type1 & t1, const _Type2 & t2 )
{
  _UNUSED_VAR(t1);
  _UNUSED_VAR(t2);
  PRECONDITION( t1 == t2 );
}

//----------------------------------------------------------------------------------------
// The func simulates a semantic environment of google unit testing
//---
template<class _Type1,class _Type2>
static void EXPECT_NE( const _Type1 & t1, const _Type2 & t2 )
{
  _UNUSED_VAR(t1);
  _UNUSED_VAR(t2);
  PRECONDITION( t1 != t2 );
}

//----------------------------------------------------------------------------------------
// Return an elementary solid by the shape type and characteristic points.
//---
static SPtr<MbSolid> _ElementarySolid(  const SArray<MbCartPoint3D> & points
                                      , ElementaryShellType solidType
                                      , const MbSNameMaker & names )
{  
  SolidSPtr solidPtr;
  SpacePointsVector auxPoints( points.begin(), points.end() );
  ::ElementarySolid( MbElementarySolidParams( solidType, auxPoints, names ), solidPtr ); // ElementarySolid( points, solidType, names, solid );

  return solidPtr;
}

//----------------------------------------------------------------------------------------
// Demonstrating sample: collision detection
/*
  This sample demonstrates how to apply collision detection utility to show
  interferencies of solids (the code is able to use in unit-tests).
*/
// ---
void SampleCollisionDetection()
{  
  /*
    Create a box, first solid.
  */
  MbSNameMaker names1( 1, MbSNameMaker::i_SideNone, 0 );
  SArray<MbCartPoint3D> points;
  points.push_back( MbCartPoint3D(0, 0, 0) );
  points.push_back( MbCartPoint3D(100, 0, 0) );
  points.push_back( MbCartPoint3D(0, 30, 0) );
  points.push_back( MbCartPoint3D(0, 0, 60) );
  SPtr<MbSolid> box1( _ElementarySolid(points, et_Block, names1) );

  /*
    Create a box, second solid.
  */
  MbSNameMaker names2( 2, MbSNameMaker::i_SideNone, 0 );
  points.clear();
  points.push_back( MbCartPoint3D(10, 20, 1) );
  points.push_back( MbCartPoint3D(100, 0, 0) );
  points.push_back( MbCartPoint3D(0, 30, 0) );
  points.push_back( MbCartPoint3D(0, 0, 60) );
  SPtr<MbSolid> box2( _ElementarySolid(points, et_Block, names2) );
  EXPECT_NE( box1, NULL_PTR );
  EXPECT_NE( box2, NULL_PTR );
  if ( box1 && box2 )
  {
    /*
      Both solids positioned as instancies having their own LCS (placement).
    */
    const MbPlacement3D lcs2( MbCartPoint3D(50, 15, 10) );
    SPtr<MbInstance> item1( new MbInstance(*box1, MbPlacement3D::global) );
    SPtr<MbInstance> item2( new MbInstance(*box2, lcs2) );

    TestVariables::viewManager->AddObject( TestVariables::ELEMENTARY_Style, item1 );
    TestVariables::viewManager->AddObject( TestVariables::ELEMENTARY_Style, item2 );    

    MbCollisionDetectionUtility cdet;

    cdet_item cdRep1 = cdet.AddSolid( *box1, item1->GetPlacement(), (void*)item1.get() );
    cdet_item cdRep2 = cdet.AddSolid( *box2, item2->GetPlacement(), (void*)item2.get() );
    EXPECT_NE( cdRep1, CDET_NULL );
    EXPECT_NE( cdRep2, CDET_NULL );
    
    /*
      Detect the collisions of the solids.
    */

    cdet_collided_faces colQuery;
    cdet_result res = cdet.CheckCollisions( colQuery );
    EXPECT_EQ( res, CDET_RESULT_Intersected );

    /*
      The cycle colouring faces interfering with each other.
    */

    cdet_collided_faces::collided_faces::iterator iter, last;
    for ( iter = colQuery.faces.begin(), last = colQuery.faces.end(); iter != last; ++iter )
    {
      const MbFace * face = isa_cast<const MbFace*>( iter->second );
      EXPECT_NE( face, NULL_PTR );
      const_cast<MbFace*>(face)->SetColor( iter->first == item1 ? RGB(0xFF,0,0):RGB(0xFF,0xFF,0) );
    }
    EXPECT_EQ( colQuery.faces.size(), 4 );
    TestVariables::viewManager->RefreshModel();    
  }
}

// eof