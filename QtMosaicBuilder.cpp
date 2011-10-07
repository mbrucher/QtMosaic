/**
 * \file QtMosaicBuilder.cpp
 */

#include <iostream>
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

void QtMosaicBuilder::create(const QPixmap* pixmap, int mosaicHeight, int mosaicWidth, float outputRatio)
{
  if(pixmap == NULL)
  {
    return; 
  }

  this->mosaicHeight = mosaicHeight;
  this->mosaicWidth = mosaicWidth;
  this->outputRatio = outputRatio;

  image = pixmap->toImage();
  processImage(image);
}

void QtMosaicBuilder::processImage(QImage& image)
{
  imageParts.clear();
  for(int j = 0; j < image.height(); j += mosaicHeight)
  {
    for(int i = 0; i < image.width(); i += mosaicWidth)
    {
      imageParts.push_back(image.copy(i, j, mosaicWidth, mosaicHeight).scaled(processor.model->scalingFactor, processor.model->scalingFactor));
    }
  }

  future = QtConcurrent::map(imageParts, processor);
  progress = new QProgressDialog("Operation in progress.", "Cancel", future.progressMinimum(), future.progressMaximum(), dynamic_cast<QWidget*>(this->parent()));
  progress->setWindowModality(Qt::WindowModal);;
  connect(progress, SIGNAL(canceled()), this, SLOT(cancel()));
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(0);
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
  image = model->getParallelDatabase()[best].second;
}

void QtMosaicBuilder::reconstructImage(QImage& image, const QVector<QImage>& vector) const
{
  image = image.scaled(image.width() * outputRatio, image.height() * outputRatio);
  QPainter painter(&image);

  int k = 0;
  for(int j = 0; j < image.height(); j += mosaicHeight * outputRatio)
  {
    for(int i = 0; i < image.width(); i += mosaicWidth * outputRatio)
    {
      painter.drawImage(i, j, vector[k].scaled(mosaicWidth * outputRatio, mosaicHeight * outputRatio));
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

void QtMosaicBuilder::update()
{
  if(future.isCanceled())
  {
    return;
  }

  progress->setValue(future.progressValue());
  if(future.isFinished())
  {
    reconstructImage(image, imageParts);
    emit updateMosaic(image);
    timer->stop();
    progress->deleteLater();
  }
}

void QtMosaicBuilder::cancel()
{
  future.cancel();
  timer->stop();
  progress->deleteLater();
}

long QtMosaicBuilder::getDatabaseSize() const
{
  return model->getDatabase().size();
}

long QtMosaicBuilder::getDatabaseDefaultHeight() const
{
  if(getDatabaseSize() > 0)
  {
    return model->getDatabase()[0].second.height();
  }
  else
  {
    return 0;
  }
}

long QtMosaicBuilder::getDatabaseDefaultWidth() const
{
  if(getDatabaseSize() > 0)
  {
    return model->getDatabase()[0].second.width();
  }
  else
  {
    return 0;
  }
}

