#include "findmodel.hpp"

#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

#include "logger.h"

FindModel::FindModel(QDialog* parent) : QDialog(parent) {
  this->setWindowFlags(Qt::Window | Qt::WindowTitleHint |
                       Qt::CustomizeWindowHint);
  this->setLayout(initializeLayout());
  connect(m_confirmbtn, &QPushButton::clicked, this, &FindModel::CloseOnClick);
  connect(m_load_labelbtn, &QPushButton::clicked, this,
          &FindModel::loadLabelMapFile);
  connect(m_load_modelbtn, &QPushButton::clicked, this,
          &FindModel::loadModelFile);
}

QString FindModel::getModelPath() const { return m_filename_model; }

QString FindModel::getLabelPath() const { return m_filename_label; }

////////////////////////////////////////////////////////////////////////////////
/// Slot
////////////////////////////////////////////////////////////////////////////////

void FindModel::loadModelFile() {
  m_filename_model = QFileDialog::getOpenFileName(
      this, tr("Open model"), QDir::homePath(), tr("Model Files(*.tflite)"));
  m_line_model->setText(m_filename_model);
}

void FindModel::loadLabelMapFile() {
  m_filename_label = QFileDialog::getOpenFileName(
      this, tr("Open Label map"), QDir::homePath(), tr("Label Files (*.txt)"));
  m_line_label->setText(m_filename_label);
}

void FindModel::CloseOnClick() {
  if (!m_filename_label.isEmpty() && !m_filename_model.isEmpty()) {
    this->close();
  } else {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("WARNING!\nSpecify model and map before continue.");
    msgBox.exec();
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Layout
////////////////////////////////////////////////////////////////////////////////

QGridLayout* FindModel::initializeLayout() {
  LOG(LevelAlert::D, "build layout FindModel")
  auto grid = new QGridLayout;
  auto model_label = new QLabel("Model path");
  auto map_label = new QLabel("Label map path");
  m_line_model = new QLineEdit;
  m_line_label = new QLineEdit;
  // disable line edit
  m_line_model->setReadOnly(true);
  m_line_label->setReadOnly(true);
  // set placeholder
  m_line_label->setPlaceholderText("path/to/label_map.txt");
  m_line_model->setPlaceholderText("path/to/model.tflite");
  // add button
  m_confirmbtn = new QPushButton("OK", this);
  m_load_modelbtn = new QPushButton("Open", this);
  m_load_labelbtn = new QPushButton("Open", this);
  // compose layout
  grid->addWidget(model_label, 0, 0);
  grid->addWidget(m_line_model, 0, 1);
  grid->addWidget(map_label, 1, 0);
  grid->addWidget(m_line_label, 1, 1);
  grid->addWidget(m_load_modelbtn, 0, 2);
  grid->addWidget(m_load_labelbtn, 1, 2);
  grid->addWidget(m_confirmbtn, 3, 2);
  return grid;
}
