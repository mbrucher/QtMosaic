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

void QtMosaicOptions::getDefaultFolder(const QString& defaultFolder)
{
  this->defaultFolder = defaultFolder;
}
