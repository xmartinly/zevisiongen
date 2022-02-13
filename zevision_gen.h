﻿#ifndef ZEVISIONGEN_H
#define ZEVISIONGEN_H

#include <QMainWindow>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

#include "qcustomplot.h"
#include "serial_comm_singleton.h"
#include "common_helper.h"
#include "comm_setup_dialog.h"
#include "inficon_inst.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ZevisionGen;
}
QT_END_NAMESPACE

class ZevisionGen : public QMainWindow {
    Q_OBJECT

  public:
    ZevisionGen(QWidget *parent = nullptr);
    ~ZevisionGen();

    QString S_version = " | Zevision Command Gen. beta 0.0.1";
    QString S_port = "COM1";
    QString S_baudrate = "115200";
    int I_connectInstTryCount = 0;
    bool B_isInstConnected = false;



  public slots:

    void onRecvResponse(QVariantMap);

    void onInstConnectState();

  private slots:
    void on_actionDataLogSetting_triggered();

    void on_actionExit_triggered();

    void on_actionCommSettings_triggered();

    void on_actionToggle_ENG_CHS_triggered();

    void on_actionHelp_triggered();

    void on_actionDocument_triggered();

    void on_send_btn_clicked();

  private:
    Ui::ZevisionGen *ui;

    CommonHelper *C_helper;
    SerialCommSingleton *C_serial;

    CommSetupDialog *D_commSet;

    QTimer *QT_statTimer;

    QLabel *L_statStr = nullptr;

};
#endif // ZEVISIONGEN_H
