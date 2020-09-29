#include "dvideo.h"
#include <helper.h>
#include <sstream>

#define dx_release(__m) if(__m) { __m->Release(); __m = 0; }

#define D3DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

DVideo::DVideo()
{
	lpD3DDevice = 0;
	lpD3D = 0;

	vertex_buffer = 0;
	surface = 0;
	texture = 0;
	scanline = 0;
	m_font = 0;

	lost = true;
	scanlines.enable = false;
	frame_counter.enable = false;
	note.active = false;
}

void DVideo::init(HWND hwnd, int width, int height, bool vsync, bool fullscreen)
{
	term();

    if ((lpD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
        throw "d3d_error";
    }

	screen.width = width;
	screen.height = height;
	screen.fullscreen = fullscreen;
	screen.vsync = vsync;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.hDeviceWindow				   = hwnd;

	d3dpp.Windowed					   = fullscreen ? false : true;
    d3dpp.BackBufferFormat			   = fullscreen ? D3DFMT_R5G6B5 : D3DFMT_UNKNOWN;
    d3dpp.BackBufferWidth			   = fullscreen ? width : 0;
    d3dpp.BackBufferHeight			   = fullscreen ? height : 0;
	d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;

	d3dpp.PresentationInterval		   = vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount			   = 1;
	d3dpp.SwapEffect					   = D3DSWAPEFFECT_FLIP;
	d3dpp.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER | D3DPRESENTFLAG_VIDEO;

	d3dpp.MultiSampleQuality			   = 0;	 //antialiasing
	d3dpp.MultiSampleType			   = D3DMULTISAMPLE_NONE;  //antialiasing
	d3dpp.EnableAutoDepthStencil		   = false;	//Z-Buffer
	d3dpp.AutoDepthStencilFormat		   = D3DFMT_UNKNOWN; //Z-Buffer

	HRESULT hr = lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
        D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpD3DDevice);

	if( !SUCCEEDED(hr) || !lpD3DDevice ) {
        throw "d3d_error";
	}

	lpD3DDevice->GetDeviceCaps(&d3dcaps);

	caps.dynamic = bool(d3dcaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES);
	caps.stretchrect = (d3dcaps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES) &&
	(d3dcaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFPOINT)  &&
	(d3dcaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)  &&
	(d3dcaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) &&
    (d3dcaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR);

	if(caps.dynamic) {
		flags.t_usage = D3DUSAGE_DYNAMIC;
		flags.v_usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
		flags.t_pool  = D3DPOOL_DEFAULT;
		flags.v_pool  = D3DPOOL_DEFAULT;
		flags.lock    = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
	} else {
		flags.t_usage = 0;
		flags.v_usage = D3DUSAGE_WRITEONLY;
		flags.t_pool  = D3DPOOL_MANAGED;
		flags.v_pool  = D3DPOOL_MANAGED;
		flags.lock    = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
	}

	lost = false;
	recover();
}

bool DVideo::recover() {
	if (!lpD3DDevice) {
        throw "d3d_error";
	}

	if(lost) {
		release_resources();
		if(lpD3DDevice->Reset(&d3dpp) != D3D_OK) return false;
	}

	lost = false;

	lpD3DDevice->SetDialogBoxMode(false);

	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //Textur color
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); //Vertex color -ignore

	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); //Textur alpha
	lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE); //Vertex alpha -ignore


	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	lpD3DDevice->SetRenderState(D3DRS_ZENABLE,  false);

    lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	//disable vertex shader
	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetFVF(D3DVERTEX); //set vertex format

	lpD3DDevice->CreateVertexBuffer(sizeof(d3dvertex) * 4, flags.v_usage, D3DVERTEX, static_cast<D3DPOOL>(flags.v_pool), &vertex_buffer, NULL);

	if(caps.stretchrect) {
		if(surface) surface->Release();
        lpD3DDevice->CreateOffscreenPlainSurface(1024, 1024, D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT, &surface, NULL);
	} else {
		if(texture) texture->Release();
        lpD3DDevice->CreateTexture(1024, 1024, 1, flags.t_usage, D3DFMT_X8R8G8B8,
			static_cast<D3DPOOL>(flags.t_pool), &texture, NULL);
	}

    D3DXCreateFont( lpD3DDevice, 14, 0, 0, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font );

	if (scanlines.enable) {
		init_scanlines();
	}
	update_filter(filter);
	clear();
	return true;
}

void DVideo::init_scanlines() {
    if(!lpD3DDevice) return;
	LPDIRECT3DSURFACE9 scanline_surface;
	D3DLOCKED_RECT d3dlr;

    lpD3DDevice->CreateTexture(8, screen.height, 1, flags.t_usage, D3DFMT_A8R8G8B8, static_cast<D3DPOOL>(flags.t_pool), &scanline, NULL);

	scanline->GetSurfaceLevel(0, &scanline_surface);
	scanline_surface->LockRect(&d3dlr, 0, flags.lock); 

	u32 *ptr = (u32*)d3dlr.pBits;
	u32 scanline_alpha = u8(double(scanlines.intensity) / 100.0 * 255.0);
	scanline_alpha = _minmax<u32>(scanline_alpha, 0, 255); 
	scanline_alpha <<= 24;

    unsigned lh = floor((screen.height / 480) + 0.5);
    unsigned lhc = lh;
    bool flip = true;

	for(u16 y = 0; y < screen.height; y++) {
        for(u16 x = 0; x < 8; x++) {
            //*ptr++ = !(y & 1) ? 0 : scanline_alpha;
            if (flip) {
                *ptr++ =  0;
            } else {
                *ptr++ = scanline_alpha;
            }
        }
        if (--lhc == 0) {
            lhc = lh;
            flip ^= 1;
        }
	}

	scanline_surface->UnlockRect();
	scanline_surface->Release();
}

void DVideo::update_filter(unsigned filter) {
	if(!lpD3DDevice) return;
	if(lost && !recover()) return;

	switch(filter) { default:
		case Filter::Point:  flags.filter = D3DTEXF_POINT;  break;
        case Filter::Linear: flags.filter = D3DTEXF_LINEAR; break;
	}

    lpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, flags.filter);
    lpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, flags.filter);
    lpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, flags.filter);
}

void DVideo::clear() {
    if(!lpD3DDevice) return;
	if(lost && !recover()) return;

	if(!caps.stretchrect) {
		texture->GetSurfaceLevel(0, &surface);
	}

	if(surface) {
		lpD3DDevice->ColorFill(surface, 0, D3DCOLOR_XRGB(0x00, 0x00, 0x00));
		if(!caps.stretchrect) surface->Release();
	}

	//clear primary display and all backbuffers
	for(unsigned i = 0; i < 3; i++) {
		lpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 1.0f, 0);
		lpD3DDevice->Present(0, 0, 0, 0);
	}
}

void DVideo::term() {
	release_resources();
	dx_release(lpD3DDevice);
	dx_release(lpD3D);
}

void DVideo::release_resources() {
	dx_release(surface);
	dx_release(texture);
	dx_release(vertex_buffer);
	dx_release(scanline);
	dx_release(m_font);
}

void DVideo::setAspectCorrect(bool state) {
    if (!state || !screen.fullscreen) {
        screen.usedWidth = screen.width;
        screen.leftWidth = 0;
    } else {
        screen.usedWidth = double(screen.height / 3) * 4;
        screen.leftWidth = (screen.width - screen.usedWidth) / 2;
    }
}

void DVideo::redraw() {
	if(lost && !recover()) return;
	
	lpD3DDevice->BeginScene();

    if(caps.stretchrect) {
		LPDIRECT3DSURFACE9 backbuffer;
		lpD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

		RECT SrcRect, DestRect;
        SetRect(&SrcRect, 0, 0, screen.input_width, screen.input_height );
        SetRect(&DestRect, screen.leftWidth, 0, screen.leftWidth + screen.usedWidth, screen.height);

        lpD3DDevice->StretchRect(surface, &SrcRect, backbuffer, &DestRect, static_cast<D3DTEXTUREFILTERTYPE>(flags.filter));
		backbuffer->Release();
	} else {
        set_vertex(screen.input_width, screen.input_height, 1024, 1024, screen.leftWidth, 0, screen.usedWidth, screen.height);
		lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

		lpD3DDevice->SetTexture(0, texture);
		lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

	if (scanlines.enable) {
        set_vertex(8, screen.height, 8, screen.height, 0, 0, screen.width, screen.height);
		lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		lpD3DDevice->SetTexture(0, scanline);
		lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
	
	D3DCOLOR fontColor;
	if (note.active) {
		if (note.frames-- > 0) {
			if (note.critical) {		
				fontColor = D3DCOLOR_ARGB(155,255,0,0);
			} else {
				fontColor = D3DCOLOR_ARGB(155,255,255,255);
			}
			RECT frct;
            SetRect(&frct, screen.leftWidth + screen.usedWidth - 270, screen.height - 20, screen.leftWidth + screen.usedWidth-5, screen.height-5);
            m_font->DrawTextA(NULL, note.mes.c_str(), -1, &frct, DT_RIGHT, fontColor );
		} else {
			note.active = false;
		}
	} else if(frame_counter.enable) {
		fontColor = D3DCOLOR_ARGB(155,255,255,255);
		RECT frct;
        SetRect(&frct, screen.leftWidth + screen.usedWidth - 25, screen.height - 20, screen.leftWidth + screen.usedWidth, screen.height);
        stringstream st;
        st << frame_counter.pos;
        m_font->DrawTextA(NULL, st.str().c_str() , -1, &frct, 0, fontColor );
	}

	lpD3DDevice->EndScene();

	if(lpD3DDevice->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) {
		lost = true;
	}
}

void DVideo::set_vertex(u16 src_w, u16 src_h, u16 tex_w, u16 tex_h, u16 dest_x, u16 dest_y, u16 dest_w, u16 dest_h) {
	d3dvertex vertex[4];

	vertex[0].x = vertex[2].x = ((float)dest_x    ) - 0.5f;
	vertex[1].x = vertex[3].x = ((float)dest_x + (float)dest_w) - 0.5f;
	vertex[0].y = vertex[1].y = ((float)dest_y    ) - 0.5f;
	vertex[2].y = vertex[3].y = ((float)dest_y + (float)dest_h) - 0.5f;

	vertex[0].z   = vertex[1].z   = 1.0;
	vertex[2].z   = vertex[3].z   = 1.0;
	vertex[0].rhw = vertex[1].rhw = 1.0;
	vertex[2].rhw = vertex[3].rhw = 1.0;

	vertex[0].u = vertex[2].u = 0;
	vertex[1].u = vertex[3].u = (float)src_w / (float)tex_w;
	vertex[0].v = vertex[1].v = 0;
	vertex[2].v = vertex[3].v = (float)src_h / (float)tex_h;

	vertex_buffer->Lock(0, sizeof(d3dvertex) * 4, (void**)&vertex_ptr, 0);
	memcpy(vertex_ptr, vertex, sizeof(d3dvertex) * 4);
	vertex_buffer->Unlock();

	lpD3DDevice->SetStreamSource(0, vertex_buffer, 0, sizeof(d3dvertex));
}

bool DVideo::lock(u32*& data, u32& pitch, unsigned width, unsigned height) {
	screen.input_width = width;
	screen.input_height = height;
	if(lost && !recover()) return false;
	if(!caps.stretchrect) texture->GetSurfaceLevel(0, &surface);

	surface->LockRect(&d3dlr, 0, flags.lock);
    pitch = d3dlr.Pitch;
	data = (u32*)d3dlr.pBits;
	return true;
}

void DVideo::unlock() {
	surface->UnlockRect();
	if(!caps.stretchrect) surface->Release();
}

#undef D3DVERTEX
