/**
 * \file qtmosaicdatabase.h
 */

#ifndef QTMOSAICDATABASE_H
#define QTMOSAICDATABASE_H

#include <QtCore/qdir.h>
#include <QtCore/QFileInfo>
#include <QtWidgets/QMainWindow>
#include "ui_QtMosaicDatabase.h"

class QFileSystemModel;
class QSortFilterProxyModel;
class QtMosaicDatabaseModel;

class QtMosaicDatabase : public QMainWindow
{
	Q_OBJECT

public:
	QtMosaicDatabase(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~QtMosaicDatabase();

  void newDatabase(QString fileName);
  void loadDatabase(QString fileName);
  void saveDatabase(QString fileName);

private:
	Ui::QtMosaicDatabaseClass ui;
  
  QAction* newAct;
  QAction* openAct;
  QAction* saveAct;

  void createModels();

  void createActions();
  void createToolbar();
  void createMenubar();

  void addImage(const QFileInfo& fileInfo);
  void addFolder(const QFileInfo& fileInfo);
  void removeImage(const QString& string);

  QFileSystemModel* model;
  QSortFilterProxyModel* filterModel;
  QtMosaicDatabaseModel* mosaicDatabaseModel;

public slots:
  void newDatabase();
  void openDatabase();
  void saveDatabase();

  void addImages();
  void removeImages();
};

#endif // QTMOSAICDATABASE_H
