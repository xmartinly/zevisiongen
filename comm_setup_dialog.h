#ifndef COMM_SETUP_DIALOG_H
#define COMM_SETUP_DIALOG_H

#include <QDialog>
#include <QVariantMap>
#include <QCloseEvent>
#include "common_helper.h"
#include "serial_comm_singleton.h"

namespace Ui {
    class CommSetupDialog;
}

class CommSetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit CommSetupDialog(QWidget *parent = nullptr);
    ~CommSetupDialog();

    const QString S_hello = "H1";

    int I_zevisionProtocol = -1;

    void setCommConfig();

    void readProtocol();

    void setProtocol(const int protocol);

    int getProtocol();

public slots:

    void onRecvResponse(QVariantMap qm_resp);

private slots:
    void on_close_btn_clicked();

    void on_conn_btn_clicked();

    void on_set_btn_clicked();

private:
    Ui::CommSetupDialog *ui;

    CommonHelper *C_helper;
    SerialCommSingleton *C_serial;

    void closeEvent(QCloseEvent *event);

signals:
    void configSetted();
};

#endif // COMM_SETUP_DIALOG_H
