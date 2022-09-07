#include "paintcommand.h"

PaintCommand::PaintCommand(QGraphicsItem* _item)
{
	item = _item;
}

void PaintCommand::undo()
{
	item->hide();
}

void PaintCommand::redo()
{
	item->show();
}
