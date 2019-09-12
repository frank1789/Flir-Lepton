#ifndef THERMALIMAGE_H
#define THERMALIMAGE_H

#include <QFile>
#include <QImage>
#include <QObject>
#include <QVector>
#include "leptoncamera.h"

class ThermalImage : public QObject {
  Q_OBJECT

 public:
  explicit ThermalImage() {}
  ThermalImage(const LeptonCamera &camera);
  ~ThermalImage();
  inline void setImage(const QVector<uint16_t> &imgU16);
  inline void setImage(const QVector<uint8_t> &imgU8);
  inline void setImage(const std::vector<uint8_t> &imgU8);
  inline void setImage(const std::vector<uint16_t> &imgU16);

  void save_raw_file();
  void save_pgm_file();

  void map_raw_data_to_rgb();

 public slots:
  void updateImageValue(const unsigned int &max, const unsigned int &min);

 signals:
  void updateImage(QImage);

 private:
  unsigned long m_index_image;

  // frame size
  uint32_t m_frame_width;
  uint32_t m_frame_height;

  unsigned int rawMin;
  unsigned int rawMax;

  // buffer
  QVector<uint8_t> rawdataU8;
  QVector<uint16_t> rawdataU16;

  // RGB image
  QImage m_rgbImage;
};

#endif  // THERMALIMAGE_H
