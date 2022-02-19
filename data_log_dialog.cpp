#include "data_log_dialog.h"
#include "ui_data_log_dialog.h"

DataLogDialog::DataLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataLogDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    C_helper = new CommonHelper;
    ui->fileOperate_cb->setHidden(true);
    ui->label_2->setHidden(true);
    readDataConfig();
}

DataLogDialog::~DataLogDialog() {
    delete ui;
}

void DataLogDialog::setDataConfig() {
    QString s_saveData = ui->save_chkb->isChecked() ? "1" : "0",
            s_filePath = ui->filePath_le->text(),
            s_fileOpt = QString::number(ui->fileOperate_cb->currentIndex()),
            s_collectIntvl = ui->interval_cb->currentText();
    QMap<QString, QString> qm_dataSetup;
    qm_dataSetup.insert("SaveData", s_saveData);
    qm_dataSetup.insert("Path", s_filePath);
    qm_dataSetup.insert("Operate", s_fileOpt);
    qm_dataSetup.insert("Interval", s_collectIntvl);
    C_helper->writeSettings(S_fileName, S_section, qm_dataSetup);
}

void DataLogDialog::readDataConfig() {
    QMap<QString, QString> qm_dataSetup = C_helper->readSection("./", S_fileName, S_section);
    if(qm_dataSetup.count() == 4) {
        ui->save_chkb->setChecked(qm_dataSetup["SaveData"].toInt());
        ui->filePath_le->setText(qm_dataSetup["Path"]);
        ui->fileOperate_cb->setCurrentIndex(qm_dataSetup["Operate"].toInt());
        ui->interval_cb->setCurrentText(qm_dataSetup["Interval"]);
    }
}

///
/// \brief DataLogDialog::on_ok_btn_clicked
///
void DataLogDialog::on_ok_btn_clicked() {
    setDataConfig();
    this->close();
}

