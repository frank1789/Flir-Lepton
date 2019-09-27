#include <QApplication>
#include <QObject>
#include <QPointer>
#include "camerathread.hpp"
#include "imagecomposerthread.hpp"
#include "leptonthread.hpp"
#include "mainwindow.hpp"

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowTitle("Flir-Lepton Video");
  // create Thread
  QPointer<LeptonThread> lepton = new LeptonThread();
  QPointer<CameraThread> raspicam = new CameraThread();
  QPointer<ImageComposerThread> composer = new ImageComposerThread();
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

  // connect composer
  QObject::connect(composer, &ImageComposerThread::updateImage, &w,
                   &MainWindow::setCompose);
  QObject::connect(&w, &MainWindow::updateMode, composer,
                   &ImageComposerThread::setMode);

  QObject::connect(lepton, &LeptonThread::updateImage, composer,
                   &ImageComposerThread::setThermalImage);
  QObject::connect(raspicam, &CameraThread::updateImage, composer,
                   &ImageComposerThread::setRGBImage);

  // open window
  w.show();

  // start thread
  lepton->start();
  raspicam->start();
  composer->start();

  return a.exec();
}
