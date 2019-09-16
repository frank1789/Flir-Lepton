#include "camerathread.hpp"

#include <fstream>
#include <iostream>

#include <QDebug>
#include <QApplication>

#include <unistd.h>



CameraThread::CameraThread() : QThread(), cameraRunning(true)
{
    qRegisterMetaType<QImage>("QImage&");
    m_buffer = new unsigned char [camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB)]();
    // Open the camera
    if (!camera.open()) {
        std::cerr << "Unable to open camera";
        cameraRunning = false;
        throw std::runtime_error("Starting camera failed");
    } else {
        cameraRunning = true;
    }
    // Wait for the camera
    usleep(RaspicamLoadTime);
}

CameraThread::~CameraThread()
{
    cameraRunning = false;
    camera.release();
    delete [] m_buffer;
}

void CameraThread::run()
{
    // While the camera is on (the user has clicked the button), capture
    while (cameraRunning) {
        // Capture
        camera.grab();
        camera.retrieve(m_buffer, raspicam::RASPICAM_FORMAT_RGB);

        // Convert the data and send to the caller to handle
        QImage image = QImage(m_buffer, camera.getWidth(), camera.getHeight(), QImage::Format_RGB888);
        emit updateImage(image);

        // Make the app process stopWork() if necessary
        // qApp->processEvents();
        usleep(RaspicamResetTime);
    }
}
