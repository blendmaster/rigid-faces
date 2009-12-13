#include "cv.h"
#include "highgui.h"
#include "pca.h"

PCA::PCA()
{

}



void PCA::runPCA(CvMat ** dataset,int count)
{
    int i;
    CvTermCriteria calcLimit;
    CvSize inputImageSize;

    // set the number of eigenvalues to use
    inputDataSet = (IplImage**)cvAlloc(sizeof(IplImage*) * count);

    IplImage stub, *dst_img;

    for (i=0; i<count; i++)
    {
        inputImageSize.width=dataset[i]->width;
        inputImageSize.height=dataset[i]->height;

        inputDataSet[i] = cvCreateImage(inputImageSize, IPL_DEPTH_8U, 1);

        for (int l=0;l<dataset[i]->width;l++)
        {
            for (int n=0;n<dataset[i]->height;n++)
            {
                CvScalar s;
                s=cvGet2D(dataset[i],n,l);
                cvSet2D(inputDataSet[i],n,l,s);


            }
        }
        // inputDataSet[i]=cvGetImage(dataset[i],&stub);
    }
    nEigens = count-1;

    eigenVectArr = (IplImage**)cvAlloc(sizeof(IplImage*) * nEigens);
    for (i=0; i<nEigens; i++)
        eigenVectArr[i] = cvCreateImage(inputImageSize, IPL_DEPTH_32F, 1);
    eigenValMat = cvCreateMat( 1, nEigens, CV_32FC1 );

    averageInput = cvCreateImage(inputImageSize, IPL_DEPTH_32F, 1);

    calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, nEigens, 1);
    cvCalcEigenObjects(
        count,
        (void*)inputDataSet,
        (void*)eigenVectArr,
        CV_EIGOBJ_NO_CALLBACK,
        0,
        0,

        &calcLimit,
        averageInput,
        eigenValMat->data.fl);
    cvNormalize(eigenValMat, eigenValMat, 1, 0, CV_L1, 0);
}
eigenVectors * PCA::returnEigens()
{

    eigenVectors * newEigen = (eigenVectors *) malloc(sizeof(eigenVectors));
    newEigen->count=nEigens;
    newEigen->eigens=eigenVectArr;
    return newEigen;

}
IplImage * PCA::returnAverage()
{
    for (int i =0;i<averageInput->width;i++)
    {
        for (int j=0;j<averageInput->height;j++)
        {
            CvScalar s;
            s =cvGet2D(averageInput,j,i);
            //printf("%e \n",s.val[0]);

        }
    }
    return averageInput;

}
