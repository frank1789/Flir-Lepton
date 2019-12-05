#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
class QLabel;
class QPushButton;
class QTextEdit;
class QGroupBox;
class QGridLayout;
QT_END_NAMESPACE

class TcpServer : public QWidget {
  Q_OBJECT

 public:
  explicit TcpServer(QWidget *parent = 0);
  ~TcpServer();

 private slots:
  /**
   * @brief
   *
   */
  void newConnection();

  /**
   * @brief
   *
   */
  void removeConnection();

  /**
   * @brief
   *
   */
  void readyRead();

  /**
   * @brief
   *
   */
  void on_disconnectClients_clicked();

 private:
  QTcpServer *m_server{nullptr};
  QVector<QTcpSocket *> m_clients;
  QHash<QTcpSocket *, QByteArray> m_receivedData;

  void newMessage(QTcpSocket *sender, const QString &message);

  // interface elements
  QPushButton *disconnectButton{nullptr};
  QLabel *connection_label{nullptr};
  QLabel *port_label{nullptr};
  QLabel *m_connection_address{nullptr};
  QLabel *m_port_number{nullptr};
  QTextEdit *m_log_text{nullptr};

  // function interface
  QGroupBox *createLogGroup();
  QGridLayout *createInformationGroup();
};

#endif  // TCPSERVER_HPP
