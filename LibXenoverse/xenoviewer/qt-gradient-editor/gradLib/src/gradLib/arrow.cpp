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

// local includes
#include "arrow.h"

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
GradientArrow::GradientArrow()
{
   QPolygon area;
   area << QPoint(0,5) << QPoint(4, 0) << QPoint(8,5) << QPoint(8, 10) << QPoint(0,10) << QPoint(0,5);
   setArea(area);
}
GradientArrow::GradientArrow(const GradientArrow &other)
      :_hasFocus(other.hasFocus()),
      _index(other.index()),
      _area(other.area()),
      _color(other.color()),
      _pos(other.position()) 
{
}
//f+//////////////////////////////////////////////////////////////////////////
//
//  Name:	dtor
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
GradientArrow::~GradientArrow(){}