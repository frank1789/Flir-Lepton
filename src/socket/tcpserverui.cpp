#include "tcpserverui.hpp"

#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QList>
#include <QString>
#include <QTextEdit>
#include <QtNetwork>

#include "../log/logger.h"
#include "commonconnection.hpp"

TCPServerUi::TCPServerUi(QWidget *parent) : QWidget(parent) {
#if LOGGER_UI
  LOG(INFO, "ctor TCPServerUi initialize ui")
  LOG(INFO, "dis/connect buttons are set disable")
#endif
  m_device_count = 0;
  // assemble ui, layout
  QGridLayout *grid = new QGridLayout;
  grid->addWidget(createInformationGroup(), 0, 0);
  grid->addWidget(createLogGroup(), 1, 0);
  setLayout(grid);
}

QString TCPServerUi::findIpAddress() {
  QString ipAddress{""};
  QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
  // use the first non-localhost IPv4 address
  for (const auto &address : ipAddressesList) {
    if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
      ipAddress = address.toString();
      break;
    }
  }
  // if we did not find one, use IPv4 localhost
  if (ipAddress.isEmpty())
    ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
#if LOGGER_UI
  LOG(DEBUG, "server use IP")
  qDebug() << "\t" << ipAddress;
#endif
  return ipAddress;
}

//////////////////////////////////////////////////////////////////////////////
//// Slot function
//////////////////////////////////////////////////////////////////////////////

void TCPServerUi::onClientConnect() {
  m_device_count++;
  m_device_label->setText(QString("%1").arg(m_device_count, 4));
  m_status_label->setText("streaming");
}

void TCPServerUi::onClientDisconnect() {
  m_device_count--;
  m_device_label->setText(QString("%1").arg(m_device_count));
  if (m_device_count == 0) {
    m_status_label->setText("ready");
  }
}

//////////////////////////////////////////////////////////////////////////////
//// Layout function
//////////////////////////////////////////////////////////////////////////////

QGroupBox *TCPServerUi::createInformationGroup() {
#if LOGGER_UI
  LOG(INFO, "build information group ui")
#endif
  // initialize grid layout
  auto grid_layout = new QGridLayout;
  // initialize group
  QGroupBox *groupBoxServer = new QGroupBox(tr("Server"));
  // initialize label
  connection_label = new QLabel("Address:");
  port_label = new QLabel("Port:");
  m_connection_address = new QLabel(findIpAddress());
  m_port_number = new QLabel(QString("%1").arg(TCP_PORT, 6));
  // assemble element
  grid_layout->addWidget(connection_label, 0, 0);
  grid_layout->addWidget(m_connection_address, 0, 1);
  grid_layout->addWidget(port_label, 1, 0);
  grid_layout->addWidget(m_port_number, 1, 1);
  groupBoxServer->setLayout(grid_layout);
  // return layout
  return groupBoxServer;
}

QGroupBox *TCPServerUi::createLogGroup() {
#if LOGGER_UI
  LOG(INFO, "build logging group ui")
#endif
  // defining layout group
  QGroupBox *groupBox = new QGroupBox(tr("Information:"));
  // init element ui
  const auto incoming = new QLabel("status:");
  const auto connected = new QLabel("connected devices:");
  m_status_label = new QLabel("ready");
  m_device_label = new QLabel(QString("%1").arg(m_device_count, 4));
  // setup grid layout
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(incoming, 0, 0);
  gridLayout->addWidget(connected, 1, 0);
  gridLayout->addWidget(m_status_label, 0, 1);
  gridLayout->addWidget(m_device_label, 1, 1);
  groupBox->setLayout(gridLayout);
  return groupBox;
}
