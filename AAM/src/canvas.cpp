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



    printf("%d \n",event->key());

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
                printf("%d %d \n",x1,y1);
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
