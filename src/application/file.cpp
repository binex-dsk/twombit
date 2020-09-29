#include "file.h"
#include <QRegExp>
#include <QMap>
#include <QDir>

#include <bzip2/bzlib.h>
#include <fex/fex.h>
#include <helper.h>
#include "exception.h"

#include <string>
using namespace std;

#define MAX_SIZE 8388608 //MAX SIZE 8 MB

File::File()
{
    data = 0;
    unload();
}

void File::setFile(QString _filePath)
{
    filePath = _filePath;
    ftype = get_file_type();
}

void File::unload() {
    free_mem(data);
    fn = filePath = "";
    fsize = 0;
    err_type = FINE;
    fex = 0;
    fp = 0;
    bzfile = 0;
}

void File::completeTo256BChunk() {
    unsigned _size = getSize();
    if (!_size || !data) return;
    unsigned _chunk = _size % 256;
    if ( _chunk == 0) return;
    unsigned missing = 256 - _chunk;

    unsigned char* dataNeeded = new unsigned char[fsize + missing];
    memset(dataNeeded, 0, fsize + missing);
    memcpy(dataNeeded, data, fsize);
    fsize += missing;
    free_mem(data);
    data = dataNeeded;
}

unsigned char* File::getData() {
    return data;
}

unsigned char* File::fetchData() {
    if (data) return data;
    try {
        switch(ftype) {
            case t_normal: {
                if (!open(r)) throw "";
                return read();
            }
            case t_zip:
            case t_rar:
            case t_7z:
            case t_gz:
            case t_tgz: {
                if (!openFex()) throw "";
                return ftype == t_tgz ? readTar( extractFex() ) : extractFex();
            }
            case t_bz2:
            case t_tbz2: {
                if (!openBZ2()) throw "";
                return ftype == t_bz2 ? extractBZ2() : readTar( extractBZ2() );
            }
            case t_tar: {
                if (!open(r)) throw "";
                return readTar( read() );
            }
            default: {
                throw "";
            }
        }
    } catch(...) {
        err_type = ERROR_OPEN;
        free_mem(data);
    }
    return data;
}

bool File::open(Mode mode) {
    QDir dir;
    fn = filePath;
    switch (mode) {
        case r:
            file.setFileName(filePath);
            if (!file.open(QIODevice::ReadOnly)) return false;
            fsize = file.size();
            if ((fsize == 0) || (fsize > MAX_SIZE+512)){
                return false;
            }
            return true;
        case w:
            if( !dir.mkpath( getPath() ) ) return false;
            file.setFileName(filePath);
            if(!file.open( QIODevice::WriteOnly)) return false;
            return true;
        default:
            return false;
    }
}

void File::write(const unsigned char* buffer, unsigned length) {
    if (!open(w)) {
        err_type = ERROR_OPEN;
        return;
    }
    file.write((const char*)buffer, length);
    file.close();
}

unsigned char* File::read() {
   data = new unsigned char[fsize];
   memset(data, 0, fsize);
   file.read((char*)data, fsize);
   file.close();
   return data;
}

//fex
bool File::openFex() {
    fex_err_t err = fex_open( &fex, filePath.toAscii().data() );
    if ( err != NULL ) return false;
    while ( !fex_done( fex ) ) {
        if (fex_stat( fex ) != NULL) return false;
        unsigned _size = fex_size( fex );
        if ((_size <= MAX_SIZE+512) && (_size > fsize)) {
            fsize = _size;
            fn = fn.fromAscii(fex_name( fex ));
            fex_pos = fex_tell_arc( fex );
        }
        if  (fex_next( fex ) != NULL) return false;
    }
    if (fsize == 0) return false;
    fex_rewind( fex );
    return true;
}

unsigned char* File::extractFex() {
    const void* p;
    if (fex_seek_arc( fex, fex_pos ) == NULL) {
        if (fex_data( fex, &p ) != NULL) throw "";
        data = new unsigned char[fsize];
        memcpy(data, (unsigned char*)p, fsize);
        fex_close( fex );
        return data;
    }
    throw "";
}
//bzip2 support
bool File::openBZ2() {
    fn = filePath;
    int end = fn.lastIndexOf(".");
    if (end != -1) fn = fn.left(end);

    int bzError;
    fp = fopen(filePath.toAscii().data(), "rb");
    if (!fp) return false;

    bzfile = BZ2_bzReadOpen(&bzError, fp, 0, 0, NULL, 0);

    if (bzError != BZ_OK) {
        fclose(fp);
        return false;
    }
    return true;
}

unsigned char* File::extractBZ2() {
    int bzError = BZ_OK;
    fsize = 0;
    char buf[4096];
    char* dataDecompressed = new char[MAX_SIZE];

    while (bzError == BZ_OK) {
        int nread = BZ2_bzRead(&bzError, bzfile, buf, sizeof buf);

        if (bzError == BZ_OK || bzError == BZ_STREAM_END) {
            memcpy(dataDecompressed + fsize, &buf, nread);
            fsize += nread;
        } else {
            BZ2_bzReadClose(&bzError, bzfile);
            fclose(fp);
            free_mem(dataDecompressed);
            throw "";
        }
    }

    BZ2_bzReadClose(&bzError, bzfile);
    fclose(fp);

    data = new unsigned char[fsize];
    memcpy(data, dataDecompressed, fsize);
    free_mem(dataDecompressed);
    return data;
}

//tar support
unsigned char* File::readTar(unsigned char* buffer) {
    fsize = 0;
    TAR* tar = (TAR*)buffer;
    unsigned length;
    char* start;
    char _fn[100];

    for( ; tar->name[0]; tar += 1 + (length+511) / 512 ) {
        sscanf( tar->size, "%o", &length);
        if ((length <= MAX_SIZE+512) && (length > fsize)) {
            fsize = length;
            sscanf( tar->name, "%s", &_fn);
            fn = QString::fromStdString(_fn);
            start = (char*)(tar+1);
        }
    }
    unsigned char* dataNeeded = new unsigned char[fsize];
    memcpy(dataNeeded, start, fsize);
    free_mem(data);
    data = dataNeeded;
    return data;
}

QString File::getFn() { //uncompressed filename without extension
    QString _fn = fn;
    int start = _fn.lastIndexOf("/");
    if (start != -1) _fn = _fn.mid(start + 1);
    int end = _fn.lastIndexOf(".");
    if (end != -1) _fn = _fn.left(end);
    return _fn;
}

QString File::getExtension() { //extension of uncompressed filename
    int end = fn.lastIndexOf(".");
    if (end == -1) return "";
    return fn.mid(end + 1);
}

QString File::getPath() {
    int end = filePath.lastIndexOf("/");
    if (end == -1) return filePath;
    return filePath.left(end + 1);
}

File::Error File::getError() {
    return err_type;
}

unsigned File::getSize() {
    return fsize;
}

File::Type File::get_file_type() {
    QString tmp_fn = filePath.toLower();
    QRegExp rx;
    QMap<QString, Type> suf;
    Type found = t_normal;

    suf[ ".+\\.tar\\.gz$"] =	t_tgz;
    suf[ ".+\\.tar\\.bz2$"] =	t_tbz2;
    suf[ ".+\\.tgz$"	] =		t_tgz;
    suf[ ".+\\.tar$"	] =		t_tar;
    suf[ ".+\\.gz$"	]     =		t_gz;
    suf[ ".+\\.zip$"	] =		t_zip;
    suf[ ".+\\.rar$"	] =		t_rar;
    suf[ ".+\\.7z$"	]     =		t_7z;
    suf[ ".+\\.bz2$"	] =		t_bz2;

    QMapIterator<QString, Type> i(suf);

    while (i.hasNext()) {
        i.next();
        rx.setPattern(i.key());
        if (rx.exactMatch (tmp_fn)) {
            if ((found == t_normal) || (i.value() == t_tgz) || (i.value() == t_tbz2)) {
                found = i.value();
            }
        }
    }
    return found;
}
