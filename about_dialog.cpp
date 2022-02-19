#include "about_dialog.h"
#include "ui_about_dialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

AboutDialog::~AboutDialog() {
    delete ui;
}

void AboutDialog::on_close_btn_clicked() {
    this->close();
}

