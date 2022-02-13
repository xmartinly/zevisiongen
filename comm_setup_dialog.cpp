#include "comm_setup_dialog.h"
#include "ui_comm_setup_dialog.h"

CommSetupDialog::CommSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommSetupDialog) {
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    C_serial = SerialCommSingleton::GetInstance();
    C_helper = new CommonHelper;
//    setWindowFlags(flags);
//    qDebug() << H1_withOutLenChksum.toHex() << H1_withOutChksum.toHex()
//             << H1_withOutLen.toHex() << H1_withLenChksum.toHex();
//             << (CommonHelper::ZevisionWithChecksum | CommonHelper::ZevisionWithLenght);
}

CommSetupDialog::~CommSetupDialog() {
    delete ui;
}

void CommSetupDialog::on_close_btn_clicked() {
    QString s_port = ui->port_cb->currentText(),
            s_baudrate = ui->baudrate_cb->currentText(),
            s_fileName = "zevision.ini",
            s_section =  "Communication";
    QMap<QString, QString> qm_commSetup;
    int i_protocol = ui->chk_chkbox->checkState() //If checked will return 2
                     + (int)ui->length_chkbox->isChecked();
    qm_commSetup.insert("Port", s_port);
    qm_commSetup.insert("Baudrate", s_baudrate);
    qm_commSetup.insert("Protocol", QString::number(i_protocol));
    C_helper->writeSettings(s_fileName, s_section, qm_commSetup);
    this->close();
}


void CommSetupDialog::on_conn_btn_clicked() {
    if(C_serial->getConnectState()) {
        C_serial->disconnInst();
    }
    QString s_port = ui->port_cb->currentText(),
            s_baudrate = ui->baudrate_cb->currentText();
//    if(C_serial->connInst(s_port, s_baudrate)) {
    for (int i = 0; i < 4 ; i++ ) {
        C_serial->cmdEnQueue(
            C_helper->ZevisonCommandGenAlpha(&S_hello, i)
        );
//        }
    }
}


void CommSetupDialog::closeEvent(QCloseEvent *event) {
//    if(QT_commTimer->isActive()) {
//        QT_commTimer->stop();
//    }
//    delete QT_commTimer;
//    delete C_helper;
//    delete C_sigmaTfc;
    event->accept();
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
