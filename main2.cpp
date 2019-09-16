#include <QApplication>
#include <QPointer>
#include <QObject>
#include "mainwindow.hpp"

#include "camerathread.hpp"
#include "leptonthread.hpp"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;

  // create Thread
  QPointer<LeptonThread> lepton = new LeptonThread();
  QPointer<CameraThread> raspicam = new CameraThread();

  // connect signal
  QObject::connect(lepton, &LeptonThread::updateImage, &w, &MainWindow::set_thermal_image);
  QObject::connect(raspicam, &CameraThread::updateImage, &w, &MainWindow::set_rgb_image);

  w.show();
  return a.exec();
}
