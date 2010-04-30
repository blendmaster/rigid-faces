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
#include "cv.h"
#include "highgui.h"
#include <canvas.h>
#include <landmarkpoint.h>

#include <QtGui/QGraphicsView>
#include <QKeyEvent>
#include <QMouseEvent>

#include <QApplication>
#include <list>
using namespace std;

canvas:: canvas(QWidget *parent):QGraphicsView(parent)
{
    move=false;
    mesh=new list< landmarkPoint* >;
}


void canvas :: mouseDoubleClickEvent(QMouseEvent *event)
{

    landmarkPoint* label = new landmarkPoint(this->parentWidget(),0);

    label->setGeometry(event->x()+x() -label->width()/2,event->y() +y() -label->height()/2,label->width(),label->height());
    mesh->push_front(label);

}
void   canvas::keyPressEvent ( QKeyEvent * event )
{
    switch (event->key())
    {

    case Qt::Key_Shift:
        move=true;
    }




}
void   canvas::keyReleaseEvent ( QKeyEvent * event )

{
    move=false;

}



void   canvas:: mouseMoveEvent (QMouseEvent * event)
{
    static int currentX=0;
    static int currentY=0;

    static int latch=0;

    if (move)
    {


        list< landmarkPoint*>::iterator it;
        if (latch==0)
        {
            currentX= event->globalX();
            currentY= event->globalY();

            latch=1;

        }
        else
        {

            for (it=mesh->begin(); it!=mesh->end(); ++it)
            {
                int x1=(*it)->x() - (currentX-event->globalX()) ;
                int y1=(*it)->y() - (currentY-event->globalY());
                (*it)->setGeometry(QRect(x1,y1, (*it)->width() ,(*it)->height()));

            }
            currentX= event->globalX();
            currentY= event->globalY();
        }

    }
    else
    {
        latch=0;
    }


}
