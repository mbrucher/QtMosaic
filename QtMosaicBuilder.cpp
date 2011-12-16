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

void QtMosaicBuilder::createParts(QImage& image)
{
  QProgressDialog progress("Destruction in progress.", "Cancel", 0, (image.height() + 1) * (image.width() + 1) / (mosaicHeight * mosaicWidth), dynamic_cast<QWidget*>(this->parent()));
  progress.setWindowModality(Qt::WindowModal);;

  imageParts.clear();
  int k = 0;
  for(int j = 0; j < image.height(); j += mosaicHeight)
  {
    for(int i = 0; i < image.width(); i += mosaicWidth)
    {
      progress.setValue(k);
      imageParts.push_back(image.copy(i, j, mosaicWidth, mosaicHeight).scaled(processor.model->scalingFactor, processor.model->scalingFactor));
      ++k;
      if(progress.wasCanceled())
      {
        return;
      }
    }
  }
}

void QtMosaicBuilder::processImage(QImage& image)
{
  createParts(image);

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
  QProgressDialog progress("Reconstruction in progress.", "Cancel", 0, vector.size(), dynamic_cast<QWidget*>(this->parent()));
  progress.setWindowModality(Qt::WindowModal);;

  image = image.scaled(image.width() * outputRatio, image.height() * outputRatio);
  QPainter painter(&image);

  int k = 0;
  int widthOutSize = mosaicWidth * outputRatio;
  int heightOutSize = mosaicHeight * outputRatio;
  for(int j = 0; j < image.height(); j += heightOutSize)
  {
    for(int i = 0; i < image.width(); i += widthOutSize)
    {
      progress.setValue(k);
      painter.drawImage(i, j, adaptImage(vector[k].scaled(mosaicWidth * outputRatio, mosaicHeight * outputRatio), imageParts[i / widthOutSize + j / heightOutSize * (image.width() + widthOutSize - 1) / widthOutSize]));
      ++k;
      if(progress.wasCanceled())
      {
        return;
      }
    }
  }
}

QImage QtMosaicBuilder::adaptImage(const QImage& image, const QImage& reference) const
{
  return image;
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
  if(future.isCanceled() && future.isFinished())
  {
    timer->stop();
    progress->deleteLater();
    return;
  }

  progress->setValue(future.progressValue());
  if(!future.isCanceled() && future.isFinished())
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

