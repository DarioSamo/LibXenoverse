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

#include "colorpickerwidget.h"
#include <QMouseEvent>

ColorPickerWidget::ColorPickerWidget(QWidget *parent)
    : QWidget(parent)
{
    ecran=0;
    colorTriangle = new QtColorTriangle(this);

    connect(colorTriangle, SIGNAL(colorChanged(QColor)), this, SLOT(colorChgd()));

    colorView = new ColorViewer(this);
    colorView->setColor(colorTriangle->color());
        connect(colorTriangle, SIGNAL(colorChanged(QColor)), colorView, SLOT(changeColor(QColor)));
    connect(colorView, SIGNAL(colorChanged(const QColor &)), this, SLOT(setColor(const QColor &)));

    //pickColor = new QPushButton(tr("Pick a color"), this);
    //    connect(pickColor, SIGNAL(released()), this, SLOT(pickMode()));

    layout = new QGridLayout;
    layout->addWidget(  colorTriangle,  0, 0, 3, 1);
    layout->addWidget(  colorView,      0, 1, 3, 1);
    //layout->addWidget(  pickColor,      2, 2, 1, 1);

    setLayout(layout);
}
void ColorPickerWidget::closeColorDialog()
{
   colorView->closeMenu();
}
QColor ColorPickerWidget::getColor()
{
   return colorView->color();
}
void ColorPickerWidget::resizeEvent(QResizeEvent * event)
{
   colorView->setMinimumHeight(event->size().height() - 22);
   colorView->setMaximumHeight(event->size().height() - 22);
}
ColorPickerWidget::~ColorPickerWidget()
{
}
void ColorPickerWidget::pickMode()
{
    if(ecran!=0)
        delete ecran;
    ecran = new screen;
        connect( ecran, SIGNAL(colorSelected(QColor)), colorTriangle, SLOT(setColor(QColor)) );
}

void ColorPickerWidget::setColor(const QColor &col)
{
    colorView->closeMenu();
    colorView->setColor(col);
    colorTriangle->setColor(col);
}


void ColorPickerWidget::colorChgd()
{
    emit colorChanged(colorView->color());
}
