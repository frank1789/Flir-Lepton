#include "tcpserver.hpp"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>

#include "../log/logger.h"

constexpr int DEFAULT_PORT{52693};
constexpr int TERMINATION_ASCII_CODE{23};

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent), m_server(new QTcpServer(this)) {
  this->setWindowTitle("Server");
  // assemble ui
  QGridLayout *grid = new QGridLayout;
  grid->addLayout(createInformationGroup(), 0, 0);
  grid->addWidget(createLogGroup(), 1, 0);
  setLayout(grid);

  if (!m_server->listen(QHostAddress::Any, DEFAULT_PORT)) {
#if LOGGER
    LOG(ERROR, "Failure while starting server:")
    qDebug() << m_server->errorString();
#endif
    m_log_text->append(
        tr("Failure while starting server: %1").arg(m_server->errorString()));
    return;
  }

  // connect signal
  connect(m_server, &QTcpServer::newConnection,
          [=]() { this->newConnection(); });
  m_connection_address->setText(m_server->serverAddress().toString());
  m_port_number->setText(QString::number(m_server->serverPort()));
  connect(disconnectButton, &QPushButton::clicked,
          [=]() { this->on_disconnectClients_clicked(); });
}

TcpServer::~TcpServer() {}

void TcpServer::newConnection() {
  qDebug() << "enter new connection";
  while (m_server->hasPendingConnections()) {
    qDebug() << "cycle while";
    QTcpSocket *socket = m_server->nextPendingConnection();
    m_clients << socket;
    disconnectButton->setEnabled(true);
    connect(socket, &QTcpSocket::disconnected, this,
            &TcpServer::removeConnection);
    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::readyRead);
    m_log_text->append(tr("* New connection: %1, port %2")
                           .arg(socket->peerAddress().toString())
                           .arg(socket->peerPort()));
  }
}

void TcpServer::removeConnection() {
  QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
  if (!socket) {
    return;
  }
  m_log_text->append(tr("* Connection removed: %1, port %2")
                         .arg(socket->peerAddress().toString())
                         .arg(socket->peerPort()));
  m_clients.removeOne(socket);
  m_receivedData.remove(socket);
  socket->deleteLater();
  disconnectButton->setEnabled(!m_clients.isEmpty());
}

void TcpServer::readyRead() {
  QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
  if (!socket) {
#if LOGGER
    LOG(ERROR, "socket not readable, then exit")
#endif
    return;
  }
  QByteArray &buffer = m_receivedData[socket];
  buffer.append(socket->readAll());
  while (true) {
    auto endIndex = buffer.indexOf(TERMINATION_ASCII_CODE);
    if (endIndex < 0) {
      break;
    }
    QString message = QString::fromUtf8(buffer.left(endIndex));
    buffer.remove(0, endIndex + 1);
    newMessage(socket, message);
  }
}

void TcpServer::on_disconnectClients_clicked() {
  foreach (QTcpSocket *socket, m_clients) { socket->close(); }
  disconnectButton->setEnabled(false);
#if LOGGER
  LOG(DEBUG, "disconnect all socket")
#endif
}

void TcpServer::newMessage(QTcpSocket *sender, const QString &message) {
  m_log_text->append(tr("Sending message: %1").arg(message));
  QByteArray messageArray = message.toUtf8();
  messageArray.append(TERMINATION_ASCII_CODE);
  for (QTcpSocket *socket : m_clients) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
      socket->write(messageArray);
    }
  }
  Q_UNUSED(sender)
}

QGridLayout *TcpServer::createInformationGroup() {
#if LOGGER
  LOG(INFO, "build information ui")
#endif

  // define group layout
  auto grid_layout = new QGridLayout;

  // init elements
  disconnectButton = new QPushButton("Disconnect");
  disconnectButton->setEnabled(false);
  connection_label = new QLabel("Connection:");
  port_label = new QLabel("Port:");
  m_log_text = new QTextEdit;
  m_connection_address = new QLabel;
  m_port_number = new QLabel;

  // assemble element
  grid_layout->addWidget(connection_label, 0, 0);
  grid_layout->addWidget(m_connection_address, 0, 1);
  grid_layout->addWidget(port_label, 0, 2);
  grid_layout->addWidget(m_port_number, 0, 3);
  grid_layout->addWidget(disconnectButton, 0, 4);

  // return layout
  return grid_layout;
}

QGroupBox *TcpServer::createLogGroup() {
#if LOGGER
  LOG(INFO, "build logging group ui")
#endif

  // defining layout group
  QGroupBox *groupBox = new QGroupBox(tr("Log"));

  // init element ui
  m_log_text = new QTextEdit;

  // setup grid layout
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(m_log_text, 0, 0);
  groupBox->setLayout(gridLayout);
  return groupBox;
}
