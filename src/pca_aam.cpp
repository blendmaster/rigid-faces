
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
#include "cxcore.h"
#include "highgui.h"
#include "pca_aam.h"
#include<aam.h>
PCA_AAM::PCA_AAM()
{

}


CvMat * PCA_AAM::returnEigenVals()
{
    return eigenValMat;

}
void PCA_AAM::runPCA(CvMat ** dataset,int count)
{
    if (count>0)
        {}
    else
        return;
    int datasetSize = dataset[0]->width*dataset[0]->height;
     nEigens =(count>datasetSize)?datasetSize :(count);
    CvMat* tmpEigenValues = cvCreateMat(1, nEigens, CV_64FC1);
    CvMat* tmpEigenVectors = cvCreateMat(nEigens, datasetSize, CV_64FC1);
    CvMat* MeanShape = cvCreateMat(1, datasetSize, CV_64FC1 );
    cvZero(MeanShape);
    CvMat * inputData=cvCreateMat(count, datasetSize, CV_64FC1);
    int i;
    for (i=0; i<count; i++)
    {
        for (int n=0;n<dataset[i]->height;n++)
        {

            for (int l=0;l<dataset[i]->width;l++)
            {


                CvScalar s;
                s=cvGet2D(dataset[i],n,l);
                cvSet2D(inputData,i,(n*dataset[i]->width) + l,s);
                CvScalar t = cvGet2D(MeanShape,0,(n*dataset[i]->width) + l);
                t.val[0]+=s.val[0];
                cvSet2D(MeanShape,0,(n*dataset[i]->width) + l,t);


            }
        }
    }
    for (int n=0;n<dataset[0]->height;n++)
    {

        for (int l=0;l<dataset[0]->width;l++)
        {

            CvScalar t = cvGet2D(MeanShape,0,(n*dataset[0]->width) + l);
            t.val[0]/=count;
            cvSet2D(MeanShape,0,(n*dataset[0]->width) + l,t);

        }
    }

    cvCalcPCA(inputData, MeanShape,tmpEigenValues, tmpEigenVectors, CV_PCA_DATA_AS_ROW);


    eigenValMat = cvCreateMat( 1, nEigens, CV_32FC1 );
    eigenVectArr = (IplImage**)cvAlloc(sizeof(IplImage*) * nEigens);
    for (i=0; i<nEigens; i++)
    {
        eigenVectArr[i] = cvCreateImage(cvSize(dataset[0]->width,dataset[0]->height), IPL_DEPTH_32F, 1);
        CvScalar s;
        s.val[0] = cvmGet(tmpEigenValues, 0, i);
        cvSet2D(eigenValMat,0,i,s);
    }
    averageInput = cvCreateImage(cvSize(dataset[0]->width,dataset[0]->height), IPL_DEPTH_32F, 1);
    for (i=0; i<nEigens; i++)
    {
        for (int n=0;n<dataset[i]->height;n++)
        {

            for (int l=0;l<dataset[i]->width;l++)
            {


                CvScalar s;

                s.val[0]= cvmGet(tmpEigenVectors,i,(n*dataset[i]->width) + l);
                cvSet2D(eigenVectArr[i],n,l,s);

            }
        }
    }
    for (int n=0;n<dataset[0]->height;n++)
    {
        for (int l=0;l<dataset[0]->width;l++)
        {

            CvScalar s;
            s= cvGet2D(MeanShape,0,(n*dataset[0]->width) + l);
            cvSet2D(averageInput,n,l,s);

        }
    }
    cvReleaseMat(&tmpEigenValues);
    cvReleaseMat(&tmpEigenVectors);
    cvReleaseMat(&inputData);
    cvReleaseMat(&MeanShape);
}
eigenVectors_AAM * PCA_AAM::returnEigens()
{
    eigenVectors_AAM * newEigen = (eigenVectors_AAM *) malloc(sizeof(eigenVectors_AAM));
    newEigen->count=nEigens;
    newEigen->eigens=eigenVectArr;
    return newEigen;

}
IplImage * PCA_AAM::returnAverage()
{

    return averageInput;

}
