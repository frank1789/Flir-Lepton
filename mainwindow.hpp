#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QImage>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include "mylabel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  void set_thermal_image(QImage img);
  void set_rgb_image(QImage img);

 private slots:
  void call_FFC();
  void call_capture_image();
  void changeColourMap();

 signals:
  void update_thermal_image(QImage);
  void update_rgb_image(QImage);
  void changeColour(const int *);
  void performFFC();
  void captureImage();

 private:
  Ui::MainWindow *ui;

  // image placeholder
  QImage *m_lepton_image;
  QImage *m_raspic_image;
  QImage *m_overlap_image;

  // label's placeholder
  MyLabel *m_lepton_label;
  MyLabel *m_raspic_label;
  MyLabel *m_overlap_label;

  // action buttons
  QPushButton *m_btn_performffc;
  QPushButton *m_btn_capture;

  // radio buttons
  QRadioButton *m_rbtn_rainbow;
  QRadioButton *m_rbtn_grayscale;
  QRadioButton *m_rbtn_ironblack;

  // group box
  QGroupBox *m_colour_group;

  // combo box
  QComboBox *m_overlap_selector;

  // layout
  QVBoxLayout *m_vertical_upper;
  QVBoxLayout *m_vertical_lower;
  QVBoxLayout *m_vertcolour_layout;
  QVBoxLayout *m_vertical_bar;
  QHBoxLayout *m_preview_label;
  QGridLayout *m_group_label;
  QGridLayout *m_group_all;

  // layout generator methods
  QGroupBox *create_colour_selector();
  QGridLayout *create_label_preview();
  QVBoxLayout *create_bar_control();
  QVBoxLayout *create_lower_control();
  QVBoxLayout *create_upper_control();
};
#endif  // MAINWINDOW_HPP
