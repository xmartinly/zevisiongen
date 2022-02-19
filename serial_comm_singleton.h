#ifndef SERIALCOMMSINGLETON_H
#define SERIALCOMMSINGLETON_H

#include"stable.h"

class SerialCommSingleton:  public QObject {
    Q_OBJECT

  public:
    static SerialCommSingleton *GetInstance();

    bool connInst(const QString &com_port, const QString &baud_rate);

    bool getConnectState() const {
        return B_isConnected;
    }

    bool getReadFinished() const {
        return B_isReadFinished;
    }

    void getResponse(QString &response);

    bool disconnInst();

    void cmdEnQueue(const QMap<QString, QByteArray> qm_cmd);


  private:
    SerialCommSingleton();

    static SerialCommSingleton *m_instance;

    QSerialPort *Comm_serialPort;

    bool B_isConnected = false, B_isReadFinished = true;

    QQueue<QMap<QString, QByteArray>> QQ_cmd;

    QString S_response = "", S_cmdString;

    QTimer *QT_recvDelayTimer, *QT_cmdQueueTimer;

  private slots:
    void recvCommData();

    void portRecvDataDelay();

    void sendCmd();

  signals:
    void sendResponse(QMap<QString, QVariant>);
};


#endif // SERIALCOMMSINGLETON_H
