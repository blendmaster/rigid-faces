#include "cv.h"
#include "highgui.h"
#include "lkFitting.h"
#include <delaunay.h>
#include "stdio.h"


void LKInverseComp::iterate()
{
    float* negNewParam = (float *)malloc(sizeof(float)*(nS+4));
    float* negNewParamnS = (float *)malloc(sizeof(float)*(nS+4));
    float* negNewParam4 = (float *)malloc(sizeof(float)*(nS+4));
    float* ParamnS = (float *)malloc(sizeof(float)*(nS+4));
    float* Param4 = (float *)malloc(sizeof(float)*(nS+4));
    float* ptemp1 = (float *)malloc(sizeof(float)*(nS+4));
    float* ptemp2 = (float *)malloc(sizeof(float)*(nS+4));
    int iter=0;
    while (iter<1)
    {
        iter++;

/*
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
*/
        CvMat * current = computeShape(param);
        CvMat * affShapeVec = cvCreateMat(40,2,CV_64FC1);

        CvMat * shapevec = cvCreateMat(1,80,CV_64FC1);
        //printf("Current Shape \n");

        for (int i=0;i<80;i++)
        {
            CvScalar s;
            s = cvGet2D(current,0,i);
            cvSet2D(shapevec,0,i,s);

         //   //printf("Shape %d , %e \n",i,s.val[0]);


        }

        for (int i=0;i<40;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(shapevec,0,2*i);
            s2 = cvGet2D(shapevec,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }

        newDelaunay->calculateAffineWarpParameters(affShapeVec);
/*
        current = computeShape(ptemp2);
        for (int i=0;i<80;i++)
        {
            CvScalar s;
            s = cvGet2D(current,0,i);
            cvSet2D(shapevec,0,i,s);

            //   //printf("Shape %d , %e \n",i,s.val[0]);


        }

        for (int i=0;i<40;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(shapevec,0,2*i);
            s2 = cvGet2D(shapevec,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }
        //    //printf("NO ERROR till HERE 1\n");



        newDelaunay->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);

*/



        //   //printf("NO ERROR till HERE 2 \n");

        CvMat * ErrorImage = cvCreateMat((meanAppearance->width*meanAppearance->height),1, CV_64FC1);
        int t=newDelaunay->numberOfPixels();
        IplImage * imageNew = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),IPL_DEPTH_8U,1);
      //printf(" number of pixels %d \n",t);
        for (int i=0;i<t;i++)
        {
            pixel * pix = newDelaunay->getpixel(i);
            pixel *pix2 =newDelaunay->findCorrespondingPixelInImage(pix);
            float imageValue =interpolate<uchar>(iterateImage,float(pix2->x),float(pix2->y));

            CvScalar val1,val2,val3;
            val1.val[0]=imageValue;
            val2.val[0]=0;
            ////printf(" %d %d  %e %e \n",i,t,pix->y+pix->ty,pix->x+pix->tx);
            if ((pix->y+pix->ty)>=0 && (pix->x+pix->tx)>=0)
            {

                val2= cvGet2D(meanAppearance,int(pix->y+pix->ty),int(pix->x+pix->tx));
                val3.val[0] = val1.val[0]-val2.val[0];
                //    //printf("Error Value %e ",val3.val[0]);
                cvSet2D(imageNew, int(pix->y+pix->ty),int(pix->x+pix->tx),val1);

                cvSet2D(ErrorImage, int(pix->y+pix->ty)*int(pix->width+10) + int(pix->x+pix->tx),0,val3);
            }



        }
        static int aaa=0;
        char aaaa[400];
        sprintf(aaaa,"%s%d","warped",aaa++);
        //cvNamedWindow(aaaa,1);
        //cvShowImage(aaaa,imageNew);
                //printf("image done \n");
       // cvWaitKey(-1);
        CvMat * parameter = cvCreateMat( nS+4,1, CV_64FC1);
        cvMatMul(HessianMatrixInverse_steepestDescentImageTranspose,ErrorImage,parameter);
        //printf("New Parameter\n");
        for (int i=0;i<(nS+4);i++)
        {
            CvScalar s = cvGet2D(parameter,i,0);
           // //printf("new param, %d %e \n",i,s.val[0]);

            negNewParam[i] = -1*s.val[0];
            negNewParamnS[i]=0;
            negNewParam4[i]=0;
        }
        for (int i=0;i<(4);i++)
        {
            CvScalar s = cvGet2D(parameter,i,0);
            negNewParam4[i]= -1*s.val[0];

        }
        for (int i=4;i<(nS+4);i++)
        {
            CvScalar s = cvGet2D(parameter,i,0);
            negNewParamnS[i]= -1*s.val[0];

        }
        CvMat * currentnS = computeShape(negNewParam);  //Modified
        CvMat * current4 = computeShape(negNewParam4);
        for (int i=0;i<40;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(currentnS,0,2*i);
            s2 = cvGet2D(currentnS,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }
        // //printf("NO ERROR till HERE 4 \n");

        newDelaunayInverse->calculateAffineWarpParameters(affShapeVec);

      /*  for (int i=0;i<40;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(current4,0,2*i);
            s2 = cvGet2D(current4,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }
        ////printf("NO ERROR till HERE 5 \n");

        newDelaunayInverse->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);
*/
        IplImage * destShape =cvCreateImage(cvSize(80,1),IPL_DEPTH_64F,1);
        IplImage * destShapeTemp =cvCreateImage(cvSize(80,1),IPL_DEPTH_64F,1);
        IplImage * destShapewithoutQ =cvCreateImage(cvSize(80,1),IPL_DEPTH_64F,1);

        cvZero(destShape);
        cvZero(destShapeTemp);
        for (int i=0;i<40;i++)
        {
            double x,y;
            double finalx,finaly;
            finalx=0;
            finaly=0;
            int t=newDelaunayInverse->ithNodeCode[i].count;
            if (t>0)
            {
                int xx=newDelaunayInverse->ithNodeCode[i].triangles[0].xi;
                int yy=newDelaunayInverse->ithNodeCode[i].triangles[0].yi;
                x = xx* newDelaunayInverse->ithNodeCode[i].triangles[0].affine[0] + yy* newDelaunayInverse->ithNodeCode[i].triangles[0].affine[1] + newDelaunayInverse->ithNodeCode[i].triangles[0].affine[2];
                y = xx* newDelaunayInverse->ithNodeCode[i].triangles[0].affine[3] + yy* newDelaunayInverse->ithNodeCode[i].triangles[0].affine[4] + newDelaunayInverse->ithNodeCode[i].triangles[0].affine[5];
            }
       //     else
                //printf("Something is wrong");


            int ind = newDelaunay->ithNodeCode[i].count;
            for (int k=0;k<ind;k++)
            {
                finalx+= x* newDelaunay->ithNodeCode[i].triangles[k].affine[0] + y* newDelaunay->ithNodeCode[i].triangles[k].affine[1] + newDelaunay->ithNodeCode[i].triangles[k].affine[2];
                finaly+= x* newDelaunay->ithNodeCode[i].triangles[k].affine[3] + y* newDelaunay->ithNodeCode[i].triangles[k].affine[4] + newDelaunay->ithNodeCode[i].triangles[k].affine[5];
            }
            finalx/=ind;
            finaly/=ind;

            CvScalar s1,s2;
            s1.val[0]=finalx;
            s2.val[0]=finaly;
            cvSet2D(destShape,0,2*i,s1);
            cvSet2D(destShape,0,2*i+1,s2);
        }
        //   //printf("NO ERROR till HERE 6 \n");

        cvSub(destShape,meanShape,destShapeTemp);


        for (int i=0;i<80;i++)
        {
            CvScalar s;
            s = cvGet2D(destShape,0,i);

            //////printf("Dest Shape %d , %e \n",i,s.val[0]);


        }


        for (int i=0;i<(nS+4);i++)
        {
            negNewParam4[i]=0;
            Param4[i]=0;


        }

        for (int i=0;i<4;i++)
        {
            double p=0;
            negNewParam4[i]=0;
            for (int j=0;j<80;j++)
            {
                CvScalar s1 = cvGet2D(combineshapevectors[i],0,j);
                CvScalar s2 = cvGet2D(destShapeTemp,0,j);
                p+=s1.val[0]*s2.val[0];
            }
            negNewParam4[i]=-1*p;
            Param4[i]=p;
        }
        for (int j=0;j<80;j++)
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
            for (int j=0;j<80;j++)
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


        cvReleaseMat(&affShapeVec);
        cvReleaseMat(&ErrorImage);
        cvReleaseMat(&shapevec);
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
    param[2]=sqrt(40)*x;
    param[3]=sqrt(40)*y;


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

    GradientX = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
    GradientY = cvCreateImage(cvSize(meanAppearance->width,meanAppearance->height),  IPL_DEPTH_32F, 1);
    cvZero(GradientX);
    cvZero(GradientY);

    cvSobel(meanAppearance, GradientX, 1, 0);
    cvSobel(meanAppearance, GradientY, 0, 1);
    cvConvertScale(GradientX, GradientX, 0.2);
    cvConvertScale(GradientY, GradientY, 0.2);
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
    CvMat * dw_xi_yi = cvCreateMat((meanAppearance->width*meanAppearance->height),40, CV_64FC1);
    //printf("%d %d \n", meanAppearance->height, meanAppearance->width );
    cvZero(dw_xi_yi);
    for (int m=0;m<40;m++)
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
                double alphatop=    ((((pix->x ) - newDelaunay->ithNodeCode[m].triangles[k].xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (((pix->y) - newDelaunay->ithNodeCode[m].triangles[k].yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double alphabottom =  (((  newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (( newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double alpha= alphatop/alphabottom;
                double betatop=    ((((pix->y ) - newDelaunay->ithNodeCode[m].triangles[k].yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi)   -  (((pix->x) - newDelaunay->ithNodeCode[m].triangles[k].xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi));
                double betabottom =  (((  newDelaunay->ithNodeCode[m].triangles[k].diff_xj_xi )*newDelaunay->ithNodeCode[m].triangles[k].diff_yk_yi)   -  (( newDelaunay->ithNodeCode[m].triangles[k].diff_yj_yi )*newDelaunay->ithNodeCode[m].triangles[k].diff_xk_xi));
                double beta= betatop/betabottom;
                double val=1-alpha -beta;
                CvScalar s;
                s.val[0]=val;
                if ((pix->y+pix->ty)>=0 && (pix->x+pix->tx)>=0)
                    cvSet2D(dw_xi_yi,  int( pix->y+pix->ty) * int(pix->width+10) + int(pix->x + pix->tx ) ,m ,s );
            }
        }
    }



    int t=meanAppearance->width*meanAppearance->height;
    for (int i=0;i<40;i++)
    {
        for (int m=0;m<(nS+4);m++)
        {

            for (int u=0;u<t;u++)
            {


                double GradT_x;
                double GradT_y;

                CvScalar s;
                s=cvGet2D(GradientX,int(u/meanAppearance->width),(u%meanAppearance->width));
                GradT_x=s.val[0];
                s=cvGet2D(GradientY,int(u/meanAppearance->width),(u%meanAppearance->width));
                GradT_y=s.val[0];


                ////printf("%d %d %d\n",m,u,i);
                CvScalar currentX = cvGet2D(steepestDescentCoeffX,u,m);
                CvScalar currentY = cvGet2D(steepestDescentCoeffY,u,m);
                CvScalar s1,s2,s3;
                s1 =cvGet2D(dw_xi_yi,u,i);
                s2 =cvGet2D(combineshapevectors[m],0,2*i);
                s3 =cvGet2D(combineshapevectors[m],0,2*i + 1);
                double temp = s1.val[0]*s2.val[0];

                currentX.val[0]+=  GradT_x*(temp);
                temp = s1.val[0]*s3.val[0];
                currentY.val[0]+=  GradT_y*(temp);

                cvSet2D(steepestDescentCoeffX,u,m,currentX);
                cvSet2D(steepestDescentCoeffY,u,m,currentY);

            }
        }

    }
    //printf("% done ");
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
                hei =(h/meanAppearance->width);
                wid =(h%meanAppearance->width);
                CvScalar s2;
                s2=cvGet2D(appearanceVectors[l],hei,wid);
                CvScalar s;
                s=cvGet2D(steepestDescentImage,h,m);
                total+= s2.val[0]*s.val[0];


            }
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

            for (int h=0;h<t;h++)
            {
                CvScalar s;
                s=cvGet2D(steepestDescentImage,h,m);
                CvScalar s2;
                s2=cvGet2D(app,h,0);
                s.val[0] -=s2.val[0];
                cvSet2D(steepestDescentImage,h,m,s);


            }


        }


    }
    cvTranspose(steepestDescentImage, steepestDescentImageTranspose);
    cvMatMul(steepestDescentImageTranspose,steepestDescentImage,HessianMatrix);

    cvInvert(HessianMatrix, HessianMatrixInverse);
    cvMatMul(HessianMatrixInverse,steepestDescentImageTranspose,HessianMatrixInverse_steepestDescentImageTranspose);


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

        CvMat * current = computeShape(ptemp1);
        CvMat * affShapeVec = cvCreateMat(40,2,CV_64FC1);

        CvMat * shapevec = cvCreateMat(1,80,CV_64FC1);
        //printf("Current Shape \n");

        for (int i=0;i<80;i++)
        {
            CvScalar s;
            s = cvGet2D(current,0,i);
            cvSet2D(shapevec,0,i,s);

            //printf("Shape %d , %e \n",i,s.val[0]);


        }

        for (int i=0;i<40;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(shapevec,0,2*i);
            s2 = cvGet2D(shapevec,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }

        newDelaunayShapeCompute->calculateAffineWarpParameters(affShapeVec);

        current = computeShape(ptemp2);
        for (int i=0;i<80;i++)
        {
            CvScalar s;
            s = cvGet2D(current,0,i);
            cvSet2D(shapevec,0,i,s);

            //   //printf("Shape %d , %e \n",i,s.val[0]);


        }

        for (int i=0;i<40;i++)
        {
            CvScalar s1,s2;
            s1 = cvGet2D(shapevec,0,2*i);
            s2 = cvGet2D(shapevec,0,2*i +1);
            cvSet2D(affShapeVec,i,0,s1);
            cvSet2D(affShapeVec,i,1,s2);

        }
        //    //printf("NO ERROR till HERE 1\n");



        newDelaunayShapeCompute->calculateAffineWarpParametersComposeWithCurrent(affShapeVec);

    CvMat * mat = cvCreateMat(1,80,CV_64FC1);

    for (int i=0;i<40;i++)
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



    IplImage * currentShape = cvCreateImage(cvSize(80,1),IPL_DEPTH_64F,1);
    cvZero(currentShape);

    IplImage * temp;


    for (int m=0;m<4;m++)
    {
        temp = cvCloneImage(combineshapevectors[m]);
        cvConvertScale( temp,temp,parameters[m],0);
        cvAdd(temp,currentShape,currentShape,0);
    }
    for (int m=4;m<nS+4;m++)
    {
        temp = cvCloneImage(combineshapevectors[m]);
        cvConvertScale( temp,temp,parameters[m],0);
        cvAdd(temp,currentShape,currentShape,0);
    }
    cvAdd(meanShape,currentShape,currentShape,0);
    CvMat * mat = cvCreateMat(1,80,CV_64FC1);
    for (int m=0;m<80;m++)
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
    //printf("%d %d \n",nA,nS);
    param = (float *) malloc((4+nS)*sizeof(float));
    nonRigidShapeVectors=(IplImage**)cvAlloc(sizeof(IplImage*) * nS);
    appearanceVectors=(IplImage**)cvAlloc(sizeof(IplImage*) * nA);
    globalShapeVectors=(IplImage**)cvAlloc(sizeof(IplImage*) * 4);
nA=0;
    combineshapevectors=(IplImage**)cvAlloc(sizeof(IplImage*) * (4+nS));
    for (int m=0;m<(nS+4);m++)
    {
        combineshapevectors[m]=cvCreateImage(cvSize(80,1),IPL_DEPTH_64F,1);
        param[m]=0;
    }

    for (int m=0;m<4;m++)
        globalShapeVectors[m]=cvCreateImage(cvSize(80,1),IPL_DEPTH_64F,1);



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

    CvMat * meanShapeDel=cvCreateMat(40,2,CV_64FC1);

    meanAppearance=(IplImage *)cvReadByName( fs,0,"avgApp",0 );
    meanShape=(IplImage *)cvReadByName( fs,0,"avgShape",0 );


    for (int m=0;m<40;m++)
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


    for (int i=0;i<40;i++)
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


        for (int j=0;j<80;j++)
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
    //printf("%d %d \n",meanAppearance->width,meanAppearance->height);

    preCompute();
    //printf("%d %d \n",meanAppearance->width,meanAppearance->height);
//   preCompute();

}
