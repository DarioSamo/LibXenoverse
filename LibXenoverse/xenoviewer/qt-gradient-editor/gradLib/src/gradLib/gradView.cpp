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

// Qt includes
#include <QPainter>
#include <QMouseEvent>

#include <OgreTexture.h>
#include <OgreHardwarePixelBuffer.h>

// local includes
#include "gradView.h"

//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	ctor
/*!
//
//  \param     
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  20.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
CGradientColorsView::CGradientColorsView(QWidget *parent)
   :QWidget(parent),
   arrowMoving(false),
   backgroundVerSpace(5),
   backgroundHorSpace(5),
   background(NULL)
{
   qRegisterMetaType<GradientArrow>("GradientArrow");
   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
   setFocusPolicy(Qt::StrongFocus);
   viewRect = QRect(QPoint(0,0), QPoint(0,0));
   setMinimumSize(parent->width() - 10, 10);
   setMouseTracking(true);

   addArrow(QColor(0, 0, 0), 0.0);
   addArrow(QColor(0, 0, 0), 1.0);

   update();
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	dtor
/*!
//
//  \param  N/A
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  20.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
CGradientColorsView::~CGradientColorsView()
{
   delete background;
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	createBackground
/*!
//
//  \param  int vertLineSpace - default 3
//          int horLineSpace  - default 3
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  22.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::createBackground(int vertLineSpace, int horLineSpace)
{
   backgroundVerSpace = vertLineSpace;
   backgroundHorSpace = horLineSpace;
   if(NULL != background)
   {
      delete background;
      background = NULL;
   }
   background = new QPixmap(QSize(800,800));
   background->fill(Qt::white);
   QPainter painter(background);
   
   // draw vertical lines
   int x = 0;
   while(x < background->width())
   {
      const QPoint lineStart(x,0);
      const QPoint lineStop(x, background->height());
      painter.drawLine(lineStart, lineStop);
      x += vertLineSpace;
   }

   // draw horizontal lines
   int y = 0;
   while(y < background->height())
   {
      const QPoint lineStart(0, y);
      const QPoint lineStop (background->width(), y);
      painter.drawLine(lineStart, lineStop);
      y += horLineSpace;
   }
   painter.end();
   update();
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	getBackGround
/*!
//
//  \param  N/A
//
//  \return QPixmap* -
//    
//  \author Stian Broen 
//
//  \date  27.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
QPixmap* CGradientColorsView::getBackGround()
{
   if(NULL == background)
   {
      createBackground(backgroundVerSpace, backgroundHorSpace);
   }
   return background;
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	toggleBackground
/*!
//
//  \param  bool onOrOff -
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  23.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::toggleBackground(bool onOrOff)
{
   if(onOrOff == false)
   {
      if(NULL == background)
      {
         background = new QPixmap(viewRect.size());
      }
      background->fill(Qt::transparent);
      update();
   }
   else
   {
      createBackground(backgroundVerSpace, backgroundHorSpace);
   }
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	removeArrow
/*!
//
//  \param  int index -
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  23.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::removeArrow(int index)
{
   if(!m_arrows.contains(index) || m_arrows.size() <= 2)
   {
      return;
   }
   QMutableHashIterator<int, GradientArrow> it(m_arrows);
   while(it.hasNext())
   {
      it.next();
      if(it.key() == index)
      {
         it.remove();
         break;
      }
   }
   it.toBack();
   if(it.hasPrevious())
   {
      it.previous();
      GradientArrow newFocus = it.value();
      newFocus.setHasFocus(true);
      it.setValue(newFocus);
   }
   update();
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	clearArrows
/*!
//
//  \param  N/A
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  24.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::clearArrows()
{
   m_arrows.clear();
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	setFocusColor
/*!
//
//  \param  const QColor &col -
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  24.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::setFocusColor(const QColor &col)
{
   QMutableHashIterator<int, GradientArrow> it(m_arrows);
   while(it.hasNext())
   {
      it.next();
      GradientArrow anArrow = it.value();
      if(anArrow.hasFocus())
      {
         anArrow.setColor(col);
         it.setValue(anArrow);
      }
   }
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	paintEvent
/*!
//
//  \param  QPaintEvent * -
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  20.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::paintEvent(QPaintEvent *)
{
	if (viewRect.size().isNull() ||
		viewRect.size().isEmpty() ||
		viewRect.topLeft() == viewRect.bottomRight())
	{
		viewRect = QRect(QPoint(0, 0), QPoint(this->width() - 10, this->height() / 3 * 2 - 10));
		viewRect.translate(5, 5);
		createBackground();
	}
	QPainter painter(this);
	if (NULL != background)
	{
		painter.drawPixmap(viewRect, *background, viewRect);
	}
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.setRenderHint(QPainter::Antialiasing);

	QPoint gradStart = QPoint(viewRect.topLeft().x(), viewRect.bottomLeft().y() / 2);
	QPoint gradStop = QPoint(viewRect.topRight().x(), viewRect.bottomRight().y() / 2);
	QLinearGradient grad(gradStart, gradStop);

	QPen lastPen = painter.pen();
	QHashIterator<int, GradientArrow> it(m_arrows);
	while (it.hasNext())
	{

		it.next();
		GradientArrow m_arrow = it.value();
		grad.setColorAt(m_arrow.position(), m_arrow.color());
		QPolygon arrowPolygon = m_arrow.area();
		arrowPolygon.translate(1 + m_arrow.position() * (this->width() - 11), this->height() / 3 * 2);
		QPainterPath paintPath;
		paintPath.addPolygon(arrowPolygon);
		painter.setBrush(QBrush(m_arrow.color()));
		painter.drawPath(paintPath);
		painter.setBrush(QBrush(Qt::NoBrush));

		QPen boldPen;
		boldPen.setWidth(2);
		painter.setPen(boldPen);
		painter.drawPolygon(arrowPolygon);
		painter.setPen(lastPen);
	}

	QBrush brush(grad);
	painter.fillRect(viewRect, brush);
	painter.drawRect(viewRect);
	painter.end();
}



//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	addArrow
/*!
//
//  \param  const QColor &color -
//          qreal position      -
//          bool hasFocus       -
//
//  \return N/A
//
//  \author Stian Broen
//
//  \date  20.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::addArrow(const QColor &color, qreal position, bool hasFocus)
{
	GradientArrow arrow;
	arrow.setHasFocus(hasFocus);
	arrow.setColor(color);
	arrow.setPosition(position);

	if (arrow.hasFocus())
	{
		QMutableHashIterator<int, GradientArrow> it(m_arrows);
		while (it.hasNext())
		{
			it.next();
			GradientArrow anArrow = it.value();
			anArrow.setHasFocus(false);
			it.setValue(anArrow);
		}
	}

	int index = 0;
	while (m_arrows.contains(index))
	{
		index++;
	}
	arrow.setIndex(index);
	m_arrows.insert(arrow.index(), arrow);

	update();
}

//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	mousePressEvent
/*!
//
//  \param  QMouseEvent *event -
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  20.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::mousePressEvent(QMouseEvent *event)
{
	dragStart = event->pos();
	QMutableHashIterator<int, GradientArrow> it(m_arrows);
	it.toBack();
	while (it.hasPrevious())
	{
		it.previous();
		GradientArrow arrow = it.value();
		QPolygon poly = arrow.area();
		poly.translate(arrow.position() * (this->width() - 10), this->height() / 3 * 2);
		if (!arrowMoving && poly.containsPoint(dragStart, Qt::OddEvenFill))
		{
			if (event->button() == Qt::LeftButton) {
				arrowMoving = true;
			}

			arrow.setHasFocus(true);
			emit newFocusColor(arrow.color(), arrow.index());
		}
		else
		{
			arrow.setHasFocus(false);
		}
		it.setValue(arrow);
	}
	update();
}


void CGradientColorsView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QMutableHashIterator<int, GradientArrow> it(m_arrows);
		it.toBack();
		while (it.hasPrevious())
		{
			it.previous();
			GradientArrow arrow = it.value();
			if (arrow.hasFocus()) {
				QColor arrow_color = QColorDialog::getColor(arrow.color(), NULL, "Select Gradient Color");
				arrow.setColor(arrow_color);
				it.setValue(arrow);
				break;
			}
		}
		update();
	}
}

//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	mouseMoveEvent
/*!
//
//  \param  QMouseEvent *event -
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  20.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::mouseMoveEvent(QMouseEvent *event)
{
   if(!arrowMoving) return;
   QMutableHashIterator<int, GradientArrow> it(m_arrows);
   int count = 0;
   while(it.hasNext())
   {
      it.next();
      GradientArrow arrow = it.value();
      if(arrow.hasFocus())
      {
         count++;
         qreal dPos = (qreal)(event->pos().x() - dragStart.x())/((qreal)this->width() - 10); 
         if(arrow.position() + dPos > 1 || arrow.position() + dPos < 0)
         {
            return;
         }
         qreal lastPos = arrow.position();
         arrow.setPosition(lastPos + dPos);
         emit arrowMove(lastPos, arrow);
         it.setValue(arrow);
      }
   }
   dragStart = event->pos();
   update();
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	mouseReleaseEvent
/*!
//
//  \param  QMouseEvent * -
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  20.05.2012
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		arrowMoving = false;
	}

	if (event->button() == Qt::RightButton) {
		bool processed_arrow = false;

		QMutableHashIterator<int, GradientArrow> it(m_arrows);
		it.toBack();
		while (it.hasPrevious())
		{
			it.previous();
			GradientArrow arrow = it.value();
			if (arrow.hasFocus()) {
				contextMenuRemoveArrow(arrow, event);
				processed_arrow = true;
				break;
			}
		}

		if (!processed_arrow) {
			contextMenuAddArrow(event);
		}
	}
}


void CGradientColorsView::contextMenuAddArrow(QMouseEvent *event) {
	QMenu myMenu;
	myMenu.addAction("Add...");

	QAction* selectedItem = myMenu.exec(event->screenPos().toPoint());
	if (selectedItem) {
		QColor arrow_color = QColorDialog::getColor(QColor(255, 255, 255), NULL, "Select Gradient Color");
		addArrow(arrow_color, (qreal)(event->pos().x() / ((qreal)this->width() - 10)));
		update();
	}
	else {
	}
}

void CGradientColorsView::contextMenuRemoveArrow(GradientArrow arrow, QMouseEvent *event) {
	QMenu myMenu;
	myMenu.addAction("Delete");

	QAction* selectedItem = myMenu.exec(event->screenPos().toPoint());
	if (selectedItem) {
		if (m_arrows.size() > 2) {
			removeArrow(arrow.index());
		}
		update();
	}
	else {
	}
}


//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	updateView
/*!
//
//  \param  N/A
//
//  \return N/A
//    
//  \author Stian Broen 
//
//  \date  20.05.2013
//
*/
//f-//////////////////////////////////////////////////////////////////////////
void CGradientColorsView::updateView()
{
   update();
}

void CGradientColorsView::paintOgreTextureSection(uchar *image_data, GradientArrow arrow_start, GradientArrow arrow_end, int width, int y_offset) {
	QColor sc = arrow_start.color();
	QColor ec = arrow_end.color();

	int sr, sg, sb, er, eg, eb;
	sc.getRgb(&sr, &sg, &sb);
	ec.getRgb(&er, &eg, &eb);

	size_t x_start = arrow_start.position() * width;
	size_t x_end = arrow_end.position() * width;
	size_t x_length = x_end - x_start;
	if (x_end > width) x_end = width;

	for (size_t x = x_start; x < x_end; x++) {
		qreal factor = qreal(x - x_start) / (qreal)x_length;
		QColor interpolated_color;
		interpolated_color.setRgb(sr + (er - sr) * factor, sg + (eg - sg) * factor, sb + (eb - sb) * factor);
		interpolated_color = interpolated_color.toRgb();

		for (size_t y = y_offset; y < y_offset+4; y++) {
			image_data[y*width * 3 + x * 3] = interpolated_color.blue();
			image_data[y*width * 3 + x * 3 + 1] = interpolated_color.green();
			image_data[y*width * 3 + x * 3 + 2] = interpolated_color.red();
		}
	}
}

void CGradientColorsView::paintOgreTexture(uchar *image_data, int width, int y_offset) {
	GradientArrow lowest_arrow;
	GradientArrow highest_arrow;
	lowest_arrow.setPosition(2.0);
	highest_arrow.setPosition(-2.0);

	QMutableHashIterator<int, GradientArrow> it(m_arrows);
	int count = 0;
	while (it.hasNext())
	{
		it.next();

		GradientArrow arrow_start = it.value();
		GradientArrow winner_arrow;
		qreal winner_distance = 2.0;
		winner_arrow.setIndex(-1);

		if (arrow_start.position() < lowest_arrow.position()) {
			lowest_arrow = arrow_start;
		}

		if (arrow_start.position() > highest_arrow.position()) {
			highest_arrow = arrow_start;
		}

		// Search for next arrow
		QMutableHashIterator<int, GradientArrow> it_sub(m_arrows);
		while (it_sub.hasNext())
		{
			it_sub.next();
			GradientArrow arrow_end = it_sub.value();
			if ((arrow_start.index() != arrow_end.index()) && (arrow_end.position() >= arrow_start.position())) {
				qreal arrow_distance = arrow_end.position() - arrow_start.position();

				if (arrow_distance < winner_distance) {
					winner_distance = arrow_distance;
					winner_arrow = arrow_end;
				}
			}
		}

		if (winner_arrow.index() != -1) {
			paintOgreTextureSection(image_data, arrow_start, winner_arrow, width, y_offset);
		}
	}

	// Paint left and right sides if there's no arrows on them
	if (lowest_arrow.position() > 0.0) {
		GradientArrow first_arrow = lowest_arrow;
		first_arrow.setPosition(0.0);
		paintOgreTextureSection(image_data, first_arrow, lowest_arrow, width, y_offset);
	}

	if (highest_arrow.position() < 1.0) {
		GradientArrow last_arrow = highest_arrow;
		last_arrow.setPosition(1.0);
		paintOgreTextureSection(image_data, highest_arrow, last_arrow, width, y_offset);
	}
}