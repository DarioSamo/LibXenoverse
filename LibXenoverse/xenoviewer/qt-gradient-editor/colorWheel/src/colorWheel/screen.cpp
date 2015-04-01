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

#include <QApplication>
#include <QDesktopWidget>
#include "screen.h"

screen::screen()
{
    label = new QLabel(this);
    label->setPixmap(QPixmap::grabWindow(QApplication::desktop()->winId()));
    label->move(0,0);

    showFullScreen();
}


void screen::mousePressEvent(QMouseEvent *event)
{
    emit colorSelected( label->pixmap()->toImage().pixel( event->pos() ) );
    close();
}
