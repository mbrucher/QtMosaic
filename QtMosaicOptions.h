/**
 * \file QtMosaicOptions.h
 */

#ifndef QTMOSAICOPTIONS
#define QTMOSAICOPTIONS

#include <qstring.h>

class QtMosaicOptions
{
  QtMosaicOptions(void);

  QString defaultFolder;

  static QtMosaicOptions defaultOptions;
public:
  static QtMosaicOptions& getInstance();

  QString getDefaultFolder() const;
  void setDefaultFolder(const QString& defaultFolder);
};

#endif
