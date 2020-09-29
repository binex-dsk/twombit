#-------------------------------------------------
#
# Project created by QtCreator 2012-02-22T22:15:03
#
#-------------------------------------------------

QT       -= core gui

TARGET = libsms
TEMPLATE = lib

DEFINES += LIBSMS_LIBRARY

INCLUDEPATH += ../lib/

SOURCES += libsms.cpp \
    ../lib/libco/x86.c \
    ../lib/libco/sjlj.c \
    ../lib/libco/ppc.c \
    ../lib/libco/libco.c \
    ../lib/libco/fiber.c \
    ../lib/libco/amd64.c \
    ../lib/blip_buffer/Blip_Buffer.cpp \
    ../lib/hashlibpp/hl_md5.cpp \
    ../sms/apu/ym2413.cpp \
    ../sms/apu/sn76489.cpp \
    ../sms/apu/ym2413/ym2413Core_update.cpp \
    ../sms/apu/ym2413/ym2413Core_tables.cpp \
    ../sms/apu/ym2413/ym2413Core.cpp \
    ../sms/bus/bus.cpp \
    ../sms/cart/cart.cpp \
    ../sms/cpu/cpu.cpp \
    ../sms/cpu/core/z80.cpp \
    ../sms/gear2gear/gear2gear.cpp \
    ../sms/input/input.cpp \
    ../sms/system/system.cpp \
    ../sms/vdp/vdp.cpp \
    ../sms/vdp/mode4.cpp \
    ../sms/vdp/mode2.cpp \
    ../sms/vdp/mode0.cpp \
    ../sms/bus/eeprom_93c46.cpp

HEADERS += libsms.h\
        libsms_global.h \
    ../lib/libco/libco.h \
    ../lib/blip_buffer/Blip_Buffer.h \
    ../lib/hashlibpp/hl_types.h \
    ../lib/hashlibpp/hl_md5.h \
    ../sms/apu/ym2413.h \
    ../sms/apu/sn76489.h \
    ../sms/apu/ym2413/ym2413Core.h \
    ../sms/bus/bus.h \
    ../sms/cart/cart.h \
    ../sms/cpu/cpu.h \
    ../sms/cpu/core/z80.h \
    ../sms/cpu/core/regs.h \
    ../sms/cpu/core/opcodes.t.h \
    ../sms/cpu/core/mem.h \
    ../sms/gear2gear/gear2gear.h \
    ../sms/input/input.h \
    ../sms/system/system.h \
    ../sms/system/interface.h \
    ../sms/vdp/vdp.h \
    ../sms/processor.h \
    ../sms/mem.h \
    ../lib/helper.h \
    ../lib/dataTypes.h \
    ../sms/bus/eeprom_93c46.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1F181BD
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = libsms.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
