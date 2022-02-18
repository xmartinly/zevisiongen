#ifndef DATA_LOG_DIALOG_H
#define DATA_LOG_DIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QDebug>

#include "common_helper.h"

namespace Ui {
    class DataLogDialog;
}

class DataLogDialog : public QDialog {
    Q_OBJECT

public:
    explicit DataLogDialog(QWidget *parent = nullptr);
    ~DataLogDialog();

    void setDataConfig();

    void readDataConfig();

    QString S_fileName = "zevision.ini";
    QString S_section = "DataLog";

private slots:

    void on_close_btn_clicked();

    void on_ok_btn_clicked();

private:
    Ui::DataLogDialog *ui;

    CommonHelper *C_helper;
};

#endif // DATA_LOG_DIALOG_H
