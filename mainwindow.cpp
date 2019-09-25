#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // init central widget
  QWidget *widget = new QWidget;
  setCentralWidget(widget);

  // init layout
  m_vertside_layout = new QVBoxLayout;

  // init combobox overlap selector
  m_overlap_selector = new QComboBox;

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



  // init push buttons
  m_btn_capture = new QPushButton("Capture");
  m_btn_performffc = new QPushButton("Perform FFC");

  // define vertical layout buttons
  m_vertside_layout->addWidget(m_btn_performffc);
  m_vertside_layout->addWidget(create_colour_selector());
  m_vertside_layout->addWidget(m_btn_capture);







  widget->setLayout(create_label_preview());

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

  // images
  delete m_lepton_image;
  delete m_raspic_image;
  delete m_overlap_image;

  // label
  delete m_lepton_label;
  delete m_raspic_label;
  delete m_overlap_label;
  delete m_group_label;

  // colour
  delete m_rbtn_rainbow;
  delete m_rbtn_grayscale;
  delete m_rbtn_ironblack;
  delete m_vertcolour_layout;
  delete m_colour_group;
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

  return m_colour_group;
}

QGridLayout * MainWindow::create_label_preview()
{
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
    m_group_label->addWidget(m_overlap_label,1,0,1,3);

    return m_group_label;
}
