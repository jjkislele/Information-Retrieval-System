//=========================================================================
//
//  filesys1.h
//  **********
//  Class definitions for:
//  HashTable: an implementation of hash-table (current implementation
//             based on UNIX's ndbm
//  StringStemHT: a sub-class of HashTable to generate the stem list from an
//             input string
//  FIleStemHT: a sub-class of StringStemHT to generate the stem list from
//             an inout file.
//  MMIndexFile: a superclass of all indexfile classes. It defines prototype
//               for text, color, texture index files.
//  InvertedIndexFile: defines an inverted index file prototype.
//  TextIndexFile: defines an inverted text index for free-text retrieval.
//
//  Implemented by:  T.S. Chua
//     Date of creation: June 1992
//     Last Modified:    3rd May 1995
//
//=========================================================================

#ifndef FILESYS1_H    // to avoid including this file twice
#define FILESYS1_H

#include <db1/ndbm.h>
#include "standard.h"
#include "utility.h"

#define RW          false
#define RD_ONLY     true
#define TO_DELETE   true
#define TO_REPLACE  true

// Added by TEO KAH CHOO on May 1995
#define LINELEN  		500
#define KEYWORD_HEADERPOS  	8
#define SUBJECT_HEADERPOS  	8
#define IMAGENAME_HEADERPOS   	20
#define IMAGENAME_LEN		150
#define DIRNAME_LEN		100
#define FILENAME_LEN		DIRNAME_LEN + IMAGENAME_LEN

// Function Defintion
// Added by TEO KAH CHOO on May 1995
int ConstructStr (char *newStr, char *oldStr, char delimiter);
char* nstrcpy (char *s, int start_pos);

//************************************************************************
//
//  class HashTable
//  ---------------
//  It uses ndbm of UNIX to provide hash-table functionalities. It could
//  be replaced by a suitable hash-table implmentation in future.
//
//  Local variables:
//  N: number of entries in HT
//  dbPtr: pointer to HT in UNIX file system
//  name:  name of HT. If no name is given, it is name NoName
//         *Note: name should be given properly to avoid duplicate, which
//                will cause problem.
//  toDelete: bool varaible to indicate whether the NDBM files should
//            be deleted permenently on exit
//
//  Meanings of functions are self-explanatory
//
//************************************************************************
class HashTable
{
   protected:
      DBM *dbPtr;       // pointer to hash-table
      char *name;
      bool toDelete; // to delete NDBM files on exit

   public:
      HashTable (const char *fileName, bool readOnly=true,
                 bool todelete=false);
      virtual ~HashTable ();
      virtual const char *Retrieve (const char *key);
      virtual int Insert (const char *key, const char *content);
      virtual int Replace (const char *key, const char *content);
      virtual int Delete (const char *key);
      virtual HashTable *DuplicateHT (const char *newName);
               // duplicate a new hashTable under the name <newName>
      virtual bool Contain (const char *name); 
               // return true if <name> is in HT
      virtual int GetDBSize();
      virtual const char *GetFirstKey ();
      virtual const char *GetNextKey ();
      virtual void ToDeleteOnExit () { toDelete = true; }
               // call to delete HashTable & ndbm files permenently
      virtual DBM *GetDBPtr () { return dbPtr; }
      virtual const char *GetDBName () { return name; }
      virtual bool GetToDelete() { return toDelete; }
      virtual void DisplayContents ();
 
   protected:
      virtual datum *MakeDatum (const char *str);
              // make a strcut of type datum from str - requred for ndbm
      virtual void RemoveDatum (datum *ptr);
              // remove the storage allocated to datum and its content
      virtual void DeleteNDBMFiles ();
              // delete the .pag and .dir files for HashTable DBMName
};


//************************************************************************
//
//  class StringStemHT <- HashTable
//  -------------------------------
//  Manage Stemming List generated etiher from an input string
//
//  Local variables:
//  maxFreq: the max frequency of stems in the whole list
//  util: utilities to perform sorting and string operations.
//
//  Local functions:
//  self-explanatory.
//
//************************************************************************
class StringStemHT: public HashTable {
   protected:
      int  maxFreq;      // store the max freq of stems in file
      HTUtilities *util; // utitlies to perform sorting & string operations
   public:
      StringStemHT (const char *name, bool readOnly=true,
                    bool todelete=false);
      virtual ~StringStemHT ();
      virtual int GenerateStemList (const char* string, StopList *stopWords,
                                    Stemming *stemAlgo);
      virtual void AddStem (const char *aStem);
      virtual void UpdateStemFreqWt (const char *aStem, int freq, float wt);
      virtual void RetrieveStemFreqWt (const char *aStem, int &freq, float &wt);
      virtual StringStemHT *Duplicate (const char *newName);
               // duplicate a new hashTable under the name <newName>
      virtual int GetMaxFreq() { return maxFreq; }
      virtual void SetMaxFreq(int maxFreq1) { maxFreq = maxFreq1; }
};


//************************************************************************
//
//  class FileStemHT <- StringStemHT
//  --------------------------------
// sub-class of StringStemHT. It generates the stem list (with frequency
// information) for an input file, "fileName"
// It uses a StopList object and Stemming object supplied by
// calling program
//
//************************************************************************
class FileStemHT: public StringStemHT {
   public:
      FileStemHT (const char *name, bool readOnly=true,
                  bool todelete=false):
         StringStemHT(name, readOnly, todelete) {};
      virtual ~FileStemHT () {};
      virtual int GenerateStemList (const char* fileName, StopList *stopWords,
                                    Stemming *stemAlgo);
};


//************************************************************************
//
//  class MMIndexFile 
//  -----------------
//  Super class for all index files designed to index MM data.
//  This includes text
//
//  The class hierarchy is:
// 
//                        MMIndexFile
//                            |
//                    InvertedIndexFile  
//                            |
//                      TextIndexFile  
//
// It defines all functions common to all index files
//
//  ****NOTE: the fullpath name of all files are stored. This is required
//            during relevance feedback to access the actual file for
//            efficiency reason. It's much faster to get all terms in a file
//            from original file than from headerDB.
//
//
//  Local variables:
//  IFName:   name of index file
//  dir:      a struct containing the names of index directories and others
//  readOnly: set to true if the inverted file is opened for read only;
//        otherwise is is opened for read/write, and auto-create if not exist
//  newIndex: set to true if exisiting index file is to be replaced.
//  util: object that provides functions needed for inverted file operations
//
//  Meanings of functions are self-explanatory
//
//************************************************************************

class MMIndexFile
{
   protected:
      char* IFName;
      DIRECTORIES *dir;
      bool readOnly;
      bool newIndex;
      HTUtilities *util;

   public:
      MMIndexFile (const char *name, DIRECTORIES *dirs, bool rdOnly,
                   bool newindex);
      virtual ~MMIndexFile();
      virtual void OpenMainIndexFiles() =0;   // open or create main index files
      virtual void CloseMainIndexFiles() =0;  // close all main index files
      virtual void ClearMainIndexFiles();     // reset and clear all index files
      virtual void DeleteAllIndexFiles() =0;  // delete all index files

      // added by TEO KAH CHOO
      virtual int ReadPLibFile (const char* fileList);

      virtual int ReadFileList(const char* fileList);
                                       // read & load contents from file list
      virtual int AddNode (const char *nodeName) =0;
                                       // add new node to file
      virtual int DeleteNode (const char *nodeName) =0;
                                       // remove an existing node from file
      virtual int ReplaceNode (const char *nodeName);   // replace a doc in file
      virtual int GetNumofTerms() =0;
      virtual int GetNumofNodes() =0;
      virtual DIRECTORIES* GetDIRS() { return dir; }
      virtual void DisplayContents() =0;
};


//************************************************************************
//
//  class InvertedIndexFile <- MMIndexFile
//  --------------------------------------
//  An implementation of inverted file organization based on hash-table.
//  It uses two hash-tables to maintain the file indices: 
//  (1) headerDB: which stores the index for all uniques terms. From the
//      term, one can access the list of nodes that contain the term.
//  (2) fileInforDB: which stores the index of all nodes stored in the
//      inverted file. At each entry, it stores information pertaining 
//      to the file necessary for the required operation. For example,
//      in the Vector-Space IR implementation, it stores the max frequency
//      of any term in the file, as well as square root of sum of weights,
//      required for computing cosine similarity.
//
//  The structure of the inverted file is depected in the following diagram:
//
//         headerDB
//                                  file-a      file-b     ...    file-k
//  term-1  | * |-->  termDB-1  [ (wd1a,Freq) (wd1b,Freq), ... (wd1k,Freq) ]
//   ..     |   |
//  term-n  | * |-->  termDB-n  [  ....    ]
//
//         fileInforDB
//
//  file-1  | [ max{tf}, sqrt(sum of wd**2) ] |
//   ..     |  ..                             |
//  file-m  |    same as above                |
//
//
//  ****NOTE: the fullpath name of all files are stored. This is required
//            during relevance feedback to access the actual file for
//            efficiency reason. It's much faster to get all terms in a file
//            from original file than from headerDB.
//
//
//  Local variables:
//  headerDB: main inverted index for terms
//  fileInforDB: index for files for accessing file related information.
//
//  Meanings of functions are self-explanatory
//
//************************************************************************
class InvertedIndexFile: public MMIndexFile
{
   protected:
      HashTable *headerDB;
      HashTable *fileInforDB;

   public:
      InvertedIndexFile (const char *name, DIRECTORIES *dirs, bool rdOnly,
                         bool newindex);
      virtual ~InvertedIndexFile();
      virtual void OpenMainIndexFiles();   // open or create main index files
      virtual void CloseMainIndexFiles();  // close all main index files
      virtual void DeleteAllIndexFiles();  // delete all index files
      virtual int AddNode (const char *nodeName) =0;
      virtual int DeleteNode (const char *nodeName) =0;
      virtual int GetMaxFreqforNode (const char* nodeName);
                                       // return max freq
      virtual int GetTermFreqinNode (const char* term, const char* nodeName);
                                       // get tf
      virtual float GetTermWtinNode (const char* term, const char* nodeName);
                                       // get wt
      virtual int GetSmallnforTerm (const char* term);
                                       // return # of files indexed
                                       // by term, ie small n
      virtual int GetNumofTerms() { return headerDB->GetDBSize(); }
      virtual int GetNumofNodes() { return fileInforDB->GetDBSize(); }
      virtual HashTable *OpenInvertedTermDB (const char *term);
      virtual HashTable *GetHeaderDB() { return headerDB; }
      virtual HashTable *GetFileInforDB() { return fileInforDB; }
      virtual void DisplayContents();

   protected:
      virtual int AddTerm (const char *term, const char *nodeName,
                           int freq, float wt);
      virtual int DeleteTerm (const char *term, const char *nodeName);
};


//************************************************************************
//
//  class TextIndexFile <- InvertedIndexFile 
//  ----------------------------------------
//  An implementation of inverted file organization for storing free text.
//  The organization of inverted file is defined in class InvertedIndexFile
//
//  Local variables:
//  stopWords: object pointer to check for stop words
//  stemAlgo:  object pointer to stem a word
//
//  Meanings of functions are self-explanatory
//
//************************************************************************
class TextIndexFile: public InvertedIndexFile
{
   protected:
      StopList *stopWords;
      Stemming *stemAlgo;

   public:
      TextIndexFile (const char *name, DIRECTORIES *dirs, bool rdOnly,
                     bool newindex, StopList *stopWords1, Stemming *stemAlgo1);
      virtual ~TextIndexFile();

      // Method added by TEO KAH CHOO in May 1995
      // to cater for reading plib text files.
      virtual int ReadPLibFile (const char *fileList);

      // Method added by TEO KAH CHOO in May 1995
      // to cater for reading plib text files.
      int AddNode (const char *nodename, const char *keyword,
		   const char *subject);

      // Method added by TEO KAH CHOO in May 1995
      // to cater for reading plib text files.      
      int ReadPLibText (const char *fileName, int volume);

      virtual int AddNode (const char *nodeName);
                                       // add new node to file
      virtual int DeleteNode (const char *nodeName);
                                       // remove an existing node from file
      StopList *GetStopWords() { return stopWords; }
      Stemming *GetStemAlgo() { return stemAlgo; }
      
};

#endif  //closing bracket for ifndef INVERTED_H
