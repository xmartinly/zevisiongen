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
    QT_statTimer->start(1000);
}

ZevisionGen::~ZevisionGen() {
    delete ui;
}

void ZevisionGen::onRecvResponse(QVariantMap qm_data) {
    qDebug() << QDateTime::currentDateTime() << qm_data["data"].toByteArray() << qm_data["data"].toByteArray().toHex();
}


void ZevisionGen::on_actionDataLogSetting_triggered() {
}


void ZevisionGen::on_actionExit_triggered() {
}


void ZevisionGen::on_actionCommSettings_triggered() {
    D_commSet = new CommSetupDialog;
    D_commSet->exec();
}


void ZevisionGen::on_actionToggle_ENG_CHS_triggered() {
}


void ZevisionGen::on_actionHelp_triggered() {
}


void ZevisionGen::on_actionDocument_triggered() {
}

void ZevisionGen::onInstConnectState() {
    QMap<QString, QString> qm_commConfig = C_helper->readSection("./", "zevision.ini", "Communication");
    if(qm_commConfig.count() < 3) {
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
        return;
    }
    B_isInstConnected = C_serial->getConnectState();
    L_statStr->setText(B_isInstConnected ? "Inst Online." : "Inst Offline." + S_version);
    QString s_port, s_baudrate;
    if(qm_commConfig.count() == 3) {
        s_baudrate = qm_commConfig["Baudrate"];
        s_port = qm_commConfig["Port"];
        I_connectInstTryCount +=  C_serial->connInst(s_port, s_baudrate) ? 0 : 1;
    }
}


void ZevisionGen::on_send_btn_clicked() {
    ZevisionInst *inst = new ZevisionInst;
    inst->CommandGen(3, "she is a girl", "she is so beatiful", "but i don't know her name..");
}

