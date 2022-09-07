#ifndef PAINTCOMMAND_H
#define PAINTCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>

class PaintCommand : public QUndoCommand
{
public:
	PaintCommand(QGraphicsItem*);
	virtual void undo();
	virtual void redo();
private:
	QGraphicsItem* item;
};

#endif // PAINTCOMMAND_H
