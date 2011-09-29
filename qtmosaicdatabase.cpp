/**
 * \file qtmosaicdatabase.cpp
 */

#include <QtCore/qdir.h>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QFileSystemModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QProgressDialog>

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
  ui.treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

  mosaicDatabaseModel = new QtMosaicDatabaseModel("", this);
  ui.listView->setModel(mosaicDatabaseModel);
  ui.listView->setResizeMode(QListView::Adjust);
  ui.listView->setViewMode(QListView::IconMode);
  ui.listView->setMovement(QListView::Free);
  ui.listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void QtMosaicDatabase::createActions()
{
  newAct = new QAction(QIcon(":/QtMosaic/Resources/new.png"), tr("&New..."), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("New mosaic database"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newDatabase()));

  openAct = new QAction(QIcon(":/QtMosaic/Resources/open.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open a mosaic database"));
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
  ui.mainToolBar->addAction(newAct);
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
  mosaicDatabaseModel->reset();
}

void QtMosaicDatabase::openDatabase()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Mosaic database"), QString(), QString::fromAscii("Mosaic database (*.mosaic)"));
  if (!fileName.isEmpty())
    loadDatabase(fileName);
}

void QtMosaicDatabase::loadDatabase(QString fileName)
{
  mosaicDatabaseModel->open(fileName);
}

void QtMosaicDatabase::saveDatabase()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save a Mosaic database"), QString(), QString::fromAscii("Mosaic database (*.mosaic)"));
  if (!fileName.isEmpty())
    saveDatabase(fileName);
}

void QtMosaicDatabase::saveDatabase(QString fileName)
{
  mosaicDatabaseModel->save(fileName);
}

void QtMosaicDatabase::addImages()
{
  QModelIndexList indexes = ui.treeView->selectionModel()->selectedRows();
  QModelIndex index;
  QModelIndex sourceIndex;

  QProgressDialog progress("Adding files...", "Abort Addition", 0, indexes.size(), this);
  progress.setWindowModality(Qt::WindowModal);

  int i = 0;
  foreach(index, indexes)
  {
    progress.setValue(i);
    sourceIndex = filterModel->mapToSource(index);
    if(model->isDir(sourceIndex))
    {
      addFolder(model->fileInfo(sourceIndex));
    }
    else
    {
      addImage(model->fileInfo(sourceIndex));
    }
    if (progress.wasCanceled())
        break;
    ++i;
  }
  progress.setValue(i);
  ui.statusbar->showMessage(tr("Current number of photos: %1").arg(mosaicDatabaseModel->rowCount()));
  ui.listView->reset();
}

void QtMosaicDatabase::addFolder(const QFileInfo& fileInfo)
{
  QDir dir(fileInfo.filePath());
  QStringList filters;
  filters << "*";
  dir.setNameFilters(filters);

  QFileInfoList list = dir.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot);
  QFileInfo info;

  foreach(info, list)
  {
    if(info.isDir())
    {
      addFolder(info);
    }
    else
    {
      addImage(info);
    }
  }
}

void QtMosaicDatabase::addImage(const QFileInfo& fileInfo)
{
  QString text = fileInfo.filePath();
  try
  {
    mosaicDatabaseModel->addElement(text);
  }
  catch(const std::exception& except)
  {
  }
}

void QtMosaicDatabase::removeImages()
{
  QModelIndexList indexes = ui.listView->selectionModel()->selectedRows();
  QModelIndex index;
  QList<QString> imageList;

  QProgressDialog progress("Removing files...", "Abort removal", 0, indexes.size() * 2, this);
  progress.setWindowModality(Qt::WindowModal);

  int i = 0;
  foreach(index, indexes)
  {
    progress.setValue(i);
    imageList.push_back(index.data(Qt::EditRole).toString());
    if (progress.wasCanceled())
        break;
    ++i;
  }
  QString image;
  foreach(image, imageList)
  {
    progress.setValue(i);
    removeImage(image);
    if (progress.wasCanceled())
        break;
    ++i;
  }
  progress.setValue(i);
  ui.statusbar->showMessage(tr("Current number of photos: %1").arg(mosaicDatabaseModel->rowCount()));
  ui.listView->reset();
}

void QtMosaicDatabase::removeImage(const QString& string)
{
  mosaicDatabaseModel->removeElement(string);
}
