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
#include "lkFittingSM.h"
#include <delaunay.h>
#include "stdio.h"

#include<aam.h>
void LKInverseComp::iterate()
{
    double maxerror=1000000000;

    //cvNamedWindow("iterate",1);
    //cvShowImage("iterate",iterateImage);
    // printf("Starting First Iteration \n");
    if (iterateImage==NULL)
        return;
    //cvWaitKey(-1);

    int iter=0;
    while (iter<1 )
    {
        for (int m=0;m<(nS+nA+4);m++)
        {
            printf("%d %d \n",m,iter);
            negNewParam[m]=0;
            negNewParamnS[m]=0;
            negNewParam4[m]=0;
            ParamnS[m]=0;
            Param4[m]=0;
            ptemp1[m]=0;
            ptemp2[m]=0;
        }
        iter++;
        //  printf("      \n");

        for (int i=0;i<(nS+nA+4);i++)
        {
            // printf("Current Param's are %e \n",param[i]);
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

        CvMat * current = computeShape(ptemp2);
        CvMat * affShapeVec = cvCreateMat(numberofpoints,2,CV_64FC1);
        for (int i=0;i<numberofpoints;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(current,0,2*i);
            s2 = cvGet2D(current,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }

        newDelaunay->calculateAffineWarpParameters(affShapeVec);

        current = computeShape(ptemp1);


        for (int i=0;i<numberofpoints;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(current,0,2*i);
            s2 = cvGet2D(current,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }
        newDelaunay->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);
        CvMat * ErrorImage = cvCreateMat((meanAppearance->width*meanAppearance->height),1, CV_64FC1);
        cvZero(ErrorImage);
        int t=newDelaunay->numberOfPixels();
        IplImage * imageNew = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),IPL_DEPTH_8U,1);
        double totalerror=0;
        double bottomStepSize=0;
        double topStepSize=0;
        for (int i=0;i<t;i++)
        {
            pixel * pix = newDelaunay->getpixel(i);
            pixel *pix2 =newDelaunay->findCorrespondingPixelInImage(pix);
            if (((double)pix2->x)<(double)iterateImage->width && ((double)pix2->y)<(double)iterateImage->height )
            {
                if ((pix2->y)>=0 && (pix2->x)>=0)
                {
                    float imageValue =interpolate<uchar>(iterateImage,double(pix2->x),double(pix2->y));

                    CvScalar val1,val2,val3;

                    val2.val[0]=0;
                    if ((pix->y+pix->ty)>=0 && (pix->x+pix->tx)>=0)
                    {

                        double totalEigenValue=0;
                        for (int k=(nS+4);k<(nS+nA+4);k++)
                        {
                            int ind=k-nS-4;
                            float imageValue =interpolate<float>(appearanceVectors[ind],double(pix->x+pix->tx),double(pix->y+pix->ty));
                            imageValue*=param[k];
                            totalEigenValue+=imageValue;
                        }

                        val1.val[0]=imageValue+totalEigenValue;

                        float imageValue1 =interpolate<float>(meanAppearance,double(pix->x+pix->tx),double(pix->y+pix->ty));
                        val2.val[0]=imageValue1;
                        val3.val[0] = val1.val[0]-val2.val[0];
                        cvSet2D(imageNew, int(pix->y+pix->ty),int(pix->x+pix->tx),val1);
                        totalerror+=pow(val3.val[0],2);
                        //   printf("Mean %e \n",val3.val[0]);

                        cvSet2D(ErrorImage, int(pix->y+pix->ty)*int(pix->width+10) + int(pix->x+pix->tx),0,val3);
                    }
                    else
                        printf("something is worng \n");

                }
                else
                    printf("something is worng \n");

            }
            else
                printf("something is worng \n");


            free(pix);
            free(pix2);


        }

        if (totalerror>maxerror)
        {
            maxerror=-1;
            return;
        }
        else
            maxerror=totalerror;
        printf("%e \n",totalerror);


        cvZero(steepestDescentImage);
        cvAdd(steepestDescentCoeffX, steepestDescentCoeffY, steepestDescentImage);



        int tot=meanAppearance->width*meanAppearance->height;
        for (int u=0;u<tot;u++)
        {
            //
            double GradT_x;
            double GradT_y;


            CvScalar s;
            s=cvGet2D(GradientX,int(floor(u/meanAppearance->width)),(u%meanAppearance->width));
            GradT_x=s.val[0];
            s=cvGet2D(GradientY,int(floor(u/meanAppearance->width)),(u%meanAppearance->width));
            GradT_y=s.val[0];

            for (int k=(nS+4);k<(nS+nA+4);k++)
            {
                double va=0;
                int ind=k-nS-4;
                int h=int(floor(u/meanAppearance->width));
                int w=(u%meanAppearance->width);
                //   printf("Index %d %d %d \n",ind,h,w);
                CvScalar s1 = cvGet2D(GradientArrayX[ind],h,w);
                va=s1.val[0]*param[k];
                GradT_x+=va;
                s1 = cvGet2D(GradientArrayY[ind],h,w);
                va=s1.val[0]*param[k];
                GradT_y+=va;
            }


            for (int i=0;i<numberofpoints;i++)
            {
                for (int m=0;m<(nS+4);m++)
                {
                    CvScalar currentX = cvGet2D(steepestDescentImage,u,m);
                    CvScalar currentY = cvGet2D(steepestDescentCoeffY,u,m);
                    currentX.val[0]*= ((GradT_x));
                    currentY.val[0]*= ((GradT_y));

                    currentX.val[0]+=currentY.val[0];
                    cvSet2D(steepestDescentImage,u,m,currentX);

                }
            }

        }

        for (int u=0;u<t;u++)
        {

            for (int k=(nS+4);k<(nS+nA+4);k++)
            {
                CvScalar s = cvGet2D(appearanceVectors[k-nS-4],int(floor(u/meanAppearance->width)),(u%meanAppearance->width));
                cvSet2D(steepestDescentImage,u,k,s);
            }
        }


        cvTranspose(steepestDescentImage, steepestDescentImageTranspose);
        cvMatMul(steepestDescentImageTranspose,steepestDescentImage,HessianMatrix);
        cvInvert(HessianMatrix, HessianMatrixInverse);
        cvMatMul(HessianMatrixInverse,steepestDescentImageTranspose,HessianMatrixInverse_steepestDescentImageTranspose);



        CvMat * parameter = cvCreateMat( nS+nA+4,1, CV_64FC1);
        cvMatMul(HessianMatrixInverse_steepestDescentImageTranspose,ErrorImage,parameter);
        for (int i=0;i<(nS+4);i++)
        {
            CvScalar s = cvGet2D(parameter,i,0);
            printf("%e param \n",s.val[0]);
            negNewParam[i] =-1*s.val[0];
            negNewParamnS[i]=0;
            negNewParam4[i]=0;
        }

        for (int i=(nS+4);i<(nS+nA+4);i++)
        {
            CvScalar s = cvGet2D(parameter,i,0);
            negNewParam[i] =s.val[0];
            negNewParamnS[i]=0;
            negNewParam4[i]=0;
        }



        for (int i=0;i<(4);i++)
        {
            negNewParam4[i]= negNewParam[i];

        }
        for (int i=4;i<(nS+4);i++)
        {
            negNewParamnS[i]= negNewParam[i];

        }
        CvMat * currentnS = computeShape(negNewParamnS);  //Modified
        CvMat * current4 = computeShape(negNewParam4);
        for (int i=0;i<numberofpoints;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(currentnS,0,2*i);
            s2 = cvGet2D(currentnS,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);
        }
        newDelaunayInverse->calculateAffineWarpParameters(affShapeVec);
        for (int i=0;i<numberofpoints;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(current4,0,2*i);
            s2 = cvGet2D(current4,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);
        }
        newDelaunayInverse->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);
        IplImage * destShape =cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
        IplImage * destShapeTemp =cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
        IplImage * destShapewithoutQ =cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
        cvZero(destShape);
        cvZero(destShapeTemp);
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
        cvSub(destShape,meanShape,destShapeTemp);
        for (int i=0;i<(nS+4);i++)
        {
            negNewParam4[i]=0;
            Param4[i]=0;
        }

        for (int i=0;i<4;i++)
        {
            double p=0;
            negNewParam4[i]=0;
            for (int j=0;j<totalnumberofpoints;j++)
            {
                CvScalar s1 = cvGet2D(combineshapevectors[i],0,j);
                CvScalar s2 = cvGet2D(destShapeTemp,0,j);
                p+=s1.val[0]*s2.val[0];
            }
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
        for (int i=0;i<nS+4;i++)
        {
            negNewParamnS[i]=0;
            ParamnS[i]=0;
        }
        for (int i=4;i<(nS+4);i++)
        {
            double p=0;
            negNewParamnS[i]=0;
            for (int j=0;j<totalnumberofpoints;j++)
            {
                CvScalar s1 = cvGet2D(combineshapevectors[i],0,j);
                CvScalar s2 = cvGet2D(destShapeTemp,0,j);
                p+=s1.val[0]*s2.val[0];
            }
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
        for (int i=(nS+4);i<(nS+nA+4);i++)
        {
            param[i]+=negNewParam[i];


        }

        cvReleaseMat(&affShapeVec);
        cvReleaseMat(&ErrorImage);
        cvReleaseMat(&parameter);
        cvReleaseMat(&current);
        cvReleaseMat(&currentnS);
        cvReleaseMat(&current4);
        cvReleaseImage(&destShape );
        cvReleaseImage(&destShapeTemp);
        cvReleaseImage(&destShapewithoutQ);
    }

}


void LKInverseComp:: setImage(IplImage* image)
{
    iterateImage=image;
}

void LKInverseComp::setParamtxty(int x,int y)
{
    param[2]=sqrt(numberofpoints)*x;
    param[3]=sqrt(numberofpoints)*y;


}

void LKInverseComp::setParam(float * parameters)
{

    for (int i =0;i<(nS+nA+4);i++)
    {
        param[i]=parameters[i];
    }

}


void LKInverseComp::preCompute()
{
    GradientX = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
    GradientY = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
    cvZero(GradientX);
    cvZero(GradientY);
    GradientArrayX=(IplImage**)cvAlloc(sizeof(IplImage*)*nA);
    GradientArrayY=(IplImage**)cvAlloc(sizeof(IplImage*)*nA);

    for (int i=0;i<nA;i++)
    {
        printf("Index i %d \n",i);
        GradientArrayX[i]=cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
        GradientArrayY[i]=cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
        cvZero(GradientArrayX[i]);
        cvZero(GradientArrayY[i]);
    }

    for (int i=0;i<meanAppearance->width;i++)
    {
        for (int j=0;j<meanAppearance->height;j++)
        {


            for (int k=0;k<nA;k++)
            {


                CvScalar im1;
                CvScalar ip1;

                im1.val[0]=0;
                ip1.val[0]=0;
                //        printf("%d %d %d %d %d %d\n",appearanceVectors[k]->height,appearanceVectors[k]->width,meanAppearance->height,meanAppearance->width,j,i);
                if ((i-1)>=0)
                    im1=cvGet2D(appearanceVectors[k],j,i-1);
                if ((i+1)<appearanceVectors[k]->width)
                    ip1=cvGet2D(appearanceVectors[k],j,i+1);
                CvScalar d;
                d.val[0]=(ip1.val[0]-im1.val[0])/2;
                cvSet2D(GradientArrayX[k],j,i,d);

                im1.val[0]=0;
                ip1.val[0]=0;

                if ((j-1)>=0)
                    im1=cvGet2D(appearanceVectors[k],j-1,i);

                if ((j+1)<appearanceVectors[k]->height)
                    ip1=cvGet2D(appearanceVectors[k],j+1,i);


                d.val[0]=(ip1.val[0]-im1.val[0])/2;

                cvSet2D(GradientArrayY[k],j,i,d);

            }

            CvScalar im1;
            CvScalar ip1;

            im1.val[0]=0;
            ip1.val[0]=0;
            //   printf("%d %d",i,j);
            if ((i-1)>=0)
                im1=cvGet2D(meanAppearance,j,i-1);
            if ((i+1)<meanAppearance->width)
                ip1=cvGet2D(meanAppearance,j,i+1);

            CvScalar k;
            k.val[0]=(ip1.val[0]-im1.val[0])/2;

            cvSet2D(GradientX,j,i,k);

            im1.val[0]=0;
            ip1.val[0]=0;

            if ((j-1)>=0)
                im1=cvGet2D(meanAppearance,j-1,i);

            if ((j+1)<meanAppearance->height)
                ip1=cvGet2D(meanAppearance,j+1,i);


            k.val[0]=(ip1.val[0]-im1.val[0])/2;

            cvSet2D(GradientY,j,i,k);

        }
    }

    HessianMatrix = cvCreateMat(nS+nA+4, nS+nA+4, CV_64FC1);
    HessianMatrixInverse = cvCreateMat(nS+nA+4, nS+nA+4, CV_64FC1);
    HessianMatrixInverse_steepestDescentImageTranspose = cvCreateMat( nS+nA+4,(meanAppearance->width*meanAppearance->height), CV_64FC1);
    steepestDescentImage  = cvCreateMat((meanAppearance->width*meanAppearance->height), nS+nA+4, CV_64FC1);
    steepestDescentImageTranspose  = cvCreateMat(nS+nA+4,(meanAppearance->width*meanAppearance->height), CV_64FC1);
    steepestDescentCoeffX = cvCreateMat((meanAppearance->width*meanAppearance->height),nS+nA+4, CV_64FC1);
    steepestDescentCoeffY = cvCreateMat((meanAppearance->width*meanAppearance->height),nS+nA+4, CV_64FC1);
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

                // printf("%e %e %e %e \n",s1.val[0],newDelaunay->ithNodeCode[m].triangles[k].xi,s2.val[0],newDelaunay->ithNodeCode[m].triangles[k].yi);
                double alphatop=    ((((pix->x ) - newDelaunay->ithNodeCode[m].triangles[k].xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (((pix->y) - newDelaunay->ithNodeCode[m].triangles[k].yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double alphabottom =  (((  newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (( newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double alpha= alphatop/alphabottom;
                double betatop=    ((((pix->y ) - newDelaunay->ithNodeCode[m].triangles[k].yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi)   -  (((pix->x) - newDelaunay->ithNodeCode[m].triangles[k].xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi));
                double betabottom =  (((  newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (( newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double beta= betatop/betabottom;
                double val=1-alpha -beta;
                CvScalar s;
                if (val<0)
                    val=0;
                s.val[0]=val;


                //printf("val %e \n",val);
                if ((pix->y+pix->ty)>=0 && (pix->x+pix->tx)>=0)
                {
                    CvScalar s1= cvGet2D(dw_xi_yi,  int( pix->y+pix->ty) * int(pix->width+10) + int(pix->x + pix->tx ) ,m);
                    //printf("%e svalue \n",s1.val[0]);
                    s.val[0]+=s1.val[0];

                    cvSet2D(dw_xi_yi,  int( pix->y+pix->ty) * int(pix->width+10) + int(pix->x + pix->tx ) ,m ,s );



                    s.val[0]=val;

                    // cvSet2D(newImage, int( pix->y+pix->ty) , int(pix->x + pix->tx ),s );

                }
            }
        }

    }



    int t=meanAppearance->width*meanAppearance->height;
    for (int u=0;u<t;u++)
    {
        //
        // double GradT_x;
        //double GradT_y;
        //CvScalar s;
        // s=cvGet2D(GradientX,int(floor(u/meanAppearance->width)),(u%meanAppearance->width));
        // GradT_x=s.val[0];
        // s=cvGet2D(GradientY,int(floor(u/meanAppearance->width)),(u%meanAppearance->width));
        //GradT_y=s.val[0];

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
                currentX.val[0]+= ((temp));
                temp = s1.val[0]*s3.val[0];
                currentY.val[0]+= ((temp));
                cvSet2D(steepestDescentCoeffX,u,m,currentX);
                cvSet2D(steepestDescentCoeffY,u,m,currentY);
            }
        }

    }



    //printf("% done ");

    //cvTranspose(steepestDescentImage, steepestDescentImageTranspose);
    //cvMatMul(steepestDescentImageTranspose,steepestDescentImage,HessianMatrix);

    //cvInvert(HessianMatrix, HessianMatrixInverse);
    //cvMatMul(HessianMatrixInverse,steepestDescentImageTranspose,HessianMatrixInverse_steepestDescentImageTranspose);


    // inv(Ma) -> Mb


    /*





        for (int i=0; i<meanAppearance->width; i++)
        {

            for (int j=0; j<meanAppearance->height; j++)
            {

                float GradT_x;
                float GradT_y;

                CvScalar s;
                s=cvGet2D(GradientX,j,i);
                GradT_x=s.val[0];
                s=cvGet2D(GradientY,j,i);
                GradT_y=s.val[0];
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 0) = GradT_x*i;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 1) = GradT_y*i;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 2) = GradT_x*j;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 3) = GradT_y*j;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 4) = GradT_x;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 5) = GradT_y;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 6) = GradT_y*j;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 7) = GradT_x;
                CV_MAT_ELEM(*steepestDescentImage, float, i*templateImage->height + j, 8) = GradT_y;
            }
        }

    */


}
CvMat * LKInverseComp::currentEstimate()
{
    float* ptemp1 = (float *)malloc(sizeof(float)*(nS+4));
    float* ptemp2 = (float *)malloc(sizeof(float)*(nS+4));

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

    CvMat * current = computeShape(ptemp2);
    CvMat * affShapeVec = cvCreateMat(numberofpoints,2,CV_64FC1);

    CvMat * shapevec = cvCreateMat(1,totalnumberofpoints,CV_64FC1);
    //printf("Current Shape \n");

    for (int i=0;i<totalnumberofpoints;i++)
    {
        CvScalar s;
        s = cvGet2D(current,0,i);
        cvSet2D(shapevec,0,i,s);

        //printf("Shape %d , %e \n",i,s.val[0]);


    }

    for (int i=0;i<numberofpoints;i++)
    {
        CvScalar s1,s2;
        s1 = cvGet2D(shapevec,0,2*i);
        s2 = cvGet2D(shapevec,0,2*i +1);
        cvSet2D(affShapeVec,i,0,s1);
        cvSet2D(affShapeVec,i,1,s2);

    }

    newDelaunayShapeCompute->calculateAffineWarpParameters(affShapeVec);

    current = computeShape(ptemp1);
    for (int i=0;i<totalnumberofpoints;i++)
    {
        CvScalar s;
        s = cvGet2D(current,0,i);
        cvSet2D(shapevec,0,i,s);

        //   //printf("Shape %d , %e \n",i,s.val[0]);


    }

    for (int i=0;i<numberofpoints;i++)
    {
        CvScalar s1,s2;
        s1 = cvGet2D(shapevec,0,2*i);
        s2 = cvGet2D(shapevec,0,2*i +1);
        cvSet2D(affShapeVec,i,0,s1);
        cvSet2D(affShapeVec,i,1,s2);

    }
    //    //printf("NO ERROR till HERE 1\n");



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


    for (int m=0;m<(4);m++)
    {

        temp = cvCloneImage(combineshapevectors[m]);

        for (int k=0;k<combineshapevectors[m]->width;k++)
        {
            for (int d=0;d<combineshapevectors[m]->height;d++)
            {
                CvScalar s;
                s=cvGet2D(combineshapevectors[m],d,k);
                s.val[0]*=parameters[m];
                cvSet2D(temp,d,k,s);
            }
        }
        cvAdd(temp,currentShape,currentShape,0);
    }

    cvAdd(meanShape,currentShape,currentShape,0);
    CvMat * mat = cvCreateMat(1,totalnumberofpoints,CV_64FC1);
    for (int m=0;m<totalnumberofpoints;m++)
    {
        CvScalar s = cvGet2D(currentShape,0,m);
        cvSet2D(mat,0,m,s);

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
    printf("%d %d \n",nA,nS);
    int sz=nS+nA+4;

    ParamnS =(float *)malloc(sizeof(float)*(nS+nA+4));
    Param4 =(float *)malloc(sizeof(float)*(nS+nA+4));
    ptemp1 = (float *)malloc(sizeof(float)*(nS+nA+4));
    ptemp2 =(float *)malloc(sizeof(float)*(nS+nA+4));
    param = (float *)malloc(sizeof(float)*(nS+nA+4));


    negNewParam = (float *)malloc(sizeof(float)*(nS+nA+4));
    negNewParamnS = (float *)malloc(sizeof(float)*(nS+nA+4));
    negNewParam4 = (float *)malloc(sizeof(float)*(nS+nA+4));

    nonRigidShapeVectors=(IplImage**)cvAlloc(sizeof(IplImage*)*nS);
    appearanceVectors=(IplImage**)cvAlloc(sizeof(IplImage*)*nA);
    globalShapeVectors=(IplImage**)cvAlloc(sizeof(IplImage*)*4);
    combineshapevectors=(IplImage**)cvAlloc(sizeof(IplImage*)*(4+nS));

    for (int m=0;m<(nS+4);m++)
    {
        combineshapevectors[m]=cvCreateImage(cvSize(totalnumberofpoints,1),IPL_DEPTH_64F,1);
    }

    for (int m=0;m<(nS+nA+4);m++)
    {
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

    meanAppearance=(IplImage *)cvReadByName( fs,0,"avgApp",0 );
    meanShape=(IplImage *)cvReadByName( fs,0,"avgShape",0 );
    for (int m=0;m<nS;m++)
    {
        char temp[200];
        sprintf(temp,"shapeEigenVectors%d",m);
        nonRigidShapeVectors[m]=(IplImage *)cvReadByName( fs,0,temp,0 );
    }

    for (int m=0;m<nA;m++)
    {
        char temp[200];
        sprintf(temp,"appEigenVectors%d",m);
        appearanceVectors[m]=(IplImage *)cvReadByName( fs,0,temp,0 );
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

        // //printf("%e %e \n",avx.val[0],avy.val[0]);

    }

    newDelaunayShapeCompute= new delaunay(meanShapeDel);
    newDelaunay = new delaunay(meanShapeDel);
    newDelaunayInverse = new delaunay(meanShapeDel);
    for (int m=0;m<4;m++)
    {

        double val=   cvNorm(globalShapeVectors[m],NULL , CV_L2, 0 );
        cvConvertScale( globalShapeVectors[m],globalShapeVectors[m],1/val,0);

        //printf(" %e \n",val);
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
        //printf(" %e \n",val);

        //  cvConvertScale( combineshapevectors[m],combineshapevectors[m],val,0);

    }

    for (int m=0;m<nA;m++)
    {

        double val=   cvNorm(appearanceVectors[m],NULL , CV_L2, 0 );
        cvConvertScale( appearanceVectors[m],appearanceVectors[m],1/val,0);

//        printf(" %e \n",val);

    }
    //printf("%d %d \n",meanAppearance->width,meanAppearance->height);

    preCompute();
    //printf("%d %d \n",meanAppearance->width,meanAppearance->height);
//   preCompute();

}
