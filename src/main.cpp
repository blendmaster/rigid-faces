/** @file */
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
#include <dirent.h>
#include <time.h>
#include <stdio.h>

#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <QtGui>
#include <QApplication>
#include <unistd.h>

#include "cv.h"
#include "highgui.h"
#include "lkFitting.h"

#include "landmarkpoint.h"
#include "delaunay.h"
#include "aamTrainer.h"
#include "qtUtils.h"
#include "pca.h"
#include "pca_shape.h"
#include <iostream>
#include <list>
#include <string>
#include <cctype>
#include <aam.h>
using namespace std;

void aamTrainer::removeSelected()
{

    QList<QListWidgetItem *>  list=ui.lv_thumbnails->selectedItems();
    QList<QListWidgetItem *>::iterator i;

    for (i = list.begin(); i != list.end(); ++i)
    {
        QListWidgetItem *item = *i;
        QString dat=item->data(Qt::UserRole).toString();
        char *ptr =  dat.toAscii().data();
        //  printf("%s \n",ptr);

        char path[200];

        sprintf(path,"./images/%s.jpg",ptr);
        remove(path);
        sprintf(path,"./images/%s.dat",ptr);
        remove(path);

    }
    ui.lv_thumbnails->clear();

    populateQList();
//verify();


}

void aamTrainer::loadSelected()
{

    QList<QListWidgetItem *>  list=ui.lv_thumbnails->selectedItems();
    QList<QListWidgetItem *>::iterator i;
    char datpath[200];
    for (i = list.begin(); i != list.end(); ++i)
    {
        QListWidgetItem *item = *i;
        QString dat=item->data(Qt::UserRole).toString();
        char *ptr =  dat.toAscii().data();
        char path[200];
        sprintf(path,"./images/%s.jpg",ptr);
        printf("%s",path);
        sprintf(datpath,"./images/%s.dat",ptr);
        printf("%s",datpath);
        static QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
        ui.landmarkImage->setScene(scene);
        ui.landmarkImage->setBackgroundBrush(QImage(path));
        ui.landmarkImage->show();
    }
    loadModel(datpath);



}

void aamTrainer::loadModel(char *path)
{

    while (!ui.landmarkImage->mesh->empty())
    {
        delete ui.landmarkImage->mesh->front();
        ui.landmarkImage->mesh->pop_front();
    }

    ui.landmarkImage->mesh->clear();

    landmarkPoint::count=0;

    FILE *fp;
    float n,x,y;
    if (fp=fopen(path,"r"))
    {
        while ( ! feof( fp ) )
        {
            fscanf(fp,"%f %f %f\n",&n,&x,&y);
            landmarkPoint* label = new landmarkPoint(ui.centralwidget,0);
            label->setGeometry((int)x+ ui.landmarkImage->x(),(int)y+ ui.landmarkImage->y(),label->width(),label->height());
            ui.landmarkImage->mesh->push_front(label);
        }
        fclose(fp);

    }

}

void aamTrainer::populateQList()
{

    ui.lv_thumbnails->clear();
    struct dirent *de=NULL;
    DIR *d=NULL;
    d=opendir("./images/");
    int k=0;

    while (de = readdir(d) )
    {
        if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0) || strcmp(de->d_name+strlen(de->d_name)-3, "dat")==0 ))
        {
            QListWidgetItem *item = new QListWidgetItem(de->d_name,ui.lv_thumbnails);
            char thumbpath[200];
            sprintf(thumbpath,"./images/%s",de->d_name);
            item->setIcon(QIcon(thumbpath));
            char editname[200];
            sprintf(editname,"%s",de->d_name);
            editname[strlen(de->d_name)-4]='\0';
            QString qstring(editname);
            item->setData(Qt::UserRole,qstring);
        }
    }



}
void aamTrainer::setQImageWebcam(QImage *input)
{
    if (!input)
        return ;

    static QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview->setScene(scene);
    ui.webcamPreview->setBackgroundBrush(*input);

    ui.webcamPreview->show();

}
void aamTrainer::saveModel()
{
    list< landmarkPoint*>::iterator it;
    int count=-1;
    QList<QListWidgetItem *>  list=ui.lv_thumbnails->selectedItems();
    QList<QListWidgetItem *>::iterator i;
    char path[200];

    for (i = list.begin(); i != list.end(); ++i)
    {
        QListWidgetItem *item = *i;
        QString dat=item->data(Qt::UserRole).toString();
        char *ptr =  dat.toAscii().data();
        //  printf("%s \n",ptr);

        sprintf(path,"./images/%s.dat",ptr);
    }


    FILE *fp;
    fp = fopen(path,"w");
    ui.landmarkImage->mesh->reverse();

    for (it=ui.landmarkImage->mesh->begin(); it!=ui.landmarkImage->mesh->end(); ++it)
    {
        count++;


        fprintf(fp, "%d %d %d\n",count,(*it)->x()-ui.landmarkImage->x(),(*it)->y()-ui.landmarkImage->y());
    }
    fclose(fp);
    ui.landmarkImage->mesh->reverse();
}
void aamTrainer::loadStaticModel()
{
    loadModel("./model.shape");
}

aamTrainer::aamTrainer(QWidget *parent)
        : QMainWindow(parent)
{


    ui.setupUi(this);


    cap=false;
    QDesktopWidget *desktop = QApplication::desktop();



    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = desktop->width(); // get width of screen
    screenHeight = desktop->height(); // get height of screen

    windowSize = size(); // size of our application window
    width = windowSize.width();
    height = windowSize.height();

    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 50;

    move ( x, y );

    connect(ui.pb_capture,SIGNAL(clicked()), this, SLOT(captureClick()));
    connect(ui.pb_delete,SIGNAL(clicked()), this, SLOT(removeSelected()));

    connect(ui.lv_thumbnails,SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(loadSelected()));

    connect(ui.pb_savemodel,SIGNAL(clicked()), this, SLOT(saveModel()));

    connect(ui.pb_model,SIGNAL(clicked()), this, SLOT(loadStaticModel()));

    connect(ui.pb_gendata,SIGNAL(clicked()), this, SLOT(gendata()));

    webcam.startCamera();
    timerId= startTimer( 20 );
    populateQList();

}

void aamTrainer::gendata()
{

    struct dirent *de=NULL;
    DIR *d=NULL;
    d=opendir("./images/");
    int k=0;

    while (de = readdir(d) )
    {
        if (strcmp(de->d_name+strlen(de->d_name)-3, "dat")==0 )
        {
            k++;
        }
    }
    CvMat * meanXY = cvCreateMat(k,2, CV_64FC1 );
    CvMat * dataset = cvCreateMat(k,numberofpoints, CV_64FC2 );
    CvMat ** testWarpSet = (CvMat **)cvAlloc(k*sizeof(CvMat *));

    for (int i=0;i<k;i++)
        testWarpSet[i]=cvCreateMat(numberofpoints,2, CV_64FC1 );

    IplImage ** images = (IplImage **)cvAlloc(k*sizeof(IplImage *));

    d=opendir("./images/");
    int index=0;
    while (de = readdir(d) )
    {

        if (strcmp(de->d_name+strlen(de->d_name)-3, "dat")==0 )
        {
            int j=0;
            int num=0;

            double meanx=0,meany=0;
            char path[200];
            sprintf(path,"./images/%s",de->d_name);
            char imagepath[200];
            sprintf(imagepath,"%s",path);
            imagepath[strlen(imagepath)-4]='\0';
            sprintf(imagepath,"%s.jpg",imagepath);
            FILE *fp,*wfp;
            float n,shape[totalnumberofpoints];
            double xavg=0,yavg=0;
            if (fp=fopen(path,"r"))
            {
                images[index]=cvLoadImage(imagepath,0);
                while ( ! feof( fp ) )
                {
                    CvScalar s;
                    s.val[0]=0;
                    s.val[1]=0;
                    s.val[2]=0;
                    fscanf(fp,"%f %f %f\n",&n,&shape[j],&shape[j+1]);
                    s.val[0]=shape[j];
                    s.val[1]=shape[j+1];
                    meanx+=s.val[0];
                    meany+=s.val[1];
                    cvSet2D(  dataset, index,num, s);



                    CvScalar t;
                    t.val[0]=s.val[0];
                    cvSet2D(testWarpSet[index],num,0,t);
                    t.val[0]=s.val[1];
                    cvSet2D(testWarpSet[index],num,1,t);

                    num++;
                    j+=2;



                }
                fclose(fp);

            }


            CvScalar s;
            s.val[0]=meanx/numberofpoints;
            cvSet2D(  meanXY, index,0, s);
            s.val[0]=meany/numberofpoints;
            cvSet2D(  meanXY, index,1, s);

            index++;

        }
    }
    double avNorm=0;
    for (index=0;index<k;index++)
    {
        for (int i=0;i<numberofpoints;i++)
        {
            CvScalar s,m1,m2;
            s=    cvGet2D(dataset, index,i);
            m1=    cvGet2D(meanXY, index,0);
            m2=    cvGet2D(meanXY, index,1);
            s.val[0]-=m1.val[0];
            s.val[1]-=m2.val[0];
            cvSet2D(  dataset, index,i, s);
        }

    }



    CvMat * x1Tx2 = cvCreateMat(2,2, CV_64FC1 );
    CvMat * x1T = cvCreateMat(2,numberofpoints, CV_64FC1 );
    CvMat * x2 = cvCreateMat(numberofpoints,2, CV_64FC1 );
    CvMat * UT= cvCreateMat(2,2, CV_64FC1 );
    CvMat * V = cvCreateMat(2,2, CV_64FC1 );
    CvMat * tempW = cvCreateMat(2,2, CV_64FC1 );
    CvMat * VUT = cvCreateMat(2,2, CV_64FC1 );
    CvMat * average = cvCreateMat(numberofpoints,2, CV_64FC1 );


    for (int i=0;i<numberofpoints;i++)
    {
        CvScalar s;
        s= cvGet2D(dataset,0,i);
        CvScalar m;
        m.val[0]=s.val[0];
        cvSet2D(  x2, i,0, m);
        m.val[0]=s.val[1];
        cvSet2D(  x2, i,1, m);
    }
    for (int iteration=0;iteration<5;iteration++)
    {
        for (index=0;index<k;index++)
        {

            for (int i=0;i<numberofpoints;i++)
            {
                CvScalar s;
                s= cvGet2D(dataset,index,i);
                CvScalar m;
                m.val[0]=s.val[0];
                cvSet2D(  x1T,0, i, m);
                m.val[0]=s.val[1];
                cvSet2D(  x1T,1, i, m);
            }

            cvMatMul(x1T,x2,x1Tx2);
            cvSVD( x1Tx2, tempW, UT, V,CV_SVD_U_T);
            cvMatMul(V,UT,VUT);

            for (int i=0;i<numberofpoints;i++)
            {
                CvScalar s;
                s= cvGet2D(dataset,index,i);
                CvScalar v1,v2,v3,v4;
                v1= cvGet2D(VUT,0,0);
                v2= cvGet2D(VUT,0,1);
                v3= cvGet2D(VUT,1,0);
                v4= cvGet2D(VUT,1,1);
                double k1 = v1.val[0]*s.val[0] + v2.val[0]*s.val[1];
                double k2 = v3.val[0]*s.val[0] + v4.val[0]*s.val[1];
                s.val[0]=k1;
                s.val[1]=k2;
                cvSet2D(dataset,index,i,s);

            }

        }
        cvZero(average);

        for (int i=0;i<numberofpoints;i++)
        {
            CvScalar s,avx,avy;
            s= cvGet2D(dataset,0,i);
            avx= cvGet2D(average,i,0);
            avy= cvGet2D(average,i,1);
            avx.val[0]+=s.val[0];
            avy.val[0]+=s.val[1];
            cvSet2D(average,i,0,avx);
            cvSet2D(average,i,1,avy);
        }
        for (index=1;index<k;index++)
        {
            for (int i=0;i<numberofpoints;i++)
            {
                CvScalar s,avx,avy;
                s= cvGet2D(dataset,index,i);
                avx= cvGet2D(average,i,0);
                avy= cvGet2D(average,i,1);
                avx.val[0]= (avx.val[0]*index + s.val[0])/(index+1);
                avy.val[0]= (avy.val[0]*index + s.val[1])/(index+1);
                cvSet2D(average,i,0,avx);
                cvSet2D(average,i,1,avy);
            }
        }

    }

    CvFileStorage *fs;
    fs = cvOpenFileStorage( "./alignedset.aam", 0, CV_STORAGE_WRITE );
    cvWrite( fs, "shape",dataset, cvAttrList(0,0) );
    cvReleaseFileStorage( &fs );

    CvMat **shapeAlignedSet =(CvMat**)cvAlloc(sizeof(CvMat*) * k);

    for (int i=0;i<k;i++)
    {
        shapeAlignedSet[i]=cvCreateMat(1,totalnumberofpoints,CV_64FC1);

        for (int m=0;m<numberofpoints;m++)
        {
            CvScalar v;
            v= cvGet2D(dataset,i,m);
            CvScalar s1,s2;
            s1.val[0]=v.val[0];
            s2.val[0]=v.val[1];
            cvSet2D(shapeAlignedSet[i],0,2*m,s1);
            cvSet2D(shapeAlignedSet[i],0,(2*m)+1,s2);
        }
    }

    double norm=0;
    for (int i=0;i<numberofpoints;i++)
    {
        CvScalar avx,avy;
        avx= cvGet2D(average,i,0);
        avy= cvGet2D(average,i,1);

        norm+=pow(avx.val[0],2)+pow(avy.val[0],2);

    }


    norm=sqrt(norm);

    for (int i=0;i<k;i++)
    {
        double newNorm=0;

        for (int m=0;m<numberofpoints;m++)
        {
            CvScalar avx,avy;

            avx=cvGet2D(shapeAlignedSet[i],0,2*m);
            avy=cvGet2D(shapeAlignedSet[i],0,(2*m)+1);
            newNorm+=pow(avx.val[0],2)+pow(avy.val[0],2);
        }
        newNorm=sqrt(newNorm);
        for (int m=0;m<numberofpoints;m++)
        {
            CvScalar avx,avy;

            avx= cvGet2D(shapeAlignedSet[i],0,2*m);
            avy=cvGet2D(shapeAlignedSet[i],0,(2*m)+1);
//            avx.val[0]*=norm/newNorm;
//            avy.val[0]*=norm/newNorm;
            cvSet2D(shapeAlignedSet[i],0,2*m,avx);
            cvSet2D(shapeAlignedSet[i],0,(2*m)+1,avy);
        }

    }
    IplImage *imgPoints =cvCreateImage(cvSize(400,400),IPL_DEPTH_8U,3);
    cvZero(imgPoints);

    cvZero(imgPoints);

    for (int m=0;m<numberofpoints;m++)
    {
        CvScalar avx,avy;

        avx= cvGet2D(average,m,0);
        avy= cvGet2D(average,m,1);
        cvCircle( imgPoints, cvPoint(avx.val[0]+100,avy.val[0]+100),1, CV_RGB(255,255,255),2 );

    }
#ifdef DEBUG
    cvNamedWindow("warped1",1);
    cvShowImage("warped1",imgPoints);
    cvWaitKey(-1);
    for (int i=0;i<k;i++)
    {
        double newNorm=0;
        cvZero(imgPoints);

        for (int m=0;m<numberofpoints;m++)
        {
            CvScalar avx,avy;

            avx=cvGet2D(shapeAlignedSet[i],0,2*m);
            avy=cvGet2D(shapeAlignedSet[i],0,(2*m)+1);
            cvCircle( imgPoints, cvPoint(avx.val[0]+100,avy.val[0]+100),1, CV_RGB(255,255,i*20),2 );

        }


        cvNamedWindow("warped1",1);
        cvShowImage("warped1",imgPoints);
        cvWaitKey(-1);

    }
#endif

    PCA_AAM  newPCAShape;
    PCA_AAM newPCAAppearance;
    newPCAShape.runPCA(shapeAlignedSet,k);



    CvMat *shape ;
    CvMat ** warpedImage = (CvMat **)cvAlloc(k*sizeof(CvMat *));
    shape= cvCreateMat(average->height,average->width, CV_64FC1);
    shape = cvCloneMat(average);
    if (shape!=NULL)
    {
        delaunay newDelaunay(shape);
        IplImage *test =cvCreateImage(cvSize(newDelaunay.width,(newDelaunay.height)),IPL_DEPTH_8U,1);
        cvZero(test);

        for (int m=0;m<k;m++)
            warpedImage[m] =  cvCreateMat(newDelaunay.height, newDelaunay.width,CV_64FC1);

        for (int m=0;m<k;m++)
        {
            cvZero(test);


            cvZero(warpedImage[m]);
            int t=newDelaunay.numberOfPixels();
            newDelaunay.calculateAffineWarpParameters(testWarpSet[m]);

            for (int i=0;i<t;i++)
            {
                pixel * pix = newDelaunay.getpixel(i);

                pixel *pix2 =newDelaunay.findCorrespondingPixelInImage(pix);

                float imageValue =interpolate<uchar>(images[m],float(pix2->x),float(pix2->y));

                CvScalar value;
                value.val[0]=imageValue;
                if (floor(pix->y+pix->ty)>=0 && floor(pix->x+pix->tx)>=0)
                {
                    cvSet2D(test,(int)floor(pix->y+pix->ty),(int)floor(pix->x+pix->tx),value);
                    cvSet2D(warpedImage[m],(int)floor(pix->y+pix->ty),(int)floor(pix->x+pix->tx),value);
                }


            }

#ifdef DEBUG

            cvNamedWindow("Warped Image",1);
            cvShowImage("Warped Image",test);
            cvWaitKey(-1);

#endif

        }


    }
    cvReleaseMat(&dataset);
    for (int i=0;i<k;i++)
    {
        cvReleaseMat(&testWarpSet[i]);
        cvReleaseMat(&shapeAlignedSet[i]);
    }
    newPCAAppearance.runPCA(warpedImage,k);
    eigenVectors_AAM *app= newPCAAppearance.returnEigens();
    eigenVectors_AAM *shapes= newPCAShape.returnEigens();
   fs = cvOpenFileStorage( "./aam.template", 0, CV_STORAGE_WRITE );
    for (int m=0;m<app->count;m++)
    {
        char temp[200];
        if(app->eigens[m]!=NULL)
        {
        sprintf(temp,"appEigenVectors%d",m);
        cvWrite( fs,temp ,app->eigens[m], cvAttrList(0,0) );
    }}
    for (int m=0;m<shapes->count;m++)
    {
        char temp[200];
        if(shapes->eigens[m]!=NULL)
        {
        sprintf(temp,"shapeEigenVectors%d",m);
        cvWrite( fs,temp ,shapes->eigens[m], cvAttrList(0,0) );
    }}
    IplImage *avgShape=newPCAShape.returnAverage();
    IplImage *avgApp=newPCAAppearance.returnAverage();
    for (int i=0;i<avgShape->width;i++)
    {
        for (int j=0;j<avgShape->height;j++)
        {

            CvScalar s1,s2;
            s1=cvGet2D(average,(i/2),i%2);
            cvSet2D(avgShape,j,i,s1);

        }
    }
    cvWrite( fs,"avgApp" ,avgApp, cvAttrList(0,0) );
    cvWrite( fs,"avgShape" ,avgShape, cvAttrList(0,0) );
    cvWriteReal( fs, "numberOfAppearanceEigenVectors",app->count);
    cvWriteReal( fs, "numberOfShapeEigenVectors",shapes->count);
    CvMat *eigenVal=newPCAShape.returnEigenVals();
    cvWrite( fs,"eigenVal" ,eigenVal, cvAttrList(0,0) );
    cvReleaseFileStorage( &fs );

printf("Training Completed \n");


}
void aamTrainer::captureClick()
{
    cap=true;


}
void aamTrainer::timerEvent( QTimerEvent * )
{
    IplImage * queryImage = webcam.queryFrame();

    QImage * qm=QImageIplImageCvt(queryImage);
    if (cap==true)
    {
        time_t ltime;
        struct tm *Tm;
        struct timeval detail_time;

        char* setDir  = new char[200];
        char* uniqueName  = new char[200];
        ltime=time(NULL);
        Tm=localtime(&ltime);
        gettimeofday(&detail_time,NULL);
        sprintf(uniqueName,"%d%d%d%d%d%d%d.jpg",Tm->tm_year,Tm->tm_mon,Tm->tm_mday,Tm->tm_hour,Tm->tm_min,Tm->tm_sec,(detail_time.tv_usec/1000));
        sprintf(setDir,"./images/%s",uniqueName);
        cvSaveImage(setDir,queryImage);
        cap=false;
        populateQList();
    }

    setQImageWebcam(qm);
    cvWaitKey(1);
    delete qm;
    cvReleaseImage(&queryImage);

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    aamTrainer tab1;
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("qt_aamTrainer_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    QRect r = tab1.geometry();
    r.moveCenter(QApplication::desktop()->availableGeometry().center());
    tab1.setGeometry(r);


    tab1.show();
    return app.exec();
}

