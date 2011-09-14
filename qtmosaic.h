/**
 * \file qtmosaic.h
 */

#ifndef QTMOSAIC_H
#define QTMOSAIC_H

#include <QtGui/QMainWindow>
#include "ui_qtmosaic.h"

class QtMosaic : public QMainWindow
{
	Q_OBJECT

public:
	QtMosaic(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtMosaic();

private:
	Ui::QtMosaicClass ui;
  QAction *openAct;
  QAction *saveAct;

  void createActions();
  void createToolbar();

  void loadFile(QString fileName);
  void saveFile(QString fileName);

public slots:
  void open();
  void save();
};

#endif // QTMOSAIC_H
