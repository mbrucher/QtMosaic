/**
 * \file QtModaicDatabaseModel.cpp
 */

#include <QtGui/qpixmap.h>

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
  database["test.jpeg"] = createThumbnail(QString("test.jpeg"));
}

void QtMosaicDatabaseModel::open(const QString& filename)
{
}

int QtMosaicDatabaseModel::rowCount(const QModelIndex &parent) const
{
  return database.size();
}

QVariant QtMosaicDatabaseModel::data(const QModelIndex &index, int role) const
{
  if(role == Qt::DisplayRole)
  {
    return "test.jpeg";
  }
  if(role == Qt::DecorationRole)
  {
    return database["test.jpeg"];
  }

  return QVariant();
}

QPixmap QtMosaicDatabaseModel::createThumbnail(const QString& filename)
{
  return QPixmap(filename).scaled(3*16, 3*12);
}
