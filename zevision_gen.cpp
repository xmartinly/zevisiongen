﻿#include "zevision_gen.h"
#include "ui_zevision_gen.h"

ZevisionGen::ZevisionGen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ZevisionGen) {
    ui->setupUi(this);
    S_fileName = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
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
    onReadConfig();
    initializeTbl();
    m_translator = new QTranslator(qApp);
    m_translator->load(":/i18n/ZevisionGen_en_US.qm");
    qApp->installTranslator(m_translator);
}

ZevisionGen::~ZevisionGen() {
    delete ui;
}

///
/// \brief ZevisionGen::on_actionToggle_CHS_ENG_triggered
///
void ZevisionGen::on_actionToggle_CHS_ENG_triggered() {
    B_isUsingTrans = !B_isUsingTrans;
    QString s_transFileName;
    s_transFileName = B_isUsingTrans ? "ZevisionGen_zh_CN.qm" :  "ZevisionGen_en_US.qm";
    m_translator->load(":/i18n/" + s_transFileName);
    ui->retranslateUi(this);
    refreshTblWidget(B_isUsingTrans);
}


///
/// \brief ZevisionGen::on_send_btn_clicked. SYSTEM private SLOT.
///
void ZevisionGen::on_send_btn_clicked() {
    onReadConfig();
    S_command = ui->cmd_le->text().toUpper();
    bool B_isCmdEmpty = S_command.length() == 0;
    bool b_isAcquireTimerActive = QT_acquireTimer->isActive();
    if(B_isCmdEmpty) {
        C_helper->normalErr(1,
                            tr("Command error"),
                            tr("Command can not be empty.")
                           );
        return;
    }
    ui->send_btn->setText(b_isAcquireTimerActive ? tr("Start") : tr("Stop"));
    ui->cmd_le->setEnabled(b_isAcquireTimerActive);
    if(b_isAcquireTimerActive) {
        QT_acquireTimer->stop();
        return;
    }
    QT_acquireTimer->start(I_collectIntvl);
}

///
/// \brief ZevisionGen::on_actionDataLogSetting_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionDataLogSetting_triggered() {
    D_dataLog = new DataLogDialog;
    D_dataLog->exec();
}

///
/// \brief ZevisionGen::on_actionExit_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionExit_triggered() {
}

///
/// \brief ZevisionGen::on_actionCommSettings_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionCommSettings_triggered() {
    D_commSet = new CommSetupDialog;
    connect(D_commSet, &CommSetupDialog::configSetted, this, &ZevisionGen::onReadConfig);
    D_commSet->exec();
}

/////
///// \brief ZevisionGen::on_actionToggle_ENG_CHS_triggered. SYSTEM private SLOT.
/////
/// deprecated
//void ZevisionGen::on_actionToggle_ENG_CHS_triggered() {
//    B_isUsingTrans = !B_isUsingTrans;
//    QTranslator translator;
//    if(B_isUsingTrans && translator.load(":/i18n/ZevisionGen_zh_CN.qm")) {
//        qApp->installTranslator(&translator);
//    } else {
//        qApp->removeTranslator(&translator);
//    }
//    ui->retranslateUi(this);
//}

///
/// \brief ZevisionGen::on_actionHelp_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionHelp_triggered() {
    D_about = new AboutDialog;
    D_about->exec();
}

///
/// \brief ZevisionGen::on_actionDocument_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionDocument_triggered() {
    C_helper->openFile("./074-785-P1A IMC-300 Remote Communication Quick Start Guide PRELIMINARY 020822.pdf");
}

///
/// \brief ZevisionGen::onInstConnectState. Custom private SLOT.
/// Connect instrument. QT_statTimer connected.
///
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
                            tr("Comm Config Error"),
                            tr("No communication config found.")
                           );
        if(QT_statTimer->isActive()) {
            QT_statTimer->stop();
        }
        return;
    }
    if(I_connectInstTryCount > 10) {
        C_helper->normalErr(1,
                            tr("Communication Error"),
                            tr("Can't open port.")
                           );
        if(QT_statTimer->isActive()) {
            QT_statTimer->stop();
        }
        return;
    }
    I_connectInstTryCount++;
}

///
/// \brief ZevisionGen::onReadCommConfig. Custom public SLOT.
/// Read config file at communication and DataLog section.
///
void ZevisionGen::onReadConfig() {
    QM_commConfig = C_helper->readSection("./", "zevision.ini", "Communication");
    S_port = QM_commConfig["Port"];
    S_baudrate = QM_commConfig["Baudrate"];
    I_zevisionProtocol = QM_commConfig["Protocol"].toInt();
    QMap<QString, QString> qm_dataSetup = C_helper->readSection("./", "zevision.ini", "DataLog");
    B_isSaveData = qm_dataSetup["SaveData"].toInt();
    S_filePath = qm_dataSetup["Path"] + "/" + S_fileName + ".csv";
    I_fileOperate = qm_dataSetup["Operate"].toInt();
    I_collectIntvl = qm_dataSetup["Interval"].toInt();
    if(C_serial->getConnectState()) {
        return;
    }
    if(!QT_statTimer->isActive()) {
        QT_statTimer->start(1000);
    }
}

///
/// \brief ZevisionGen::onSendCommand. Custom public SLOT.
/// Send command to serialport. Connected to QT_acquireTimer.
///
void ZevisionGen::onSendCommand() {
    bool b_isConnected = C_serial->getConnectState();
    QMap<QString, QByteArray> qm_cmd = C_helper->zevisonCommandGenAlpha(&S_command, I_zevisionProtocol);
    QByteArray ba_cmd = qm_cmd["cmd_treated"];
    QStringList sl_msg = C_helper->zevisionMsgtoList(ba_cmd, I_zevisionProtocol);
    setTblData(sl_msg, ui->msg_tb);
    QSL_dataToSave.clear();
    QSL_dataToSave.append(QDateTime::currentDateTime().toString("hh:mm:ss.z"));
    QSL_dataToSave.append(S_command);
    QSL_dataToSave.append(sl_msg[6]);
    QSL_dataToSave.append(C_helper->bytearrayToString(ba_cmd));
    QSL_dataToSave.append(ba_cmd.toHex());
    if(b_isConnected) {
        C_serial->cmdEnQueue(
            qm_cmd
        );
    } else if(!QT_statTimer->isActive()) {
        QT_statTimer->start(1000);
    }
    setWidgeBackgroundColor(false);
}

///
/// \brief ZevisionGen::onRecvResponse. Custom public SLOT.
/// Connected to SerialCommSingleton.
/// \param qm_data. QVariantMap
///
void ZevisionGen::onRecvResponse(QVariantMap qm_data) {
    QByteArray ba_resp = qm_data["data"].toByteArray();
    QStringList sl_msg = C_helper->zevisionMsgtoList(ba_resp, I_zevisionProtocol);
    QSL_dataToSave.append(QDateTime::currentDateTime().toString("hh:mm:ss.z"));
    QSL_dataToSave.append(C_helper->bytearrayToString(ba_resp));
    QSL_dataToSave.append(ba_resp.toHex() + "\n");
    if(B_isSaveData && QT_acquireTimer->isActive()) {
        C_helper->saveData(QSL_dataToSave, S_filePath, 1);
    }
    setTblData(sl_msg, ui->resp_tb);
    setWidgeBackgroundColor(true);
}



///
/// \brief ZevisionGen::initializeTbl. Initialize message and response table.
/// Each table has two columns and 9 rows.
///
void ZevisionGen::initializeTbl() {
    ///Fixed table headers.
    QStringList QSL_tableHead = {
        ("Time"),
        ("Start Char"),
        ("Msg Length"),
        ("Data"),
        ("Checksum"),
        ("Stop Char"),
        ("Protocol"),
        ("Hex"),
        ("String")
    };
    ui->msg_tb->clear();
    ui->msg_tb->setColumnCount(2);
    ui->resp_tb->clear();
    ui->resp_tb->setColumnCount(2);
    QStringList tblHeader;
    tblHeader << "Item" << "Value";
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
}

///
/// \brief ZevisionGen::setTblData.
/// \param sl_data, const QStringList
/// \param tbl, const QTableWidget *
///
void ZevisionGen::setTblData(const QStringList sl_data, const QTableWidget *tbl) {
    int i_slLength = sl_data.length();
    if(i_slLength > 9) {
        if(QT_acquireTimer->isActive()) {
            QT_acquireTimer->stop();
            ui->send_btn->setText(tr("Start"));
            ui->cmd_le->setEnabled(true);
        } else {
            return;
        }
        C_helper->normalErr(
            1,
            tr("Message error"),
            sl_data.last()
        );
        return;
    }
    for (int i = 0; i < 9 ; i++ ) {
        tbl->item(i, 1)->setText(sl_data.at(i));
    }
}

void ZevisionGen::refreshTblWidget(bool b_useCHS) {
    QStringList tblHeader_CHS, tblHeader_ENG;
    tblHeader_CHS << u8"项目" << u8"值";
    tblHeader_ENG << "Item" << "Value";
    QStringList QSL_tableHead_CHS = {
        (u8"时间"),
        (u8"开始字符串"),
        (u8"数据长度"),
        (u8"数据"),
        (u8"校验码"),
        (u8"截止字符串"),
        (u8"协议"),
        (u8"十六进制"),
        (u8"字符串")
    };
    QStringList QSL_tableHead_ENG = {
        ("Time"),
        ("Start Char"),
        ("Msg Length"),
        ("Data"),
        ("Checksum"),
        ("Stop Char"),
        ("Protocol"),
        ("Hex"),
        ("String")
    };
    QStringList QSL_tableHead = b_useCHS ? QSL_tableHead_CHS : QSL_tableHead_ENG;
    ui->msg_tb->setHorizontalHeaderLabels(b_useCHS ? tblHeader_CHS : tblHeader_ENG);
    ui->resp_tb->setHorizontalHeaderLabels(b_useCHS ? tblHeader_CHS : tblHeader_ENG);
    for(int i = 0; i < 9; i++) {
        QString s_head = QSL_tableHead.at(i);
        ui->msg_tb->item(i, 0)->setText(s_head);
        ui->resp_tb->item(i, 0)->setText(s_head);
    }
}

///
/// \brief ZevisionGen::setWidgeBackgroundColor
/// \param b_isResponse
///
void ZevisionGen::setWidgeBackgroundColor(const bool b_isResponse) {
    QString s_activeStyleSheet = "background-color: #393E46",
            s_inActiveStyleSheet = "background-color: #FFFFFF";
    ui->msg_widget->setStyleSheet(b_isResponse ? s_inActiveStyleSheet : s_activeStyleSheet);
    ui->resp_widget->setStyleSheet(b_isResponse ? s_activeStyleSheet : s_inActiveStyleSheet);
}



