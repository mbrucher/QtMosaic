/**
 * \file QtMosaicBuilder.h
 */

#ifndef QTMOSAICBUILDER_H
#define QTMOSAICBUILDER_H

#include <QtCore/qobject.h>
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
  };

private:
  void processImage(QImage& image) const;
  QtMosaicProcessor processor;
  QtMosaicDatabaseModel* model;
};

#endif
