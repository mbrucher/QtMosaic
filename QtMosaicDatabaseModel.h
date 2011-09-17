/**
 * \file QtModaicDatabaseModel.h
 */

#ifndef QTMOSAICDATABASEMODEL_H
#define QTMOSAICDATABASEMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/qmap.h>

class QtMosaicDatabaseModel :
  public QAbstractListModel
{
public:
  QtMosaicDatabaseModel(const QString& filename = QString(), QObject* parent = 0);
  ~QtMosaicDatabaseModel(void);

  int rowCount(const QModelIndex &parent = QModelIndex()) const ;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
  QMap<QString, QPixmap> database;

  void reset();
  void open(const QString& filename);

  static QPixmap createThumbnail(const QString& filename);
};

#endif // QTMOSAICDATABASEMODEL_H
