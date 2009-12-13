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

