
#ifndef DVIDEO_H
#define DVIDEO_H

#include "../drvvideo.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
using namespace std;

#include <dataTypes.h>

class DVideo : public DrvVideo {
	LPDIRECT3D9 lpD3D;
	D3DPRESENT_PARAMETERS d3dpp;
	LPDIRECT3DDEVICE9 lpD3DDevice;
	D3DCAPS9 d3dcaps;
	LPDIRECT3DSURFACE9 surface;
	LPDIRECT3DTEXTURE9 texture, scanline;
	LPDIRECT3DVERTEXBUFFER9 vertex_buffer, *vertex_ptr;
	D3DLOCKED_RECT d3dlr;
    ID3DXFont* m_font;

	struct {
		bool fullscreen;
		int width;
        int usedWidth;
        int leftWidth;
		int height;
		bool vsync;
		unsigned input_width;
		unsigned input_height;
	} screen;

	struct {
		bool dynamic;      //device supports dynamic textures
		bool stretchrect;  //device supports StretchRect
	} caps;

	struct {
		u32 t_usage, v_usage;
		u32 t_pool,  v_pool;
		u32 lock;
		u32 filter;
	} flags;

    enum Filter { Point = 0, Linear = 1 };

	struct d3dvertex {
		float x, y, z, rhw;	//screen coords
		u32 color;			//diffuse color
		float u, v;			//texture coords
	};

	bool lost;
	unsigned filter;

	struct Note {
		bool active;
		bool critical;
        string mes;
		int frames;
	};

	Note note;

	struct {
		bool enable;
		unsigned intensity;
	} scanlines;

	struct {
		bool enable;
		unsigned pos;
	} frame_counter;

public:
    void set_display_mes(string display_mes, int display_frames=90, bool critical=false) {
		note.active = true;
		note.critical = critical;
		note.mes = display_mes;
		note.frames = display_frames;
	}
	void term();
	void init(HWND, int, int, bool, bool);
	void redraw();
	void set_vertex(u16 src_w, u16 src_h, u16 tex_w, u16 tex_h, u16 dest_x, u16 dest_y, u16 dest_w, u16 dest_h);

	void release_resources();
	void init_scanlines();
	bool recover();
	void clear();
	bool lock(u32*& data, u32& pitch, unsigned width, unsigned height);
	void unlock();
	void update_filter(unsigned _filter);
    void setAspectCorrect(bool state);
	void set_filter(unsigned _filter) {
		filter = _filter;
		update_filter(filter);
	}
    void set_scanlines(bool state, unsigned intensity) {
		scanlines.enable = state;
		scanlines.intensity = intensity;
        if (scanlines.enable) {
			init_scanlines();
		}
	}

	void set_counter(bool state) {
		frame_counter.enable = state;
	}

	void set_counter_pos(unsigned pos) {
		frame_counter.pos = pos;
	}

    DVideo();
	~DVideo() {}
};

#endif
