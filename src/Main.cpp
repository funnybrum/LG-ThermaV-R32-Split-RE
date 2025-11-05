#include "Main.h"

Logger logger = Logger(false);
Settings settings = Settings();
WiFiManager wifi = WiFiManager(&logger, &settings.getSettings()->network);
HttpServer httpServer = HttpServer(&logger, &settings.getSettings()->network);
DataCollector dataCollector = DataCollector(
    &logger,
    NULL,
    &settings.getSettings()->influxDbCollector,
    &settings.getSettings()->network);

ThermaV thermaV = ThermaV();
DS18B20 tempSensors = DS18B20(4, 20000);
CirculationPump dhwCirculationPump = CirculationPump(19);
ActuatedValve dhwValve = ActuatedValve(0, 2);

void setup()
{ 
    Serial.begin(300);
    while (!Serial) {
        delay(1);
    }

    logger.begin();
    settings.begin();
    wifi.begin();
    httpServer.begin();
    dataCollector.begin();
    tempSensors.begin();

    wifi.connect();

    thermaV.begin();
    dhwCirculationPump.begin();
    dhwValve.begin();
}

void loop() {
    settings.loop();
    wifi.loop();
    httpServer.loop();
    dataCollector.loop();
    tempSensors.loop();
    dhwCirculationPump.loop();
    dhwValve.loop();
    
    thermaV.loop();

    delay(1);
}
