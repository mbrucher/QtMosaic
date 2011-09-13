#ifndef QTMOSAIC_H
#define QTMOSAIC_H

#include <QtGui/QMainWindow>
#include "ui_qtmosaic.h"

class QtMosaic : public QMainWindow
{
	Q_OBJECT

public:
	QtMosaic(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtMosaic();

private:
	Ui::QtMosaicClass ui;
};

#endif // QTMOSAIC_H
