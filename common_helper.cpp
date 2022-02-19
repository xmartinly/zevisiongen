#include "common_helper.h"


CommonHelper::CommonHelper(QObject *parent): QObject(parent) {
}

///
/// \brief CommonHelper::zevisionErrorMsg.
/// \param code. char
/// \return error string. QString
///
QString CommonHelper::zevisionErrorMsg(const char code) const {
//    QString s_error;
//    A = 0x41,
//    B = 0x42,
//    C = 0x43,
//    D = 0x44,
//    F = 0x46,
//    L = 0x4c,
//    T = 0x54
    QMap<char, QString> qm_errs = {
        {'A', "Illegal command, the command is not supported."},
        {'B', "Illegal parameter value"},
        {'C', "Checksum error (only when checksum is enabled)."},
        {'D', "Illegal format"},
        {'F', "State error, the system is in the wrong state."},
        {'L', "Length error (only when length is enabled)."},
        {'T', "Time out, only part of a command was received."},
    };
    bool b_msgFound = qm_errs.contains(code);
    return b_msgFound ? qm_errs.find(code).value() : "Undefined." ;
}

///
/// \brief CommonHelper::zevisionMsgSplit. Split message.
/// \param msg. QByteArray
/// \return qm_msgList. QVariantMap.
///
QVariantMap CommonHelper::zevisionMsgSplit(const QByteArray msg) {
    QVariantMap qm_msgList;
    QList<QByteArray> qlb_msg = msg.split(';');
    int qlb_length = qlb_msg.length();
    qm_msgList["resp_count"] = qlb_length;
    for(int i = 0 ; i < qlb_length; i++) {
        QByteArray ba_msg = qlb_msg.at(i);
        QVariantMap _qm_temp;
        QStringList sl_data = QString(ba_msg).replace("+", "").split(":");
        _qm_temp["resp_hex"] = ba_msg.toHex();
//        _qm_temp["resp_cmd"] = sl_data[0];
        _qm_temp["resp_data"] = sl_data[1];
        qm_msgList[sl_data[0]] = _qm_temp;
    }
    return qm_msgList;
}

///
/// \brief CommonHelper::zevisionProtocolCal. Calculate protocol status.
/// \param protocol. const int
/// \return QList.{i_isLength, i_isChksum}
///
QList<int> CommonHelper::zevisionProtocolCal(const int protocol) {
    int i_isLength = protocol % 2;
    int i_isChksum = protocol > 1 ;
    return {i_isLength, i_isChksum};
}

///
/// \brief CommonHelper::zevisionMsgtoList. Calculate message according to the given protocol.
/// \param msg. const QByteArray
/// \param protocol. const int
/// \return qsl_msg. QStringList
///
QStringList CommonHelper::zevisionMsgtoList(const QByteArray msg, const int protocol) {
// 0   "Time",
// 1   "Start Char",
// 2   "Msg Length",
// 3   "Data",
// 4   "Checksum",
// 5   "Stop Char",
// 6   "Protocol",
// 7   "Hex",
// 8   "String"
    //initialize message string list.
    QStringList qsl_msg;
    qsl_msg << "n/a" << "n/a" << "n/a" << "n/a" << "n/a" << "n/a" << "n/a" << "n/a" << "n/a";
    QString s_protocol;
    int i_msgLength = msg.length(),
        i_skipLength = 0,
        i_msgOffsetPos = 1,
        i_negCharPos = msg.indexOf('-');
    if(i_negCharPos != -1) { //Found error char '-'
        char c_error = msg.at(i_negCharPos + 1);
        qsl_msg.append(zevisionErrorMsg(c_error));
        return qsl_msg;
    }
    qsl_msg[0] = QDateTime::currentDateTime().toString("hh:mm:ss.z");
    qsl_msg[1] = msg.at(0); //start char
    qsl_msg[5] = msg.at(i_msgLength - 1); //stop char
    qsl_msg[7] = formatHexStr(msg); //hex string
    qsl_msg[8] = bytearrayToString(msg); //message string
    if(protocol == ZevisionChecksum || protocol == ZevisionOptional) { //add checksum string
        i_skipLength += 1;
        int i_chksum = msg.at(i_msgLength - 2) & 0xff;
        qsl_msg[4] = "hex: " + QString::number(i_chksum, 16) + " | int: " + QString::number(i_chksum);
        s_protocol += "@WithChecksum";
    }
    if(protocol == ZevisionLength || protocol == ZevisionOptional) { //add length string
        i_skipLength += 1;
        i_msgOffsetPos += 2;
        QByteArray ba_length = msg.mid(1, 2);
        int i_length = lengthBytesCal(ba_length);
        qsl_msg[2] = "hex: " + ba_length.toHex() + " | int: " + QString::number(i_length);
        s_protocol += "@WithLength";
    }
    qsl_msg[6] = s_protocol; //add protocol string
    qsl_msg[3] = msg.mid(i_msgOffsetPos, i_msgLength - i_msgOffsetPos - i_skipLength); //slice message data
    return qsl_msg;
}

///
/// \brief CommonHelper::zevisonRespCal. Calculate response data.
/// \param response. const QByteArray
/// \param protocol. int
/// \return qm_resp. QVariantMap.
///
QVariantMap CommonHelper::zevisonRespCal(const QByteArray response, int protocol) {
    QVariantMap qm_resp;
    QByteArray ba_resp;
    QStringList sl_resp;
    int i_responseLen = response.length();  //response length
    int i_respStatusCharNegPos = response.indexOf('-'); //find negative char
    int i_dataStartOffset = 1; //initialize data string beging offset
    int i_msgLength = i_responseLen - 2; //initialize reponse message length, minus 2 to drop '{' and '}'
    bool b_msgStatusFialed = i_respStatusCharNegPos != -1; //negative char found
    qm_resp["msg_failed"] = b_msgStatusFialed;
    qm_resp["portocol"] = protocol;
    if(b_msgStatusFialed) { // negative char found
        char c_error = response.at(i_respStatusCharNegPos + 1);
        qm_resp["msg_err_char"] = c_error;
        qm_resp["msg_err"] = zevisionErrorMsg(c_error);
        return qm_resp;
    }
    //protocol enabled length
    if(protocol == ZevisionLength || protocol == ZevisionOptional) {
        i_dataStartOffset += 2; //add 2 to start position to skip length bytes
        i_msgLength -= 2; //minus 2, message length bytes
        QByteArray ba_length = response.mid(1, 2);
        qm_resp["msg_length"] = lengthBytesCal(ba_length);
        qm_resp["msg_length_bytes"] = ba_length.toHex();
    }
    //protocol enabled checksum
    if(protocol == ZevisionChecksum || protocol == ZevisionOptional) {
        i_msgLength -= 1; //minus 1, checksum bytes
//        i_dataEndOffset += 2; //add 2 to end offset to skip '+' and checksum byte
        char c_chksum = response.at(i_responseLen - 2);
        qm_resp["checksum_byte"] = c_chksum & 0xFF;
        qm_resp["checksum"] = infTFCCks(response.mid(i_dataStartOffset, i_msgLength)) == (int)(c_chksum & 0xFF);
    }
    ba_resp = response.mid(i_dataStartOffset, i_msgLength);
    qm_resp["resp"] = zevisionMsgSplit(ba_resp);
    return qm_resp;
}

///
/// \brief CommonHelper::ZevisonCommandGenAlpha. Generate command map for a given cmd string.
/// \param cmd
/// \return
///
QMap<QString, QByteArray> CommonHelper::zevisonCommandGenAlpha(const QString *cmd, const int protocol) {
    QMap<QString, QByteArray> qm_cmd;
    QByteArray ba_cmd = cmd->toLocal8Bit(); //convert to byte array
    QByteArray ba_length = msgLengthCal(ba_cmd);
    int i_chksum = infTFCCks(ba_cmd);
    qm_cmd.insert("cmd_str", ba_cmd + '&' + '#' + QString::number(protocol).toLocal8Bit()); //insert cmd string to qmap
    if(protocol == ZevisionChecksum || protocol == ZevisionOptional) { //calculate checksum
        ba_cmd.append(i_chksum);
    }
    if(protocol == ZevisionLength || protocol == ZevisionOptional) { //calculate cmd length
        ba_cmd.prepend(ba_length);
    }
    ba_cmd.prepend('{');
    ba_cmd.append('}');
    qm_cmd.insert("cmd_treated", ba_cmd);
    return qm_cmd;
}

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
/// \brief CommonHelper::saveData
/// \param data
/// \param s_file
/// \param operate
///
void CommonHelper::saveData(const QStringList data, const QString s_file, const int operate) {
    QFile outFile(s_file);
    QString s_data = data.join(",");
    if (outFile.size () == 0 && outFile.open(QFile::WriteOnly)) {
        outFile.close();
    }
    if(outFile.open(QFile::WriteOnly | QIODevice::Append)) {
        QTextStream out(&outFile);
        out << s_data;
        outFile.close();
    }
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

///
/// \brief CommonHelper::formatHexStr. Format QByteArray to hex string with blank.
/// \param msg. const QByteArray
/// \return s_msg. QString
///
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

///
/// \brief CommonHelper::msgLengthCal. Calculate message length to QByteArray.
/// \param msg. const QByteArray
/// \return ba_length. QByteArray
///
QByteArray CommonHelper::msgLengthCal(const QByteArray msg) {
    QByteArray ba_length;
    int i_totalLength = msg.length();
    int i_highByte = floor(i_totalLength / 256);
    int i_lowByte = i_totalLength % 256;
    ba_length.append(i_highByte);
    ba_length.append(i_lowByte);
    return ba_length;
}

///
/// \brief CommonHelper::lengthBytesCal. Calculate message length from QByteArray.
/// \param length. const QByteArray
/// \return int. message length in decimal.
///
int CommonHelper::lengthBytesCal(const QByteArray length) {
    if(length.length() != 2) {
        return -1;
    }
    return (int)length.at(0) * 256 + (int)length.at(1);
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
/// \brief CommonHelper::bytearrayToString
/// \param data
/// \return
///
QString CommonHelper::bytearrayToString(const QByteArray data) {
    std::string message(data.constData(), data.length());
    return QString::fromStdString(message);
}

///
/// \brief CommonHelper::openFile
/// \param file
///
void CommonHelper::openFile(const QString file) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
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
