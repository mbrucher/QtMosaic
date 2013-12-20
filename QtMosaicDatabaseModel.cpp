/**
 * \file QtModaicDatabaseModel.cpp
 */

#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtGui/qpixmap.h>
#include <QtWidgets/QMessageBox>

#include <algorithm>
#include <stdexcept>

#include "QtMosaicDatabaseModel.h"

QtMosaicDatabaseModel::QtMosaicDatabaseModel(const QString& filename, QObject* parent)
  :QAbstractListModel(parent), method(0)
{
  if(filename != "")
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
  beginResetModel();
  endResetModel();
}

void QtMosaicDatabaseModel::setMethod(int method)
{
  this->method = method;
}

void QtMosaicDatabaseModel::open(const QString& filename)
{
  reset();
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
  if(role == Qt::EditRole)
  {
    return database.at(index.row()).first;
  }

  return QVariant();
}

QPixmap QtMosaicDatabaseModel::createThumbnail(const QString& filename)
{
  QPixmap image(filename);
  if(image.isNull())
  {
    throw std::invalid_argument("Not an image file");
  }
  return QPixmap(filename).scaled(scalingFactor*widthFactor, scalingFactor*heightFactor);
}

void QtMosaicDatabaseModel::addElement(const QString& filename)
{
  if(std::find_if(database.begin(), database.end(), [&](Database::value_type& value){return value.first == filename;}) != database.end())
  {
    return;
  }
  database.append(std::make_pair(filename, createThumbnail(filename)));
}

void QtMosaicDatabaseModel::removeElement(const QString& filename)
{
  Database::iterator it = std::find_if(database.begin(), database.end(), [&](Database::value_type& value){return value.first == filename;});
  if(it != database.end())
  {
    database.erase(it);
  }
}

void QtMosaicDatabaseModel::build()
{
  Database::value_type image;
  tree.setMethod(method);

  foreach(image, database)
  {
    QImage temp = image.second.scaled(scalingFactor, scalingFactor).toImage();
    thumbnails.push_back(temp);
    parallelDatabase.push_back(std::make_pair(image.first, image.second.toImage()));
  }
  tree.build(thumbnails);
}
