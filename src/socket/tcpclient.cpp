#include "tcpclient.hpp"

#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkConfiguration>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QTextEdit>

#include "../log/logger.h"

constexpr int TERMINATION_ASCII_CODE{23};

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent), m_tcp_socket(new QTcpSocket(this)) {
  // assemble ui
  connectButton = new QPushButton("Connect");
  disconnectButton = new QPushButton("Disconnect");
  connectButton->setEnabled(false);
  disconnectButton->setEnabled(false);
#if LOGGER
  LOG(INFO, "dis/connect buttons are set disable")
#endif
  QGridLayout *grid = new QGridLayout;
  grid->addWidget(createInformationGroup(), 0, 0, 1, 3);
  grid->addWidget(connectButton, 1, 1);
  grid->addWidget(disconnectButton, 1, 2);
  grid->addWidget(createLogGroup(), 2, 0, 1, 3);
  setLayout(grid);
#if LOGGER
  LOG(INFO, "ctor Client init params")
#endif

  // config client
  m_data.setDevice(m_tcp_socket);
  m_data.setVersion(QDataStream::Qt_4_0);

  // connect functions
  connect(m_tcp_socket, &QTcpSocket::readyRead, [=]() { this->readyRead(); });
  connect(m_tcp_socket, &QTcpSocket::connected,
          [=]() { this->connectedToServer(); });
  connect(m_tcp_socket, &QTcpSocket::disconnected,
          [=]() { this->disconnectByServer(); });

  // connect buttons
  connect(disconnectButton, &QPushButton::clicked,
          [=]() { this->on_disconnect_clicked(); });
  connect(connectButton, &QPushButton::clicked,
          [=]() { this->on_connect_clicked(); });

  // connect ui
  connect(hostCombo, &QComboBox::editTextChanged, this,
          &TcpClient::enableConnectButton);
  connect(m_port_linedit, &QLineEdit::textChanged, this,
          &TcpClient::enableConnectButton);
  // connect error
  // connect(m_tcp_socket, &QIODevice::readyRead, this,
  // &TcpClient::readFortune);
  connect(m_tcp_socket,
          QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
          this, &TcpClient::displayError);

  // network manager
  QNetworkConfigurationManager manager;
  if (manager.capabilities() &
      QNetworkConfigurationManager::NetworkSessionRequired) {
    // Get saved network configuration
    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    const QString id =
        settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
    settings.endGroup();
    // If the saved network configuration is not currently discovered use the
    // system default
    QNetworkConfiguration config = manager.configurationFromIdentifier(id);
    if ((config.state() & QNetworkConfiguration::Discovered) !=
        QNetworkConfiguration::Discovered) {
      config = manager.defaultConfiguration();
    }
    networkSession = new QNetworkSession(config, this);
    connect(networkSession, &QNetworkSession::opened, this,
            &TcpClient::sessionOpened);

    connectButton->setEnabled(false);
    m_log_text->append(tr("Opening network session."));
    LOG(INFO, "Opening network session.")
    networkSession->open();
  }
}

QGroupBox *TcpClient::createInformationGroup() {
  LOG(INFO, "build information group ui")
  // defining layout group
  QGroupBox *groupBox = new QGroupBox(tr("Server Configuration"));

  // init element ui
  QLabel *address_label = new QLabel("Address:");
  QLabel *port_label = new QLabel("Port:");
  QLabel *user_label = new QLabel("User:");
  hostCombo = new QComboBox;
  hostCombo->setEditable(true);
  m_port_linedit = new QLineEdit;
  m_user_linedit = new QLineEdit;

  // find out name of this machine
  QString name = QHostInfo::localHostName();
  m_user_linedit->setText(name);
  if (!name.isEmpty()) {
    hostCombo->addItem(name);
    QString domain = QHostInfo::localDomainName();
    if (!domain.isEmpty()) hostCombo->addItem(name + QChar('.') + domain);
  }
  if (name != QLatin1String("localhost"))
    hostCombo->addItem(QString("localhost"));

  // find out IP addresses of this machine
  QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
  // add non-localhost addresses
#if LOGGER
  LOG(DEBUG, "display all IP:")
#endif
  for (int i = 0; i < ipAddressesList.size(); ++i) {
    if (!ipAddressesList.at(i).isLoopback()) {
      hostCombo->addItem(ipAddressesList.at(i).toString());
    }
  }

  // add localhost addresses
  for (int i = 0; i < ipAddressesList.size(); ++i) {
    if (ipAddressesList.at(i).isLoopback()) {
      hostCombo->addItem(ipAddressesList.at(i).toString());
    }
  }
#if LOGGER
  for (const auto &list : ipAddressesList) {
    qDebug() << "find out IP addresses:" << list.toString();
  }
#endif

  // validate input port must be 1 <= x <= 65535
  m_port_linedit->setValidator(new QIntValidator(1, 65535, this));

  // setup grid layout
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(address_label, 0, 0);
  gridLayout->addWidget(hostCombo, 0, 1);
  gridLayout->addWidget(port_label, 1, 0);
  gridLayout->addWidget(m_port_linedit, 1, 1);
  gridLayout->addWidget(user_label, 2, 0);
  gridLayout->addWidget(m_user_linedit, 2, 1);
  groupBox->setLayout(gridLayout);
  return groupBox;
}

QGroupBox *TcpClient::createLogGroup() {
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

void TcpClient::sessionOpened() {
  // Save the used configuration
  QNetworkConfiguration config = networkSession->configuration();
  QString id;
  if (config.type() == QNetworkConfiguration::UserChoice)
    id = networkSession
             ->sessionProperty(QLatin1String("UserChoiceConfiguration"))
             .toString();
  else
    id = config.identifier();

  QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
  settings.beginGroup(QLatin1String("QtNetwork"));
  settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
  settings.endGroup();
  connectButton->setEnabled(true);
}

void TcpClient::enableConnectButton() {
  connectButton->setEnabled((!networkSession || networkSession->isOpen()) &&
                            !hostCombo->currentText().isEmpty() &&
                            !m_port_linedit->text().isEmpty());
}

void TcpClient::displayError(QAbstractSocket::SocketError socketError) {
  switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
      break;
    case QAbstractSocket::HostNotFoundError:
      QMessageBox::information(this, tr("Client"),
                               tr("The host was not found. Please check the "
                                  "host name and port settings."));
      break;
    case QAbstractSocket::ConnectionRefusedError:
      QMessageBox::information(this, tr("Client"),
                               tr("The connection was refused by the peer. "
                                  "Make sure the fortune server is running, "
                                  "and check that the host name and port "
                                  "settings are correct."));
      break;
    default:
      QMessageBox::information(this, tr("Client"),
                               tr("The following error occurred: %1.")
                                   .arg(m_tcp_socket->errorString()));
  }
  m_tcp_socket->abort();
  updateGui(QAbstractSocket::UnconnectedState);
}

void TcpClient::readFortune() {
  m_data.startTransaction();

  QString nextFortune;
  m_data >> nextFortune;

  if (!m_data.commitTransaction()) return;

  if (nextFortune == currentFortune) {
    QTimer::singleShot(100, this, &TcpClient::connectedToServer);
    return;
  }

  currentFortune = nextFortune;
  m_log_text->append(currentFortune);
  connectButton->setEnabled(true);
}

void TcpClient::sendTestMessage() {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
#if LOGGER
    LOG(WARN, "socket test function not connected, then exit.")
#endif
    return;
  }
  QString test_message{"Test message sended form code."};
  QString message = QString("%1: %2")
                        .arg(m_user_linedit->text())
                        .arg(test_message.simplified());
  QByteArray ba_message = message.toUtf8();
  ba_message.append(TERMINATION_ASCII_CODE);
  m_tcp_socket->write(ba_message);
}

void TcpClient::readyRead() {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
#if LOGGER
    LOG(WARN, "impossible read from socket.")
#endif
    return;
  }
  receive_data.append(m_tcp_socket->readAll());
  while (true) {
    auto end_index = receive_data.indexOf(TERMINATION_ASCII_CODE);
    if (end_index < 0) {
      break;
    }
    QString message = QString::fromUtf8(receive_data.left(end_index));
    receive_data.remove(0, end_index + 1);
    m_log_text->append(message);
  }
}

void TcpClient::on_connect_clicked() {
  if (m_user_linedit->text().isEmpty()) {
#if LOGGER
    LOG(ERROR, "unable to connect, must define user name.")
#endif
    m_log_text->append(
        tr("== Unable to connect to server.\nYou must define an user name."));
    return;
  }
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
#if LOGGER
    LOG(DEBUG, "try connect.")
#endif
    m_log_text->append(tr("== Connecting..."));
    auto port = static_cast<quint16>(m_port_linedit->text().toInt());
    m_tcp_socket->connectToHost(hostCombo->currentText(), port);
    auto result =
        QString("== Connected %1:%2.").arg(hostCombo->currentText()).arg(port);
    m_log_text->append(result);
#if LOGGER
    LOG(DEBUG, "display connection status:")
    qDebug() << result;
#endif
  }
}

void TcpClient::connectedToServer() {
#if LOGGER
  LOG(INFO, "update connection status: Connect to server.")
#endif
  m_log_text->append(tr("== Connected to server."));
  updateGui(QAbstractSocket::ConnectedState);
}

void TcpClient::on_disconnect_clicked() {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectingState) {
    m_log_text->append(tr("== Abort connecting."));
#if LOGGER
    LOG(INFO, "update connection status: abort connecting.")
#endif
  }
  m_tcp_socket->abort();
  updateGui(QAbstractSocket::UnconnectedState);
}

void TcpClient::disconnectByServer() {
#if LOGGER
  LOG(INFO, "update connection status: disconnected by server.")
  qDebug() << "connection state: " << m_tcp_socket->state();
#endif
  m_log_text->append(tr("== Disconnected by server."));
  updateGui(QAbstractSocket::UnconnectedState);
}

void TcpClient::updateGui(QAbstractSocket::SocketState state) {
  const bool connected = (state == QAbstractSocket::ConnectedState);
  const bool unconnected = (state == QAbstractSocket::UnconnectedState);
#if LOGGER
  LOG(DEBUG, "connection state %s", connected ? "true" : "false")
  LOG(DEBUG, "un-connection state %s", unconnected ? "true" : "false")
#endif
  // update ui
  connectButton->setEnabled(unconnected);
  hostCombo->setEnabled(unconnected);
  m_port_linedit->setEnabled(unconnected);
  m_user_linedit->setEnabled(unconnected);
  disconnectButton->setEnabled(!unconnected);
  m_log_text->setEnabled(connected);
}
