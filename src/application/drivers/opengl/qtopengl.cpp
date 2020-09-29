#include "qtopengl.h"

#include <sstream>
#include <helper.h>

#ifdef __linux__
    #include <GL/glx.h>
    #define glGetProcAddress(name) (*glXGetProcAddress)((const GLubyte*)(name))

#elif __APPLE__

#endif

QtOpenGL::QtOpenGL()
{
    filter = GL_NEAREST;
    widget = 0;
    aspectCorrect = false;
    vsync = false;
    layout = new QVBoxLayout;
    layout->setMargin(0);
    scanlines.enable;
    scanlines.intensity = 0;
    note.active = false;
    frame_counter.enable = false;
    nextFillSecondBuffer = false;
    lastFillSecondBuffer = false;
    twoScreenMode = false;
}

void QtOpenGL::set_filter(unsigned _filter) {
    switch (_filter) {
        case 0:
        default:
            filter = GL_NEAREST;
            break;
        case 1:
            filter = GL_LINEAR;
            break;
    }
}

void QtOpenGL::setAspectCorrect(bool state) {
    aspectCorrect = state;
}

string QtOpenGL::fetchText(bool& critical) {
    string out = "";

    if(note.active) {
        if(note.frames-- > 0) {
            critical = note.critical;
            out = note.mes;
        } else {
            note.active = false;
        }
    } else if(frame_counter.enable) {
        stringstream st;
        st << frame_counter.pos;
        out = st.str();
    }
    return out;
}

void QtOpenGL::GLWidget::drawScanline() {
    if (!wrapper->scanlines.enable) return;
    glBindTexture(GL_TEXTURE_2D, glScanTexture);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width(), 0, height(), -1.0, 1.0);
    glViewport(0,0,width(), height());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 8);
    glTexSubImage2D(GL_TEXTURE_2D,
        0, 0, 0,
        8, height(), GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, scanBuffer);

    double w = 1.0;
    double h = 1.0;
    int u = width();
    int v = height();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 0); glVertex3i(0, v, 0);
    glTexCoord2f(w, 0); glVertex3i(u, v, 0);
    glTexCoord2f(0, h); glVertex3i(0, 0, 0);
    glTexCoord2f(w, h); glVertex3i(u, 0, 0);

    glEnd();
}

void QtOpenGL::GLWidget::setScanline() {

    if (glScanTexture == 0) {
        glGenTextures(1, &glScanTexture);
    }

    glBindTexture(GL_TEXTURE_2D, glScanTexture);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 8);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    if(scanBuffer) {
        delete[] scanBuffer;
    }

    scanBuffer = new unsigned[8 * height()]();
    unsigned scanline_alpha = unsigned(double(wrapper->scanlines.intensity) / 100.0 * 255.0);
    scanline_alpha = _minmax<unsigned>(scanline_alpha, 0, 255);
    scanline_alpha <<= 24;

    unsigned* scan = scanBuffer;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 8, height(), 0, GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV, scanBuffer);

    unsigned lh = floor((double(height()) / 480.0) + 0.5);
    unsigned lhc = lh;
    bool flip = true;

    for(unsigned y = 0; y < height(); y++) {
        for(unsigned x = 0; x < 8; x++) {
            if (flip) {
                *scan++ =  0;
            } else {
                *scan++ = scanline_alpha;
            }
        }
        if (--lhc == 0) {
            lhc = lh;
            flip ^= 1;
        }
    }
}

void QtOpenGL::GLWidget::drawText(const char* text, int length, int x, int y, bool warning) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, width(), 0, height());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();

    if (warning) {
        glColor3f(1.0, 0.0, 0.0);
    } else {
        glColor3f(1.0, 1.0, 1.0);
    }

    glDisable(GL_TEXTURE_2D);
    glRasterPos2i(x, y);
    for(int i=0; i<length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)text[i]);
    }
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void QtOpenGL::GLWidget::setTexture(unsigned width, unsigned height) {
    textureWidth  = width;
    textureHeight = height;

    buffer = new unsigned[textureWidth * textureHeight]();
    memset(buffer, 0, textureWidth * textureHeight * sizeof(unsigned));

    secondBuffer = new unsigned[textureWidth * textureHeight]();
    memset(secondBuffer, 0, textureWidth * textureHeight * sizeof(unsigned));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glGenTextures(1, &gltexture);
    glBindTexture(GL_TEXTURE_2D, gltexture);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, textureWidth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        textureWidth, textureHeight, 0, GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV, buffer);

    glGenTextures(1, &gltextureSecond);
    glBindTexture(GL_TEXTURE_2D, gltextureSecond);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, textureWidth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        textureWidth, textureHeight, 0, GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV, secondBuffer);
}

void QtOpenGL::GLWidget::calcRenderPos(bool secondScreen) {
    unsigned outWidth = width();
    unsigned outHeight = height();

    if (!secondScreen) {
        if (!wrapper->aspectCorrect) {
            if (wrapper->twoScreenMode) {
                screen.usedWidth = (outWidth - 10) / 2;
            } else {
                screen.usedWidth = outWidth;
            }
            screen.leftWidth = 0;
        } else {
            screen.usedWidth = double(outHeight / 3) * 4;
            screen.leftWidth = (outWidth - screen.usedWidth) / 2;
        }
    } else {
        screen.usedWidth = (outWidth - 10) / 2;
        screen.leftWidth = screen.usedWidth + 10;
    }
}

void QtOpenGL::GLWidget::paintScreen(bool secondScreen) {
    unsigned outWidth = width();
    unsigned outHeight = height();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, secondScreen ? gltextureSecond : gltexture);
    glOrtho(0, outWidth, 0, outHeight, -1.0, 1.0);
    calcRenderPos(secondScreen);
    glViewport(screen.leftWidth, 0, screen.usedWidth, outHeight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, textureWidth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, wrapper->filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, wrapper->filter);

    glTexSubImage2D(GL_TEXTURE_2D,
        0, 0, 0,
        screen.inputWidth, screen.inputHeight, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, secondScreen ? secondBuffer : buffer);

    double w = double(screen.inputWidth)  / double(textureWidth);
    double h = double(screen.inputHeight) / double(textureHeight);
    int u = outWidth;
    int v = outHeight;
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 0); glVertex3i(0, v, 0);
    glTexCoord2f(w, 0); glVertex3i(u, v, 0);
    glTexCoord2f(0, h); glVertex3i(0, 0, 0);
    glTexCoord2f(w, h); glVertex3i(u, 0, 0);
    glEnd();
}

void QtOpenGL::GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    paintScreen(false);
    if(wrapper->twoScreenMode) {
        paintScreen(true);
    }

    drawScanline();
    bool critical = false;
    string text = wrapper->fetchText(critical);
    if (text != "" && text != "0")
        drawText(text.c_str(), text.size(), 10, 10, critical);

#ifdef __linux__
    glFinish();
#endif
}

void QtOpenGL::GLWidget::updateVsync() {
#ifdef __APPLE__
    makeCurrent();
    GLint value = wrapper->vsync ? 1 : 0;
    //aglSetInteger(aglGetCurrentContext(), AGL_SWAP_INTERVAL, &value);
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &value);
#elif __linux__
    if(!glSwapInterval) glSwapInterval = (int (*)(int))glGetProcAddress("glXSwapIntervalSGI");
    if(!glSwapInterval) glSwapInterval = (int (*)(int))glGetProcAddress("glXSwapIntervalMESA");
    if( glSwapInterval) glSwapInterval(wrapper->vsync ? 1 : 0);
#endif
}


void QtOpenGL::GLWidget::initializeGL() {
    term();
    format().setDoubleBuffer(true);

    updateVsync();
    setTexture(768,512);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glDisable(GL_ALPHA_TEST);
    //glDisable(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_STENCIL_TEST);

    glEnable(GL_DITHER);
    glEnable(GL_TEXTURE_2D);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void QtOpenGL::GLWidget::resizeGL(int width, int height) {
    QGLWidget::resizeGL(width, height);
    setScanline();
    clear();
}

void QtOpenGL::GLWidget::term() {
    if (gltexture) {
        glDeleteTextures(1, &gltexture);
        gltexture = 0;
    }
    if (gltextureSecond) {
        glDeleteTextures(1, &gltextureSecond);
        gltextureSecond = 0;
    }
    if (glScanTexture) {
        glDeleteTextures(1, &glScanTexture);
        glScanTexture = 0;
    }
    if (buffer) {
        delete[] buffer;
        buffer = 0;
    }
    if (secondBuffer) {
        delete[] secondBuffer;
        secondBuffer = 0;
    }
    if (scanBuffer) {
        delete[] scanBuffer;
        scanBuffer = 0;
    }
}

void QtOpenGL::GLWidget::clear() {
    if (buffer) {
        memset(buffer, 0, textureWidth * textureHeight * sizeof(unsigned));
    }
    if (secondBuffer) {
        memset(secondBuffer, 0, textureWidth * textureHeight * sizeof(unsigned));
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    updateGL();
}

bool QtOpenGL::lock(unsigned*& data, unsigned& pitch, unsigned width, unsigned height) {
    widget->screen.inputWidth = width;
    widget->screen.inputHeight = height;

    pitch = widget->textureWidth * 4;
    return data = nextFillSecondBuffer ? widget->secondBuffer : widget->buffer;
}

void QtOpenGL::unlock() {}

void QtOpenGL::clear() {
    if (!widget) return;
    frame_counter.pos = 0;
    widget->clear();
}

void QtOpenGL::redraw() {
    if (twoScreenMode && !nextFillSecondBuffer) {
        return;
    }
    widget->updateGL();
}

void QtOpenGL::init(QWidget* _parent, bool _vsync) {
    parent = _parent;
    vsync = _vsync;
    if (widget) {
        clear();
        widget->updateVsync();
        return;
    }
    widget = new GLWidget(this);
    layout->addWidget(widget);
    parent->setLayout(layout);

#ifdef __linux__
    char *myargv[1];
    int myargc=1;
    myargv[0] = strdup ("tmb");
    glutInit(&myargc, myargv);
#endif
}

void QtOpenGL::incommingSecondBuffer(bool state) {
    nextFillSecondBuffer = state;
    twoScreenMode = false;
    if (lastFillSecondBuffer != nextFillSecondBuffer) {
        twoScreenMode = true;
    }
    lastFillSecondBuffer = nextFillSecondBuffer;
}





void QtOpenGL::GLWidget::createCGL() {
    //CGDisplayCapture(kCGDirectMainDisplay);
//    CGLPixelFormatAttribute attribs[] =
//    {
//            kCGLPFANoRecovery,
//            kCGLPFADoubleBuffer,
//            kCGLPFAWindow,
//        kCGLPFADisplayMask, (CGLPixelFormatAttribute) CGDisplayIDToOpenGLDisplayMask ( kCGDirectMainDisplay ),
//        ( CGLPixelFormatAttribute) NULL
//    };

//    CGLPixelFormatAttribute attribs[] =
//    {
//            kCGLPFANoRecovery,
//            kCGLPFAAccelerated,
//            kCGLPFAWindow,
//            kCGLPFAColorSize,
//            ( CGLPixelFormatAttribute) 32,
//            kCGLPFADoubleBuffer,
//        ( CGLPixelFormatAttribute) 0
//    };

//    CGLPixelFormatObj pixelFormatObj;
//    GLint numPixelFormats;
//    CGLChoosePixelFormat( attribs, &pixelFormatObj, &numPixelFormats);
//    CGLCreateContext(pixelFormatObj, 0, &_cglContext);
//    CGLSetCurrentContext(_cglContext);
}

