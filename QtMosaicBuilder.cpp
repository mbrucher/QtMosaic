/**
 * \file QtMosaicBuilder.cpp
 */

#include "QtMosaicBuilder.h"
#include "QtMosaicDatabaseModel.h"

QtMosaicBuilder::QtMosaicBuilder(QObject* parent)
  :QObject(parent)
{
}

void QtMosaicBuilder::build(const QString& database)
{
  model = new QtMosaicDatabaseModel(database, this);
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
  for(int j = 0; j < image.height(); j += model->scalingFactor * model->heightFactor)
  {
    for(int i = 0; i < image.width(); i += model->scalingFactor * model->widthFactor)
    {
      image.setPixel(i, j, 0);
    }
  }
}
