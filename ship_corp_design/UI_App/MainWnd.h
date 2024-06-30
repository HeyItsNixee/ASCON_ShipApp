#ifndef __MAINWND_H
#define __MAINWND_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWnd.h"
#include "DrawingWdgt.h"
#include "MapWdgt.h"
#include <qwindow>
#include <qpushbutton>
#include <QScrollArea>
#include <QToolBar>
#include <QMenuBar>
#include <QActionGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidget>


class MainWnd : public QMainWindow
{
    Q_OBJECT

public:
    MainWnd(QWidget* parent = nullptr);
    ~MainWnd();

protected:
    void PreConstructSetup();
    void resizeEvent(QResizeEvent* event);
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();

private:
    Ui::MainWindow ui;
    QPushButton* treeShowBtn;
    DrawingWdgt* DrawingWidget;
    MapWdgt* MapWidget;
    QScrollArea* Scroller;
    QTreeWidget* TreeWidget;
};

#endif