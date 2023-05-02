#pragma once

#define HOSTNAME "lg-therma-v"
#define HEAT_PUMP_COMMAND_TIMEOUT_MS 5*60*1000

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