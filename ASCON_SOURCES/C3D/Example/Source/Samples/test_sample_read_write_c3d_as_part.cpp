//////////////////////////////////////////////////////////////////////////////////////////
//
//    The file contains source codes demonstrating samples 
//    how to read and write c3d data as part of binary file.
//
//    WARNING: This code is only an example. The real application can require more 
//             functionality and data types for implementation.
// 
//////////////////////////////////////////////////////////////////////////////////////////

#include <test_samples.h>
#include <test_variables.h>
#include <test_manager.h>
#include <test_service.h>
//
#include <templ_s_array.h>
#include <templ_sptr.h>
#include <io_tape.h>
#include <io_memory_buffer.h>
#include <mb_operation_result.h>
#include <mb_cart_point3d.h>
#include <name_item.h>
#include <creator.h>
#include <solid.h>
#include <model_entity.h>
#include <action_solid.h>
//
#include <math_namespace.h>
#include <last.h>

using namespace c3d;


//------------------------------------------------------------------------------
// 
// ---
inline
MbSolid * CreateSampleSphere( double radius, double xpos )
{
  SpacePointsVector points;
  points.push_back( MbCartPoint3D( xpos, 0.0, 0.0) );
  points.push_back( MbCartPoint3D( xpos, 0.0, radius) );
  points.push_back( MbCartPoint3D( xpos + radius, 0.0, 0.0) );
  MbSNameMaker operNames( ct_ElementarySolid, MbSNameMaker::i_SideNone, 0 );
  SolidSPtr solid;
  ::ElementarySolid( MbElementarySolidParams(ElementaryShellType::et_Sphere, points, operNames), solid ); // ::ElementarySolid( points, ElementaryShellType::et_Sphere, operNames, sphere );

  PRECONDITION( solid != nullptr );
  return solid.detach();
}

//------------------------------------------------------------------------------
// 
// ---
static
void ReadWriteModelAsBinaryPart()
{
  SPtr<MbModel> model;

  SPtr<MbSolid> solid1( ::CreateSampleSphere( 10.0,  0.0 ) );
  SPtr<MbSolid> solid2( ::CreateSampleSphere( 10.0, 50.0 ) );

  model = new MbModel;
  model->AddItem( *solid1, 1 );
  model->AddItem( *solid2, 2 );

  uint64 signature0 = 3210123; // sample signature of c3d-data in binary file.

  { // Write model items
    std::ofstream file( "c:/temp/rwTest.dat", std::ios::binary );

    if ( file.is_open() ) {
      membuf memBuf;
      {
        writer::writer_ptr wrt = writer::CreateMemWriter( memBuf, 0 );

        ::WriteModelItems( *wrt, *model );
        memBuf.setMaxRegCount( wrt->GetMaxRegisteredCount() );
        memBuf.closeBuff();

        uint64 bufsize = memBuf.getMemLen();
        const char * buffer = nullptr;
        memBuf.toMemory(buffer);

        file.write( reinterpret_cast<char *>(&signature0), sizeof(signature0) );
        file.write( reinterpret_cast<char *>(&bufsize), sizeof(bufsize) );
        file.write( buffer, std::streamsize(bufsize) );
        _ASSERT( !file.fail() );
        file.close();

        delete[] buffer;
      }
    }
  }

  model = nullptr;

  { // Read model items
    std::ifstream file("c:/temp/rwTest.dat",std::ios::binary);

    if ( file.is_open() ) {
      uint64 signature = 0;
      file.read( reinterpret_cast<char *>(&signature), sizeof(signature) );
      PRECONDITION( signature0 == signature );
      if ( signature0 == signature ) {
        uint64 bufsize = 0;
        file.read( reinterpret_cast<char *>(&bufsize), sizeof(bufsize) );
        PRECONDITION( bufsize > 0 );
        if ( bufsize > 0 ) {
          char * buffer = new char[bufsize];
          file.read( buffer, bufsize );
          file.close();

          model = new MbModel();

          membuf memBuf;
          memBuf.fromMemory( buffer );
          {
            reader::reader_ptr rdr = reader::CreateMemReader( memBuf, 0 );
            ::ReadModelItems( *rdr, *model );
            PRECONDITION( model->ItemsCount() == 2 );
          }
          delete [] buffer;
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
// How to read and write c3d data as part of binary file.
// ---
void SampleReadWriteModelAsPart()
{
  ::ReadWriteModelAsBinaryPart();
}
