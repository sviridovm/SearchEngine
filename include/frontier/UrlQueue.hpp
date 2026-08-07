#pragma once

#include <queue>
#include <stdexcept>
#include <string>
#include <vector>
#include "static_ranker/StaticRanker.h"
#include "cf/ParsedUrl.hpp"


// Data Structure that abstracts random K access to a queue of URLs
class UrlQueue {
    
    private:
        std::vector<std::string> urls;

        //? CAN THIS BE A VECTOR ?
        //* YUH BECAUSE ORDERING DOESN:T MATTER IF ITS ALREADY IIN THE POOL
        std::priority_queue<std::string, std::vector<std::string>, StaticRanker> urlPool;

        
        static constexpr size_t MAX_POOL_CANDIDATES = 20000;
        static constexpr size_t MAX_POOL_SIZE = 5000;

        void fillUrlPool() {
            // select random K urls from urls and add them to urlPool

            const size_t k = std::min(urls.size(), MAX_POOL_SIZE);
            const size_t N = std::min(urls.size(), MAX_POOL_CANDIDATES);


            unsigned int count = 0;

            while (count < N) {
                const unsigned int randomIndex = rand() % urls.size();
                const std::string& selectedUrl = urls[randomIndex];
                std::string curr = ParsedUrl(selectedUrl).Host;
               

                count++;


                urlPool.push(selectedUrl);
                   
                 // swap the selected url with the last url in the vector to efficiently remote it
                std::swap(urls[randomIndex], urls[urls.size() - 1]);
                urls.pop_back(); 


            }

            for (size_t i = 0; i < (N - k); i++) {
                std::string top = urlPool.top();
                urls.emplace_back(top);
                urlPool.pop();
            }
                

            // remaining 5000 urls are sorted in reverse

        }

    public:

        const std::vector<std::string>& getUrls() const{
            return urls;
        }

        UrlQueue() = default;

        void addUrl(const std::string &url) {
            urls.push_back(url);
        }

        std::string getNextUrl() {

            if (urlPool.empty() and urls.empty()) {
                throw std::runtime_error("No URLs available");
            }

            if (urlPool.empty()) {
                fillUrlPool();
            }

            std::string nextUrl, curr;
            nextUrl = urlPool.top();
            urlPool.pop();
            curr = ParsedUrl(nextUrl).Host;
            return nextUrl;
        }

        inline bool empty() const {
            return urls.empty() and urlPool.empty();
        }

        inline bool vecempty() const {
            return urls.empty();
        }

        inline int size() const {
            return (urls.size() + urlPool.size());
        }

        const std::string &at(int i) const {
            return urls[i];
        }

        inline void erase(int i) {
            std::swap(urls[urls.size() - 1], urls[i]);
            urls.pop_back();
        }

};

