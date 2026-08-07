#pragma once


#include <cmath>
#include <cf/crypto.h>
#include <vector>
#include <mutex>
#include <string>



class BloomFilter
   {
   public:
    
   BloomFilter(){
         // Determine the size of bits of our data vector, and resize.
      
         // Use the formula: m = - (n * log(p)) / (log(2)^2)
         const unsigned int optimized_size = (int)(- (NUM_OBJECTS * log(FALSE_POSITIVE_RATE)) / (log(2) * log(2)));

         // Determine number of hash functions to use.
         const unsigned int n = (int)( (optimized_size / NUM_OBJECTS) * log(2) );
            
         this->num_hashes = n;
         bits.resize( optimized_size, false ); 
   }


   ~BloomFilter() = default;

   inline void insert( const std::string& s)
      {
      // Hash the string into two unique hashes.
      // const auto s_new = std::string(s.c_str());
      // Use double hashing to get unique bit, and repeat for each hash function.


      if (s.size() == 0)
         return;
      const auto hashes = crypto.doubleHash(s);
      for ( unsigned int i = 0; i < num_hashes; ++i )
         {
         const unsigned int index = ( (hashes.first + i * hashes.second) % bits.size() );
         bits[index] = true; 
         }
      }



   inline bool contains( const std::string& s ) 
      {
         // Hash the string into two unqiue hashes.

         // Use double hashing to get unique bit, and repeat for each hash function.
         // If bit is false, we know for certain this unique string has not been inserted.

         // If all bits were true, the string is likely inserted, but false positive is possible.

         if (s.size() == 0)
            return false;
         const auto hashes = crypto.doubleHash(s);
         for ( unsigned int i = 0; i < num_hashes; ++i ) 
            {
               const unsigned int index = ( (hashes.first + i * hashes.second) % bits.size() );
               if ( !bits[index] ) return false; 
            }
         return true;
      }


   private:

      static constexpr size_t NUM_OBJECTS = 10000000; // 10 million
      static constexpr double FALSE_POSITIVE_RATE = 0.005; // .5%
      
      unsigned int num_hashes;
      std::vector<bool> bits; 
      Crypto crypto;
   
   };
