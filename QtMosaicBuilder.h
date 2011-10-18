/**
 * \file QtMosaicBuilder.h
 */

#ifndef QTMOSAICBUILDER_H
#define QTMOSAICBUILDER_H

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <QtCore/qtimer.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qprogressdialog.h>
#include <QtConcurrentMap>

#include "AntipoleTree.h"

class QtMosaicDatabaseModel;

class QtMosaicBuilder: public QObject
{
	Q_OBJECT

public:
  QtMosaicBuilder(QObject* parent = NULL);

  void build(const QString& database);
  void create(const QPixmap* pixmap, int mosaicHeight, int mosaicWidth, float outputRatio);

  class QtMosaicProcessor
  {
  public:
    void operator()(QImage& image);

    QtMosaicDatabaseModel* model;
    AntipoleTree* tree;

    static float distance(const QImage& image1, const QImage& image2);
    static float distance(const QRgb& rgb1, const QRgb& rgb2);
  };

  long getDatabaseSize() const;
  long getDatabaseDefaultHeight() const;
  long getDatabaseDefaultWidth() const;

private:
  void processImage(QImage& image);
  void reconstructImage(QImage& image, const QVector<QImage>& vector) const;

  QFuture<void> future;
  QProgressDialog* progress;
  QTimer* timer;

  QtMosaicProcessor processor;
  QtMosaicDatabaseModel* model;
  AntipoleTree tree;

  QImage image;
  QVector<QImage> imageParts;

  int mosaicHeight;
  int mosaicWidth;
  float outputRatio;

public slots:
  void update();
  void cancel();

signals:
  void updateMosaic(QImage image);
};

#endif
