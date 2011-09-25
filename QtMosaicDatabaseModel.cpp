/**
 * \file QtModaicDatabaseModel.cpp
 */

#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qmessagebox.h>

#include "QtMosaicDatabaseModel.h"

QtMosaicDatabaseModel::QtMosaicDatabaseModel(const QString& filename, QObject* parent)
  :QAbstractListModel(parent)
{
  if(filename == "")
  {
    reset();
  }
  else
  {
    open(filename);
  }
}

QtMosaicDatabaseModel::~QtMosaicDatabaseModel(void)
{
}

void QtMosaicDatabaseModel::reset()
{
  database.clear();
  Parent::reset();
}

void QtMosaicDatabaseModel::open(const QString& filename)
{
  database.clear();
  Parent::reset();
  QFile file(filename);
  file.open(QIODevice::ReadOnly);
  QDataStream openedFile(&file);

  int version;
  openedFile >> version; // Version
  int size;
  openedFile >> size;
  for(int i = 0; i < size; ++i)
  {
    QString picname;
    QPixmap pixmap;
    openedFile >> picname;
    openedFile >> pixmap;
    database.append(std::make_pair(picname, pixmap));
  }
}

void QtMosaicDatabaseModel::save(const QString& filename)
{
  QFile file(filename);
  file.open(QIODevice::WriteOnly);
  QDataStream openedFile(&file);

  int version = 1;

  openedFile << version; // Version
  openedFile << database.size();
  for(Database::const_iterator it = database.begin(); it != database.end(); ++it)
  {
    openedFile << it->first;
    openedFile << it->second;
  }
}

int QtMosaicDatabaseModel::rowCount(const QModelIndex &parent) const
{
  return database.size();
}

QVariant QtMosaicDatabaseModel::data(const QModelIndex &index, int role) const
{
  if(index.row() > database.size())
  {
    return QVariant();
  }
  if(role == Qt::DisplayRole)
  {
    return QFileInfo(database.at(index.row()).first).fileName();
  }
  if(role == Qt::DecorationRole)
  {
    return database.at(index.row()).second;
  }

  return QVariant();
}

QPixmap QtMosaicDatabaseModel::createThumbnail(const QString& filename)
{
  return QPixmap(filename).scaled(scalingFactor*widthFactor, scalingFactor*heightFactor);
}

void QtMosaicDatabaseModel::addElement(const QString& filename)
{
  database.append(std::make_pair(filename, createThumbnail(filename)));
  QMessageBox::about(NULL, filename, filename);
}
