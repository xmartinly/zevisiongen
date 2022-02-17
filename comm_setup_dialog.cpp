﻿#include "comm_setup_dialog.h"
#include "ui_comm_setup_dialog.h"

CommSetupDialog::CommSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommSetupDialog) {
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    C_serial = SerialCommSingleton::GetInstance();
    C_helper = new CommonHelper;
    connect(C_serial, &SerialCommSingleton::sendResponse, this, &CommSetupDialog::onRecvResponse );
    QMap<QString, QString> qm_commConfig = C_helper->readSection("./", "zevision.ini", "Communication"); //Read communication config from .ini file.
    if(qm_commConfig.count() == 3) {
        ui->baudrate_cb->setCurrentText(qm_commConfig["Baudrate"]);
        ui->port_cb->setCurrentText(qm_commConfig["Port"]);
        int i_protocol = qm_commConfig["Protocol"].toInt();
        bool b_isLength = i_protocol % 2;
        bool b_isChksum = i_protocol > 1 ;
        ui->chk_chkbox->setChecked(b_isChksum);
        ui->length_chkbox->setChecked(b_isLength);
    }
}

CommSetupDialog::~CommSetupDialog() {
    delete ui;
}

void CommSetupDialog::setCommConfig() {
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
}

void CommSetupDialog::on_close_btn_clicked() {
    setCommConfig();
    this->close();
}


void CommSetupDialog::on_conn_btn_clicked() {
//    int i_test = 1;
//    int i_result = i_test << 1;
//    qDebug() << i_result;
    if(C_serial->getConnectState()) {
        C_serial->disconnInst();
    }
    QString s_port = ui->port_cb->currentText(),
            s_baudrate = ui->baudrate_cb->currentText();
    if(C_serial->connInst(s_port, s_baudrate)) {
        for (int i = 0; i < 4 ; i++ ) {
            C_serial->cmdEnQueue(
                C_helper->zevisonCommandGenAlpha(&S_hello, i)
            );
        }
    }
}


void CommSetupDialog::closeEvent(QCloseEvent *event) {
    setCommConfig();
    emit configSetted();
    event->accept();
}

void CommSetupDialog::onRecvResponse(QVariantMap qm_resp) {
    QByteArray ba_response = qm_resp["data"].toByteArray();
    QVariantMap qvm_response = C_helper->zevisonMessageCal(ba_response, CommonHelper::ZevisionOptional);
    qDebug() << qvm_response << qm_resp["cmd"].toByteArray().right(1);
}

void CommSetupDialog::on_set_btn_clicked() {
    QString s_protocolConfig = QString("UG1305:%1;UG1304:%2").arg((int)ui->length_chkbox->isChecked()).arg((int)ui->chk_chkbox->isChecked());
//    qDebug() << s_protocolConfig;
    if(!C_serial->getConnectState()) {
        C_helper->normalErr(1, "Connection", "Connect instrument first.");
        return;
    }
    for (int i = 0; i < 4 ; i++ ) {
        C_serial->cmdEnQueue(
            C_helper->zevisonCommandGenAlpha(&s_protocolConfig, i)
        );
    }
}

