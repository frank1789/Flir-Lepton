#ifndef MYLABEL_HPP
#define MYLABEL_HPP

#include <QLabel>
#include <QWidget>
#include <QtCore>

/**
 * @brief Class extend QLabel to give it an extra slot, setImage because we
 * can't pass a QPixmap from our thread so we have to pass a QImage and turn the
 * QImage into a QPixmap before display in QLabel.
 */
class MyLabel : public QLabel {
  Q_OBJECT

 public:
  /**
   * @brief Construct a new My Label object
   *
   * @param parent
   */
  MyLabel(QWidget *parent = nullptr);

  /**
   * @brief Destroy the My Label object
   *
   */
  ~MyLabel();

 public slots:
  /**
   * @brief Set the Image object function that allows the transition from QImage
   * to QPixmap.
   */
  void setImage(QImage);
};

#endif  // MYLABEL_HPP
