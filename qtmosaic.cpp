/**
 * \file qtmosaic.cpp
 */

#include <QtGui\QFileDialog>
#include <QtGui\QMessageBox>

#include "qtmosaic.h"

QtMosaic::QtMosaic(QWidget *parent, Qt::WFlags flags)
  : QMainWindow(parent, flags)
{
  ui.setupUi(this);

  createActions();
  createToolbar();
}

void QtMosaic::createActions()
{
  openAct = new QAction(QIcon(":/QtMosaic/Resources/open.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an image"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon(":/QtMosaic/Resources/save.png"), tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the mosaic to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
}

void QtMosaic::createToolbar()
{
  ui.mainToolBar->addAction(openAct);
  ui.mainToolBar->addAction(saveAct);
}

QtMosaic::~QtMosaic()
{

}

void QtMosaic::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  if (!fileName.isEmpty())
    loadFile(fileName);
}

void QtMosaic::loadFile(QString fileName)
{
  QImage image(fileName);
  if (image.isNull()) {
    QMessageBox::information(this, tr("Image Viewer"),
      tr("Cannot load %1.").arg(fileName));
    return;
  }
  ui.originalImage->setPixmap(QPixmap::fromImage(image));
}

void QtMosaic::save()
{
}
