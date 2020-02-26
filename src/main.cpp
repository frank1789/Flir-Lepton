#include <QApplication>
#include <QObject>
#include <QPointer>

#include "log/instrumentor.h"
#include "../tf/findmodel.hpp"
#include "../tf/model_tpu.hpp"
#include "../tf/util_label_image.hpp"
#include "leptonthread.hpp"
#include "mainwindow.hpp"

int main(int argc, char **argv) {
#if PROFILING
  Instrumentor::Get().BeginSession("Profile");
#endif
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowTitle("Flir-Lepton Video");
  // create Thread
  QPointer<LeptonThread> lepton = new LeptonThread();
  // connect signal from lepton thread class to mainwindow
  QObject::connect(lepton, &LeptonThread::updateImage, &w,
                   &MainWindow::set_thermal_image);
  QObject::connect(lepton, &LeptonThread::updateCam, &w,
                   &MainWindow::set_rgb_image);
  QObject::connect(lepton, &LeptonThread::updateOverlay, &w,
                   &MainWindow::setCompose);
  // connect signal from mainwindow to lepton thread class
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
  // initialize ui for select TensorFlow lite nad label map
  FindModel m;
  m.exec();
  while (m.isVisible()) {
    m.show();
  }
  // get path of file and initialize model
  auto dd = m.getLabelPath();
  auto zz = m.getModelPath();
  LabelDetection label(dd);
  label.read();
  ModelTensorFlowLite modeltflite(zz);
  modeltflite.setLabel(label.getLabels());
  QObject::connect(lepton, &LeptonThread::updateCam, [&modeltflite](QImage img){modeltflite.imageAvailable(QPixmap::fromImage(img)));

  auto r = a.exec();
#if PROFILING
  Instrumentor::Get().EndSession();
#endif
  return r;
}
