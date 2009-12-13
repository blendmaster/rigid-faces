/*
    COP - Active Appearance Model Face Tracking - using Inverse Compositional Approach
    Rohan Anil , under Dr.Radhika Vathsan , BITS Pilani Goa Campus
    rohan.ani;@gmail.com
    License : GPLV3
*/
#include "cv.h"
#include "highgui.h"
#include "opencvWebcam.h"
#include "lkFitting.h"

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

    LKInverseComp lk("./aam.template");
    lk.setParamtxty(150,115);

    cvShowImage( "image",camImage  );



    int flag=0;
if(webcam.startCamera()==1)
{
        for (;;)
        {
            double t = (double)cvGetTickCount();
            double t1=0;
  //backGroundSubtraction = webcam.queryFrame();
            while (t1<111 )
            {
              camImage = webcam.queryFrame();
                cvShowImage( "image",camImage);
               // cvWaitKey(1);
                cvReleaseImage(&camImage);
                t1 = ((double)cvGetTickCount() - t)/((double)cvGetTickFrequency()*1000.);
            }

            camImage = webcam.queryFrame();
            CvSize photo_size = cvSize(camImage->width,camImage->height);
            grayIterateImage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);

            cvCvtColor(camImage, grayIterateImage, CV_RGB2GRAY);
            iterateImage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);
            cvCopy(grayIterateImage, iterateImage);
            //cvShowImage( "image",camImage);

            CvMat* newMat =lk.computeShape(lk.param);
            //for(int i=0;i<8;i++)
           // printf("%d %e \n",i,lk.param[i]);
            for(int i=0;i<40;i++)
            {

            CvScalar s1,s2;
            s1 =cvGet2D(newMat,0,2*i);
            s2 =cvGet2D(newMat,0,2*i + 1);
          //  printf(" %e %e \n",s1.val[0],s2.val[0]);

            cvCircle( camImage, cvPoint((int)s1.val[0],(int)s2.val[0]), 1, CV_RGB(i*4,(255-i*4),255), 2 );



            }
            lk.setImage(iterateImage);
           // printf("ITERATE STARTED \n");
            lk.iterate();


            cvShowImage( "image",camImage);

           // char a[300];
           // sprintf(a,"/home/rohananil/COP/images/%06d.jpg",counter++);
           //     cvSaveImage(a,camImage);

            //  cvWriteFrame(writer,camImage);      // add the frame to the file
           cvWaitKey(1);
            cvReleaseImage(&camImage);
            cvReleaseImage(&grayIterateImage);
            cvReleaseImage(&iterateImage);
            // cvReleaseImage(&templateImage);
cvReleaseMat(&newMat);


        }
    }
    cvDestroyWindow("template");
    cvDestroyWindow("image");


}
