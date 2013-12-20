/**
 * \file QtMosaicBuilder.h
 */

#ifndef QTMOSAICBUILDER_H
#define QTMOSAICBUILDER_H

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <QtCore/qtimer.h>
#include <QtGui/qpixmap.h>
#include <QtWidgets/qprogressdialog.h>
#include <QtConcurrent/QtConcurrentMap>

class QtMosaicDatabaseModel;

class QtMosaicBuilder: public QObject
{
	Q_OBJECT

public:
  QtMosaicBuilder(QObject* parent = NULL);

  void build(const QString& database, int method = 0);
  void create(const QPixmap* pixmap, int mosaicHeight, int mosaicWidth, float outputRatio);

  class QtMosaicProcessor
  {
  public:
    void operator()(QImage& image);

    QtMosaicDatabaseModel* model;

    static float distance(const QImage& image1, const QImage& image2);
    static float distance(const QRgb& rgb1, const QRgb& rgb2);
  };

  long getDatabaseSize() const;
  long getDatabaseDefaultHeight() const;
  long getDatabaseDefaultWidth() const;

private:
  void processImage(QImage& image);
  void createParts(QImage& image);
  void reconstructImage(QImage& image, const QVector<QImage>& vector) const;

  QFuture<void> future;
  QProgressDialog* progress;
  QTimer* timer;

  QtMosaicProcessor processor;
  QtMosaicDatabaseModel* model;

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

QImage adaptImage(const QImage& image, const QImage& reference);
void computeMeans(const QImage& image, long& red, long& green, long& blue);

#endif
