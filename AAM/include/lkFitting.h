#include "cv.h"
#include "highgui.h"

#include <delaunay.h>
#ifndef _INCL_GUARD_LKINVERSE
#define _INCL_GUARD_LKINVERSE


const float DeltaE = 1E-5f;
const int numberOfIterations = 25;

class LKInverseComp
{

public:

    CvMat* WarpMatrix ;

    LKInverseComp(char *path);
    void iterate();
//   void setTemplate(IplImage* image);
    void setImage(IplImage* image);
  void setParamtxty(int x,int y);
    void setParam(float * parameters);
    CvMat * computeShape(float *);
        float * param;
CvMat * currentEstimate();

private:
    void preCompute();
    //  void setWarpMatrix(CvMat* Warp,float p1,float p2,float p3,float p4,float p5,float p6);
    int nA,nS;

    // Variables
    IplImage* templateImage ;
    IplImage* iterateImage ;
    IplImage* GradientX;
    IplImage* GradientY;
    IplImage * shapeParameters;
    IplImage ** globalShapeVectors;
    IplImage ** nonRigidShapeVectors;
    IplImage ** appearanceVectors;
       IplImage **  combineshapevectors;
    IplImage* meanShape;
    IplImage* meanAppearance;
    delaunay * newDelaunay;
        delaunay * newDelaunayInverse;
        delaunay * newDelaunayShapeCompute;

    CvMat* steepestDescentImage;
    CvMat* steepestDescentImageTranspose ;
    CvMat* errorImage ;
    //  CvMat* steepestDescentImageTranspose_X_errorImage;
    CvMat* HessianMatrixInverse_steepestDescentImageTranspose ;
    CvMat* HessianMatrix ;
    CvMat* HessianMatrixInverse ;
    CvMat* DeltaP ;
    CvMat* WarpMatrixDeltaP;
    CvMat* WarpMatrixDeltaPInverse ;
};

#endif
