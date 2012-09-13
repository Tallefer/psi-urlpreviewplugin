CONFIG += release
include(../../psiplugin.pri)

SOURCES += \
            urlpreviewplugin.cpp \
    juickdownloader.cpp

HEADERS += \
    juickdownloader.h \
    defines.h \
    urlpreviewplugin.h

QT += network webkit

FORMS += \
    settings.ui
