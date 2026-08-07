#include <grpcpp/grpcpp.h>
#include "frontier_service/frontier_service.hpp"

int main() {
    FrontierService service;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(
        "0.0.0.0:50051",
        grpc::InsecureServerCredentials()
    );

    builder.RegisterService(&service);

    auto server = builder.BuildAndStart();

    server->Wait();
}