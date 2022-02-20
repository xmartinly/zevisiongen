#include "serial_comm_singleton.h"

SerialCommSingleton::SerialCommSingleton() {
    Comm_serialPort = new QSerialPort(this);
    if(Comm_serialPort->isOpen()) {
        Comm_serialPort->clear();
        Comm_serialPort->close();
    }
    QT_recvDelayTimer = new QTimer(this);
    QT_cmdQueueTimer = new QTimer(this);
    connect(Comm_serialPort, &QSerialPort::readyRead, this, &SerialCommSingleton::portRecvDataDelay);
    connect(QT_recvDelayTimer, &QTimer::timeout, this, &SerialCommSingleton::recvCommData);
    connect(QT_cmdQueueTimer, &QTimer::timeout, this, &SerialCommSingleton::sendCmd);
}

///
/// \brief SerialCommSingleton::GetInstance
/// \return SerialCommSingleton. Instance pointer.
///
SerialCommSingleton *SerialCommSingleton::GetInstance() {
    return m_instance;
}

///
/// \brief SerialCommSingleton::m_instance. SerialCommSingleton private construct.
///
SerialCommSingleton *SerialCommSingleton::m_instance = new SerialCommSingleton();

///
/// \brief SerialCommSingleton::connInst. Connect the instrument with given com port and baudrate.
/// Return TRUE if connect success, FALSE if connect failed.
/// \param com_port
/// \param baud_rate
/// \return
///
bool SerialCommSingleton::connInst(const QString &com_port, const QString &baud_rate) {
    if(Comm_serialPort->isOpen()) {
        return false;
    }
    if(com_port == "" || baud_rate == NULL) {
        return false;
    }
    uint ui_baudrate = baud_rate.toUInt();
    Comm_serialPort->setPortName(com_port);
    Comm_serialPort->setBaudRate(ui_baudrate);
    Comm_serialPort->setParity(QSerialPort::NoParity);
    Comm_serialPort->setDataBits(QSerialPort::Data8);
    Comm_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    Comm_serialPort->setStopBits(QSerialPort::OneStop);
    B_isConnected = Comm_serialPort->open(QIODevice::ReadWrite);
    if(B_isConnected) {
        QT_cmdQueueTimer->start(30);
    }
    return B_isConnected;
}

///
/// \brief SerialCommSingleton::sendCmd. Write command to the opened port. CUSTOM private SLOT.
/// Command is dequequed from the QQ_cmd.
/// If serial port is not open, function will stop.
/// If QQ_cmd queque is empty, funciton will stop.
///
void SerialCommSingleton::sendCmd() {
    if(!Comm_serialPort->isOpen()) {
        return;
    }
    if(!B_isReadFinished || QQ_cmd.count() < 1) {
        return;
    } else {
        QMap<QString, QByteArray> qm_cmd = QQ_cmd.dequeue();
        S_cmdString = qm_cmd["cmd_str"];
        B_isReadFinished = false;
        Comm_serialPort->write(qm_cmd["cmd_treated"]);
        Comm_serialPort->flush();
    }
}

///
/// \brief SerialCommSingleton::portRecvDataDelay. Read port data delay for 10ms
/// to avoid incomplete data.
///
void SerialCommSingleton::portRecvDataDelay() {
    QT_recvDelayTimer->stop();
    QT_recvDelayTimer->start(20);
}

///
/// \brief SerialCommSingleton::getResponse. Public function will write response to the given var.
/// \param response
///
void SerialCommSingleton::getResponse(QString &response) {
    if(S_response.length() > 0) {
        response = S_cmdString
                   + ":"
                   + S_response;
    }
    S_response = "";
    S_cmdString = "";
}

///
/// \brief SerialCommSingleton::recvCommData. Read serial port data.
/// If serial port not open, function stops.
///    Otherwise function will emit the sendResponse signal.
///
void SerialCommSingleton::recvCommData() {
    QT_recvDelayTimer->stop();
    if(!Comm_serialPort->isOpen()) {
        return;
    }
    QByteArray ba_response = Comm_serialPort->readLine();
    S_response = QString::number(QDateTime::currentMSecsSinceEpoch())
                 + ":" + ba_response;
    B_isReadFinished = true;
    QMap<QString, QVariant> qm_resp;
    qm_resp.insert("cmd", S_cmdString);
    qm_resp.insert("data", ba_response);
    emit sendResponse(qm_resp);
}

///
/// \brief SerialCommSingleton::disconnInst. Disconnect the serial port connection.
/// \return bool. serialport isOpen.
///
bool SerialCommSingleton::disconnInst() {
    if(QT_cmdQueueTimer->isActive()) {
        QT_cmdQueueTimer->stop();
    }
    if(!QQ_cmd.isEmpty()) {
        QQ_cmd.clear();
    }
    B_isConnected = false;
    B_isReadFinished = true;
    if(!Comm_serialPort->isOpen()) {
        return true;
    }
    Comm_serialPort->flush();
    Comm_serialPort->readAll();
    Comm_serialPort->clear();
    Comm_serialPort->close();
    return !Comm_serialPort->isOpen();
}

///
/// \brief SerialCommSingleton::cmdEnQueue. Add command data to the queue.
///  For example: {"cmd_str", !#@O7}
/// \param qm_cmd
///
void SerialCommSingleton::cmdEnQueue(const QMap<QString, QByteArray> qm_cmd) {
    QQ_cmd.enqueue(qm_cmd);
}
