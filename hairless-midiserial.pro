#-------------------------------------------------
#
# Project created by QtCreator 2011-08-08T17:57:42
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = hairless-midiserial
TEMPLATE = app

DEFINES +=APPNAME=\"hairless-midiserial\"

DEFINES += VERSION=\\\"0.6.0\\\"

# Main Program

SOURCES += \
    src/LuaMidiInOut.cpp \
    src/QMidiDebug.cpp \
    src/QRtMidiInOut.cpp \
    src/QRtMidiOut.cpp \
    src/QSerialMidiInOut.cpp \
    src/main.cpp \
    src/BlinkenLight.cpp \
    src/ui/BridgeHead.cpp \
    src/ui/MidiFrame.cpp \
    src/ui/SerialFrame.cpp \
    src/ui/mainwindow.cpp \
    src/ui/aboutdialog.cpp \
    src/ui/portsettingsdialog.cpp

HEADERS  += \
    src/LuaMidiInOut.h \
    src/QMidi.h \
    src/QMidiDebug.h \
    src/QRtMidiInOut.h \
    src/QRtMidiOut.h \
    src/QSerialMidiInOut.h \
    src/Settings.h \
    src/BlinkenLight.h \
    src/ui/BridgeHead.h \
    src/ui/MidiFrame.h \
    src/ui/SerialFrame.h \
    src/ui/commons.h \
    src/ui/mainwindow.h \
    src/ui/aboutdialog.h \
    src/ui/portsettingsdialog.h

FORMS += src/ui/mainwindow.ui \
    src/ui/BridgeHead.ui \
    src/ui/MidiFrame.ui \
    src/ui/SerialFrame.ui \
    src/ui/settingsdialog.ui \
    src/ui/aboutdialog.ui

INCLUDEPATH += src/

# Universal binary for OS X

macx {
    CONFIG += ppc x86_64
}


# Static flags for windows

win32 {
  QMAKE_LFLAGS += -static -static-libgcc
}

# Icon stuff

win32 {
  RC_FILE = "images/icon.rc"
}
macx {
  ICON = "images/icon.icns"
}
# linux has no baked-in icon support, but we load a resource to use as a window icon

# qextserialport

HEADERS                 += libraries/qextserialport/src/qextserialport.h \
                          libraries/qextserialport/src/qextserialenumerator.h \
                          libraries/qextserialport/src/qextserialport_global.h
SOURCES                 += libraries/qextserialport/src/qextserialport.cpp \
                           libraries/qextserialport/src/qextserialenumerator.cpp

unix:SOURCES           += libraries/qextserialport/src/qextserialport_unix.cpp
unix:!macx:SOURCES     += libraries/qextserialport/src/qextserialenumerator_linux.cpp
unix:!macx:LIBS        += -ludev

macx {
  SOURCES          += libraries/qextserialport/src/qextserialenumerator_osx.cpp
  LIBS             += -framework IOKit -framework CoreFoundation
}

win32 {
  SOURCES          += libraries/qextserialport/src/win_qextserialport.cpp \
                      libraries/qextserialport/src/qextserialenumerator_win.cpp
  DEFINES          += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
  LIBS             += -lsetupapi
}

INCLUDEPATH += libraries/qextserialport/src/

# RtMidi

HEADERS +=    libraries/rtmidi/RtMidi.h \
              src/QRtMidiIn.h \

SOURCES +=    libraries/rtmidi/RtMidi.cpp \
              src/QRtMidiIn.cpp

INCLUDEPATH += libraries/rtmidi/

linux-* { # linux doesn't get picked up, not sure what else to use
  DEFINES += __LINUX_ALSASEQ__ __LINUX_ALSA__
  CONFIG += link_pkgconfig x11
  PKGCONFIG += alsa
  LIBS += -lpthread
}
win32 {
  DEFINES += __WINDOWS_MM__
  LIBS += -lwinmm
}
macx {
    DEFINES += __MACOSX_CORE__
    LIBS += -framework \
        CoreMidi \
        -framework \
        CoreAudio \
        -framework \
        CoreFoundation
}

# PortLatency
SOURCES += src/PortLatency.cpp
HEADERS += src/PortLatency.h
linux-* {
   SOURCES += src/PortLatency_linux.cpp
}
win32 {
   SOURCES += src/PortLatency_win32.cpp
}
macx {
   SOURCES += src/PortLatency_osx.cpp
}

# LUA
SOURCES += libraries/lua/lapi.c libraries/lua/lauxlib.c libraries/lua/lbaselib.c libraries/lua/lcode.c libraries/lua/lcorolib.c libraries/lua/lctype.c libraries/lua/ldblib.c libraries/lua/ldebug.c libraries/lua/ldo.c libraries/lua/ldump.c libraries/lua/lfunc.c libraries/lua/lgc.c libraries/lua/linit.c libraries/lua/liolib.c libraries/lua/llex.c libraries/lua/lmathlib.c libraries/lua/lmem.c libraries/lua/loadlib.c libraries/lua/lobject.c libraries/lua/lopcodes.c libraries/lua/loslib.c libraries/lua/lparser.c libraries/lua/lstate.c libraries/lua/lstring.c libraries/lua/lstrlib.c libraries/lua/ltable.c libraries/lua/ltablib.c libraries/lua/ltests.c libraries/lua/ltm.c libraries/lua/lundump.c libraries/lua/lutf8lib.c libraries/lua/lvm.c libraries/lua/lzio.c

HEADERS += libraries/lua/lapi.h libraries/lua/lauxlib.h libraries/lua/lcode.h libraries/lua/lctype.h libraries/lua/ldebug.h libraries/lua/ldo.h libraries/lua/lfunc.h libraries/lua/lgc.h libraries/lua/ljumptab.h libraries/lua/llex.h libraries/lua/llimits.h libraries/lua/lmem.h libraries/lua/lobject.h libraries/lua/lopcodes.h libraries/lua/lopnames.h libraries/lua/lparser.h libraries/lua/lprefix.h libraries/lua/lstate.h libraries/lua/lstring.h libraries/lua/ltable.h libraries/lua/ltests.h libraries/lua/ltm.h libraries/lua/luaconf.h libraries/lua/lualib.h libraries/lua/lundump.h libraries/lua/lvm.h libraries/lua/lzio.h

HEADERS += libraries/sol2/single/include/sol/config.hpp  libraries/sol2/single/include/sol/forward.hpp  libraries/sol2/single/include/sol/sol.hpp
INCLUDEPATH += libraries/sol2/single/include/

# Resources

RESOURCES += \
    resources.qrc
