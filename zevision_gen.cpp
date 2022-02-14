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
//    ui->splitter->setSizes({{9, 1}});
    QT_statTimer = new QTimer( this );
    connect( QT_statTimer, SIGNAL(timeout()), this, SLOT(onInstConnectState()));
    onReadCommConfig();
}

ZevisionGen::~ZevisionGen() {
    delete ui;
}

void ZevisionGen::onRecvResponse(QVariantMap qm_data) {
//    qDebug() << QDateTime::currentDateTime() << qm_data["data"].toByteArray() << qm_data["data"].toByteArray().toHex();
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
        if(QT_statTimer->isActive()) {
            QT_statTimer->stop();
        }
    }
//    qDebug() << L_statStr->text() << S_version;
    if(QM_commConfig.count() != 3) {
        C_helper->normalErr(1,
                            u8"Comm Config Error",
                            u8"No communication config found."
                           );
        if(QT_statTimer->isActive()) {
            QT_statTimer->stop();
        }
        return;
    }
    if(I_connectInstTryCount > 10) {
        C_helper->normalErr(1,
                            u8"通信错误",
                            u8"仪器无法连接, 请检测连接设置."
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
    I_protocol = QM_commConfig["Protocol"].toInt();
    if(!QT_statTimer->isActive()) {
        QT_statTimer->start(1000);
    }
}


void ZevisionGen::on_send_btn_clicked() {
//    ZevisionInst *inst = new ZevisionInst;
//    inst->CommandGen(3, "she is a girl", "she is so beatiful", "but i don't know her name..");
}

