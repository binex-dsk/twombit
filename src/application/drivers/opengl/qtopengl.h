#ifndef QTOPENGL_H
#define QTOPENGL_H

#include <QtGui/QtGui>
#include <Qt/qgl.h>
#include "../drvvideo.h"
#include <string>
using namespace std;

#ifdef __linux__
    #include <GL/gl.h>
    #include <GL/freeglut.h>
#elif __APPLE__
    #include <Opengl/gl.h>
    #include <OpenGL.h>
    #include <GLUT/glut.h>
#endif

class QtOpenGL : public DrvVideo
{
    QWidget* parent;
    QVBoxLayout* layout;

    unsigned filter;
    bool aspectCorrect;
    bool vsync;

    bool nextFillSecondBuffer;
    bool lastFillSecondBuffer;
    bool twoScreenMode;

    struct Note {
        bool active;
        bool critical;
        string mes;
        int frames;
    };
    Note note;

    struct {
        bool enable;
        unsigned pos;
    } frame_counter;

    struct {
        bool enable;
        unsigned intensity;
    } scanlines;


    class GLWidget : public QGLWidget {
    public:
        QtOpenGL* wrapper;
        int (*glSwapInterval)(int);

     /*   QPaintEngine* paintEngine() const {
            return 0;
        }

        void paintEvent(QPaintEvent* event) {
            event->ignore();
        }*/

        GLWidget(QtOpenGL* _wrapper)
            : wrapper(_wrapper), glSwapInterval(0)
        {
            buffer = 0;
            secondBuffer = 0;
            scanBuffer = 0;
            textureWidth = 0;
            textureHeight = 0;
            gltexture = 0;
            gltextureSecond = 0;
            glScanTexture = 0;
        }
        ~GLWidget() {
            term();
        }

        unsigned* buffer;
        unsigned* secondBuffer;
        unsigned* scanBuffer;
        GLuint gltexture;
        GLuint gltextureSecond;
        GLuint glScanTexture;
        unsigned textureWidth;
        unsigned textureHeight;

        struct {
            unsigned usedWidth;
            unsigned leftWidth;
            unsigned inputWidth;
            unsigned inputHeight;
        } screen;

        void setTexture(unsigned width, unsigned height);
        void paintGL();
        void initializeGL();
        void resizeGL(int width, int height);
        void clear();
        void term();
        void drawText(const char* text, int length, int x, int y, bool warning = false);
        void setScanline();
        void drawScanline();
        void updateVsync();
        void createCGL();
        void calcRenderPos(bool secondScreen);
        void paintScreen(bool secondScreen);

    } *widget;

public:
    QtOpenGL();
    bool lock(unsigned*& data, unsigned& pitch, unsigned width, unsigned height);
    void unlock();
    void clear();
    void redraw();
    void init(QWidget* _parent, bool _vsync);
    void set_filter(unsigned _filter);
    void setAspectCorrect(bool state);
    string fetchText(bool& critical);
    void incommingSecondBuffer(bool state);

    void set_display_mes(string display_mes, int display_frames=90, bool critical=false) {
        note.active = true;
        note.critical = critical;
        note.mes = display_mes;
        note.frames = display_frames;
    }
    void set_counter(bool state) {
        frame_counter.enable = state;
    }

    void set_counter_pos(unsigned pos) {
        frame_counter.pos = pos;
    }

    void set_scanlines(bool state, unsigned intensity) {
        scanlines.enable = state;
        scanlines.intensity = intensity;
        if (scanlines.enable) {
            if (widget) {
                widget->setScanline();
            }
        }
    }

};

#endif // QTOPENGL_H
