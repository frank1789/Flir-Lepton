#ifndef IMAGE_COMPOSER_THREAD_HPP
#define IMAGE_COMPOSER_THREAD_HPP


#include <QThread>

class ImageComposer : public QThread {

    Q_OBJECT

public:
    ImageComposer(QWidget *parent=nullptr);
    ~ImageComposer();
    void recalculate_result();
    void run() override;


signals:
    void updateImage(QImage);


public slots:
    void set_thermal_image(QImage img);
    void set_rgb_image(QImage img);

private:
    QImage *m_source;
    QImage *m_destination;
    QImage *m_result;
};

#endif // IMAGE_COMPOSER_THREAD_HPP
