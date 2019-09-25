#include "mainwindow.hpp"
#include "palettes.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // init central widget
  QWidget *widget = new QWidget;
  setCentralWidget(widget);

  // init complete group
  m_group_all = new QGridLayout;

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

  m_group_all->addLayout(create_label_preview(), 0, 0);
  m_group_all->addLayout(create_bar_control(), 0, 1);
  widget->setLayout(m_group_all);

  // connect signal from this to respective classes label
  connect(this, &MainWindow::update_thermal_image, m_lepton_label,
          &MyLabel::setImage);
  connect(this, &MainWindow::update_rgb_image, m_raspic_label,
          &MyLabel::setImage);

  // connect signal button and radio buttons
  connect(m_btn_performffc, &QPushButton::clicked, [=]() { this->call_FFC(); });
  connect(m_btn_capture, &QPushButton::clicked,
          [=]() { this->call_capture_image(); });
  connect(m_rbtn_rainbow, &QRadioButton::clicked,[=]() { this->changeColour(); });
  connect(m_rbtn_grayscale, &QRadioButton::clicked,[=]() { this->changeColour(); });
  connect(m_rbtn_ironblack, &QRadioButton::clicked,[=]() { this->changeColour(); });
}

MainWindow::~MainWindow() {
  delete ui;

  // images
  delete m_lepton_image;
  delete m_raspic_image;
  delete m_overlap_image;

  // label
  delete m_lepton_label;
  delete m_raspic_label;
  delete m_overlap_label;
  delete m_group_label;

  // command
  delete m_btn_capture;
  delete m_btn_performffc;
  delete m_overlap_selector;
  delete m_vertical_lower;
  delete m_vertical_upper;
  delete m_vertical_bar;

  // colour
  delete m_rbtn_rainbow;
  delete m_rbtn_grayscale;
  delete m_rbtn_ironblack;
  delete m_vertcolour_layout;
  delete m_colour_group;
}

void MainWindow::set_thermal_image(QImage img) {
  emit update_thermal_image(img);
}

void MainWindow::set_rgb_image(QImage img) { emit update_rgb_image(img); }

void MainWindow::call_FFC() { emit performFFC(); }

void MainWindow::call_capture_image() { emit captureImage(); }

void MainWindow::changeColour() {
  if (m_rbtn_rainbow->isChecked()) {
      qDebug() << "change palette to rainbow";
    emit changeColourMap(colormap::rainbow);
  }
  if (m_rbtn_grayscale->isChecked()) {
      qDebug() << "change palette to grayscale";
    emit changeColourMap(colormap::grayscale);
  }
  if (m_rbtn_ironblack->isChecked()) {
      qDebug() << "change palette to ironblack";
    emit changeColourMap(colormap::ironblack);
  }
}

QGroupBox *MainWindow::create_colour_selector() {
  // init groupbox colour
  m_colour_group = new QGroupBox("Colour map");

  // init layout
  m_vertcolour_layout = new QVBoxLayout;

  // make radio buttons
  m_rbtn_rainbow = new QRadioButton("Rainbow");
  m_rbtn_grayscale = new QRadioButton("Gray scale");
  m_rbtn_ironblack = new QRadioButton("Iron Black");


  // define layout
  m_vertcolour_layout->addWidget(m_rbtn_ironblack);
  m_vertcolour_layout->addWidget(m_rbtn_grayscale);
  m_vertcolour_layout->addWidget(m_rbtn_rainbow);
  m_vertcolour_layout->addStretch(1);
  m_vertcolour_layout->setSpacing(10);
  m_colour_group->setLayout(m_vertcolour_layout);

  // set default colour option
  m_rbtn_ironblack->setChecked(true);
  m_rbtn_rainbow->setChecked(false);
  m_rbtn_grayscale->setChecked(false);

  return m_colour_group;
}

QGridLayout *MainWindow::create_label_preview() {
  // create horizzontal layout
  m_group_label = new QGridLayout;

  // allocate label's placeholder
  m_lepton_label = new MyLabel(this);
  m_raspic_label = new MyLabel(this);
  m_overlap_label = new MyLabel(this);

  // set minimum size
  m_lepton_label->setMinimumSize(320, 240);
  m_raspic_label->setMinimumSize(320, 240);
  m_overlap_label->setMinimumSize(640, 480);

  // load image
  m_lepton_label->setPixmap(QPixmap::fromImage(*m_lepton_image));
  m_raspic_label->setPixmap(QPixmap::fromImage(*m_raspic_image));
  m_overlap_label->setPixmap(QPixmap::fromImage(*m_overlap_image));

  // define horizzontal layout
  m_group_label->addWidget(m_lepton_label, 0, 0);
  m_group_label->addWidget(m_raspic_label, 0, 1);
  m_group_label->addWidget(m_overlap_label, 1, 0, 1, 3);

  return m_group_label;
}

QVBoxLayout *MainWindow::create_bar_control() {
  // init layout
  m_vertical_bar = new QVBoxLayout;

  // collect layout
  m_vertical_bar->addLayout(create_upper_control());
  m_vertical_bar->addStretch(0);
  m_vertical_bar->addLayout(create_lower_control());

  return m_vertical_bar;
}

QVBoxLayout *MainWindow::create_lower_control() {
  // init layout
  m_vertical_lower = new QVBoxLayout;

  // init push button
  m_btn_capture = new QPushButton("Capture");

  // init combobox overlap selector
  m_overlap_selector = new QComboBox;

  // define vertical lower layout button
  m_vertical_lower->addWidget(m_overlap_selector);
  m_vertical_lower->addStretch(1);
  m_vertical_lower->addWidget(m_btn_capture);

  return m_vertical_lower;
}

QVBoxLayout *MainWindow::create_upper_control() {
  // init layout
  m_vertical_upper = new QVBoxLayout;

  // init push buttons
  m_btn_performffc = new QPushButton("Perform FFC");

  // define vertical upper layout button
  m_vertical_upper->addWidget(m_btn_performffc);
  m_vertical_upper->addStretch(0);
  m_vertical_upper->addWidget(create_colour_selector());
  return m_vertical_upper;
}
