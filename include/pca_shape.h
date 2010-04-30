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
#include "cvaux.h"

#include "highgui.h"

#ifndef _INCL_GUARD_PCA_AAM
#define _INCL_GUARD_PCA_AAM

typedef struct eigenVectors_AAM
{
    IplImage ** eigens;
    int count;

}eigenVectors_AAM;


class PCA_AAM{


public:
 PCA_AAM();

void runPCA(CvMat ** dataset,int count);
CvMat * returnEigenVals();

eigenVectors_AAM * returnEigens();
IplImage * returnAverage();
private:
int nEigens;
IplImage** eigenVectArr;
IplImage ** inputDataSet;
CvMat * eigenValMat;

IplImage * averageInput;
};


#endif
