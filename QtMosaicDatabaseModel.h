/**
 * \file QtModaicDatabaseModel.h
 */

#ifndef QTMOSAICDATABASEMODEL_H
#define QTMOSAICDATABASEMODEL_H

#include <QtCore/QAbstractListModel>

class QtMosaicDatabaseModel :
  public QAbstractListModel
{
public:
  QtMosaicDatabaseModel(QObject* parent = 0);
  ~QtMosaicDatabaseModel(void);

  int rowCount(const QModelIndex &parent = QModelIndex()) const ;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // QTMOSAICDATABASEMODEL_H
