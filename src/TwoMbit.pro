QT += xml
QT += core
QT += gui

INCLUDEPATH += lib/
CODECFORTR = UTF-8
QMAKE_CXXFLAGS += -std=gnu++0x

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -O3 -fomit-frame-pointer
    QMAKE_CXXFLAGS += -fprofile-generate
    QMAKE_LFLAGS += -fprofile-generate
}
CONFIG(debug, debug|release) {

}
DEFINES -= UNICODE

win32-* {
    RC_FILE = images/twombit.rc
    INCLUDEPATH += $(DXSDK_DIR)/include

    LIBS += -L"$$(DXSDK_DIR)/Lib/x86/" -ldxguid
    LIBS += -L"$$(DXSDK_DIR)/Lib/x86/" -ldsound
    LIBS += -L"$$(DXSDK_DIR)/Lib/x86/" -ldinput8
    LIBS += -L"$$(DXSDK_DIR)/Lib/x86/" -ld3d9
    LIBS += -L"$$(DXSDK_DIR)/Lib/x86/" -ld3dx9

    SOURCES += \
        application/drivers/directx/dsound.cpp \
        application/drivers/directx/dinput.cpp \
        application/drivers/directx/dvideo.cpp

    HEADERS += \
        application/drivers/directx/dinput.h \
        application/drivers/directx/dsound.h \
        application/drivers/directx/dvideo.h
}
linux-* {
    QT += opengl
    LIBS += -lGL -lX11 -lSDL -lglut -lGLU
    LIBS += -lopenal
    LIBS += -lXfixes

    SOURCES += \
        application/drivers/openal/openal.cpp \
        application/drivers/x11/xinput.cpp \
        application/drivers/sdl/sinput.cpp \
        application/drivers/opengl/qtopengl.cpp

    HEADERS += \
        application/drivers/openal/openal.h \
        application/drivers/x11/xlibkeys.h \
        application/drivers/x11/xinput.h \
        application/drivers/sdl/sinput.h \
        application/drivers/opengl/qtopengl.h
}
macx-* {
    QT += opengl
    LIBS += -framework SDL -framework Cocoa
    LIBS += -framework openal
    LIBS += -framework carbon
    LIBS += -framework opengl -framework GLUT
    ICON = images/tmb.icns

    SOURCES += \
        application/drivers/openal/openal.cpp \
        application/drivers/carbon/cinput.cpp \
        application/drivers/sdl/sinput.cpp \
        application/drivers/opengl/qtopengl.cpp

    HEADERS += \
        application/drivers/openal/openal.h \
        application/drivers/carbon/cinput.h \
        application/drivers/sdl/sinput.h \
        application/drivers/opengl/qtopengl.h
}

SOURCES += \
    application/main.cpp \
    gui/mgui.cpp \
    application/app.cpp \
    application/lang.cpp \
    application/settings.cpp \
    application/config.cpp \
    gui/canvas.cpp \
    gui/cgui.cpp \
    application/recent.cpp \
    application/resolution.cpp \
    application/emulator.cpp \
    application/file.cpp \
    lib/bzip2/randtable.c \
    lib/bzip2/huffman.c \
    lib/bzip2/decompress.c \
    lib/bzip2/crctable.c \
    lib/bzip2/compress.c \
    lib/bzip2/bzlib.c \
    lib/bzip2/blocksort.c \
    lib/7z_C/LzmaDec.c \
    lib/7z_C/Bra86.c \
    lib/7z_C/Bcj2.c \
    lib/7z_C/7zStream.c \
    lib/7z_C/7zItem.c \
    lib/7z_C/7zIn.c \
    lib/7z_C/7zHeader.c \
    lib/7z_C/7zExtract.c \
    lib/7z_C/7zDecode.c \
    lib/7z_C/7zCrc.c \
    lib/7z_C/7zBuf.c \
    lib/7z_C/7zAlloc.c \
    lib/zlib/zutil.c \
    lib/zlib/inftrees.c \
    lib/zlib/inflate.c \
    lib/zlib/inffast.c \
    lib/zlib/crc32.c \
    lib/zlib/adler32.c \
    lib/fex/Binary_Extractor.cpp \
    lib/fex/blargg_common.cpp \
    lib/fex/blargg_errors.cpp \
    lib/fex/Data_Reader.cpp \
    lib/fex/fex.cpp \
    lib/fex/File_Extractor.cpp \
    lib/fex/Gzip_Extractor.cpp \
    lib/fex/Gzip_Reader.cpp \
    lib/fex/Rar_Extractor.cpp \
    lib/fex/Zip_Extractor.cpp \
    lib/fex/Zip7_Extractor.cpp \
    lib/fex/Zlib_Inflater.cpp \
    lib/unrar/unrar_open.cpp \
    lib/unrar/unrar_misc.cpp \
    lib/unrar/unrar.cpp \
    lib/unrar/unpack20.cpp \
    lib/unrar/unpack15.cpp \
    lib/unrar/unpack.cpp \
    lib/unrar/unicode.cpp \
    lib/unrar/suballoc.cpp \
    lib/unrar/rawread.cpp \
    lib/unrar/rarvmtbl.cpp \
    lib/unrar/rarvm.cpp \
    lib/unrar/model.cpp \
    lib/unrar/getbits.cpp \
    lib/unrar/extract.cpp \
    lib/unrar/encname.cpp \
    lib/unrar/crc.cpp \
    lib/unrar/coder.cpp \
    lib/unrar/arcread.cpp \
    lib/unrar/archive.cpp \
    sms/apu/sn76489.cpp \
    sms/bus/bus.cpp \
    sms/cart/cart.cpp \
    sms/cpu/cpu.cpp \
    sms/gear2gear/gear2gear.cpp \
    sms/input/input.cpp \
    sms/system/system.cpp \
    sms/vdp/vdp.cpp \
    sms/vdp/mode4.cpp \
    sms/vdp/mode2.cpp \
    sms/vdp/mode0.cpp \
    lib/libco/x86.c \
    lib/libco/sjlj.c \
    lib/libco/ppc.c \
    lib/libco/libco.c \
    lib/libco/fiber.c \
    lib/libco/amd64.c \
    lib/blip_buffer/Blip_Buffer.cpp \
    gui/config/inputGui.cpp \
    lib/hashlibpp/hl_md5.cpp \
    lib/sms_ntsc/sms_ntsc.c \
    gui/config/audioGui.cpp \
    gui/config/cheatsGui.cpp \
    gui/config/optionsGui.cpp \
    gui/config/pathsGui.cpp \
    gui/config/videoGui.cpp \
    libsms/libsms.cpp \
    application/colorInterface/ntsc.cpp \
    application/colorInterface/direct.cpp \
    application/colorInterface/color.cpp \
    application/inputmanager/inputmanager.cpp \
    lib/string/qstring.cpp \
    sms/apu/ym2413.cpp \
    sms/apu/ym2413/ym2413Core.cpp \
    sms/apu/ym2413/ym2413Core_update.cpp \
    sms/apu/ym2413/ym2413Core_tables.cpp \
    application/resample/resampler.cpp \
    sms/bus/eeprom_93c46.cpp \
    application/geometry.cpp \
    sms/cheats/cheats.cpp \
    application/cheatshandler.cpp \
    sms/cpu/cpu_serialization.cpp \
    sms/vdp/vdp_serialization.cpp \
    sms/apu/sn_serialization.cpp \
    sms/apu/ym_serialization.cpp \
    sms/apu/ym2413/ymcore_serialization.cpp \
    sms/bus/bus_serialization.cpp \
    sms/bus/eeprom_serialization.cpp \
    sms/input/input_serialization.cpp \
    sms/system/system_serialization.cpp \
    application/statehandler.cpp \
    chips/z80/z80_serialization.cpp \
    chips/z80/z80.cpp \
    #chips/68k/opcodes.cpp \
    #chips/68k/logic.cpp \
    #chips/68k/68k.cpp


HEADERS += \
    application/main.h \
    application/app.h \
    application/exception.h \
    application/dialog.h \
    application/lang.h \
    application/settings.h \
    application/config.h \
    gui/canvas.h \
    gui/mgui.h \
    gui/cgui.h \
    application/recent.h \
    application/resolution.h \
    application/emulator.h \
    application/file.h \
    lib/bzip2/bzlib.h \
    lib/bzip2/bzlib_private.h \
    lib/helper.h \
    lib/7z_C/Types.h \
    lib/7z_C/LzmaDec.h \
    lib/7z_C/CpuArch.h \
    lib/7z_C/Bra.h \
    lib/7z_C/Bcj2.h \
    lib/7z_C/7zItem.h \
    lib/7z_C/7zIn.h \
    lib/7z_C/7zHeader.h \
    lib/7z_C/7zExtract.h \
    lib/7z_C/7zDecode.h \
    lib/7z_C/7zCrc.h \
    lib/7z_C/7zBuf.h \
    lib/7z_C/7zAlloc.h \
    lib/zlib/zutil.h \
    lib/zlib/zlib.h \
    lib/zlib/zconf.h \
    lib/zlib/inftrees.h \
    lib/zlib/inflate.h \
    lib/zlib/inffixed.h \
    lib/zlib/inffast.h \
    lib/zlib/crc32.h \
    lib/fex/Binary_Extractor.h \
    lib/fex/blargg_common.h \
    lib/fex/blargg_config.h \
    lib/fex/blargg_endian.h \
    lib/fex/blargg_errors.h \
    lib/fex/blargg_source.h \
    lib/fex/Data_Reader.h \
    lib/fex/fex.h \
    lib/fex/File_Extractor.h \
    lib/fex/Gzip_Extractor.h \
    lib/fex/Gzip_Reader.h \
    lib/fex/Rar_Extractor.h \
    lib/fex/Zip_Extractor.h \
    lib/fex/Zip7_Extractor.h \
    lib/fex/Zlib_Inflater.h \
    lib/unrar/unrar.h \
    lib/unrar/unpack.hpp \
    lib/unrar/unicode.hpp \
    lib/unrar/suballoc.hpp \
    lib/unrar/rawread.hpp \
    lib/unrar/rarvm.hpp \
    lib/unrar/rar.hpp \
    lib/unrar/model.hpp \
    lib/unrar/headers.hpp \
    lib/unrar/getbits.hpp \
    lib/unrar/encname.hpp \
    lib/unrar/compress.hpp \
    lib/unrar/coder.hpp \
    lib/unrar/array.hpp \
    lib/unrar/archive.hpp \
    sms/apu/sn76489.h \
    sms/bus/bus.h \
    sms/cart/cart.h \
    sms/cpu/cpu.h \
    sms/gear2gear/gear2gear.h \
    sms/input/input.h \
    sms/system/system.h \
    sms/system/interface.h \
    sms/vdp/vdp.h \
    sms/processor.h \
    sms/mem.h \
    lib/dataTypes.h \
    lib/libco/libco.h \
    lib/blip_buffer/Blip_Buffer.h \
    gui/config/inputGui.h \
    lib/hashlibpp/hl_types.h \
    lib/hashlibpp/hl_md5.h \
    lib/sms_ntsc/sms_ntsc.h \
    lib/sms_ntsc/sms_ntsc_config.h \
    lib/sms_ntsc/sms_ntsc_impl.h \
    gui/config/audioGui.h \
    gui/config/cheatsGui.h \
    gui/config/optionsGui.h \
    gui/config/pathsGui.h \
    gui/config/videoGui.h \
    libsms/libsms_global.h \
    libsms/libsms.h \
    application/colorInterface/color.h \
    application/inputmanager/inputmanager.h \
    application/inputmanager/inputmap.h \
    lib/string/qstring.h \
    sms/apu/ym2413.h \
    sms/apu/ym2413/ym2413Core.h \
    application/resample/resampler.h \
    sms/bus/eeprom_93c46.h \
    application/geometry.h \
    sms/cheats/cheats.h \
    application/cheatshandler.h \
    lib/string/_string.h \
    lib/serializer/serializer.h \
    application/statehandler.h \
    application/drivers/drvvideo.h \
    application/drivers/drvaudio.h \
    application/drivers/drvinput.h \
    chips/z80/z80.h \
    chips/z80/regs.h \
    chips/z80/opcodes.t.h \
    chips/z80/mem.h \
    #chips/68k/regs.h \
    #chips/68k/mem.h \
    #chips/68k/68k.h

OTHER_FILES += \
    language/german.xml \
    language/english.xml \
    images/unload.png \
    images/uncheck.png \
    images/tmb.ico \
    images/sun.png \
    images/speed.png \
    images/screen.png \
    images/save_state.png \
    images/reset.png \
    images/region.png \
    images/recent.png \
    images/power.png \
    images/options.png \
    images/load_state.png \
    images/load.png \
    images/joypad.png \
    images/folder.png \
    images/filter.png \
    images/exit.png \
    images/clear.png \
    images/check.png \
    images/cheat.png \
    images/change_slot.png \
    images/audio.png \
    images/arrow.png \
    images/about.png


RESOURCES += \
    images/icons.qrc
