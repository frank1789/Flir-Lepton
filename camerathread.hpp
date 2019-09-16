#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <QImage>
#include <QThread>

#include <raspicam/raspicam.h>

// This makes it so we don't have to prefix everything with raspicam::
// So raspicam::RaspiCam becomes just RaspiCam



constexpr unsigned int RaspicamLoadTime{3000000};     // 3 s = 3000 ms = 3e+6 us
constexpr unsigned int RaspicamResetTime{200};     // 0.0002 s = 0.2 ms = 200 us

/*!
 * \brief Class to run the camera and get data from it.
 *
 * This class is responsible for grabbing data from the Raspberry Pi camera
 * using the [raspicam library](https://github.com/cedricve/raspicam)
 * and converting it to a QImage so it can be displayed by MainWindow.
 */
class CameraThread : public QThread
{
    Q_OBJECT
public:
    /*!
     * \brief CameraWorker constructor.
     *
     * Registers QImage as a metatype for this class and initializes data
     * to be the correct size for RASPICAM_FORMAT_RGB image format.
     */
    CameraThread();

    //! CameraWorker destructor.
    ~CameraThread();

    void run();

private:
    /*!
     * \brief Raspberry Pi camera.
     *
     * Access the Raspberry Pi camera using the [raspicam library]
     * (https://github.com/cedricve/raspicam).
     */
    raspicam::RaspiCam camera;

    /*!
     * \brief whether or not the camera is running.
     *
     * Flag for the camera's state so that the program doesn't try to start
     * the camera when it's already running.
     */
    bool cameraRunning;

    /*!
     * \brief data grabbed from the camera.
     *
     * The raw data grabbed from the camera, before it is converted to a
     * QImage.
     */
    unsigned char *m_buffer;

signals:
    /*!
     * \brief emits the image from the camera.
     *
     * This signal is emitted by the doWork function when it grabs an image
     * from the camera and converts it to a QImage. Classes that use this
     * worker connect a slot to this signal; for example, see how this signal
     * is connected to a slot of the same name in
     * MainWindow::on_btnStart_clicked().
     *
     * \param image the image that is emitted and handled by connected slots.
     */
    void updateImage(QImage &image);

    /*!
     * \brief emitted when the worker is done.
     *
     * This signal is emitted when this worker is done. Classes that use this
     * worker connect a slot to this signal; for example, see how this signal
     * is connected to the MainWindow::cameraFinished slot in
     * MainWindow::on_btnStart_clicked().
     */
    void finished();

public slots:
    /*!
     * \brief start the camera begin grabbing frames.
     *
     * First, grabs a frame from the camera in RGB format (RASPICAM_FORMAT_RGB).
     * Next, converts the raw RGB image to a QImage and emits it. Finally,
     * Finally, force processing of events in this thread with
     * QApplication::proccessEvents() so that stopWork() is called if necessary.
     */
    void doWork();

    /*!
     * \brief stops this worker.
     *
     * Sets the cameraRunning flag to false.
     */
    void stopWork();
};

#endif // CAMERAWORKER_H
