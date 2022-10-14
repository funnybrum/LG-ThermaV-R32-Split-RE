#include "Main.h"
#include "DataCollector.h"

DataCollector::DataCollector(Logger* logger,
                             WiFiManager* wifi,
                             InfluxDBCollectorSettings* settings,
                             NetworkSettings* networkSettings):
    InfluxDBCollectorBase(logger, wifi, settings, networkSettings) {
}

bool DataCollector::shouldCollect() {
    return thermaV.getOutputPower() > -1 &&
        thermaV.getOutputPower() < 20000;
}

void DataCollector::collectData() {
    append("heat_output", thermaV.getOutputPower());
    append("uptime", millis()/1000);
}

bool DataCollector::shouldPush() {
    return false;
}   

void DataCollector::onPush() {
}