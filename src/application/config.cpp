
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "config.h"
#include "settings.h"
#include <string/qstring.h>
#include "resolution.h"
#include "main.h"

bool Config::load(QString path) {
    QStringList line;
    QFile f( path );
    if(!f.open( QIODevice::ReadOnly )) return false;

    QTextStream in( &f );
    QString buffer;

    while ( !(buffer = in.readLine()).isNull() ) {
        buffer.replace( "\r\n", "\n" );
        buffer.replace( "\t", "" );

        if ( (buffer.length() == 0) || (buffer.at(0) == ';') ) continue;

        line = buffer.split("=");

        if (line.size() < 2) continue;

        line[1] = line[1].trimmed();
        line[0].replace( " ", "" );

        __QString::remove_quote(line[0]);
        __QString::remove_quote(line[1]);

        for (unsigned z = 0; z < Settings::vector_pos; z++) {
            if (QString::compare(line[0], (*Settings::list[z]).name, Qt::CaseSensitive) == 0) {
                if ((*Settings::list[z]).type != Settings::STR) {
                    line[1].replace( " ", "" );
                    (*Settings::list[z]) = prepareIntSetting(Settings::list[z], line[1]);
                }
                else {
                    (*Settings::list[z]) = prepareStringSetting(Settings::list[z], line[1]);
                }
            }
        }
        line.clear();
    }
    f.close();
    return true;
}

QString Config::prepareStringSetting(Settings* setting, QString in) {
    if ( Resolution::isRes(setting->str_init) ) {
        if ( Resolution::isRes(in) ) {
            return in;
        }
        return setting->str_init;
    }
    return in;
}

int Config::prepareIntSetting(Settings* setting, QString in) {
    bool error = false;
    int result = __QString::string_to_int((__QString::Type)setting->type, in, error);
    if (error) {
        return setting->init;
    }
    if (setting->type == Settings::DEC) {
         if (result < setting->minLimit) return setting->init;
         if (result > setting->maxLimit) return setting->init;
    }
    return result;
}

bool Config::save(QString path) {
    QStringList lines;
    QFile f( path );
    QString data;

    if(!f.open( QIODevice::WriteOnly )) return false;

    QTextStream out(&f);

    for (unsigned z = 0; z < Settings::vector_pos; z++) {
        if (!(*Settings::list[z]).saveable) continue;

        data = (*Settings::list[z]).desc;

        lines = data.split("\n");

        out << ";--------------------------------------------------" << endl;

        for (int i = 0; i < lines.size(); i++) {
            out << "; " + lines[i] << endl;
        }
        lines.clear();

        if ( (*Settings::list[z]).type != Settings::STR) {
            out << "; default = " + __QString::int_to_string((__QString::Type)(*Settings::list[z]).type, (*Settings::list[z]).init) << endl;
            out << ";--------------------------------------------------" << endl;
            out << (*Settings::list[z]).name + " = " + __QString::int_to_string((__QString::Type)(*Settings::list[z]).type, (*Settings::list[z]).data) + "\n" << endl;
        } else {
            out << "; default = " + (*Settings::list[z]).str_init << endl;
            out << ";--------------------------------------------------" << endl;
            out << (*Settings::list[z]).name + " = " + (*Settings::list[z]).str_data + "\n" << endl;
        }
    }
    f.close();
    return true;
}

Config::Config()
{
    initSettings();
}

void Config::initSettings() {
    new Settings("save_slot", 0);
    new Settings("video_mode_window", "window size at startup", 0, Settings::DEC, 0, 9);
    new Settings("video_mode_fullscreen", "active fullscreen resolution", 0, Settings::DEC, 0, 9);

    new Settings("window_mode_0", "window mode 0", "320x240");
    new Settings("window_mode_1", "window mode 1", "480x360");
    new Settings("window_mode_2", "window mode 2", "640x480");
    new Settings("window_mode_3", "window mode 3", "720x576");
    new Settings("window_mode_4", "window mode 4", "800x600");
    new Settings("window_mode_5", "window mode 5", "960x720");
    new Settings("window_mode_6", "window mode 6", "1120x840");
    new Settings("window_mode_7", "window mode 7", "1280x960");
    new Settings("window_mode_8", "window mode 8", "1440x1080");
    new Settings("window_mode_9", "window mode 9", "1600x1200");

    new Settings("fullscreen_mode_0", "fullscreen mode 0", "800x600");
    new Settings("fullscreen_mode_1", "fullscreen mode 1", "1024x768");
    new Settings("fullscreen_mode_2", "fullscreen mode 2", "1280x800");
    new Settings("fullscreen_mode_3", "fullscreen mode 3", "1280x1024");
    new Settings("fullscreen_mode_4", "fullscreen mode 4", "1440x900");
    new Settings("fullscreen_mode_5", "fullscreen mode 5", "1680x1050");
    new Settings("fullscreen_mode_6", "fullscreen mode 6", "1366x768");
    new Settings("fullscreen_mode_7", "fullscreen mode 7", "1152x864");
    new Settings("fullscreen_mode_8", "fullscreen mode 8", "1920x1080");
    new Settings("fullscreen_mode_9", "fullscreen mode 9", "1920x1200");

    new Settings("video_vsync", "Use Vsync", 0, Settings::BIN);
    new Settings("video_aspect_correct", "Force correct aspect ratio", 0, Settings::BIN);

    new Settings("sms_audio_frequency",
               "Adjust sms audio frequency\n"
               " -300 to 300",
               0, Settings::DEC, -300, 300);

    new Settings("audio_volume",
               "Adjust audio volume",
               50, Settings::DEC, 0, 100);

    new Settings(  "video_region",
                "Select video region usage\n"
                " 0 = usa\n"
                " 1 = japanese\n"
                " 2 = europe\n"
                " 3 = auto",
                3, Settings::DEC, 0, 3);

    new Settings("video_filter_hw",
               "Select hardware filter for video\n"
               " 0 = Point\n"
               " 1 = Linear",
               0, Settings::DEC, 0, 1);

    new Settings("video_filter_sw",
               "Select software filter for video\n"
               " 0 = Normal (no filter)\n"
               " 1 = NTSC",
               0, Settings::DEC, 0, 1);

    new Settings("ntsc_filter",
                   "Select ntsc filter output\n"
                   " 0 = Composite TV cable\n"
                   " 1 = SVideo TV cable\n"
                   " 2 = RGB TV cable",
                   0, Settings::DEC, 0, 2);

    new Settings("video_scanlines", "Enable Scanlines", 0, Settings::BIN);

    new Settings("video_scanline_intensity", "Set Scanline intensity in %", 0, Settings::DEC, 0, 100);

    new Settings("video_contrast",
                   "Set Contrast\n"
                   " range -100 - 100\n"
                   " < 0 = lower contrast\n"
                   " > 0 = higher contrast",
                   0, Settings::DEC, -100, 100);

    new Settings("video_brightness",
                   "Set Brightness\n"
                   " range -100 - 100\n"
                   " < 0 = darker image\n"
                   " > 0 = brighter image",
                   0, Settings::DEC, -100, 100);

    new Settings("video_gamma",
                   "Set Gamme\n"
                   " range 0.5 - 3.5\n"
                   " multiply value with 100",
                   100, Settings::DEC, 50, 350);

    new Settings("audio_active", "Enable audio output", 1, Settings::BIN);

    new Settings("show_fps", "Show framerate bottom right", 1, Settings::BIN);

    new Settings("speed_select", 0);

    new Settings("speed_0", 100);
    new Settings("speed_1", "custom speed in %", 50, Settings::DEC, 1, 500);
    new Settings("speed_2", "custom speed in %", 75, Settings::DEC, 1, 500);
    new Settings("speed_3", "custom speed in %", 150, Settings::DEC, 1, 500);
    new Settings("speed_4", "custom speed in %", 200, Settings::DEC, 1, 500);

    new Settings("language_file", "setup a language file\n"
        "all language files must be stored in a folder named language, relative to the emu path", _LANG_FILE);

    new Settings("sms_joypad_1_up", "Joypad1 up", 0x0000, Settings::HEX);
    new Settings("sms_joypad_1_down", "Joypad1 down", 0x0000, Settings::HEX);
    new Settings("sms_joypad_1_left", "Joypad1 left", 0x0000, Settings::HEX);
    new Settings("sms_joypad_1_right", "Joypad1 right", 0x0000, Settings::HEX);
    new Settings("sms_joypad_1_a", "Joypad1 A", 0x0000, Settings::HEX);
    new Settings("sms_joypad_1_b", "Joypad1 B", 0x0000, Settings::HEX);
    new Settings("sms_joypad_1_start", "Joypad1 Start", 0x0000, Settings::HEX);

    new Settings("sms_joypad_2_up", "Joypad2 up", 0x0000, Settings::HEX);
    new Settings("sms_joypad_2_down", "Joypad2 down", 0x0000, Settings::HEX);
    new Settings("sms_joypad_2_left", "Joypad2 left", 0x0000, Settings::HEX);
    new Settings("sms_joypad_2_right", "Joypad2 right", 0x0000, Settings::HEX);
    new Settings("sms_joypad_2_a", "Joypad2 A", 0x0000, Settings::HEX);
    new Settings("sms_joypad_2_b", "Joypad2 B", 0x0000, Settings::HEX);
    new Settings("sms_joypad_2_start", "Joypad2 Start", 0x0000, Settings::HEX);

    new Settings("sms_lightgun_1_x", "Lightgun1 x Axis", 0x0000, Settings::HEX);
    new Settings("sms_lightgun_1_y", "Lightgun1 y Axis", 0x0000, Settings::HEX);
    new Settings("sms_lightgun_1_trigger", "Lightgun1 trigger", 0x0000, Settings::HEX);

    new Settings("sms_lightgun_2_x", "Lightgun2 x Axis", 0x0000, Settings::HEX);
    new Settings("sms_lightgun_2_y", "Lightgun2 y Axis", 0x0000, Settings::HEX);
    new Settings("sms_lightgun_2_trigger", "Lightgun2 trigger", 0x0000, Settings::HEX);

    new Settings("sms_terebi_1_x", "Terebi Oekaki x Axis", 0x0000, Settings::HEX);
    new Settings("sms_terebi_1_y", "Terebi Oekaki y Axis", 0x0000, Settings::HEX);
    new Settings("sms_terebi_1_trigger", "Terebi Oekaki trigger", 0x0000, Settings::HEX);

    new Settings("sms_paddle_1_x", "Paddle1 x Axis", 0x0000, Settings::HEX);
    new Settings("sms_paddle_1_a", "Paddle1 a", 0x0000, Settings::HEX);

    new Settings("sms_paddle_2_x", "Paddle2 x Axis", 0x0000, Settings::HEX);
    new Settings("sms_paddle_2_a", "Paddle2 a", 0x0000, Settings::HEX);

    new Settings("sms_sports_pad_1_x", "Sports Pad1 x Axis", 0x0000, Settings::HEX);
    new Settings("sms_sports_pad_1_y", "Sports Pad1 y Axis", 0x0000, Settings::HEX);
    new Settings("sms_sports_pad_1_a", "Sports Pad1 a", 0x0000, Settings::HEX);
    new Settings("sms_sports_pad_1_b", "Sports Pad1 b", 0x0000, Settings::HEX);

    new Settings("sms_sports_pad_2_x", "Sports Pad2 x Axis", 0x0000, Settings::HEX);
    new Settings("sms_sports_pad_2_y", "Sports Pad2 y Axis", 0x0000, Settings::HEX);
    new Settings("sms_sports_pad_2_a", "Sports Pad2 a", 0x0000, Settings::HEX);
    new Settings("sms_sports_pad_2_b", "Sports Pad2 b", 0x0000, Settings::HEX);

    new Settings("sms_pause_pause", "Pause", 0x0000, Settings::HEX);

    new Settings("sms_port_1",
                 "Select Controller for Port 1\n"
                 "  0 = None\n"
                 "  1 = Joypad\n"
                 "  2 = LightGun\n"
                 "  3 = Paddle\n"
                 "  4 = SportsPad\n"
                 "  5 = Terebi Oekaki"
                 , 1, Settings::DEC, 0, 5);

    new Settings("sms_port_2",
                 "Select Controller for Port 2\n"
                 "  0 = None\n"
                 "  1 = Joypad\n"
                 "  2 = LightGun\n"
                 "  3 = Paddle\n"
                 "  4 = SportsPad"
                 , 0, Settings::DEC, 0, 4);

    new Settings("sms_path_rom", "setup standard path for rom (game) files", "");
    new Settings("sms_path_sram", "setup path for sram (battery) files\n"
                    "no path means sram folder relative to app", "");
    new Settings("sms_path_jp_bios", "japanese/korean Bios file for SMS", "");
    new Settings("sms_path_us_bios", "us/eu Bios file for SMS", "");
    new Settings("sms_path_savestate", "setup path for savestate files\n"
                    "no path means savestate folder relative to app", "");
    new Settings("sms_path_cheat", "setup path for cheat files\n"
                    "no path means cheat folder relative to app", "");

    new Settings("gg_path_rom", "setup standard path for rom (game) files", "");
    new Settings("gg_path_sram", "setup path for sram (battery) files\n"
                    "no path means sram folder relative to app", "");
    new Settings("gg_path_jp_bios", "Bios file for GG", "");
    new Settings("gg_path_savestate", "setup path for savestate files\n"
                    "no path means savestate folder relative to app", "");
    new Settings("gg_path_cheat", "setup path for cheat files\n"
                    "no path means cheat folder relative to app", "");

    new Settings("sg_path_rom", "setup standard path for rom (game) files", "");
    new Settings("sg_path_sram", "setup path for sram (battery) files\n"
                    "no path means sram folder relative to app", "");
    new Settings("sg_path_savestate", "setup path for savestate files\n"
                    "no path means savestate folder relative to app", "");
    new Settings("sg_path_cheat", "setup path for cheat files\n"
                    "no path means cheat folder relative to app", "");

    new Settings("sms_revision",
        "Select SMS Base Revision\n"
        "  0 = SMS Revision 1\n"
        "  1 = SMS Revision 2",
        1, Settings::DEC, 0, 1);

    new Settings("sms_use_bios",
        "Use prepared Bios files",
        1, Settings::BIN);

    new Settings("gg_use_bios",
        "Use prepared Bios files",
        1, Settings::BIN);

    new Settings("sms_disable_border",
        "Disable sms border",
        0, Settings::BIN);

    new Settings("sg_disable_border",
        "Disable sg-1000 border",
        0, Settings::BIN);


    new Settings("sms_use_fm",
        "Use fm Sound",
        0, Settings::BIN);

    new Settings("sms_disable_3d",
        "Disable 3D display",
        0, Settings::BIN);

    new Settings("gg_gear_to_gear", 0);

    new Settings("recent0", "recently loaded roms", "" );
    new Settings("recent1", "recently loaded roms", "" );
    new Settings("recent2", "recently loaded roms", "" );
    new Settings("recent3", "recently loaded roms", "" );
    new Settings("recent4", "recently loaded roms", "" );
    new Settings("recent5", "recently loaded roms", "" );
    new Settings("recent6", "recently loaded roms", "" );
    new Settings("recent7", "recently loaded roms", "" );
    new Settings("recent8", "recently loaded roms", "" );
    new Settings("recent9", "recently loaded roms", "" );

    new Settings("recent_lock", "dont change current rom list", 0, Settings::BIN);
}
