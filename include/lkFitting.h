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

#include <delaunay.h>
#ifndef _INCL_GUARD_LKINVERSE
#define _INCL_GUARD_LKINVERSE


const float DeltaE = 1E-5f;
const int numberOfIterations = 25;

class LKInverseComp
{

public:
    delaunay * newDelaunay;
        delaunay * newDelaunayInverse;
        delaunay * newDelaunayShapeCompute;
    CvMat* WarpMatrix ;

    LKInverseComp(char *path);
    double iterate();
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
    //  void setWarpMatrix(CvMat* Warp,float p1,float p2,float p3,float p4,float p5,float p6);

    IplImage* minProjVal;
    IplImage* maxProjVal;
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
float coeffGlobal[4];
    float* negNewParam;
    float* negNewParamnS ;
    float* negNewParam4 ;
    float* ParamnS ;
    float* Param4 ;
    float* ptemp1;
    float* ptemp2 ;
private:
IplImage * srcShape ;
int totalNumberOfPixels;
        double totalerror;
        double bottomStepSize;
        double topStepSize;
float* interpolateMean;
CvMat* eigenVal;
        IplImage * destShape;
        IplImage * destShapeTemp;
        IplImage * destShapewithoutQ ;
                CvMat * ErrorImage  ;


};
#endif
