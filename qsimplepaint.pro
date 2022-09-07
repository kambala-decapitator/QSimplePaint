QT += svg
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport widgets

SOURCES += \
	QSimplePaint/main.cpp \
	QSimplePaint/paintcommand.cpp \
	QSimplePaint/paintscene.cpp \
	QSimplePaint/qsimplepaint.cpp \

HEADERS += \
	QSimplePaint/paintcommand.h \
	QSimplePaint/paintscene.h \
	QSimplePaint/qsimplepaint.h \

FORMS += QSimplePaint/qsimplepaint.ui

RESOURCES += QSimplePaint/qsimplepaint.qrc

RC_FILE = QSimplePaint/qsimplepaint.rc
