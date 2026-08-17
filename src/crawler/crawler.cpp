#include "crawler/crawler.hpp"

HttpResponse Crawler::crawl(
    const ParsedUrl& url
)
{
    return client.get(url);
}