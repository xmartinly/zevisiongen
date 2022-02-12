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

    const QByteArray H1_withOutLenChksum = QByteArrayLiteral("\x7b\x48\x31\x7d");
    const QByteArray H1_withOutLen = QByteArrayLiteral("\x7b\x48\x31\x79\x7d");
    const QByteArray H1_withOutChksum = QByteArrayLiteral("\x7b\x02\x00\x48\x31\x7d");
    const QByteArray H1_withLenChksum = QByteArrayLiteral("\x7b\x02\x00\x48\x31\x79\x7d");



  public slots:

    void onRecvResponse(QVariantMap qm_resp);

  private slots:
    void on_close_btn_clicked();

    void on_conn_btn_clicked();

  private:
    Ui::CommSetupDialog *ui;

    CommonHelper *C_helper;
    SerialCommSingleton *C_serial;

    void closeEvent(QCloseEvent *event);
};

#endif // COMM_SETUP_DIALOG_H
