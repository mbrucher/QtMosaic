/**
 * \file QtMosaicBuilder.cpp
 */

#include <QtConcurrentMap>
#include <QtGui/qpainter.h>

#include "QtMosaicBuilder.h"
#include "QtMosaicDatabaseModel.h"

QtMosaicBuilder::QtMosaicBuilder(QObject* parent)
  :QObject(parent)
{
}

void QtMosaicBuilder::build(const QString& database)
{
  model = new QtMosaicDatabaseModel(database, this);
  processor.model = model;
}

QPixmap QtMosaicBuilder::create(const QPixmap* pixmap)
{
  if(pixmap == NULL)
  {
    return QPixmap(); 
  }

  QImage image = pixmap->toImage();

  QPixmap newpixmap;

  processImage(image);

  newpixmap.convertFromImage(image);
  return newpixmap;
}

void QtMosaicBuilder::processImage(QImage& image) const
{
  QVector<QImage> vector;

  for(int j = 0; j < image.height(); j += processor.model->scalingFactor * processor.model->heightFactor)
  {
    for(int i = 0; i < image.width(); i += processor.model->scalingFactor * processor.model->widthFactor)
    {
      vector.push_back(image.copy(i, j, processor.model->scalingFactor * processor.model->widthFactor, processor.model->scalingFactor * processor.model->heightFactor));
    }
  }

  QtConcurrent::map(vector, processor).waitForFinished();

  reconstructImage(image, vector);
}

void QtMosaicBuilder::QtMosaicProcessor::operator()(QImage& image)
{
  
}

void QtMosaicBuilder::reconstructImage(QImage& image, const QVector<QImage>& vector) const
{
  QPainter painter(&image);

  int k = 0;
  for(int j = 0; j < image.height(); j += processor.model->scalingFactor * processor.model->heightFactor)
  {
    for(int i = 0; i < image.width(); i += processor.model->scalingFactor * processor.model->widthFactor)
    {
      painter.drawImage(i, j, vector[k]);
      ++k;
    }
  }
}
