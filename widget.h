#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Annotator {
namespace Plugins {
class NCS;
}
}

namespace Ui {
class Widget;
}

class Widget : public QWidget {
  Q_OBJECT

 public:
  explicit Widget(QWidget *parent = 0);
  ~Widget();
  void setNCS(Annotator::Plugins::NCS *ncs);

 signals:
  void labelFileChanged(QString);
  void graphFileChanged(QString);

 private slots:

  void on_graphButton_clicked();

  void on_labelsButton_clicked();

  void on_alexNetRadioButton_toggled(bool checked);

  void on_googleNetRadioButton_toggled(bool checked);

  void on_squeezeNetRadioButton_toggled(bool checked);

 private:
  Ui::Widget *ui;
  Annotator::Plugins::NCS *ncs;
};

#endif  // WIDGET_H
