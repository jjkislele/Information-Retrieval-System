#ifndef STANDARD_H    // to avoid including this file twice
#define STANDARD_H

// define constants for relevant feedback
#define UNRANKED    0    // node has not been judged for relevance
#define NONRELEVANT 1    // node has been judged as non-relevant
#define RELEVANT    2    // node has been judged as relevant
#define SEEN        3    // node has been seen by users before

// boolean data type
//enum BOOLEAN {false, true};

typedef float DOUBLE;  // not necessary to use double precision here

//#define NULL           0
#define PI             3.1416

#define max(a,b) ( ((a)>(b)) ? (a) : (b) )
#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define sq(a) ( (a)*(a) )
#define abs(a) ( ((a)>0) ? (a) : -(a) )


// data structure for storing a term with frequency in a linked list
struct Data 	{
   char *name;
   int freq;
   float weight;
   struct Data *next;
};

// data structure for storing a ranked term with similarity values
struct SimNode 	{
   char *name;
   float sumWqWd;    // accumulate the sum of Wq*Wd
   float similarity; // the similarity value between doc name and query
   int   status;     // whether the nodes has been judged RELEVANT or SEEN
   struct SimNode *next;
};

// data structure for storing the directories needed for retrieval operation
typedef struct DIRECTORIES {
   char* mainDIR;     // main user directory - use to create temp files
   char* invertedDIR; // dir where index for inverted file is stored 
   char* directDIR;   // contain index of direct files
   char* imageBrowseDIR; // dir used for browsing images
   char* imageEXT;    // ext of image files
   char* textEXT;     // extension of text files
};

// data structure for storing a combined term with similarity values
struct CombNode 	{
   char  name[200];
   float sumWqWd;    // accumulate the sum of Wq*Wd for text
   float clrsim;
   float txtusim;
   float txtsim;     // the similarity value between doc name and query
   int   status;     // whether the nodes has been judged RELEVANT or SEEN
   struct CombNode *next;
}; // Add by Shen Mojun 

#endif   // closing bracket for #ifndef STANDARD_H
