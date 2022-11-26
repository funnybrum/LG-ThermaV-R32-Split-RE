#include "Main.h"

char buffer[4096];

HttpServer::HttpServer(Logger* logger, NetworkSettings* networkSettings)
    :HttpServerBase(networkSettings, logger) {
}

void HttpServer::registerHandlers() {
    server->on("/", std::bind(&HttpServer::handle_root, this));
    server->on("/settings", std::bind(&HttpServer::handle_settings, this));
    server->on("/get", std::bind(&HttpServer::handle_get, this));
    server->on("/debug", std::bind(&HttpServer::handle_debug, this));
    server->on("/debug/on", std::bind(&HttpServer::handle_debugOn, this));
    server->on("/debug/off", std::bind(&HttpServer::handle_debugOff, this));
    server->on("/set", std::bind(&HttpServer::handle_setTemp, this));
}

void HttpServer::handle_root() {
    server->sendHeader("Location","/settings");
    server->send(303);
}

void HttpServer::handle_settings() {
    wifi.parse_config_params(this);
    dataCollector.parse_config_params(this);

    char network_settings[strlen_P(NETWORK_CONFIG_PAGE) + 32];
    wifi.get_config_page(network_settings);

    char influx_settings[strlen_P(INFLUXDB_CONFIG_PAGE) + 64];
    dataCollector.get_config_page(influx_settings);

    sprintf_P(
        buffer,
        CONFIG_PAGE,
        network_settings,
        influx_settings);
    server->send(200, "text/html", buffer);
}

void HttpServer::handle_get() {

    char state[10];
    switch (thermaV.getMode()) {
        case HP_OFF:
            strcpy(state, "off");
            break;
        case HP_HEAT:
            strcpy(state, "heat");
            break;
        case HP_DHW_HEAT:
            strcpy(state, "dhw_heat");
            break;
        case HP_COOL:
            strcpy(state, "unknown");
            break;
        default:
            strcpy(state, "unknown");
    }

    sprintf_P(buffer,
              GET_JSON,
              WiFi.RSSI(),
              thermaV.getOutputPower(),
              thermaV.getPumpOutputPower(),
              thermaV.getFlow(),
              thermaV.getInflowTemp(),
              thermaV.getOutflowTemp(),
              thermaV.getOutdoorTemp(),
              thermaV.getHeatingSetTemp(),
              thermaV.getDhwSetTemp(),
              tempSensors.getInflowTemp(),
              tempSensors.getOutflowTemp(),
              thermaV.getIndoorTemp(),
              state,
              thermaV.isOutdoorUnitRunning()?"true":"false");
    server->send(200, "application/json", buffer);
}

void HttpServer::handle_debugOn() {
    thermaV.setDebug(true);
    server->send(200, "application/json");   
}

void HttpServer::handle_debugOff() {
    thermaV.setDebug(false);
    server->send(200, "application/json");   
}

void HttpServer::handle_debug() {
    thermaV.getOutput();
    server->send(200, "application/json");   
}

void HttpServer::handle_setTemp() {
     if (server->args() != 1
        || (!server->argName(0).equals("ufh") && !server->argName(0).equals("dhw"))
        || server->arg(0).length() != 2) {
        server->send(400, "text/plain", "try with /set?ufh=32 or /set?dhw=45");
        return;
    }

    uint8_t temp = atoi(server->arg(0).c_str());
    if (temp < 10 || temp > 60) {
        server->send(400, "text/plain", "supported temperature is from 10C to 60C");
        return;
    }

    if (server->argName(0).equals("ufh")) {
        thermaV.setUfhTemp(temp);
    }

    if (server->argName(0).equals("dhw")) {
        thermaV.setDhwTemp(temp);
    }

    server->send(200);
}