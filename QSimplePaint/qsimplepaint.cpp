#include <QtGui/QMessageBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QImageWriter>

#include <QtSvg/QGraphicsSvgItem>

#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>

#include "qsimplepaint.h"


QSimplePaint::QSimplePaint(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
    ui.setupUi(this);

    paintScene = new PaintScene;
    ui.graphicsView->setScene(paintScene);
//	ui.graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setCentralWidget(ui.graphicsView);

    lastFilePath = QString();

    QList<QAction *> figuresList = QList<QAction *>() << ui.actionLine << ui.actionEraser << ui.actionRectangle << ui.actionRectFill
                   << ui.actionRectRound << ui.actionRectRoundFill << ui.actionCircle << ui.actionCircleFill;
    QList<QAction *> widthsList = QList<QAction *>() << ui.actionLineWidth1 << ui.actionLineWidth2 << ui.actionLineWidth3 << ui.actionLineWidth4
                  << ui.actionLineWidth5;
    QList<QAction *> lineStylesList = QList<QAction *>() << ui.actionSolidLine << ui.actionDash << ui.actionDot << ui.actionDashDot
                      << ui.actionDashDotDot;
    QList<QAction *> brushStylesList = QList<QAction *>() << ui.actionSolidBrush << ui.actionExtremelyDense << ui.actionVeryDense
                       << ui.actionSomewhatDense << ui.actionHalfDense << ui.actionSomewhatSparse << ui.actionVerySparse
                       << ui.actionExtremelySparse << ui.actionHorizontalLines << ui.actionVerticalLines << ui.actionSquares
                       << ui.actionBackwardDiagonals << ui.actionForwardDiagonals << ui.actionCrossedDiagonals;
    groupActions(figuresList);
    groupActions(widthsList);
    groupActions(lineStylesList);
    groupActions(brushStylesList);

    paintToolbar = new QToolBar(tr("Paint Toolbar"), this);
    paintToolbar->setMovable(false);
    paintToolbar->addWidget(twoActionsInHorizontalLayout(ui.actionLine, ui.actionEraser));
    paintToolbar->addWidget(twoActionsInHorizontalLayout(ui.actionRectangle, ui.actionRectFill));
    paintToolbar->addWidget(twoActionsInHorizontalLayout(ui.actionRectRound, ui.actionRectRoundFill));
    paintToolbar->addWidget(twoActionsInHorizontalLayout(ui.actionCircle, ui.actionCircleFill));
    addToolBar(Qt::LeftToolBarArea, paintToolbar);

    connect(ui.menuView, SIGNAL(aboutToShow()), SLOT(updateToolbarsStateInMenu()));
    connect(paintScene, SIGNAL(sceneChanged(bool)), SLOT(setWindowModified(bool)));
    connect(paintScene, SIGNAL(sceneChanged(bool)), ui.actionSave, SLOT(setEnabled(bool)));
    connect(paintScene, SIGNAL(sceneChanged(bool)), ui.actionSaveAs, SLOT(setEnabled(bool)));

    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.actionAbout, SIGNAL(triggered()), SLOT(about()));
    connect(ui.actionPaintToolbar, SIGNAL(toggled(bool)), paintToolbar, SLOT(setVisible(bool)));
    connect(ui.actionDeleteAll, SIGNAL(triggered()), SLOT(deleteAll()));
    connect(ui.actionNew, SIGNAL(triggered()), SLOT(newImage()));
    connect(ui.actionOpen, SIGNAL(triggered()), SLOT(open()));
    connect(ui.actionSave, SIGNAL(triggered()), SLOT(save()));
    connect(ui.actionSaveAs, SIGNAL(triggered()), SLOT(saveAs()));
    connect(ui.actionPrint, SIGNAL(triggered()), SLOT(printImage()));
    connect(ui.actionBorderColor, SIGNAL(triggered()), SLOT(chooseBorderColor()));
    connect(ui.actionBrushColor, SIGNAL(triggered()), SLOT(chooseBrushColor()));
    connect(ui.actionZoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
    connect(ui.actionZoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
    connect(ui.actionZoomOriginal, SIGNAL(triggered()), SLOT(zoomOriginal()));
    connect(ui.actionUndo, SIGNAL(triggered()), paintScene, SLOT(undo()));
    connect(ui.actionRedo, SIGNAL(triggered()), paintScene, SLOT(redo()));

    QList<QAction *> noFillShapeActions = QList<QAction *>() << ui.actionLine << ui.actionEraser << ui.actionRectangle << ui.actionRectRound
                                          << ui.actionCircle;
    foreach (QAction *action, noFillShapeActions)
        connect(action, SIGNAL(toggled(bool)), ui.actionBrushColor, SLOT(setDisabled(bool)));
    connect(ui.actionEraser, SIGNAL(toggled(bool)), ui.actionBorderColor, SLOT(setDisabled(bool)));

    mapActionsToInt(figuresList, SLOT(setDrawMode(int)));
    mapActionsToInt(widthsList, SLOT(setPenWidth(int)));
    mapActionsToInt(lineStylesList, SLOT(setPenStyle(int)));
    mapActionsToInt(brushStylesList, SLOT(setBrushStyle(int)));

    QTimer::singleShot(0, this, SLOT(initSceneRect()));
}

void QSimplePaint::groupActions(const QList<QAction *> &actionList)
{
    QActionGroup *actionGroup = new QActionGroup(this);
    foreach (QAction *action, actionList)
        actionGroup->addAction(action);
}

QWidget *QSimplePaint::twoActionsInHorizontalLayout(QAction *leftAction, QAction *rightAction)
{
    QWidget *widgetWithHorizontalActions = new QWidget(this);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(widgetWithHorizontalActions);
    QToolButton *leftToolButton = new QToolButton(widgetWithHorizontalActions),
    *rightToolButton = new QToolButton(widgetWithHorizontalActions);
    leftToolButton->setDefaultAction(leftAction); rightToolButton->setDefaultAction(rightAction);
    horizontalLayout->addWidget(leftToolButton); horizontalLayout->addWidget(rightToolButton);
    return widgetWithHorizontalActions;
}

void QSimplePaint::updateToolbarsStateInMenu()
{
    ui.actionMainToolbar->setChecked(ui.mainToolBar->isVisible());
    ui.actionPaintToolbar->setChecked(paintToolbar->isVisible());
}

void QSimplePaint::mapActionsToInt(const QList<QAction *> &actionList, const char *connectionSlot)
{
    QSignalMapper *signalMapper = new QSignalMapper;
    int i = 1;
    foreach (QAction *action, actionList)
    {
        connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
        signalMapper->setMapping(action, i++);
    }
    connect(signalMapper, SIGNAL(mapped(int)), connectionSlot);
}

bool QSimplePaint::saved()
{
    if (isWindowModified())
    {
#ifdef Q_WS_MAC
        QMessageBox macMsgBox(this);
        macMsgBox.setText(tr("The image has been modified."));
        macMsgBox.setInformativeText(tr("Do you want to save your changes?"));
        macMsgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        macMsgBox.setDefaultButton(QMessageBox::Save);
        macMsgBox.setWindowModality(Qt::WindowModal);
        int answer = macMsgBox.exec();
#else
        int answer = QMessageBox::warning(this, QApplication::applicationName(), tr("The image has been modified.\nDo you want to save your changes?"),
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
#endif
        if (answer == QMessageBox::Save)
            save();
        else if (answer == QMessageBox::Cancel)
            return false;
        else
            paintScene->setEraserWasUsed(false);
    }
    return true;
}

void QSimplePaint::closeEvent(QCloseEvent *e)
{
    if (isWindowModified())
    {
        if (!saved())
        {
            e->ignore();
            return;
        }
    }
    e->accept();
}

void QSimplePaint::imageHasNoChanges(const QString &fileName)
{
    setWindowModified(false);
    ui.actionSave->setDisabled(true);
    if (fileName != QString::null && fileName != "")
        setWindowTitle(QString("%1 - %2[*]").arg(fileName).arg(QApplication::applicationName()));
}

void QSimplePaint::newImage()
{
    if (!saved())
        return;

    paintScene->clear();
    initSceneRect();
    ui.actionSaveAs->setDisabled(true);
    imageHasNoChanges(tr("untitled"));
}

void QSimplePaint::open()
{
    if (!saved())
        return;

    QString openedFile = QFileDialog::getOpenFileName(this, tr("Open image..."), lastFilePath,
                                                      tr("All supported images (*.svg *.png *.jpg *.bmp);;"
                                                         "SVG Image (*.svg);;PNG Image (*.png);;JPG Image (*.jpg);;BMP Image (*.bmp)"));
    if (!openedFile.isEmpty())
    {
        QString fileExtension = openedFile.right(3).toLower();
        if (!QFile(openedFile).open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, QApplication::applicationName(), tr("Unable to open image %1").arg(lastFilePath));
            return;
        }

        paintScene->clear();
        initSceneRect();

        if (fileExtension == "svg")
            paintScene->addItem(new QGraphicsSvgItem(openedFile));
        else
        {
            paintScene->addPixmap(QPixmap(openedFile));
            ui.graphicsView->setSceneRect(QRectF());
        }

        lastFilePath = openedFile;
        imageHasNoChanges(fileName(openedFile));
    }
}

bool QSimplePaint::saveRasterImage()
{
    QImage image(paintScene->width(), paintScene->height(), QImage::Format_ARGB32_Premultiplied);
    image.fill(QColor(Qt::white).rgb());
    QPainter painter(&image);
    if (painter.isActive())
    {
        painter.setRenderHint(QPainter::Antialiasing);
        paintScene->render(&painter);
        painter.end();
        image.save(lastFilePath);
        return true;
    }
    else
    {
        QMessageBox::critical(this, QApplication::applicationName(), tr("Unable to save image %1").arg(lastFilePath));
        return false;
    }
}

void QSimplePaint::save()
{
    if (isWindowModified())
    {
        if (lastFilePath.isEmpty())
            saveAs();
        else
        {
            if (lastFilePath.right(3) == "svg" && !paintScene->eraserWasUsed())
                paintScene->saveAsSvg(lastFilePath);
            else if (!saveRasterImage())
                return;

            imageHasNoChanges();
        }
    }
}

void QSimplePaint::saveAs()
{
    QString selectedFilter, supportedFileTypes = tr("PNG Image (*.png);;JPG Image (*.jpg);;BMP Image (*.bmp)");
    if (!paintScene->eraserWasUsed())
        supportedFileTypes = tr("SVG Image (*.svg);;") + supportedFileTypes;
    lastFilePath = QFileDialog::getSaveFileName(this, tr("Save file as..."), lastFilePath, supportedFileTypes, &selectedFilter);
    if (!lastFilePath.isEmpty())
    {
        selectedFilter = "." + selectedFilter.left(3).toLower();
        if (!lastFilePath.endsWith(selectedFilter, Qt::CaseInsensitive))
            lastFilePath += selectedFilter;
        if (selectedFilter == ".svg")
            paintScene->saveAsSvg(lastFilePath);
        else if (!saveRasterImage())
            return;

        imageHasNoChanges(fileName(lastFilePath));
    }
}

void QSimplePaint::printImage()
{
    QPrinter printer(QPrinter::HighResolution);
    if (QPrintDialog(&printer, this).exec() == QDialog::Accepted)
    {
        QPainter painter(&printer);
        if (painter.isActive())
        {
            painter.setRenderHint(QPainter::Antialiasing);
            paintScene->render(&painter);
        }
        else
            QMessageBox::critical(this, QApplication::applicationName(), tr("Unable to print image"));
    }
}

QString QSimplePaint::fileName(const QString &filePath)
{
    return QFileInfo(filePath).fileName();
}

void QSimplePaint::deleteAll()
{
    paintScene->clear();
    ui.actionSave->setDisabled(true);
    ui.actionSaveAs->setDisabled(true);
}

void QSimplePaint::zoomIn()
{
    ui.graphicsView->scale(scaleFactor, scaleFactor);
}

void QSimplePaint::zoomOut()
{
    ui.graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}

void QSimplePaint::zoomOriginal()
{
    ui.graphicsView->resetMatrix();
}

void QSimplePaint::chooseBorderColor()
{
    QColor selectedColor = QColorDialog::getColor(paintScene->borderColor(), this, tr("Border color"), QColorDialog::ShowAlphaChannel);
    if (selectedColor.isValid())
        paintScene->setBorderColor(selectedColor);
}

void QSimplePaint::chooseBrushColor()
{
    QColor selectedColor = QColorDialog::getColor(paintScene->brushColor(), this, tr("Brush color"), QColorDialog::ShowAlphaChannel);
    if (selectedColor.isValid())
        paintScene->setBrushColor(selectedColor);
}

void QSimplePaint::about()
{
#ifdef Q_WS_MAC
    QMessageBox::information
#else
    QMessageBox::about
#endif
            (this, tr("About %1").arg(QApplication::applicationName()),
                tr("%1 1.0\nAuthors: Filipenkov Andrey, Maschenko Elena & Ignatenko Maxim\nFaculty of Cybernetics")
                .arg(QApplication::applicationName()));
}

void QSimplePaint::initSceneRect()
{
    ui.graphicsView->setSceneRect(ui.graphicsView->rect());
    connect(paintScene, SIGNAL(itemAdded()), SLOT(updateSceneRect()));
}

#include <algorithm>
using std::min;
using std::max;

void QSimplePaint::updateSceneRect()
{
    QRectF viewRect = ui.graphicsView->rect();
    QRectF itemsRect = paintScene->itemsBoundingRect();
    QRectF sceneRect;
    sceneRect.setLeft(min(viewRect.left(),itemsRect.left()));
    sceneRect.setTop(min(viewRect.top(),itemsRect.top()));
    sceneRect.setRight(max(viewRect.right(),itemsRect.right()));
    sceneRect.setBottom(max(viewRect.bottom(),itemsRect.bottom()));
    ui.graphicsView->setSceneRect(sceneRect);
    if (sceneRect == itemsRect)
    {
        ui.graphicsView->setSceneRect(QRectF());
        disconnect(paintScene, SIGNAL(itemAdded()), this, SLOT(updateSceneRect()));
    }
}
