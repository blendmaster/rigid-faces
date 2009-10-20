/*
    COP - Active Appearance Model Face Tracking - using Inverse Compositional Approach
    Rohan Anil , under Dr.Radhika Vathsan , BITS Pilani Goa Campus
    rohan.ani;@gmail.com
    License : GPLV3
*/
#include "cv.h"
#include "highgui.h"
#include "opencvWebcam.h"
#include "LKInverseCompRobustClass.h"
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <iostream>
#include <stdio.h>
CvPoint ptTop,ptBottom;
CvPoint piTop,piBottom;
CvPoint presetTop,presetBottom;
IplImage* camImage;
IplImage* grayIterateImage = 0;
IplImage* templateImage = 0;
IplImage* iterateImage = 0;
IplImage* backGroundSubtraction = 0;

int flag=0;
int templateLatch=0;

void on_mouse( int event, int x, int y, int flags, void* param )
{

    if ( event == CV_EVENT_LBUTTONDOWN )
    {
        if (templateLatch>=2 && templateLatch<4)
        {

            if (templateLatch==2)
                piTop = cvPoint(x,y);
            else
                piBottom = cvPoint(x,y);
            templateLatch++;

        }
        else if (templateLatch>=4)
        {

            if (templateLatch%2==0)
                presetTop = cvPoint(x,y);
            else
                presetBottom = cvPoint(x,y);
            templateLatch++;

        }
        else
        {
            if (templateLatch==0)
                ptTop = cvPoint(x,y);
            else
                ptBottom = cvPoint(x,y);
            templateLatch++;


        }
    }
}
int main(int argc, char *argv[])
{

    /////
/*
    CvVideoWriter *writer = 0;
int isColor = 1;
int fps     = 25;  // or 30
int frameW  = 640; // 744 for firewire cameras
int frameH  = 480; // 480 for firewire cameras
writer=cvCreateVideoWriter("out.avi",-1,
                     fps,cvSize(frameW,frameH),isColor);
 */
    /////
    opencvWebcam webcam;
    cvNamedWindow("template");

    cvNamedWindow("image");
    cvSetMouseCallback( "image", on_mouse, 0 );

int counter=0;

    LKInverseComp lk;


    CvPoint p1,p2,p3,p4;

    cvShowImage( "image",camImage  );
    Display *dpy = XOpenDisplay (0);



    int flag=0;
    if (webcam.startCamera()==1)
    {
        for (;;)
        {
            double t = (double)cvGetTickCount();
            double t1;
  //backGroundSubtraction = webcam.queryFrame();
                cvShowImage( "template",camImage);
            while (t1<1000 &&flag==0)
            {
              camImage = webcam.queryFrame();
                cvShowImage( "image",camImage);
                cvWaitKey(11);
                cvReleaseImage(&camImage);
                t1 = ((double)cvGetTickCount() - t)/((double)cvGetTickFrequency()*1000.);
            }

            camImage = webcam.queryFrame();
            if(camImage==0)
            { //cvReleaseVideoWriter(&writer);
            break; }
          //  cvSub(camImage,backGroundSubtraction,camImage);
            CvSize photo_size = cvSize(camImage->width,camImage->height);
            grayIterateImage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);

            cvCvtColor(camImage, grayIterateImage, CV_RGB2GRAY);
            iterateImage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);
            cvCopy(grayIterateImage, iterateImage);
            cvShowImage( "image",camImage);
            while (templateLatch<4)
            {
                cvWaitKey(1);
            }
            if (templateLatch>4)
            {
                if (templateLatch%2==0)
                {

                    lk.setCurrentWarpEstimate(0,0,0,0,presetTop.x,presetTop.y);


                }
            }
            if((ptBottom.x-ptTop.x)%4!=0)
            ptBottom.x+=(4-(ptBottom.x-ptTop.x)%4);

            if((ptBottom.y-ptTop.y)%4!=0)
            ptBottom.y+=(4-(ptBottom.y-ptTop.y)%4);



            piBottom.x=ptBottom.x;
            piBottom.y=ptBottom.y;
            piTop.x=ptTop.x;
            piTop.y=ptTop.y;
            if (flag==0)
            {
                templateImage = cvCreateImage(cvSize(ptBottom.x-ptTop.x,ptBottom.y-ptTop.y), IPL_DEPTH_8U, 1);
                cvSetImageROI(grayIterateImage ,cvRect(ptTop.x,ptTop.y,ptBottom.x-ptTop.x,ptBottom.y-ptTop.y));
                cvResize(  grayIterateImage, 	templateImage, CV_INTER_LINEAR );
                cvResetImageROI( grayIterateImage);
                lk.setTemplate(templateImage);
                double ratX = double(piBottom.x-piTop.x)/double(ptBottom.x-ptTop.x);
                double ratY = double(piBottom.y-piTop.y)/double(ptBottom.y-ptTop.y);

                lk.setCurrentWarpEstimate(0,0,0,0,piTop.x,piTop.y);
                cvShowImage( "template",templateImage);
                //cvSaveImage("/home/rohananil/COP/images/template.jpg",templateImage);


                flag=1;
            }
            lk.setImage(iterateImage);
            lk.iterate();



            p1.x= int(  CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p1.y=  int( CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            p2.x= int(  CV_MAT_ELEM(*lk.WarpMatrix, float,0, 1)*(ptBottom.y-ptTop.y) + CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p2.y=  int( CV_MAT_ELEM(*lk.WarpMatrix, float,1, 1)*(ptBottom.y-ptTop.y) +CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            p3.x= int( CV_MAT_ELEM(*lk.WarpMatrix, float,0, 1)*(ptBottom.y-ptTop.y)+ CV_MAT_ELEM(*lk.WarpMatrix, float,0, 0)*(ptBottom.x-ptTop.x) + CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p3.y=  int(CV_MAT_ELEM(*lk.WarpMatrix, float,1, 1)*(ptBottom.y-ptTop.y)+ CV_MAT_ELEM(*lk.WarpMatrix, float,1, 0)*(ptBottom.x-ptTop.x) +CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            p4.x= int(  CV_MAT_ELEM(*lk.WarpMatrix, float,0, 0)*(ptBottom.x-ptTop.x) + CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p4.y=  int( CV_MAT_ELEM(*lk.WarpMatrix, float,1, 0)*(ptBottom.x-ptTop.x) +CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            cvLine(camImage,p1,p2, cvScalar(0,255,255), 2);
            cvLine(camImage,p2,p3, cvScalar(255,255,0), 2);
            cvLine(camImage,p3,p4, cvScalar(255,0,255), 2);
            cvLine(camImage,p4,p1, cvScalar(0,0,255), 2);
            cvShowImage( "image",camImage);

double rat1=(  CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2))/IMAGE_WIDTH;
double rat2=(  CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2))/IMAGE_HEIGHT;
rat1*=1680;
rat2*=1050 ;
printf("%e %e \n",CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2),CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));

                            XTestFakeMotionEvent (dpy, DefaultScreen (dpy),int(rat1) ,int( rat2), 0);
                XFlush (dpy);

           // char a[300];
           // sprintf(a,"/home/rohananil/COP/images/%06d.jpg",counter++);
           //     cvSaveImage(a,camImage);

            //  cvWriteFrame(writer,camImage);      // add the frame to the file
            cvWaitKey(2);

            cvReleaseImage(&camImage);
            cvReleaseImage(&grayIterateImage);
            cvReleaseImage(&iterateImage);
            // cvReleaseImage(&templateImage);


        }
    }
    cvDestroyWindow("template");
    cvDestroyWindow("image");


}
