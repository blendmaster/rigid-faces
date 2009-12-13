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
