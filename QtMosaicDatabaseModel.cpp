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
  database.append(std::make_pair("test.jpeg", createThumbnail(QString("test.jpeg"))));
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
  if(index.row() > database.size())
  {
    return QVariant();
  }
  if(role == Qt::DisplayRole)
  {
    return database.at(index.row()).first;
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
