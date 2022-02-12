#ifndef COMM_SETUP_DIALOG_H
#define COMM_SETUP_DIALOG_H

#include <QDialog>
#include <QVariantMap>
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

public slots:

    void onRecvResponse(QVariantMap qm_resp);

private slots:
    void on_close_btn_clicked();

    void on_conn_btn_clicked();

private:
    Ui::CommSetupDialog *ui;
};

#endif // COMM_SETUP_DIALOG_H
