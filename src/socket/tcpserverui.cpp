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
  grid->addLayout(createInformationGroup(), 0, 0);
  grid->addWidget(createLogGroup(), 1, 0);
  setLayout(grid);
}

//////////////////////////////////////////////////////////////////////////////
//// Slot function
//////////////////////////////////////////////////////////////////////////////

void TCPServerUi::onClientConnect() {
  m_device_count++;
  m_device_label->setText(QString("%1").arg(m_device_count, 4));
}

void TCPServerUi::onClientDisconnect() {
  m_device_count--;
  m_device_label->setText(QString("%1").arg(m_device_count));
}

//////////////////////////////////////////////////////////////////////////////
//// Layout function
//////////////////////////////////////////////////////////////////////////////

QGridLayout *TCPServerUi::createInformationGroup() {
#if LOGGER_UI
  LOG(INFO, "build information group ui")
#endif
  // define group layout
  auto grid_layout = new QGridLayout;
  // init elements
  connection_label = new QLabel("Connection:");
  port_label = new QLabel("Port:");
  m_connection_address = new QLabel;
  m_port_number = new QLabel;
  // assemble element
  grid_layout->addWidget(connection_label, 0, 0);
  grid_layout->addWidget(m_connection_address, 0, 1);
  grid_layout->addWidget(port_label, 0, 2);
  grid_layout->addWidget(m_port_number, 0, 3);
  // return layout
  return grid_layout;
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
  auto status = new QLabel;
  m_device_label = new QLabel("0");

  // setup grid layout
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(incoming, 0, 0);
  gridLayout->addWidget(connected, 0, 1);
  gridLayout->addWidget(status, 1, 0);
  gridLayout->addWidget(m_device_label, 0, 2);
  groupBox->setLayout(gridLayout);
  return groupBox;
}
