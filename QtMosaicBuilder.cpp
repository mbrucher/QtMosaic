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
}

QPixmap QtMosaicBuilder::create(const QPixmap* pixmap)
{
  if(pixmap == NULL)
  {
    return QPixmap(); 
  }
  return *pixmap;
}
