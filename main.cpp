#include <QApplication>
#include <QObject>
#include <QPointer>
#include "mainwindow.hpp"

#include "camerathread.hpp"
#include "leptonthread.hpp"

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  MainWindow w;
  // create Thread
  QPointer<LeptonThread> lepton = new LeptonThread();
  QPointer<CameraThread> raspicam = new CameraThread();
  // connect signal
  QObject::connect(lepton, &LeptonThread::updateImage, &w,
                   &MainWindow::set_thermal_image);
  QObject::connect(raspicam, &CameraThread::updateImage, &w,
                   &MainWindow::set_rgb_image);
  // open window
  w.show();

  raspicam->start();
  lepton->start();

  // //create a FFC button
  // QPushButton *button1 = new QPushButton("Reset Cam", myWidget);
  // button1->setGeometry(660, 480/6, 100, 30);

  // //create a Snapshot button
  // QPushButton *button2 = new QPushButton("Capture", myWidget);
  // button2->setGeometry(660, 480/6*2, 100, 30);

  // //create a GrayScale button
  // QPushButton *button3 = new QPushButton("Black White", myWidget);
  // button3->setGeometry(660, 480/6*3, 100, 30);

  // //create a Rainbow button
  // QPushButton *button4 = new QPushButton("Colorful", myWidget);
  // button4->setGeometry(660, 480/6*4, 100, 30);

  // //create a IronBlack button
  // QPushButton *button5 = new QPushButton("ResetColor", myWidget);
  // button5->setGeometry(660, 480/6*5, 100, 30);

  // //create a thread to gather SPI data
  // //when the thread emits updateImage, the label should update its image
  // accordingly LeptonThread *thread = new LeptonThread();
  // QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel,
  // SLOT(setImage(QImage)));

  // //connect ffc button to the thread's ffc action
  // QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));
  // thread->start();

  // //connect snapshot button to the thread's snapshot action
  // QObject::connect(button2, SIGNAL(clicked()), thread, SLOT(snapImage()));
  // thread->start();

  // //connect GrayScale button to the thread's snapshot action
  // QObject::connect(button3, SIGNAL(clicked()), thread, SLOT(greyMap()));
  // thread->start();

  // //connect Rainbow button to the thread's snapshot action
  // QObject::connect(button4, SIGNAL(clicked()), thread, SLOT(rainMap()));
  // thread->start();

  // //connect IronBlack button to the thread's snapshot action
  // QObject::connect(button5, SIGNAL(clicked()), thread, SLOT(ironMap()));
  // thread->start();

  // myWidget->show();

  return a.exec();
}
