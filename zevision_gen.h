#ifndef ZEVISIONGEN_H
#define ZEVISIONGEN_H

#include <QMainWindow>
#include <QFile>
#include <QMessageBox>

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

private:
    Ui::ZevisionGen *ui;
};
#endif // ZEVISIONGEN_H
