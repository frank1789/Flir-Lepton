#include <QApplication>
#include <QObject>
#include <QPointer>
#include "leptonthread.hpp"
#include "mainwindow.hpp"

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowTitle("Flir-Lepton Video");
  // create Thread
  QPointer<LeptonThread> lepton = new LeptonThread();
  // connect signal
  QObject::connect(lepton, &LeptonThread::updateImage, &w,
                   &MainWindow::set_thermal_image);
  QObject::connect(lepton, &LeptonThread::updateCam, &w,
                   &MainWindow::set_rgb_image);
  QObject::connect(lepton, &LeptonThread::updateOverlay, &w,
                   &MainWindow::setCompose);
  QObject::connect(&w, &MainWindow::changeColourMap, lepton,
                   &LeptonThread::changeColourMap);
  QObject::connect(&w, &MainWindow::performFFC, lepton,
                   &LeptonThread::performFFC);
  QObject::connect(&w, &MainWindow::captureImage, lepton,
                   &LeptonThread::snapImage);
  QObject::connect(&w, &MainWindow::updateMode, lepton, &LeptonThread::setMode);
  // open window
  w.show();

  // start thread
  lepton->start();

  return a.exec();
}
