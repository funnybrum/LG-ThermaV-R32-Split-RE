#pragma once

#define HOSTNAME "esp32-blank"

#include "esp32-base.h"
#include "HttpServer.h"
#include "DataCollector.h"
#include "DS18B20.h"
#include "ThermaV.h"

extern WiFiManager wifi;
extern Logger logger;
extern Settings settings;
extern DataCollector dataCollector;
extern DS18B20 tempSensors;
extern ThermaV thermaV;