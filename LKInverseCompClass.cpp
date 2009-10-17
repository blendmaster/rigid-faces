/*
    COP - Active Appearance Model Face Tracking - using Inverse Compositional Approach
    Rohan Anil , under Dr. Radhika Vatsan , BITS Pilani Goa Campus
    rohan.ani;@gmail.com
    License : GPLV3
*/
#include "cv.h"
#include "highgui.h"
#include "LKInverseCompClass.h"

#include <stdio.h>

LKInverseComp::LKInverseComp()
{
// Create matrices.
    WarpMatrix = cvCreateMat(3, 3, CV_32F);
    cvSet(WarpMatrix, cvScalar(0));

    WarpMatrixDeltaP = cvCreateMat(3, 3, CV_32F);
    cvSet(WarpMatrixDeltaP, cvScalar(0));


    WarpMatrixDeltaPInverse = cvCreateMat(3, 3, CV_32F);
    cvSet(WarpMatrixDeltaPInverse, cvScalar(0));


    HessianMatrix  = cvCreateMat(6, 6, CV_32F);
    cvSet(HessianMatrix, cvScalar(0));

    HessianMatrixInverse= cvCreateMat(6, 6, CV_32F);
    cvSet(HessianMatrixInverse, cvScalar(0));

    DeltaP = cvCreateMat(6, 1, CV_32F);
    cvSet(HessianMatrixInverse, cvScalar(0));



}
void LKInverseComp::iterate()
{
    int k=numberOfIterations;
//printf("%d %d\n",templateImage->width,templateImage->height);
    int pixelCount=0;
    double totalError=0;
    double averageError=0;

    while (k>0)
    {
        // ///    printf("%f %f \n ", CV_MAT_ELEM(*WarpMatrix, float,0, 1),CV_MAT_ELEM(*WarpMatrix, float,1, 1));
        // ///    printf("%f %f \n", CV_MAT_ELEM(*WarpMatrix, float,0, 0),CV_MAT_ELEM(*WarpMatrix, float,1, 0));
        // ///   printf("%f %f \n ", CV_MAT_ELEM(*WarpMatrix, float,0, 2),CV_MAT_ELEM(*WarpMatrix, float,1, 2));

        k--;

        for (int i=0; i<templateImage->width; i++)
        {

            for (int j=0; j<templateImage->height; j++)
            {

                CV_MAT_ELEM(*errorImage, float, i*templateImage->height + j, 0) = 0;

                float templateValue ;
                CvScalar s;
                s=cvGet2D(templateImage,j,i);
                templateValue=s.val[0];

                float Wi = CV_MAT_ELEM(*WarpMatrix, float,0, 0)*i +      CV_MAT_ELEM(*WarpMatrix, float,0, 1)*j +  CV_MAT_ELEM(*WarpMatrix, float,0, 2);
                float Wj = CV_MAT_ELEM(*WarpMatrix, float,1, 0)*i +      CV_MAT_ELEM(*WarpMatrix, float,1, 1)*j +  CV_MAT_ELEM(*WarpMatrix, float,1, 2);

                int Wi_floor = cvFloor(Wi);
                int Wj_floor = cvFloor(Wj);


                if (Wi_floor>=0 && Wi_floor<iterateImage->width && Wj_floor>=0 && Wj_floor<iterateImage->height)
                {
                    pixelCount++;
                    float imageValue =interpolate<uchar>(iterateImage, Wi, Wj);
                    totalError+=imageValue-templateValue;
                    CV_MAT_ELEM(*errorImage, float, i*templateImage->height + j, 0) =imageValue-templateValue;
                }
            }



        }


        if (pixelCount!=0)
            averageError =totalError/(double)pixelCount;

        cvMatMul(HessianMatrixInverse_steepestDescentImageTranspose,errorImage,DeltaP);
        //cvMatMul(HessianMatrixInverse,steepestDescentImageTranspose_X_errorImage,DeltaP);

        float p1= CV_MAT_ELEM(*DeltaP, float,0, 0);
        float p2= CV_MAT_ELEM(*DeltaP,float, 1, 0);
        float p3= CV_MAT_ELEM(*DeltaP,float, 2, 0);
        float p4= CV_MAT_ELEM(*DeltaP,float, 3, 0);
        float p5= CV_MAT_ELEM(*DeltaP,float, 4, 0);
        float p6= CV_MAT_ELEM(*DeltaP,float, 5, 0);
        //     printf ("%f %f %f %f %f %f %f \n",p1,p2,p3,p4,p5,p6);
        //  printf("%e MEAN ERROR \n",averageError);
        setWarpMatrix(WarpMatrixDeltaP,p1,p2,p3,p4,p5,p6);
        cvInvert(WarpMatrixDeltaP,WarpMatrixDeltaPInverse);
        cvMatMul(WarpMatrixDeltaPInverse,WarpMatrix,WarpMatrix);


        if (fabs(p1)<=DeltaE && fabs(p2)<=DeltaE && fabs(p3)<=DeltaE && fabs(p4)<=DeltaE && fabs(p5)<=DeltaE && fabs(p6)<=DeltaE)
        {
            printf("Broken on Kth Iteration %d \n",k);
            break;
        }
    }

}
void LKInverseComp:: setTemplate(IplImage* image)
{
    templateImage=image;
    preCompute();
}

void LKInverseComp:: setImage(IplImage* image)
{
    iterateImage=image;
}

void LKInverseComp:: setCurrentWarpEstimate(float p1,float p2,float p3 ,float p4,float p5,float p6)
{
    setWarpMatrix(WarpMatrix,p1,p2,p3,p4,p5,p6);
}

void LKInverseComp::setWarpMatrix(CvMat* Warp,float p1=0,float p2=0,float p3 =0,float p4=0 ,float p5=0,float p6=0)
{
    CV_MAT_ELEM(*Warp, float,0, 0)=1+p1;
    CV_MAT_ELEM(*Warp, float,1, 0)=p2;

    CV_MAT_ELEM(*Warp, float,0, 1)=p3;
    CV_MAT_ELEM(*Warp, float,1, 1)=1+p4;

    CV_MAT_ELEM(*Warp, float,0, 2)=p5;
    CV_MAT_ELEM(*Warp, float,1, 2)=p6;

    CV_MAT_ELEM(*Warp, float,2, 0)=0;
    CV_MAT_ELEM(*Warp, float,2, 1)=0;
    CV_MAT_ELEM(*Warp, float,2, 2)=1;
}

void LKInverseComp::preCompute()
{

    GradientX = cvCreateImage(cvSize(templateImage->width,templateImage->height),  IPL_DEPTH_32F, 1);
    GradientY = cvCreateImage(cvSize(templateImage->width,templateImage->height),  IPL_DEPTH_32F, 1);
cvZero(GradientX);
cvZero(GradientY);


// Normal Gradient Doesnt seem to be working ???
/*
    for (int j=0; j<templateImage->height; j++)
    {
        for (int i=0; i<templateImage->width; i++)
        {
            CvScalar s1,s2,s3;
                            s1=cvGet2D(templateImage,j,i);

            if((i+1)< templateImage->width)
           {
            s2=cvGet2D(templateImage,j,i+1);
            s3.val[0]=s2.val[0]-s1.val[0];
                cvSet2D(GradientX,j,i,s3);
           }
                if((j+1)< templateImage->height)
           {
            s2=cvGet2D(templateImage,j+1,i);
            s3.val[0]=s2.val[0]-s1.val[0];
                cvSet2D(GradientY,j,i,s3);
           }
        }

    }
*/
   cvSobel(templateImage, GradientX, 1, 0);
   cvSobel(templateImage, GradientY, 0, 1);
  cvConvertScale(GradientX, GradientX, 0.2);
   cvConvertScale(GradientY, GradientY, 0.2);



    steepestDescentImage  = cvCreateMat((templateImage->width*templateImage->height), 6, CV_32F);
    cvSet(steepestDescentImage, cvScalar(0));
    printf("Width %d %d  \n",templateImage->width,templateImage->height);
    errorImage = cvCreateMat((templateImage->width*templateImage->height),1, CV_32F);
    cvSet(errorImage, cvScalar(0));

    steepestDescentImageTranspose  = cvCreateMat( 6,(templateImage->width*templateImage->height), CV_32F);
    cvSet(steepestDescentImageTranspose, cvScalar(0));


    HessianMatrixInverse_steepestDescentImageTranspose =cvCreateMat( 6,(templateImage->width*templateImage->height), CV_32F);
    cvSet(HessianMatrixInverse_steepestDescentImageTranspose, cvScalar(0));


    /*
    Find Gradient of The Image
    */


    for (int i=0; i<templateImage->width; i++)
    {

        for (int j=0; j<templateImage->height; j++)
        {

            float GradT_x;
            float GradT_y;

            CvScalar s;
            s=cvGet2D(GradientX,j,i);
            GradT_x=s.val[0];
            s=cvGet2D(GradientY,j,i);
            GradT_y=s.val[0];
//
            //  printf("Grad tx = %e \n",GradT_x);
            CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 0) = GradT_x*i;
            CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 1) = GradT_y*i;
            CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 2) = GradT_x*j;
            CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 3) = GradT_y*j;
            CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 4) = GradT_x;
            CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 5) = GradT_y;
        }
    }

    cvTranspose(steepestDescentImage, steepestDescentImageTranspose);
    cvMatMul(steepestDescentImageTranspose,steepestDescentImage,HessianMatrix);


    double inv = cvInvert(HessianMatrix,HessianMatrixInverse);
    cvMatMul(HessianMatrixInverse,steepestDescentImageTranspose,HessianMatrixInverse_steepestDescentImageTranspose);

}
