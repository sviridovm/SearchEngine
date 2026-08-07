#include <fstream>
#include <queue>
#include <unordered_map>
#include <chrono>
#include <sstream>

#include "frontier/BloomFilter.hpp"
#include <filesystem>


struct DomainState {
    std::queue<std::string> urls;
    int active = 0;
    size_t next_allowed = 0;
};  

class Frontier {
private:
    std::filesystem::path log_file_path;
    BloomFilter bloom_filter;

    std::queue<std::string> pending;
    std::unordered_map<std::string, long long> in_progress;

    long long now() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    void append_log(const std::string& url, const std::string& state) {
        std::ofstream out(log_file_path, std::ios::app);
        out << url << "|" << state << "|" << now() << "\n";
    }

    void load() {
        std::ifstream in(log_file_path);
        std::string line;

        while (std::getline(in, line)) {
            std::stringstream ss(line);
            std::string url, state, ts;

            std::getline(ss, url, '|');
            std::getline(ss, state, '|');
            std::getline(ss, ts, '|');

            bloom_filter.insert(url);

            if (state == "pending") {
                pending.push(url);
            }
        }
    }

public:
    Frontier(const std::filesystem::path& path) : log_file_path(path) {
        std::ifstream f(path);
        if (f.good()) {
            load();
        }
    }

    bool add(const std::string& url) {
        if (bloom_filter.contains(url)) return false;

        bloom_filter.insert(url);
        pending.push(url);
        append_log(url, "pending");

        return true;
    }

    std::string get(int lease_seconds = 30) {
        if (pending.empty()) return "";

        std::string url = pending.front();
        pending.pop();

        in_progress[url] = now() + lease_seconds;
        append_log(url, "in_progress");

        return url;
    }

    void mark_done(const std::string& url) {
        in_progress.erase(url);
        append_log(url, "done");
    }

    void mark_failed(const std::string& url) {
        in_progress.erase(url);
        pending.push(url);
        append_log(url, "pending");
    }

    void reap_expired() {
        long long t = now();

        for (auto it = in_progress.begin(); it != in_progress.end();) {
            if (it->second < t) {
                pending.push(it->first);
                append_log(it->first, "pending");
                it = in_progress.erase(it);
            } else {
                ++it;
            }
        }
    }
};