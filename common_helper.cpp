#include "common_helper.h"


CommonHelper::CommonHelper(QObject *parent): QObject(parent) {
}

QString CommonHelper::zevisionErrorMsg(ZevisionErrorCode code) const {
    return QM_errorMsg[code];
}

QVariantMap CommonHelper::zevisionMsgSplit(const QStringList msg_list) {
    QVariantMap qm_msgList;
    int i_cmdSq = 1;
    foreach (QString s, msg_list) {
        QStringList _sl = s.split(":");
        qm_msgList.insert(_sl.at(0), _sl.at(1));
        i_cmdSq++;
    }
    return qm_msgList;
}

//QString CommonHelper::zevisionErrorMsg(const int error_code) const {
//    ZevisionErrorCode  error = error_code;
//    return QM_errorMsg[error_code];
//}

///
/// \brief CommonHelper::writeSettings. Write settings to .ini file.
/// \param fileName
/// \param section
/// \param values
///
void CommonHelper::writeSettings(const QString fileName, const QString section, QMap<QString, QString> values) {
    QSettings *f_iniFile = openSettingFile(&fileName);
    f_iniFile->beginGroup(section);
    QMap<QString, QString>::const_iterator i;
    for( i = values.constBegin(); i != values.constEnd(); ++i) {
        f_iniFile->setValue(i.key(), i.value());
    }
    f_iniFile->endGroup();
    closeSettingFile(f_iniFile);
}

///
/// \brief CommonHelper::deleteSection. Delete settings at the given section
/// \param fileName
/// \param group
///
void CommonHelper::deleteSection(const QString *fileName, const QString *group) {
    QSettings *f_iniFile = openSettingFile(fileName);
    f_iniFile->remove(*group);
    closeSettingFile(f_iniFile);
}

///
/// \brief CommonHelper::closeSettingFile. Close .ini setting file.
/// \param settingFile
///
void CommonHelper::closeSettingFile(QSettings *settingFile) {
    delete settingFile;
    settingFile = nullptr;
}

///
/// \brief CommonHelper::readGroups.  Read setting groups from the given .ini file.
/// \param fileName
/// \return QStringList. Config file groups.
///
QStringList CommonHelper::readGroups(const QString *fileName) {
    QSettings *f_iniFile = openSettingFile(fileName);
    QStringList sl_groups = f_iniFile->childGroups();
    closeSettingFile(f_iniFile);
    return sl_groups;
}

///
/// \brief CommonHelper::openSettingFile. Open a setting file for more operate.
/// \param fileName
/// \param filePath
/// \return QSettings*. Config file pointer.
///
QSettings *CommonHelper::openSettingFile(const QString *fileName, QString filePath) {
    QString currentDir = QDir::currentPath();
    QString s_file = (filePath != "" ? filePath : currentDir + "/") + *fileName;
    QFile outFile(s_file);
    if (outFile.size () == 0 ) {
        outFile.open(QFile::WriteOnly);
        outFile.close();
    }
    QSettings *f_iniFile = new QSettings(s_file, QSettings::IniFormat);
    return f_iniFile;
}

///
/// \brief CommonHelper::readSection. Read section data at the given section from a .ini file.
/// \param filePath
/// \param fileName
/// \param section
/// \return QMap<QString, QString>. Config file section data.
///
QMap<QString, QString> CommonHelper::readSection(
    const QString filePath,
    const QString fileName,
    const QString section) {
    QMap<QString, QString> qm_config;
    QSettings *f_iniFile = openSettingFile(&fileName);
    f_iniFile->beginGroup(section);
    QStringList sl_keyList = f_iniFile->childKeys();
    f_iniFile->childKeys();
    foreach(QString key, sl_keyList) {
        qm_config.insert(key, f_iniFile->value(key).toString());
    }
    f_iniFile->endGroup();
    return qm_config;
}

///
/// \brief CommonHelper::readData. Read settings from the .ini file.
/// \param fileName
/// \return QMap<QString, QStringList>. Config file data.
///
QMap<QString, QStringList> CommonHelper::readData(const QString *fileName) {
    QSettings *f_iniFile = openSettingFile(fileName);
    QMap<QString, QStringList> qm_data;
    QStringList sl_groupList = readGroups(fileName);
    foreach(QString group, sl_groupList) {
        QStringList sl_values;
        f_iniFile->beginGroup(group);
        QStringList keyList = f_iniFile->childKeys();
        foreach(QString key, keyList) {
            sl_values.append(f_iniFile->value(key).toString().toStdString().c_str());
        }
        f_iniFile->endGroup();
        qm_data.insert(group, sl_values);
    }
    closeSettingFile(f_iniFile);
    return qm_data;
}

///
/// \brief CommonHelper::readDataMap. Read setting file data map.
/// \param fileName
/// \return QMap<QString, QMap<QString, QString>>, data map of the config file.
///
QMap<QString, QMap<QString, QString>> CommonHelper::readDataMap(const QString *fileName) {
    QSettings *f_iniFile = openSettingFile(fileName);
    QMap<QString, QMap<QString, QString>> qm_data;
    QStringList sl_groupList = readGroups(fileName);
    foreach(QString group, sl_groupList) {
        QStringList sl_values;
        QMap<QString, QString> group_data;
        f_iniFile->beginGroup(group);
        QStringList keyList = f_iniFile->childKeys();
        foreach(QString key, keyList) {
            group_data.insert(key, f_iniFile->value(key).toString());
        }
        f_iniFile->endGroup();
        qm_data.insert(group, group_data);
    }
    closeSettingFile(f_iniFile);
    return qm_data;
}


///
/// \brief formatHexStr
/// \param hexStr
/// \return
///
QString CommonHelper::formatHexStr(QString hexStr) {
    return hexStr.indexOf("0") == 0 ? hexStr.mid(1, 1) : hexStr;
}

QString CommonHelper::formatHexStr(const QByteArray msg) {
    QString s_hexMsg = msg.toHex();
    int i_msgLength = msg.length();
    QString s_msg;
    for (int i = 0; i < i_msgLength; i++) {
        s_msg.append(s_hexMsg.mid(i * 2, 2) + " ");
    }
    return s_msg.trimmed().toUpper();
}


///
/// \brief CommonHelper::infTFCCks. Calculate data checksum.
/// \param bytes
/// \return int. Checksum value.
///
int CommonHelper::infTFCCks(QByteArray bytes) {
    int sum = 0;
    foreach (char s, bytes) {
        int _i = (int)s;
        sum += _i;
    }
    return sum % 256;
}

QByteArray CommonHelper::msgLengthCal(const QByteArray msg) {
    QByteArray ba_length;
    int i_totalLength = msg.length();
    int i_highByte = floor(i_totalLength / 256);
    int i_lowByte = i_totalLength % 256;
    ba_length.append(i_highByte);
    ba_length.append(i_lowByte);
    return ba_length;
}

int CommonHelper::lengthBytesCal(const QByteArray length) {
    if(length.length() != 2) {
        return -1;
    }
    return (int)length.at(0) * 256 + (int)length.at(1);
}

///
/// \brief CommonHelper::ZevisonCommandGenAlpha. Generate command map for a given cmd string.
/// \param cmd
/// \return
///
QMap<QString, QByteArray> CommonHelper::zevisonCommandGenAlpha(const QString *cmd, const int protocol) {
    QMap<QString, QByteArray> qm_cmd;
    QByteArray ba_cmd = cmd->toLocal8Bit(); //convert to byte array
    qm_cmd.insert("cmd_str", ba_cmd + '&' + (char)protocol); //insert cmd string to qmap
    int i_sum = 0;
    if(protocol == ZevisionChecksum || protocol == ZevisionOptional) { //calculate checksum
        foreach (char s, ba_cmd) {
            i_sum += (int)s;
        }
        ba_cmd.append((int)(i_sum % 256));
    }
    if(protocol == ZevisionLength || protocol == ZevisionOptional) { //calculate cmd length
        int i_cmdLen = cmd->length();
        ba_cmd.prepend((char)i_cmdLen);
        if(i_cmdLen < 255) {
            ba_cmd.prepend('\x00');
        }
    }
    ba_cmd.prepend('{');
    ba_cmd.append('}');
    qm_cmd.insert("cmd_treated", ba_cmd);
//    qm_cmd.insert("cmd_protocol", (char)protocol);
    return qm_cmd;
}

QVariantMap CommonHelper::zevisonMessageCal(const QByteArray response, int protocol) {
    QVariantMap qm_resp;
    QByteArray ba_resp;
    int i_responseLen = response.length();  //response length
    int i_respStatusCharNegPos = response.indexOf('-'); //find negative char
    int i_respStatusCharPosPos = response.indexOf('+'); //find postive char
    int i_dataStartOffset = 1; //initialize data string beging offset
//    int i_dataEndOffset = 1; //initialize data string stop offset
    int i_msgLength = i_responseLen - 2; //initialize reponse message length, minus 2 to drop '{' and '}'
    bool b_msgStatusFound = i_respStatusCharNegPos != -1; //negative char found
    qm_resp["msg_status"] = i_respStatusCharNegPos == -1;
//    qDebug() << i_responseLen << i_respStatusCharNegPos << i_respStatusCharPosPos << i_dataStartOffset << i_dataEndOffset;
    if(b_msgStatusFound) { // negative char found
        char c_error = response.at(i_respStatusCharNegPos + 1);
        qm_resp.insert("msg_status", false);
        qm_resp.insert("msg_err_char", c_error);
        qm_resp.insert("msg_err", zevisionErrorMsg((ZevisionErrorCode)c_error));
        return qm_resp;
    }
//    if(protocol == ZevisionDefault) {
//        ba_resp = response.mid(i_dataStartOffset, i_responseLen - i_dataEndOffset);
//        QStringList sl_resp = QString(ba_resp).split(";");
//        qm_resp.unite(zevisionMsgSplit(sl_resp));
//        return qm_resp;
//    }
    if(protocol == ZevisionLength || protocol == ZevisionOptional) {
        i_dataStartOffset += 2; //add 2 to start position to skip length bytes
        i_msgLength -= 2; //minus 2, message length bytes
        QByteArray ba_length = response.mid(1, 2);
        qm_resp.insert("msg_length", lengthBytesCal(ba_length));
        qm_resp.insert("msg_length_bytes", ba_length.toHex());
    }
    if(protocol == ZevisionChecksum || protocol == ZevisionOptional) {
        i_msgLength -= 1; //minus 1, checksum bytes
//        i_dataEndOffset += 2; //add 2 to end offset to skip '+' and checksum byte
        char c_chksum = response.at(i_respStatusCharPosPos + 1);
        qm_resp.insert("checksum_byte", c_chksum & 0xFF);
        qm_resp.insert("checksum", infTFCCks(response.mid(i_dataStartOffset, i_msgLength)) == (int)(c_chksum & 0xFF));
    }
    ba_resp = response.mid(i_dataStartOffset, i_msgLength - 1); //minus 1 byte to skip status char
    QStringList sl_resp = QString(ba_resp).split(";");
    qm_resp.unite(zevisionMsgSplit(sl_resp));
    return qm_resp;
}

///
/// \brief CommonHelper::getPixmap. Set icon.
/// \param name
/// \param ratio
/// \param size
/// \return QPixmap.
///
QPixmap CommonHelper::getPixmap(const QString name, const qreal ratio, const QSize size) {
    QString s_path = ":/Resource/Picture/svg/";
    const QIcon &icon = QIcon(s_path + name + ".svg");
    QPixmap pixmap = icon.pixmap(size * ratio).scaled(size * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}

///
/// \brief CommonHelper::normalErr. Error message box.
/// \param errLevel. 1: Information, 2: Warning, 3: Critical, default: NoIcon.
/// \param errorTitle
/// \param errorMsg
///
void CommonHelper::normalErr(const int errLevel, const QString errorTitle, const QString errorMsg) {
    QMessageBox box;
    box.setWindowTitle(errorTitle);
    box.setText(errorMsg + "\t\r\n");
    switch (errLevel) {
        case 1:
            box.setIcon(QMessageBox::Information);
            break;
        case 2:
            box.setIcon(QMessageBox::Warning);
            break;
        case 3:
            box.setIcon(QMessageBox::Critical);
            break;
        default:
            box.setIcon(QMessageBox::NoIcon);
            break;
    }
    box.setStandardButtons (QMessageBox::Ok);
    box.setButtonText (QMessageBox::Ok, "OK");
    box.setStyleSheet("QPushButton{ width: 50px; height: 25px;}");
    box.exec();
}

///
/// \brief CommonHelper::sigmaTfcChecksum. Sigma instrument CRC checksum calculate.
/// \param msg
/// \param is_response
/// \return QList<unsigned char>. CRC checksum value.
///
QList<unsigned char> CommonHelper::sigmaTfcChecksum(const QByteArray *msg, bool is_response) {
    const unsigned char *str = reinterpret_cast<const unsigned char *>(msg->data());
    QList<unsigned char> ql_crc_value;
    short crc = 0x0;
    int length = 0;
    if(str != nullptr) {
        length =  1 + str[1] - (is_response ? 35 : 34);
    }
    if (length > 0) {
        crc = 0x3fff;
        for (int jx = 1; jx <= length; jx++) {
            crc =  (crc ^ str[jx]);
            for (int ix = 0; ix < 8; ix++) {
                short tmpCRC = crc;
                crc = (crc >> 1);
                if ((tmpCRC & 0x1) == 1) {
                    crc = (crc ^ 0x2001);
                }
            }
            crc = crc & 0x3fff;
        }
    }
    ql_crc_value << static_cast<unsigned char>((crc & 0x7f) + 34) << static_cast<unsigned char>(((crc >> 7) & 0x7f) + 34) ;
    return ql_crc_value;
}


///
/// \brief hexStrToInt. Convert Hex string to int value.
/// \param hex_str
/// \return int.
///
int CommonHelper::hexStrToInt(QString hex_str) {
    int x ;
    sscanf_s(hex_str.toStdString().c_str(), "%x", &x);
    return x;
}


///
/// \brief strToByteArray. Convert string to byte array.
/// \param s_cmd
/// \return QByteArray.
///
QByteArray CommonHelper::strToByteArray(const QString *s_cmd) {
    QByteArray ba_cmd;
    foreach (auto s, *s_cmd) {
        ba_cmd.append(s.unicode());
    }
    ba_cmd.append('\x04');
    ba_cmd.append('\x00');
    return  ba_cmd;
}

///
/// \brief hexToBinary. Calculate string data to binary string.
/// \param QString Hex.
/// \return string, binary string.
///
QString CommonHelper::hexToBinary(const QString Hex) {
    QString str;
    int i;
    for(i = 0; i < Hex.length(); i++) {
        char buf[5];
        memset(buf, 0, 5);
        if (Hex.at(i).toLatin1() >= '0' && Hex.at(i).toLatin1() <= '9') {
            _itoa_s(Hex.at(i).toLatin1() - 48, buf, 2);
        } else if (Hex.at(i).toLatin1() >= 'A' && Hex.at(i).toLatin1() <= 'Z') {
            _itoa_s(Hex.at(i).toLatin1() - 65 + 10, buf, 2);
        } else if (Hex.at(i).toLatin1() >= 'a' && Hex.at(i).toLatin1() <= 'z') {
            _itoa_s(Hex.at(i).toLatin1() - 97 + 10, buf, 2);
        }
        if (strlen(buf) == 0) {
            str += "0000";
        }
        if (strlen(buf) == 1) {
            str += "000";
        }
        if (strlen(buf) == 2) {
            str += "00";
        }
        if (strlen(buf) == 3) {
            str += "0";
        }
        str += buf;
    }
    return str;
}

///
/// \brief byteArrayToFloat. Calculate byte array data to float value.
/// \param QByteArray data
/// \return float
///
float CommonHelper::byteArrayToFloat(QByteArray data) {
    return *reinterpret_cast<float *>(data.data());
}

///
/// \brief freqCal, float. Calculate INFICON instrument byte array data to float valule.
/// \param QByteArray data, bool is_xtc
/// \param is_xtc
/// \return float, frequency value.
///
float CommonHelper::freqCal(const QByteArray data, bool is_xtc) {
    QByteArray ba_revised = reviseData(data);
    bool ok;
    double c = is_xtc ? 0.0034924596 : 0.000873114913702011;
    return ba_revised.toHex().toLongLong(&ok, 16) * c;
}

///
/// \brief reviseData, QByteArray. Revise byte array.
/// \param QByteArray Bytes
/// \return QByteArray
///
QByteArray CommonHelper::reviseData(QByteArray Bytes) {
    QByteArray _Bytes;
    _Bytes.reserve(Bytes.length());
    for(int i = Bytes.length() - 1; i >= 0; --i) {
        _Bytes.append(Bytes.at(i));
    }
    return _Bytes;
}
