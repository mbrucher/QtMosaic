/**
 * \file qtmosaic.h
 */

#ifndef QTMOSAIC_H
#define QTMOSAIC_H

#include <QtGui/QMainWindow>

#include "ui_qtmosaic.h"

class QtMosaicDatabase;
class QtMosaicBuilder;

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
  QAction *execAct;
  QAction *exitAct;
  QAction *openDatabaseAct;
  QAction *editDatabaseAct;

  void createActions();
  void createToolbar();
  void createMenubar();

  void loadFile(QString fileName);
  void saveFile(QString fileName);
  void loadDatabase(QString fileName);

  QtMosaicDatabase* databaseUI;
  QtMosaicBuilder* builder;

  QString database;

public slots:
  void open();
  void save();
  void exec();

  void editDatabase();
  void openDatabase();
};

#endif // QTMOSAIC_H
