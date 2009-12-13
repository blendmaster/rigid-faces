/** @file */

#include "cv.h"
#include <QtGui>
#include "highgui.h"
#ifndef _INCL_QTUTILS
#define _INCL_QTUTILS
/**
* Convert IplImage to QImage
*@param input Input Image
*@result Output QImage
*/
QImage *QImageIplImageCvt(IplImage *input);
#endif
