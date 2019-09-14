#include "SPI.h"
#include <fcntl.h>
#include <getopt.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <string>

//============================================================================
// Lepton SPI Communication
//============================================================================

// SPI config
int spi_cs0_fd{-1};
int spi_cs1_fd{-1};
unsigned char spi_mode{SPI_MODE_3};
unsigned char spi_bits_per_word{8};
unsigned int spi_speed{10000000};  // 1000000 = 1MHz (1uS per bit)

// Open SPI port
int leptonSPI_OpenPort(int spi_device) {
  int status_value{-1};
  int *spi_cs_fd{nullptr};
  if (spi_device) {
    spi_cs_fd = &spi_cs1_fd;
  } else {
    spi_cs_fd = &spi_cs0_fd;
  }

  // Select SPI device and open communication
  if (spi_device) {
    *spi_cs_fd = open(std::string("/dev/spidev0.1").c_str(), O_RDWR);
  } else {
    *spi_cs_fd = open(std::string("/dev/spidev0.0").c_str(), O_RDWR);
  }

  if (*spi_cs_fd < 0) {
    std::cerr << "Error - Could not open SPI device" << std::endl;
    throw std::runtime_error("Connection failed.");
  }
  // Set SPI mode WR
  // SPI_MODE_0 (0,0) CPOL=0 (Clock Idle low level),  CPHA=0 (SDO transmit/change edge active to idle)
  // SPI_MODE_1 (0,1) CPOL=0 (Clock Idle low level),  CPHA=1 (SDO transmit/change edge idle to active)
  // SPI_MODE_2 (1,0) CPOL=1 (Clock Idle high level), CPHA=0 (SDO transmit/change edge active to idle)
  // SPI_MODE_3 (1,1) CPOL=1 (Clock Idle high level), CPHA=1
  // (SDO transmit/change edge idle to active)
  status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);
  if (status_value < 0) {
    std::cerr << "Could not set SPIMode (WR)...ioctl fail" << std::endl;
    throw std::runtime_error("SPI config failed.");
  }

  // Set SPI Mode RD
  status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);
  if (status_value < 0) {
    std::cerr << "Could not set SPIMode (RD)...ioctl fail" << std::endl;
    throw std::runtime_error("SPI config failed.");
  }

  // Set SPI bits per word WR
  status_value =
      ioctl(*spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits_per_word);
  if (status_value < 0) {
    std::cerr << "Could not set SPI bitsPerWord (WR)...ioctl fail" << std::endl;
    throw std::runtime_error("SPI config failed.");
  }

  // Set SPI bits per word Rd
  status_value =
      ioctl(*spi_cs_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits_per_word);
  if (status_value < 0) {
    std::cerr << "Could not set SPI bitsPerWord(RD)...ioctl fail" << std::endl;
    throw std::runtime_error("SPI config failed.");
  }

  // Set SPI bus speed WR
  status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
  if (status_value < 0) {
    std::cerr << "Could not set SPI speed (WR)...ioctl fail" << std::endl;
    throw std::runtime_error("SPI config failed.");
  }

  // Set SPI bus speed RD
  status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
  if (status_value < 0) {
    std::cerr << "Could not set SPI speed (RD)...ioctl fail" << std::endl;
    throw std::runtime_error("SPI config failed.");
  }

  std::cout << "Open SPI port: " << spi_device << ", with address "
            << *spi_cs_fd << std::endl;
  return (status_value);
}

// Close SPI connection
int leptonSPI_ClosePort(int spi_device) {
  int status_value{-1};
  int *spi_cs_fd{nullptr};
  if (spi_device) {
    spi_cs_fd = &spi_cs1_fd;
  } else {
    spi_cs_fd = &spi_cs0_fd;
  }

  // Close connection
  status_value = close(*spi_cs_fd);
  if (status_value < 0) {
    std::cerr << "Error - Could not close SPI device" << std::endl;
    throw std::runtime_error("Closing connection failed.");
  }
  std::cout << "Close SPI port: " << spi_device << ", with address "
            << *spi_cs_fd << std::endl;
  return (status_value);
}
