#include "cv.h"
#include "highgui.h"

#ifndef _INCL_GUARD_WEBCAM
#define _INCL_GUARD_WEBCAM
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

 class opencvWebcam
    {
    public:
        opencvWebcam(void);
	IplImage *queryFrame();
int startCamera();
void stopCamera();
    private:
	CvCapture* capture;

    };
#endif
