/**
 * \file QtModaicDatabaseModel.h
 */

#ifndef QTMOSAICDATABASEMODEL_H
#define QTMOSAICDATABASEMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/qlist.h>

#include "AntipoleTree.h"

class QtMosaicDatabaseModel :
  public QAbstractListModel
{
public:
  typedef QAbstractListModel Parent;

  QtMosaicDatabaseModel(const QString& filename = QString(), QObject* parent = 0);
  ~QtMosaicDatabaseModel(void);

  int rowCount(const QModelIndex &parent = QModelIndex()) const ;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  void reset();
  void open(const QString& filename);
  void save(const QString& filename);

  void addElement(const QString& filename);
  void removeElement(const QString& filename);

  typedef QList<std::pair<QString, QPixmap> > Database;
  typedef QList<std::pair<QString, QImage> > ParallelDatabase;
  void build();
  void setMethod(int method);

  const QVector<QImage>& getThumbnails() const
  {
    return thumbnails;
  }
  const ParallelDatabase& getParallelDatabase() const
  {
    return parallelDatabase;
  }
  const Database& getDatabase() const
  {
    return database;
  }
  const AntipoleTree& getTree() const
  {
    return tree;
  }

private:
  Database database;
  ParallelDatabase parallelDatabase;
  AntipoleTree tree;
  int method;

  QVector<QImage> thumbnails;

  static QPixmap createThumbnail(const QString& filename);

public:
  static const int scalingFactor = 3;
  static const int widthFactor = 16;
  static const int heightFactor = 12;
};

#endif // QTMOSAICDATABASEMODEL_H
