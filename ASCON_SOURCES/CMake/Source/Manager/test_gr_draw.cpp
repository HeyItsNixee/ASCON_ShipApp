#include <test_gr_draw.h>
#include <test_manager.h>
#include <test_service.h>
#include <test.h>
#include <mb_axis3d.h>
#include <mb_class_traits.h>
#include <mesh.h>
#include <assembly.h>
#include <instance_item.h>
#include <attr_color.h>
#include <attr_selected.h>
#if defined(C3D_MacOS) // mac os X
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif // mac os X
#include <time.h>
#include <math_namespace.h>
#include <last.h>

using namespace c3d;


///////////////////////////////////////////////////////////////////////////////////////////
//
// Инструмент рисования в конкретном окне 
//
///////////////////////////////////////////////////////////////////////////////////////////


#define PIXEL_TO_INCH     96.0   // Число пикселей в дюйме (будет пересчитано).

//------------------------------------------------------------------------------
// Отрисовка вершин с указанной толщиной, если для вершины задан атрибут цвета.
// ---
// #define DEBUG_DRAW_MESH_POINTS
#ifdef DEBUG_DRAW_MESH_POINTS
#define DEBUG_DRAW_MESH_POINTS_WIDTH 5 // Размер вершин для отладочной отрисовки
#endif // DEBUG_DRAW_MESH_POINTS


IGrDraw::IGrDraw( WorkWindow& _parent, const MbVector3D & lt )
  : parent        ( _parent )
  , backgr        ( 255 )
  , xPixelPerMM   ( double( PIXEL_TO_INCH )/LOGIC_INCH )
  , yPixelPerMM   ( double( PIXEL_TO_INCH )/LOGIC_INCH )
  , lightLocal    ()
  , lightSide     ()
  , lightAmbient  ( (float)MB_AMBIENT )
  , lightDiffuse  ( (float)MB_DIFFUSE )
  , lightSpecular ( (float)MB_SPECULARITY )
  , oglInitialized( false )
  , oglModelReady ( false )
  , renderMode    ( ovm_NormalRenderWithEdges )//( ovm_NormalRender )
{
  SetLight( lt );
}


//------------------------------------------------------------------------------
// Инициализация для работы окна в полутоновом режиме
// ---
bool IGrDraw::InitializeGL() 
{
  float red = (float)( GetRValue(backgr)/255.0 );
  float gre = (float)( GetGValue(backgr)/255.0 );
  float blu = (float)( GetBValue(backgr)/255.0 );
  glClearColor (red, gre, blu, 0.0);

  // Enable depth testing and backface culling.
  glEnable (GL_DEPTH_TEST);
//  glEnable (GL_CULL_FACE);
  glEnable( GL_FRONT_AND_BACK );
  glEnable( GL_NORMALIZE ); // Влияет на хиты для селектирования при разных видеокартах
//  glDisable( GL_NORMALIZE ); // Влияет на хиты для селектирования при разных видеокартах

  // Add a light to the scene.
  GLfloat glfLightAmbient[]  = { lightAmbient, lightAmbient, lightAmbient, 1.0f };
  GLfloat glfLightDiffuse[]  = { lightDiffuse, lightDiffuse, lightDiffuse, 1.0f };
  GLfloat glfLightSpecular[] = { lightSpecular, lightSpecular, lightSpecular, 1.0f };
  glLightfv (GL_LIGHT0, GL_AMBIENT,  glfLightAmbient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE,  glfLightDiffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, glfLightSpecular);

  GLfloat glfLightLocal[] = { (GLfloat)lightLocal.x, (GLfloat)lightLocal.y, (GLfloat)lightLocal.z, 1.0f };
  GLfloat glfLightSide[]  = { (GLfloat)lightSide.x,  (GLfloat)lightSide.y,  (GLfloat)lightSide.z,  1.0f };
  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, glfLightAmbient );
  glLightModelfv( GL_LIGHT_MODEL_LOCAL_VIEWER, glfLightLocal );
  glLightModelfv( GL_LIGHT_MODEL_TWO_SIDE, glfLightSide );

  glDisable( GL_BLEND ); // Запрещаем смешивание цветов (Непрозрачность)
//  glDisable(GL_DEPTH_TEST);
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );

  ::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  ::glMatrixMode( GL_MODELVIEW ); // Делаем текущей видовую матрицу
/*
  {
    // Альфа-копонент не используется до тех пор, пока не включено смешивание (глава 6).
    // До этого момента его можно смело игнорировать.

    glClearColor(1.0, 1.0, 1.0, 0.0);

    GLfloat light_ambient[]        = {lightAmbient, lightAmbient, lightAmbient, 1.0}; // Cвет фонового освещения - ослепление.
    GLfloat light_diffuse[]        = {lightDiffuse, lightDiffuse, lightDiffuse, 1.0}; // Cвет рассеянного освещения.
    GLfloat light_specular[]       = {lightSpecular, lightSpecular, lightSpecular, 1.0}; // Cвет отраженного света - матовость.
    GLfloat light_position[]       = {1.0, 1.0, 1.0, 0.0}; // Координаты источника света. Свет из бесконечности.
    GLfloat light_spot_direction[] = {0.0, 0.0, -1.0};     // Направление распространения света.

    GLfloat light_model_ambient[]  = {(GLfloat)0.1, (GLfloat)0.1, (GLfloat)0.1, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT,        light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,        light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR,       light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION,       light_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient); // Оставляем.

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
  }
*/
  return true;
}


//------------------------------------------------------------------------------
// Установить сцену
// ---
void IGrDraw::SetSizeGL( int cx, int cy, double scale ) 
{
  ::glViewport( 0, 0, cx, cy );   // Задаем размеры окна вывода

  ::glMatrixMode( GL_PROJECTION );  // Делаем текущей проекционную матрицу
  ::glLoadIdentity();               // Матрицу делаем единичной

  // Установить проекционную матрицу
  double sideX = (double)cx / xPixelPerMM / scale;
  double sideY = (double)cy / yPixelPerMM / scale;
  double sideZ = std_max( sideX, sideY ) * DELTA_MAX;

  // Устанавливаем объем видимости для определения матрицы перспективы
//  if ( vista.z>DELTA_MIN ) {
//    double angleY = atan2(vista.y/factorY/scale,vista.z/factorY/scale)*360/M_PI;
//    ::gluPerspective(angleY, sideX/sideY, 0.1*vista.z/factorY/scale, sideZ);
//    ::glFrustum(-0.05*sideX, 0.05*sideX, -0.05*sideY, 0.05*sideY, 0.1*vista.z/factorY/scale, sideZ );
//  }
//  else
    ::glOrtho( 0, sideX, -sideY, 0, -sideZ, sideZ );
}


//------------------------------------------------------------------------------
// Очистить OpenGL 
// ---
void IGrDraw::ClearGL() const
{
  ::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  //SetGLModelViewMatrix();
  {
    ::glMatrixMode( GL_MODELVIEW ); // Делаем текущей видовую матрицу
  }
}


//------------------------------------------------------------------------------
// Загрузить OpenGL видовую матрицу преобразования
// ---
void IGrDraw::MatrixGL() const
{
  MbMatrix3D into0            = parent.GetOwnPlacement().GetMatrixInto();
  const MbCartPoint3D & vista = parent.GetVista();
  double scale                = parent.GetScale();

  if ( vista.z > DELTA_MIN ) {
    double zPixelPerMM = ONE_HALF * ( xPixelPerMM + yPixelPerMM );
    double x = -vista.x / xPixelPerMM / scale;
    double y =  vista.y / yPixelPerMM / scale;
    double z = -vista.z / zPixelPerMM / scale;
    ::glLoadIdentity();                  // Матрицу делаем единичной
    ::glTranslated( x, y, z );           // Сдвигаем матрицу
    ::glMultMatrixd( (double *)&into0 ); // Умножаем на матрицу преобразования вида
  }
  else {
    ::glLoadMatrixd( (double *)&into0 ); // Прираниваем видовую матрицу матрице into0
  }
}


//------------------------------------------------------------------------------
// Подготовить данные ГСК
// ---
static
void FillWCS( MbMesh & mesh )
{
  mesh.Flush();

  SPtr<MbVisual> meshVisual( new MbVisual );
  meshVisual->SetOpacity( 0.5 );
  mesh.AddAttribute( meshVisual );

  double scale = TestVariables::viewManager->GetWindowScale();
  double l = WCS_AXIS_LENGTH / scale;

  MbCartPoint3D p0( 0, 0, 0 ),
                px( l, 0, 0 ),
                py( 0, l, 0 ),
                pz( 0, 0, l ); // Точки в мировой СК

  mesh.ApexReserve( 1 );
  mesh.PolygonsReserve( 3 );

  MbApex3D * apex = mesh.AddApex(); // добавить новый пустой объект
  apex->Init( p0 );
  {
    MbCartPoint3D pnt;
    SPtr<MbColor> polyColor;

    MbPolygon3D * poly = mesh.AddPolygon();
    poly->Reserve( 2 );
    poly->AddPoint( p0 );
    poly->AddPoint( px );
    polyColor = new MbColor( ::RGB2uint32( 1, 0, 0 ) );
    poly->AddAttribute( polyColor );

    poly = mesh.AddPolygon();    
    poly->Reserve( 2 );
    poly->AddPoint( p0 );
    poly->AddPoint( py );
    polyColor = new MbColor( ::RGB2uint32( 0, 1, 0 ) );
    poly->AddAttribute( polyColor );

    poly = mesh.AddPolygon();    
    poly->Reserve( 2 );
    poly->AddPoint( p0 );
    poly->AddPoint( pz );
    polyColor = new MbColor( ::RGB2uint32( 0, 0, 1 ) );
    poly->AddAttribute( polyColor );
  }

  mesh.SetMeshType( st_AssistedItem );
}


//------------------------------------------------------------------------------
// Подготовить данные
// ---
void IGrDraw::PrepareGL( const MbModel & drawModel, COLORREF select ) const
{
  // Заполняем листы сеткой объектов
  MbModel::ItemConstIterator drawIter( drawModel.CBegin() );
  MbModel::ItemConstIterator drawEnd ( drawModel.CEnd() );

  if ( TestVariables::enableWCS ) // Отрисовка ГСК
  { 
    MbMesh wcsMesh;
    MbMatrix3D wcsMatr;
    ::FillWCS( wcsMesh );
    RenderItemGL( &wcsMesh, wcsMatr, nullptr, nullptr, select, false );
  }

  const MbVisual * hostVisual = static_cast<const MbVisual *>( drawModel.GetSimpleAttribute(at_Visual) );
  const MbColor *  hostColor  = static_cast<const MbColor *> ( drawModel.GetSimpleAttribute(at_Color) );

  for ( ; drawIter != drawEnd; ++drawIter ) 
  {    
    if ( const MbItem * obj = *drawIter ) 
    {
      MbMatrix3D matr;
      RenderItemGL( obj, matr, hostVisual, hostColor, select, obj->IsSelected() );
    }
#ifdef __USE_GL_LISTS
    GLuint listName = (GLuint)(uint)(size_t)obj;
    if ( glIsList( (GLuint)listName ) == GL_FALSE ) 
      glCallList( listName );
#endif // __USE_GL_LISTS
  }
}


//------------------------------------------------------------------------------
// Очистить OpenGL
// ---
void IGrDraw::FinishGL() const 
{
  ::glFinish();
}


//------------------------------------------------------------------------------
// Нарисовать сетку
// ---
static
void RenderGridGL( const MbGrid * gr, const MbMatrix3D & from, 
                   COLORREF parentColor, COLORREF otherColor, bool useOtherColor, OGLViewMode renderMode,
                   float ambient, float diffuse, float specularity,
                   float shininess, float opacity, float emission )
{
  PRECONDITION( gr );

  bool visual_bit = false;
  const MbColor * colorAttr = static_cast<const MbColor *>( gr->GetSimpleAttribute( at_Color ) );

  float ambientR = ambient;
  float ambientG = ambient;
  float ambientB = ambient;

  float diffuseR = diffuse;
  float diffuseG = diffuse;
  float diffuseB = diffuse;

  const MbVisual * visualAttr = static_cast<const MbVisual *>( gr->GetSimpleAttribute( at_Visual ) );
  if ( visualAttr != nullptr ) {
    visualAttr->Get( ambientR, diffuseB, specularity, shininess, opacity, emission, 0 );
    visualAttr->Get( ambientG, diffuseG, specularity, shininess, opacity, emission, 1 );
    visualAttr->Get( ambientB, diffuseB, specularity, shininess, opacity, emission, 2 );

    ambient = ((float)ONE_THIRD) * (ambientR + ambientG + ambientB);
    diffuse = ((float)ONE_THIRD) * (diffuseR + diffuseG + diffuseB);
  }

  if ( colorAttr != nullptr || visualAttr != nullptr ) {
    visual_bit = true;
    ::glPushAttrib( GL_LIGHTING_BIT );

    uint32 color;

    if ( colorAttr != nullptr ) {
      color = colorAttr->Color();
      if ( useOtherColor )
        color = otherColor;
    }
    else {
      color = parentColor;
      if ( useOtherColor )
        color = otherColor;
    }

    GLfloat R = (GLfloat)( GetRValue( color ) / 255.0 );
    GLfloat G = (GLfloat)( GetGValue( color ) / 255.0 );
    GLfloat B = (GLfloat)( GetBValue( color ) / 255.0 );

    if ( opacity < 1.0 - DELTA_MIN ) // Прозрачность
      glEnable( GL_BLEND ); // Разрешаем смешивание цветов (прозрачность)
    else
      glDisable(GL_BLEND ); // Запрещаем смешивание цветов

    GLfloat colorAmbient[]  = { R * ambientR,    G * ambientG,    B * ambientB,    opacity }; // Фоновый цвет материала - ослепление. (перемножаются)
    GLfloat colorDiffuse[]  = { R * diffuseR,    G * diffuseG,    B * diffuseB,    opacity }; // Рассеянный цвет материала - цвет. (перемножаются)
    GLfloat colorSpecular[] = { R * specularity, G * specularity, B * specularity, opacity }; // Цвет отраженного света - матовость (перемножаются)
    GLfloat colorEmission[] = { R * emission,    G * emission,    B * emission,    opacity }; // Излучаемый цвет материала - светимость.
    GLfloat colorIndexes[]  = { ambient, diffuse, opacity }; // Индексы фонового, рассеянного и отражённого цветов.

    glMaterialfv( GL_FRONT, GL_AMBIENT,   colorAmbient  );
    glMaterialfv( GL_FRONT, GL_DIFFUSE,   colorDiffuse  );
    glMaterialfv( GL_FRONT, GL_SPECULAR,  colorSpecular );
    glMaterialfv( GL_FRONT, GL_EMISSION,  colorEmission );
    glMaterialf ( GL_FRONT, GL_SHININESS, shininess     );
    glMaterialfv( GL_FRONT, GL_COLOR_INDEXES, colorIndexes );

    R = (GLfloat)((R + 1.0) / 2.0f); // Смесь с серым - цвет изнанки незамкнутого тела
    G = (GLfloat)((G + 1.0) / 2.0f); // Смесь с серым - цвет изнанки незамкнутого тела
    B = (GLfloat)((B + 1.0) / 2.0f); // Смесь с серым - цвет изнанки незамкнутого тела
    float opacity0 = opacity; // Непрозрачность
    if ( opacity0 < DELTA_MIN ) // Полная прозрачность
      opacity0 = (float)DELTA_MOD;
    GLfloat colorAmbient0[]  = { R * ambientR,    G * ambientG,    B * ambientB,    opacity0 };
    GLfloat colorDiffuse0[]  = { R * diffuseR,    G * diffuseG,    B * diffuseB,    opacity0 };
    GLfloat colorSpecular0[] = { R * specularity, G * specularity, B * specularity, opacity0 };
    GLfloat colorEmission0[] = { R * emission,    G * emission,    B * emission,    opacity0 };
    GLfloat colorIndexes0[]  = { ambient, diffuse, opacity0 }; // Индексы фонового, рассеянного и отражённого цветов.
    glMaterialfv( GL_BACK, GL_AMBIENT, colorAmbient0 );
    glMaterialfv( GL_BACK, GL_DIFFUSE, colorDiffuse0 );
    glMaterialfv( GL_BACK, GL_SPECULAR, colorSpecular0 );
    glMaterialfv( GL_BACK, GL_EMISSION, colorEmission0 );
    glMaterialf ( GL_BACK, GL_SHININESS, shininess );
    glMaterialfv( GL_BACK, GL_COLOR_INDEXES, colorIndexes0 );
  }

  bool needColor = renderMode == ovm_PrimitiveRender  || // Цветные треугольники и квадраты (полосы разбираются)
                   renderMode == ovm_StripePrimRender || // Цветные полосы треугольники и квадраты (+ одиночные примитивы)
                   renderMode == ovm_StripesRender;      // Только полосы - без одиночных примитивов

  bool needPrims = renderMode == ovm_NormalRender     || // Нормальный - "Классический полутон"
                   renderMode == ovm_NormalRenderWithEdges || // Нормальный - "Классический полутон" с ребрами
                   renderMode == ovm_PrimitiveRender  || // Цветные треугольники и квадраты (полосы разбираются)
                   renderMode == ovm_StripePrimRender;   // Цветные полосы треугольники и квадраты (+ одиночные примитивы)

  if ( needColor ) {
    glDisable( GL_LIGHTING );
    glDisable( GL_POLYGON_SMOOTH ); // Запрещаем смешивание цветов (Непрозрачность)

    // Козулин Ю.А. 2017-03-20 Для отладки триангуляции нужна большая стабильность раздачи цветов.
    // ::srand( (uint)time( nullptr ) );
    ::srand( (uint)0 );
  }
  
  MbCartPoint3D p0, p1, p2, p3, p4, p5, p6, p7, p9;
  MbVector3D n0, n1, n2, n3, n4, n5, n6, n7;
  size_t k, kCnt;
  bool single = from.IsSingle();
  bool invert = from.IsInvert();

  if ( needPrims ) {
    // Треугольники
    glBegin( GL_TRIANGLES );
    for ( k = 0, kCnt = gr->TrianglesCount(); k < kCnt; k++ ) {
      if ( needColor ) 
        glColor3f( (float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX );

      if ( gr->GetTrianglePoints ( k, p0, p1, p2 ) &&
           gr->GetTriangleNormals( k, n0, n1, n2 ) ) {
        if ( !single ) {
          p0.Transform( from );
          p1.Transform( from );
          p2.Transform( from );
          n0.Transform( from );
          n1.Transform( from );
          n2.Transform( from );
        }
        if ( invert ) {
          n5 = n1;
          n1 = n2;
          n2 = n5;
          p5 = p1;
          p1 = p2;
          p2 = p5;
        }
        glNormal3f( (GLfloat)n0.x, (GLfloat)n0.y, (GLfloat)n0.z );
        glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
        glNormal3f( (GLfloat)n1.x, (GLfloat)n1.y, (GLfloat)n1.z );
        glVertex3f( (GLfloat)p1.x, (GLfloat)p1.y, (GLfloat)p1.z );
        glNormal3f( (GLfloat)n2.x, (GLfloat)n2.y, (GLfloat)n2.z );
        glVertex3f( (GLfloat)p2.x, (GLfloat)p2.y, (GLfloat)p2.z );
      }
    }
    glEnd();

    // Четырёхугольники
    glBegin( GL_QUADS );
    for ( k = 0, kCnt = gr->QuadranglesCount(); k < kCnt; k++ ) {
      if ( needColor ) 
        glColor3f( (float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX );

      if ( gr->GetQuadranglePoints ( k, p0, p1, p2, p3 ) &&
           gr->GetQuadrangleNormals( k, n0, n1, n2, n3 ) ) {
        if ( !single ) {
          p0.Transform( from );
          p1.Transform( from );
          p2.Transform( from );
          p3.Transform( from );
          n0.Transform( from );
          n1.Transform( from );
          n2.Transform( from );
          n3.Transform( from );
        }
        if ( invert ) {
          n5 = n1;
          n1 = n3;
          n3 = n5;
          p5 = p1;
          p1 = p3;
          p3 = p5;
        }
        glNormal3f( (GLfloat)n0.x, (GLfloat)n0.y, (GLfloat)n0.z );
        glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
        glNormal3f( (GLfloat)n1.x, (GLfloat)n1.y, (GLfloat)n1.z );
        glVertex3f( (GLfloat)p1.x, (GLfloat)p1.y, (GLfloat)p1.z );
        glNormal3f( (GLfloat)n2.x, (GLfloat)n2.y, (GLfloat)n2.z );
        glVertex3f( (GLfloat)p2.x, (GLfloat)p2.y, (GLfloat)p2.z );
        glNormal3f( (GLfloat)n3.x, (GLfloat)n3.y, (GLfloat)n3.z );
        glVertex3f( (GLfloat)p3.x, (GLfloat)p3.y, (GLfloat)p3.z );
      }
    }
    /*// Код ниже временный и будет удален, так как для элементов будем строить отрисовочный MbMesh из четырехугольных пластин для наружних стенок в методе CreateMesh() и др. 
    for ( k = 0, kCnt = gr->ElementsCount(); k < kCnt; k++ ) {
      if ( needColor ) 
        glColor3f( (float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX );

      if ( gr->GetElementPoints ( k, p0, p1, p2, p3, p4, p5, p6, p7 ) ) {
        n6.Init( p0, p1 ); n7.Init( p0, p4 ); n0.SetVecM( n6, n7 ); n0.Normalize();
        n6.Init( p1, p2 ); n7.Init( p1, p5 ); n1.SetVecM( n6, n7 ); n1.Normalize();
        n6.Init( p2, p3 ); n7.Init( p2, p6 ); n2.SetVecM( n6, n7 ); n2.Normalize();
        n6.Init( p3, p0 ); n7.Init( p3, p7 ); n3.SetVecM( n6, n7 ); n3.Normalize();
        n6.Init( p0, p3 ); n7.Init( p0, p1 ); n4.SetVecM( n6, n7 ); n4.Normalize();
        n6.Init( p4, p5 ); n7.Init( p4, p7 ); n5.SetVecM( n6, n7 ); n5.Normalize();

        if ( !single ) {
          p0.Transform( from );
          p1.Transform( from );
          p2.Transform( from );
          p3.Transform( from );
          p4.Transform( from );
          p5.Transform( from );
          p6.Transform( from );
          p7.Transform( from );
          n0.Transform( from );
          n1.Transform( from );
          n2.Transform( from );
          n3.Transform( from );
          n4.Transform( from );
          n5.Transform( from );
        }
        if ( invert ) {
          p9 = p1;
          p1 = p3;
          p3 = p9;
          p9 = p5;
          p5 = p6;
          p6 = p9;
        }
        glNormal3f( (GLfloat)n0.x, (GLfloat)n0.y, (GLfloat)n0.z );
        glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
        glNormal3f( (GLfloat)n0.x, (GLfloat)n0.y, (GLfloat)n0.z );
        glVertex3f( (GLfloat)p1.x, (GLfloat)p1.y, (GLfloat)p1.z );
        glNormal3f( (GLfloat)n0.x, (GLfloat)n0.y, (GLfloat)n0.z );
        glVertex3f( (GLfloat)p5.x, (GLfloat)p5.y, (GLfloat)p5.z );
        glNormal3f( (GLfloat)n0.x, (GLfloat)n0.y, (GLfloat)n0.z );
        glVertex3f( (GLfloat)p4.x, (GLfloat)p4.y, (GLfloat)p4.z );

        glNormal3f( (GLfloat)n1.x, (GLfloat)n1.y, (GLfloat)n1.z );
        glVertex3f( (GLfloat)p1.x, (GLfloat)p1.y, (GLfloat)p1.z );
        glNormal3f( (GLfloat)n1.x, (GLfloat)n1.y, (GLfloat)n1.z );
        glVertex3f( (GLfloat)p2.x, (GLfloat)p2.y, (GLfloat)p2.z );
        glNormal3f( (GLfloat)n1.x, (GLfloat)n1.y, (GLfloat)n1.z );
        glVertex3f( (GLfloat)p6.x, (GLfloat)p6.y, (GLfloat)p6.z );
        glNormal3f( (GLfloat)n1.x, (GLfloat)n1.y, (GLfloat)n1.z );
        glVertex3f( (GLfloat)p5.x, (GLfloat)p5.y, (GLfloat)p5.z );

        glNormal3f( (GLfloat)n2.x, (GLfloat)n2.y, (GLfloat)n2.z );
        glVertex3f( (GLfloat)p2.x, (GLfloat)p2.y, (GLfloat)p2.z );
        glNormal3f( (GLfloat)n2.x, (GLfloat)n2.y, (GLfloat)n2.z );
        glVertex3f( (GLfloat)p3.x, (GLfloat)p3.y, (GLfloat)p3.z );
        glNormal3f( (GLfloat)n2.x, (GLfloat)n2.y, (GLfloat)n2.z );
        glVertex3f( (GLfloat)p7.x, (GLfloat)p7.y, (GLfloat)p7.z );
        glNormal3f( (GLfloat)n2.x, (GLfloat)n2.y, (GLfloat)n2.z );
        glVertex3f( (GLfloat)p6.x, (GLfloat)p6.y, (GLfloat)p6.z );

        glNormal3f( (GLfloat)n3.x, (GLfloat)n3.y, (GLfloat)n3.z );
        glVertex3f( (GLfloat)p3.x, (GLfloat)p3.y, (GLfloat)p3.z );
        glNormal3f( (GLfloat)n3.x, (GLfloat)n3.y, (GLfloat)n3.z );
        glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
        glNormal3f( (GLfloat)n3.x, (GLfloat)n3.y, (GLfloat)n3.z );
        glVertex3f( (GLfloat)p4.x, (GLfloat)p4.y, (GLfloat)p4.z );
        glNormal3f( (GLfloat)n3.x, (GLfloat)n3.y, (GLfloat)n3.z );
        glVertex3f( (GLfloat)p7.x, (GLfloat)p7.y, (GLfloat)p7.z );

        glNormal3f( (GLfloat)n4.x, (GLfloat)n4.y, (GLfloat)n4.z );
        glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
        glNormal3f( (GLfloat)n4.x, (GLfloat)n4.y, (GLfloat)n4.z );
        glVertex3f( (GLfloat)p3.x, (GLfloat)p3.y, (GLfloat)p3.z );
        glNormal3f( (GLfloat)n4.x, (GLfloat)n4.y, (GLfloat)n4.z );
        glVertex3f( (GLfloat)p2.x, (GLfloat)p2.y, (GLfloat)p2.z );
        glNormal3f( (GLfloat)n4.x, (GLfloat)n4.y, (GLfloat)n4.z );
        glVertex3f( (GLfloat)p4.x, (GLfloat)p4.y, (GLfloat)p4.z );

        glNormal3f( (GLfloat)n5.x, (GLfloat)n5.y, (GLfloat)n5.z );
        glVertex3f( (GLfloat)p4.x, (GLfloat)p4.y, (GLfloat)p4.z );
        glNormal3f( (GLfloat)n5.x, (GLfloat)n5.y, (GLfloat)n5.z );
        glVertex3f( (GLfloat)p5.x, (GLfloat)p5.y, (GLfloat)p5.z );
        glNormal3f( (GLfloat)n5.x, (GLfloat)n5.y, (GLfloat)n5.z );
        glVertex3f( (GLfloat)p6.x, (GLfloat)p6.y, (GLfloat)p6.z );
        glNormal3f( (GLfloat)n5.x, (GLfloat)n5.y, (GLfloat)n5.z );
        glVertex3f( (GLfloat)p7.x, (GLfloat)p7.y, (GLfloat)p7.z );
      }
    }
    */// Конец временного кода, который будет удален.
    glEnd();
  }
  if ( visual_bit == true )
    glPopAttrib();
}


//------------------------------------------------------------------------------
// Нарисовать каркасный объект.
// ---
static
void RenderWireFrameGL( const MbMesh * mesh, bool ownColor, const MbMatrix3D & from, bool useMatrix,
                        GLfloat R, GLfloat G, GLfloat B, bool edgesOnly )
{
  if ( (mesh != nullptr) && (mesh->PolygonsCount() > 0) ) {
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable( GL_LIGHTING );
    glDisable( GL_BLEND ); // Запрещаем смешивание цветов

    GLfloat ownR = R;
    GLfloat ownG = G;
    GLfloat ownB = B;
    glColor3f( ownR, ownG, ownB );

    size_t count = mesh->PolygonsCount();
    MbCartPoint3D p;

    int meshWidth = mesh->GetWidth();
    meshWidth = std_max( meshWidth, 1 );
    glLineWidth((GLfloat)meshWidth);

    for ( size_t k = 0; k < count; k++ ) {
      const MbPolygon3D * polygon = mesh->GetPolygon(k);
      if ( polygon == nullptr )
        continue;
      if ( edgesOnly ) { // ребра
        const MbTopItem * item = polygon->TopItem();
        if ( (item == nullptr) || (item->IsA() != tt_CurveEdge) )
          continue;
      }
      if ( !polygon->IsVisible() && edgesOnly )
        continue;

      if ( ownColor ) {
        const MbColor * colorAttr= static_cast<const MbColor *>(polygon->GetSimpleAttribute(at_Color));
        if ( colorAttr != nullptr ) {
          ownR = (GLfloat)( GetRValue( colorAttr->Color() ) / 255.0 );
          ownG = (GLfloat)( GetGValue( colorAttr->Color() ) / 255.0 );
          ownB = (GLfloat)( GetBValue( colorAttr->Color() ) / 255.0 );
        }
        glColor3f( ownR, ownG, ownB );
      }
      
      int edgeWidth = polygon->GetWidth();
      if ( edgeWidth != 0 ) { // Если индивидуальная толщина установлена
        edgeWidth = std_max( edgeWidth, 1 );
        glLineWidth( (GLfloat)edgeWidth );
      }
      else {
        glLineWidth( (GLfloat)meshWidth );
      }
      
      glBegin( GL_LINE_STRIP );
      size_t pointsCnt = polygon->Count();
      for ( size_t n = 0; n < pointsCnt; n++ ) {
        polygon->GetPoint( n, p );
        if ( useMatrix ) 
          p.Transform( from );
        glVertex3f ((GLfloat)p.x, (GLfloat)p.y, (GLfloat)p.z);
      }
      glEnd();

      glLineWidth((GLfloat)mesh->GetWidth());

      ownR = R;
      ownG = G;
      ownB = B;
    }

    glLineWidth(1);
    glPopAttrib();
  } // Кривая
}


//------------------------------------------------------------------------------
// Нарисовать точечный объект.
// ---
static
void RenderPointFrameGL( const MbMesh * mesh, bool ownColor, const MbMatrix3D & from, bool useMatrix,
                         GLfloat R, GLfloat G, GLfloat B, const bool bDrawDebug = false )
{
  if ( (mesh != nullptr) && (mesh->ApexesCount() > 0) ) {
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable( GL_LIGHTING );
    glDisable( GL_BLEND ); // Запрещаем смешивание цветов

    GLfloat ownR = R;
    GLfloat ownG = G;
    GLfloat ownB = B;
    glColor3f( ownR, ownG, ownB );
    int width = mesh->GetWidth();
    width = std_max( width, 1 );

    size_t count = mesh->ApexesCount();
    MbCartPoint3D p;

    bool isWCS = ((mesh->GetMeshType() == st_AssistedItem) && (mesh->GetRefItem() == nullptr)); // ГСК

    int pointSize = 1;
    if ( !isWCS ) {
      pointSize = mesh->GetWidth();
      pointSize = std_max( pointSize, (int)5 );
    }
    glPointSize( (GLfloat)pointSize );
    //glEnable( GL_POINT_SMOOTH ); // Иначе вылет под дебагом при запуске на некоторых видеокартах AMD.

    for ( size_t k = 0; k < count; k++ ) {
      const MbApex3D * apex = mesh->GetApex(k);
      if ( apex == nullptr )
        continue;
      if ( !apex->IsVisible() )
        continue;

      int p_width = apex->GetWidth();
#ifdef DEBUG_DRAW_MESH_POINTS
      if ( bDrawDebug )
        p_width = DEBUG_DRAW_MESH_POINTS_WIDTH;
#endif // DEBUG_DRAW_MESH_POINTS

      p_width = std_max( p_width, 1 );
      if ( width != p_width )
        glLineWidth((GLfloat)p_width);

      if ( ownColor ) {
        const MbColor * colorAttr= static_cast<const MbColor *>(apex->GetSimpleAttribute(at_Color));
        if ( colorAttr != nullptr )
        {
          ownR = (GLfloat) ( GetRValue( colorAttr->Color() ) / 255.0 );
          ownG = (GLfloat) ( GetGValue( colorAttr->Color() ) / 255.0 );
          ownB = (GLfloat) ( GetBValue( colorAttr->Color() ) / 255.0 );
        }
        else if ( bDrawDebug )
          continue; // рисовать только те вершины, для которых задан атрибут

        glColor3f( ownR, ownG, ownB );
      }
      glBegin( GL_POINTS );
      apex->GetPoint( p );
      if ( useMatrix ) 
        p.Transform( from );
      glVertex3f ((GLfloat)p.x, (GLfloat)p.y, (GLfloat)p.z);
      glEnd();
    }

    //glDisable( GL_POINT_SMOOTH ); // Иначе вылет под дебагом при запуске на некоторых видеокартах AMD.
    //glPointSize(1);
    glPopAttrib();
  } // Точка
}


//------------------------------------------------------------------------------
// Нарисовать объект.
// ---
void IGrDraw::RenderMeshGL( const MbMesh * mesh, const MbMatrix3D & from, const MbVisual * hostVisual,
                            const MbColor * hostColor, COLORREF select, bool useSelect ) const
{
  if ( mesh != nullptr ) {
    size_t k, count;
    bool single = from.IsSingle();
    //bool useSelect = mesh->IsSelected();

#ifdef __USE_GL_LISTS
    GLuint listName = (GLuint)name;
#endif // __USE_GL_LISTS

    COLORREF color = (COLORREF)MB_C3DCOLOR; // Цвет элемента по умолчанию.
    const MbColor * colorAttr = static_cast<const MbColor *>(mesh->GetSimpleAttribute(at_Color));
    if ( colorAttr != nullptr )
      color = colorAttr->Color(); // Цвет элемента.
    else {
      if ( hostColor != nullptr )
        color = hostColor->Color(); // Цвет родительского элемента.
    }

    if ( useSelect )
      color = select; // Цвет селектирования.

    GLfloat R = (GLfloat)( GetRValue( color ) / 255.0 );
    GLfloat G = (GLfloat)( GetGValue( color ) / 255.0 );
    GLfloat B = (GLfloat)( GetBValue( color ) / 255.0 );

    GLfloat RW = (GLfloat)1.;
    GLfloat GW = (GLfloat)1.;
    GLfloat BW = (GLfloat)1.;

#ifdef __USE_GL_LISTS
    glNewList( listName, GL_COMPILE ); //_AND_EXECUTE );
#endif // __USE_GL_LISTS

    switch ( renderMode ) 
    {
      case ovm_NormalRender     :      // Нормальный - "Классический полутон"
      case ovm_NormalRenderWithEdges : // Нормальный - "Классический полутон" с ребрами
      case ovm_PrimitiveRender  :      // Цветные треугольники и квадраты (полосы разбираются)
      case ovm_StripePrimRender :      // Цветные полосы треугольники и квадраты (+ одиночные примитивы)
      case ovm_StripesRender    :      // Только полосы - без одиночных примитивов
      {
        if ( (mesh->GridsCount() > 0) || // Есть триангуляция.
             (mesh->GetMeshType() == st_Surface) || // Тело или поверхность
             (mesh->GetMeshType() == st_Solid) ) {
          float ambient     = (float)MB_AMBIENT;     // Коэффициент общего фона.
          float diffuse     = (float)MB_DIFFUSE;     // Коэффициент диффузного отражения.
          float specularity = (float)MB_SPECULARITY; // Коэффициент зеркального отражения.
          float shininess   = (float)MB_SHININESS;   // Блеск (показатель степени в законе зеркального отражения) - сфокусированность зеркального блика.
          float opacity     = (float)MB_OPACITY;     // Коэффициент непрозрачности.
          float emission    = (float)MB_EMISSION;    // Коэффициент излучения.

          const MbVisual * visualAttr = static_cast<const MbVisual *>( mesh->GetSimpleAttribute(at_Visual) );
          if ( visualAttr != nullptr )
            visualAttr->Get( ambient, diffuse, specularity, shininess, opacity, emission );
          else {
            if ( hostVisual != nullptr )
              hostVisual->Get( ambient, diffuse, specularity, shininess, opacity, emission );
          }

          glEnable( GL_LIGHTING );
          if ( opacity < (1.0 - DELTA_MIN) ) { // Прозрачность
            glEnable ( GL_BLEND ); // Разрешаем смешивание цветов (прозрачность)
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
          }
          else
            glDisable( GL_BLEND ); // Запрещаем смешивание цветов

          if ( mesh->IsClosed() && (opacity > DELTA_MIN) ) // Тело
            glEnable ( GL_CULL_FACE ); // Односторонняя закраска (изнанка цвета фона)
          else
            glDisable( GL_CULL_FACE ); // Двусторонняя закраска

          glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//          glPolygonMode( GL_BACK,  GL_LINE );
          glDisable(GL_POLYGON_OFFSET_FILL);                 

          GLfloat colorAmbient[]  = { R * ambient,     G * ambient,     B * ambient,     opacity };
          GLfloat colorDiffuse[]  = { R * diffuse,     G * diffuse,     B * diffuse,     opacity };
          GLfloat colorSpecular[] = { R * specularity, G * specularity, B * specularity, opacity };
          GLfloat colorEmission[] = { R * emission,    G * emission,    B * emission,    opacity };
          GLfloat colorIndexes[]  = { ambient, diffuse, opacity }; // Индексы фонового, рассеянного и отражённого цветов.

          if ( ((mesh->GetMeshType() == st_Solid) && !mesh->IsClosed()) || // Оболочка
                (mesh->GetMeshType() == st_Surface) || // Поверхность
                (opacity < DELTA_MIN) ) { // Полная прозрачность
            glMaterialfv( GL_FRONT, GL_AMBIENT,   colorAmbient  );
            glMaterialfv( GL_FRONT, GL_DIFFUSE,   colorDiffuse  );
            glMaterialfv( GL_FRONT, GL_SPECULAR,  colorSpecular );
            glMaterialfv( GL_FRONT, GL_EMISSION,  colorEmission );
            glMaterialf ( GL_FRONT, GL_SHININESS, shininess     );
            glMaterialfv( GL_FRONT, GL_COLOR_INDEXES, colorIndexes );
            R = (GLfloat)( (R+1.0)/2.0f ); // Смесь с серым - цвет изнанки незамкнутого тела
            G = (GLfloat)( (G+1.0)/2.0f ); // Смесь с серым - цвет изнанки незамкнутого тела
            B = (GLfloat)( (B+1.0)/2.0f ); // Смесь с серым - цвет изнанки незамкнутого тела
            float opacity0 = opacity; // Непрозрачность
            if ( opacity0 < DELTA_MIN ) // Полная прозрачность
              opacity0 = (float)DELTA_MOD;
            GLfloat colorAmbient0[]  = { R * ambient,     G * ambient,     B * ambient,     opacity0 };
            GLfloat colorDiffuse0[]  = { R * diffuse,     G * diffuse,     B * diffuse,     opacity0 };
            GLfloat colorSpecular0[] = { R * specularity, G * specularity, B * specularity, opacity0 };
            GLfloat colorEmission0[] = { R * emission,    G * emission,    B * emission,    opacity0 };
            GLfloat colorIndexes0[]  = { ambient, diffuse, opacity0 }; // Индексы фонового, рассеянного и отражённого цветов.
            glMaterialfv( GL_BACK, GL_AMBIENT,   colorAmbient0  );
            glMaterialfv( GL_BACK, GL_DIFFUSE,   colorDiffuse0  );
            glMaterialfv( GL_BACK, GL_SPECULAR,  colorSpecular0 );
            glMaterialfv( GL_BACK, GL_EMISSION,  colorEmission0 );
            glMaterialf ( GL_BACK, GL_SHININESS, shininess      );
            glMaterialfv( GL_BACK, GL_COLOR_INDEXES, colorIndexes0 );
          }
          else {
            glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   colorAmbient  );
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   colorDiffuse  );
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  colorSpecular );
            glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  colorEmission );
            glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, shininess     );
            glMaterialfv( GL_FRONT_AND_BACK, GL_COLOR_INDEXES, colorIndexes );
          }
/*
          GLfloat ambientLight[]  = { ambient, ambient, ambient, 1.0 }; // Свет рассеяный.
          GLfloat diffuseLight[]  = { diffuse, diffuse, diffuse, 1.0 }; // Свет диффузный.
          GLfloat lightPosition[] = { 0.0, 0.0, 100.0, 1.0 }; // Свет диффузный.

          glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
          glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );
          glEnable( GL_LIGHT0 ); // Разрешаем освещение.
          glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
*/
//          glMaterialf ( GL_FRONT_AND_BACK, GL_OPACITY, opacity );

          // Отрисовка граней

          glPushAttrib( GL_ALL_ATTRIB_BITS );

          if ( renderMode == ovm_NormalRenderWithEdges ) {
            glEnable( GL_POLYGON_OFFSET_FILL );
            glPolygonOffset( 0.5f, 1.0f );
          }

          for ( size_t i = 0, iCount = mesh->GridsCount(); i < iCount; i++ ) {
            const MbGrid * grid = mesh->GetGrid( i );
            if ( grid != nullptr ) {
              if ( !grid->IsVisible() )
                continue;

              ::RenderGridGL( grid, from, color, select, useSelect, renderMode, 
                              ambient, diffuse, specularity, 
                              shininess, opacity, emission );
            }
          }

          if ( renderMode == ovm_NormalRenderWithEdges ) { // Отрисовка рёбер
            glDisable( GL_POLYGON_OFFSET_FILL );
            ::RenderWireFrameGL( mesh, !useSelect, from, !single, 0, 0, 0, true ); // Кривые
          }

          glPopAttrib();
        } // Тело или поверхность

        if ( (mesh->GetMeshType() == st_Undefined) || 
             (mesh->GetMeshType() == st_Assembly) || 
             (mesh->GetMeshType() == st_Curve3D) || 
             (mesh->GetMeshType() == st_AssistedItem) ) { // Каркас, ЛСК, вспомогательные объекты
          ::RenderWireFrameGL ( mesh, !useSelect, from, !single, R, G, B, false ); // Кривые
        }
        if ( (mesh->GetMeshType() == st_Undefined) || 
             (mesh->GetMeshType() == st_Assembly) || 
             (mesh->GetMeshType() == st_Point3D) || 
             (mesh->GetMeshType() == st_AssistedItem) ) { // Точки, ЛСК, вспомогательные объекты
          ::RenderPointFrameGL( mesh, !useSelect, from, !single, R, G, B ); // Точки
        }
        else
        {
#ifdef DEBUG_DRAW_MESH_POINTS
          ::RenderPointFrameGL( mesh, !useSelect, from, !single, R, G, B, true ); // Точки
#endif // DEBUG_DRAW_MESH_POINTS

        }
      }
      break;

      case ovm_IsoparamMesh      :    // Поверхности - изопараметрическими линиями
      case ovm_TriangleMesh      :    // Поверхности - триангуляционными решетками (примитивами - квадраты+треугольники)
      case ovm_IsoparTriMesh     :    // Поверхности - изопараметрическими линиями + триангуляция (примитивами)
      case ovm_ParamTriangleMesh :    // Поверхность - триангуляция в параметрической области
      {
        if ( (mesh->GetMeshType() == st_Surface) || // Тело или поверхность
             (mesh->GetMeshType() == st_Solid) ) {
          // Нужны изопараметрические линии
          bool needIsoparLines = renderMode == ovm_IsoparamMesh   || // Поверхности - изопараметрическими линиями
                                 renderMode == ovm_IsoparTriMesh;    // Поверхности - изопараметрическими линиями + триангуляция (примитивами)

          // Нужна триангуляция
          bool needTriangulation = renderMode == ovm_TriangleMesh   || // Поверхности - триангуляционными решетками (примитивами - квадраты+треугольники)
                                   renderMode == ovm_IsoparTriMesh;    // Поверхности - изопараметрическими линиями + триангуляция (примитивами)

          // Нужна триангуляция в параметрической области
          bool needParamTriangles = renderMode == ovm_ParamTriangleMesh;    // Поверхность - триангуляция в параметрической области

          glPolygonMode( GL_FRONT_AND_BACK, /*GL_LINE*/ GL_FILL );

          glDisable( GL_LIGHTING );
          glDisable( GL_BLEND ); // Запрещаем смешивание цветов
          glColor3f( R, G, B );

          if ( needIsoparLines ) {
            ::RenderWireFrameGL( mesh, !useSelect, from, !single, R, G, B, false ); // Кривые
          }

          if ( needTriangulation || needParamTriangles ) {
            for ( int iTr = 0; iTr < 2; iTr++ ) {
              if ( iTr ) {
                glEnable( GL_BLEND ); // Разрешаем смешивание цветов
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
                glColor3f( RW, GW, BW );
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset( (GLfloat)1., (GLfloat)1. );
              }
              else {
                glDisable( GL_BLEND ); // Запрещаем смешивание цветов
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                glColor3f( R, G, B );
                glDisable( GL_POLYGON_OFFSET_FILL );                 
              }

              MbFloatPoint3D p0, p1, p2, p3;
              MbFloatVector3D n0, n1, n2, n3;

              if ( needTriangulation ) {
                for ( size_t i = 0, iCount = mesh->GridsCount(); i < iCount; i++ ) {
                  const MbGrid * gr = mesh->GetGrid( i );
                  if ( gr != nullptr ) {
                    count = gr->TrianglesCount();
                    glBegin( GL_TRIANGLES );
                    for ( k = 0; k < count; k++ ) {
                      if ( gr->GetTrianglePoints( k, p0, p1, p2 ) ) {
                        if ( !single ) {
                          p0.Transform( from );
                          p1.Transform( from );
                          p2.Transform( from );
                        }
                        glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
                        glVertex3f( (GLfloat)p1.x, (GLfloat)p1.y, (GLfloat)p1.z );
                        glVertex3f( (GLfloat)p2.x, (GLfloat)p2.y, (GLfloat)p2.z );
                      }
                    }
                    glEnd();

                    count = gr->QuadranglesCount();
                    glBegin( GL_QUADS );
                    for ( k = 0; k < count; k++ ) {
                      if ( gr->GetQuadranglePoints(k, p0, p1, p2, p3) ) {
                        if ( !single ) {
                          p0.Transform( from );
                          p1.Transform( from );
                          p2.Transform( from );
                          p3.Transform( from );
                        }
                        glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
                        glVertex3f( (GLfloat)p1.x, (GLfloat)p1.y, (GLfloat)p1.z );
                        glVertex3f( (GLfloat)p2.x, (GLfloat)p2.y, (GLfloat)p2.z );
                        glVertex3f( (GLfloat)p3.x, (GLfloat)p3.y, (GLfloat)p3.z );
                      }
                    }
                    glEnd();
                  }
                }
              }

              // !!! отображаем нормали
              for ( size_t i = 0, iCnt = mesh->GridsCount(); i < iCnt; i++ ) {
                const MbGrid * gr = mesh->GetGrid( i );
                if ( gr != nullptr ) {
                  count = gr->PointsCount();
                  glBegin( GL_LINES );
                  for ( k = 0; k < count; k++ ) {
                    gr->GetPoint ( k, p0 );
                    gr->GetNormal( k, n1);
                    n1.x += p0.x;
                    n1.y += p0.y;
                    n1.z += p0.z;
                    if ( !single ) {
                      p0.Transform( from );
                      n1.Transform( from );
                    }
                    glVertex3f( (GLfloat)p0.x, (GLfloat)p0.y, (GLfloat)p0.z );
                    glVertex3f( (GLfloat)n1.x, (GLfloat)n1.y, (GLfloat)n1.z );
                  }
                  glEnd();
                }
              }
            }
          }
        } // Тело или поверхность

        if ( (mesh->GetMeshType() == st_Undefined) || 
             (mesh->GetMeshType() == st_Assembly) || 
             (mesh->GetMeshType() == st_Curve3D) || 
             (mesh->GetMeshType() == st_AssistedItem) ) { // Каркас, ЛСК, вспомогательные объекты
          ::RenderWireFrameGL ( mesh, !useSelect, from, !single, R, G, B, false ); // Кривые
        }
        if ( (mesh->GetMeshType() == st_Undefined) || 
             (mesh->GetMeshType() == st_Assembly) || 
             (mesh->GetMeshType() == st_Point3D) || 
             (mesh->GetMeshType() == st_AssistedItem) ) { // Точки, ЛСК, вспомогательные объекты
          ::RenderPointFrameGL( mesh, !useSelect, from, !single, R, G, B ); // Точки
        }
      }
      break;

      default: break;
    }
#ifdef __USE_GL_LISTS
    glEndList();
#endif // __USE_GL_LISTS
  }
}


//----------------------------------------------------------------------------------------
// Нарисовать объект.
// ---
void IGrDraw::RenderItemGL( const MbItem * obj, const MbMatrix3D & matr, const MbVisual * hostVisual
                          , const MbColor * hostColor, COLORREF select, bool useSelect ) const
{
  if ( obj != nullptr && obj->IsVisible() )
  {
    MbMatrix3D from( matr );
    WorkViewSort viewSort = parent.GetViewSort();
    const MbVisual * visualAttr = static_cast<const MbVisual *>(obj->GetSimpleAttribute(at_Visual));
    if ( visualAttr != nullptr )
      hostVisual = visualAttr;
    const MbColor * colorAttr = static_cast<const MbColor *>(obj->GetSimpleAttribute(at_Color));
    if ( colorAttr != nullptr )
      hostColor = colorAttr;

    switch ( obj->IsA() ) 
    {
      case st_Mesh : {
        RenderMeshGL( static_cast<const MbMesh *>(obj), from, hostVisual, hostColor, select, useSelect ); // Полигонный плоскогранный объект
      } break;

      case st_Instance : 
      {          
        if ( const MbInstance * inst = static_cast<const MbInstance*>( obj ) ) 
        {
          const MbMatrix3D _from( matr, inst->GetPlacement().GetMatrixFrom() );
          RenderItemGL( inst->GetItem(), _from, hostVisual, hostColor, select, useSelect || inst->IsSelected() );
        }
      } break;

      case st_Assembly : {
        const MbAssembly * asse = static_cast<const MbAssembly*>( obj );
        if ( asse != nullptr ) {
          if ( viewSort == wvs_All || viewSort == wvs_Geometry ) { // Drawing the geometric items.
            for ( size_t i = 0, iCount = asse->ItemsCount(); i < iCount; i++ ) {
              if ( const MbItem * asseItem = asse->GetItem( i ) ) {
                RenderItemGL( asseItem, matr, hostVisual, hostColor, select, useSelect || asseItem->IsSelected() );
              }
            }
          }
          if ( viewSort == wvs_All || viewSort == wvs_Constraints ) { // Drawing the geometric constraints.
            std::vector<const MbMesh *> meshes;
            asse->GetConstraintMesh( meshes ); // Get all polygonal objects for drawing the geometric constraints.
            for ( size_t i = 0, iCount = meshes.size(); i < iCount; i++ ) {
              RenderMeshGL( meshes[i], from, hostVisual, hostColor, select, useSelect );
            }
          }
        }
      } break;

      default:break;
    }

  }
}


//------------------------------------------------------------------------------
// Перерисовать
// ---
void IGrDraw::ResetGL() 
{}


//------------------------------------------------------------------------------
// Визуальные свойства
// ---
void IGrDraw::SetGLProperty( const MbModel & drawModel, double la, double ld, double ls, MbVector3D &ll ) 
{
  lightAmbient  = (float)la;
  lightDiffuse  = (float)ld;
  lightSpecular = (float)ls;
  lightLocal = ll;
  lightSide  = ll;

  {
    MbModel::ItemConstIterator drawIter( drawModel.CBegin() );
    MbModel::ItemConstIterator drawEnd ( drawModel.CEnd() );
    for ( ; drawIter != drawEnd; ++drawIter ) {
      const MbItem * obj = *drawIter;
    //for ( size_t i = 0, iCount = drawModel.ItemsCount(); i < iCount; i++ ) {
    //  const MbItem * obj = drawModel.GetModelItem( i ); // Выдать объект по индексу
      GLuint listName = (GLuint)(uint)(size_t)obj;
      if ( glIsList(listName) )
        glDeleteLists( listName, 1 );
    }

    GLfloat glfLightAmbient[]  = { lightAmbient, lightAmbient, lightAmbient, 1.0f };
    GLfloat glfLightDiffuse[]  = { lightDiffuse, lightDiffuse, lightDiffuse, 1.0f };
    GLfloat glfLightSpecular[] = { lightSpecular, lightSpecular, lightSpecular, 1.0f };
    glLightfv (GL_LIGHT0, GL_AMBIENT,  glfLightAmbient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE,  glfLightDiffuse);
    glLightfv (GL_LIGHT0, GL_SPECULAR, glfLightSpecular);

    GLfloat glfLightLocal[] = { (GLfloat)lightLocal.x, (GLfloat)lightLocal.y, (GLfloat)lightLocal.z, 1.0f };
    GLfloat glfLightSide[]  = { (GLfloat)lightSide.x,  (GLfloat)lightSide.y,  (GLfloat)lightSide.z, 1.0f };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, glfLightAmbient );
    glLightModelfv( GL_LIGHT_MODEL_LOCAL_VIEWER, glfLightLocal );
    glLightModelfv( GL_LIGHT_MODEL_TWO_SIDE, glfLightSide );
  }
}


//------------------------------------------------------------------------------
// Визуальные свойства
// ---
void IGrDraw::GetGLProperty( double &la, double &ld, double &ls, MbVector3D &ll ) {
  la = lightAmbient;
  ld = lightDiffuse;
  ls = lightSpecular;
  ll = lightSide;
}


//------------------------------------------------------------------------------
// Выдать мировую точку по экранной
// ---
/* Функция не работает, матрицы отсутствуют
MbCartPoint3D IGrDraw::GetGLCartPoint( POINT mouseCoord ) {
  GLdouble modelMatrix[16];
  GLdouble projMatrix[16];
  GLint viewPort[4];
  GLdouble wx = mouseCoord.x;
  GLdouble wy = mouseCoord.y;
  GLdouble wz = -parent.GetVista().z;

  glMatrixMode( GL_MODELVIEW );
  glGetDoublev( GL_MODELVIEW_MATRIX, modelMatrix ); // Модельная матрица
  glMatrixMode( GL_PROJECTION );
  glGetDoublev( GL_PROJECTION_MATRIX, projMatrix ); // Проекционная матрица
  glGetIntegerv( GL_VIEWPORT, viewPort ); // Экранная матрица

  MbCartPoint3D pnt;
  // Преобразование оконной точки в мировую
  gluUnProject( wx, wy, wz, modelMatrix, projMatrix, viewPort, &pnt.x, &pnt.y, &pnt.z );
  return pnt;
}
// */


//------------------------------------------------------------------------------
// Удалить данные
// ---
void IGrDraw::DeleteGL( const MbModel & drawModel ) 
{
  MbModel::ItemConstIterator drawIter( drawModel.CBegin() );
  MbModel::ItemConstIterator drawEnd ( drawModel.CEnd() );
  for ( ; drawIter != drawEnd; ++drawIter ) {
    const MbItem * obj = *drawIter;
  //for ( size_t i = 0, iCount = drawModel.ItemsCount(); i < iCount; i++ ) {
  //  const MbItem * obj = drawModel.GetModelItem( i ); // Выдать объект по индексу
    GLuint listName = (GLuint)(uint)LoUint32((size_t)obj);
    if ( glIsList(listName) )
      glDeleteLists( listName, 1 );
  }
  RefreshOGL();
}

