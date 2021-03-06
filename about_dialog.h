#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include"stable.h"

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog {
    Q_OBJECT

  public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

  private slots:
    void on_close_btn_clicked();

  private:
    Ui::AboutDialog *ui;
};

#endif // ABOUT_DIALOG_H
