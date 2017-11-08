#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QFileDialog>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
}

Widget::~Widget() { delete ui; }

void Widget::on_graphButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Select Graph File"), ".", tr("All (*.*)"));
    if (QFile::exists(fileName)) {
        this->ui->graphLineEdit->setText(fileName);
    }
}
