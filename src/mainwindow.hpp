#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPainter>

#include "mylabel.hpp"
#include "socket/tcpclient.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QComboBox;
class QGroupBox;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;
class QImage;
class QPushButton;
class QRadioButton;
class QString;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  /**
   * @brief Set the thermal image object
   *
   * updates the image from the thermal camera.
   *
   * @param img[in] input image
   */
  void set_thermal_image(QImage img);

  /**
   * @brief Set the rgb image object
   *
   * updates the image from the thermal camera.
   *
   * @param img[in] input image
   */
  void set_rgb_image(QImage img);

  /**
   * @brief Set the Compose object
   *
   * the function works on two images with a method specified by the ui and
   * circumvents the image on the label.
   *
   * @param img[in] input image
   */
  void setCompose(QImage img);

  /**
   * @brief Update index of combo box selector.
   *
   * updates the index of the merging method currently in use.
   *
   * @param index[in] integer corresponding to the index.
   */
  void indexChanged(int index);

 private slots:
  void call_FFC();
  void call_capture_image();
  void changeColour();

 signals:
  void update_thermal_image(QImage);
  void update_rgb_image(QImage);
  void updateCompose(QImage);
  void changeColourMap(const int *);
  void performFFC();
  void captureImage();
  void updateMode(int);

 private:
  Ui::MainWindow *ui;

  // image placeholder
  QImage *m_lepton_image{nullptr};
  QImage *m_raspic_image{nullptr};
  QImage *m_overlap_image{nullptr};

  // label's placeholder
  MyLabel *m_lepton_label{nullptr};
  MyLabel *m_raspic_label{nullptr};
  MyLabel *m_overlap_label{nullptr};

  // action buttons
  QPushButton *m_btn_performffc{nullptr};
  QPushButton *m_btn_capture{nullptr};

  // radio buttons
  QRadioButton *m_rbtn_rainbow{nullptr};
  QRadioButton *m_rbtn_grayscale{nullptr};
  QRadioButton *m_rbtn_ironblack{nullptr};

  // combo box
  QComboBox *m_overlap_selector{nullptr};

  // layout generator methods
  /**
   * @brief Create a colour selector object.
   *
   * @return QGroupBox* [out] layout.
   */
  QGroupBox *create_colour_selector();

  /**
   * @brief Create a label preview object.
   *
   * function that builds the layout of the UI.
   *
   * @return QGridLayout* [out] layout.
   */
  QGridLayout *create_label_preview();

  /**
   * @brief Create a bar control object.
   *
   * function that builds the layout of the UI.
   *
   * @return QVBoxLayout* [out] layout.
   */
  QVBoxLayout *create_bar_control();

  /**
   * @brief Create a lower control object.
   *
   * function that builds the layout of the UI.
   *
   * @return QVBoxLayout* [out] layout.
   */
  QVBoxLayout *create_lower_control();

  /**
   * @brief Create a upper control object.
   *
   * function that builds the layout of the UI.
   *
   * @return QVBoxLayout* [out] layout.
   */
  QVBoxLayout *create_upper_control();

  // group box
  void addOp(QPainter::CompositionMode mode, const QString &name);

  // socket client layout
  TcpClient *client{nullptr};
};
#endif  // MAINWINDOW_HPP
