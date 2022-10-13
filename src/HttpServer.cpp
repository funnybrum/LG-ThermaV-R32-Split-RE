#include "Main.h"

char buffer[4096];

HttpServer::HttpServer(Logger* logger, NetworkSettings* networkSettings)
    :HttpServerBase(networkSettings, logger) {
}

void HttpServer::registerHandlers() {
    server->on("/", std::bind(&HttpServer::handle_root, this));
    server->on("/settings", std::bind(&HttpServer::handle_settings, this));
    server->on("/get", std::bind(&HttpServer::handle_get, this));
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
    sprintf_P(buffer,
              GET_JSON,
              WiFi.RSSI());
    server->send(200, "application/json", buffer);
}
