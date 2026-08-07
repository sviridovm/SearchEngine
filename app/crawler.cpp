#include "frontier_service/frontier_client.hpp"
#include "crawler/crawler.hpp"
#include "utils/ParsedUrl.hpp"


int main() {
    constexpr int numUrls = 100;

    FrontierClient frontierClient("localhost:50051");
    auto urlsToCrawl = frontierClient.getURLs(numUrls, "node1");

    Crawler crawler;

    for (const auto& url: urlsToCrawl) {
        const auto&  parsedURL = ParsedUrl(url);
        auto response = crawler.crawl(parsedURL);
        // if (response.statusCode == 200) {
            // frontierClient.submitCrawledURL(url, response.body);
        // }
    }


}