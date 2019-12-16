#include "tcpclient.hpp"

#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkConfiguration>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QTextEdit>

#include "commonconnection.hpp"
#include "log/logger.h"

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent), m_tcp_socket(new QTcpSocket(this)) {
  // assemble ui
  connectButton = new QPushButton("Connect");
  disconnectButton = new QPushButton("Disconnect");
  connectButton->setEnabled(false);
  disconnectButton->setEnabled(false);
#if LOGGER_UI
  LOG(INFO, "dis/connect buttons are set disable")
#endif
  QGridLayout *grid = new QGridLayout;
  grid->addWidget(createInformationGroup(), 0, 0, 1, 3);
  grid->addWidget(connectButton, 1, 1);
  grid->addWidget(disconnectButton, 1, 2);
  grid->addWidget(createLogGroup(), 2, 0, 1, 3);
  setLayout(grid);
#if LOGGER_CLIENT
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
          [=]() { this->onDisconnectClicked(); });
  connect(connectButton, &QPushButton::clicked,
          [=]() { this->onConnectClicked(); });

  // connect ui
  connect(hostCombo, &QComboBox::editTextChanged, this,
          &TcpClient::enableConnectButton);
  connect(m_port_linedit, &QLineEdit::textChanged, this,
          &TcpClient::enableConnectButton);
  //  connect(m_tcp_socket, &QIODevice::readyRead, this,
  //  &TcpClient::readFortune);

  // connect error
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
#if LOGGER
  QTimer *timer = new QTimer(this);
  timer->setInterval(1000);
  timer->start();
#if TEST_IMAGE
  connect(timer, &QTimer::timeout, [=]() { this->sendImageMessage(); });
#else
  connect(timer, &QTimer::timeout, [=]() { this->sendTestMessageStream(); });
#endif
#endif
}

void TcpClient::sendImage(QImage image) {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
#if LOGGER_CLIENT
    LOG(WARN, "socket test function not connected, then exit.")
#endif
    return;
  }
  send_message_image(m_tcp_socket, image);
}

//////////////////////////////////////////////////////////////////////////////
//// Slot function
//////////////////////////////////////////////////////////////////////////////

void TcpClient::connectedToServer() {
#if LOGGER_CLIENT
  LOG(INFO, "update connection status: Connect to server.")
#endif
  m_log_text->append(tr("== Connected to server."));
  updateGui(QAbstractSocket::ConnectedState);
}

void TcpClient::onDisconnectClicked() {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectingState) {
    m_log_text->append(tr("== Abort connecting."));
#if LOGGER_CLIENT
    LOG(INFO, "update connection status: abort connecting.")
#endif
  }
  m_tcp_socket->abort();
  updateGui(QAbstractSocket::UnconnectedState);
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

void TcpClient::enableConnectButton() {
  connectButton->setEnabled((!networkSession || networkSession->isOpen()) &&
                            !hostCombo->currentText().isEmpty() &&
                            !m_port_linedit->text().isEmpty());
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

void TcpClient::readyRead() {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
#if LOGGER_CLIENT
    LOG(ERROR, "impossible read from socket, disconnected")
#endif
    return;
  }
  m_data.startTransaction();
  QString header{""};
  qint32 size{0};
  m_data >> header >> size;
#if LOGGER_CLIENT
  LOG(DEBUG,
      "incoming message control through header identification:\n"
      "\t* message containing text if the header corresponds to the code UTF-8 "
      "'\\u001D' or the ASCII code %d\n"
      "\t* message containing images if the header corresponds to the code "
      "UTF-8 '\\u001E' or the ASCII code %d\n",
      GROUP_SEPARATOR_ASCII_CODE, RECORD_SEPARATOR_ASCII_CODE)
  qDebug() << "\tincoming message header: " << header << "\tsize: " << size
           << "\n";
#endif
  if (header == QString(GROUP_SEPARATOR_ASCII_CODE)) {
    QString message;
    m_data >> message;
#if LOGGER_CLIENT
    LOG(DEBUG, "check message is not empty: %s",
        (!message.isEmpty()) ? "true" : "false")
    LOG(DEBUG, "server read message in redyRead()\n\tmessage received:")
    qDebug() << "\t" << message << "\n";
#endif
    if (!message.isEmpty()) m_log_text->append(message);
  } else if (header == QString(RECORD_SEPARATOR_ASCII_CODE)) {
#if LOGGER_CLIENT
    LOG(DEBUG, "image incoming")
#endif
    QImage image;
    m_data >> image;
#if LOGGER_CLIENT
    LOG(DEBUG, "check image is not empty: %s",
        (!image.isNull()) ? "true" : "false")
#endif
    if (!image.isNull()) emit updateImage(image);
  } else {
    m_data.abortTransaction();
    return;
  }
  if (!m_data.commitTransaction()) {
    return;
  }
}

void TcpClient::onConnectClicked() {
  if (m_user_linedit->text().isEmpty()) {
#if LOGGER_CLIENT
    LOG(ERROR, "unable to connect, must define user name.")
#endif
    m_log_text->append(
        tr("== Unable to connect to server.\nYou must define an user name."));
    return;
  }
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
    m_log_text->append(tr("== Connecting..."));
    auto port = static_cast<quint16>(m_port_linedit->text().toInt());
    m_tcp_socket->connectToHost(hostCombo->currentText(), port);
    auto result =
        QString("== Connected %1:%2.").arg(hostCombo->currentText()).arg(port);
    m_log_text->append(result);
#if LOGGER_CLIENT
    LOG(DEBUG, "try connect, display connection status:")
    qDebug() << "\t" << result;
#endif
  }
}

void TcpClient::disconnectByServer() {
#if LOGGER_CLIENT
  LOG(INFO, "update connection status: disconnected by server.")
  qDebug() << "\t"
           << "connection state: " << m_tcp_socket->state();
#endif
  m_log_text->append(tr("== Disconnected by server."));
  updateGui(QAbstractSocket::UnconnectedState);
}

//////////////////////////////////////////////////////////////////////////////
//// Layout function
//////////////////////////////////////////////////////////////////////////////

QGroupBox *TcpClient::createInformationGroup() {
#if LOGGER_CLIENT
  LOG(INFO, "build information group ui")
#endif
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
#if LOGGER_CLIENT
  LOG(DEBUG, "find out all IP address:")
  for (const auto &list : ipAddressesList) {
    qDebug() << "\t" << list.toString();
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
#if LOGGER_UI
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

void TcpClient::updateGui(QAbstractSocket::SocketState state) {
  const bool connected = (state == QAbstractSocket::ConnectedState);
  const bool unconnected = (state == QAbstractSocket::UnconnectedState);
#if LOGGER_UI
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

//////////////////////////////////////////////////////////////////////////////
//// Test function
//////////////////////////////////////////////////////////////////////////////

void TcpClient::sendTestMessageStream() {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
#if LOGGER_CLIENT
    LOG(WARN, "socket test function not connected, then exit.")
#endif
    return;
  }
  const QString test_message{"Test datastrem message, sended form code."};
  QString message =
      QString("%1: %2").arg(m_user_linedit->text()).arg(test_message);
  send_message_text(m_tcp_socket, message);
  //  m_log_text->append(message);
}

#if TEST_IMAGE

QImage randomImage() {
  qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

  QDir dir("/Users/francesco/Desktop/landingzone/CiterX");
  dir.setFilter(QDir::Files);
  QFileInfoList entries = dir.entryInfoList();

  if (entries.size() == 0) {
    qDebug("No images to show!");
    return QImage();
  }
  qDebug() << entries.at(qrand() % entries.size()).absoluteFilePath();
  return QImage(entries.at(qrand() % entries.size()).absoluteFilePath());
}

void TcpClient::sendImageMessage() {
  if (m_tcp_socket->state() != QAbstractSocket::ConnectedState) {
#if LOGGER_CLIENT
    LOG(WARN, "socket test function not connected, then exit.")
#endif
    return;
  }
  send_message_image(m_tcp_socket, randomImage());
}

#endif
