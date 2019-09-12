#ifndef THERMALIMAGE_H
#define THERMALIMAGE_H

#include <QFile>
#include <QImage>
#include <QVector>
#include <QObject>
#include "leptoncamera.h"

class ThermalImage : public QObject {
  Q_OBJECT

 public:
  ThermalImage(const LeptonCamera &camera);
  ~ThermalImage();

  void setImage(const std::vector<uint8_t> &imgU8);
  void setImage(const std::vector<uint16_t> &imgU16);

  void save_raw_file();
  void save_pgm_file();



  void map_raw_data_to_rgb();
public slots:
    void updateImageValue(uint16_t max, uint16_t min);

 private:
  unsigned long m_index_image;

  // frame size
  uint32_t m_frame_width;
  uint32_t m_frame_height;

  uint16_t rawMin;
  uint16_t rawMax;

  // buffer
  QVector<uint8_t> rawdataU8;
  QVector<uint16_t> rawdataU16;

  // RGB image
  QImage rgbImage;
};

#endif  // THERMALIMAGE_H
