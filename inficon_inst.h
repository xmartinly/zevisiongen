#ifndef INFICON_INST_H
#define INFICON_INST_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QDateTime>
#include <QDebug>

class InficonInst: public QObject {
    Q_OBJECT

  public:
    virtual QMap<QString, QByteArray> CommandGen(int num, const char *msg, ...);

    virtual QMap<QString, QByteArray> ResponseCal(const QByteArray response, ...);

};

class ZevisionInst: public InficonInst {
    Q_OBJECT
  public:

    ///
    /// \brief Binary format.0000: no checksum no length,0001 no checksum; 0010: no length.
    /// 0011: checksum and length
    ///
    enum ZevisionPortocol {
        ZevisionLenght = 1, ///< Command with length only.
        ZevisionChecksum = 2, ///< Command with checksum only.
        ZevisionOptional = 3 ///< Command with length and checksum.
    };
    Q_ENUM(ZevisionPortocol)

    ///
    /// \brief CommandGen
    /// \param cmd
    /// \param protocol
    /// \return
    ///
    inline QMap<QString, QByteArray> CommandGen(int num, const char *msg, ...) {
        QMap<QString, QByteArray> qm_cmd;
        QString tmp;
        va_list args;
        tmp += "[ " + QString(msg) + " ] ";
        va_start(args, msg);
        QString str = QString(va_arg(args, char *));
        if(!str.trimmed().isEmpty()) {
            tmp += "[ " + str + " ] ";
        }
        va_end(args);
        qDebug() << QString("(%1) ").arg(num) + tmp;
//        QByteArray ba_cmd = cmd.toLocal8Bit(); //convert to byte array
//        qm_cmd.insert("cmd_str", ba_cmd + '&' + (char)protocol); //insert cmd string to qmap
//        int i_sum = 0;
//        if(protocol == ZevisionInst::ZevisionChecksum || protocol == ZevisionInst::ZevisionOptional) { //calculate checksum
//            foreach (char s, ba_cmd) {
//                i_sum += (int)s;
//            }
//            ba_cmd.append((int)(i_sum % 256));
//        }
//        if(protocol == ZevisionInst::ZevisionLenght || protocol == ZevisionInst::ZevisionOptional) { //calculate cmd length
//            int i_cmdLen = cmd.length();
//            ba_cmd.prepend((char)i_cmdLen);
//            if(i_cmdLen < 255) {
//                ba_cmd.prepend('\x00');
//            }
//        }
//        ba_cmd.prepend('{');
//        ba_cmd.append('}');
//        qm_cmd.insert("cmd_treated", ba_cmd);
        return qm_cmd;
    }
};



#endif // INFICON_INST_H
