/*

Copyright (C) 2009, Etienne Moutot <e.moutot@gmail.com>

This file is part of colorPickerWidget.

    colorPickerWidget is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    colorPickerWidget is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "colorviewer.h"
#include <QColorDialog>
#include <QMenu>
#include <QWidgetAction>

ColorViewer::ColorViewer(QWidget *parent)
    : QPushButton(parent)
{
   setMinimumSize(10, 10);

   
}

ColorViewer::~ColorViewer()
{
}
void ColorViewer::closeMenu()
{
   if(NULL != this->menu())
   {
      this->menu()->close();
   }
}
void ColorViewer::mousePressEvent(QMouseEvent *event)
{
   QWidgetAction *colorDialogAction = new QWidgetAction(this);
   colorDialog = new QColorDialog(this);
   colorDialog->setCurrentColor(actualColor);
   colorDialog->setOption(QColorDialog::ShowAlphaChannel);
   connect(colorDialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(changeColor(const QColor &)));
   connect(colorDialog, SIGNAL(finished(int)), this, SLOT(catchCancel(int)));
   colorDialogAction->setDefaultWidget(colorDialog);
   QMenu *colorMenu = new QMenu(this);
   colorMenu->addAction(colorDialogAction);
   this->setMenu(colorMenu);
   this->menu()->show();
}
void ColorViewer::catchCancel(int)
{
   if(NULL != this->menu())
    {
       this->menu()->close();
    }
}
void ColorViewer::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(actualPen);
    p.setBrush(QBrush(actualColor));
    p.drawRect( QRect( 2, 2, width()-4, height()-4 ) );
}

void ColorViewer::setPen(const QPen &pen)
{
    actualPen = pen;
}

QPen ColorViewer::pen() const
{
    return actualPen;
}

void ColorViewer::setColor(const QColor &color)
{
    actualColor = color;
}
QColor ColorViewer::color() const
{
    return actualColor;
}


void ColorViewer::changeColor(const QColor &color)
{
    actualColor = color;
    emit colorChanged(actualColor);
    if(NULL != this->menu())
    {
       this->menu()->close();
    }
    update();
}
