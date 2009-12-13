#include<delaunay.h>
#include<stdio.h>
#include<math.h>
int delaunay::findInMeanShape(double x,double y)
{
    CvScalar x1,y1;
    x1=cvGet2D(meanShape,0,0);
    y1=cvGet2D(meanShape,0,1);

    double minx =fabs(x1.val[0]-x);
    double miny =fabs(y1.val[0]-y);

    double indx=-1,indy=-1;

    for (int i=0;i<meanShape->height;i++)
    {
        x1=cvGet2D(meanShape,i,0);
        y1=cvGet2D(meanShape,i,1);
        x1.val[0]+=tx;
        y1.val[0]+=ty;
//printf("X %e  %e \n",x,y,x1.val[0]);

        if (minx>fabs(x1.val[0]-x))
        {
            minx=fabs(x1.val[0]-x);
            indx=i;
        }

        if (miny>fabs(y1.val[0]-y))
        {
            indy=i;

            miny=fabs(y1.val[0]-y);
        }
        if (x1.val[0]==x && y1.val[0]==y)
        {
            return 1;
        }
    }


    if (minx<5E-1f && miny<5E-1f && indx==indy)
        return indx;

    return -1;
}

void delaunay::draw_subdiv_facet( IplImage* img, CvSubdiv2DEdge edge, int k )
{

    CvSubdiv2DEdge t = edge;
    int i, count = 0;
    CvPoint* buf = 0;
    int arr[3];
    int g=0;

    do
    {
        CvSubdiv2DPoint* a = cvSubdiv2DEdgeDst(t);
        CvSubdiv2DPoint* b = cvSubdiv2DEdgeOrg(t);
        //  printf("%e %e \n",a->pt.x,a->pt.y);

        //cvCircle(rasterImage, cvPoint(a->pt.x -tx,a->pt.y-ty), 4, CV_RGB(255,255,255), 4 );

        if (findInMeanShape(a->pt.x,a->pt.y)>=0)
        {

            if (findInMeanShape(b->pt.x,b->pt.y)>=0)
            {
                arr[g]= findInMeanShape(a->pt.x,a->pt.y);
                g++;
                // printf("true1 \n");
                //if (findInMeanShape(b->pt.x,b->pt.y)==1)
                //   {
                count++;
            }
            // }
        }
        t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );
    }
    while (t != edge );



    for (i=0;i<3;i++)
    {
        for (int l=0;l<3-i-1;l++)
        {
            int m;

            if (arr[l]>arr[l+1])
            {
                m=arr[l];
                arr[l]=arr[l+1];
                arr[l+1]=m;
            }


        }
    }
//printf("Total Count %d \n",count);
    buf = (CvPoint*)malloc( count * sizeof(buf[0]));

    // gather points
    t = edge;
    if (count!=3)
        return;

    for ( i = 0; i < count; i++ )
    {
        CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg( t );
        if ( !pt ) break;
        buf[i] = cvPoint( cvRound(pt->pt.x), cvRound(pt->pt.y));

//printf("C %d %d \n",buf[i].x,buf[i].y);
        t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );
    }




    if ( i == count )
    {
        colorTriangleCode[k][0]=arr[0];
        colorTriangleCode[k][1]=arr[1];
        colorTriangleCode[k][2]=arr[2];
        // printf("%d %d %d \n",colorTriangleCode[k][0],colorTriangleCode[k][1],colorTriangleCode[k][2]);


        cvFillConvexPoly( img,buf, count,CV_RGB(k,k,k), 0, 0 );
    }

    free( buf );


}

CvSubdiv2D* delaunay::init_delaunay( CvMemStorage* storage,
                                     CvRect rect )
{
    CvSubdiv2D* subdiv;

    subdiv = cvCreateSubdiv2D( CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
                               sizeof(CvSubdiv2DPoint),
                               sizeof(CvQuadEdge2D),
                               storage );
    cvInitSubdivDelaunay2D( subdiv, rect );

    return subdiv;
}

delaunay::delaunay(CvMat* shape)
{

    CvScalar minx,miny,maxx,maxy;
    minx=cvGet2D(shape,0,0);
    miny=cvGet2D(shape,0,1);
    maxx=minx;
    maxy=miny;
    meanShape = cvCreateMat(shape->height,2, CV_64FC1 );
    storage = cvCreateMemStorage(0);



    for (int i = 0; i <  shape->height; i++ )
    {


        CvScalar x,y;
        x=cvGet2D(shape,i,0);

        if (x.val[0]<minx.val[0])
            minx.val[0]=x.val[0];

        if (x.val[0]>maxx.val[0])
            maxx.val[0]=x.val[0];

        y=cvGet2D(shape,i,1);

        if (y.val[0]<miny.val[0])
            miny.val[0]=y.val[0];

        if (y.val[0]>maxy.val[0])
            maxy.val[0]=y.val[0];

        cvSet2D(meanShape,i,0,x);
        cvSet2D(meanShape,i,1,y);

//printf("%e %e \n",x.val[0],y.val[0]);



    }

    tx=-1*minx.val[0];
    ty=-1*miny.val[0];
    printf("%e %e \n",tx,ty);

    rect.x=0;
    rect.y=0;

    rect.width=maxx.val[0]-minx.val[0]+10;
    rect.height=maxy.val[0]-miny.val[0]+10;
    width=rect.width-10;
    height=rect.height-10;
    printf("%d %d %d \n", shape->height,rect.width,rect.height);
    subdiv = init_delaunay( storage, rect );

    rasterImage=   cvCreateImage( cvSize( rect.width, rect.height),IPL_DEPTH_8U, 3);

    cvZero(rasterImage);

    for (int i = 0; i <  shape->height; i++ )
    {
        CvScalar x,y;
        x=cvGet2D(shape,i,0);
        y=cvGet2D(shape,i,1);


        CvPoint2D32f fp = cvPoint2D32f(tx+ x.val[0],ty+y.val[0]);

        cvSubdivDelaunay2DInsert(subdiv,fp);
    }
    // printf("no error \n");

    CvSeqReader  reader;
    int i, total = subdiv->edges->total;
    int elem_size = subdiv->edges->elem_size;
    //alcSubdivVoronoi2D( subdiv );
    cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );

    colorTriangleCode = (int **)malloc(total*4*sizeof(int*));
    for ( i = 0; i < 4*total; i++ )
    {
        colorTriangleCode[i]= (int *)malloc(3*sizeof(int));
    }

    int k=0;
    for ( i = 0; i < total; i++ )
    {
        CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

        if ( CV_IS_SET_ELEM( edge ))
        {
            CvSubdiv2DEdge e = (CvSubdiv2DEdge)edge;
            k++;
            draw_subdiv_facet( rasterImage, cvSubdiv2DRotateEdge( e, 0 ),k);
            k++;
            draw_subdiv_facet( rasterImage, cvSubdiv2DRotateEdge( e, 2 ),k);
        }
        CV_NEXT_SEQ_ELEM( elem_size, reader );
    }

    int * arrayT=(int *)malloc(k*sizeof(int));

    int count=1;


    for (i=0;i<k;i++)
        arrayT[i]=0;
    int pixelCount=0;
    for (i=0;i<rasterImage->height;i++)
    {

        for (int j=0;j<rasterImage->width;j++)
        {
            CvScalar s;
            s=cvGet2D(rasterImage,i,j);
            if (s.val[0]!=0)
            {
                pixelCount++;
                if (arrayT[int(s.val[0])-1]==0)
                {
                    //  printf("%d \n",int(s.val[0]));
                    arrayT[int(s.val[0])-1]=count;
                    s.val[0]=count;
                    count++;
                }
                else
                {
                    s.val[0]=arrayT[int(s.val[0])-1];
                }

            }

            s.val[1]=0;
            s.val[2]=0;

            //  s.val[0]=count;
            cvSet2D(rasterImage,i,j,s);
        }
    }

    totalNumberOfPixels=pixelCount;
    pixelCode = (int **)malloc((totalNumberOfPixels)*sizeof(int*));
    for (int m=0;m<pixelCount;m++)
        pixelCode[m] = (int *)malloc((3)*sizeof(int));

    int in=0;

    for (i=0;i<rasterImage->height;i++)
    {

        for (int j=0;j<rasterImage->width;j++)
        {

            CvScalar s;
            s=cvGet2D(rasterImage,i,j);
            if (s.val[0]!=0)
            {
                pixelCode[in][0]=s.val[0];
                pixelCode[in][1]=i;
                pixelCode[in][2]=j;

                in++;

            }
        }

    }


    printf("%d \n",count);

    colorTriangleCodeFixed = (int **)malloc((count)*sizeof(int*));
    // for ( i = 0; i < count; i++ )
    //  {
    //colorTriangleCodeFixed[i]= (int *)malloc(3*sizeof(int));


    //  }

    for (i=0;i<k;i++)
    {
        if (arrayT[i]>0)
        {
            colorTriangleCodeFixed[arrayT[i]]= colorTriangleCode[i+1];


        }
    }

    for ( i = 1; i < count; i++ )
    {

        printf(" %d %d %d %d \n",i,colorTriangleCodeFixed[i][0],colorTriangleCodeFixed[i][1],colorTriangleCodeFixed[i][2]);
    }




    ithNodeCode = (barycentrics*)malloc(sizeof(barycentrics)* (shape->height));

    barycentric temp[20];


    int ind=0;

    for ( int w = 0; w < shape->height; w++ )
    {
        ind =0;
        //   ithNodeCode[i]->triangles =
        for (int j=0;j<3;j++)
        {
            for ( i = 1; i < count; i++ )
            {
                int m =  colorTriangleCodeFixed[i][j];
                int kk,jj;
                if (m==w)
                {
//printf("%d \n",w);
                    CvScalar x = cvGet2D(shape,w,0);
                    CvScalar y = cvGet2D(shape,w,1);

                    if (j==0)
                    {
                        kk=1;
                        jj=2;

                    }
                    else if (j==1)
                    {
                        kk=0;
                        jj=2;
                    }
                    else if (j==2)
                    {
                        kk=0;
                        jj=1;
                    }
                    int kthnode =  colorTriangleCodeFixed[i][kk];
                    int jthnode =  colorTriangleCodeFixed[i][jj];
                    //printf(" %d %d \n",kthnode,jthnode);
                    CvScalar xk = cvGet2D(shape,kthnode,0);
                    CvScalar yk = cvGet2D(shape,kthnode,1);
                    CvScalar xj = cvGet2D(shape,jthnode,0);
                    CvScalar yj = cvGet2D(shape,jthnode,1);
                    temp[ind].diff_xj_xi= xj.val[0] - x.val[0];
                    temp[ind].diff_yj_yi= yj.val[0] - y.val[0];
                    temp[ind].diff_xk_xi= xk.val[0] - x.val[0];
                    temp[ind].diff_yk_yi= yk.val[0] - x.val[0];
                    temp[ind].xi=x.val[0];
                    temp[ind].yi=y.val[0];
                    temp[ind].colorcode=i;
                    ind++;
                }
            }





        }

        ithNodeCode[w].triangles =  (barycentric*)malloc(sizeof(barycentric)* (ind));
        for (int b =0 ;b<ind ;b++)
        {
            ithNodeCode[w].triangles[b].diff_xj_xi =     temp[b].diff_xj_xi;
            ithNodeCode[w].triangles[b].diff_yj_yi =     temp[b].diff_yj_yi;
            ithNodeCode[w].triangles[b].diff_xk_xi =     temp[b].diff_xk_xi;
            ithNodeCode[w].triangles[b].diff_yk_yi =     temp[b].diff_yk_yi;
            ithNodeCode[w].triangles[b].xi         =     temp[b].xi;
            ithNodeCode[w].triangles[b].yi         =     temp[b].yi;
            ithNodeCode[w].triangles[b].colorcode  =     temp[b].colorcode;
            ithNodeCode[w].triangles[b].affine[0]  =     1;
            ithNodeCode[w].triangles[b].affine[1]  =     0;
            ithNodeCode[w].triangles[b].affine[2]  =     0;
            ithNodeCode[w].triangles[b].affine[3]  =     0;
            ithNodeCode[w].triangles[b].affine[4]  =     1;
            ithNodeCode[w].triangles[b].affine[5]  =     0;


            ithNodeCode[w].count                    =     ind;
            ithNodeCode[w].nodenumber                     =     w;


        }
    }


//    printf("%d %d \n",count-1,shape->height);
    totalNumberofTriangles=count-1;

    warpTriangleCode=(double **)malloc((totalNumberofTriangles+1)*sizeof(double*));
    for (int m=0;m<totalNumberofTriangles+1;m++)
        warpTriangleCode[m]=(double *)malloc((6)*sizeof(double));




    showRasterImage();



}
void delaunay:: showRasterImage()
{
    cvNamedWindow("test",1);
    cvShowImage("test",rasterImage);

    //cvWaitKey(-1);

}
aamImage*
delaunay:: warpImageToMeanShape(aamImage*input)
{

}

void
delaunay:: calculateAffineWarpParameters(CvMat *targetVectors)
{


    for (int i = 1; i <= totalNumberofTriangles; i++ )
    {


        int node1=colorTriangleCodeFixed[i][0];
        int node2=colorTriangleCodeFixed[i][1];
        int node3=colorTriangleCodeFixed[i][2];
//printf("%d %d %d \n",node1,node2,node3);
        CvScalar mx1,my1,sx1,sy1;
        CvScalar mx2,my2,sx2,sy2;
        CvScalar mx3,my3,sx3,sy3;
        mx1=cvGet2D(meanShape,node1,0);
        my1=cvGet2D(meanShape,node1,1);

        mx2=cvGet2D(meanShape,node2,0);
        my2=cvGet2D(meanShape,node2,1);

        mx3=cvGet2D(meanShape,node3,0);
        my3=cvGet2D(meanShape,node3,1);
///printf("NO ERRROR TILL MEAN \n");

        sx1=cvGet2D(targetVectors,node1,0);
        sy1=cvGet2D(targetVectors,node1,1);

        sx2=cvGet2D(targetVectors,node2,0);
        sy2=cvGet2D(targetVectors,node2,1);

        sx3=cvGet2D(targetVectors,node3,0);
        sy3=cvGet2D(targetVectors,node3,1);
///printf("NO ERRROR TILL TARGET \n");


        CvMat * A = cvCreateMat(3,3,CV_64FC1 );
        CvMat * AINV = cvCreateMat(3,3,CV_64FC1 );
        CvScalar one;
        one.val[0]=1;
        cvSet2D(A,0,0,mx1);
        cvSet2D(A,1,0,mx2);
        cvSet2D(A,2,0,mx3);

        cvSet2D(A,0,1,my1);
        cvSet2D(A,1,1,my2);
        cvSet2D(A,2,1,my3);

        cvSet2D(A,0,2,one);
        cvSet2D(A,1,2,one);
        cvSet2D(A,2,2,one);

        cvInvert(A,AINV,CV_LU);

        CvMat * B1 = cvCreateMat(3,1,CV_64FC1 );
        CvMat * B2 = cvCreateMat(3,1,CV_64FC1 );

        CvMat * affine1 = cvCreateMat(3,1,CV_64FC1 );
        CvMat * affine2 = cvCreateMat(3,1,CV_64FC1 );

        cvSet2D(B1,0,0,sx1);
        cvSet2D(B1,1,0,sx2);
        cvSet2D(B1,2,0,sx3);

        cvSet2D(B2,0,0,sy1);
        cvSet2D(B2,1,0,sy2);
        cvSet2D(B2,2,0,sy3);

        cvMatMul(AINV,B1,affine1);
        cvMatMul(AINV,B2,affine2);


        CvScalar a1,a2,a3,a4,a5,a6;
///printf("NO ERRROR TILL AFFINE \n");
        a1=cvGet2D(affine1,0,0);
        a2=cvGet2D(affine1,1,0);
        a3=cvGet2D(affine1,2,0);
        a4=cvGet2D(affine2,0,0);
        a5=cvGet2D(affine2,1,0);
        a6=cvGet2D(affine2,2,0);

        warpTriangleCode[i][0]=a1.val[0];
        warpTriangleCode[i][1]=a2.val[0];
        warpTriangleCode[i][2]=a3.val[0];
        warpTriangleCode[i][3]=a4.val[0];
        warpTriangleCode[i][4]=a5.val[0];
        warpTriangleCode[i][5]=a6.val[0];


        int w,ind;

        w=node1;
        ind = ithNodeCode[w].count;
        for (int b =0 ;b<ind ;b++)
        {
            if (ithNodeCode[w].triangles[b].colorcode==i)
            {
              //  printf("%d true \n");
                ithNodeCode[w].triangles[b].affine[0]  =     a1.val[0];
                ithNodeCode[w].triangles[b].affine[1]  =     a2.val[0];
                ithNodeCode[w].triangles[b].affine[2]  =     a3.val[0];
                ithNodeCode[w].triangles[b].affine[3]  =     a4.val[0];
                ithNodeCode[w].triangles[b].affine[4]  =     a5.val[0];
                ithNodeCode[w].triangles[b].affine[5]  =     a6.val[0];
            }


        }

        w=node2;
        ind = ithNodeCode[w].count;
        for (int b =0 ;b<ind ;b++)
        {
            if (ithNodeCode[w].triangles[b].colorcode==i)
            {
                ithNodeCode[w].triangles[b].affine[0]  =     a1.val[0];
                ithNodeCode[w].triangles[b].affine[1]  =     a2.val[0];
                ithNodeCode[w].triangles[b].affine[2]  =     a3.val[0];
                ithNodeCode[w].triangles[b].affine[3]  =     a4.val[0];
                ithNodeCode[w].triangles[b].affine[4]  =     a5.val[0];
                ithNodeCode[w].triangles[b].affine[5]  =     a6.val[0];
            }


        }

        w=node3;
        ind = ithNodeCode[w].count;
        for (int b =0 ;b<ind ;b++)
        {
            if (ithNodeCode[w].triangles[b].colorcode==i)
            {
                ithNodeCode[w].triangles[b].affine[0]  =     a1.val[0];
                ithNodeCode[w].triangles[b].affine[1]  =     a2.val[0];
                ithNodeCode[w].triangles[b].affine[2]  =     a3.val[0];
                ithNodeCode[w].triangles[b].affine[3]  =     a4.val[0];
                ithNodeCode[w].triangles[b].affine[4]  =     a5.val[0];
                ithNodeCode[w].triangles[b].affine[5]  =     a6.val[0];
            }


        }

    }

}


void
delaunay:: calculateAffineWarpParametersComposeWithCurrent(CvMat *targetVectors)
{
    for (int i = 1; i <= totalNumberofTriangles; i++ )
    {


        int node1=colorTriangleCodeFixed[i][0];
        int node2=colorTriangleCodeFixed[i][1];
        int node3=colorTriangleCodeFixed[i][2];

        CvScalar mx1,my1,sx1,sy1;
        CvScalar mx2,my2,sx2,sy2;
        CvScalar mx3,my3,sx3,sy3;
        mx1=cvGet2D(meanShape,node1,0);
        my1=cvGet2D(meanShape,node1,1);
        mx2=cvGet2D(meanShape,node2,0);
        my2=cvGet2D(meanShape,node2,1);
        mx3=cvGet2D(meanShape,node3,0);
        my3=cvGet2D(meanShape,node3,1);

        sx1=cvGet2D(targetVectors,node1,0);
        sy1=cvGet2D(targetVectors,node1,1);
        sx2=cvGet2D(targetVectors,node2,0);
        sy2=cvGet2D(targetVectors,node2,1);
        sx3=cvGet2D(targetVectors,node3,0);
        sy3=cvGet2D(targetVectors,node3,1);


        CvMat * A = cvCreateMat(3,3,CV_64FC1 );
        CvMat * AINV = cvCreateMat(3,3,CV_64FC1 );

        cvSet2D(A,0,0,mx1);
        cvSet2D(A,1,0,mx2);
        cvSet2D(A,2,0,mx3);

        cvSet2D(A,0,1,my1);
        cvSet2D(A,1,1,my2);
        cvSet2D(A,2,1,my3);
        CvScalar one,zero;
        one.val[0]=1;
        zero.val[0]=0;
        cvSet2D(A,0,2,one);
        cvSet2D(A,1,2,one);
        cvSet2D(A,2,2,one);

        cvInvert(A,AINV,CV_LU);

        CvMat * B1 = cvCreateMat(3,1,CV_64FC1 );
        CvMat * B2 = cvCreateMat(3,1,CV_64FC1 );

        CvMat * affine1 = cvCreateMat(3,1,CV_64FC1 );
        CvMat * affine2 = cvCreateMat(3,1,CV_64FC1 );

        cvSet2D(B1,0,0,sx1);
        cvSet2D(B1,1,0,sx2);
        cvSet2D(B1,2,0,sx3);

        cvSet2D(B2,0,0,sy1);
        cvSet2D(B2,1,0,sy2);
        cvSet2D(B2,2,0,sy3);

        cvMatMul(AINV,B1,affine1);
        cvMatMul(AINV,B2,affine2);


        CvScalar a1,a2,a3,a4,a5,a6;

        a1=cvGet2D(affine1,0,0);
        a2=cvGet2D(affine1,1,0);
        a3=cvGet2D(affine1,2,0);
        a4=cvGet2D(affine2,0,0);
        a5=cvGet2D(affine2,1,0);
        a6=cvGet2D(affine2,2,0);

        CvMat * AFF1 = cvCreateMat(3,3,CV_64FC1 );
        CvMat * AFF2 = cvCreateMat(3,3,CV_64FC1 );
        cvSet2D(AFF2,0,0,a1);
        cvSet2D(AFF2,0,1,a2);
        cvSet2D(AFF2,0,2,a3);
        cvSet2D(AFF2,1,0,a4);
        cvSet2D(AFF2,1,1,a5);
        cvSet2D(AFF2,1,2,a6);

        cvSet2D(AFF2,2,0,zero);
        cvSet2D(AFF2,2,1,zero);
        cvSet2D(AFF2,2,2,one);


        a1.val[0]=  warpTriangleCode[i][0];
        a2.val[0]=warpTriangleCode[i][1];
        a3.val[0]=warpTriangleCode[i][2];
        a4.val[0]=warpTriangleCode[i][3];
        a5.val[0]= warpTriangleCode[i][4];
        a6.val[0]=warpTriangleCode[i][5];
        cvSet2D(AFF1,0,0,a1);
        cvSet2D(AFF1,0,1,a2);
        cvSet2D(AFF1,0,2,a3);
        cvSet2D(AFF1,1,0,a4);
        cvSet2D(AFF1,1,1,a5);
        cvSet2D(AFF1,1,2,a6);

        cvSet2D(AFF1,2,0,zero);
        cvSet2D(AFF1,2,1,zero);
        cvSet2D(AFF1,2,2,one);

        cvMatMul(AFF2,AFF1,AFF1);

        a1=cvGet2D(AFF1,0,0);
        a2=cvGet2D(AFF1,0,1);
        a3=cvGet2D(AFF1,0,2);
        a4=cvGet2D(AFF1,1,0);
        a5=cvGet2D(AFF1,1,1);
        a6=cvGet2D(AFF1,1,2);

        warpTriangleCode[i][0]=a1.val[0];
        warpTriangleCode[i][1]=a2.val[0];
        warpTriangleCode[i][2]=a3.val[0];
        warpTriangleCode[i][3]=a4.val[0];
        warpTriangleCode[i][4]=a5.val[0];
        warpTriangleCode[i][5]=a6.val[0];



        int w,ind;

        w=node1;
        ind = ithNodeCode[w].count;
        for (int b =0 ;b<ind ;b++)
        {
            if (ithNodeCode[w].triangles[b].colorcode==i)
            {
                ithNodeCode[w].triangles[b].affine[0]  =     a1.val[0];
                ithNodeCode[w].triangles[b].affine[1]  =     a2.val[0];
                ithNodeCode[w].triangles[b].affine[2]  =     a3.val[0];
                ithNodeCode[w].triangles[b].affine[3]  =     a4.val[0];
                ithNodeCode[w].triangles[b].affine[4]  =     a5.val[0];
                ithNodeCode[w].triangles[b].affine[5]  =     a6.val[0];
            }


        }

        w=node2;
        ind = ithNodeCode[w].count;
        for (int b =0 ;b<ind ;b++)
        {
            if (ithNodeCode[w].triangles[b].colorcode==i)
            {
                ithNodeCode[w].triangles[b].affine[0]  =     a1.val[0];
                ithNodeCode[w].triangles[b].affine[1]  =     a2.val[0];
                ithNodeCode[w].triangles[b].affine[2]  =     a3.val[0];
                ithNodeCode[w].triangles[b].affine[3]  =     a4.val[0];
                ithNodeCode[w].triangles[b].affine[4]  =     a5.val[0];
                ithNodeCode[w].triangles[b].affine[5]  =     a6.val[0];
            }


        }

        w=node3;
        ind = ithNodeCode[w].count;
        for (int b =0 ;b<ind ;b++)
        {
            if (ithNodeCode[w].triangles[b].colorcode==i)
            {
                ithNodeCode[w].triangles[b].affine[0]  =     a1.val[0];
                ithNodeCode[w].triangles[b].affine[1]  =     a2.val[0];
                ithNodeCode[w].triangles[b].affine[2]  =     a3.val[0];
                ithNodeCode[w].triangles[b].affine[3]  =     a4.val[0];
                ithNodeCode[w].triangles[b].affine[4]  =     a5.val[0];
                ithNodeCode[w].triangles[b].affine[5]  =     a6.val[0];
            }


        }

    }

}


pixel*
delaunay:: getpixel(int num)
{
    if (num>=totalNumberOfPixels)
        return NULL;

    pixel* newPixel=(pixel*)malloc(sizeof(pixel));
    newPixel->x=pixelCode[num][2]-ty;
    newPixel->y=pixelCode[num][1]-tx;
    newPixel->tx=tx;
    newPixel->ty=ty;
    newPixel->a1=warpTriangleCode[pixelCode[num][0]][0];
    newPixel->a2=warpTriangleCode[pixelCode[num][0]][1];
    newPixel->a3=warpTriangleCode[pixelCode[num][0]][2];
    newPixel->a4=warpTriangleCode[pixelCode[num][0]][3];
    newPixel->a5=warpTriangleCode[pixelCode[num][0]][4];
    newPixel->a6=warpTriangleCode[pixelCode[num][0]][5];
    newPixel->type=pixelCode[num][0];
    newPixel->width=width;
    newPixel->height=height;

//CvScalar s1 = cvGet2D(meanImage,newPixel->y,newPixel->x)
//newPixel->value=s1.val[0];

    return newPixel;

}


int
delaunay::numberOfPixels()
{
    return totalNumberOfPixels;

}


pixel*
delaunay::findCorrespondingPixelInImage(pixel* pix)
{

    double dx= pix->a1*pix->x  + pix->a2*pix->y  + pix->a3;
    double dy= pix->a4*pix->x  + pix->a5*pix->y  + pix->a6;


    pixel * newPixel= (pixel * )malloc(sizeof(pixel));
    newPixel->x=dx;
    newPixel->y=dy;
    newPixel->tx=0;
    newPixel->ty=0;

    newPixel->a1=0;
    newPixel->a2=0;
    newPixel->a3=0;
    newPixel->a4=0;
    newPixel->a5=0;
    newPixel->a6=0;
    newPixel->width=0;
    newPixel->height=0;
//calculateAffineWarpParameters(input->shape)

    return newPixel;

}

/*

private:
IplImage * rasterImage;
CvMat* meanShape;
CvMat* meanImage;
pixel * eachPixel;
int totalNumberofTriangles;
int totalNumberOfPixels;

};
*/
