#ifndef UTILITY_H    // to avoid including this file twice
#define UTILITY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <ctype.h>
#include "standard.h"

#define STOPNUM 300
#define WDSIZE  16

//************************************************************************
//
//  classStopList
//  -------------
//  Maintain the stop list for stop words removal.
//
//  Local variables:
//  stopPtr: string pointer to the list of stop words loaded from a stoplist
//           file. The stop words are previously stored in file in increasing
//           order.
//  numStopWords: number of stop words used.
//
//  Local functions:
//  IsStopWord(char*): return true if the input word is a stop word.
//  GetNumOfStopWords(): return the number of stop words in list
//
//************************************************************************
class StopList  {
protected:
   char *stopPtr;
   int  numStopWords;
public:
   StopList (const char *dictionary);
   ~StopList ();
   bool IsStopWord (char *key);
   int GetNumOfStopWords() { return numStopWords; }
};


//************************************************************************
//
//  class HTUtilities
//  -----------------
//  It contains the set of utility functions to perfrom sorting and
//  string operations
//
//  See below for Meanings of the functions used.
//
//************************************************************************
class HTUtilities
{
   public:
      HTUtilities() {};
      virtual ~HTUtilities() {};
      virtual char* FloatToString(float value);
              // convert a floating number value to string 
      virtual char* IntToString(int value);
              // convert an integer number value to string 
      virtual char* IntFloatToString(int freq, float wt);
              // packed an integer and a floating number into a string
              // separated by space 
      virtual char* IntStringArrayToString(int n, const char **str);
              // packed an integer n follow by n strings into a long string
              // each item is separated by spaces 
      virtual void StringToIntFloat(const char *str, int &freq, float &wt);
              // convert a string created previously using IntFloatToString
              // into its integer and float constituents 
      virtual char** StringToIntStringArray(const char *packedStr, int &n);
              // convert a packed string created previously by
              // IntStringArrayToString into its constuents 
      virtual char* GetFullImageName(const char* fullTextName);
              // obtain the full path name from the correponding image file
              // of the fullTextName. Here it assumes that the text are stored
              // under ../TEXT/, whereas the images are stored under ../IMAGE/.
      virtual char* GetFullTextName(const char* fullImageName);
              // corresponding name for text
      virtual char* JustNameExt(const char* fullName);
              // extract just the name with extension
      virtual char* JustName(const char* fullName);
              // extract the just the name without path and file extension
      virtual SimNode* SortList (SimNode *list);
              // sort in input linked lsit of type SimNode in ascending 
              //order based on similarity

     virtual char *FloatToFormatString (float value);
          // convert a floating value into a string with certain length
          // no zero brfore the point. The format of string is ".xxxx".
          //----Shen Mojun 

     virtual char *FloatArrayToString(int n, float *value);
             // packed  n float into a long string
             // each item is separated by spaces --- Shen Mojun
     
     virtual float *StringToFloatArray(const char *packedStr, int n);
              // convert a packed string created previously by
              // FloatArrayToString into its constituents---Shen Mojun
};


//************************************************************************
//  class Stemming
//  --------------
//  Stemm a word using the porter's stemming algorithm
//  Local functions:
//   Stem(key): Stem key and returns the result in key.
//
//************************************************************************
class Stemming  {
   public:
      Stemming();
      ~Stemming();
      int Stem(char* key);  // retruns the stem of key in key

   protected:
      int VCNum (char *word, int i);
      int Vowel (char *word, int i);
      int DoubleLetter (char *word, int i);
      int Endcvc (char *word, int i);
};

#endif   // closing bracket for #ifndef UTILITY_H

