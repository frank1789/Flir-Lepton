#include <QApplication>
#include <QObject>
#include <QPointer>
#include "mainwindow.hpp"

#include "camerathread.hpp"
#include "leptonthread.hpp"

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowTitle("Flir-Lepton Video");
  // create Thread
  QPointer<LeptonThread> lepton = new LeptonThread();
  QPointer<CameraThread> raspicam = new CameraThread();
  // connect signal
  QObject::connect(lepton, &LeptonThread::updateImage, &w,
                   &MainWindow::set_thermal_image);
  QObject::connect(raspicam, &CameraThread::updateImage, &w,
                   &MainWindow::set_rgb_image);

  QObject::connect(&w, &MainWindow::changeColourMap, lepton,
                   &LeptonThread::changeColourMap);
  QObject::connect(&w, &MainWindow::performFFC, lepton,
                   &LeptonThread::performFFC);
  QObject::connect(&w, &MainWindow::captureImage, lepton,
                   &LeptonThread::snapImage);

  // open window
  w.show();

  // start thread
  lepton->start();
  raspicam->start();

  return a.exec();
}
