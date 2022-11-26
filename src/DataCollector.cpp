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
    append("heat_output_raw", thermaV.getPumpOutputPower());
    append("supply_temp", thermaV.getOutflowTemp());
    append("return_temp", thermaV.getInflowTemp());
    append("outdoor_temp", thermaV.getOutdoorTemp());
    append("supply_temp_s", tempSensors.getOutflowTemp(), 1);
    append("return_temp_s", tempSensors.getInflowTemp(), 1);
    append("set_temp", thermaV.getHeatingSetTemp());
    append("flow", thermaV.getFlow(), 1);
    thermaV.resetFreshC601();
}

bool DataCollector::shouldPush() {
    return false;
}

void DataCollector::onPush() {
}