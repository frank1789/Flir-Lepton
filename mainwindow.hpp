#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QImage>
#include <QMainWindow>
#include "MyLabel.h"

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
  void setCompose(QImage img);

 signals:
  void update_thermal_image(QImage);
  void update_rgb_image(QImage);
  void updateCompose(QImage);

 private:
  Ui::MainWindow *ui;
  // image placeholder
  QImage *m_lepton_image;
  QImage *m_raspic_image;
  QImage *m_overlap_image;

  // label's placeholder
  MyLabel *m_lepton_label;
  MyLabel *m_raspic_label;
  MyLabel * m_compose_label;
};
#endif  // MAINWINDOW_HPP
