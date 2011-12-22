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

/*  int best = 0;
  float bestDistance = distance(image, model->getThumbnails()[0]);

  for(int i = 1; i < model->getThumbnails().size(); ++i)
  {
    float newDistance = distance(image, model->getThumbnails()[i]);
    if(newDistance < bestDistance)
    {
      bestDistance = newDistance;
      best = i;
    }
  }*/
  int best = model->getTree().getClosestThumbnail(thumbnail);
  image = adaptImage(model->getParallelDatabase()[best].second, image);
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
      painter.drawImage(i, j, vector[k].scaled(mosaicWidth * outputRatio, mosaicHeight * outputRatio));
      ++k;
      if(progress.wasCanceled())
      {
        return;
      }
    }
  }
}

void computeMeans(const QImage& image, long& red, long& green, long& blue)
{
  red = 0;
  green = 0;
  blue = 0;

  for(int j = 0; j < image.height(); ++j)
  {
    for(int i = 0; i < image.width(); ++i)
    {
      const QRgb& rgb = image.pixel(i, j);
      red += qRed(rgb);
      green += qGreen(rgb);
      blue += qBlue(rgb);
    }
  }

  red /= image.height() * image.width();
  green /= image.height() * image.width();
  blue /= image.height() * image.width();
}

QImage adaptImage(const QImage& image, const QImage& reference)
{
  long red_ref, blue_ref, green_ref;
  long red_img, blue_img, green_img;
  computeMeans(reference, red_ref, green_ref, blue_ref);
  computeMeans(image, red_img, green_img, blue_img);

  QImage newImage = image;
  for(int j = 0; j < newImage.height(); ++j)
  {
    for(int i = 0; i < newImage.width(); ++i)
    {
      const QRgb& rgb = newImage.pixel(i, j);
      int red = qRed(rgb) + red_ref - red_img;
      int blue = qBlue(rgb) + blue_ref - blue_img;
      int green = qGreen(rgb) + green_ref - green_ref;
      red = std::min(std::max(0, red), 255);
      blue = std::min(std::max(0, blue), 255);
      green = std::min(std::max(0, green), 255);
      newImage.setPixel(i, j, qRgb(red, green, blue));
    }
  }
  return newImage;
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

