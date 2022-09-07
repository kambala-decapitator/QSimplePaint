#include "paintscene.h"

#include <QtSvg/QSvgGenerator>

#include <QtGui/QPainter>
#include <QtGui/QGraphicsView>

#include "paintcommand.h"

static const qreal cornerRadius = 15.0; // for rounded rect

PaintScene::PaintScene(QObject *parent) : QGraphicsScene(parent)
{
    _drawMode = LINE;
    _penStyle = Qt::SolidLine;
    _brushStyle = Qt::SolidPattern;
	_penWidth = 1;
	_eraserWasUsed = false;
}

QRectF PaintScene::rectWithMovingPoint(const QPointF &currentPoint)
{
	if (rectTopLeft.x() <= currentPoint.x() && rectTopLeft.y() <= currentPoint.y())
		return QRectF(rectTopLeft, currentPoint);
	else if (rectTopLeft.x() > currentPoint.x() && rectTopLeft.y() <= currentPoint.y())
		return QRectF(currentPoint.x(), rectTopLeft.y(), rectTopLeft.x() - currentPoint.x(), currentPoint.y() - rectTopLeft.y());
	else if (rectTopLeft.x() > currentPoint.x() && rectTopLeft.y() > currentPoint.y())
		return QRectF(currentPoint, rectTopLeft);
	else if (rectTopLeft.x() <= currentPoint.x() && rectTopLeft.y() > currentPoint.y())
		return QRectF(rectTopLeft.x(), currentPoint.y(), currentPoint.x() - rectTopLeft.x(), rectTopLeft.y() - currentPoint.y());
	return QRectF();
}

void PaintScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    isDrawing = true;
	emit sceneChanged();

	QPointF pressPoint = mouseEvent->scenePos();
	switch (_drawMode)
    {
    case LINE:
		line = addLine(QLineF(pressPoint, pressPoint), QPen(QBrush(_borderColor, _brushStyle), _penWidth, _penStyle));
		lastItem = line;
		break;
	case ERASER:
		{
			_eraserWasUsed = true;
			QPainterPath path(pressPoint);
			path.addEllipse(pressPoint, 0.01, 0.01);
			eraserPath = addPath(path, QPen(QBrush(Qt::white, Qt::SolidPattern), _penWidth + 2, Qt::SolidLine));
			lastItem = eraserPath;
		}
		break;
	case RECT:
		rect = addRect(QRectF(pressPoint, pressPoint), QPen(QBrush(_borderColor, _brushStyle), _penWidth, _penStyle));
		rectTopLeft = pressPoint;
		lastItem = rect;
		break;
	case RECT_FILL:
		rect = addRect(QRectF(pressPoint, pressPoint), QPen(QBrush(_borderColor, _brushStyle), _penWidth, _penStyle),
					   QBrush(_brushColor, _brushStyle));
		rectTopLeft = pressPoint;
		lastItem = rect;
		break;
	case RECT_ROUND:
		{
			QPainterPath path;
			path.addRoundedRect(QRectF(pressPoint, pressPoint), cornerRadius, cornerRadius);
			roundedRectPath = addPath(path, QPen(QBrush(_borderColor, _brushStyle), _penWidth, _penStyle));
			rectTopLeft = pressPoint;
			lastItem = roundedRectPath;
			break;
		}
	case RECT_ROUND_FILL:
		{
			QPainterPath path;
			path.addRoundedRect(QRectF(pressPoint, pressPoint), cornerRadius, cornerRadius);
			roundedRectPath = addPath(path, QPen(QBrush(_borderColor, _brushStyle), _penWidth, _penStyle), QBrush(_brushColor, _brushStyle));
			rectTopLeft = pressPoint;
			lastItem = roundedRectPath;
			break;
		}
	case CIRCLE:
		ellipse = addEllipse(QRectF(pressPoint, pressPoint), QPen(QBrush(_borderColor, _brushStyle), _penWidth, _penStyle));
		lastItem = ellipse;
		break;
	case CIRCLE_FILL:
		ellipse = addEllipse(QRectF(pressPoint, pressPoint), QPen(QBrush(_borderColor, _brushStyle), _penWidth, _penStyle),
							 QBrush(_brushColor, _brushStyle));
		lastItem = ellipse;
		break;
    default:
		isDrawing = false;
		break;
    }
}

void PaintScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (isDrawing)
    {
		emit sceneChanged();

		QPointF currentPoint = mouseEvent->scenePos();
		switch (_drawMode)
		{
		case LINE:
			line->setLine(QLineF(line->line().p1(), currentPoint));
			break;
		case ERASER:
			{
				_eraserWasUsed = true;
				QPainterPath path = eraserPath->path();
				path.lineTo(currentPoint);
				eraserPath->setPath(path);
			}
			break;
		case RECT: case RECT_FILL:
			rect->setRect(rectWithMovingPoint(currentPoint));
			break;
		case RECT_ROUND: case RECT_ROUND_FILL:
			{
				QPainterPath path;
				path.addRoundedRect(rectWithMovingPoint(currentPoint), cornerRadius, cornerRadius);
				roundedRectPath->setPath(path);
				break;
			}
		case CIRCLE: case CIRCLE_FILL:
			ellipse->setRect(QRectF(ellipse->rect().topLeft(), currentPoint));
			break;
		default:
			break;
		}
	}
}

void PaintScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	Q_UNUSED(mouseEvent);
	isDrawing = false;
	undoStack.push(new PaintCommand(lastItem));
	emit itemAdded();
}

void PaintScene::wheelEvent(QGraphicsSceneWheelEvent *e)
{
	if (e->modifiers() == Qt::ControlModifier)
	{
		qreal newScaleFactor = e->delta() < 0 ? scaleFactor : 1.0 / scaleFactor;
		views().at(0)->scale(newScaleFactor, newScaleFactor);
	}
}

void PaintScene::saveAsSvg(QString filePath)
{
	QSvgGenerator gen;
	gen.setFileName(filePath);
	gen.setSize(this->sceneRect().size().toSize());
	QPainter painter;
	painter.begin(&gen);
	this->render(&painter);
	painter.end();
}

void PaintScene::undo()
{
	undoStack.undo();
	emit sceneChanged();
}

void PaintScene::redo()
{
	undoStack.redo();
	emit sceneChanged();
}

void PaintScene::clear()
{
	undoStack.clear();
	QGraphicsScene::clear();
}
