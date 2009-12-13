/** @file */


#include <QtGui/QLabel>

#include "landmarkpoint.h"
#include <QApplication>

 #include <QMouseEvent>

 int landmarkPoint:: count=0;
landmarkPoint:: landmarkPoint(QWidget * parent = 0, Qt::WindowFlags f = 0) :QLabel(parent,f)
{

    count++;
     QFont font;
        font.setPointSize(6);
        setFont(font);
        setLayoutDirection(Qt::RightToLeft);
        setTextFormat(Qt::PlainText);
        move=false;
        char name[20];

        sprintf(name,"%d",count);
        printf("%d \n",count);
        if(strlen(name)==2)
        widthVal=12;
        else
        widthVal=6;
        setGeometry(QRect(406, 90, widthVal,12));

        setText(QApplication::translate("landmark", name, 0, QApplication::UnicodeUTF8));
        setStyleSheet(QApplication::translate("landmark", "background-color: rgb(42, 0, 255, 30%);\n"
"color: rgb(255, 255, 255);", 0, QApplication::UnicodeUTF8));
show();


}
void   landmarkPoint:: mouseMoveEvent (QMouseEvent * event)
{
if(move)
{
int x1=x() +event->x() - widthVal/2;
int y1=y() +event->y() - widthVal/2;

printf("%d %d \n",x1,y1);
        setGeometry(QRect(x1,y1, widthVal ,12 ));
               setStyleSheet(QApplication::translate("landmark", "background-color:none\n"
"color: rgb(255, 255, 255);", 0, QApplication::UnicodeUTF8));
show();

}

}
void   landmarkPoint:: mousePressEvent (QMouseEvent * event)
{
        move=true;

}
void   landmarkPoint:: mouseReleaseEvent (QMouseEvent * event)
{       setStyleSheet(QApplication::translate("landmark", "background-color: rgb(42, 0, 255, 30%);\n"
"color: rgb(255, 255, 255);", 0, QApplication::UnicodeUTF8));
show();

        move=false;
       // setGeometry(QRect(x(),y(), 4,4));

}
