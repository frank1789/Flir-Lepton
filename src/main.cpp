#include <QApplication>
#include <QObject>
#include <QPointer>

#include "findmodel.hpp"
#include "instrumentor.h"
#include "labels.hpp"
#include "leptonthread.hpp"
#include "mainwindow.hpp"
#include "model_tpu.hpp"

const QString local_model_path{"/resources/detect.tflite"};
const QString local_label_path{"/resources/coco_labels.txt"};

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
  auto model_path = QApplication::applicationDirPath() + local_model_path;
  auto label_path = QApplication::applicationDirPath() + local_label_path;
  LabelDetection label(label_path);
  label.read();
  ModelTensorFlowLite modeltflite(model_path);
  modeltflite.setLabel(label.getLabels());
  QObject::connect(lepton, &LeptonThread::updateCam,
                   [&modeltflite](QImage img) {
                     modeltflite.imageAvailable(QPixmap::fromImage(img));
                   });
  auto r = a.exec();
#if PROFILING
  Instrumentor::Get().EndSession();
#endif
  return r;
}
