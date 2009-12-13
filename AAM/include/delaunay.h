#include "cv.h"
#include "highgui.h"

#ifndef _INCL_GUARD_DEL
#define _INCL_GUARD_DEL

typedef struct barycentric
{
double diff_yk_yi;
double diff_yj_yi;
double diff_xk_xi;
double diff_xj_xi;
double affine[6];
double xi;
double yi;

int colorcode;
}barycentric;



typedef struct barycentrics
{
barycentric * triangles;
int nodenumber;
int count;
}barycentrics;


typedef struct aamImage
{

    CvMat* shape;
    IplImage* image;

}aamImage;



typedef struct pixel
{
    double x;
    double y;
    double a1;
    double a2;
    double a3;
    double a4;
    double a5;
    double a6;
    double tx;
    double ty;
    double type;

    double width;
    double height;


}pixel;


template <class T>
double interpolate(IplImage* pImage, double x, double y)
{
    int xi = cvFloor(x);
    int yi = cvFloor(y);

    double k1 = x-xi;
    double k2 = y-yi;

    int f1 = xi<pImage->width-1;
    int f2 = yi<pImage->height-1;

    T* row1 = &CV_IMAGE_ELEM(pImage, T, yi, xi);
    T* row2 = &CV_IMAGE_ELEM(pImage, T, yi+1, xi);
    double interpolated_value = (1.0f-k1)*(1.0f-k2)*(double)row1[0] +
                                (f1 ? ( k1*(1.0f-k2)*(double)row1[1] ):0) +
                                (f2 ? ( (1.0f-k1)*k2*(double)row2[0] ):0) +
                                ((f1 && f2) ? ( k1*k2*(double)row2[1] ):0) ;

    return interpolated_value;
}

class delaunay
{
public:
//done
    delaunay(CvMat* shape);


    aamImage* warpImageToMeanShape(aamImage*input);

    void calculateAffineWarpParameters(CvMat *targetVectors);
    void calculateAffineWarpParametersComposeWithCurrent(CvMat *targetVectors);
    pixel* getpixel(int num);

    void showRasterImage();
    int numberOfPixels();

    int **pixelCode;
    double **warpTriangleCode;
    int **colorTriangleCode;
    int **colorTriangleCodeFixed;
    barycentrics * ithNodeCode;
    pixel* findCorrespondingPixelInImage(pixel* pix);
    double width;
    double height;
        int totalNumberofTriangles;
    int totalNumberOfPixels;
private:
    void draw_subdiv_facet( IplImage* img, CvSubdiv2DEdge edge, int k);
    int findInMeanShape(double x,double y);

    CvRect rect;
    CvMemStorage* storage;
    CvSubdiv2D* subdiv;

    CvSubdiv2D* init_delaunay( CvMemStorage* storage,
                               CvRect rect );
    IplImage * rasterImage;
    double tx,ty;
    CvMat* meanShape;
    CvMat* meanImage;
    pixel * eachPixel;





};


#endif
