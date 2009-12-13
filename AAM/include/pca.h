
#include "cv.h"
#include "cvaux.h"

#include "highgui.h"

#ifndef _INCL_GUARD_PCA
#define _INCL_GUARD_PCA

typedef struct eigenVectors
{
    IplImage ** eigens;
    int count;

}eigenVectors;


class PCA{


public:
 PCA();

void runPCA(CvMat ** dataset,int count);
eigenVectors * returnEigens();
IplImage * returnAverage();
private:
int nEigens;
IplImage** eigenVectArr;
IplImage ** inputDataSet;
CvMat * eigenValMat;

IplImage * averageInput;
};


#endif
