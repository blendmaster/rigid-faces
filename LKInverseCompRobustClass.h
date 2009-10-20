/*
    COP - Active Appearance Model Face Tracking - using Inverse Compositional Approach
    Rohan Anil , under Dr. Radhika Vatsan , BITS Pilani Goa Campus
    rohan.ani;@gmail.com
    License : GPLV3
*/
#include "cv.h"
#include "highgui.h"

#ifndef _INCL_GUARD_LKINVERSE
#define _INCL_GUARD_LKINVERSE
/*
6 Parameter Affine -2d Lucas Kanade Tracker Class
*/

    const float DeltaE = 1E-7f;
	const int numberOfIterations = 15;

class LKInverseComp
{

public:

    CvMat* WarpMatrix ;

    LKInverseComp(void);
    void iterate();
    void setTemplate(IplImage* image);
    void setImage(IplImage* image);
    void setCurrentWarpEstimate(float p1,float p2,float p3 ,float p4 ,float p5,float p6);
private:
    void preCompute();
    void setWarpMatrix(CvMat* Warp,float p1,float p2,float p3,float p4,float p5,float p6);

    // Variables
    IplImage* templateImage ;
    IplImage* iterateImage ;

    IplImage* GradientX ;
    IplImage* GradientY ;


    CvMat* steepestDescentImage;
    CvMat* steepestDescentImageTranspose ;
    CvMat* errorImage[16] ;
  //  CvMat* steepestDescentImageTranspose_X_errorImage;
    CvMat* steepestDescentImageBlockMatrix[16];
    CvMat* steepestDescentImageTransposeMatrix[16];

    CvMat* HessianBlockMatrix[16];
    int MVALUE[16];
    float errorValues[16];
    CvMat* steepestDescentImageTranspose_X_errorImage_MVALUE ;
CvMat* steepestDescentImageTranspose_X_errorImage_MVALUE_perBlock[16] ;

    CvMat* HessianMatrix ;
    CvMat* HessianMatrixInverse ;
    CvMat* DeltaP ;
    CvMat* WarpMatrixDeltaP;
    CvMat* WarpMatrixDeltaPInverse ;

};

template <class T>
float interpolate(IplImage* pImage, float x, float y)
{
    int xi = cvFloor(x);
    int yi = cvFloor(y);

    float k1 = x-xi;
    float k2 = y-yi;

    int f1 = xi<pImage->width-1;
    int f2 = yi<pImage->height-1;

    T* row1 = &CV_IMAGE_ELEM(pImage, T, yi, xi);
    T* row2 = &CV_IMAGE_ELEM(pImage, T, yi+1, xi);
    float interpolated_value = (1.0f-k1)*(1.0f-k2)*(float)row1[0] + (f1 ? ( k1*(1.0f-k2)*(float)row1[1] ):0) +
                               (f2 ? ( (1.0f-k1)*k2*(float)row2[0] ):0) + ((f1 && f2) ? ( k1*k2*(float)row2[1] ):0) ;

    return interpolated_value;
}
#endif
