#ifndef ZEVISIONGEN_H
#define ZEVISIONGEN_H

#include <QMainWindow>
#include <QFile>
#include <QMessageBox>
#include <QTimer>
#include <QLocale>
#include <QTranslator>

#include "qcustomplot.h"
#include "common_helper.h"
#include "serial_comm_singleton.h"
#include "comm_setup_dialog.h"
//#include "inficon_inst.h"

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

    ///
    /// \brief S_version. Version string shown at status bar.
    ///
    QString S_version = " | Zevision Command Gen. beta 0.0.1";

    ///
    /// \brief S_port. Default com port string.
    ///
    QString S_port = "COM1";

    ///
    /// \brief S_baudrate. Default comm baudrate string.
    ///
    QString S_baudrate = "115200";

    ///
    /// \brief S_command. Command string from the cmd_le.
    ///
    QString S_command = "";

    ///
    /// \brief I_connectInstTryCount. Connection tries count. Will stop QT_statTimer when increase to 10.
    ///
    int I_connectInstTryCount = 0;

    ///
    /// \brief I_zevisionProtocol
    ///
    int I_zevisionProtocol = 0;

    ///
    /// \brief B_isInstConnected
    ///
    bool B_isInstConnected = false;

    bool B_isUsingTrans = false;

    ///Communication configs. Include port, baudrate, protocol
    QMap<QString, QString> QM_commConfig;

    ///Fixed table headers.
    QStringList QSL_tableHead = {
        tr("Time"),
        tr("Start Char"),
        tr("Msg Length"),
        tr("Data"),
        tr("Checksum"),
        tr("Stop Char"),
        tr("Protocol"),
        tr("Hex"),
        tr("String")
    };

    ///
    /// \brief initializeTbl
    ///
    void initializeTbl();

    ///
    /// \brief setTblData
    /// \param sl_data
    /// \param tbl
    ///
    void setTblData(const QStringList sl_data, const QTableWidget* tbl);

public slots:

    ///
    /// \brief onRecvResponse. Receive data from serialCommSingleton Class.
    ///
    void onRecvResponse(QVariantMap);

    ///
    /// \brief onInstConnectState
    ///
    void onInstConnectState();

    ///
    /// \brief onReadCommConfig
    ///
    void onReadCommConfig();

    ///
    /// \brief onSendCommand
    ///
    void onSendCommand();


private slots:
    void on_actionDataLogSetting_triggered();

    void on_actionExit_triggered();

    void on_actionCommSettings_triggered();

    void on_actionHelp_triggered();

    void on_actionDocument_triggered();

    void on_send_btn_clicked();

    void on_actionToggle_CHS_ENG_triggered();

private:
    Ui::ZevisionGen *ui;

    ///
    /// \brief C_helper. CommonHelper pointer.
    ///
    CommonHelper *C_helper;

    ///
    /// \brief C_serial. SerialCommSingleton pointer.
    ///
    SerialCommSingleton *C_serial;

    ///
    /// \brief D_commSet. CommSetupDialog pointer.
    ///
    CommSetupDialog *D_commSet;

    ///
    /// \brief QT_statTimer.
    ///
    QTimer *QT_statTimer;

    ///
    /// \brief QT_acquireTimer
    ///
    QTimer *QT_acquireTimer;

    ///
    /// \brief L_statStr. Lable for status bar.
    ///
    QLabel *L_statStr = nullptr;

    QTranslator *m_translator;

};
#endif // ZEVISIONGEN_H
