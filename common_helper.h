#ifndef COMMONHELPER_H
#define COMMONHELPER_H

#include <QMessageBox>
#include <QIcon>
#include <QFile>
#include <QMap>
#include <QDir>
#include <QDebug>
#include <QSettings>

class CommonHelper {
public:
    CommonHelper();


    enum CommandType {
        ReadCommand,
        SetCommand
    };

    enum ControllerBand {
        SigmaInst,
        InficonInst
    };


    static int infTFCCks(QByteArray bytes);

    static int hexStrToInt( QString hex_str);

    static float byteArrayToFloat(QByteArray data);

    static float freqCal(const QByteArray data, bool is_xtc = false);

    static void normalErr(const int errLevel, const QString errorTitle, const QString errorMsg);

    static QByteArray reviseData(QByteArray Bytes);

    static QString formatHexStr(QString hexStr);


    QByteArray strToByteArray(const QString *s_cmd);

    QString hexToBinary(const QString Hex);

    QMap<QString, QByteArray> ZevisonCommandGenAlpha(const QString *cmd);

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
    static int maxNumber(const int a, const int b) {
        return a > b ? a : b;
    }

    ///
    /// \brief maxNumber, float
    /// \param a
    /// \param b
    /// \return float
    ///
    static float maxNumber(const float a, const float b) {
        return a > b ? a : b;
    }
    ///
    /// \brief maxNumber, double
    /// \param a
    /// \param b
    /// \return double
    ///
    static double maxNumber(const double a, const double b) {
        return a > b ? a : b;
    }

    ///
    /// \brief minNumber, int
    /// \param a
    /// \param b
    /// \return int
    ///
    static int minNumber(const int a, const int b) {
        return a > b ? b : a;
    }

    ///
    /// \brief minNumber, float
    /// \param a
    /// \param b
    /// \return float
    ///
    static int minNumber(const float a, const float b) {
        return a > b ? b : a;
    }

    ///
    /// \brief minNumber, double
    /// \param a
    /// \param b
    /// \return double
    ///
    static int minNumber(const double a, const double b) {
        return a > b ? b : a;
    }
};

#endif // COMMONHELPER_H
