/**
 * \file QtMosaicBuilder.h
 */

#ifndef QTMOSAICBUILDER_H
#define QTMOSAICBUILDER_H

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <QtGui/qpixmap.h>

class QtMosaicDatabaseModel;

class QtMosaicBuilder: public QObject
{
public:
  QtMosaicBuilder(QObject* parent = NULL);

  void build(const QString& database);
  QPixmap create(const QPixmap* pixmap);

  class QtMosaicProcessor
  {
  public:
    void operator()(QImage& image);

    QtMosaicDatabaseModel* model;

    static float distance(const QImage& image1, const QImage& image2);
    static float distance(const QRgb& rgb1, const QRgb& rgb2);
  };

private:
  void processImage(QImage& image) const;
  void reconstructImage(QImage& image, const QVector<QImage>& vector) const;

  QtMosaicProcessor processor;
  QtMosaicDatabaseModel* model;
};

#endif
