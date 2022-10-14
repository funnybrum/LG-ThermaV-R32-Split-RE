#include "Main.h"
#include "DataCollector.h"

DataCollector::DataCollector(Logger* logger,
                             WiFiManager* wifi,
                             InfluxDBCollectorSettings* settings,
                             NetworkSettings* networkSettings):
    InfluxDBCollectorBase(logger, wifi, settings, networkSettings) {
}

bool DataCollector::shouldCollect() {
    return millis() > 60000;
}

void DataCollector::collectData() {
    
    append("heat_output", thermaV.getOutputPower());
}

bool DataCollector::shouldPush() {
    return false;
}   

void DataCollector::onPush() {
}