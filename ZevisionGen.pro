QT       += core gui serialport network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about_dialog.cpp \
    comm_setup_dialog.cpp \
    common_helper.cpp \
    data_log_dialog.cpp \
    inficon_instrument.cpp \
    main.cpp \
    qcustomplot.cpp \
    serial_comm_singleton.cpp \
    zevision_gen.cpp

HEADERS += \
    about_dialog.h \
    comm_setup_dialog.h \
    common_helper.h \
    data_log_dialog.h \
    inficon_instrument.h \
    qcustomplot.h \
    serial_comm_singleton.h \
    zevision_gen.h

FORMS += \
    about_dialog.ui \
    comm_setup_dialog.ui \
    data_log_dialog.ui \
    zevision_gen.ui

TRANSLATIONS += \
    ZevisionGen_zh_CN.ts \
    ZevisionGen_en_US.ts

CODECFORTR = UTF-8

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

RC_ICONS = tuning_fork.ico
