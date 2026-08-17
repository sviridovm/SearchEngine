#pragma once


#include <memory>
#include <unordered_map>
#ifdef __AVX2__
#include <immintrin.h>  // For AVX2 intrinsics
#endif

#include <array>
#include <cstdio>
#include <string>
#include <vector>


class ShingleMap {
    public:
    static constexpr int SHINGLE_SIZE = 8;
    static constexpr int SIGNATURE_SIZE = 64;
    static constexpr int NUM_BANDS = 8;
    static constexpr int BAND_SIZE = 8; //SIGNATURE_SIZE / NUM_BANDS
    static const int& NUM_SIMILARITY() {
        static const int value = static_cast<int>(SIMILARITY_THRESHOLD * SIGNATURE_SIZE);
        return value;
    }
    static constexpr float SIMILARITY_THRESHOLD = 0.90;

    // FNV hash constants
    static constexpr size_t shingleFnvPrime = 1099511628211ul;
    static constexpr size_t shingleFnvOffset = 146959810393466560ul;
    static constexpr size_t shingleMod = ~size_t(0);
    static constexpr size_t p = 255;

    //constants for the signature hash function
    static constexpr auto a = std::to_array<size_t>({
        0x12345678, 0x23456789, 0x3456789A, 0x456789AB,
        0x56789ABC, 0x6789ABCD, 0x789ABCDE, 0x89ABCDEF,
        0x9ABCDEF0, 0xABCDEF01, 0xBCDEF012, 0xCDEF0123,
        0xDEF01234, 0xEF012345, 0xF0123456, 0x01234567,
        0x12345678, 0x23456789, 0x3456789A, 0x456789AB,
        0x56789ABC, 0x6789ABCD, 0x789ABCDE, 0x89ABCDEF,
        0x9ABCDEF0, 0xABCDEF01, 0xBCDEF012, 0xCDEF0123,
        0xDEF01234, 0xEF012345, 0xF0123456, 0x01234567,
        0x12345678, 0x23456789, 0x3456789A, 0x456789AB,
        0x56789ABC, 0x6789ABCD, 0x789ABCDE, 0x89ABCDEF,
        0x9ABCDEF0, 0xABCDEF01, 0xBCDEF012, 0xCDEF0123,
        0xDEF01234, 0xEF012345, 0xF0123456, 0x01234567,
        0x12345678, 0x23456789, 0x3456789A, 0x456789AB,
        0x56789ABC, 0x6789ABCD, 0x789ABCDE, 0x89ABCDEF,
        0x9ABCDEF0, 0xABCDEF01, 0xBCDEF012, 0xCDEF0123,
        0xDEF01234, 0xEF012345, 0xF0123456, 0x01234567
    });

    //constants for the signature hash function
    static constexpr auto b = std::to_array<size_t>({
        0x87654321, 0x98765432, 0xA9876543, 0xBA987654,
        0xCBA98765, 0xDCBA9876, 0xEDCBA987, 0xFEDCBA98,
        0x0FEDCBA9, 0x10FEDCBA, 0x210FEDCB, 0x3210FEDC,
        0x43210FED, 0x543210FE, 0x6543210F, 0x76543210,
        0x87654321, 0x98765432, 0xA9876543, 0xBA987654,
        0xCBA98765, 0xDCBA9876, 0xEDCBA987, 0xFEDCBA98,
        0x0FEDCBA9, 0x10FEDCBA, 0x210FEDCB, 0x3210FEDC,
        0x43210FED, 0x543210FE, 0x6543210F, 0x76543210,
        0x87654321, 0x98765432, 0xA9876543, 0xBA987654,
        0xCBA98765, 0xDCBA9876, 0xEDCBA987, 0xFEDCBA98,
        0x0FEDCBA9, 0x10FEDCBA, 0x210FEDCB, 0x3210FEDC,
        0x43210FED, 0x543210FE, 0x6543210F, 0x76543210,
        0x87654321, 0x98765432, 0xA9876543, 0xBA987654,
        0xCBA98765, 0xDCBA9876, 0xEDCBA987, 0xFEDCBA98,
        0x0FEDCBA9, 0x10FEDCBA, 0x210FEDCB, 0x3210FEDC,
        0x43210FED, 0x543210FE, 0x6543210F, 0x76543210
    });

    ShingleMap()
    {
        for (int i = 0; i < NUM_BANDS; ++i)
            bandTables[i] = nullptr;
    }

    ~ShingleMap() 
    {
        if (!shingles_signatures.empty())
            for (char* signature : shingles_signatures)
                if (signature != nullptr)
                    delete[] signature;
    }

    /*
    *add a document to the shingle map
    *@param document: the document as a vector of strings
    *modifies: bandTables, shingles_signatures
    *effects: adds the document to the shingle map, stores the signature in shingles_signatures
    */
    void addDocument(const std::vector<std::string> &document);
    //directly add a signature to the shingle map
    void addDocument(char* signature);
    /*
    *check if a document is similar to any of the documents in the shingle map
    *@param document: the document as a vector of strings
    *@returns: true if the document's signature meets the SIMILARITY_THRESHOLD
    *with any candidate signatures found through LSH, false otherwise
    */
    bool isSimilar (const std::vector<std::string> &document);
    //directly check if a signature is similar to any of the signatures in the shingle map
    bool isSimilar(char* signature);

    private:
    std::unordered_map<size_t, std::vector<size_t>>* getBandTable(int index) 
    {
    if (!bandTables[index])
        bandTables[index] = std::make_unique<std::unordered_map<size_t, std::vector<size_t>>>();
    return bandTables[index].get();
    }
    std::array<std::unique_ptr<std::unordered_map<size_t, std::vector<size_t>>>, NUM_BANDS> bandTables;  // Array of pointers to unordered_maps on heap
    std::vector<char*> shingles_signatures;
    size_t shingleHash(char (&band)[BAND_SIZE]);
    size_t shingleHash(const std::vector<std::string> &words, size_t start, size_t shingleSize = SHINGLE_SIZE);
    void sign(const std::vector<size_t> &shingles, char* signature);
    std::vector<size_t> createShingles(const std::vector<std::string> &words);
};
