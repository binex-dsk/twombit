#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QFile>

struct fex_t;
typedef void BZFILE;

class File
{
public:
    File();

    enum Error { ERROR_OPEN, FINE };
    enum Type { t_gz, t_zip, t_rar, t_7z, t_tgz, t_tar, t_normal, t_bz2, t_tbz2 };
    enum Mode { r, w };

    unsigned char* fetchData();
    unsigned char* getData();
    void write(const unsigned char* buffer, unsigned length);
    QString getFn();
    QString getPath();
    unsigned getSize();
    Error getError();

    static QString getSupportedExtensions() {
        return "*.zip *.rar *.gz *.7z *.bz2 *.tar.gz *.tar.bz2 *.tgz *.tar";
    }
    QString getExtension();
    void unload();
    void setFile(QString _filePath);
    void completeTo256BChunk();
private:
    struct TAR {
        char name[100];   char _unused[24];
        char size[12];    char _padding[376];
    };

    QString filePath;
    QString fn;
    unsigned long fsize;
    QFile file;
    FILE* fp;
    BZFILE* bzfile;
    Type ftype;
    Error err_type;
    unsigned char* data;
    fex_t* fex;
    int fex_pos;

    bool open(Mode mode);
    unsigned char* read();
    unsigned char* extractFex();
    bool openFex();
    bool openBZ2();
    unsigned char* extractBZ2();

    Type get_file_type();
    unsigned char* readTar(unsigned char* buffer);
};

#endif // FILE_H

