/**
 * \file qtmosaic.cpp
 */

#include <QtCore/QString>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "qtmosaic.h"
#include "qtmosaicdatabase.h"
#include "QtMosaicBuilder.h"
#include "QtMosaicOptions.h"

QtMosaic::QtMosaic(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags), databaseUI(NULL)
{
  ui.setupUi(this);

  createActions();
  createToolbar();
  createMenubar();

  builder = new QtMosaicBuilder(this);
}

void QtMosaic::createActions()
{
  openAct = new QAction(QIcon(":/QtMosaic/Resources/open.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an image"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  reloadAct = new QAction(QIcon(":/QtMosaic/Resources/reload.png"), tr("&Reload"), this);
  reloadAct->setStatusTip(tr("Reload the original photo"));
  connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));

  execAct = new QAction(QIcon(":/QtMosaic/Resources/exec.png"), tr("&Exec"), this);
  execAct->setStatusTip(tr("Create a new mosaic"));
  connect(execAct, SIGNAL(triggered()), this, SLOT(exec()));

  saveAct = new QAction(QIcon(":/QtMosaic/Resources/save.png"), tr("&Save..."), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the mosaic to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  exitAct = new QAction(QIcon(":/QtMosaic/Resources/close.png"), tr("&Quit"), this);
  exitAct->setShortcuts(QKeySequence::Close);
  exitAct->setStatusTip(tr("Exit"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  openDatabaseAct = new QAction(QIcon(":/QtMosaic/Resources/opendatabase.png"), tr("Open &Database..."), this);
  openDatabaseAct->setStatusTip(tr("Open a mosaic database"));
  connect(openDatabaseAct, SIGNAL(triggered()), this, SLOT(openDatabase()));

  editDatabaseAct = new QAction(QIcon(":/QtMosaic/Resources/editdatabase.png"), tr("&Edit Database"), this);
  editDatabaseAct->setStatusTip(tr("Open the Edit Database window"));
  connect(editDatabaseAct, SIGNAL(triggered()), this, SLOT(editDatabase()));
}

void QtMosaic::createToolbar()
{
  ui.mainToolBar->addAction(openAct);
  ui.mainToolBar->addAction(saveAct);
  ui.mainToolBar->addAction(execAct);
  ui.mainToolBar->addSeparator();
  ui.mainToolBar->addAction(openDatabaseAct);
  ui.mainToolBar->addAction(editDatabaseAct);
}

void QtMosaic::createMenubar()
{
  ui.menuFile->addAction(openAct);
  ui.menuFile->addAction(reloadAct);
  ui.menuFile->addAction(saveAct);
  ui.menuFile->addAction(execAct);
  ui.menuFile->addSeparator();
  ui.menuFile->addAction(exitAct);

  ui.menuDatabase->addAction(openDatabaseAct);
  ui.menuDatabase->addAction(editDatabaseAct);
}

QtMosaic::~QtMosaic()
{

}

void QtMosaic::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open an image"), QtMosaicOptions::getInstance().getDefaultFolder());
  if (!fileName.isEmpty())
  {
    QtMosaicOptions::getInstance().setDefaultFolder(QFileInfo(fileName).absolutePath());
    originalPhoto = fileName;
    loadFile(fileName);
  }
}

void QtMosaic::reload()
{
  if (!originalPhoto.isEmpty())
  {
    loadFile(originalPhoto);
  }
}

void QtMosaic::loadFile(QString fileName)
{
  QImage image(fileName);
  if (image.isNull()) {
    QMessageBox::information(this, tr("Image Viewer"),
      tr("Cannot load %1.").arg(fileName));
    return;
  }
  QPixmap pixmap = QPixmap::fromImage(image);
  ui.originalImage->setPixmap(pixmap);
  ui.mosaicImage->setPixmap(pixmap);
}

void QtMosaic::save()
{
  if(ui.mosaicImage->pixmap() ==NULL)
  {
    QMessageBox::information(this, tr("Image Viewer"),
      tr("No image loaded yet"));
    return;
  }
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save an image"), QtMosaicOptions::getInstance().getDefaultFolder());
  if (!fileName.isEmpty())
  {
    QtMosaicOptions::getInstance().setDefaultFolder(QFileInfo(fileName).absolutePath());
    saveFile(fileName);
  }
  else
  {
    QMessageBox::information(this, tr("Image Viewer"),
      tr("No given filename"));
  }
}

void QtMosaic::saveFile(QString fileName)
{
  ui.mosaicImage->pixmap()->save(fileName);
}

void QtMosaic::openDatabase()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Mosaic database"), QtMosaicOptions::getInstance().getDefaultFolder(), QString::fromLatin1("Mosaic database (*.mosaic)"));
  if (!fileName.isEmpty())
  {
    QtMosaicOptions::getInstance().setDefaultFolder(QFileInfo(fileName).absolutePath());
    loadDatabase(fileName);
  }
}

void QtMosaic::loadDatabase(QString filename)
{
  database = filename;
  builder->build(filename);

  ui.databaseSize->setText(QString::number(builder->getDatabaseSize()));
  ui.mosaicHeight->setValue(builder->getDatabaseDefaultHeight());
  ui.mosaicHeight->setMaximum(builder->getDatabaseDefaultHeight());
  ui.mosaicWidth->setValue(builder->getDatabaseDefaultWidth());
  ui.mosaicWidth->setMaximum(builder->getDatabaseDefaultWidth());
  ui.outputRatio->setValue(1);

  ui.databaseArea->setEnabled(true);
}

void QtMosaic::editDatabase()
{
  if(databaseUI == NULL)
    databaseUI = new QtMosaicDatabase();
  if(database != "")
    databaseUI->loadDatabase(database);

  databaseUI->show();
}

void QtMosaic::exec()
{
  if(database != "")
  {
    connect(builder, SIGNAL(updateMosaic(QImage)), this, SLOT(updateMosaic(QImage)));
    builder->create(ui.originalImage->pixmap(), ui.mosaicHeight->value(), ui.mosaicWidth->value(), ui.outputRatio->value());
  }
  else
  {
    QMessageBox::information(this, tr("Image Viewer"),
      tr("You need to load a database first"));
  }
}

void QtMosaic::updateMosaic(QImage image)
{
  QPixmap newpixmap;
  newpixmap.convertFromImage(image);
  ui.mosaicImage->setPixmap(newpixmap);
}
