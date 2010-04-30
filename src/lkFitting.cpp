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
#include "lkFitting.h"
#include <delaunay.h>
#include "stdio.h"

#include<aam.h>

double LKInverseComp::iterate()
{

    if (iterateImage==NULL)
        return 0;
    //cvWaitKey(-1);

       double timer = (double)cvGetTickCount();

        //printf("\n");

        for (int i=0;i<(4);i++)
        {
            ptemp1[i]=param[i];


        }

        for (int i=4;i<(nS+4);i++)
        {
            ptemp2[i]=param[i];

        }

        CvMat * affShapeVec = computeShape(ptemp2);
        newDelaunay->calculateAffineWarpParameters(affShapeVec);
        affShapeVec = computeShape(ptemp1);
        newDelaunay->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);

        for (int i=0;i<numberofpoints;i++)
        {
            double x,y;
            x=newDelaunay->ithNodeCode[i].triangles[0].xi;
            y=newDelaunay->ithNodeCode[i].triangles[0].yi;
            int k=0;
            double xx= x* newDelaunay->ithNodeCode[i].triangles[k].affine[0] + y* newDelaunay->ithNodeCode[i].triangles[k].affine[1] + newDelaunay->ithNodeCode[i].triangles[k].affine[2];
            double yy= x* newDelaunay->ithNodeCode[i].triangles[k].affine[3] + y* newDelaunay->ithNodeCode[i].triangles[k].affine[4] + newDelaunay->ithNodeCode[i].triangles[k].affine[5];

            CvScalar s1,s2;
            s1.val[0]=xx;
            s2.val[0]=yy;
            cvSet2D(srcShape,0,2*i,s1);
            cvSet2D(srcShape,0,2*i+1,s2);
        }

        // cvZero(ErrorImage);


        totalerror=0;
        bottomStepSize=0;
        topStepSize=0;
        for (int i=0;i<totalNumberOfPixels;i++)
        {
            int flag=1;


            pixel * pix = newDelaunay->getpixel(i);
            pixel *pix2 =newDelaunay->findCorrespondingPixelInImage(pix);

            if (((double)pix2->x)<(double)iterateImage->width && ((double)pix2->y)<(double)iterateImage->height )
            {
                if ((pix2->y)>=0 && (pix2->x)>=0)
                {
                    flag=0;
                    CvScalar val1,val2,val3;


                    val1.val[0]=(interpolate<uchar>(iterateImage,double(pix2->x),double(pix2->y)));

                    val2.val[0]=0;
                    if ((pix->y+pix->ty)>=0 && (pix->x+pix->tx)>=0)
                    {
                        val2.val[0]=interpolateMean[i];
                        val3.val[0] = val1.val[0]-val2.val[0];
                        bottomStepSize+=pow(val2.val[0],2);
                        topStepSize+=val2.val[0]*val1.val[0];
                        totalerror+=pow(val3.val[0],2);
                        ErrorImage->data.db[ (int(pix->y+pix->ty)*int(pix->width) + int(pix->x+pix->tx)) *ErrorImage->cols + 0 ] =val3.val[0];

                        //   cvSet2D(ErrorImage, int(pix->y+pix->ty)*int(pix->width+10) + int(pix->x+pix->tx),0,val3);

                    }

                }

            }



        }
//        timer = (double)cvGetTickCount() - timer;
//        printf( " Error Estimate = %gms\n", (timer)/((double)cvGetTickFrequency()*1000.) );
//
//




        //printf("Error = %e \n",totalerror);

//        timer= (double)cvGetTickCount();
        static CvMat * parameter = cvCreateMat( nS+4,1, CV_64FC1);
        cvMatMul(HessianMatrixInverse_steepestDescentImageTranspose,ErrorImage,parameter);

//
//
//        timer = (double)cvGetTickCount() - timer;
//        printf( " MULTIPLICATION = %gms\n", timer/((double)cvGetTickFrequency()*1000.) );



        //printf("Rat %e\n",(bottomStepSize/topStepSize));
        for (int i=0;i<(nS+4);i++)
        {
            CvScalar s = cvGet2D(parameter,i,0);
            negNewParam[i] =-1*(bottomStepSize/topStepSize)*s.val[0];
        }
        for (int i=0;i<(4);i++)
        {
            negNewParam4[i]= negNewParam[i];

        }
        for (int i=4;i<(nS+4);i++)
        {
            negNewParamnS[i]= negNewParam[i];

        }

//        timer= (double)cvGetTickCount();

        affShapeVec = computeShape(negNewParamnS);  //Modified
        newDelaunayInverse->calculateAffineWarpParameters(affShapeVec);
        affShapeVec = computeShape(negNewParam4);
        newDelaunayInverse->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);
//        timer = (double)cvGetTickCount() - timer;
//        printf( " Compose = %gms\n", timer/((double)cvGetTickFrequency()*1000.) );
        for (int i=0;i<numberofpoints;i++)
        {
            double x,y;
            double finalx,finaly;
            finalx=0;
            finaly=0;
            int ind = newDelaunayInverse->ithNodeCode[i].count;
            x=newDelaunayInverse->ithNodeCode[i].triangles[0].xi;
            y=newDelaunayInverse->ithNodeCode[i].triangles[0].yi;


            for (int k=0;k<ind;k++)
            {
                double xx= x* newDelaunayInverse->ithNodeCode[i].triangles[k].affine[0] + y* newDelaunayInverse->ithNodeCode[i].triangles[k].affine[1] + newDelaunayInverse->ithNodeCode[i].triangles[k].affine[2];
                double yy= x* newDelaunayInverse->ithNodeCode[i].triangles[k].affine[3] + y* newDelaunayInverse->ithNodeCode[i].triangles[k].affine[4] + newDelaunayInverse->ithNodeCode[i].triangles[k].affine[5];
                finalx+= xx* newDelaunay->ithNodeCode[i].triangles[k].affine[0] + yy* newDelaunay->ithNodeCode[i].triangles[k].affine[1] + newDelaunay->ithNodeCode[i].triangles[k].affine[2];
                finaly+= xx* newDelaunay->ithNodeCode[i].triangles[k].affine[3] + yy* newDelaunay->ithNodeCode[i].triangles[k].affine[4] + newDelaunay->ithNodeCode[i].triangles[k].affine[5];

            }
            finalx/=ind;
            finaly/=ind;
            CvScalar s1,s2;
            s1.val[0]=finalx;
            s2.val[0]=finaly;
            cvSet2D(destShape,0,2*i,s1);
            cvSet2D(destShape,0,2*i+1,s2);
        }

        cvAddWeighted(srcShape, 0.3, destShape, 0.7, 0, destShape);



        cvSub(destShape,meanShape,destShapeTemp);
        for (int i=0;i<4;i++)
        {
            double p=cvDotProduct(combineshapevectors[i],destShapeTemp);
            negNewParam4[i]=-1*p;
            Param4[i]=p;
        }

        for (int j=0;j<totalnumberofpoints;j++)
        {
            CvScalar s2 = cvGet2D(destShape,0,j);
            double p=0;
            for (int i=0;i<4;i++)
            {
                CvScalar s1 = cvGet2D(combineshapevectors[i],0,j);
                p+= (negNewParam4[i] * s1.val[0]);
            }

            CvScalar t;
            t.val[0] = s2.val[0] + p;
            cvSet2D(destShapewithoutQ,0,j,t);
        }
        cvSub(destShapewithoutQ,meanShape,destShapeTemp);
        for (int i=4;i<(nS+4);i++)
        {
            double p=cvDotProduct(combineshapevectors[i],destShapeTemp);
            negNewParamnS[i]=-1*p;
            ParamnS[i]=p;
        }
        for (int i=4;i<nS+4;i++)
        {
            param[i] =ParamnS[i];
        }
        for (int i=0;i<4;i++)
        {
            param[i] =Param4[i];
        }
//        printf(" -------------- \n");
     for (int i=0;i<nS+4;i++)
        {
if(i>3)
{
 CvScalar s;
 s= cvGet2D(eigenVal,0,i-4);
// printf("%e \n",s.val[0]);
 if(param[i]>((.7)*sqrt(s.val[0])))
 param[i]=((.7)*sqrt(s.val[0]));

 if(param[i]<((-.7)*sqrt(s.val[0])))
 param[i]=((-.7)*sqrt(s.val[0]));



}
//         printf("param %e \n",param[i]);
        }

                timer = (double)cvGetTickCount() - timer;
        printf( " Current Estimate = %gms\n", timer/((double)cvGetTickFrequency()*1000.) );

    return totalerror;

}


void LKInverseComp:: setImage(IplImage* image)
{
    iterateImage=image;
}

void LKInverseComp::setParamtxty(int x,int y)
{
    param[0]=1;
    param[2]=sqrt(numberofpoints)*x;
    param[3]=sqrt(numberofpoints)*y;
}

void LKInverseComp::setParam(float * parameters)
{

    for (int i =0;i<(nS+4);i++)
    {
        param[i]=parameters[i];
    }

}


void LKInverseComp::preCompute()
{
    IplImage * imgMask = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_8U, 1);
    GradientX = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
    GradientY = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);

    IplImage*      GradientXTemp = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
    IplImage*      GradientYTemp = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
    cvZero(GradientX);
    cvZero(GradientY);
    cvZero(GradientXTemp);
    cvZero(GradientYTemp);
    cvZero(imgMask);
    int t1=newDelaunay->numberOfPixels();
    for (int i=0;i<t1;i++)
    {

        pixel * pix = newDelaunay->getpixel(i);
        CvScalar s;
        s.val[0]=1;

        cvSet2D(imgMask,pix->y+pix->ty,pix->x+pix->tx,s);

    }

    cvSobel(meanAppearance, GradientXTemp, 1, 0);
 cvSobel(meanAppearance, GradientYTemp, 0, 1);

//
//  for (int i=0;i<meanAppearance->width;i++)
//    {
//        for (int j=0;j<meanAppearance->height;j++)
//        {
//
//            CvScalar im1;
//            CvScalar ip1;
//
//            im1.val[0]=0;
//            ip1.val[0]=0;
//         //   printf("%d %d",i,j);
//            if ((i-1)>=0)
//                im1=cvGet2D(meanAppearance,j,i-1);
//            if ((i+1)<meanAppearance->width)
//                ip1=cvGet2D(meanAppearance,j,i+1);
//
//            CvScalar k;
//            k.val[0]=(ip1.val[0]-im1.val[0])/2;
//
//            cvSet2D(GradientXTemp,j,i,k);
//
//            im1.val[0]=0;
//            ip1.val[0]=0;
//
//            if ((j-1)>=0)
//                im1=cvGet2D(meanAppearance,j-1,i);
//
//            if ((j+1)<meanAppearance->height)
//                ip1=cvGet2D(meanAppearance,j+1,i);
//
//
//            k.val[0]=(ip1.val[0]-im1.val[0])/2;
//
//            cvSet2D(GradientYTemp,j,i,k);
//
//        }
//    }
//////
//cvConvertScale(GradientXTemp,GradientXTemp,2);
//cvConvertScale(GradientYTemp,GradientYTemp,2);
//


    cvAnd(GradientXTemp,GradientXTemp,GradientX,imgMask);
    cvAnd(GradientYTemp,GradientYTemp,GradientY,imgMask);
//    cvNamedWindow("mean",1);
//    cvShowImage("mean",GradientX);
//    cvWaitKey(-1);
    HessianMatrix = cvCreateMat(nS+4, nS+4, CV_64FC1);
    HessianMatrixInverse = cvCreateMat(nS+4, nS+4, CV_64FC1);
    HessianMatrixInverse_steepestDescentImageTranspose = cvCreateMat( nS+4,(meanAppearance->width*meanAppearance->height), CV_64FC1);
    steepestDescentImage  = cvCreateMat((meanAppearance->width*meanAppearance->height), nS+4, CV_64FC1);
    steepestDescentImageTranspose  = cvCreateMat( nS+4,(meanAppearance->width*meanAppearance->height), CV_64FC1);
    CvMat * steepestDescentCoeffX = cvCreateMat((meanAppearance->width*meanAppearance->height),nS+4, CV_64FC1);
    CvMat * steepestDescentCoeffY = cvCreateMat((meanAppearance->width*meanAppearance->height),nS+4, CV_64FC1);
    CvMat * app = cvCreateMat((meanAppearance->width*meanAppearance->height),1, CV_64FC1);
    cvZero(steepestDescentCoeffX);
    cvZero(steepestDescentCoeffY);
    CvMat * dw_xi_yi = cvCreateMat((meanAppearance->width*meanAppearance->height),numberofpoints, CV_64FC1);
    cvZero(dw_xi_yi);
    for (int m=0;m<numberofpoints;m++)
    {
        int t=newDelaunay->numberOfPixels();
        for (int i=0;i<t;i++)
        {
            pixel * pix = newDelaunay->getpixel(i);
            int typ=pix->type;
            int flag=0;
            int k=0;
            for ( k=0;k<newDelaunay->ithNodeCode[m].count;k++)
            {
                if (newDelaunay->ithNodeCode[m].triangles[k].colorcode==typ)
                {
                    flag=1;
                    break;
                }
            }
            if (flag==1)
            {
                CvScalar s1 = cvGet2D(meanShape,0,2*m);
                CvScalar s2 = cvGet2D(meanShape,0,2*m +1);
                double alphatop=    ((((pix->x ) - newDelaunay->ithNodeCode[m].triangles[k].xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (((pix->y) - newDelaunay->ithNodeCode[m].triangles[k].yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double alphabottom =  (((  newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (( newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double alpha= alphatop/alphabottom;
                double betatop=    ((((pix->y ) - newDelaunay->ithNodeCode[m].triangles[k].yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi)   -  (((pix->x) - newDelaunay->ithNodeCode[m].triangles[k].xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi));
                double betabottom =  (((  newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (( newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double beta= betatop/betabottom;
                double val=1-alpha -beta;
                CvScalar s;
                if (val<0)
                {
                    //printf("val %e \n",val);
                    val=0;
                }
                s.val[0]=val;
                if ((pix->y+pix->ty)>=0 && (pix->x+pix->tx)>=0)
                {
                    CvScalar s1= cvGet2D(dw_xi_yi,  int( pix->y+pix->ty) * int(pix->width) + int(pix->x + pix->tx ) ,m);
                    s.val[0]+=s1.val[0];
                    cvSet2D(dw_xi_yi,  int( pix->y+pix->ty) * int(pix->width) + int(pix->x + pix->tx ) ,m ,s );
                }
            }
        }
    }



    int t=meanAppearance->width*meanAppearance->height;
    for (int u=0;u<t;u++)
    {
        double GradT_x;
        double GradT_y;
        CvScalar s;
        s=cvGet2D(GradientX,int(floor(u/meanAppearance->width)),(u%meanAppearance->width));
        GradT_x=s.val[0];
        s=cvGet2D(GradientY,int(floor(u/meanAppearance->width)),(u%meanAppearance->width));
        GradT_y=s.val[0];

        for (int i=0;i<numberofpoints;i++)
        {
            for (int m=0;m<(nS+4);m++)
            {
                CvScalar currentX = cvGet2D(steepestDescentCoeffX,u,m);
                CvScalar currentY = cvGet2D(steepestDescentCoeffY,u,m);
                CvScalar s1,s2,s3;
                s1 =cvGet2D(dw_xi_yi,u,i);
                s2 =cvGet2D(combineshapevectors[m],0,2*i);
                s3 =cvGet2D(combineshapevectors[m],0,2*i + 1);
                double temp;
                temp = s1.val[0]*s2.val[0];
                currentX.val[0]+= (GradT_x*(temp));
                temp = s1.val[0]*s3.val[0];
                currentY.val[0]+= (GradT_y* (temp));
                cvSet2D(steepestDescentCoeffX,u,m,currentX);
                cvSet2D(steepestDescentCoeffY,u,m,currentY);
            }
        }

    }
    cvAdd(steepestDescentCoeffX, steepestDescentCoeffY, steepestDescentImage);

    for (int m=0;m<(nS+4);m++)
    {

        cvZero(app);
        for (int l=0;l<(nA);l++)
        {
            double total=0;
            for (int h=0;h<t;h++)
            {

                int wid;
                int hei;
                hei =floor(h/meanAppearance->width);
                wid =(h%meanAppearance->width);
                CvScalar s2;
                s2=cvGet2D(appearanceVectors[l],hei,wid);
                CvScalar s;
                s=cvGet2D(steepestDescentImage,h,m);
                total+= s2.val[0]*s.val[0];


            }

           // cvDotProduct(appearanceVectors[i],destShapeTemp);



            for (int h=0;h<t;h++)
            {
                int wid;
                int hei;
                hei =(h/meanAppearance->width);
                wid =(h%meanAppearance->width);
                CvScalar s;
                s=cvGet2D(app,h,0);
                CvScalar s2;
                s2=cvGet2D(appearanceVectors[l],hei,wid);
                s.val[0]+= (s2.val[0]*total);
                cvSet2D(app,h,0,s);
            }



        }

        for (int h=0;h<t;h++)
        {
            CvScalar s;
            s=cvGet2D(steepestDescentImage,h,m);
            CvScalar s2;
            s2=cvGet2D(app,h,0);
            s.val[0]-=s2.val[0];
            cvSet2D(steepestDescentImage,h,m,s);


        }
    }
    cvTranspose(steepestDescentImage, steepestDescentImageTranspose);
    cvMatMul(steepestDescentImageTranspose,steepestDescentImage,HessianMatrix);
    cvInvert(HessianMatrix, HessianMatrixInverse);
    cvMatMul(HessianMatrixInverse,steepestDescentImageTranspose,HessianMatrixInverse_steepestDescentImageTranspose);
}
CvMat * LKInverseComp::currentEstimate()
{
    static float* ptemp1 = (float *)malloc(sizeof(float)*(nS+4));
    static float* ptemp2 = (float *)malloc(sizeof(float)*(nS+4));

    for (int i=0;i<(nS+4);i++)
    {
        ptemp1[i]=0;
        ptemp2[i]=0;
    }


    for (int i=0;i<(4);i++)
    {
        ptemp1[i]=param[i];

    }

    for (int i=4;i<(nS+4);i++)
    {
        ptemp2[i]=param[i];

    }

    CvMat * affShapeVec = computeShape(ptemp2);
    newDelaunayShapeCompute->calculateAffineWarpParameters(affShapeVec);
    affShapeVec = computeShape(ptemp1);
    newDelaunayShapeCompute->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);

    CvMat * mat = cvCreateMat(1,totalnumberofpoints,CV_64FC1);

    for (int i=0;i<numberofpoints;i++)
    {
        double x,y;
        double finalx,finaly;
        finalx=0;
        finaly=0;
        int t=newDelaunayShapeCompute->ithNodeCode[i].count;
        if (t>0)
        {
            int xx=newDelaunayShapeCompute->ithNodeCode[i].triangles[0].xi;
            int yy=newDelaunayShapeCompute->ithNodeCode[i].triangles[0].yi;
            x = xx* newDelaunayShapeCompute->ithNodeCode[i].triangles[0].affine[0] + yy* newDelaunayShapeCompute->ithNodeCode[i].triangles[0].affine[1] + newDelaunayShapeCompute->ithNodeCode[i].triangles[0].affine[2];
            y = xx* newDelaunayShapeCompute->ithNodeCode[i].triangles[0].affine[3] + yy* newDelaunayShapeCompute->ithNodeCode[i].triangles[0].affine[4] + newDelaunayShapeCompute->ithNodeCode[i].triangles[0].affine[5];
            CvScalar s1,s2;
            s1.val[0]=x;
            s2.val[0]=y;
            cvSet2D(mat,0,2*i,s1);
            cvSet2D(mat,0,2*i+1,s2);
        }

    }


    return mat;

}
CvMat * LKInverseComp::computeShape(float *parameters)
{



    IplImage * currentShape = cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
    cvZero(currentShape);

    IplImage * temp;


    for (int m=0;m<(nS+4);m++)
    {

        temp = cvCloneImage(combineshapevectors[m]);
        cvConvertScale(combineshapevectors[m],temp,parameters[m]);

//        for (int k=0;k<combineshapevectors[m]->width;k++)
//        {
//            for (int d=0;d<combineshapevectors[m]->height;d++)
//            {
//                CvScalar s;
//                s=cvGet2D(combineshapevectors[m],d,k);
//                s.val[0]*=parameters[m];
//                cvSet2D(temp,d,k,s);
//            }
//        }
        cvAdd(temp,currentShape,currentShape,0);
    }

    cvAdd(meanShape,currentShape,currentShape,0);
    static CvMat * mat = cvCreateMat(numberofpoints,2,CV_64FC1);
    for (int m=0;m<numberofpoints;m++)
    {
        CvScalar s1 = cvGet2D(currentShape,0,2*m);
        CvScalar s2 = cvGet2D(currentShape,0,2*m +1);

        cvSet2D(mat,m,0,s1);

        cvSet2D(mat,m,1,s2);
    }
    cvReleaseImage(&currentShape);
    return mat;

}
LKInverseComp::LKInverseComp(char *path)
{
    CvFileStorage *fs;

    fs = cvOpenFileStorage(path, 0, CV_STORAGE_READ );
    nA = (int)cvReadRealByName(fs, 0, "numberOfAppearanceEigenVectors", 0);
    nS = (int)cvReadRealByName(fs, 0, "numberOfShapeEigenVectors", 0);
printf("%d %d \n",nS,nA);

//nS=16;
//
//nA=2;
//nS=4;/
nA=10;
nA+=1;








    negNewParam = (float *)malloc(sizeof(float)*(nS+4));
    negNewParamnS = (float *)malloc(sizeof(float)*(nS+4));
    negNewParam4 = (float *)malloc(sizeof(float)*(nS+4));
    ParamnS = (float *)malloc(sizeof(float)*(nS+4));
    Param4 = (float *)malloc(sizeof(float)*(nS+4));
    ptemp1 = (float *)malloc(sizeof(float)*(nS+4));
    ptemp2 = (float *)malloc(sizeof(float)*(nS+4));
    param = (float *) malloc((4+nS)*sizeof(float));
    nonRigidShapeVectors=(IplImage**)cvAlloc(sizeof(IplImage*) * nS);
    appearanceVectors=(IplImage**)cvAlloc(sizeof(IplImage*) * nA);
    globalShapeVectors=(IplImage**)cvAlloc(sizeof(IplImage*) * 4);
    combineshapevectors=(IplImage**)cvAlloc(sizeof(IplImage*) * (4+nS));
    srcShape =cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);

    destShape =cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
    destShapeTemp =cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
    destShapewithoutQ =cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);


    for (int m=0;m<(nS+4);m++)
    {
        combineshapevectors[m]=cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
        param[m]=0;

        negNewParam[m]=0;
        negNewParamnS[m]=0;
        negNewParam4[m]=0;
        ParamnS[m]=0;
        Param4[m]=0;
        ptemp1[m]=0;
        ptemp2[m]=0;
    }
    for (int m=0;m<4;m++)
        globalShapeVectors[m]=cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);


    CvMat * meanShapeDel=cvCreateMat(numberofpoints,2,CV_64FC1);
eigenVal=   (CvMat *)cvReadByName( fs,0,"eigenVal",0 );

    // minProjVal=(IplImage *)cvReadByName( fs,0,"minProjVal",0 );
    // maxProjVal=(IplImage *)cvReadByName( fs,0,"maxProjVal",0 );

    meanAppearance=(IplImage *)cvReadByName( fs,0,"avgApp",0 );
    ErrorImage = cvCreateMat((meanAppearance->width*meanAppearance->height),1, CV_64FC1);

    IplImage* meanShapeTemp=(IplImage *)cvReadByName( fs,0,"avgShape",0 );
    meanShape=cvCreateImage(cvSize(meanShapeTemp->width,meanShapeTemp->height),IPL_DEPTH_64F,1);

    cvConvertScale( meanShapeTemp,meanShape,1,0);
    for (int m=0;m<nS;m++)
    {
        char temp[200];
        sprintf(temp,"shapeEigenVectors%d",m);
        nonRigidShapeVectors[m]=(IplImage *)cvReadByName( fs,0,temp,0 );
    }

    for (int m=0;m<nA-1;m++)
    {
        char temp[200];
        sprintf(temp,"appEigenVectors%d",m);
        appearanceVectors[m]=(IplImage *)cvReadByName( fs,0,temp,0 );
    }
        appearanceVectors[nA-1]=cvCreateImage(cvSize(appearanceVectors[nA-2]->width,appearanceVectors[nA-2]->height),IPL_DEPTH_32F,1);


for(int i=0;i<(appearanceVectors[nA-2]->width);i++)
{
    for(int j=0;j<appearanceVectors[nA-2]->height;j++)
    {
     CvScalar s;
     s.val[0]=1;
     cvSet2D(appearanceVectors[nA-1],j,i,s);
    }
}

//   IplImage * newImage = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),IPL_DEPTH_8U,1);
    for (int m=0;m<nA;m++)
    {
        //   cvZero(newImage);

        for (int i=0;i<meanAppearance->width;i++)
        {
            for (int j=0;j<meanAppearance->height;j++)
            {

                CvScalar s= cvGet2D(appearanceVectors[m],j,i);
                s.val[0]*=255;
                CvScalar s1= cvGet2D(meanAppearance,j,i);

                s.val[0]+=s1.val[0];

                ////printf("%e \n",s.val[0]);
                //   cvSet2D(newImage,j,i,s);
            }
        }
        //            // cvNamedWindow("yoyo",1);
        //   cvShowImage("yoyo",newImage);

        //   cvWaitKey(-1);
    }


    for (int m=0;m<numberofpoints;m++)
    {
        CvScalar s1,s2;
        s1=cvGet2D(meanShape,0,2*m);
        s2=cvGet2D(meanShape,0,2*m+1);
        cvSet2D(meanShapeDel,m,0,s1);
        cvSet2D(meanShapeDel,m,1,s2);

        cvSet2D(globalShapeVectors[0],0,2*m,s1);
        cvSet2D(globalShapeVectors[0],0,2*m+1,s2);

        cvSet2D(combineshapevectors[0],0,2*m,s1);
        cvSet2D(combineshapevectors[0],0,2*m+1,s2);


        s2.val[0]*=-1;
        cvSet2D(globalShapeVectors[1],0,2*m,s2);
        cvSet2D(globalShapeVectors[1],0,2*m+1,s1);

        cvSet2D(combineshapevectors[1],0,2*m,s2);
        cvSet2D(combineshapevectors[1],0,2*m+1,s1);

        CvScalar one,zero;
        one.val[0]=1;
        zero.val[0]=0;
        cvSet2D(globalShapeVectors[2],0,2*m,one);
        cvSet2D(globalShapeVectors[2],0,2*m+1,zero);
        cvSet2D(globalShapeVectors[3],0,2*m,zero);
        cvSet2D(globalShapeVectors[3],0,2*m+1,one);
        cvSet2D(combineshapevectors[2],0,2*m,one);
        cvSet2D(combineshapevectors[2],0,2*m+1,zero);
        cvSet2D(combineshapevectors[3],0,2*m,zero);
        cvSet2D(combineshapevectors[3],0,2*m+1,one);
    }


    for (int i=0;i<numberofpoints;i++)
    {
        CvScalar avx,avy;


        avx=cvGet2D(  meanShapeDel, i,0);
        avy=cvGet2D(  meanShapeDel, i,1);

        // ////printf("%e %e \n",avx.val[0],avy.val[0]);

    }

    newDelaunayShapeCompute= new delaunay(meanShapeDel);
    newDelaunay = new delaunay(meanShapeDel);
    newDelaunayInverse = new delaunay(meanShapeDel);
    for (int m=0;m<4;m++)
    {

        double val=   cvNorm(globalShapeVectors[m],NULL , CV_L2, 0 );
        cvConvertScale( globalShapeVectors[m],globalShapeVectors[m],1/val,0);

        ////printf(" %e \n",val);
        //  cvConvertScale( combineshapevectors[m],combineshapevectors[m],val,0);

    }

    for (int m=4;m<(nS+4);m++)
    {


        for (int j=0;j<totalnumberofpoints;j++)
        {
            CvScalar s1 = cvGet2D(nonRigidShapeVectors[m-4],0,j);
            cvSet2D(combineshapevectors[m],0,j,s1);
        }

    }
    for (int m=0;m<nS+4;m++)
    {

        double val=   cvNorm(combineshapevectors[m],NULL , CV_L2, 0 );
        cvConvertScale( combineshapevectors[m],combineshapevectors[m],1/val,0);
        ////printf(" %e \n",val);

        //  cvConvertScale( combineshapevectors[m],combineshapevectors[m],val,0);

    }

    for (int m=0;m<nA;m++)
    {

        double val=   cvNorm(appearanceVectors[m],NULL , CV_L2, 0 );
        cvConvertScale( appearanceVectors[m],appearanceVectors[m],1/val,0);

//        //printf(" %e \n",val);

    }
    ////printf("%d %d \n",meanAppearance->width,meanAppearance->height);
    totalNumberOfPixels=newDelaunay->numberOfPixels();
    interpolateMean=(float*)malloc(sizeof(float)*totalNumberOfPixels);
    for (int i=0;i<totalNumberOfPixels;i++)
    {
        interpolateMean[i]=0;
        pixel * pix = newDelaunay->getpixel(i);
        interpolateMean[i]=interpolate<float>(meanAppearance,double(pix->x+pix->tx),double(pix->y+pix->ty));


    }

    preCompute();
    ////printf("%d %d \n",meanAppearance->width,meanAppearance->height);
//   preCompute();

}
