/**
 * \file main.cpp
 */

#include "qtmosaic.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtMosaic w;
	w.show();
	return a.exec();
}
