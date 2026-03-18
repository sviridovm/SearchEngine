// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.

#include "HtmlParser.h"
#include <iostream>
#include <cstring>

string stops[19] = {
   "the", 
   "be", 
   "to",
   "of", 
   "", 
   "a",
   "and"
   "an",
   "but",
   "in",
   "I",
   "or",
   "for",
   "that",
   "not",
   "have",
   "of",
   "on",
   "with"
};

const char * punctuation[23] = {
   "<",
   ">",
   ":",
   ";",
   "(",
   ")",
   "'",
   "\"",
   "\\",
   "{",
   "}",
   "=",
   ".",
   ",",
   "?",
   "!",
   "#",
   "%",
   "*",
   "$",
   "|",
   "[",
   "]"
};

void increment(const char * p, const char * end, int amt) {
   if (p + amt > end)
      p = end;
   else
      p += amt;
}

void HtmlParser::appendWord(const string &word,
                           vector< string > &vec, bool append) {
   for (auto &i : stops)
      if (word == i)
         return;
   for (int i = 0; i < 20; i++)
      if (word.contains(punctuation[i]))
         return;
   if (append && !vec.empty()) {
      vec.back() += word;
   } else {
      vec.push_back(word);
   }
}

bool HtmlParser::appendLink(const Link &link) {
   if (link.URL.charcount('?') > 0 || link.URL.charcount('#') > 0 || link.URL.charcount('+') > 0
      || link.URL.size() > 128 || link.URL.charcount('=') > 0)
      return false;
   if (link.URL.find("http") != -1 && link.URL.charcount('/') < 5)
      links.push_back(link);
   else
      return false;
   return true;
}

void HtmlParser::appendWord(const char * ptr, long len,
                           vector< string > &vec, const char *end) {
   string str(ptr, len, end);
   appendWord(str, vec, false);
}

string HtmlParser::complete_link(string link, string base_url)
{
   if (link.find("http://") == 0 || link.find("https://") == 0) {
      return link;
   } else if (link.find(".") != -1) {
      return "";
   } else { 
      ParsedUrl purl = ParsedUrl(base_url);
      base_url = purl.Service + "://" + purl.Host;
      if (*link.at(0) == '/')
         return base_url + link; 
      else
         return base_url + string("/") + link; 
   }
}

HtmlParser::HtmlParser( const char *buffer, const size_t length, const string &baseName )
   {
      bool inzone = false;
      const char *p = buffer;
      const char *end = buffer + length; 
      while (p != end && *p != '\0') {
         if (string(p, 14, end) == "User-agent: *\n") {
            increment(p, end, 14);
            inzone = true;
         }
         if (inzone) {
            if (string(p, 7, end) == "Allow: ") {
               p += 7;
               const char * start = p;
               while (*p != '\n' && p != end)
                  p++;
               appendWord(baseName + string(start, p - start, end), bodyWords, true);
            }
            if (string(p, 10, end) == "Disallow: ") {
               p += 10;
               const char * start = p;
               while (*p != '\n' && p != end)
                  p++;
               appendWord(baseName + string(start, p - start, end), bodyWords, true);
            }
            if (*p == '#')
               break;
         }
         p++;
      }
   }

HtmlParser::HtmlParser( const char *buffer, size_t length )
   {
   size_t i = 0;  
   bool inTitle = false;  
   bool indiscard = false;  
   bool inAnchor = false;
   bool inHead = false;
   bool inItalic = false;
   bool inBold = false;
   bool pastHtml = false;
   string url;  
   vector< string > curr_anchorText;  
   const char *p = buffer;  
   const char *end = buffer + length - 1; 

      if ( length == 0 )
         return;

   if ( *p == 'h' && *(p+1) == 't' && *(p+2) == 't' && *(p+3) == 'p' )
   {
      while ( *p != '\n' )
         p++;
      base = string(buffer, p - buffer, end);
      pURL = ParsedUrl(base);
      p++;
   }
   while (*p != '<' && p < end) {
      // this page is a redirect
      if (string(p, 9, end) == "ocation: ") {
         p += 9;
         const char *s = p;
         int size = 0;
         while (*p != '\r') {
            size++;
            p++;
         }
         appendLink(Link(string(s, size, end)));
         base = "";
         return;
      }
      p++;
   }
      

   while ( p < end )
      {
      if ( *p == '<' )
         {
         p++;  
         bool endflag = ( *p == '/' );  
         if ( endflag )
            p++;  

         const char *tag_start = p;  
         while ( p < end && *p != '>' && *p != '/' && !isspace( *p ) )
            p++;  

         DesiredAction action = LookupPossibleTag( tag_start, p );  
         if ( endflag )
            {
            if ( action == DesiredAction::Title )
               inTitle = false;  
            else if ( action == DesiredAction::Head )
               inHead = false;
            else if ( action == DesiredAction::Anchor )
               {
               if ( inAnchor )
                  {
                  inAnchor = false;  
                  if ( !url.empty() )
                     {
                     url = complete_link(url, base);
                     if (!url.empty()) {
                        Link curr_link( url );  
                        if (appendLink( curr_link ))
                           links.back().anchorText = curr_anchorText;  
                     }
                     }
                  curr_anchorText.clear();  
                  }
               else
                  {
                  while ( p < end && *p!= '>' )
                     p++;  
                  p ++;  
                  continue;  
                  }
               }
            else if ( action == DesiredAction::DiscardSection )
               indiscard = false;  
            }
         else
            {
            switch ( action )
               {
               case DesiredAction::Title:
                  if ( inTitle )
                     {
                     while ( p < end && *p != '>' )
                        p++;  
                     p++;  
                     continue;  
                     }
                  inTitle = true;  
                  break;  

               case DesiredAction::Base:
                  while ( p < end && base.empty() )
                     {
                     if ( strncmp( p, "href=", 5 ) == 0 && p[ 5 ] == '"' )
                        {
                        p += 6;  
                        const char *start = p;  
                        while ( *p != '"' && p < end ) 
                           p++;  
                        base.assign( start, p - start );  
                        break;  
                        }
                     else if ( memcmp( p, "/>", 2 ) == 0 )
                        break;  
                     p++;  
                     }
                  break;  

               case DesiredAction::Embed:
                  {
                  string embed_url = "";  
                  while ( p < end && *p != '>' )
                     {
                     if ( memcmp( p, "src=", 4 ) == 0 && p[ 4 ] == '"' )
                        {
                        p += 5;  
                        const char *start = p;  
                        while ( *p != '"' && p < end ) 
                           p++;  
                        embed_url.assign( start, p - start );  
                        break;  
                        }
                     p++;  
                     }
                  if ( !embed_url.empty() )
                     {
                        url = complete_link(embed_url, base);
                        appendLink( url );
                     }
                  break;  
                  }
               case DesiredAction::Anchor:
                  while ( p < end && *p != '>' )
                     {
                     if ( strncmp( p, "href=", 5 ) == 0 && p[ 5 ] == '"' ) 
                        {
                        if ( inAnchor )
                           {
                           if ( !url.empty() )
                              {
                              url = complete_link(url, base);
                              if (!url.empty()) {
                                 Link curr_link( url );  
                                 if (appendLink( curr_link )) 
                                    links.back().anchorText = curr_anchorText;  
                              }
                              
                              curr_anchorText.clear();  
                              }
                           curr_anchorText.clear();  
                           }
                        inAnchor = true;  
                        p += 6;  
                        const char *start = p;  
                        while ( *p != '"' && p < end )
                           p++;  
                        url.assign( start, p - start );  
                        break;  
                        }
                     p++;  
                     }
                  break;  

               case DesiredAction::Comment:
                  while ( p < end - 2 && !( p[ 0 ] == '-' && p[ 1 ] == '-' && p[ 2 ] == '>' ) ) 
                     p++;  
                  break;  

               case DesiredAction::DiscardSection:
                  indiscard = true;  
                  break;  
               case DesiredAction::HTML:
                  if (!pastHtml) {
                     while (string(p, 9, end) != "lang=\"en\"" && string(p, 9, end) != "lang=\"en-" 
                            && *p != '>' && p != end)
                        p++;
                     if (*p == '>' || p == end) {
                        links.clear();
                        bodyWords.clear();
                        headWords.clear();
                        titleWords.clear();
                        base = "";
                        
                        return; //this page isn't in english or doesn't specify; either way, we discard it
                     }
                     pastHtml = true;
                  }
                  
                     

               case DesiredAction::Head:
                  inHead = true;
                  break;
               case DesiredAction::OrdinaryText:
                  {
                  const char *start = p - 2;  
                  while ( p < end - 1 && *p != '<' && !isspace( *p ) && !( p[ 0 ] == '/' && p[ 1 ] == '>' ) )
                     p++;  
                  const string word( start, p - start, end );  
                  const bool emptyWord = ( p - start ) == 0;  

                  if ( !emptyWord && !indiscard )
                     {
                     if ( inTitle )
                        {
                        if ( !isspace( *( start - 1 ) ) && !titleWords.empty() )
                           appendWord(word, titleWords, true);
                        else 
                           appendWord(word, titleWords, false);

                        if ( inAnchor ) 
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;  
                           else
                              curr_anchorText.push_back( word );  
                           }
                        }
                     else if ( inHead )
                        {
                        if ( !isspace( *( start - 1 ) ) && !headWords.empty() )
                           appendWord(word, headWords, true);
                        else
                           appendWord(word, headWords, false);
                        if ( !isspace( *( start - 1 ) ) && !bodyWords.empty() )
                           appendWord(word, bodyWords, true);
                        else
                           appendWord(word, bodyWords, false);
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.push_back( word );
                           }
                        }
                     else if ( inAnchor )
                        {
                        if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                           {
                           curr_anchorText.back() += word;  
                           appendWord(word, bodyWords, true); 
                           } 
                        else
                           {
                           curr_anchorText.push_back( word );  
                           appendWord(word, bodyWords, false);
                           }
                        }
                     else
                        {
                        if ( !isspace( *( start - 1) ) ) 
                           appendWord(word, bodyWords, true);
                        else 
                           appendWord(word, bodyWords, false);
                        }
                     
                     continue;  
                     }
                  }
               default:
                  break;  
               }
            }
            
         while ( p < end && *p != '>' ) 
            p++;  
         p++;  
         }
      else
         {
         const char *start = p;  
         while ( p < end && *p != '<' && !isspace( *p ) && !( p[ 0 ] == '/' && p[ 1 ] == '>' ) )
            p++;  

         const bool isEmptyWord = ( p - start ) == 0;  

         if ( !isEmptyWord && !indiscard )
            {
            count++;
            if ( inTitle )
               {
               appendWord(start, p - start, titleWords, end);
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start, end );  
               }
            else 
               {
               appendWord(start, p - start, bodyWords, end);
               if ( inHead )
                  {
                  appendWord(start, p - start, headWords, end);
                  if ( inAnchor )
                     curr_anchorText.emplace_back( start, p - start, end );
                  }
               else if ( inAnchor )
                  {
                  curr_anchorText.emplace_back( start, p - start, end );  
                  }
               }


            }
         if ( p < end && *p == '<' ) 
            continue;  
         else 
            p++;  
         }
      }
   }