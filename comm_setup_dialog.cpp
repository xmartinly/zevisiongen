#include "comm_setup_dialog.h"
#include "ui_comm_setup_dialog.h"

CommSetupDialog::CommSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommSetupDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    C_serial = SerialCommSingleton::GetInstance();
    C_helper = new CommonHelper;
    connect(C_serial, &SerialCommSingleton::sendResponse, this, &CommSetupDialog::onRecvResponse );
    //Read communication config from .ini file.
    QMap<QString, QString> qm_commConfig = C_helper->readSection("./", S_fileName, S_section);
    if(qm_commConfig.count() == 3) {
        ui->baudrate_cb->setCurrentText(qm_commConfig["Baudrate"]);
        ui->port_cb->setCurrentText(qm_commConfig["Port"]);
        int i_protocol = qm_commConfig["Protocol"].toInt();
        setProtocol(i_protocol);
    }
}

CommSetupDialog::~CommSetupDialog() {
    delete ui;
}

///
/// \brief CommSetupDialog::on_set_btn_clicked
///
void CommSetupDialog::on_set_btn_clicked() {
    if(!C_serial->getConnectState()) {
        C_helper->normalErr(1, "Connection", "Connect instrument first.");
        return;
    }
    QString s_protocolConfig = QString("UG1305:%1;UG1304:%2")
                               .arg((int)ui->length_chkbox->isChecked())
                               .arg((int)ui->chk_chkbox->isChecked());
    C_serial->cmdEnQueue(
        C_helper->zevisonCommandGenAlpha(&s_protocolConfig, I_zevisionProtocol)
    );
}

///
/// \brief CommSetupDialog::on_close_btn_clicked
///
void CommSetupDialog::on_close_btn_clicked() {
    setCommConfig();
    this->close();
}

///
/// \brief CommSetupDialog::on_conn_btn_clicked
///
void CommSetupDialog::on_conn_btn_clicked() {
    if(C_serial->getConnectState()) {
        C_serial->disconnInst();
    }
    QString s_protocolConfig = QString("QG1304;QG1305;H1");
    QString s_port = ui->port_cb->currentText(),
            s_baudrate = ui->baudrate_cb->currentText();
    if(C_serial->connInst(s_port, s_baudrate)) {
        findInst(s_protocolConfig);
    }
}

///
/// \brief CommSetupDialog::closeEvent
/// \param event
///
void CommSetupDialog::closeEvent(QCloseEvent *event) {
    setCommConfig();
    emit configSetted();
    event->accept();
}

///
/// \brief CommSetupDialog::onRecvResponse
/// \param qm_resp
///
void CommSetupDialog::onRecvResponse(QVariantMap qm_resp) {
    QStringList sl_cmd = QString(qm_resp["cmd"].toString()).split("&#");
    QByteArray ba_response = qm_resp["data"].toByteArray();
    QVariantMap qvm_response = C_helper->zevisonRespCal(
                                   ba_response,
                                   sl_cmd.at(1).toInt()
                               );
    bool b_isMsgFailed = qvm_response["msg_failed"].toBool();
    if(b_isMsgFailed) {
        return;
    } else { //set current protocol
        setProtocol(sl_cmd.at(1).toInt());
    }
    QMap _resp = qvm_response["resp"].toMap();
    if(_resp.contains("H1")) { //show H1 information on label
        QMap __resp = _resp["H1"].toMap();
        ui->resp_lb->setText(__resp["resp_data"].toString() + "\n"
                             + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    if(_resp.contains("UG1305") || _resp.contains("UG1304")) { //send H2 commond to check the current protocol
        findInst("H2");
    }
}

///
/// \brief CommSetupDialog::setCommConfig
///
void CommSetupDialog::setCommConfig() {
    QString s_port = ui->port_cb->currentText(),
            s_baudrate = ui->baudrate_cb->currentText();
    QMap<QString, QString> qm_commSetup;
    qm_commSetup.insert("Port", s_port);
    qm_commSetup.insert("Baudrate", s_baudrate);
    qm_commSetup.insert("Protocol", QString::number(I_zevisionProtocol));
    C_helper->writeSettings(S_fileName, S_section, qm_commSetup);
}

///
/// \brief CommSetupDialog::setProtocol
/// \param protocol
///
void CommSetupDialog::setProtocol(const int protocol) {
    I_zevisionProtocol = protocol;
    QList ql_protocol = C_helper->zevisionProtocolCal(protocol);
    bool b_isLength = ql_protocol.at(0);
    bool b_isChksum = ql_protocol.at(1) ;
    ui->chk_chkbox->setChecked(b_isChksum);
    ui->length_chkbox->setChecked(b_isLength);
}

///
/// \brief CommSetupDialog::findInst. Send cmd to instrument using all protocol.
/// \param cmd. QString
///
void CommSetupDialog::findInst(const QString cmd) {
    QString s_cmd = cmd.toUpper();
    if(C_serial->getConnectState()) {
        for (int i = 0; i < 4 ; i++ ) {
            C_serial->cmdEnQueue(
                C_helper->zevisonCommandGenAlpha(&s_cmd, i)
            );
        }
    }
}

