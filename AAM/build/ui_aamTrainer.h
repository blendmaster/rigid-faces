/********************************************************************************
** Form generated from reading ui file 'aamTrainer.ui'
**
** Created: Sat Dec 5 01:47:57 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_AAMTRAINER_H
#define UI_AAMTRAINER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include "canvas.h"

QT_BEGIN_NAMESPACE

class Ui_aamTrainer
{
public:
    QWidget *centralwidget;
    QGraphicsView *webcamPreview;
    QPushButton *pb_capture;
    QListWidget *lv_thumbnails;
    canvas *landmarkImage;
    QPushButton *pb_model;
    QPushButton *pb_savemodel;
    QPushButton *pb_delete;
    QPushButton *pb_gendata;

    void setupUi(QMainWindow *aamTrainer)
    {
        if (aamTrainer->objectName().isEmpty())
            aamTrainer->setObjectName(QString::fromUtf8("aamTrainer"));
        aamTrainer->resize(701, 476);
        centralwidget = new QWidget(aamTrainer);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        webcamPreview = new QGraphicsView(centralwidget);
        webcamPreview->setObjectName(QString::fromUtf8("webcamPreview"));
        webcamPreview->setGeometry(QRect(15, 18, 320, 240));
        webcamPreview->setMaximumSize(QSize(320, 240));
        webcamPreview->setFrameShape(QFrame::NoFrame);
        webcamPreview->setFrameShadow(QFrame::Plain);
        webcamPreview->setLineWidth(0);
        webcamPreview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        webcamPreview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        webcamPreview->setSceneRect(QRectF(0, 0, 320, 240));
        pb_capture = new QPushButton(centralwidget);
        pb_capture->setObjectName(QString::fromUtf8("pb_capture"));
        pb_capture->setGeometry(QRect(224, 258, 111, 20));
        pb_capture->setIconSize(QSize(32, 32));
        lv_thumbnails = new QListWidget(centralwidget);
        lv_thumbnails->setObjectName(QString::fromUtf8("lv_thumbnails"));
        lv_thumbnails->setGeometry(QRect(6, 283, 689, 187));
        lv_thumbnails->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lv_thumbnails->setProperty("showDropIndicator", QVariant(false));
        lv_thumbnails->setDragEnabled(false);
        lv_thumbnails->setDragDropMode(QAbstractItemView::NoDragDrop);
        lv_thumbnails->setIconSize(QSize(90, 110));
        lv_thumbnails->setMovement(QListView::Static);
        lv_thumbnails->setFlow(QListView::LeftToRight);
        lv_thumbnails->setSpacing(6);
        lv_thumbnails->setViewMode(QListView::IconMode);
        landmarkImage = new canvas(centralwidget);
        landmarkImage->setObjectName(QString::fromUtf8("landmarkImage"));
        landmarkImage->setGeometry(QRect(360, 19, 320, 240));
        landmarkImage->setMaximumSize(QSize(320, 240));
        landmarkImage->setMouseTracking(true);
        landmarkImage->setFrameShape(QFrame::NoFrame);
        landmarkImage->setFrameShadow(QFrame::Plain);
        landmarkImage->setLineWidth(0);
        landmarkImage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        landmarkImage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        landmarkImage->setSceneRect(QRectF(0, 0, 320, 240));
        pb_model = new QPushButton(centralwidget);
        pb_model->setObjectName(QString::fromUtf8("pb_model"));
        pb_model->setGeometry(QRect(361, 258, 111, 20));
        pb_model->setIconSize(QSize(32, 32));
        pb_savemodel = new QPushButton(centralwidget);
        pb_savemodel->setObjectName(QString::fromUtf8("pb_savemodel"));
        pb_savemodel->setGeometry(QRect(474, 258, 90, 20));
        pb_savemodel->setIconSize(QSize(32, 32));
        pb_delete = new QPushButton(centralwidget);
        pb_delete->setObjectName(QString::fromUtf8("pb_delete"));
        pb_delete->setGeometry(QRect(582, 449, 111, 20));
        pb_delete->setIconSize(QSize(32, 32));
        pb_gendata = new QPushButton(centralwidget);
        pb_gendata->setObjectName(QString::fromUtf8("pb_gendata"));
        pb_gendata->setGeometry(QRect(566, 258, 117, 20));
        pb_gendata->setIconSize(QSize(32, 32));
        aamTrainer->setCentralWidget(centralwidget);

        retranslateUi(aamTrainer);

        QMetaObject::connectSlotsByName(aamTrainer);
    } // setupUi

    void retranslateUi(QMainWindow *aamTrainer)
    {
        aamTrainer->setWindowTitle(QApplication::translate("aamTrainer", "MainWindow", 0, QApplication::UnicodeUTF8));
        webcamPreview->setStyleSheet(QApplication::translate("aamTrainer", "background-color: rgb(0, 0, 0);", 0, QApplication::UnicodeUTF8));
        pb_capture->setText(QApplication::translate("aamTrainer", "Capture Frame", 0, QApplication::UnicodeUTF8));
        landmarkImage->setStyleSheet(QApplication::translate("aamTrainer", "background-color: rgb(0, 0, 0);", 0, QApplication::UnicodeUTF8));
        pb_model->setText(QApplication::translate("aamTrainer", "Initialize Model", 0, QApplication::UnicodeUTF8));
        pb_savemodel->setText(QApplication::translate("aamTrainer", "Save Model", 0, QApplication::UnicodeUTF8));
        pb_delete->setText(QApplication::translate("aamTrainer", "Delete Selected", 0, QApplication::UnicodeUTF8));
        pb_gendata->setText(QApplication::translate("aamTrainer", "Generate Dataset", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(aamTrainer);
    } // retranslateUi

};

namespace Ui {
    class aamTrainer: public Ui_aamTrainer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AAMTRAINER_H
