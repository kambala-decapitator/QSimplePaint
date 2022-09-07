#include <QtGui/QApplication>

#include "qsimplepaint.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName("QSimplePaint");
	QSimplePaint w;
	w.show();
	return a.exec();
}
