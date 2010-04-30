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
#ifndef _INCL_GUARD_WEBCAM
#define _INCL_GUARD_WEBCAM
/**
* OpenCV Webcam Class. This class wraps over the opencv Image Query Functions.
*/
class opencvWebcam
{
public:
    /**
    *The Constructor
    *Currently does nothing
    */
    opencvWebcam(void);
    /**
    *Query Image From Webcam
    *@result Image From the Webcam
    */
    IplImage *queryFrame();
    /**
    *Initialize Webcam for Querying Image
    *@result returns 1 for Sucess and 0 for Failure
    */
    int startCamera();
    /**
    *Deinitialize Webcam Structures
    */
    void stopCamera();

private:
    /**
    *Opencv Capture Structure
    */
    CvCapture* capture;

};
#endif
