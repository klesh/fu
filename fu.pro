#-------------------------------------------------
#
# Project created by QtCreator 2019-04-29T21:23:38
#
#-------------------------------------------------

QT       += core sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fu
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# app version auto-updationg
APP_VERSION = $$system(cd \""$$_PRO_FILE_PWD_"\" && git rev-parse --abbrev-ref HEAD)
DEFINES += APP_VERSION=\"\\\"$${APP_VERSION}\\\"\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        src/aboutdialog.cpp \
        src/components/errormessage.cpp \
        src/components/flowlayout.cpp \
        src/components/folderpicker.cpp \
        src/components/previewbox.cpp \
        src/components/tagbutton.cpp \
        src/components/tagsedit.cpp \
        src/components/thumbnaillabel.cpp \
        src/configdialog.cpp \
        src/core/error.cpp \
        src/core/outputformatservice.cpp \
        src/core/serverservice.cpp \
        src/core/settingservice.cpp \
        src/core/tagservice.cpp \
        src/formdialog.cpp \
        src/historywindow.cpp \
        src/application.cpp \
        src/main.cpp \
        src/protocols/ftpprotocol.cpp \
        src/protocols/localstorageprotocol.cpp \
        src/store/migration.cpp \
        src/store/migrations/migrationv0.cpp \
        src/store/migrator.cpp \
        src/store/sqlstore.cpp \
        src/upgradedialog.cpp

HEADERS += \
        src/aboutdialog.h \
        src/components/errormessage.h \
        src/components/flowlayout.h \
        src/components/folderpicker.h \
        src/components/previewbox.h \
        src/components/tagbutton.h \
        src/components/tagsedit.h \
        src/components/thumbnaillabel.h \
        src/configdialog.h \
        src/core/error.h \
        src/core/outputformatservice.h \
        src/core/serverservice.h \
        src/core/settingservice.h \
        src/core/tagservice.h \
        src/historywindow.h \
        src/application.h \
        src/models/outputformat.h \
        src/models/protocol.h \
        src/models/record.h \
        src/models/server.h \
        src/models/tag.h \
        src/protocols/ftpprotocol.h \
        src/protocols/localstorageprotocol.h \
        src/store/migration.h \
        src/store/migrations/migrationv0.h \
        src/store/migrator.h \
        src/store/sqlstore.h \
        src/upgradedialog.h \
        src/uploaddialog.h

FORMS += \
        src/aboutdialog.ui \
        src/configdialog.ui \
        src/historywindow.ui \
        src/upgradedialog.ui \
        src/uploaddialog.ui

RESOURCES += \
        fu.qrc

TRANSLATIONS = \
        i18n/fu.zh_cn.ts \
        i18n/fu.en_us.ts

win32 {
    RC_FILE = platforms/win32/fu.rc
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

