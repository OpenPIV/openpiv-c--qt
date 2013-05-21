/*
====================================================================================

File: pivdisplay.h
Description: This class inherits QGraphicsView and is used to show the image and,
    eventully, the PIV data.
Copyright (C) 2010  OpenPIV (http://www.openpiv.net)

Contributors to this code:
Zachary Taylor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

====================================================================================
*/

#ifndef PIVDISPLAY_H
#define PIVDISPLAY_H

#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>

#include "imagepaint.h"
#include "imagedata.h"
#include "settings.h"
#include "itempaint.h"
#include "datacontainer.h"
#include "pivdata.h"

//! This class to show the image and,eventully, the PIV data.
/*!
    This class inherits QGraphicsView.  The display allows for user interaction (zoom, pan)
    with the images.  Once the velocity data are computed it also allows for layering of the
    vectors.
*/

class PivDisplay : public QGraphicsView
{

Q_OBJECT

public:
    //! Constructor
    PivDisplay(QWidget* parent = 0);
    //! Destructor
    virtual ~PivDisplay();

    //! Returns the scene-mapped point of the cursor when the mouse is clicked
    QPointF getPoint1();
    //! Returns the scene-mapped point of the cursor when the mouse is released
    QPointF getPoint2();

    //! Receives a pointer to the global settings object
    void setSettings(Settings *settingsPass);
    //! Receives a pointer to the global data container
    void setData(DataContainer *filedataPass);

signals:
    //! Signal emitted when the mouse is moved
    void mouseMoved(QPointF pointPass);

public slots:
    //! Refreshs what is currently drawn
    void refreshView();
    //! Displays the current image pair (selection is held in the DataContainer object)
    void displayCurrent();
    //! Handles showing/hiding of the mask in the display
    void maskToggled(bool isTrue);
    //! Handles showing/hiding of the vectors in the display
    void vectorsToggled(bool isTrue);

    //! Provides the zoom in function by zooming in in 5% increments
    void zoomIn();
    //! Provides the zoom out function by zooming out in 5% increments
    void zoomOut();
    //! Fits the scene to the size of the display area
    void zoomFit();
    //! Notifies the display object of a change to the displayed vectors
    void vectorsChanged();

protected:
    void resize();

    //! Wheel event handler inherited from QGraphicsView
    void wheelEvent(QWheelEvent* event);
    //! Mouse press event handler inherited from QGraphicsView
    void mousePressEvent(QMouseEvent* event);
    //! Mouse release event handler inherited from QGraphicsView
    void mouseReleaseEvent(QMouseEvent* event);
    //! Mouse move event handler inherited from QGraphicsView
    void mouseMoveEvent(QMouseEvent* event);
    //! Resize event handler inherited from QGraphicsView
    void resizeEvent(QResizeEvent* event);

    //! Draws the scene layer by layer
    void drawLayers();
    //! Draws the mask image
    void drawMaskImage();
    //! Draws the PIV image
    void drawImage(QImage imagePass);

private:
    QPointF point1;
    QPointF point2;

    bool mouseIsPressed;

    // PIV display image
    QImage *image;
    // Mask display image
    QImage *maskImage;

    // The scene of the viewer
    QGraphicsScene *_scene;
    // The image painter for PIV images
    ImagePaint *imPaint;
    // The image painter for the mask image
    ImagePaint *maskPaint;
    // Item painter (grid points/vectors)
    ItemPaint *itemPaint;

    bool imPaintAdded;
    bool maskPaintAdded;

    // Scene integer rectangle
    QRect *rect;
    // Scene float rectangle
    QRectF *rectF;
    // View foat rectangle
    QRectF *viewRectF;
    bool viewRectFSet;
    // Central point of the viewer
    QPointF centerPoint;

    // Image data for the current image (QImage is required for display)
    ImageData *currentImage;
    QString currentFileName;
    bool currentImageCreated;

    // Boolean for if the mask is active
    bool maskActive;
    // Boolean for if the vectors are active
    bool vectorsActive;
    // Boolean for if the vectors are drawn
    bool vectorsDrawn;

    // Pointer to the global settings object
    Settings *settings;
    // Pointer to the displayed PivData object
    PivData *pivData;
    // Pointer to the global DataContainer object
    DataContainer *filedata;

};
#endif // PIVDISPLAY_H
