
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
#include "cv.h"
#include "highgui.h"
#include "opencvWebcam.h"
#include "lkFitting.h"
#include<aam.h>
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
double Lagrangian(int n, int k, double t)
{
    double l=1.0, tj, tk;
    tk=k/(n-1.0);

    for (int j=0;j<n;j++)
    {
        if (j!=k)

        {
            tj=j/(n-1.0);
            l*=(t-tj)/(tk-tj);


        }

    }
    return l;

}
void drawCRSpline(IplImage* img,CvPoint* p,int num,int thickness=1)
{
    for (float t=0;t<1;t=t+.001)
    {
        double x=0;
        double y=0;
        for (int i=0;i<num;i++)
        {
            x+=Lagrangian(num,i,t)*p[i].x;
            y+=Lagrangian(num,i,t)*p[i].y;


        }
        cvCircle(img, cvPoint(x,y), thickness, cvScalar(0,0,0), 1);
    }
}
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
    int frameW  = 6totalnumberofpoints; // 744 for firewire cameras
    int frameH  = 480; // 480 for firewire cameras
    writer=cvCreateVideoWriter("out.avi",-1,
                         fps,cvSize(frameW,frameH),isColor);
     */
    /////
    opencvWebcam webcam;
    cvNamedWindow("template");
    cvNamedWindow("carimage");

    cvNamedWindow("image");
    cvSetMouseCallback( "image", on_mouse, 0 );

    int counter=0;

    LKInverseComp lk("./aam.template");
    lk.setParamtxty(175,140);

    cvShowImage( "image",camImage  );


    static CvMat* newMat=cvCreateMat(1,totalnumberofpoints,CV_64FC1);
    int frameIndex=0;
    int minFrameIndex=0;
    int stepSize=1;
    double min=0;
    CvVideoWriter *writer = 0;
    int isColor = 1;
    int fps     = 25;  // or 30
    int frameW  = 1280;// 744 for firewire cameras
    int frameH  = 480; // for firewire cameras
    writer=cvCreateVideoWriter("out1.mpg",-1,
                               fps,cvSize(frameW,frameH),isColor);
    int flag=0;
    if (webcam.startCamera()==1)
    {
        camImage = webcam.queryFrame();

        camImage = webcam.queryFrame();

        camImage = webcam.queryFrame();

        camImage = webcam.queryFrame();
        camImage = webcam.queryFrame();
        camImage = webcam.queryFrame();

        camImage = webcam.queryFrame();
        camImage = webcam.queryFrame();
        for (;;)
        {
            double t = (double)cvGetTickCount();
            double t1=0;
            //backGroundSubtraction = webcam.queryFrame();

            camImage = webcam.queryFrame();
            if (camImage==NULL)
                break;
            frameIndex++;
            CvSize photo_size = cvSize(camImage->width,camImage->height);
            grayIterateImage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);
            static IplImage * colorresized =cvCreateImage(cvSize(1280,480), IPL_DEPTH_8U, 3);
            static IplImage * carricatureImg = cvCreateImage(photo_size, IPL_DEPTH_8U, 3);
            cvZero(carricatureImg);
            cvConvertScale(carricatureImg,carricatureImg,0,255);
            cvCvtColor(camImage, grayIterateImage, CV_RGB2GRAY);
            iterateImage = cvCreateImage(photo_size, IPL_DEPTH_8U, 1);
            cvCopy(grayIterateImage, iterateImage);
            //cvShowImage( "image",camImage);

            //    if (flag==0)
            newMat =lk.currentEstimate();



            for (int i = 1; i <= lk.newDelaunay->totalNumberofTriangles; i++ )
            {


                int node1=lk.newDelaunay->colorTriangleCodeFixed[i][0];
                int node2=lk.newDelaunay->colorTriangleCodeFixed[i][1];
                int node3=lk.newDelaunay->colorTriangleCodeFixed[i][2];

                CvScalar s1,s2;
                s1 =cvGet2D(newMat,0,2*(node1));
                s2 =cvGet2D(newMat,0,2*(node1) + 1);

                CvPoint p1,p2,p3;

                p1=cvPoint(s1.val[0],s2.val[0]);

                s1 =cvGet2D(newMat,0,2*(node2));
                s2 =cvGet2D(newMat,0,2*(node2) + 1);
                p2=cvPoint(s1.val[0],s2.val[0]);


                s1 =cvGet2D(newMat,0,2*(node3));
                s2 =cvGet2D(newMat,0,2*(node3) + 1);
                p3=cvPoint(s1.val[0],s2.val[0]);
                cvLine(camImage, p1, p2, cvScalar(0,255,0), 1);
                cvLine(camImage, p2, p3, cvScalar(0,255,0), 1);
                cvLine(camImage, p1, p3, cvScalar(0,255,0), 1);



            }
            CvScalar px1,px2,px3,px4, py1,py2,py3,py4,px5,py5;
            CvPoint p1[4],p2[9],p3[15];

            px1 =cvGet2D(newMat,0,2*(29));
            py1 =cvGet2D(newMat,0,2*(29) + 1);
            px2 =cvGet2D(newMat,0,2*(30));
            py2 =cvGet2D(newMat,0,2*(30) + 1);

            px3 =cvGet2D(newMat,0,2*(31));
            py3 =cvGet2D(newMat,0,2*(31) + 1);


            px4 =cvGet2D(newMat,0,2*(32));
            py4 =cvGet2D(newMat,0,2*(32) + 1);


            px5 =cvGet2D(newMat,0,2*(33));
            py5 =cvGet2D(newMat,0,2*(33) + 1);

            p1[0].x=(cvGet2D(newMat,0,2*(29))).val[0];
            p1[0].y=py1.val[0];
            p1[1].x=px2.val[0];
            p1[1].y=py2.val[0];
            p1[2].x=px3.val[0];
            p1[2].y=py3.val[0];
            p1[3].x=px4.val[0];
            p1[3].y=py4.val[0];
            p1[4].x=px5.val[0];
            p1[4].y=py5.val[0];


            drawCRSpline(carricatureImg,p1,5);



            px1 =cvGet2D(newMat,0,2*(34));
            py1 =cvGet2D(newMat,0,2*(34) + 1);
            px2 =cvGet2D(newMat,0,2*(35));
            py2 =cvGet2D(newMat,0,2*(35) + 1);

            px3 =cvGet2D(newMat,0,2*(36));
            py3 =cvGet2D(newMat,0,2*(36) + 1);


            px4 =cvGet2D(newMat,0,2*(37));
            py4 =cvGet2D(newMat,0,2*(37) + 1);


            px5 =cvGet2D(newMat,0,2*(38));
            py5 =cvGet2D(newMat,0,2*(38) + 1);

            p1[0].x=px1.val[0];
            p1[0].y=py1.val[0];
            p1[1].x=px2.val[0];
            p1[1].y=py2.val[0];
            p1[2].x=px3.val[0];
            p1[2].y=py3.val[0];
            p1[3].x=px4.val[0];
            p1[3].y=py4.val[0];
            p1[4].x=px5.val[0];
            p1[4].y=py5.val[0];

            drawCRSpline(carricatureImg,p1,5);

            p1[0].x=(cvGet2D(newMat,0,2*(39))).val[0];
            p1[0].y=(cvGet2D(newMat,0,2*(39) + 1)).val[0];
            p1[1].x=(cvGet2D(newMat,0,2*(40))).val[0];
            p1[1].y=(cvGet2D(newMat,0,2*(40) + 1)).val[0];
            p1[2].x=(cvGet2D(newMat,0,2*(41))).val[0];
            p1[2].y=(cvGet2D(newMat,0,2*(41) + 1)).val[0];
            p1[3].x=(cvGet2D(newMat,0,2*(42))).val[0];
            p1[3].y=(cvGet2D(newMat,0,2*(42) + 1)).val[0];
            p1[4].x=(cvGet2D(newMat,0,2*(43))).val[0];
            p1[4].y=(cvGet2D(newMat,0,2*(43) + 1)).val[0];

            drawCRSpline(carricatureImg,p1,5);


            p1[0].x=(cvGet2D(newMat,0,2*(39))).val[0];
            p1[0].y=(cvGet2D(newMat,0,2*(39) + 1)).val[0];
            p1[1].x=(cvGet2D(newMat,0,2*(46))).val[0];
            p1[1].y=(cvGet2D(newMat,0,2*(46) + 1)).val[0];
            p1[2].x=(cvGet2D(newMat,0,2*(45))).val[0];
            p1[2].y=(cvGet2D(newMat,0,2*(45) + 1)).val[0];
            p1[3].x=(cvGet2D(newMat,0,2*(44))).val[0];
            p1[3].y=(cvGet2D(newMat,0,2*(44) + 1)).val[0];
            p1[4].x=(cvGet2D(newMat,0,2*(43))).val[0];
            p1[4].y=(cvGet2D(newMat,0,2*(43) + 1)).val[0];

            drawCRSpline(carricatureImg,p1,5);



            p1[0].x=(cvGet2D(newMat,0,2*(39))).val[0];
            p1[0].y=(cvGet2D(newMat,0,2*(39) + 1)).val[0];
            p1[1].x=(cvGet2D(newMat,0,2*(58))).val[0];
            p1[1].y=(cvGet2D(newMat,0,2*(58) + 1)).val[0];
            p1[2].x=(cvGet2D(newMat,0,2*(59))).val[0];
            p1[2].y=(cvGet2D(newMat,0,2*(59) + 1)).val[0];
            p1[3].x=(cvGet2D(newMat,0,2*(60))).val[0];
            p1[3].y=(cvGet2D(newMat,0,2*(60) + 1)).val[0];
            p1[4].x=(cvGet2D(newMat,0,2*(43))).val[0];
            p1[4].y=(cvGet2D(newMat,0,2*(43) + 1)).val[0];

            drawCRSpline(carricatureImg,p1,5);



            p1[0].x=(cvGet2D(newMat,0,2*(39))).val[0];
            p1[0].y=(cvGet2D(newMat,0,2*(39) + 1)).val[0];
            p1[1].x=(cvGet2D(newMat,0,2*(61))).val[0];
            p1[1].y=(cvGet2D(newMat,0,2*(61) + 1)).val[0];
            p1[2].x=(cvGet2D(newMat,0,2*(62))).val[0];
            p1[2].y=(cvGet2D(newMat,0,2*(62) + 1)).val[0];
            p1[3].x=(cvGet2D(newMat,0,2*(43))).val[0];
            p1[3].y=(cvGet2D(newMat,0,2*(43) + 1)).val[0];
            p1[4].x=(cvGet2D(newMat,0,2*(43))).val[0];
            p1[4].y=(cvGet2D(newMat,0,2*(43) + 1)).val[0];

            drawCRSpline(carricatureImg,p1,4);




            p3[0].x=(cvGet2D(newMat,0,2*(0))).val[0];
            p3[0].y=(cvGet2D(newMat,0,2*(0) + 1)).val[0];

            p3[1].x=(cvGet2D(newMat,0,2*(4))).val[0];
            p3[1].y=(cvGet2D(newMat,0,2*(4) + 1)).val[0];

            p3[2].x=(cvGet2D(newMat,0,2*(6))).val[0];
            p3[2].y=(cvGet2D(newMat,0,2*(6) + 1)).val[0];

            p3[3].x=(cvGet2D(newMat,0,2*(8))).val[0];
            p3[3].y=(cvGet2D(newMat,0,2*(8) + 1)).val[0];



            p3[4].x=(cvGet2D(newMat,0,2*(12))).val[0];
            p3[4].y=(cvGet2D(newMat,0,2*(12) + 1)).val[0];


            drawCRSpline(carricatureImg,p3,5,1);



            p3[0].x=(cvGet2D(newMat,0,2*(21))).val[0];
            p3[0].y=(cvGet2D(newMat,0,2*(21) + 1)).val[0];

            p3[1].x=(cvGet2D(newMat,0,2*(22))).val[0];
            p3[1].y=(cvGet2D(newMat,0,2*(22) + 1)).val[0];

            p3[2].x=(cvGet2D(newMat,0,2*(23))).val[0];
            p3[2].y=(cvGet2D(newMat,0,2*(23) + 1)).val[0];

            p3[3].x=(cvGet2D(newMat,0,2*(24))).val[0];
            p3[3].y=(cvGet2D(newMat,0,2*(24) + 1)).val[0];



            p3[4].x=(cvGet2D(newMat,0,2*(25))).val[0];
            p3[4].y=(cvGet2D(newMat,0,2*(25) + 1)).val[0];

            p3[5].x=(cvGet2D(newMat,0,2*(26))).val[0];
            p3[5].y=(cvGet2D(newMat,0,2*(26) + 1)).val[0];






            p3[6].x=(cvGet2D(newMat,0,2*(21))).val[0];
            p3[6].y=(cvGet2D(newMat,0,2*(21) + 1)).val[0];

            drawCRSpline(carricatureImg,p3,7,1);


            p3[0].x=(cvGet2D(newMat,0,2*(13))).val[0];
            p3[0].y=(cvGet2D(newMat,0,2*(13) + 1)).val[0];

            p3[1].x=(cvGet2D(newMat,0,2*(14))).val[0];
            p3[1].y=(cvGet2D(newMat,0,2*(14) + 1)).val[0];

            p3[2].x=(cvGet2D(newMat,0,2*(15))).val[0];
            p3[2].y=(cvGet2D(newMat,0,2*(15) + 1)).val[0];

            p3[3].x=(cvGet2D(newMat,0,2*(16))).val[0];
            p3[3].y=(cvGet2D(newMat,0,2*(16) + 1)).val[0];



            p3[4].x=(cvGet2D(newMat,0,2*(17))).val[0];
            p3[4].y=(cvGet2D(newMat,0,2*(17) + 1)).val[0];

            p3[5].x=(cvGet2D(newMat,0,2*(18))).val[0];
            p3[5].y=(cvGet2D(newMat,0,2*(18) + 1)).val[0];


            p3[6].x=(cvGet2D(newMat,0,2*(13))).val[0];
            p3[6].y=(cvGet2D(newMat,0,2*(13) + 1)).val[0];

            drawCRSpline(carricatureImg,p3,7,1);




            p3[0].x=(cvGet2D(newMat,0,2*(48))).val[0];
            p3[0].y=(cvGet2D(newMat,0,2*(48) + 1)).val[0];

            p3[1].x=(cvGet2D(newMat,0,2*(51))).val[0];
            p3[1].y=(cvGet2D(newMat,0,2*(51) + 1)).val[0];

            p3[2].x=(cvGet2D(newMat,0,2*(52))).val[0];
            p3[2].y=(cvGet2D(newMat,0,2*(52) + 1)).val[0];

            p3[3].x=(cvGet2D(newMat,0,2*(53))).val[0];
            p3[3].y=(cvGet2D(newMat,0,2*(53) + 1)).val[0];



            p3[4].x=(cvGet2D(newMat,0,2*(56))).val[0];
            p3[4].y=(cvGet2D(newMat,0,2*(56) + 1)).val[0];



            drawCRSpline(carricatureImg,p3,5,1);
//    /   else
            //      flag=0;
            for (int i=0;i<numberofpoints;i++)
            {
                CvScalar s1,s2;
                s1 =cvGet2D(newMat,0,2*i);
                s2 =cvGet2D(newMat,0,2*i + 1);
                //printf("%e %e \n",s1.val[0],s2.val[0]);
                cvCircle( camImage, cvPoint((int)s1.val[0],(int)s2.val[0]), 1, CV_RGB(255,(0),0), 1 );


            }

            lk.setImage(iterateImage);
            // cvWaitKey(-1);
            // printf("ITERATE STARTED \n");
            double timer= (double)cvGetTickCount();

            for (int i=0;i<60;i++)
            {

                double er = lk.iterate();

                if ((i==0 && frameIndex==1) || ( frameIndex > (stepSize + minFrameIndex) ) )
                {
                    min=er;
                    minFrameIndex=frameIndex;

                    printf("%e \n",min);

                }

                if (min>er)
                {
                    min=er;
                    minFrameIndex=frameIndex;

                }
                else
                {
                    break;
                    flag=1;
                }
            }

            //      timer = (double)cvGetTickCount() - timer;
//printf( "detection time = %gms\n", timer/((double)cvGetTickFrequency()*1000.) );

cvSetImageROI(colorresized,cvRect(0,0,640,480));

            cvResize(camImage,colorresized);
cvResetImageROI(colorresized);

            cvSetImageROI(colorresized,cvRect(639,0,640,480));
            cvResize(carricatureImg,colorresized);
cvResetImageROI(colorresized);
cvResetImageROI(colorresized);

            cvWriteFrame(writer,colorresized);      // add the frame to the file
            cvShowImage( "image",camImage);

            cvShowImage( "carimage",carricatureImg);

            // char a[300];
            // sprintf(a,"/home/rohananil/COP/images/%06d.jpg",counter++);
            //     cvSaveImage(a,camImage);

            //  cvWriteFrame(writer,camImage);      // add the frame to the file
            cvWaitKey(3);
            cvReleaseImage(&camImage);
            cvReleaseImage(&grayIterateImage);
            cvReleaseImage(&iterateImage);
            // cvReleaseImage(&templateImage);

        }
        cvReleaseVideoWriter(&writer);
    }
    cvDestroyWindow("template");
    cvDestroyWindow("image");


}
