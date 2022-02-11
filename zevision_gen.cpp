#include "zevision_gen.h"
#include "ui_zevision_gen.h"

ZevisionGen::ZevisionGen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ZevisionGen)
{
    ui->setupUi(this);
}

ZevisionGen::~ZevisionGen()
{
    delete ui;
}

