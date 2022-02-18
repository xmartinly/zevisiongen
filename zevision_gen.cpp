#include "zevision_gen.h"
#include "ui_zevision_gen.h"

ZevisionGen::ZevisionGen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ZevisionGen) {
    ui->setupUi(this);
    C_serial = SerialCommSingleton::GetInstance();
    connect(C_serial, &SerialCommSingleton::sendResponse, this, &ZevisionGen::onRecvResponse);
    L_statStr = new QLabel("Inst Offline." + S_version);
    L_statStr->setAlignment(Qt::AlignRight);
    statusBar()->setSizeGripEnabled(false);
    statusBar()->addPermanentWidget(L_statStr, 1);
    QT_statTimer = new QTimer( this );
    QT_acquireTimer = new QTimer( this );
    connect( QT_statTimer, SIGNAL(timeout()), this, SLOT(onInstConnectState()));
    connect( QT_acquireTimer, SIGNAL(timeout()), this, SLOT(onSendCommand()));
    onReadCommConfig();
    initializeTbl();
}

ZevisionGen::~ZevisionGen() {
    delete ui;
}

void ZevisionGen::initializeTbl() {
    ui->msg_tb->clear();
    ui->msg_tb->setColumnCount(2);
    ui->resp_tb->clear();
    ui->resp_tb->setColumnCount(2);
    QStringList tblHeader;
    tblHeader << tr("Item") << tr("Value");
    ui->msg_tb->setHorizontalHeaderLabels(tblHeader);
    ui->msg_tb->verticalHeader()->setVisible(false);
    ui->msg_tb->horizontalHeader()->resizeSection(0, 90);
    ui->msg_tb->horizontalHeader()->resizeSection(1, 85);
    ui->msg_tb->horizontalHeader()->setStretchLastSection(true);
    ui->msg_tb->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->msg_tb->setRowCount(9);
    ui->resp_tb->setHorizontalHeaderLabels(tblHeader);
    ui->resp_tb->verticalHeader()->setVisible(false);
    ui->resp_tb->horizontalHeader()->resizeSection(0, 90);
    ui->resp_tb->horizontalHeader()->resizeSection(1, 85);
    ui->resp_tb->horizontalHeader()->setStretchLastSection(true);
    ui->resp_tb->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resp_tb->setRowCount(9);
    for(int i = 0; i < 9; i++) {
        QString s_head = QSL_tableHead.at(i);
        ui->msg_tb->setItem(i, 0, new QTableWidgetItem(s_head));
        ui->resp_tb->setItem(i, 0, new QTableWidgetItem(s_head));
        ui->msg_tb->setItem(i, 1, new QTableWidgetItem("n/a"));
        ui->resp_tb->setItem(i, 1, new QTableWidgetItem("n/a"));
    }
//    ui->msg_tb->setItem(1, 1, new QTableWidgetItem("{"));
//    ui->msg_tb->setItem(2, 7, new QTableWidgetItem("}"));
//    ui->resp_tb->setItem(1, 1, new QTableWidgetItem("{"));
    //    ui->resp_tb->setItem(2, 7, new QTableWidgetItem("}"));
}

void ZevisionGen::setTblData(const QStringList sl_data, const QTableWidget *tbl) {
    int i_slLength = sl_data.length();
    if(i_slLength != 9) {
        return;
    }
    for (int i = 0; i < i_slLength ; i++ ) {
        tbl->item(i, 1)->setText(sl_data.at(i));
    }
}


void ZevisionGen::on_actionDataLogSetting_triggered() {
}


void ZevisionGen::on_actionExit_triggered() {
}


void ZevisionGen::on_actionCommSettings_triggered() {
    D_commSet = new CommSetupDialog;
    connect(D_commSet, &CommSetupDialog::configSetted, this, &ZevisionGen::onReadCommConfig);
    D_commSet->exec();
}


void ZevisionGen::on_actionToggle_ENG_CHS_triggered() {
}


void ZevisionGen::on_actionHelp_triggered() {
}


void ZevisionGen::on_actionDocument_triggered() {
}

void ZevisionGen::onInstConnectState() {
    B_isInstConnected = C_serial->getConnectState();
    L_statStr->setText((B_isInstConnected ? "Port open." : "Port close.") + S_version);
    if(!B_isInstConnected) {
        C_serial->connInst(S_port, S_baudrate);
    } else {
        QString s_hello  = "H1";
        C_serial->cmdEnQueue(
            C_helper->zevisonCommandGenAlpha(&s_hello, I_zevisionProtocol)
        );
        if(QT_statTimer->isActive()) {
            QT_statTimer->stop();
        }
    }
    if(QM_commConfig.count() != 3) {
        C_helper->normalErr(1,
                            "Comm Config Error",
                            "No communication config found."
                           );
        if(QT_statTimer->isActive()) {
            QT_statTimer->stop();
        }
        return;
    }
    if(I_connectInstTryCount > 10) {
        C_helper->normalErr(1,
                            "Communication Error",
                            "Can't open port."
                           );
        if(QT_statTimer->isActive()) {
            QT_statTimer->stop();
        }
//        D_commSet->exec();
        return;
    }
    I_connectInstTryCount++;
}

void ZevisionGen::onReadCommConfig() {
    QM_commConfig = C_helper->readSection("./", "zevision.ini", "Communication");
    S_port = QM_commConfig["Port"];
    S_baudrate = QM_commConfig["Baudrate"];
    I_zevisionProtocol = QM_commConfig["Protocol"].toInt();
    if(C_serial->getConnectState()) {
        return;
    }
    if(!QT_statTimer->isActive()) {
        QT_statTimer->start(1000);
    }
}

void ZevisionGen::onSendCommand() {
    bool b_isConnected = C_serial->getConnectState();
    QString s_currentTime = QDateTime::currentDateTime().toString("hh:mm:ss.z");
    QMap<QString, QByteArray> qm_cmd = C_helper->zevisonCommandGenAlpha(&S_command, I_zevisionProtocol);
    QStringList sl_msg = C_helper->zevisionMsgtoList(qm_cmd["cmd_treated"], I_zevisionProtocol);
    setTblData(sl_msg, ui->msg_tb);
    if(b_isConnected) {
        C_serial->cmdEnQueue(
            qm_cmd
        );
    } else if(!QT_statTimer->isActive()) {
        QT_statTimer->start(1000);
    }
}


void ZevisionGen::onRecvResponse(QVariantMap qm_data) {
    QByteArray ba_resp = qm_data["data"].toByteArray();
    QStringList sl_msg = C_helper->zevisionMsgtoList(ba_resp, I_zevisionProtocol);
    setTblData(sl_msg, ui->resp_tb);
}



void ZevisionGen::on_send_btn_clicked() {
    onReadCommConfig();
    S_command = ui->cmd_le->text().toUpper();
    bool b_isAcquireTimerActive = QT_acquireTimer->isActive();
    ui->send_btn->setText(b_isAcquireTimerActive ? "Start" : "Stop");
    ui->cmd_le->setEnabled(b_isAcquireTimerActive);
    if(b_isAcquireTimerActive) {
        QT_acquireTimer->stop();
        return;
    }
    QT_acquireTimer->start(1000);
}

