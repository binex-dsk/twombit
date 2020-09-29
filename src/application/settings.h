#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVector>
#include <QString>

template <typename T> struct IsInteger { static bool const value = false; };
template <> struct IsInteger<int> { static bool const value = true; };

template <typename T> struct IsBool { static bool const value = false; };
template <> struct IsBool<bool> { static bool const value = true; };

template <typename T> struct IsQString { static bool const value = false; };
template <> struct IsQString<QString> { static bool const value = true; };

class Config;

class Settings {
public:
    enum Type { STR, DEC, BIN, HEX };
    static QVector<Settings*> list;
    static unsigned vector_pos;
private:
    friend class Config;
    void listSetting();

protected:
    int data, init;
    int minLimit, maxLimit;
    Type type;
    QString name, desc, str_data, str_init;
    bool saveable;

public:

    Settings(QString _name, QString _desc, int _data, Type _type, int _minLimit = 0, int _maxLimit = 0 );
    Settings(QString _name, QString _desc, QString _data);
    Settings(QString _name, int _data);

    static Settings* findSetting(QString name);
    static QStringList findSettingsByName(QString name);
    static void toggle(QString name);
    static int setInit(QString name);

    template<typename T> class type_info{};

    static int get(type_info<int> t, const QString& name){
        Settings* setting = findSetting(name);
        return setting->data;
    }

    static void set(type_info<int> t, const QString& name, int value){
        Settings* setting = findSetting(name);
        setting->data = value;
    }

    static bool get(type_info<bool> t, const QString& name){
        Settings* setting = findSetting(name);
        return setting->data == 0 ? false : true;
    }

    static void set(type_info<bool> t, const QString& name, bool value){
        Settings* setting = findSetting(name);
        setting->data = !value ? 0 : 1;
    }

    static QString get(type_info<QString> t, const QString& name){
        Settings* setting = findSetting(name);
        return setting->str_data;
    }

    static void set(type_info<QString> t, const QString& name, QString value){
        Settings* setting = findSetting(name);
        setting->str_data = value;
    }

    template<typename T> static T get(const QString& name) {
        static_assert(IsInteger<T>::value | IsBool<T>::value | IsQString<T>::value , "unsupported type of setting");
        return get(type_info<T>(), name);
    }

    template<typename T> static void set(const QString& name, T value) {
        static_assert(IsInteger<T>::value | IsBool<T>::value | IsQString<T>::value , "unsupported type of setting");
        set(type_info<T>(), name, value);
    }

    bool operator==(QString _data) {
        if (str_data == _data) return true;
        return false;
    }

    bool operator==(int _data) {
        if (data == _data) return true;
        return false;
    }

    void operator=(int _data) {
        data = _data;
    }

    void operator=(QString _data) {
        str_data = _data;
    }

    operator QString() {
        return str_data;
    }

    operator int() {
        return data;
    }

    void toggle() {
        data ^= 1;
    }

    int set_init() {
        data = init;
        return init;
    }
};

#endif // SETTINGS_H
