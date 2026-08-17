# C++ Search Engine

This is a C++ 20  fork/rewrite of the Search Engine I wrote for a class in Winter 2025

The original code can be found [here](git@github.com:SearchEngineDesign/SearchEngine.git)
Build everything with cmake, the only dependency is OpenSSL

``` bash
cmake -B build
cmake --build build
```

In order to crawl, configure everything and run the frontier and as many crawlers as you'd like

``` bash
./build/frontier_server

# On separate machines ...
./build/crawler
```

In order to run the search engine after crawling all of the documents, run 

```bash
./build/search_engine

# on the same machines with the indexes
./build/index_server

```
