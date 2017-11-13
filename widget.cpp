#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QFileDialog>

#include "ncs.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
}

Widget::~Widget() { delete ui; }

void Widget::setNCS(Annotator::Plugins::NCS *ncs) { this->ncs = ncs; }

void Widget::on_graphButton_clicked() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select Graph File"),
                                                  ".", tr("All (*.*)"));
  if (QFile::exists(fileName)) {
    this->ui->graphLineEdit->setText(fileName);
    ncs->setGraph(fileName.toStdString());
  }
}

void Widget::on_labelsButton_clicked() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select Label File"),
                                                  ".", tr("All (*.*)"));
  if (QFile::exists(fileName)) {
    this->ui->labelLineEdit->setText(fileName);
    ncs->setLabelmap(fileName.toStdString());
  }
}
