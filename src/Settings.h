#pragma once

#include "SettingsBase.h"
#include "WiFiManager.h"
#include "InfluxDBCollectorBase.h"

struct SettingsData {
    NetworkSettings network;
    InfluxDBCollectorSettings influxDbCollector;
};

class Settings: public SettingsBase<SettingsData> {
    public:
        Settings();
        SettingsData* getSettings();

    protected:
        void initializeSettings();

    private:
        SettingsData settingsData;
};
