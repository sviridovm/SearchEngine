//stemmer.h
#pragma once
#include "utf8proc/utf8proc.h"


//takes: UTF-8 encoded string with whatever, in unicode
//returns: ASCII string
std::string standardize (const std::string& word);

//algorithm from Algorithm for Suffix Stripping by M.F. Porter
//MODIFIES: word
void stem (std::string s); 

//takes: string, returns stemmed string
std::string stemWord(std::string s);

//takes: vector of strings, stems each string
//MODIFIES: each string in the vector
void stem ( std::vector<std::string> v );