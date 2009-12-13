/** @file */

#include "cv.h"
#include "highgui.h"
#include <QtGui/QLabel>

#ifndef _INCL_GUARD_LAND
#define _INCL_GUARD_LAND

/**
* Land Mark Points
*/
class landmarkPoint:public QLabel
{
        Q_OBJECT

public:
landmarkPoint(QWidget * parent, Qt::WindowFlags f );
static int count;

private:
int widthVal,heightVal;

bool move;
protected:
 void mouseMoveEvent ( QMouseEvent * event );

 void mousePressEvent ( QMouseEvent * event );

 void mouseReleaseEvent ( QMouseEvent * event );
};
#endif

