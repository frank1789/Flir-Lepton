#ifndef LEPTONCAMERA_H
#define LEPTONCAMERA_H

#include <vector>
#include <cstdint>
#include <thread>
#include <atomic>
#include <mutex>
#include "leptonsensor.h"

/**
 * @brief Lepton parallel camera interface based on a grabber thread that
 *        ensures that all frames are read from the sensors
 */
class LeptonCamera {
public:

    /**
     * @brief Lepton camera constructor/destructor
     * @throw Runtime error when installed Lepton module is not recognized
     */
    LeptonCamera();
    // Delete copy constructor and copy operator
    LeptonCamera(LeptonCamera const&) = delete;
    virtual ~LeptonCamera();

    /**
     * @brief Start camera grabber thread
     */
    void start();

    /**
     * @brief Stop camera grabber thread
     */
    void stop();

    /**
     * @brief Sends an I2C command to the Lepton sensor
     * @param cmd     I2C known command, see LeptonCommon.h
     * @param buffer  Data buffer, some I2C commands return data (buffer should
     *                have proper size to fit all the returned data)
     * @return true, if I2C command succeed, false otherwise
     */
    bool sendCommand(LeptonI2CCmd cmd, void* buffer);

    /**
     * @brief Lepton frame accessors
     */
    inline bool hasFrame() const {return m_has_frame; }
    void getFrameU8(std::vector<uint8_t>& frame);
    void getFrameU16(std::vector<uint16_t>& frame);

    /**
     * @brief Lepton sensor specification accessors
     */
    inline double getTemperature() const { return m_temperature; }
    inline LeptonSensorType LeptonVersion() const { return m_lepton_type; }
    inline uint32_t width() const { return m_config.width; }
    inline uint32_t height() const { return m_config.height; }

private:
    /**
     * @brief Camera grabber (runs in a parallel thread)
     */
    void run();

    // Camera grabber thread
    std::thread grabber_thread_;
    std::atomic<bool> run_thread_;
    std::mutex lock_;

    // IR frame double buffer
    std::vector<uint16_t> m_frame_read;
    std::vector<uint16_t> m_frame_write;
    std::atomic<bool> m_has_frame;

    // Sensor info
    LeptonSensor m_sensor;
    LeptonSensorType m_lepton_type;
    LeptonCameraConfig m_config;
    double m_temperature;
};
