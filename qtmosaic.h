/**
 * \file qtmosaic.h
 */

#ifndef QTMOSAIC_H
#define QTMOSAIC_H

#include <QtWidgets/QMainWindow>

#include "ui_qtmosaic.h"

class QtMosaicDatabase;
class QtMosaicBuilder;

class QtMosaic : public QMainWindow
{
	Q_OBJECT

public:
	QtMosaic(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~QtMosaic();

private:
	Ui::QtMosaicClass ui;
  QAction* openAct;
  QAction* reloadAct;
  QAction* saveAct;
  QAction* execAct;
  QAction* exitAct;
  QAction* openDatabaseAct;
  QAction* editDatabaseAct;
  QActionGroup* convertGroupAct;
  QAction* convertRGBAct;
  QAction* convertLabAct;
  QAction* convertLcdAct;

  void createActions();
  void createToolbar();
  void createMenubar();

  void loadFile(QString fileName);
  void saveFile(QString fileName);
  void loadDatabase(QString fileName);

  QtMosaicDatabase* databaseUI;
  QtMosaicBuilder* builder;

  QString database;
  QString originalPhoto;

public slots:
  void open();
  void reload();
  void save();
  void exec();

  void editDatabase();
  void openDatabase();

  void updateMosaic(QImage image);
};

#endif // QTMOSAIC_H
