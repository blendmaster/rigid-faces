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
    int nA,nS;
private:
    void preCompute();
    float* negNewParam;
    float* negNewParamnS;
    float* negNewParam4;
    float* ParamnS;
    float* Param4;
    float* ptemp1;
    float* ptemp2;
    IplImage** GradientArrayX;
    IplImage** GradientArrayY;
    CvMat* errorImage ;
    CvMat* HessianMatrixInverse_steepestDescentImageTranspose ;
    CvMat* HessianMatrix ;
    CvMat* HessianMatrixInverse ;

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
    CvMat * steepestDescentCoeffX;
        CvMat * steepestDescentCoeffY;
    CvMat* steepestDescentImage;
    CvMat* steepestDescentImageTranspose ;



};

#endif
