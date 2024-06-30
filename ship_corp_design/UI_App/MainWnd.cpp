#include "MainWnd.h"

const QSize defaultSize(650, 648);
const QRect defaultRect(0, 22, 650, 650);
const QRect defaultTreeRect(655, 120, 180, 475);
const QRect defaultMapSize(650, 0, 180, 140);
const int drawingWidth = 188;
const int drawingHeight = 10;
const int mapWidth = 183;
const int treeHeight = 34;
const int sizeBegin = 0;


void MainWnd::PreConstructSetup() {
    TreeWidget->setGeometry(defaultTreeRect);
    //TreeWidget->addTopLevelItem(new QTreeWidgetItem(12));
    Scroller->setWidget(DrawingWidget);
    Scroller->setStyleSheet("background-image: url(:/new/prefix1/Icons/Setka.png)");
    QActionGroup* fileGroup = new QActionGroup(this);
    fileGroup->addAction(ui.save);
    fileGroup->addAction(ui.saveAs);
    fileGroup->addAction(ui.open);

    connect(ui.save, &QAction::triggered, this, &MainWnd::onSaveFile);
    connect(ui.saveAs, &QAction::triggered, this, &MainWnd::onSaveFileAs); 
    connect(ui.open, &QAction::triggered, this, &MainWnd::onOpenFile);
}


MainWnd::MainWnd(QWidget* parent) :
    QMainWindow(parent),
    DrawingWidget(new DrawingWdgt(this, 250, 250)),
    Scroller(new QScrollArea()),
    MapWidget(new MapWdgt(this)),
    TreeWidget(new QTreeWidget(this)){
    ui.setupUi(this);
    setWindowIcon(QIcon(":/new/prefix1/Icons/icon.ico"));
    PreConstructSetup();
    MapWidget->setGeometry(defaultMapSize);
    setCentralWidget(Scroller);
    std::cout << "MainWnd.size() = " << this->size().width() << "x" << this->size().height() << std::endl;
}

void MainWnd::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    DrawingWidget->setFixedSize(this->size().width() - drawingWidth, this->size().height() - drawingHeight);
    Scroller->setFixedSize(DrawingWidget->size());
    MapWidget->move(this->size().width() - mapWidth, sizeBegin);
    TreeWidget->move(this->size().width() - mapWidth, MapWidget->size().height() + treeHeight);
}

MainWnd::~MainWnd()
{

}


void MainWnd::onSaveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), "", tr(".c3d"));

}

void MainWnd::onSaveFileAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), "", tr("C3D files (*.c3d)"));
    QFile outputFile(fileName);
    outputFile.open(QIODevice::WriteOnly);
    setWindowTitle(fileName);
}

void MainWnd::onOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Save as..."), "", tr("C3D files (*.c3d)"));
    if (fileName.isEmpty())
        return;
    else {

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }
    }
    setWindowTitle(fileName);
}

