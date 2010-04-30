/** @file */

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

#include <math.h>
#include <float.h>
#include <limits.h>
#include "cv.h"
#include <QtGui>
#include "highgui.h"
#include <cxcore.h>
#include <stdio.h>
#include "qtUtils.h"
#include "aam.h"

QImage *QImageIplImageCvt(IplImage *input)
{
    if (!input)
        return 0;

    QImage *image = new QImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB32);
    QRgb value;
    uchar* pBits         = image->bits();
    int nBytesPerLine    = image->bytesPerLine();

    int n,m;
    for (n=0;n<IMAGE_HEIGHT;n++)
    {
        for (m= 0;m<IMAGE_WIDTH;m++)
        {
            CvScalar s;
            s=cvGet2D(input,n,m);
            QRgb value;

            value = qRgb((uchar)s.val[2], (uchar)s.val[1], (uchar)s.val[0]);

//            //printf("%d %d %d \n",(uchar)s.val[2], (uchar)s.val[1], (uchar)s.val[0]);
            uchar * scanLine = pBits+n*nBytesPerLine;
            ((uint *)scanLine)[m] = value;
//image->setPixel(m, n, value);
        }
    }




    return image;
}
