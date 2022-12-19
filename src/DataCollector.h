#pragma once

#include "InfluxDBCollectorBase.h"

class DataCollector: public InfluxDBCollectorBase {
    public:
        DataCollector(Logger* logger,
                      WiFiManager* wifi,
                      InfluxDBCollectorSettings* settings,
                      NetworkSettings* networkSettings);
        bool shouldCollect();
        void collectData();
        void onPush();
        void beforePush();
        bool shouldPush();
};
