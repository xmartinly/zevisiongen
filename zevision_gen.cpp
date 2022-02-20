#include "zevision_gen.h"
#include "ui_zevision_gen.h"

ZevisionGen::ZevisionGen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ZevisionGen) {
    ui->setupUi(this);
    S_fileName = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    C_serial = SerialCommSingleton::GetInstance();
    connect(C_serial, &SerialCommSingleton::sendResponse, this, &ZevisionGen::onRecvResponse);
    L_statStr = new QLabel();
    L_statStr->setAlignment(Qt::AlignRight);
    L_verStr = new QLabel(S_version);
    L_verStr->setAlignment(Qt::AlignRight);
//    L_statStr->setIndent(1);
    QLB_leftStatus = new QLabel(this);
    QLB_leftStatus->setPixmap(QP_inactivePic);
//    QLB_leftStatus->setGeometry(0, 50, 16, 16);
    QLB_leftStatus->setAlignment(Qt::AlignLeft);
    statusBar()->setSizeGripEnabled(false);
    statusBar()->addWidget(QLB_leftStatus, 2);
    statusBar()->addWidget(L_statStr, 1);
    statusBar()->addPermanentWidget(L_verStr);
//    statusBar()->
    QT_statTimer = new QTimer( this );
    QT_acquireTimer = new QTimer( this );
    connect( QT_statTimer, SIGNAL(timeout()), this, SLOT(onInstConnectState()));
    connect( QT_acquireTimer, SIGNAL(timeout()), this, SLOT(onSendCommand()));
    onReadConfig();
    initializeTbl();
    m_translator = new QTranslator(qApp);
//    m_translator->load(":/i18n/ZevisionGen_en_US.qm");
    qApp->installTranslator(m_translator);
}

ZevisionGen::~ZevisionGen() {
    delete ui;
}

///
/// \brief ZevisionGen::on_actionToggle_CHS_ENG_triggered
///
void ZevisionGen::on_actionToggle_CHS_ENG_triggered() {
    if(QT_acquireTimer->isActive()) {
        on_send_btn_clicked();
    }
    B_isUsingTrans = !B_isUsingTrans;
    QString s_transFileName;
    s_transFileName = B_isUsingTrans ? "ZevisionGen_zh_CN.qm" :  "ZevisionGen_en_US.qm";
    m_translator->load(":/i18n/" + s_transFileName);
    ui->retranslateUi(this);
    refreshTblWidget(B_isUsingTrans);
}

///
/// \brief ZevisionGen::on_cmd_le_returnPressed
///
void ZevisionGen::on_cmd_le_returnPressed() {
    S_command = ui->cmd_le->text();
    onSendCommand();
}

///
/// \brief ZevisionGen::on_send_btn_clicked. SYSTEM private SLOT.
///
void ZevisionGen::on_send_btn_clicked() {
    S_command = ui->cmd_le->text();
    bool b_isAcquireTimerActive = QT_acquireTimer->isActive();
    ui->send_btn->setText(b_isAcquireTimerActive ? tr("Start") : tr("Stop"));
    ui->cmd_le->setEnabled(b_isAcquireTimerActive);
    if(b_isAcquireTimerActive) {
        QLB_leftStatus->setPixmap(QP_inactivePic);
        QT_acquireTimer->stop();
        return;
    }
    QT_acquireTimer->start(I_collectIntvl);
}

///
/// \brief ZevisionGen::on_actionDataLogSetting_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionDataLogSetting_triggered() {
    if(QT_acquireTimer->isActive()) {
        on_send_btn_clicked();
    }
    if(QT_statTimer->isActive()) {
        QT_statTimer->stop();
    }
    D_dataLog = new DataLogDialog;
    connect(D_dataLog, &DataLogDialog::configSetted, this, &ZevisionGen::onReadConfig);
    D_dataLog->exec();
}

///
/// \brief ZevisionGen::on_actionExit_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionExit_triggered() {
    this->close();
}

///
/// \brief ZevisionGen::on_actionCommSettings_triggered. SYSTEM private SLOT.
///
void ZevisionGen::on_actionCommSettings_triggered() {
    if(QT_acquireTimer->isActive()) {
        on_send_btn_clicked();
    }
    if(QT_statTimer->isActive()) {
        QT_statTimer->stop();
    }
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
    bool b_connectStateSuccess = C_serial->getConnectState();
    if(b_connectStateSuccess &&  QT_statTimer->isActive()) {
        QT_statTimer->stop();
        return;
    }
    if(!b_connectStateSuccess) {
        b_connectStateSuccess = C_serial->connInst(S_port, S_baudrate);
    }
    L_statStr->setText((b_connectStateSuccess ? "Port open." : "Port close."));
    if(I_connectInstTryCount > 10 && QT_statTimer->isActive()) {
        C_helper->normalErr(1,
                            tr("Communication Error"),
                            tr("Can't open port.")
                           );
        QT_statTimer->stop();
    }
    I_connectInstTryCount++;
}

///
/// \brief ZevisionGen::onReadCommConfig. Custom public SLOT.
/// Read config file at communication and DataLog section.
///
void ZevisionGen::onReadConfig() {
    C_serial->disconnInst();
    if(QT_statTimer->isActive()) {
        QT_statTimer->stop();
    }
    QMap<QString, QString> qm_commSetup = C_helper->readSection("./", "zevision.ini", "Communication");
    I_connectInstTryCount = 0;
    S_port = qm_commSetup["Port"];
    S_baudrate = qm_commSetup["Baudrate"];
    I_zevisionProtocol = qm_commSetup["Protocol"].toInt();
    QMap<QString, QString> qm_dataSetup = C_helper->readSection("./", "zevision.ini", "DataLog");
    B_isSaveData = qm_dataSetup["SaveData"].toInt();
    S_filePath = qm_dataSetup["Path"] + "/" + S_fileName + ".csv";
    I_fileOperate = qm_dataSetup["Operate"].toInt();
    I_collectIntvl = qm_dataSetup["Interval"].toInt();
    QT_statTimer->start(1000);
}

///
/// \brief ZevisionGen::onSendCommand. Custom public SLOT.
/// Send command to serialport. Connected to QT_acquireTimer.
///
void ZevisionGen::onSendCommand() {
    setStatusActiveIcon(false);
    bool B_isCmdEmpty = S_command.length() == 0;
    if(B_isCmdEmpty) {
        C_helper->normalErr(1,
                            tr("Command error"),
                            tr("Command can not be empty.")
                           );
        return;
    }
    bool b_isConnected = C_serial->getConnectState();
    QMap<QString, QByteArray> qm_cmd = C_helper->zevisonCommandGenAlpha(&S_command, I_zevisionProtocol);
    QByteArray ba_cmd = qm_cmd["cmd_treated"];
    QStringList sl_msg = C_helper->zevisionMsgtoList(&ba_cmd, I_zevisionProtocol);
    setTblData(sl_msg, ui->msg_tb);
    QSL_dataToSave.clear();
    QSL_dataToSave.append(QDateTime::currentDateTime().toString("hh:mm:ss.z"));
    QSL_dataToSave.append(S_command);
    QSL_dataToSave.append(sl_msg[6]);
    QSL_dataToSave.append(C_helper->bytearrayToString(ba_cmd));
    QSL_dataToSave.append(C_helper->formatHexStr(ba_cmd));
    if(b_isConnected) {
        C_serial->cmdEnQueue(
            qm_cmd
        );
    }
}

///
/// \brief ZevisionGen::onRecvResponse. Custom public SLOT.
/// Connected to SerialCommSingleton.
/// \param qm_data. QVariantMap
///
void ZevisionGen::onRecvResponse(QVariantMap qm_data) {
    QByteArray ba_resp = qm_data["data"].toByteArray();
    QStringList sl_msg = C_helper->zevisionMsgtoList(&ba_resp, I_zevisionProtocol);
    QSL_dataToSave.append(QDateTime::currentDateTime().toString("hh:mm:ss.z"));
    QSL_dataToSave.append(C_helper->bytearrayToString(ba_resp));
    QSL_dataToSave.append(C_helper->formatHexStr(ba_resp) + "\n");
    if(B_isSaveData && QT_acquireTimer->isActive()) {
        C_helper->saveData(QSL_dataToSave, S_filePath, 1);
    }
    setTblData(sl_msg, ui->resp_tb);
    setStatusActiveIcon(true);
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
    ui->msg_tb->horizontalHeader()->setStyleSheet(S_tableHeaderStyle);
    ui->msg_tb->horizontalHeader()->setHighlightSections(false);
    ui->resp_tb->horizontalHeader()->setStyleSheet(S_tableHeaderStyle);
    ui->resp_tb->horizontalHeader()->setHighlightSections(false);
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
void ZevisionGen::setStatusActiveIcon(const bool b_isResponse) {
    if(!QT_acquireTimer->isActive()) {
        return;
    }
    QLB_leftStatus->setPixmap(b_isResponse ? QP_finalStatePic : QP_activePic);
    statusBar()->update();
}




