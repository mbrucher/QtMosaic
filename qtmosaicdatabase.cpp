/**
 * \file qtmosaicdatabase.cpp
 */

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QFileSystemModel>
#include <QtGui/QSortFilterProxyModel>

#include "qtmosaicdatabase.h"
#include "QtMosaicDatabaseModel.h"

QtMosaicDatabase::QtMosaicDatabase(QWidget *parent, Qt::WFlags flags)
  : QMainWindow(parent, flags)
{
  ui.setupUi(this);

  createModels();
  
  createActions();
  createToolbar();
  createMenubar();
}

void QtMosaicDatabase::createModels()
{
  model = new QFileSystemModel(this);
  model->setRootPath(QDir::currentPath());
  filterModel = new QSortFilterProxyModel(this);
  filterModel->setSourceModel(model);

  ui.treeView->setModel(filterModel);
  ui.treeView->setSortingEnabled (true);
  ui.treeView->sortByColumn (0, Qt::AscendingOrder);
  ui.treeView->setSelectionMode(QAbstractItemView::MultiSelection);

  mosaicDatabaseModel = new QtMosaicDatabaseModel("", this);
  ui.listView->setModel(mosaicDatabaseModel);
  ui.listView->setViewMode(QListView::IconMode);
  ui.listView->setMovement(QListView::Free);
}

void QtMosaicDatabase::createActions()
{
  newAct = new QAction(this);
  connect(newAct, SIGNAL(triggered()), this, SLOT(newDatabase()));

  openAct = new QAction(QIcon(":/QtMosaic/Resources/open.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an image"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(openDatabase()));

  saveAct = new QAction(QIcon(":/QtMosaic/Resources/save.png"), tr("&Save..."), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the mosaic to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(saveDatabase()));

  connect(ui.addButton, SIGNAL(clicked()), this, SLOT(addImages()));
  connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(removeImages()));
}

void QtMosaicDatabase::createToolbar()
{
  ui.mainToolBar->addAction(openAct);
  ui.mainToolBar->addAction(saveAct);
}

void QtMosaicDatabase::createMenubar()
{
  ui.menuFile->addAction(newAct);
  ui.menuFile->addAction(openAct);
  ui.menuFile->addAction(saveAct);
  ui.menuFile->addSeparator();
}

QtMosaicDatabase::~QtMosaicDatabase()
{

}

void QtMosaicDatabase::newDatabase()
{
}

void QtMosaicDatabase::openDatabase()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Mosaic database"), QString(), QString::fromAscii("Mosaic database (*.mosaic)"));
  if (!fileName.isEmpty())
    loadDatabase(fileName);
}

void QtMosaicDatabase::loadDatabase(QString fileName)
{
}

void QtMosaicDatabase::saveDatabase()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save a Mosaic database"), QString(), QString::fromAscii("Mosaic database (*.mosaic)"));
  if (!fileName.isEmpty())
    saveDatabase(fileName);
}

void QtMosaicDatabase::saveDatabase(QString fileName)
{
  
}
