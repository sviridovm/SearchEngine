#include <unordered_set>
#include <string>
#include "queryCompiler/stopwords.h"
#include "index/stemmer/stemmer.h"

// Initialize the unordered_set at compile time
const std::unordered_set<std::string> StopWords::stopwords = []() {
    std::unordered_set<std::string> result;
    for (const auto& word : stopwords_array) {
        result.insert(std::string(word));
    }
    return result;
}();

bool StopWords::isStopword(const std::string& word) 
    {
    std::string s = standardize(word);
    std::string std_s = s.c_str();
    return stopwords.find(std_s) != stopwords.end();
    }

const std::unordered_set<std::string>& StopWords::getStopwords() {
    return stopwords;
} 