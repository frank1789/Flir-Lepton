#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <QWidget>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QNetworkSession;
class QGroupBox;
QT_END_NAMESPACE

class TcpClient : public QWidget
{
  Q_OBJECT
 public:
  explicit TcpClient(QWidget *parent = nullptr);

  void test();
 signals:

 public slots:
  void sendTestMessage();

 private slots:
  void connectedToServer();
  void on_disconnect_clicked();

  void sessionOpened();

  void displayError(QAbstractSocket::SocketError socketError);
  void enableConnectButton();

  void readFortune();

  void readyRead();

  void on_connect_clicked();

 private:
  /**
   * @brief Create a Information Group object
   *
   * @return QGroupBox*
   */
  QGroupBox *createInformationGroup();

  /**
   * @brief Create a Log Group object
   *
   * @return QGroupBox*
   */
  QGroupBox *createLogGroup();

  // ui variables
  QLineEdit *m_port_linedit{nullptr};
  QLineEdit *m_user_linedit{nullptr};
  QTextEdit *m_log_text{nullptr};

  QComboBox *hostCombo{nullptr};
  QPushButton *connectButton{nullptr};
  QPushButton *disconnectButton{nullptr};

  QString currentFortune;

  // network variables
  QTcpSocket *m_tcp_socket{nullptr};
  QDataStream m_data;

  QNetworkSession *networkSession{nullptr};
  void disconnectByServer();

  QByteArray recive_data;
  void updateGui(QAbstractSocket::SocketState state);
};

#endif  // TCPCLIENT_HPP
