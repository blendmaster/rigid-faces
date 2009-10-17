/*
    COP - Active Appearance Model Face Tracking - using Inverse Compositional Approach
    Rohan Anil , under Dr. Radhika Vatsan , BITS Pilani Goa Campus
    rohan.ani;@gmail.com
    License : GPLV3
*/
#include "cv.h"
#include "highgui.h"
#include "opencvWebcam.h"
#include "LKInverseCompClass.h"

#include <iostream>
#include <stdio.h>
CvPoint ptTop,ptBottom;
CvPoint piTop,piBottom;
CvPoint presetTop,presetBottom;


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
    opencvWebcam webcam;
    cvNamedWindow( "LK Inverse Compositional- Affine 6 Parameter", 0 );
    cvNamedWindow("template");

    cvNamedWindow("image");
    cvSetMouseCallback( "image", on_mouse, 0 );
    IplImage* pColorPhoto = 0;

    IplImage* pGrayPhoto = 0;
    IplImage* pImgT = 0;
    IplImage* pImgI = 0;

    // pColorPhoto = cvLoadImage("/home/rohananil/abc.jpg");

    // Create other images.


    //cvShowImage( "image",pColorPhoto  );





    LKInverseComp lk;


    CvPoint p1,p2,p3,p4;

    cvShowImage( "image",pColorPhoto  );



    int flag=0;
    if (webcam.startCamera()==1)
    {
        for (;;)
        {
            double t = (double)cvGetTickCount();
            double t1;

            while (t1<6000 &&flag==0)
            {
                pColorPhoto = webcam.queryFrame();
                cvShowImage( "image",pColorPhoto);
                cvWaitKey(11);
                cvReleaseImage(&pColorPhoto);
                t1 = ((double)cvGetTickCount() - t)/((double)cvGetTickFrequency()*1000.);
            }

            pColorPhoto = webcam.queryFrame();
            CvSize photo_size = cvSize(pColorPhoto->width,pColorPhoto->height);
            pGrayPhoto = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);

            cvCvtColor(pColorPhoto, pGrayPhoto, CV_RGB2GRAY);
            pImgI = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);
            cvCopy(pGrayPhoto, pImgI);
            cvShowImage( "image",pColorPhoto);
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
            piBottom.x=ptBottom.x;
            piBottom.y=ptBottom.y;
            piTop.x=ptTop.x;
            piTop.y=ptTop.y;
            if (flag==0)
            {
                pImgT = cvCreateImage(cvSize(ptBottom.x-ptTop.x,ptBottom.y-ptTop.y), IPL_DEPTH_8U, 1);
                cvSetImageROI(pGrayPhoto ,cvRect(ptTop.x,ptTop.y,ptBottom.x-ptTop.x,ptBottom.y-ptTop.y));
                cvResize(  pGrayPhoto, 	pImgT, CV_INTER_LINEAR );
                cvResetImageROI( pGrayPhoto);
                lk.setTemplate(pImgT);
                double ratX = double(piBottom.x-piTop.x)/double(ptBottom.x-ptTop.x);
                double ratY = double(piBottom.y-piTop.y)/double(ptBottom.y-ptTop.y);

                lk.setCurrentWarpEstimate(0,0,0,0,piTop.x,piTop.y);
                cvShowImage( "template",pImgT);


// cvRectangle(pColorPhoto, ptTop, ptBottom, CV_RGB(255,0,0), 3, 8, 0 );
//            cvRectangle(pColorPhoto, piTop, piBottom, CV_RGB(0,255,0), 3, 8, 0 );
                flag=1;
            }
            lk.setImage(pImgI);
            lk.iterate();



            p1.x= int(  CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p1.y=  int( CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            p2.x= int(  CV_MAT_ELEM(*lk.WarpMatrix, float,0, 1)*(ptBottom.y-ptTop.y) + CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p2.y=  int( CV_MAT_ELEM(*lk.WarpMatrix, float,1, 1)*(ptBottom.y-ptTop.y) +CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            p3.x= int( CV_MAT_ELEM(*lk.WarpMatrix, float,0, 1)*(ptBottom.y-ptTop.y)+ CV_MAT_ELEM(*lk.WarpMatrix, float,0, 0)*(ptBottom.x-ptTop.x) + CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p3.y=  int(CV_MAT_ELEM(*lk.WarpMatrix, float,1, 1)*(ptBottom.y-ptTop.y)+ CV_MAT_ELEM(*lk.WarpMatrix, float,1, 0)*(ptBottom.x-ptTop.x) +CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            p4.x= int(  CV_MAT_ELEM(*lk.WarpMatrix, float,0, 0)*(ptBottom.x-ptTop.x) + CV_MAT_ELEM(*lk.WarpMatrix, float,0, 2));
            p4.y=  int( CV_MAT_ELEM(*lk.WarpMatrix, float,1, 0)*(ptBottom.x-ptTop.x) +CV_MAT_ELEM(*lk.WarpMatrix, float,1, 2));
            cvLine(pColorPhoto,p1,p2, cvScalar(0,255,255), 1);
            cvLine(pColorPhoto,p2,p3, cvScalar(0,255,255), 1);
            cvLine(pColorPhoto,p3,p4, cvScalar(0,255,255), 1);
            cvLine(pColorPhoto,p4,p1, cvScalar(0,255,255), 1);
            cvLine(pColorPhoto,p4,p1, cvScalar(0,255,255), 1);
            cvShowImage( "image",pColorPhoto);
            cvWaitKey(3);

            cvReleaseImage(&pColorPhoto);
            cvReleaseImage(&pGrayPhoto);
            cvReleaseImage(&pImgI);
            // cvReleaseImage(&pImgT);


        }
    }
    cvDestroyWindow("template");
    cvDestroyWindow("image");


}
