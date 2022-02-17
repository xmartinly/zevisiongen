#ifndef COMMONHELPER_H
#define COMMONHELPER_H

#include <QMessageBox>
#include <QObject>
#include <QIcon>
#include <QFile>
#include <QMap>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <math.h>

class CommonHelper : public QObject {
    Q_OBJECT
//    Q_ENUMS(ZevisionPortocol)
//    Q_ENUMS(ControllerType)
public:
    CommonHelper(QObject *parent = 0);


    enum CommandType {
        ReadCommand,
        SetCommand
    };

    enum ControllerBand {
        SigmaInst,
        InficonInst
    };
    enum ControllerType {
        SQC310,
        XTC3,
        IC6CYGNUS2,
        IMC300
    };
    Q_ENUM(ControllerType)
//    enum Priority { High, Low, VeryHigh, VeryLow };
//    Q_ENUM(Priority)
    ///
    /// \brief Binary format.0000: no checksum no length; 0001: no checksum; 0010: no length;
    /// 0011: checksum and length.
    ///
    enum ZevisionPortocol {
        ZevisionDefault = 0, ///< Command without length and checksum.
        ZevisionLength = 1, ///< Command with length only.
        ZevisionChecksum = 2, ///< Command with checksum only.
        ZevisionOptional = 3 ///< Command with length and checksum.
    };
    Q_ENUM(ZevisionPortocol)

    ///
    /// \brief The ZevisionAction enum.
    ///
    enum ZevisionAction {
        Update,
        Query,
        Add,
        Delete,
        Echo
    };
    Q_ENUM(ZevisionAction)

    ///
    /// \brief The ZevisionErrorCode enum.
    /// A: Illegal command, the command is not supported;
    /// B: Illegal parameter value;
    /// C: Checksum error (only when checksum is enabled);
    /// D: Illegal format;
    /// F: State error, the system is in the wrong state;
    /// L: Length error (only when length is enabled);
    /// T: Time out, only part of a command was received.
    /// Success: +
    /// Fail: -
    ///
    enum ZevisionErrorCode {A = 0x41, B = 0x42, C = 0x43, D = 0x44, F = 0x46, L = 0x4c, T = 0x54, Success = 0x2b, Fail = 0x2d};
    Q_ENUM(ZevisionErrorCode)

    const QMap<ZevisionErrorCode, QString> QM_errorMsg = {
        {A, "Illegal command, the command is not supported."},
        {B, "Illegal parameter value"},
        {C, "Checksum error (only when checksum is enabled)."},
        {D, "Illegal format"},
        {F, "State error, the system is in the wrong state."},
        {L, "Length error (only when length is enabled)."},
        {T, "Time out, only part of a command was received."},
    };
    static int infTFCCks(QByteArray bytes);

    inline QByteArray msgLengthCal(const QByteArray msg);

    inline int lengthBytesCal(const QByteArray length);

    inline static int hexStrToInt( QString hex_str);

    inline static float byteArrayToFloat(QByteArray data);

    inline static float freqCal(const QByteArray data, bool is_xtc = false);

    static void normalErr(const int errLevel, const QString errorTitle, const QString errorMsg);

    inline static QByteArray reviseData(QByteArray Bytes);

    inline static QString formatHexStr(QString hexStr);

    static QString formatHexStr(const QByteArray msg);

    QByteArray strToByteArray(const QString *s_cmd);

    QString hexToBinary(const QString Hex);



    QMap<QString, QByteArray> zevisonCommandGenAlpha(const QString *cmd, const int protocol);

    QVariantMap zevisonMessageCal(const QByteArray response, int protocol);

    QString zevisionErrorMsg(ZevisionErrorCode) const;

    QVariantMap zevisionMsgSplit(const QStringList msg_list);


    QMap<QString, QStringList> readData(const QString *fileName);

    QMap<QString, QMap<QString, QString> > readDataMap(const QString *fileName);

    QMap<QString, QString> readSection(const QString filePath, const QString fileName, const QString section);

    QPixmap getPixmap(const QString name, const qreal ratio, const QSize size);

    QList<unsigned char> sigmaTfcChecksum(const QByteArray *msg, bool is_response);

    QStringList readGroups(const QString *fileName);

    QSettings *openSettingFile(const QString *fileName, QString filePath = "" );


    void writeSettings(const QString fileName, const QString section, QMap<QString, QString> value);

    void deleteSection(const QString *fileName, const QString *group);

    void closeSettingFile(QSettings *settingFile);

//    void saveData(const QMap<QString, QString> *tfc_data, QString s_fileName) {
//        QString s_file = "./data/" + s_fileName;
//        QFile outFile(s_file);
//        QStringList sl_header, sl_data;
//        sl_header << "Time\t"
//                  << "Filtered Rate\t"
//                  << "Fundamental Frequency\t"
//                  << "Output Status\t"
//                  << "Input Status\t"
//                  << "Transient Messages\t"
//                  << "DAC Error\n";
//        sl_data  << (*tfc_data)["Time"] + "\t"
//                 << (*tfc_data)["Filtered Rate"] + "\t"
//                 << (*tfc_data)["Fundamental Frequency"] + "\t"
//                 << "'" + (*tfc_data)["Output Status"] + "\t"
//                 << "'" + (*tfc_data)["Input Status"] + "\t"
//                 << (*tfc_data)["Transient Messages"] + "\t"
//                 << (*tfc_data)["DAC Error"] + "\n";
//        if (outFile.size () == 0 && outFile.open(QFile::WriteOnly)) {
//            foreach (QString s, sl_header) {
//                outFile.write(s.toStdString().c_str());
//            }
//            outFile.close();
//        }
//        if(outFile.open(QFile::WriteOnly | QIODevice::Append)) {
//            foreach (QString s, sl_data) {
//                outFile.write(s.toStdString().c_str());
//            }
//            outFile.close();
//        }
//    }



//    void  str2hex(char*str, char *hex) {
//        const char * cHex  = "0123456789ABCDEF" ;
//        int  i = 0 ;
//        for ( int  j  = 0 ; j  <  strlen(str); j ++ ) {
//            unsigned int a  = (unsigned int) str[j];
//            hex[i ++ ] =  cHex[(a & 0xf0) >> 4];
//            hex[i ++ ] =  cHex[(a & 0x0f)];
//        }
//        hex[i]  =   '\0' ;
//    }

    ///
    /// \brief hex2int
    /// \param c
    /// \return
    ///
//    unsigned int hex2int(char c) {
//        if (c >= '0' && c <= '9') {
//            return  (unsigned  int) (c - 48);
//        } else if (c >= 'A' && c <= 'B') {
//            return  (unsigned int) (c - 65);
//        } else if (c >= 'a' &&  c <= 'b') {
//            return (unsigned int) (c - 97);
//        } else {
//            return 0 ;
//        }
//    }

//    void  hex2str( char *  hex, char *  str) {
//        int i = 0;
//        for ( int j = 0 ; j < strlen(hex) - 1;) {
//            unsigned int a =  hex2int(hex[j++]);
//            unsigned int b =  hex2int(hex[j++]);
//            str[i ++ ] = char(a  * 16 + b);
//        }
//        str[i]  =   '\0' ;
//    }


    ///
    /// \brief maxNumber, int
    /// \param a
    /// \param b
    /// \return int
    ///
    inline static int maxNumber(const int a, const int b) {
        return a > b ? a : b;
    }

    ///
    /// \brief maxNumber, float
    /// \param a
    /// \param b
    /// \return float
    ///
    inline static float maxNumber(const float a, const float b) {
        return a > b ? a : b;
    }
    ///
    /// \brief maxNumber, double
    /// \param a
    /// \param b
    /// \return double
    ///
    inline static double maxNumber(const double a, const double b) {
        return a > b ? a : b;
    }

    ///
    /// \brief minNumber, int
    /// \param a
    /// \param b
    /// \return int
    ///
    inline static int minNumber(const int a, const int b) {
        return a > b ? b : a;
    }

    ///
    /// \brief minNumber, float
    /// \param a
    /// \param b
    /// \return float
    ///
    inline static int minNumber(const float a, const float b) {
        return a > b ? b : a;
    }

    ///
    /// \brief minNumber, double
    /// \param a
    /// \param b
    /// \return double
    ///
    inline static int minNumber(const double a, const double b) {
        return a > b ? b : a;
    }
};
//extern const QMetaObject staticMetaObject;
//Q_DECLARE_METATYPE(CommonHelper::ZevisionPortocol)
//Q_DECLARE_METATYPE(CommonHelper::ControllerType)

#endif // COMMONHELPER_H
