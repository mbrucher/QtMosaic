/**
 * \file QtModaicDatabaseModel.h
 */

#ifndef QTMOSAICDATABASEMODEL_H
#define QTMOSAICDATABASEMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/qlist.h>

class QtMosaicDatabaseModel :
  public QAbstractListModel
{
public:
  QtMosaicDatabaseModel(const QString& filename = QString(), QObject* parent = 0);
  ~QtMosaicDatabaseModel(void);

  int rowCount(const QModelIndex &parent = QModelIndex()) const ;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
  QList<std::pair<QString, QPixmap> > database;

  void reset();
  void open(const QString& filename);

  static QPixmap createThumbnail(const QString& filename);

  static const int scalingFactor = 3;
  static const int widthFactor = 12;
  static const int heightFactor = 12;
};

#endif // QTMOSAICDATABASEMODEL_H
