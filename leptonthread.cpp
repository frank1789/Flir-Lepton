#include "leptonthread.hpp"

LeptonThread::LeptonThread() : QThread(), m_image(m_camera) {
}

LeptonThread::~LeptonThread() {
    m_camera.stop();
}

void LeptonThread::run()
{
    m_camera.start();
}

