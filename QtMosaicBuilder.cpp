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
  model->build();
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
  QImage thumbnail = image.scaled(model->scalingFactor, model->scalingFactor);

  if(model->getThumbnails().empty())
  {
    return;
  }

  int best = 0;
  float bestDistance = distance(image, model->getThumbnails()[0]);

  for(int i = 1; i < model->getThumbnails().size(); ++i)
  {
    float newDistance = distance(image, model->getThumbnails()[i]);
    if(newDistance < bestDistance)
    {
      bestDistance = newDistance;
      best = i;
    }
  }
  image = model->getDatabase()[best].second.toImage();
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

float QtMosaicBuilder::QtMosaicProcessor::distance(const QImage& image1, const QImage& image2)
{
  float result = 0;

  for(int j = 0; j < image1.height(); ++j)
  {
    for(int i = 0; i < image1.width(); ++i)
    {
      result += distance(image1.pixel(i, j), image2.pixel(i, j));
    }
  }
  return result;
}

float QtMosaicBuilder::QtMosaicProcessor::distance(const QRgb& rgb1, const QRgb& rgb2)
{
  return (qRed(rgb1) - qRed(rgb2)) * (qRed(rgb1) - qRed(rgb2)) + (qGreen(rgb1) - qGreen(rgb2)) * (qGreen(rgb1) - qGreen(rgb2)) + (qBlue(rgb1) - qBlue(rgb2)) * (qBlue(rgb1) - qBlue(rgb2));
}
