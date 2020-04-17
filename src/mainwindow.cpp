#include "mainwindow.hpp"

#include <unistd.h>

#include <QColor>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QImage>
#include <QMutexLocker>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>

#include "instrumentor.h"
#include "logger.h"
#include "palettes.h"
#include "socket/tcpserverui.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // init central widget
  QWidget *widget = new QWidget;
  setCentralWidget(widget);

  // init complete group
  auto m_group_all = new QGridLayout;

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

  server = new TCPServer();
  auto ui_server = new TCPServerUi();
  connect(server, &TCPServer::Connect, ui_server,
          &TCPServerUi::onClientConnect);
  connect(server, &TCPServer::Disconnect, ui_server,
          &TCPServerUi::onClientDisconnect);
  m_group_all->addLayout(create_label_preview(), 0, 0);
  m_group_all->addLayout(create_bar_control(), 0, 1);
  m_group_all->addWidget(ui_server, 0, 2);
  widget->setLayout(m_group_all);

  // connect signal from this to respective classes label
  connect(this, &MainWindow::update_thermal_image,
          [=](QImage img) { m_lepton_label->setImage(img); });
  connect(this, &MainWindow::update_rgb_image,
          [=](QImage img) { m_raspic_label->setImage(img); });
  connect(this, &MainWindow::updateCompose,
          [=](QImage img) { m_overlap_label->setImage(img); });

  // connect signal button and radio buttons
  connect(m_btn_performffc, &QPushButton::clicked, [=]() { this->call_FFC(); });
  connect(m_btn_capture, &QPushButton::clicked,
          [=]() { this->call_capture_image(); });
  connect(m_rbtn_rainbow, &QRadioButton::clicked,
          [=]() { this->changeColour(); });
  connect(m_rbtn_grayscale, &QRadioButton::clicked,
          [=]() { this->changeColour(); });
  connect(m_rbtn_ironblack, &QRadioButton::clicked,
          [=]() { this->changeColour(); });

  // connect combobox
  connect(m_overlap_selector,
          QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index) { this->indexChanged(index); });

  // connect Camera to TcpClient
  connect(this, &MainWindow::update_rgb_image, [=](QImage image) {
    PROFILE_FUNCTION();
    QMutexLocker locker(&mutex);
    QPixmap img = QPixmap::fromImage(image);
    QByteArray bImage;
    QBuffer bBuffer(&bImage);
    // Putting every image in the buffer
    bBuffer.open(QIODevice::ReadWrite);
    img.save(&bBuffer, "JPG");
    // Sending to TCPServer function to display the image
    server->is_newImg(bImage);
  });
}

MainWindow::~MainWindow() {
  delete ui;
  delete server;
}

void MainWindow::set_thermal_image(QImage img) {
  emit update_thermal_image(img);
}

void MainWindow::set_rgb_image(QImage img) { emit update_rgb_image(img); }

void MainWindow::setCompose(QImage img) {
  PROFILE_FUNCTION();
  emit updateCompose(img);
}

void MainWindow::call_FFC() { emit performFFC(); }

void MainWindow::call_capture_image() { emit captureImage(); }

void MainWindow::changeColour() {
  if (m_rbtn_rainbow->isChecked()) {
#if LOGGER
    LOG(TRACE, "change palette to rainbow")
#endif
    emit changeColourMap(colormap::rainbow);
  }
  if (m_rbtn_grayscale->isChecked()) {
#if LOGGER
    LOG(TRACE, "change palette to grayscale")
#endif
    emit changeColourMap(colormap::grayscale);
  }
  if (m_rbtn_ironblack->isChecked()) {
#if LOGGER
    LOG(TRACE, "change palette to ironblack")
#endif
    emit changeColourMap(colormap::ironblack);
  }
}

void MainWindow::indexChanged(int index) { emit updateMode(index); }

QGroupBox *MainWindow::create_colour_selector() {
#if LOGGER
  LOG(INFO, "make colour selector ui")
#endif

  // init groupbox colour
  auto m_colour_group = new QGroupBox("Colour map");

  // init layout
  auto m_vertcolour_layout = new QVBoxLayout;

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

  // return layout
  return m_colour_group;
}

QGridLayout *MainWindow::create_label_preview() {
#if LOGGER
  LOG(INFO, "make preview label ui.")
#endif

  // create horizontal layout
  auto m_group_label = new QGridLayout;

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

  // define horizontal layout
  m_group_label->addWidget(m_lepton_label, 0, 0);
  m_group_label->addWidget(m_raspic_label, 0, 1);
  m_group_label->addWidget(m_overlap_label, 1, 0, 1, 3);

  // return layout
  return m_group_label;
}

QVBoxLayout *MainWindow::create_bar_control() {
#if LOGGER
  LOG(INFO, "make bar control ui.")
#endif

  // init layout
  auto m_vertical_bar = new QVBoxLayout;

  // collect layout
  m_vertical_bar->addLayout(create_upper_control());
  m_vertical_bar->addStretch(0);
  m_vertical_bar->addLayout(create_lower_control());

  // return layout
  return m_vertical_bar;
}

QVBoxLayout *MainWindow::create_lower_control() {
#if LOGGER
  LOG(INFO, "make lower control ui.")
#endif

  // init layout
  auto m_vertical_lower = new QVBoxLayout;

  // init push button
  m_btn_capture = new QPushButton("Capture");

  // init combobox overlap selector
  m_overlap_selector = new QComboBox;
  addOp(QPainter::CompositionMode_SourceOver, tr("SourceOver"));
  addOp(QPainter::CompositionMode_DestinationOver, tr("DestinationOver"));
  addOp(QPainter::CompositionMode_Clear, tr("Clear"));
  addOp(QPainter::CompositionMode_Source, tr("Source"));
  addOp(QPainter::CompositionMode_Destination, tr("Destination"));
  addOp(QPainter::CompositionMode_SourceIn, tr("SourceIn"));
  addOp(QPainter::CompositionMode_DestinationIn, tr("DestinationIn"));
  addOp(QPainter::CompositionMode_SourceOut, tr("SourceOut"));
  addOp(QPainter::CompositionMode_DestinationOut, tr("DestinationOut"));
  addOp(QPainter::CompositionMode_SourceAtop, tr("SourceAtop"));
  addOp(QPainter::CompositionMode_DestinationAtop, tr("DestinationAtop"));
  addOp(QPainter::CompositionMode_Xor, tr("Xor"));
  addOp(QPainter::CompositionMode_Plus, tr("Plus"));
  addOp(QPainter::CompositionMode_Multiply, tr("Multiply"));
  addOp(QPainter::CompositionMode_Screen, tr("Screen"));
  addOp(QPainter::CompositionMode_Overlay, tr("Overlay"));
  addOp(QPainter::CompositionMode_Darken, tr("Darken"));
  addOp(QPainter::CompositionMode_Lighten, tr("Lighten"));
  addOp(QPainter::CompositionMode_ColorDodge, tr("ColorDodge"));
  addOp(QPainter::CompositionMode_ColorBurn, tr("ColorBurn"));
  addOp(QPainter::CompositionMode_HardLight, tr("HardLight"));
  addOp(QPainter::CompositionMode_SoftLight, tr("SoftLight"));
  addOp(QPainter::CompositionMode_Difference, tr("Difference"));
  addOp(QPainter::CompositionMode_Exclusion, tr("Exclusion"));

  // default value overlap_selector
  m_overlap_selector->setCurrentIndex(15);

  // define vertical lower layout button
  m_vertical_lower->addWidget(m_overlap_selector);
  m_vertical_lower->addStretch(15);
  m_vertical_lower->addWidget(m_btn_capture);

  // return layout
  return m_vertical_lower;
}

QVBoxLayout *MainWindow::create_upper_control() {
#if LOGGER
  LOG(INFO, "make upper control ui.")
#endif

  // init layout
  auto m_vertical_upper = new QVBoxLayout;

  // init push buttons
  m_btn_performffc = new QPushButton("Perform FFC");

  // define vertical upper layout button
  m_vertical_upper->addWidget(m_btn_performffc);
  m_vertical_upper->addStretch(0);
  m_vertical_upper->addWidget(create_colour_selector());

  // return layout
  return m_vertical_upper;
}

void MainWindow::addOp(QPainter::CompositionMode mode, const QString &name) {
  m_overlap_selector->addItem(name, mode);
}
