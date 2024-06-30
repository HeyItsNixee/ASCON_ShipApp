#ifndef __TEST_MAIN_DIALOGS_H
#define __TEST_MAIN_DIALOGS_H


//#include <QWidget>
//#include <QGLWidget>
#include <QDialog>
//#include <QInputDialog>


QT_BEGIN_NAMESPACE
class QSpinBox;
class QDoubleSpinBox;
QT_END_NAMESPACE


//------------------------------------------------------------------------------
// Диалог
// ---
class TimeTestResultDialog : public QDialog
{
  Q_OBJECT

public:
  TimeTestResultDialog( QWidget *parent, const void * params );
};


//------------------------------------------------------------------------------
// Диалог
// ---
class SurfaceDegreeDialog : public QDialog
{
  Q_OBJECT

protected:
  QSpinBox* edU;
  QSpinBox* edV;

public:
  SurfaceDegreeDialog( QWidget *parent );

  void SetValue( size_t u, size_t v );
  void GetValue( size_t& u, size_t& v ) const;
};


//------------------------------------------------------------------------------
// Диалог
// ---
class ExactPointDialog : public QDialog
{
  Q_OBJECT

protected:
  QDoubleSpinBox* edX;
  QDoubleSpinBox* edY;
  QDoubleSpinBox* edZ;

public:
  ExactPointDialog( QWidget *parent, bool is3D );

  void SetValue( double x, double y, double z = 0.0 );
  void GetValue( double& x, double& y ) const;
  void GetValue( double& x, double& y, double& z ) const;
};


#endif // __TEST_MAIN_DIALOGS_H
