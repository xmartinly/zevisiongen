#include "comm_setup_dialog.h"
#include "ui_comm_setup_dialog.h"

CommSetupDialog::CommSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommSetupDialog) {
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
}

CommSetupDialog::~CommSetupDialog() {
    delete ui;
}

void CommSetupDialog::on_close_btn_clicked() {
}


void CommSetupDialog::on_conn_btn_clicked() {
}


void CommSetupDialog::onRecvResponse(QVariantMap qm_resp) {
//    QString s_cmd = qm_resp["cmd"].toString();
//    QByteArray ba_data = qm_resp["data"].toByteArray();
//    QMap<QString, QVariant> qm_data = C_sigmaTfc->sigmaResponseCal(ba_data, s_cmd);
//    if(qm_resp.size() < 2) {
//        return;
//    }
    qDebug() << qm_resp;
}
