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
    // dataCollector.begin();

    wifi.connect();

    thermaV.begin();
}

void loop() {
    settings.loop();
    wifi.loop();
    httpServer.loop();
    // dataCollector.loop();
    
    thermaV.loop();

    delay(1);
}
