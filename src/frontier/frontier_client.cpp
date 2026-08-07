#include "frontier_service/frontier_client.hpp"


FrontierClient::FrontierClient(const std::string& address)
{
    auto channel = grpc::CreateChannel(
        address,
        grpc::InsecureChannelCredentials()
    );

    stub = frontier::Frontier::NewStub(channel);
}


std::vector<std::string> FrontierClient::getURLs(
    int count,
    const std::string& nodeId
) {
    frontier::URLRequest request;

    request.set_count(count);
    request.set_node_id(nodeId);

    frontier::URLResponse response;

    grpc::ClientContext context;

    grpc::Status status = stub->GetURLs(
        &context,
        request,
        &response
    );

    if (!status.ok()) {
        throw std::runtime_error(
            status.error_message()
        );
    }


    std::vector<std::string> urls;

    for (const auto& url : response.urls()) {
        urls.push_back(url);
    }

    return urls;
}


int FrontierClient::submitURLs(
    const std::vector<std::string>& urls
) {
    frontier::URLBatch request;

    for (const auto& url : urls) {
        request.add_urls(url);
    }

    frontier::SubmitResponse response;

    grpc::ClientContext context;

    grpc::Status status = stub->SubmitURLs(
        &context,
        request,
        &response
    );

    if (!status.ok()) {
        throw std::runtime_error(
            status.error_message()
        );
    }

    return response.accepted();
}