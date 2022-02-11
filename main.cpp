#include "zevision_gen.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <Windows.h>


LONG __RPC_CALLEE ApplicationCrashHandler(EXCEPTION_POINTERS *pException) { //程式异常捕获
    /*
      ***保存数据代码***
    */
    //这里弹出一个错误对话框并退出程序
    EXCEPTION_RECORD *record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode, 16)), errAdr(QString::number((uint)record->ExceptionAddress, 16)), errMod;
    QMessageBox::critical(NULL, u8"程式崩溃", u8"<FONT size=4><div><b>对于发生的错误，表示诚挚的歉意</b><br/></div>" +
                          QString(u8"<div>错误代码：%1</div><div>错误地址：%2</div></FONT>").arg(errCode).arg(errAdr),
                          QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);
//    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ZevisionGen_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    QFile qss(":/Resource/Qss/common.qss");
    qss.open(QFile::ReadOnly);
    if(qss.isOpen()) {
        qApp->setStyleSheet(qss.readAll());
        qss.close();
    }
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数
    ZevisionGen w;
    w.show();
    return a.exec();
}
