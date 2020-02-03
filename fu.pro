#-------------------------------------------------
#
# Project created by QtCreator 2019-04-29T21:23:38
#
#-------------------------------------------------

QT       += core sql network
#QMAKE_CXXFLAGS += -fopenmp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fu
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# app version auto-updationg
VERSION = $$system(cd \""$$_PRO_FILE_PWD_"\" && git rev-parse --abbrev-ref HEAD)
DEFINES += APP_VERSION=\\\"$${VERSION}\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        src/aboutdialog.cpp \
        src/components/errormessage.cpp \
        src/components/flowlayout.cpp \
        src/components/pathpicker.cpp \
        src/components/previewbox.cpp \
        src/components/tagbutton.cpp \
        src/components/tagsedit.cpp \
        src/components/thumbnaillabel.cpp \
        src/configdialog.cpp \
        src/core/clipservice.cpp \
        src/core/formatservice.cpp \
        src/core/serverservice.cpp \
        src/core/settingservice.cpp \
        src/core/tagservice.cpp \
        src/core/uploadservice.cpp \
        src/core/uploadthread.cpp \
        src/historywindow.cpp \
        src/application.cpp \
        src/main.cpp \
        src/protocols/ftpprotocol.cpp \
        src/protocols/localstorageprotocol.cpp \
        src/protocols/imgurprotocol.cpp \
        src/protocols/qiniuprotocol.cpp \
        src/protocols/sftpprotocol.cpp \
        src/runguard.cpp \
        src/store/migrations/migrationv0.cpp \
        src/store/migrator.cpp \
        src/upgradedialog.cpp \
        src/uploaddialog.cpp

HEADERS += \
        src/aboutdialog.h \
        src/components/errormessage.h \
        src/components/flowlayout.h \
        src/components/focuswatcher.h \
        src/components/pathpicker.h \
        src/components/previewbox.h \
        src/components/tagbutton.h \
        src/components/tagsedit.h \
        src/components/thumbnaillabel.h \
        src/configdialog.h \
        src/core/clipservice.h \
        src/core/formatservice.h \
        src/core/serverservice.h \
        src/core/settingservice.h \
        src/core/tagservice.h \
        src/core/uploadservice.h \
        src/core/uploadthread.h \
        src/core/utils.h \
        src/historywindow.h \
        src/application.h \
        src/models/clip.h \
        src/models/format.h \
        src/models/server.h \
        src/models/tag.h \
        src/models/upload.h \
        src/protocols/base.h \
        src/protocols/ftpprotocol.h \
        src/protocols/localstorageprotocol.h \
        src/protocols/imgurprotocol.h \
        src/protocols/qiniuprotocol.h \
        src/protocols/sftpprotocol.h \
        src/runguard.h \
        src/store/migration.h \
        src/store/migrations/migrationv0.h \
        src/store/migrator.h \
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

TRANSLATIONS_FILES =
LANGS =
qtPrepareTool(LRELEASE, lrelease)
for(tsfile, TRANSLATIONS) {
    qmfile = $$shadowed($$tsfile)
    qmfile ~= s,.ts$,.qm,
    qmdir = $$dirname(qmfile)
    LANGS = $$LANGS,$$basename(qmfile)
    !exists($$qmdir) {
        mkpath($$qmdir)|error("Aborting.")
    }
    command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile
    system($$command)|error("Failed to run: $$command")
    TRANSLATIONS_FILES += $$qmfile
}

DEFINES += LANGS=\\\"$${LANGS}\\\"
CONFIG(release, debug|release) {
    BUILD = release
}
CONFIG(debug, debug|release) {
    BUILD = debug
}
msvc:COMPILER=msvc
gcc:COMPILER=gcc

ROOTDIR = $$PWD/..
DEPSDIR = $$ROOTDIR/deps/$$QT_ARCH/$$BUILD
DESTDIR = $$ROOTDIR/builds/fu-$$COMPILER-$$QT_ARCH-$$BUILD-$$VERSION

win32 {
    RC_FILE = platforms/win32/fu.rc

    INCLUDEPATH += $$DEPSDIR/include
    LIBS += -L$$DEPSDIR/bin -lqcurl1

    dlls.path = $$DESTDIR
    dlls.files = $$DEPSDIR/bin/*.dll
    INSTALLS += dlls

    langs.path = $$DESTDIR/i18n
    langs.files = $$qmdir/*.qm
    INSTALLS += langs
}

unix {
    LIBS +=  -lqcurl
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

