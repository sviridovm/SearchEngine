// class for frontier server
// accepts connections from crawlers and sends them urls to crawl

#pragma once

#include "frontier.grpc.pb.h"
#include <queue>

class FrontierService final : public frontier::Frontier::Service {
public:
    grpc::Status GetURLs(
        grpc::ServerContext* context,
        const frontier::URLRequest* request,
        frontier::URLResponse* response
    ) override;

    grpc::Status SubmitURLs(
        grpc::ServerContext* context,
        const frontier::URLBatch* request,
        frontier::SubmitResponse* response
    ) override;

private:
    // TODO: implement state management here maybe ptr to a FrontierState class that handles the queue and seen URLs

    std::queue<std::string> urlQueue;
    std::unordered_set<std::string> seenURLs;
};