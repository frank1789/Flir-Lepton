#ifndef LEPTONTHREAD_HPP
#define LEPTONTHREAD_HPP

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
};

#endif // LEPTONTHREAD_HPP
