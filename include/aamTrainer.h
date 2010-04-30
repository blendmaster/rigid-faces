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
/** @file */

#include "ui_aamTrainer.h"
#include "opencvWebcam.h"
/**
* AAM Trainer Class.
*/
class aamTrainer: public QMainWindow
{
    Q_OBJECT

public:
    /**
    *The Constructor
    *Sets up the UI for the Main Window and connect Signals to Slots
    */
    aamTrainer(QWidget *parent = 0);

    /**
    *OpenCV Webcam Object
    */
    opencvWebcam webcam;


private:
    bool cap;


    int timerId;
    /**
    *UI Object of the Face Trainer Main Window
    */
    Ui::aamTrainer ui;
    //  	opencvWebcamThread * thread;
protected:
    /**
    *timerEvent of MainWindow Overload
    */
    void populateQList();
    void loadModel(char *path);
    void timerEvent( QTimerEvent* );

public slots:
    /**
    *Sets the image of QGraphicsView which Displays the Webcam
    *@param input that needs to be set.
    */

    void setQImageWebcam(QImage *);
  void captureClick();
  void loadSelected();
    void loadStaticModel();
      void gendata();

      void removeSelected();
            void saveModel();


};
