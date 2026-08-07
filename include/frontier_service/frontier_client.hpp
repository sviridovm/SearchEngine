#pragma once

#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include "frontier.grpc.pb.h"

class FrontierClient {
public:
    FrontierClient(const std::string& address);

    std::vector<std::string> getURLs(
        int count,
        const std::string& nodeId
    );

    int submitURLs(
        const std::vector<std::string>& urls
    );

private:
    std::unique_ptr<frontier::Frontier::Stub> stub;
};