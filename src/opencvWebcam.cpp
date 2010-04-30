/** @file */

/*
    OpenCV Webcam CLASS
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://code.google.com/p/pam-face-authentication/

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include "opencvWebcam.h"
#include "aam.h"


static IplImage * orginalFrame=0;
IplImage * frame=0;
IplImage * frame_copy=0;

opencvWebcam::opencvWebcam()
{

}
void opencvWebcam::stopCamera()
{
   if(capture!=0)
   cvReleaseCapture( &capture );
}
int opencvWebcam::startCamera()
{
    capture =cvCaptureFromAVI("/home/rohananil/Videos/Webcam/2010-01-23-185034.ogv");
    //capture =cvCaptureFromCAM(0);
    if(capture==0)
    return 0;
    else
    return 1;
}
IplImage *opencvWebcam::queryFrame()
{

orginalFrame = cvQueryFrame( capture );
    if (orginalFrame==NULL) return 0;
    frame = cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U, orginalFrame->nChannels );
    cvResize(orginalFrame,frame, CV_INTER_LINEAR);
    if ( !frame )
        return 0;

    frame_copy = cvCreateImage( cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );
    if ( frame->origin == IPL_ORIGIN_TL )
    cvCopy( frame, frame_copy, 0 );
   else
     cvFlip( frame, frame_copy, 0 );
  cvReleaseImage(&frame);

//static int latch=4;
//if(latch==0)
//latch=4;
//
//IplImage *img;
//
//switch(latch)
//{
//    case 4:
//    img =cvLoadImage("./images/01-4m.jpg",1);
//
//    break;
//    case 3:
//    img =cvLoadImage("./images/01-3m.jpg",1);
//
//    break;
//    case 2:
//    img =cvLoadImage("./images/01-2m.jpg",1);
//
//    break;
//    case 1:
//    img =cvLoadImage("./images/01-1m.jpg",1);
//
//    break;
//}
//
//latch--;
//    img =cvLoadImage("./images/110017161411340.jpg",1);
//
//
//return img;

return frame_copy;


}

