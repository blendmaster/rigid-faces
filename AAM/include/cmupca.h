
#include "cv.h"
#include "cvaux.h"

#include "highgui.h"

#ifndef _INCL_GUARD_CMUPCA
#define _INCL_GUARD_CMUPCA
typedef struct eigenVectorsCMUPCA
{
    IplImage ** eigens;
    int count;

}eigenVectorsCMUPCA;

class CMUPCA{


public:
 CMUPCA();


void tqli(double d[] , double e[],int n, double ** z);
void corcol(double **data, int n, int m, double **symmat);
/**  Variance-covariance matrix: creation  *****************************/

void covcol(double ** data, int n,int  m, double **symmat);
/**  Sums-of-squares-and-cross-products matrix: creation  **************/

void scpcol(double **data, int n,int m, double **symmat);
/**  Error handler  **************************************************/

void erhand(char err_msg[]);

/**  Allocation of vector storage  ***********************************/

double *vector(int n);

/**  Allocation of double matrix storage  *****************************/

double **matrix(int n,int m);
/**  Deallocate vector storage  *********************************/

void free_vector(double * v,int n);
void free_matrix(double ** mat,int n,int m);
void tred2(double **a, int n,double * d, double *e);
/**  Tridiagonal QL algorithm -- Implicit  **********************/



void  runPCA(CvMat** dataset,int count);


eigenVectorsCMUPCA * returnEigens();
IplImage * returnAverage();
private:
int height;
int width;
int nEigens;
IplImage** eigenVectArr;
IplImage * eigenValMat;
IplImage * averageInput;
};
#endif
