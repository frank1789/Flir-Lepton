#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
class QUdpSocket;
class QLabel;
class QPushButton;
class QTextEdit;
class QGroupBox;
class QGridLayout;
QT_END_NAMESPACE

class TcpServer : public QWidget {
  Q_OBJECT

 public:
  explicit TcpServer(QWidget *parent = nullptr);
  ~TcpServer();

 private slots:
  /**
   * @brief Adds clients to the server.
   *
   * Function that adds a new client to the server, if this happens successfully
   * a message is shown in the log.
   *
   */
  void newConnection();

  /**
   * @brief Removes client connected to the server.
   *
   * Function that removes clients connected to the server when the disconnect
   * button is pressed in clients UI.
   *
   */
  void removeConnection();

  /**
   * @brief Read from socket.
   *
   * Function that reads the socket when data are available.
   *
   */
  void readyRead();

  /**
   * @brief Removes clients connected to the server.
   *
   * Function that removes all clients connected to the server when the
   * disconnect button is pressed.
   *
   */
  void onDisconnectClientsClicked();

 private:
  /**
   * @brief sends text message
   *
   * Function that shows updates the server log and sends the message.
   *
   * @param sender[in] tcp-socket required to send the buffer.
   * @param message[in] string containing the text message.
   */
  void newMessage(QTcpSocket *sender, const QString &message);

  /**
   * @brief sends picture message
   *
   * Function that shows updates the server log and sends the message.
   *
   * @param sender[in] tcp-socket required to send the buffer.
   * @param image[in] containing the picture.
   */
  void newMessage(QTcpSocket *sender, const QImage &image);

  /**
   * @brief Create a Log Group object
   *
   * @return QGroupBox* widget.
   */
  QGroupBox *createLogGroup();

  /**
   * @brief Create a Information Group object
   *
   * @return QGridLayout* layout.
   */
  QGridLayout *createInformationGroup();

  /**
   * @brief search which address and port the client should connect to.
   *
   * In then displays the port QTcpServer picked in a label, so that user knows
   * which port the client should connect to.
   *
   * @return QString the address which port the client should connect to.
   */
  QString findIpAddress();

  // interface elements
  QPushButton *disconnectButton{nullptr};
  QLabel *connection_label{nullptr};
  QLabel *port_label{nullptr};
  QLabel *m_connection_address{nullptr};
  QLabel *m_port_number{nullptr};
  QTextEdit *m_log_text{nullptr};

  QTcpServer *m_server{nullptr};
  QVector<QTcpSocket *> m_clients;
  QHash<QTcpSocket *, QByteArray> m_receivedData;
};

#endif  // TCPSERVER_HPP
