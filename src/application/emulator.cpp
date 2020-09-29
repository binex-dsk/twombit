#include "emulator.h"
#include "drivers/drvvideo.h"
#include "../libsms/libsms.h"
#include "settings.h"
#include "dialog.h"
#include "lang.h"
#include "exception.h"
#include "recent.h"
#include "cheatshandler.h"
#include "statehandler.h"
#include "inputmanager/inputmanager.h"

#include <dataTypes.h>
#include "app.h"
#include <time.h>

QVector<Emulator::Models> Emulator::models;
Emulator* Emulator::instance = 0;

Emulator::Emulator(MGui& mgui, CGui& cgui)
:
mgui(mgui),
cgui(cgui),
color(),
drvVideo(),
#ifdef __WIN32__
drvVideoGG(),
#endif
drvAudio(0),
drvAudioGG(1),
drvInput(),
inputmanager(),
resampler(),
file(),
biosFile()
{

    instance = this;
    cartLoaded = false;
    fpsCounter[0].frames_executed = fpsCounter[0].frame_count = 0;
    fpsCounter[1].frames_executed = fpsCounter[1].frame_count = 0;
    useMouse = false;
}

void Emulator::init() {
    initInput();
    initSound();
    initVideo();
}

void Emulator::initModels() {
    models.resize(3);
    models[0].id = 0;
    models[0].ident = "sms";
    models[0].desc = "SMS";
    models[0].extension = "SMS (*.sms *.sg *.mv ";
    models[0].inUse = true;

    models[1].id = 1;
    models[1].ident = "gg";
    models[1].desc = "Game Gear";
    models[1].inUse = true;
    models[1].extension = "Game Gear (*.gg *.sms ";

    models[2].id = 2;
    models[2].ident = "sg";
    models[2].desc = "SG-1000";
    models[2].extension = "SG-1000 (*.sg *.mv ";
    models[2].inUse = true;
}

Emulator::Models Emulator::getModelByIdent(QString ident) {
    for (int i = 0; i < models.size(); i++) {
        if (models[i].ident == ident) return models[i];
    }
    return models[0];
}

void Emulator::unload() {
    saveBackupRam();
    saveCheat();
    file.unload();
    biosFile.unload();
    smsUnload();
    smsResetCheat();
    cartLoaded = false;
}

void Emulator::saveCheat() {
    CheatsHandler::getInstance()->save(file.getFn(), model.ident);
    cgui.cheat_gui_refresh();
    cgui.cheat_gui_enable_elements(false);
}

void Emulator::loadCheat() {
    CheatsHandler::getInstance()->load(file.getFn(), model.ident);
    cgui.cheat_gui_refresh();
    cgui.cheat_gui_enable_elements(true);
}

void Emulator::saveState() {
    bool g2g = Settings::get<int>("gg_gear_to_gear") == 1;
    if (g2g) return;
    unsigned pos = Settings::get<int>("save_slot");
    QString errorMes = Lang::getInstance()->get("state_error_save") + QString::number(pos+1, 10);
    QString successMes = Lang::getInstance()->get("state_saved") + QString::number(pos+1, 10);

    unsigned stateSize = smsSavestateSize();
    unsigned char* stateData = new unsigned char[stateSize];

    if (!smsSaveState(stateData, stateSize)) {
        drvVideo.set_display_mes(errorMes.toStdString(), 90, true);
        return;
    }
    if (!StateHandler::getInstance()->save(file.getFn(), model.ident, pos, stateData, stateSize)) {
        drvVideo.set_display_mes(errorMes.toStdString(), 90, true);
        return;
    }
    mgui.setSlotTs(pos, QDateTime::currentMSecsSinceEpoch() / 1000);
    drvVideo.set_display_mes(successMes.toStdString());
}

void Emulator::loadState() {
    bool g2g = Settings::get<int>("gg_gear_to_gear") == 1;
    if (g2g) return;
    unsigned pos = Settings::get<int>("save_slot");
    QString errorMes = Lang::getInstance()->get("state_error_load") + QString::number(pos+1, 10);
    QString errorMes2 = Lang::getInstance()->get("state_incompatible") + QString::number(pos+1, 10);
    QString successMes = Lang::getInstance()->get("state_loaded") + QString::number(pos+1, 10);

    unsigned char* data = StateHandler::getInstance()->load(file.getFn(), model.ident, pos);

    if (!data) {
        drvVideo.set_display_mes(errorMes.toStdString(), 90, true);
        return;
    }
    if (!smsLoadState(data, StateHandler::getInstance()->getSize())) {
        drvVideo.set_display_mes(errorMes2.toStdString(), 90, true);
        return;
    }
    drvVideo.set_display_mes(successMes.toStdString());
}

void Emulator::setState() {
    bool g2g = Settings::get<int>("gg_gear_to_gear") == 1;
    if (g2g) return;
    unsigned long long tsSort[10], ts[10];
    for(unsigned i = 0; i < 10; i++) {
        unsigned char* data = StateHandler::getInstance()->load(file.getFn(), model.ident, i);
        unsigned long long _ts = 0;
        if (data) {
            _ts = smsSavestateTimeStamp(data);
        }
        tsSort[i] = ts[i] = _ts;
        mgui.setSlotTs(i, _ts);
    }
    _sort(tsSort, sizeof(tsSort) / sizeof(unsigned long long));
    if(tsSort[9] == 0)
        return;

    for(unsigned i = 0; i < 10; i++) {
        if (tsSort[9] == ts[i]) {
            mgui.change_slot(i);
            break;
        }
    }
}

void Emulator::changeSlot(unsigned pos) {
    if (!cartLoaded)  return;
    QString mes = Lang::getInstance()->get("slot_changed") + QString::number(pos+1, 10);
    drvVideo.set_display_mes(mes.toStdString());
}

bool Emulator::openRom(QString fn, Models _model) {
    if (cartLoaded) {
        unload();
    }
    file.setFile(fn);
    file.fetchData();
    file.completeTo256BChunk();

    if (file.getError() != File::FINE) {
        Dialog::Fail(Lang::getInstance()->get("load_error"));
        return false;
    }
    mgui.setTitle(file.getFn());
    model = _model;
    smsSetRegion(Settings::get<int>("video_region"));
    loadCheat();

    if (model.ident == "sms") loadSMS();
    else if (model.ident == "gg") loadGG();
    else if (model.ident == "sg") loadSG();

    Recent* recent = new Recent();
    if (recent->add(model.ident + " | " + fn)) {
        recent->save();
    }
    smsPower();
    setState();
    cartLoaded = true;
    return true;
}

QString Emulator::getBackupPath(QString fn) {
    QString _path = "";
    if (model.ident == "sms") _path = Settings::get<QString>("sms_path_sram");
    else if (model.ident == "gg") _path = Settings::get<QString>("gg_path_sram");
    if (_path == "") {
        _path = App::getDir() + "sram";
    }
    _path += "/" + fn + ".srm";
    return _path;
}

void Emulator::loadBackupRam() {
    QString _path = getBackupPath( file.getFn() );
    File backup;
    backup.setFile(_path);
    unsigned char* _data = backup.fetchData();
    if (backup.getError() != File::FINE) return;
    unsigned char* sram = smsGetMemoryData(SMS_MEMORY_SRAM);
    unsigned sramSize = smsGetMemorySize(SMS_MEMORY_SRAM);
    if (!sram) return;
    memcpy(sram, _data, _min(sramSize, backup.getSize() ) );
    backup.unload();
}

void Emulator::saveBackupRam() {
    QString _path = getBackupPath( file.getFn() );
    unsigned char* sram = smsGetMemoryData(SMS_MEMORY_SRAM);
    unsigned sramSize = smsGetMemorySize(SMS_MEMORY_SRAM);
    if (!sram) return;
    for (unsigned i = 0; i < sramSize; i++) {
        if (sram[i] != 0) {
            File backup;
            backup.setFile(_path);
            backup.write(sram, sramSize);
            break;
        }
    }
}

void Emulator::loadSMS() {
    smsEnableYamahaSoundChip(Settings::get<bool>("sms_use_fm"));
    smsDisable3D(Settings::get<bool>("sms_disable_3d"));
    smsLoad(file.getData(), file.getSize() );
    smsSetRevision(Settings::get<int>("sms_revision"));
    smsDisableBorder(Settings::get<bool>("sms_disable_border"));
    unsigned region = smsGetRegion();
    loadBackupRam();

    if (Settings::get<bool>("sms_use_bios")) {
        QString biosFN = Settings::get<QString>("sms_path_" + QString(region == SMS_REGION_JAPAN ? "jp" : "us") + "_bios");
        if (biosFN != "") {
            biosFile.setFile(biosFN);
            biosFile.fetchData();
            if (biosFile.getError() == File::FINE) {
                smsLoadBios(biosFile.getData(), biosFile.getSize());
            } else {
                Dialog::Warn(Lang::getInstance()->get("load_error"), "bios error");
            }
        }
    }
}

void Emulator::loadSG() {
    smsLoadSG1000(file.getData(), file.getSize());
    smsDisableBorder(Settings::get<bool>("sg_disable_border"));
}

void Emulator::loadGG() {
    unsigned _mode = GG_MODE_NORMAL;
    QString ext = file.getExtension();
    if (ext == "sms") _mode = GG_MODE_SMS;
    else if (Settings::get<int>("gg_gear_to_gear")) _mode = GG_MODE_GEAR_TO_GEAR;

    smsLoadGameGear(file.getData(), file.getSize(), _mode );
    loadBackupRam();

    if (Settings::get<bool>("gg_use_bios")) {
        QString biosFN = Settings::get<QString>("gg_path_jp_bios");
        if (biosFN != "") {
            biosFile.setFile(biosFN);
            biosFile.fetchData();
            if (biosFile.getError() == File::FINE) {
                smsLoadBios(biosFile.getData(), biosFile.getSize());
            } else {
                Dialog::Warn(Lang::getInstance()->get("load_error"), "bios error");
            }
        }
    }
}

void Emulator::power() {
    smsPower();
}

void Emulator::reset() {
    smsReset();
}

void Emulator::run() {
    smsRun();
}

//video
void Emulator::countFrames(unsigned screenPos) {
    fpsCounter[screenPos].frame_count++;
    time(&fpsCounter[screenPos].curr_t);
    if( fpsCounter[screenPos].curr_t != fpsCounter[screenPos].prev_t ) {
        fpsCounter[screenPos].frames_executed = fpsCounter[screenPos].frame_count;
        fpsCounter[screenPos].frame_count = 0;
    }
    fpsCounter[screenPos].prev_t = fpsCounter[screenPos].curr_t;
}


void Emulator::_render(const unsigned short* frame, unsigned width, unsigned height, unsigned modeId, bool secondGG) {
    instance->render(frame, width, height, modeId, secondGG);
}

void Emulator::render(const unsigned short* frame, unsigned width, unsigned height, unsigned modeId, bool secondGG) {
#ifdef __WIN32__
    DrvVideo* screen = !secondGG ? &drvVideo : &drvVideoGG;
    unsigned _c = secondGG ? 1 : 0;
#else
    DrvVideo* screen = &drvVideo;
    unsigned _c = 0;
#endif
    color.update();
    u32* gpu_data = 0;
    u32 gpu_pitch;

    countFrames(secondGG ? 1 : 0);
    screen->set_counter_pos( fpsCounter[_c].frames_executed );

    unsigned outwidth;
    unsigned outheight;

    color.active_filter->size(outwidth, outheight, width, height);
#ifndef __WIN32__
    drvVideo.incommingSecondBuffer(secondGG);
#endif
    if ( screen->lock(gpu_data, gpu_pitch, outwidth, outheight) ) {
        color.active_filter->render((Color::Mode)modeId, gpu_data, gpu_pitch, (u16*)frame, width, height);
        screen->unlock();
        screen->redraw();
    }
}

void Emulator::updateVideo(unsigned width, unsigned height, bool fullscreen) {
    try {
#ifdef __WIN32__
    drvVideo.init(fullscreen ? mgui.getWId() : mgui.getWindowWId(), width, height, Settings::get<bool>("video_vsync"), fullscreen);
#else
    drvVideo.init(mgui.getWidget(), Settings::get<bool>("video_vsync"));
#endif

    drvVideo.setAspectCorrect(!fullscreen ? false : Settings::get<bool>("video_aspect_correct") );

#ifdef __WIN32__
    if (Settings::get<int>("gg_gear_to_gear") == 1) {
        drvVideoGG.init(mgui.getWindowWIdSecond(), width, height, Settings::get<bool>("video_vsync"), false);
        drvVideoGG.setAspectCorrect(false);
    }
#endif
    } catch(const char* errorKey) {
        throw Exception( Lang::getInstance()->get(QString::fromStdString(errorKey)) );
    }
}

void Emulator::clearVideo() {
    drvVideo.clear();
#ifdef __WIN32__
    drvVideoGG.clear();
#endif
}

void Emulator::updateColorTable() {
    color.update_parameter(Settings::get<int>("video_contrast"), Settings::get<int>("video_gamma"), Settings::get<int>("video_brightness"));
}

void Emulator::updateScanlines() {
    drvVideo.set_scanlines(Settings::get<bool>("video_scanlines"), Settings::get<int>("video_scanline_intensity"));
#ifdef __WIN32__
    drvVideoGG.set_scanlines(Settings::get<bool>("video_scanlines"), Settings::get<int>("video_scanline_intensity"));
#endif
}

void Emulator::updateSwFilter() {
    color.set_filter(Settings::get<int>("video_filter_sw"), Settings::get<int>("ntsc_filter"));
#ifndef __WIN32__
    drvVideo.clear();
#endif
}

void Emulator::updateHwFilter() {
    drvVideo.set_filter(Settings::get<int>("video_filter_hw"));
#ifdef __WIN32__
    drvVideoGG.set_filter(Settings::get<int>("video_filter_hw"));
#endif
}

void Emulator::activateFPS(bool state) {
    drvVideo.set_counter(state);
#ifdef __WIN32__
    drvVideoGG.set_counter(state);
#endif
}

void Emulator::initVideo() {
    updateColorTable();
    updateScanlines();
    updateHwFilter();
    updateSwFilter();
    activateFPS(Settings::get<bool>("show_fps"));
    smsSetVideoRefresh(&Emulator::_render);
}

//input
void Emulator::_pollInput() {
    instance->pollInput();

}

void Emulator::pollInput() {
    inputmanager.fetchInput();
    inputmanager.update();
}

void Emulator::pollInputDevices() {
    if (inputmanager.pollDevices()) {
        cgui.inputPollingComplete();
        checkIfMouseIsInUse();
    }
    inputmanager.update();
}

unsigned Emulator::getDeviceCount() {
    return inputmanager.getDeviceCount();
}

QString Emulator::getDeviceName(unsigned pos) {
    if (pos >= getDeviceCount()) return "";
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    return QString::fromStdString( list[pos]->getDeviceName() );
}

QString Emulator::getDevicePort(unsigned pos) {
    if (pos >= getDeviceCount()) return "";
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    unsigned port = list[pos]->getDevicePort();
    switch (port) {
        case 0: return "Port 1: ";
        case 1: return "Port 2: ";
        default: return "";
    }
}

void Emulator::emptyDevice(unsigned pos) {
    if (pos >= getDeviceCount()) return;
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    list[pos]->unmap();
}

unsigned Emulator::getDeviceObjectCount(unsigned pos) {
    if (pos >= getDeviceCount()) return 0;
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    return list[pos]->countInputObjects();
}

QString Emulator::getDeviceObjectEntry(unsigned devPos, unsigned objPos) {
    if (devPos >= getDeviceCount()) return "";
    if (objPos >= getDeviceObjectCount(devPos)) return "";
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    std::vector<Inputmanager::InputObject*> objList = list[devPos]->getObjectList();
    return QString::fromStdString( objList[objPos]->getEntry() );
}

QString Emulator::getUpdateString(unsigned devPos, unsigned objPos) {
    if (devPos >= getDeviceCount()) return "";
    if (objPos >= getDeviceObjectCount(devPos)) return "";
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    std::vector<Inputmanager::InputObject*> objList = list[devPos]->getObjectList();
    bool isButton = objList[objPos]->isButton();
    QString text = Lang::getInstance()->get("press_key");
    if (!isButton) {
        text = Lang::getInstance()->get("move_mouse");
    }
    return text + " " + Lang::getInstance()->get( QString::fromStdString( objList[objPos]->getName() ) );
}

void Emulator::captureReady(unsigned devPos, unsigned objPos) {
    if (devPos >= getDeviceCount()) return;
    if (objPos >= getDeviceObjectCount(devPos)) return;
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    std::vector<Inputmanager::InputObject*> objList = list[devPos]->getObjectList();
    inputmanager.captureReady(objList[objPos]);
}

QVector<Emulator::Devices> Emulator::getDevicesForPort(unsigned port) {
    QVector<Devices> devList(0);
    std::vector<Inputmanager::Device*> list = inputmanager.getDeviceList();
    for (int i = 0; i < getDeviceCount(); i++) {
        if (list[i]->getDevicePort() == port) {
            Devices device;
            device.id = list[i]->getDeviceId();
            device.name = QString::fromStdString( list[i]->getDeviceName() );
            devList.append(device);
        }
    }
    return devList;
}

void Emulator::updatePort() {
    smsSetDevice(SMS_PORT_1, Settings::get<int>("sms_port_1"));
    smsSetDevice(SMS_PORT_2, Settings::get<int>("sms_port_2"));
    checkIfMouseIsInUse();
}

void Emulator::checkIfMouseIsInUse() {
    useMouse = false;
    if (inputmanager.areThereMouseInputObjects((Inputmanager::Port)SMS_PORT_1, (Inputmanager::DeviceID)Settings::get<int>("sms_port_1"))) {
        useMouse = true;
    }
    if (inputmanager.areThereMouseInputObjects((Inputmanager::Port)SMS_PORT_2, (Inputmanager::DeviceID)Settings::get<int>("sms_port_2"))) {
        useMouse = true;
    }
}

signed Emulator::_readInput(unsigned port, unsigned deviceId, unsigned objectId) {
    return instance->readInput(port, deviceId, objectId);
}

signed Emulator::readInput(unsigned port, unsigned deviceId, unsigned objectId) {
    return inputmanager.getState((Inputmanager::Port)port, (Inputmanager::DeviceID) deviceId, (Inputmanager::IObjectID) objectId);
}

string Emulator::getKeyName(unsigned element) {
    return drvInput.getKeyName(element);
}

string Emulator::getJoyName(unsigned joy) {
    return drvInput.getJoyName(joy);
}

void Emulator::fetchInputTable(i16* table) {
    drvInput.poll( table );
}

void Emulator::initInput() {
    reinitInput();
    updatePort();
    smsSetInputPoll(&Emulator::_pollInput);
    smsSetInputState(&Emulator::_readInput);
}

void Emulator::inputAcquire() {
    if (cartLoaded && useMouse) {
        drvInput.m_acquire();
    }
}

void Emulator::inputUnacquire() {
   // if (cartLoaded && useMouse) {
        drvInput.m_unacquire();
   // }
}

bool Emulator::isInputAcquired() {
    return drvInput.m_isAcquired();
}

void Emulator::reinitInput() {
    try {
        drvInput.init<WId>(mgui.getWId()); //WId is platform dependant
    } catch(const char* errorKey) {
        throw Exception( Lang::getInstance()->get(QString::fromStdString(errorKey)) );
    }
}

//audio
void Emulator::initSound() {
    resampler.setFrequency(49716.0, 44100.0); //resample ym2413
    smsSetAudioSample(&Emulator::_audioSample);
    updateFrequency();
    adjustVolume(Settings::get<int>("audio_volume"));
    activateSound(Settings::get<bool>("audio_active"));
    playSound();
}

void Emulator::updateFrequency() {
    int speedPos = Settings::get<int>("speed_select");
    try {
        drvAudio.init(mgui.getWId()
                      ,Settings::get<int>("speed_" + QString::number(speedPos))
                      ,Settings::get<int>("sms_audio_frequency"));
        drvAudioGG.init(mgui.getWId()
                      ,Settings::get<int>("speed_" + QString::number(speedPos))
                      ,Settings::get<int>("sms_audio_frequency"));
    } catch(const char* errorKey) {
        throw Exception( Lang::getInstance()->get(QString::fromStdString(errorKey)) );
    }
}

void Emulator::adjustVolume(unsigned volume) {
    drvAudio.adjustVolume(volume);
    drvAudioGG.adjustVolume(volume);
}

void Emulator::activateSound(bool state) {
    drvAudio.activateAudio(state);
    drvAudioGG.activateAudio(state);
}

void Emulator::playSound() {
    drvAudio.play();
    drvAudioGG.play();
}

void Emulator::stopSound() {
    drvAudio.stop();
    drvAudioGG.stop();
}

void Emulator::clearAudio() {
    drvAudio.clear();
    drvAudioGG.clear();
}

void Emulator::_audioSample(signed sampleLeft, signed sampleRight, unsigned soundChip) {
    instance->audioSample(sampleLeft, sampleRight, soundChip);
}

void Emulator::audioSample(signed sampleLeft, signed sampleRight, unsigned soundChip) {
    if (soundChip == SMS_SOUND_SN76489 ) {
        drvAudio.setSample( (sampleLeft & 0xffff) << 16 | (sampleRight & 0xffff) );
    } else if (soundChip == SMS_SOUND_YM2413) {
        signed samples[] = { sampleLeft, sampleRight };
        resampler.sample(samples);
        while(resampler.pending()) {
            resampler.read(samples);
            drvAudioGG.setSample( (samples[0] & 0xffff) << 16 | (samples[1] & 0xffff) );
        }
    } else if (soundChip == GG_SOUND_SN76489) {
        drvAudioGG.setSample( (sampleLeft & 0xffff) << 16 | (sampleRight & 0xffff) );
    }
}

//Cheats
bool Emulator::setCheat(const char* code) {
    return smsSetCheat(code);
}

void Emulator::resetCheat() {
    smsResetCheat();
}
