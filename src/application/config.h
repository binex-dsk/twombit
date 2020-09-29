#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Settings;

class Config
{
    int prepareIntSetting(Settings* setting, QString in);
    QString prepareStringSetting(Settings* setting, QString in);
    void initSettings();

public:
    bool load(QString path);
    bool save(QString path);
    Config();
};

#endif // CONFIG_H
