#pragma once

#include "utils/http_client.hpp"

class Crawler {

public:

    Crawler() = default;

    HttpResponse crawl(
        const ParsedUrl& url
    );


private:
    HttpClient client;
};