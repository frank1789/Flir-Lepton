#include "tcpserver.hpp"

#include "commonconnection.hpp"

TCPServer::TCPServer() { listen(QHostAddress::Any, TCP_PORT); }

void TCPServer::incomingConnection(qintptr socket) {
  TCPThread *thread = new TCPThread(socket);
  connect(thread, SIGNAL(Disconnect()), this, SLOT(on_thread_close()));
  connect(this, SIGNAL(newImg(QByteArray)), thread,
          SLOT(on_newImg(QByteArray)));
  thread->start();
  emit Connect();
}

void TCPServer::on_thread_close() { emit Disconnect(); }

void TCPServer::is_newImg(QByteArray img) { emit newImg(img); }