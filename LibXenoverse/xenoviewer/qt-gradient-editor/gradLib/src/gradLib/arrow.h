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

#ifndef _GRAD_ARROW_H
#define _GRAD_ARROW_H

// Qt includes
#include <QPolygon>
#include <QColor>

class GradientArrow
{
public:
   explicit GradientArrow();
   GradientArrow(const GradientArrow &other);
   ~GradientArrow();

   // read
   inline bool hasFocus() const { return _hasFocus; }
   inline int index() const { return _index; }
   inline const QPolygon area() const { return _area; }
   inline const QColor color() const { return _color; }
   inline qreal position() const { return _pos; }

   // write
   inline void setHasFocus(bool val) { _hasFocus = val; }
   inline void setIndex(int val) { _index = val; }
   inline void setArea(const QPolygon &val) { _area = val; }
   inline void setColor(const QColor &val) { _color = val; }
   inline void setPosition(qreal val) { _pos = val; }

private:
   bool _hasFocus;
   int _index;
   QPolygon _area;
   QColor _color;
   qreal _pos;
};

#endif