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
