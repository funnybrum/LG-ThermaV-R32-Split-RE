#include "Main.h"
#include "DataCollector.h"

DataCollector::DataCollector(Logger* logger,
                             WiFiManager* wifi,
                             InfluxDBCollectorSettings* settings,
                             NetworkSettings* networkSettings):
    InfluxDBCollectorBase(logger, wifi, settings, networkSettings) {
}

bool DataCollector::shouldCollect() {
    return thermaV.freshC601();
}

void DataCollector::collectData() {
    append("heat_output", thermaV.getOutputPower());
    thermaV.resetFreshC601();
}

bool DataCollector::shouldPush() {
    return false;
}   

void DataCollector::onPush() {
}