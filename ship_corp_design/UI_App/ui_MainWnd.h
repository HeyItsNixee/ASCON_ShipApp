/********************************************************************************
** Form generated from reading UI file 'MainWndpMhtwk.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWNDPMHTWK_H
#define MAINWNDPMHTWK_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction* save;
    QAction* saveAs;
    QAction* open;
    QWidget* centralwidget;
    QMenuBar* menuBar;
    QMenu* fileMenu;

    void setupUi(QMainWindow* MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(900, 700);
        MainWindow->setMinimumSize(QSize(900, 700));
        MainWindow->setCursor(QCursor(Qt::ArrowCursor));
        save = new QAction(MainWindow);
        save->setObjectName(QString::fromUtf8("save"));
        saveAs = new QAction(MainWindow);
        saveAs->setObjectName(QString::fromUtf8("saveAs"));
        open = new QAction(MainWindow);
        open->setObjectName(QString::fromUtf8("open"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setBaseSize(QSize(650, 648));
        centralwidget->setCursor(QCursor(Qt::CrossCursor));
        MainWindow->setCentralWidget(centralwidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 900, 22));
        fileMenu = new QMenu(menuBar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(fileMenu->menuAction());
        fileMenu->addAction(save);
        fileMenu->addAction(saveAs);
        fileMenu->addAction(open);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow* MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\237\321\200\320\270\320\273\320\276\320\266\320\265\320\275\320\270\320\265", nullptr));
        save->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", nullptr));
        saveAs->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\272\320\260\320\272...", nullptr));
        open->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", nullptr));
        fileMenu->setTitle(QCoreApplication::translate("MainWindow", "\320\244\320\260\320\271\320\273", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow : public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWNDPMHTWK_H
