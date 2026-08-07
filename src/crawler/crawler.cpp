#include "crawler/crawler.hpp"


Crawler::Crawler()
    : client()
{}

HttpResponse Crawler::crawl(
    const ParsedUrl& url
)
{
    return client.get(url);
}