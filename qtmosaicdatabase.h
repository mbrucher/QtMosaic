/**
 * \file qtmosaicdatabase.h
 */

#ifndef QTMOSAICDATABASE_H
#define QTMOSAICDATABASE_H

#include <QtGui/QMainWindow>
#include "ui_qtmosaicdatabase.h"

class QFileSystemModel;
class QSortFilterProxyModel;

class QtMosaicDatabase : public QMainWindow
{
	Q_OBJECT

public:
	QtMosaicDatabase(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtMosaicDatabase();

private:
	Ui::QtMosaicDatabaseClass ui;
  
  QAction* newAct;
  QAction* openAct;
  QAction* saveAct;

  void createActions();
  void createToolbar();
  void createMenubar();

  void newDatabase(QString fileName);
  void loadDatabase(QString fileName);
  void saveDatabase(QString fileName);

  QFileSystemModel *model;
  QSortFilterProxyModel *filterModel;

public slots:
  void newDatabase();
  void openDatabase();
  void saveDatabase();
};

#endif // QTMOSAICDATABASE_H
