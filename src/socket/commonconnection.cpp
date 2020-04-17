#include "commonconnection.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QImage>
#include <QString>
#include <QTcpSocket>

#include "logger.h"

MessageType identifies_message_type(const QString &header, const qint32 &size) {
#if LOGGER
  LOG(LevelAlert::D,
      "incoming message control through header identification:\n"
      "\t* message containing text if the header corresponds to the code UTF-8 "
      "'\\u001D' or the ASCII code %d\n"
      "\t* message containing images if the header corresponds to the code "
      "UTF-8 '\\u001E' or the ASCII code %d",
      GROUP_SEPARATOR_ASCII_CODE, RECORD_SEPARATOR_ASCII_CODE)
  qDebug() << "\tincoming message header: " << header << "\tsize: " << size
           << "\n";
#endif

  if (header == QString(GROUP_SEPARATOR_ASCII_CODE)) {
    return MessageType::Text;
  }

  else if (header == QString(RECORD_SEPARATOR_ASCII_CODE)) {
    return MessageType::Image;
  }
  return MessageType::Unknow;
}

void send_message_text(QTcpSocket *socket, const QString &message) {
  // check image is not empty
  if (message.isNull()) {
#if LOGGER
    LOG(LevelAlert::E, "image is not valid: %s",
        message.isNull() ? "true" : "false")
#endif
    return;
  }
  // prepare datastream
  QByteArray ba_message;
  QDataStream out(&ba_message, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  // serialize information
  out << QString(GROUP_SEPARATOR_ASCII_CODE)
      << static_cast<quint32>(ba_message.size()) << message;
  socket->write(ba_message);
#if LOGGER
  LOG(LevelAlert::T, "sending text")
  qDebug() << "\theader: " << QString(GROUP_SEPARATOR_ASCII_CODE)
           << "\tsize: " << static_cast<quint32>(ba_message.size());
  qDebug() << "\t" << message << "\n";
#endif
}

void send_message_image(QTcpSocket *socket, const QImage &image) {
  // check image is not null
  if (image.isNull()) {
#if LOGGER
    LOG(LevelAlert::E, "image is not valid: %s",
        image.isNull() ? "true" : "false")
#endif
    return;
  }

  // prepare datastream and serialize information
  QByteArray ba_message;
  QDataStream out(&ba_message, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  out << QString(RECORD_SEPARATOR_ASCII_CODE)
      << static_cast<quint32>(image.sizeInBytes()) << image;
#if LOGGER
  LOG(LevelAlert::T, "serialized image")
  qDebug() << "\theader: " << QString(RECORD_SEPARATOR_ASCII_CODE)
           << "\tsize:" << static_cast<quint32>(image.sizeInBytes()) << "\n";
  qDebug() << ba_message;
#endif
  socket->write(ba_message);
}
