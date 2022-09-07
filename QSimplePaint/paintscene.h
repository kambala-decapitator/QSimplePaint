#ifndef PAINTSCENE_H
#define PAINTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QGraphicsSceneWheelEvent>
#include <QUndoStack>

static const qreal scaleFactor = 1.5;

class PaintScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum DrawMode {LINE = 1, ERASER, RECT, RECT_FILL, RECT_ROUND, RECT_ROUND_FILL, CIRCLE, CIRCLE_FILL};

    PaintScene(QObject *parent = 0);

    DrawMode drawMode() const { return _drawMode; }
    Qt::PenStyle penStyle() const { return _penStyle; }
    Qt::BrushStyle brushStyle() const { return _brushStyle; }
    int penWidth() const { return _penWidth; }
	QColor borderColor() const { return _borderColor; }
	QColor brushColor() const { return _brushColor; }
	bool eraserWasUsed() const { return _eraserWasUsed; }

public slots:
    void setDrawMode(DrawMode newMode) { _drawMode = newMode; }
    void setPenStyle(Qt::PenStyle newPenStyle) { _penStyle = newPenStyle; }
    void setBrushStyle(Qt::BrushStyle newBrushStyle) { _brushStyle = newBrushStyle; }
    void setPenWidth(int newPenWidth) { _penWidth = newPenWidth; }
	void setBorderColor(const QColor &newColor) { _borderColor = newColor; }
	void setBrushColor(const QColor &newColor) { _brushColor = newColor; }
	void setEraserWasUsed(bool newValue) { _eraserWasUsed = newValue; }
	void saveAsSvg(QString filePath);
	void undo();
	void redo();
	virtual void clear();

signals:
	void sceneChanged(bool hasChanged = true);
	void itemAdded();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void wheelEvent(QGraphicsSceneWheelEvent *e);

private:
    QGraphicsLineItem *line;
	QGraphicsRectItem *rect;
	QGraphicsEllipseItem *ellipse;
	QGraphicsPathItem *roundedRectPath;
	QGraphicsPathItem *eraserPath;
	QGraphicsItem *lastItem;
    DrawMode _drawMode;
    Qt::PenStyle _penStyle;
    Qt::BrushStyle _brushStyle;
	QColor _borderColor;
	QColor _brushColor;
    int _penWidth;
    bool isDrawing;
	QPointF rectTopLeft;
	bool _eraserWasUsed;
	QUndoStack undoStack;

	QRectF rectWithMovingPoint(const QPointF &currentPoint);
};

#endif // PAINTSCENE_H
