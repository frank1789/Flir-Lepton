#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // allocate image
  m_lepton_image = new QImage(320, 240, QImage::Format_RGB888);
  m_raspic_image = new QImage(320, 240, QImage::Format_RGB888);
  m_overlap_image = new QImage(640, 480, QImage::Format_RGB888);
  for (int i = 0; i < 320; i++) {
    for (int j = 0; j < 240; j++) {
      m_lepton_image->setPixel(i, j, qRgb(255, 0, 0));
      m_raspic_image->setPixel(i, j, qRgb(0, 255, 0));
    }
  }

  for (int i = 0; i < 640; i++) {
    for (int j = 0; j < 480; j++) {
      m_overlap_image->setPixel(i, j, qRgb(0, 0, 255));
    }
  }

  // allocate label's placeholder
  m_lepton_label = new MyLabel(this);
  m_raspic_label = new MyLabel(this);
  m_overlap_label = new MyLabel(this);
  m_lepton_label->setGeometry(10, 10, 320, 240);
  m_raspic_label->setGeometry(340, 10, 320, 240);
  m_overlap_label->setGeometry(10, 260, 640, 480);

  m_lepton_label->setPixmap(QPixmap::fromImage(*m_lepton_image));
  m_raspic_label->setPixmap(QPixmap::fromImage(*m_raspic_image));
  m_overlap_label->setPixmap(QPixmap::fromImage(*m_overlap_image));

  // connect signal from this to respective classes label
  connect(this, &MainWindow::update_thermal_image, m_lepton_label,
          &MyLabel::setImage);
  connect(this, &MainWindow::update_rgb_image, m_raspic_label,
          &MyLabel::setImage);
}

void MainWindow::set_thermal_image(QImage img) {
  emit update_thermal_image(img);
}
void MainWindow::set_rgb_image(QImage img) { emit update_rgb_image(img); }

MainWindow::~MainWindow() {
  delete ui;
  delete m_lepton_label;
  delete m_raspic_label;
  delete m_overlap_label;
  delete m_lepton_image;
  delete m_raspic_image;
  delete m_overlap_image;
}
