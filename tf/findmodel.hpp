#ifndef FINDMODEL_HPP
#define FINDMODEL_HPP

#include <QDialog>
#include <QObject>

QT_BEGIN_NAMESPACE
class QDialog;
class QLineEdit;
class QString;
class QGridLayout;
QT_END_NAMESPACE

class FindModel : public QDialog {
  Q_OBJECT
 public:
  explicit FindModel(QDialog *parent = nullptr);
  ~FindModel() = default;
  QString getModelPath() const;
  QString getLabelPath() const;

 private slots:
  void loadModelFile();
  void loadLabelMapFile();
  void CloseOnClick();

 signals:

 private:
  // methods
  QGridLayout *initializeLayout();
  // attributes
  QLineEdit *m_line_model{nullptr};
  QLineEdit *m_line_label{nullptr};
  QPushButton *m_confirmbtn{nullptr};
  QPushButton *m_load_modelbtn{nullptr};
  QPushButton *m_load_labelbtn{nullptr};
  QString m_filename_model{""};
  QString m_filename_label{""};
};

#endif  // FINDMODEL_HPP
