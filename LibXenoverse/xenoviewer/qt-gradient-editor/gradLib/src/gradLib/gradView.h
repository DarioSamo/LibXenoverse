/****************************************************************************/
// This file is part of the gradLib library originally made by Stian Broen
//
// For more free libraries, please visit <http://broentech.no>
//
// gradLib is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this library.  If not, see <http://www.gnu.org/licenses/>
/****************************************************************************/

#ifndef _GRAD_VIEW_H
#define _GRAD_VIEW_H

// Qt includes
#include <QWidget>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QHash>

// solution includes
#include "arrow.h"

// Qt forward declarations
class QPaintEvent;
class QMouseEvent;
class QPixmap;

class CGradientColorsView : public QWidget
{
   Q_OBJECT

public:
   explicit CGradientColorsView(QWidget *parent = NULL);
   ~CGradientColorsView();
   void toggleBackground(bool onOrOff);
   void createBackground(int vertLineSpace = 5, int horLineSpace = 5);
   void setFocusColor(const QColor &col);
   void removeArrow(int index);
   void clearArrows();
   QPixmap *getBackGround();
   void addArrow(const QColor &color, qreal position = 0.5, bool hasFocus = true);
   void paintOgreTextureSection(uchar *image_data, GradientArrow arrow_start, GradientArrow arrow_end, int width, int y_offset);
   void paintOgreTexture(uchar *image_data, int width, int y_offset);
protected:
   void paintEvent       (QPaintEvent *);
   void mousePressEvent  (QMouseEvent *);
   void mouseDoubleClickEvent (QMouseEvent *);
   void mouseMoveEvent   (QMouseEvent *);
   void mouseReleaseEvent(QMouseEvent *);
   void contextMenuAddArrow(QMouseEvent *);
   void contextMenuRemoveArrow(GradientArrow arrow, QMouseEvent *);
public slots:
   void updateView();
signals:
   void arrowMove(qreal lastPos, const GradientArrow &arrow);
   void newFocusColor(const QColor &col, int index);

private:
   bool arrowMoving;
   int backgroundVerSpace;
   int backgroundHorSpace;
   QRect viewRect;
   QSize viewSize;
   QPixmap *background;
   QPoint dragStart;
   QHash<int, GradientArrow> m_arrows;
};

#endif