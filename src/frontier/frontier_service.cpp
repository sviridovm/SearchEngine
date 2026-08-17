#include "frontier_service/frontier_service.hpp"
#include <cstdint>
#include <sys/types.h>

grpc::Status FrontierService::GetURLs(
    grpc::ServerContext* context,
    const frontier::URLRequest* request,
    frontier::URLResponse* response
) {

    (void)context;

    int count = request->count();

    while (count-- > 0 && !urlQueue.empty()) {
        response->add_urls(urlQueue.front());
        urlQueue.pop();
    }

    return grpc::Status::OK;
}


grpc::Status FrontierService::SubmitURLs(
    grpc::ServerContext* context,
    const frontier::URLBatch* request,
    frontier::SubmitResponse* response
) {
    (void)context;


    uint32_t accepted = 0;

    for (const auto& url : request->urls()) {
        if (seenURLs.insert(url).second) {
            urlQueue.push(url);
            accepted++;
        }
    }

    response->set_accepted(accepted);

    return grpc::Status::OK;
}