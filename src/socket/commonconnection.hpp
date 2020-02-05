#ifndef COMMONCONNECTION_HPP
#define COMMONCONNECTION_HPP

#include <QByteArray>

#include <QImage>
#include <QString>
#include <QTcpSocket>

constexpr int TCP_PORT{52693};
constexpr int TERMINATION_ASCII_CODE{23};
constexpr int RECORD_SEPARATOR_ASCII_CODE{30};
constexpr int GROUP_SEPARATOR_ASCII_CODE{29};

/**
 * \file commonconnection.hpp
 * @enum MessageType
 *
 * @brief The MessageType is a strongly typed enum class representing the type
 * of message.
 * 
 */
enum class MessageType : quint8 {
  Text,   /**< is coded as quint8 of value 0 */
  Image,  /**< is coded as quint8 of value 1 */
  Unknow, /**< is coded as quint8 of value 2 */
};

/**
 * @brief identifies_message_type allows to identify the message from the
 * header.
 *
 * Once the QDatastream is obtained from the socket and the transition is 
 * started, the header and dimensions, which represent the input parameters, are
 * extracted.
 * Then the checking of incoming messages by identifying the header:
 * - message containing text if the header corresponds to the UTF-8 code
 * '\u001D' or to the ASCII code 29.
 * - message containing images if the header corresponds to the UTF-8 code
 * '\u001E' or to the ASCII code 30.
 *
 * @param[in] header string containing the message header.
 * @param[in] size message size expressed in bytes.
 * @return a message type as indicated by ::MessageType.
 */
MessageType identifies_message_type(const QString &header, const qint32 &size);

/**
 * @brief send_message_text send the message.
 *
 * The function checks that the message is valid otherwise it returns.
 * If the message to be sent is valid, the buffer and the object containing the
 * stream are constructed. the message is composed of the header, then the size,
 * the text of the message.
 *
 * @param[in] socket tcp-socket required to send the buffer.
 * @param[in] message string containing the text message.
 */
void send_message_text(QTcpSocket *socket, const QString &message);

/**
 * @brief send_message_image send the message.
 *
 * The function checks that the message is valid otherwise it returns.
 * If the message to be sent is valid, the buffer and the object containing the
 * stream are constructed. the message is composed of the header, then the size,
 * the image.
 *
 * @param[in] socket tcp-socket required to send the buffer.
 * @param[in] image containing the picture.
 */
void send_message_image(QTcpSocket *socket, const QImage &image);

#endif  // COMMONCONNECTION_HPP
