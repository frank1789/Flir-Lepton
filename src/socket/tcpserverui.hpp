#ifndef TCPSERVERUI_HPP
#define TCPSERVERUI_HPP

#include <QWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QGroupBox;
class QGridLayout;
QT_END_NAMESPACE

class TCPServerUi : public QWidget {
  Q_OBJECT
 public:
  explicit TCPServerUi(QWidget *parent = nullptr);
  /**
   * @brief Create a Information Group object.
   *
   * @return QGroupBox* box widget.
   */
  QGroupBox *createInformationGroup();

  /**
   * @brief Create a Log Group object.
   *
   * @return QGroupBox* box widget.
   */
  QGroupBox *createLogGroup();

 public slots:
  void onClientDisconnect();
  void onClientConnect();

 private:
  /**
   * @brief Detect IP address of server
   *
   * @return QString return IP address
   */
  QString findIpAddress();

 private:
  unsigned int m_device_count;
  // interface elements
  QLabel *connection_label{nullptr};
  QLabel *port_label{nullptr};
  QLabel *m_connection_address{nullptr};
  QLabel *m_port_number{nullptr};
  QLabel *m_device_label{nullptr};
  QLabel *m_status_label{nullptr};
};

#endif  // TCPSERVERUI_HPP
