#include "dsound.h"
#include <helper.h>

#define dx_release(__m) if(__m) { __m->Release(); __m = 0; }

DSound::DSound(unsigned _id)
{
	device.buffer = 0;
	ds = 0;
	dsb = 0;
	settings.latency = 120;
	settings.speed = 100;
    settings.frequency = 44100;
	settings.volume = DSBVOLUME_MAX;
	settings.audio_active = true;
}

void DSound::stop() {
	if (dsb) dsb->Stop();
}

void DSound::play() {
	if (dsb) dsb->Play(0, 0, DSBPLAY_LOOPING);
}

void DSound::setSample(u32 sample) {
	if (!settings.audio_active) sample = 0;
    device.buffer[device.buffer_pos++] = sample;

	if(device.buffer_pos < device.latency) return;
	device.buffer_pos = 0;

    DWORD pos, size;
	void *output;

	while(device.distance >= device.rings - 1) {
		dsb->GetCurrentPosition(&pos, 0);
        unsigned activering = pos / (device.latency * 4);
		if(activering == device.readring) {
			continue;
		}

		//subtract number of played rings from ring distance counter
		device.distance -= (device.rings + activering - device.readring) % device.rings;
		device.readring = activering;

		if(device.distance < 2) {
		//buffer underflow; set max distance to recover quickly
			device.distance  = device.rings - 1;
			device.writering = (device.rings + device.readring - 1) % device.rings;
			break;
		}
	}

	device.writering = (device.writering + 1) % device.rings;
	device.distance  = (device.distance  + 1) % device.rings;

    if(dsb->Lock(device.writering * device.latency * 4, device.latency * 4, &output, &size, 0, 0, 0) == DS_OK) {
        memcpy(output, device.buffer, device.latency * 4);
		dsb->Unlock(output, size, 0, 0);
	}
}

void DSound::term() {
	free_mem(device.buffer);
	dx_release(dsb);
	dx_release(ds);
}

void DSound::create(HWND hwnd) {
	term();

	device.frequency = (double(settings.frequency) * double(settings.speed)) / 100.0;
	device.rings = 12;
	device.latency = device.frequency * settings.latency / device.rings / 1000.0 + 0.5;

	device.buffer_size = device.latency * device.rings;
    device.buffer = new u32[device.buffer_size * sizeof(u32)];

	DWORD result = DirectSoundCreate(0, &ds, 0);
    if(result != DS_OK) {
        throw "dsound_error";
    }

    ds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);

	memset(&wfx, 0, sizeof(wfx));
	wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = 2;
	wfx.nSamplesPerSec  = device.frequency;
	wfx.wBitsPerSample  = 16;
	wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes   = device.buffer_size * sizeof(u32);

    GUID guidNULL;
    memset(&guidNULL,0,sizeof(GUID));

    dsbd.guid3DAlgorithm = guidNULL;
	dsbd.lpwfxFormat     = &wfx;
	ds->CreateSoundBuffer(&dsbd, &dsb, 0);
	dsb->SetFrequency(device.frequency);
   
	clear();
}

void DSound::clear() {
	device.readring  = 0;
	device.writering = device.rings - 1;
	device.distance  = device.rings - 1;

	device.buffer_pos = 0;
	if(device.buffer) memset(device.buffer, 0, device.buffer_size);

	stop();
	dsb->SetCurrentPosition(0);

	DWORD size;
	void *output;
    dsb->Lock(0, device.buffer_size * sizeof(u32), &output, &size, 0, 0, 0);
	memset(output, 0, size);
	dsb->Unlock(output, size, 0, 0);

	play();
}

void DSound::init(HWND hwnd, unsigned _speed, signed _frequencyCorrection) {
    settings.speed = _speed;
    settings.frequency = 44100 + _frequencyCorrection;
    create(hwnd);
}

void DSound::adjustVolume(unsigned _volume) {
    settings.volume = _volume;
    if (dsb)
        dsb->SetVolume( DSVOLUME_TO_DB(settings.volume) );
}
