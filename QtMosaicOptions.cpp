/**
 * \file QtMosaicOptions.cpp
 */

#include "QtMosaicOptions.h"

QtMosaicOptions QtMosaicOptions::defaultOptions;

QtMosaicOptions::QtMosaicOptions(void)
{
}

QtMosaicOptions& QtMosaicOptions::getInstance()
{
  return defaultOptions;
}

QString QtMosaicOptions::getDefaultFolder() const
{
  return defaultFolder;
}

void QtMosaicOptions::setDefaultFolder(const QString& defaultFolder)
{
  this->defaultFolder = defaultFolder;
}
