/*
    Copyright (C) 2009-2010 Rohan Anil (rohan.anil@gmail.com) , Dr. Radhika Vathsan
    BITS Pilani Goa Campus
    http://code.google.com/p/aam-opencv/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
/** @file */

#include "cv.h"
#include "highgui.h"
#include <landmarkpoint.h>

#ifndef _INCL_GUARD_CAN
#define _INCL_GUARD_CAN
#include <list>
#include <QtGui/QGraphicsView>
using namespace std;
/**
* Land Mark Points
*/
class canvas: public QGraphicsView
{
        Q_OBJECT

public:
canvas(QWidget *);
list<landmarkPoint *>  * mesh;
private:
bool move;
protected:
void   keyPressEvent ( QKeyEvent * event );
void   keyReleaseEvent ( QKeyEvent * event );
void  mouseMoveEvent (QMouseEvent * event);
void  mouseDoubleClickEvent(QMouseEvent *event);
};
#endif

