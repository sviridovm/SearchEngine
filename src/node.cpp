#include "crawler/crawler.hpp"
#include <memory>


// using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds


void Node::handle_signal(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nInterrupt received. Shutting down gracefully..." << std::endl;
        shutdown(true); 
        urlReceiver->stopListening();
    }
}

Node::Node(const unsigned int id_in, const unsigned int numNodes): 
    id(id_in), 
    numNodes(numNodes), 
    keepRunning(true),
    frontier(numNodes, id_in),

    crawlResultsQueue(),
    urlReceiver(),
    tPool(NUM_CRAWL_THREADS + NUM_PARSER_THREADS + NUM_INDEX_THREADS + 1)
{
    urlReceiver = std::make_shared<UrlReceiver>(id, 8080, &frontier);
    
}



void Node::start(const char * seedlistPath, const char * bfPath) {
    std::cout << "Node " << id << " started." << std::endl;

    if (frontier.buildFrontier(seedlistPath, bfPath) == 1) {
        shutdown(false);
        return;
    }


    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);

    for (size_t i = 0; i < NUM_CRAWL_THREADS; i++)
    {
        tPool.submit(crawlEntry, (void*) this);
    }

    for (size_t i = 0; i < NUM_PARSER_THREADS; i++)
    {
        tPool.submit(parseEntry, (void*) this);
    }

    tPool.submit(urlReceiver->listenerEntry, (void*) urlReceiver.get());
    
    
    for (size_t i = 0; i < NUM_INDEX_THREADS; i++)
    {
        tPool.submit(indexEntry, (void*) this);
    }
}

void Node::shutdown(bool writeFrontier) {
    if (keepRunning) {
        keepRunning = false;
        std::cout << frontier.size() << " items in frontier." << std::endl;
        parseResultsQueue.stop();
        if (writeFrontier)  
            frontier.writeFrontier(); 
        frontier.startReturningEmpty();
        crawlResultsQueue.stop();
        std::cout << "Shutdown complete." << std::endl;
    }
}



