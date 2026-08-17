#pragma once
#include "index/index.h"
#include "cf/IndexBlob.h"
#include <memory>

class ISREndDoc;


class ISR {
public:
   virtual ~ISR();
   virtual const SerialPost *Next() = 0; // -> 
   virtual const SerialPost *NextDocument() = 0; // -> return 
   virtual const SerialPost *Seek(Location target) = 0; // -> return delta

   Location GetStartLocation() const; // -> return start
   Location GetEndLocation() const; // -> return end
   size_t GetMatchingDoc() const; // return result

   unsigned GetDocumentCount() const;
   unsigned GetNumberOfOccurrences() const;

   const SerialPost *GetCurrentPost() const;
   void SetCurrentPost(const SerialPost *p);
   void SetPostingList( const SerialPostingList *pl );

   
   protected:
      Location getStart() const;
      Location getEnd() const;

      void setStart(Location start);
      void setEnd(Location end);


   private:
      const SerialPostingList *postingList; 
      const SerialPost *curr; // current post 
      Location start = 0;
      Location end = 0;
      
      std::unique_ptr<ISREndDoc> _EndDoc;
      size_t matchingDocument; // final result
};

class ISREndDoc
{
public:
   const SerialPost *Seek(Location target); // -> return delta
   const SerialPost *NextDocument(); // -> return 
   const SerialPost *Next();

   // type-specific data
   size_t GetDocumentLength();
   size_t GetTitleLength();
   size_t GetUrlLength();

   void SetDocumentLength(size_t length);
   void SetTitleLength(size_t length);
   void SetUrlLength(size_t length);

   Location GetStartLocation(); // -> return start
   Location GetEndLocation(); // -> return end
   size_t GetMatchingDoc(); // return result

   const SerialPost *GetCurrentPost();
   void SetCurrentPost(const SerialPost *p);
   void SetPostingList( const SerialPostingList *pl );

private:
   size_t documentLength = 0;
   size_t titleLength = 0;
   size_t urlLength = 0;
   const SerialPostingList *postingList; 
   const SerialPost *curr; // current post 
   Location start = 0;
   Location end = 0;
   size_t matchingDocument;

};


class ISRWord : public ISR
{
public:
   ~ISRWord() override;
   const SerialPost *Next() override; // -> 
   const SerialPost *NextDocument() override; // -> return 
   const SerialPost *Seek(Location target) override; // -> return delta

};



class ISROr : public ISR
{
public:
   ~ISROr() {
      std::cout << "or destructor\n";
      for (size_t i = 0; i < NumberOfTerms; i ++) {
         if (Terms[i] != nullptr)
            delete Terms[i];
      }
      delete [] Terms;

      delete EndDoc;
   }

   ISR **Terms;
   unsigned int NumberOfTerms;
   Location GetStartLocation();
   Location GetEndLocation();

   const SerialPost *Seek(Location target) override;
   const SerialPost *Next() override;
   const SerialPost *NextDocument() override;


private:
   unsigned nearestTerm = 0;
   Location nearestStartLocation;
   Location nearestEndLocation;
};

class ISRAnd : public ISR
{
public:
   ~ISRAnd() {
      std::cout << "and destructor\n";
      for (size_t i = 0; i < NumberOfTerms; i ++) {
         if (Terms[i] != nullptr)
            delete Terms[i];
      }
      delete [] Terms;

      delete EndDoc;
   }

   ISR **Terms;
   unsigned int NumberOfTerms;

   const SerialPost *Seek(Location target) override;
   const SerialPost *Next() override;
   const SerialPost *NextDocument() override;

private:
   unsigned int nearestTerm = 0;
   unsigned int farthestTerm = 0;
   Location nearestStartLocation;
   Location farthestStartLocation;
};

class ISRPhrase : public ISR
{
public:
   ~ISRPhrase(){
      std::cout << "phrase destructor\n";
      for (size_t i = 0; i < NumberOfTerms; i ++) {
         if (Terms[i] != nullptr)
            delete Terms[i];
      }
      delete [] Terms;

      delete EndDoc;
   }

   ISR **Terms;
   unsigned int NumberOfTerms;

   const SerialPost *Seek(Location target) override;
   const SerialPost *Next() override;
   const SerialPost *NextDocument() override;

private:
   unsigned int nearestTerm;
   unsigned int farthestTerm;
   Location nearestStartLocation;
   Location nearestEndLocation;
};


class ISRContainer : public ISR
{
public:

   // ISRContainer( unsigned int countContained, unsigned int countExcluded );  // TODO: think about init
   ~ISRContainer( ) {
      std::cout << "container destructor\n";
      for (size_t i = 0; i < CountContained; i ++) {
         if (Contained[i] != nullptr)
            delete Contained[i];
      }
      for (size_t i = 0; i < CountExcluded; i ++) {
         if (Excluded[i] != nullptr)
            delete Excluded[i];
      }
      delete [] Contained;
      delete [] Excluded;

      delete EndDoc;
   }

   // Location Next( );
   const SerialPost *Seek( Location target );
   const SerialPost *Next( ); // next container isr
   const SerialPost *NextDocument();

   ISR **Contained, **Excluded;
   unsigned int CountContained, CountExcluded;

private:
   unsigned int nearestContained = 0, farthestContained = 0;
   Location nearestStartLocation, farthestStartLocation;
};


class Dictionary
{
public:
   ISR *OpenIsr(char *token);
   // The first line of index.txt, in order
   size_t GetNumberOfWords() const;
   size_t GetNumberOfUniqueWords() const; // number of posting list
   size_t GetNumberOfDocuments() const;

private:
   size_t numOfWords;
   size_t numOfUniqueWords;
   size_t numOfDocuments;
   size_t SetNum(size_t numOfWords, size_t numOfUniqueWords, size_t numOfDocuments);
};