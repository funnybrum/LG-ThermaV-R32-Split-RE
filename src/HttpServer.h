#pragma once

#include "Main.h"
#include "HttpServerBase.h"
#include "WebPages.h"

class HttpServer: public HttpServerBase {
    public:
        HttpServer(Logger* logger, NetworkSettings* networkSettings);
        void registerHandlers();
        void handle_root();
        void handle_settings();
        void handle_get();
        void handle_debugOn();
        void handle_debugOff();
        void handle_debug();
};
