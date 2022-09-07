#ifndef QSIMPLEPAINT_H
#define QSIMPLEPAINT_H

#include <QtGui/QMainWindow>
#include <QtGui/QToolBar>
#include <QtGui/QCloseEvent>

#include <QtCore/QList>

#include "ui_qsimplepaint.h"
#include "paintscene.h"


class QSimplePaint : public QMainWindow
{
    Q_OBJECT

public:
    QSimplePaint(QWidget *parent = 0, Qt::WFlags flags = 0);

private slots:
	void deleteAll();
    void updateToolbarsStateInMenu();
	void setDrawMode(int drawMode) { paintScene->setDrawMode((PaintScene::DrawMode)drawMode); }
    void setPenStyle(int penStyle) { paintScene->setPenStyle((Qt::PenStyle)penStyle); }
    void setBrushStyle(int brushStyle) { paintScene->setBrushStyle((Qt::BrushStyle)brushStyle); }
    void setPenWidth(int penWidth) { paintScene->setPenWidth(penWidth); }
	void newImage();
	void open();
	void save();
	void saveAs();
	void printImage();
	void chooseBorderColor();
	void chooseBrushColor();
	void about();
	void zoomIn();
	void zoomOut();
	void zoomOriginal();
	void initSceneRect();
	void updateSceneRect();

protected:
	void closeEvent(QCloseEvent *e);

private:
    Ui::QSimplePaintClass ui;

    PaintScene *paintScene;
    QToolBar *paintToolbar;
	QString lastFilePath;

	void groupActions(const QList<QAction *> &actionList);
    QWidget *twoActionsInHorizontalLayout(QAction *leftAction, QAction *rightAction);
    void mapActionsToInt(const QList<QAction *> &actionList, const char *connectionSlot);
	QString fileName(const QString &filePath);
	bool saved();
	bool saveRasterImage();
	void imageHasNoChanges(const QString &fileName = QString::null);
};

#endif // QSIMPLEPAINT_H
