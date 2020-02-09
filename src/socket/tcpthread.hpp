#ifndef TCPTHREAD_HPP
#define TCPTHREAD_HPP

#include <QHostAddress>
#include <QTcpSocket>
#include <QThread>

class TCPThread : public QThread {
  Q_OBJECT
 public:
  TCPThread(int nsocket);
 signals:
  void Disconnect();
 public slots:
  void on_newImg(QByteArray img);

 private:
  QTcpSocket* socket;
  char nbClient;
};

#endif  // TCPTHREAD_HPP