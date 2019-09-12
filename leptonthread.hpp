#ifndef LEPTONTHREAD_HPP
#define LEPTONTHREAD_HPP

#include <cstdint>
#include <QVector>
#include <QObject>
#include <QThread>
#include "leptoncamera.h"
#include "thermalimage.h"

class LeptonThread : public QThread
{
    Q_OBJECT
public:
    LeptonThread();
    ~LeptonThread();

    void run();

signals:
    void updateImage(QImage);

private:
    LeptonCamera m_camera;
    ThermalImage m_image;
    QVector<uint8_t> *m_frameU8;
};

#endif // LEPTONTHREAD_HPP
