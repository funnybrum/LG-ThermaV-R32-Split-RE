#include "Main.h"
#include "DataCollector.h"

DataCollector::DataCollector(Logger* logger,
                             WiFiManager* wifi,
                             InfluxDBCollectorSettings* settings,
                             NetworkSettings* networkSettings):
    InfluxDBCollectorBase(logger, wifi, settings, networkSettings) {
}

bool DataCollector::shouldCollect() {
    return millis() > 10000;
}

void DataCollector::collectData() {
    append("uptime", millis() / 1000);
    append("const_temp", 22.8f, 1);
}

bool DataCollector::shouldPush() {
    return false;
}   

void DataCollector::onPush() {
}