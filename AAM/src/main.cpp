/** @file */

/*
    QT Face Trainer MAIN
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://code.google.com/p/pam-face-authentication/

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
#include "cmupca.h"
#include <iostream>
#include <list>
#include <string>
#include <cctype>
using namespace std;
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240
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
        printf("%s",path);
        remove(path);
        sprintf(path,"./images/%s.aam",ptr);
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
        //  printf("%s \n",ptr);

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


//verify();


}

void aamTrainer::loadModel(char *path)
{
    /*
    while (!ui.landmarkImage->mesh->empty())
    {
    free(ui.landmarkImage->mesh->front())
    mylist.pop_front();
    }



    for (it=ui.landmarkImage->mesh->begin(); it!=ui.landmarkImage->mesh->end(); ++it)
    {

    }
    */

    while (!ui.landmarkImage->mesh->empty())
    {
        delete ui.landmarkImage->mesh->front();
        ui.landmarkImage->mesh->pop_front();
    }

    ui.landmarkImage->mesh->clear();

    landmarkPoint::count=0;

    FILE *fp;
    int n,x,y;
    if (fp=fopen(path,"r"))
    {
        while ( ! feof( fp ) )
        {
            fscanf(fp,"%d %d %d\n",&n,&x,&y);
            landmarkPoint* label = new landmarkPoint(ui.centralwidget,0);
            label->setGeometry(x+ ui.landmarkImage->x(),y+ ui.landmarkImage->y(),label->width(),label->height());
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
        printf("%s",path);
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
    // landmarkPoint* label = new landmarkPoint(ui.centralwidget,0);
    //  landmarkPoint* labe1l = new landmarkPoint(ui.centralwidget,0);

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

// little computations
    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 50;

// move window to desired coordinates
    move ( x, y );

    connect(ui.pb_capture,SIGNAL(clicked()), this, SLOT(captureClick()));
    connect(ui.pb_delete,SIGNAL(clicked()), this, SLOT(removeSelected()));

    connect(ui.lv_thumbnails,SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(loadSelected()));

    connect(ui.pb_savemodel,SIGNAL(clicked()), this, SLOT(saveModel()));

    connect(ui.pb_model,SIGNAL(clicked()), this, SLOT(loadStaticModel()));

    connect(ui.pb_gendata,SIGNAL(clicked()), this, SLOT(gendata()));

    /*   connect(ui.pb_back_t2,SIGNAL(clicked()), this, SLOT(showTab1()));
    connect(ui.pb_about,SIGNAL(clicked()), this, SLOT(about()));
    connect(ui.pb_ds,SIGNAL(clicked()), this, SLOT(removeSelected()));
    connect(ui.pb_adv,SIGNAL(clicked()), this, SLOT(showAdvDialog()));
    */

    webcam.startCamera();
    timerId= startTimer( 20 );
    populateQList();
    //connect(ui.button1,SIGNAL(clicked()), this, SLOT(verify()));
    //connect(ui.but,SIGNAL(clicked()), this, SLOT(butClick()));


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
    printf("%d \n",k);
    CvMat * meanXY = cvCreateMat(k,2, CV_64FC1 );
    CvMat * dataset = cvCreateMat(k,40, CV_64FC2 );
    CvMat ** testWarpSet = (CvMat **)cvAlloc(k*sizeof(CvMat *));

    for (int i=0;i<k;i++)
        testWarpSet[i]=cvCreateMat(40,2, CV_64FC1 );

    IplImage ** images = (IplImage **)cvAlloc(k*sizeof(IplImage *));

    d=opendir("./images/");
    int index=0;
    while (de = readdir(d) )
    {

        if (strcmp(de->d_name+strlen(de->d_name)-3, "dat")==0 )
        {
            int j=0;
            int num=0;

            printf("%d \n",index);
            double meanx=0,meany=0;
            char path[200];
            sprintf(path,"./images/%s",de->d_name);
            char imagepath[200];
            sprintf(imagepath,"%s",path);
            imagepath[strlen(imagepath)-4]='\0';
            sprintf(imagepath,"%s.jpg",imagepath);
            printf("%s \n",imagepath);
            FILE *fp,*wfp;
//dangerous 40 , shouldnt hardcode, oh well ....
            int n,shape[80];
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
                    fscanf(fp,"%d %d %d\n",&n,&shape[j],&shape[j+1]);
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
            s.val[0]=meanx/40;
            cvSet2D(  meanXY, index,0, s);
            s.val[0]=meany/40;
            cvSet2D(  meanXY, index,1, s);

            index++;

        }
    }
// Remove Translation


    double avNorm=0;
    for (index=0;index<k;index++)
    {
        for (int i=0;i<40;i++)
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
    /*
    avNorm/=k;

        for (index=0;index<k;index++)
        {
         for (int i=0;i<80;i++)
            {

                CvScalar s;
                s=    cvGet2D(dataset, index,i);
                s.val[0]*=avNorm;
                cvSet2D(  dataset, index,i, s);

            }
        }

        */





    CvMat * x1Tx2 = cvCreateMat(2,2, CV_64FC1 );
    CvMat * x1T = cvCreateMat(2,40, CV_64FC1 );
    CvMat * x2 = cvCreateMat(40,2, CV_64FC1 );
    CvMat * UT= cvCreateMat(2,2, CV_64FC1 );
    CvMat * V = cvCreateMat(2,2, CV_64FC1 );
    CvMat * tempW = cvCreateMat(2,2, CV_64FC1 );
    CvMat * VUT = cvCreateMat(2,2, CV_64FC1 );
    CvMat * average = cvCreateMat(40,2, CV_64FC1 );


    for (int i=0;i<40;i++)
    {
        CvScalar s;
        s= cvGet2D(dataset,0,i);
        CvScalar m;
        m.val[0]=s.val[0];
        cvSet2D(  x2, i,0, m);
        m.val[0]=s.val[1];
        cvSet2D(  x2, i,1, m);
    }


    for (int iteration=0;iteration<30;iteration++)
    {
        for (index=0;index<k;index++)
        {

            for (int i=0;i<40;i++)
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

            for (int i=0;i<40;i++)
            {
                CvScalar s;
                s= cvGet2D(dataset,index,i);
                CvScalar v1,v2,v3,v4;
                v1= cvGet2D(VUT,0,0);
                v2= cvGet2D(VUT,0,1);
                v3= cvGet2D(VUT,1,0);
                v4= cvGet2D(VUT,1,1);
                //   printf("%e %e %e %e \n",v1.val[0],v2.val[0],v3.val[0],v4.val[0]);
                double k1 = v1.val[0]*s.val[0] + v2.val[0]*s.val[1];
                double k2 = v3.val[0]*s.val[0] + v4.val[0]*s.val[1];
                // printf("%e %e %e %e \n",k1,k2,s.val[0],s.val[1]);
                s.val[0]=k1;
                s.val[1]=k2;
                cvSet2D(dataset,index,i,s);

            }


        }
        cvZero(average);
        for (index=0;index<k;index++)
        {
            for (int i=0;i<40;i++)
            {
                CvScalar s,avx,avy;
                s= cvGet2D(dataset,index,i);
                avx= cvGet2D(average,i,0);
                avy= cvGet2D(average,i,1);
                avx.val[0]+=s.val[0];
                avy.val[0]+=s.val[1];
                cvSet2D(average,i,0,avx);
                cvSet2D(average,i,1,avy);
            }
        }
        for (int i=0;i<40;i++)
        {
            CvScalar avx,avy;
            avx= cvGet2D(average,i,0);
            avy= cvGet2D(average,i,1);
            avx.val[0]= avx.val[0]/k;
            avy.val[0]= avy.val[0]/k;
            cvSet2D(  x2, i,0, avx);
            cvSet2D(  x2, i,1,avy);

            cvSet2D(  average, i,0, avx);
            cvSet2D(  average, i,1,avy);

        }
    }

    CvFileStorage *fs;
    fs = cvOpenFileStorage( "./alignedset.aam", 0, CV_STORAGE_WRITE );
    cvWrite( fs, "shape",dataset, cvAttrList(0,0) );
    cvReleaseFileStorage( &fs );

    CvMat **shapeAlignedSet =(CvMat**)cvAlloc(sizeof(CvMat*) * k);
    CvMat **appearanceWarpedSet =(CvMat**)cvAlloc(sizeof(CvMat*) * k);

    for (int i=0;i<k;i++)
    {
        shapeAlignedSet[i]=cvCreateMat(1,80,CV_64FC1);

        for (int m=0;m<40;m++)
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



   CMUPCA newCMU;


    PCA newPCAShape,newPCAAppearance;

   newCMU.runPCA(shapeAlignedSet,k);
    newPCAShape.runPCA(shapeAlignedSet,k);




    CvMat *shape ;
    CvMat ** warpedImage = (CvMat **)cvAlloc(k*sizeof(CvMat *));

    shape= cvCreateMat(x2->height,x2->width, CV_64FC1);
    shape = cvCloneMat(average);
    if (shape!=NULL)
    {


        delaunay newDelaunay(shape);
        for (int m=0;m<k;m++)
            warpedImage[m] =  cvCreateMat(newDelaunay.height+10, newDelaunay.width+10,CV_64FC1);
        char windowtitle[200];
        sprintf(windowtitle,"hello");
        for (int m=0;m<k;m++)
        {

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
                //printf(" %d %d  %e %e \n",i,t,pix->y+pix->ty,pix->x+pix->tx);
                if ((pix->y+pix->ty)>=0 && (pix->x+pix->tx)>=0)
                    cvSet2D(warpedImage[m],int(pix->y+pix->ty),int(pix->x+pix->tx),value);

//                    printf(" %d %d %e   %e %e \n",i,t,pix->type,pix->y+pix->ty,pix->x+pix->tx);

            }

               cvNamedWindow(windowtitle,1);
            cvShowImage(windowtitle,warpedImage[m]);
            sprintf(windowtitle,"%shello",windowtitle);



        }


    }

    newPCAAppearance.runPCA(warpedImage,k);
    eigenVectors *app= newPCAAppearance.returnEigens();
    eigenVectorsCMUPCA *shapes= newCMU.returnEigens();

    printf("LK started \n");

    fs = cvOpenFileStorage( "./aam.template", 0, CV_STORAGE_WRITE );

    for (int m=0;m<app->count;m++)
    {
        char temp[200];
        sprintf(temp,"appEigenVectors%d",m);
        cvWrite( fs,temp ,app->eigens[m], cvAttrList(0,0) );
    }
    for (int m=0;m<shapes->count;m++)
    {
        char temp[200];
        sprintf(temp,"shapeEigenVectors%d",m);
        cvWrite( fs,temp ,shapes->eigens[m], cvAttrList(0,0) );
    }
    IplImage *avgShape=newCMU.returnAverage();
    IplImage *avgApp=newPCAAppearance.returnAverage();



    cvWrite( fs,"avgApp" ,avgApp, cvAttrList(0,0) );
    cvWrite( fs,"avgShape" ,avgShape, cvAttrList(0,0) );
    cvWriteReal( fs, "numberOfAppearanceEigenVectors",app->count);
    cvWriteReal( fs, "numberOfShapeEigenVectors",shapes->count);
    cvReleaseFileStorage( &fs );
    cvWaitKey(-1);

printf("LK started \n");


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
    // sleep(1);

    delete qm;
    cvReleaseImage(&queryImage);

}






int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    aamTrainer tab1;
    //tab1.setWindowTitle("Face Trainer");
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

