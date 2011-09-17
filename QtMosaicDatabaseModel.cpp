/**
 * \file QtModaicDatabaseModel.cpp
 */

#include "QtMosaicDatabaseModel.h"

QtMosaicDatabaseModel::QtMosaicDatabaseModel(QObject* parent)
  :QAbstractListModel(parent)
{
}

QtMosaicDatabaseModel::~QtMosaicDatabaseModel(void)
{
}

int QtMosaicDatabaseModel::rowCount(const QModelIndex &parent) const
{
  return 2;
}

QVariant QtMosaicDatabaseModel::data(const QModelIndex &index, int role) const
{
  return QVariant();
}
