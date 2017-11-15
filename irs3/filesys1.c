//=========================================================================
//
//  filesys1.c
//  Function definitions for: HashTable, InvertedFile and HTUtilities
//  classes
//
//  Implemented by:  T.S. Chua
//     Date of creation: June 1992
//     Last Modified:    3rd May 1994
//
//=========================================================================

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fcntl.h>
#include <math.h>
#include <dirent.h>
#include "standard.h"
#include "filesys1.h"

// Added by TEO KAH CHOO on May 1995
int ConstructStr (char *newStr, char *oldStr, char delimiter)
{  
    for (; *oldStr != '\0'; ++oldStr)
    { 
       if (*oldStr != delimiter) {         
          *newStr = *oldStr;
          newStr++;
       }    
    }
    *newStr = '\0';
    return 1;
}

// Added by TEO KAH CHOO on May 1995
char* nstrcpy (char *s, int start_pos)
{
   int i;
   for (i=0;i< start_pos; i++) s++;
   return s;
}

//**************************************************************
//
//  Definition of functions for HashTable class
//
//***************************************************************
//
//==============================================
// Constructor for HashTable
//==============================================
HashTable :: HashTable(const char *fileName, bool readOnly, bool todelete)
{
   dbPtr = NULL;
   name = strdup(fileName);
   toDelete = todelete;

   if (readOnly) { 
      if ((dbPtr=dbm_open(name, O_RDONLY, 0444)) == NULL) {
         printf ("\nndbm database <%s> does not exist!\n", name);
         printf ("Check your Directory setting\n");
         exit(-1);
      }
   }
   else
      if ((dbPtr=dbm_open(name, O_RDWR|O_CREAT, 0664)) == NULL) {
         printf ("\nndbm database <%s> does not exist!\n", name);
         return;
      }
}

//==============================================
// Destructor of HashTable
//==============================================
HashTable :: ~HashTable()
{
   delete name;
   if (dbPtr) dbm_close(dbPtr);
   if (toDelete) DeleteNDBMFiles();
}

//==============================================
// function to retriefve the contents of hash address key
//==============================================
const char* HashTable:: Retrieve(const char *key)
{
   datum *keyD = MakeDatum(key);
   datum a;
   if (dbPtr) a = dbm_fetch(dbPtr, *keyD);
   RemoveDatum(keyD);
   return a.dptr;
};

//==============================================
// Add content into HashTable at hash address key
//==============================================
int HashTable:: Insert(const char *key, const char *content)
{
   int OK = 0;
   datum *keyD = MakeDatum(key);
   datum *contentD = MakeDatum(content);
   if (dbPtr) {
      if (dbm_store(dbPtr, *keyD, *contentD, DBM_INSERT) < 0)
	 printf("cannot put item (*%s*, *%s*) in HashTable *%s*\n",
                key, content, name);
      else OK = 1;
   };
   RemoveDatum(keyD);
   RemoveDatum(contentD);
   return OK;
}

//==============================================
// Replace contents of hash address key by content
//==============================================
int HashTable:: Replace(const char *key, const char *content)
{
   int OK = 0;
   datum *keyD = MakeDatum(key);
   datum *contentD = MakeDatum(content);
   if (dbPtr) {
      if (dbm_store(dbPtr, *keyD, *contentD, DBM_REPLACE) < 0)
	 printf("cannot replace item (*%s*, *%s*) in HashTable *%s*\n",
                key, content, name);
      else
	 OK = 1;
   };
   RemoveDatum(keyD);
   RemoveDatum(contentD);
   return OK;
}

//==============================================
// Delete the entry at hash address key of NDBM
//==============================================
int HashTable:: Delete(const char *key)
{
   int OK = 0;
   datum *keyD = MakeDatum(key);
   if (dbPtr) {
      if (dbm_delete (dbPtr, *keyD) < 0)
	 printf("HashTable:Delete: <%s> not in file <%s>\n",name, key);
      else  OK = 1;
   };
   RemoveDatum(keyD);
   return OK;
}

//==============================================
// Duplicate the HT under the name name <newName>
//==============================================
HashTable* HashTable:: DuplicateHT(const char *newName)
{
   HashTable *newHT = new HashTable(newName, RW);
   if (GetToDelete()) newHT->ToDeleteOnExit();
   // now, copy contents of current HT to newName HT
   for (const char* key=GetFirstKey(); key!=NULL; key=GetNextKey()) {
      const char *content = Retrieve(key);
      newHT->Insert(key, content);
   }
   return newHT;
}

//==============================================
// Return true if <item> is in the HT
//==============================================
bool HashTable:: Contain(const char *item)
{
   bool found = false;
   if (Retrieve(item) != NULL) found = true;
   return found;
}

//==============================================
// Return the no of entries in Hash Table
//==============================================
int HashTable:: GetDBSize()
{
   int N = 0;
   for (const char* key=GetFirstKey(); key!=NULL; key=GetNextKey()) N++;
   return N;
}

//==============================================
// Get the first entry in HashTable
//==============================================
const char* HashTable:: GetFirstKey()
{
   datum aDatum = dbm_firstkey(dbPtr);
   return aDatum.dptr;
}

//==============================================
// Get the next entry in HashTable
//==============================================
const char* HashTable:: GetNextKey()
{
   datum aDatum = dbm_nextkey(dbPtr);
   return aDatum.dptr;
}

//================================================================
// convert 'str' into datum for ndbm access
//================================================================
datum* HashTable:: MakeDatum(const char *str)
{
   datum *nameD = new datum;
   nameD->dsize = strlen(str) + 1;
   nameD->dptr = strdup(str);
   return nameD;
}

//================================================================
// de-allocate memories allocated to a datum datatype
//================================================================
void HashTable:: RemoveDatum(datum *nameD)
{
   if (nameD != NULL) {
      if (nameD->dptr != NULL) delete nameD->dptr;
      delete nameD;
   }
}

//================================================================
// DeleteNDBMFiles: remove the .pag and .dir for ndbm hash-table
//                  named fileName
//================================================================
void HashTable:: DeleteNDBMFiles()
{
   char source[50];
   strcpy(source, name);
   strcat(source, ".pag");
   if (unlink(source)<0) {
      printf("DeleteNDBMFile: Can't remove <%s>, file not found!\n");
      exit(1);
   }
   strcpy (source, name);
   strcat (source, ".dir");
   if (unlink(source)<0) {
      printf("DeleteNDBMFile: Can't remove <%s>, file not found!\n");
      exit(1);
   }
}

//================================================================
// DisplayContents: print out the contents of HashTable
//================================================================
void HashTable:: DisplayContents()
{
   printf("\n**Contents of Hash-Table <%s> is:\n", name);
   const char *key = GetFirstKey();
   if (key == NULL)
      printf("==> No entry in Hash-Table\n");
   else { 
      while (key != NULL) {
         const char *content = Retrieve(key);
         printf("  ==%10s %s\n", key, content);
         key  =GetNextKey();
      }
   }
}



//**************************************************************
//
//  Definition of functions for StringStemHT class
//  StringStemHT <- HashTable
//
//***************************************************************
//
//===================================
// Constructor for StringStemHt
//===================================
StringStemHT:: StringStemHT (const char *name, bool readOnly,
                             bool todelete):
     HashTable (name, readOnly, todelete)
{
   maxFreq = 0;
   util = new HTUtilities();
}


//===================================
// Destructor for StringStemHt
//===================================
StringStemHT:: ~StringStemHT()
{
   delete util;
}


//===========================================================
// Construct the stem list for an input string using stemAlgo
//============================================================
int StringStemHT:: GenerateStemList (const char *string, StopList *stopWords,
                                     Stemming *stemAlgo)
{
   char inStr[40];
   char newStr[40];
   int  i, n=0;

   // skip leading non-alphabetic charcters
   while (!( (string[n]>64 && string[n]<91) ||
             (string[n]>96 && string[n]<123) )  &&  string[n]!='\0')
      n++;

   while (string[n] != '\0') {   // main loop

      for (i=0; i<40; i++) newStr[i] = inStr[i] = '\0';

      // read in next word in inStr
      int k=0;
      while (( (string[n]>64 && string[n]<91) ||
               (string[n]>96 && string[n]<123) )  &&  string[n]!='\0') {
         inStr[k] = string[n];
         k++; n++;
      }
      inStr[k] = '\0';

      // convert inStr to lower letters and store in newStr
      for (i=0; i<strlen(inStr); i++)   {
         if (( int(inStr[i]) < 91) && (int(inStr[i] > 64)))
            newStr[i] = char (int(inStr[i] + 32));
         else
         if (( int(inStr[i]) > 96) && (int(inStr[i] < 123)))
            newStr[i] = inStr[i];
      }

      // Add newStr to Stem HashTable
      stemAlgo->Stem(newStr);
      if (newStr[0] != '\0')
         if ( !stopWords->IsStopWord(newStr) )
            AddStem ( (const char*)newStr );

      // skip until next alphabet
      while (!( (string[n]>64 && string[n]<91) ||
                (string[n]>96 && string[n]<123) )  &&  string[n]!='\0')
         n++;
   }
   return (0);
}


//===========================================================
// Add aStem to Stem-Hash-Table. 
// It also updates the freq of aStem and maxFreq of the whole list.
//============================================================
void StringStemHT:: AddStem (const char* aStem)
{
   float wt = 0.0;
   int freq = 1;  // freq is set to 1 if aStem is new
   const char* str = Retrieve(aStem);
   if (str != NULL) {
      // aStem is already in HT, update freq and maxFreq
      util->StringToIntFloat(str, freq, wt);
      freq += 1;
   }
   if (maxFreq < freq) maxFreq = freq;
   // store (freq,wt) back to HT
   char *content = util->IntFloatToString(freq,wt);
   if (str == NULL)  Insert(aStem, (const char*)content);
   else             Replace(aStem, content);
   delete content;
}


//===========================================================
// Update the freq and wt information of aStem in HT
//============================================================
void StringStemHT:: UpdateStemFreqWt (const char *aStem, int freq, float wt)
{
   const char *str = Retrieve(aStem);
   if (str == NULL) {
      printf("\nError in UpdateStemFreqWt()! %s not found\n", aStem);
      exit(-1);
   }
   else {
      char *content = util->IntFloatToString(freq,wt);
      Replace (aStem, (const char*)content);
      delete content;
   }
}


//===========================================================
// Retrieve the freq and wt of aStem in HT
//============================================================
void StringStemHT:: RetrieveStemFreqWt (const char *aStem, int& freq, float& wt)
{
   const char *content = Retrieve(aStem);
   if (content == NULL) {
      printf("\nError in RetrieveStemFreqWt()! %s not found\n", aStem);
      exit(-1);
   }
   else 
      util->StringToIntFloat(content, freq, wt);
}

//==============================================
// Duplicate the HT under the name name <newName>
// This is identical to DuplicateHT defined under its
// parent class HashTable.
// This function is needed because of the return type.
// There ought to be a better way to do this
//==============================================
StringStemHT* StringStemHT:: Duplicate(const char *newName)
{
   StringStemHT *newHT = new StringStemHT(newName, RW);
   if (GetToDelete()) newHT->ToDeleteOnExit();
   newHT->SetMaxFreq( GetMaxFreq() );
   // now, copy contents of current HT to newName HT
   for (const char* key=GetFirstKey(); key!=NULL; key=GetNextKey()) {
      const char *content = Retrieve(key);
      newHT->Insert(key, content);
   }
   return newHT;
}


//********************************************************************
//
//  Definition of functions for class FileStemHT <- StringStemHT
//
//********************************************************************
//
//===========================================================
// Construct the stem list for a given file using stemAlgo
//============================================================
int FileStemHT:: GenerateStemList (const char* fileName, StopList *stopWords,
                                   Stemming *stemAlgo)
{
   char inStr[40];
   char passStr[40];
   char newStr[40];
   int  i;

   FILE *textFile = fopen (fileName, "rt");
   if (textFile == NULL) {
      printf (" *FileStemList::Error in opening file %s\n", fileName);
      exit(-1);
   }
   if (feof(textFile)) return (0);

   fscanf (textFile, "%[^A-Za-z]", passStr);  // skip non-alphebet characters
   while (!feof(textFile)) {   // main loop
      for (i=0; i<40; i++)
         newStr[i] = inStr[i] = '\0';

      // read in next word in inStr
      fscanf (textFile, "%[A-Za-z]", inStr);

      // convert inStr to lower letters and store in newStr
      for (i=0; i<strlen(inStr); i++)   {
         if (( int(inStr[i]) < 91) && (int(inStr[i] > 64)))
            newStr[i] = char (int(inStr[i] + 32));
         else
         if (( int(inStr[i]) > 96) && (int(inStr[i] < 123)))
            newStr[i] = inStr[i];
      }

      // Add newStr to Stem HashTable
      stemAlgo->Stem(newStr);
      if (newStr[0] != '\0')
         if ( !stopWords->IsStopWord(newStr) )
            AddStem ( (const char*)newStr );

      // Skip until next alphebet
      if (!feof(textFile))
         fscanf (textFile, "%[^A-Za-z]", passStr); 
   }
   fclose (textFile);
   return (1);
}



//**************************************************************
//
//  Definition of functions for MMIndexFile class
//
//  Super class for all index files designed to index MM data.
//  This includes text, color, and texture (coarseness, contrast, directionality)
//
//  The class hierarchy is:
// 
//                        MMIndexFile
//                      /            \
//       InvertedIndexFile        DirectIndexFile
//            /                /    \   \                  \
//  TextIndexFile     ColorIndexFile \ CoarsenessIndexFile  DirectionalityIndexFile
//                                    \
//                                 ContrastIndexFile
//
// It defines all functions common to all index files
//
//***************************************************************

//===================================================
// Constructor
//===================================================
MMIndexFile:: MMIndexFile(const char *name, DIRECTORIES *dirs, bool rdOnly,
                          bool newindex)
{
   IFName = strdup(name);
   dir = dirs;
   readOnly = rdOnly;
   newIndex = newindex;
   util = new HTUtilities();
}

//====================================================
// Destructor
//====================================================
MMIndexFile:: ~MMIndexFile()
{
   delete IFName;
   delete util;
}



//===================================================
// function to reset and clear headerDB and fileInforDB
//===================================================
void MMIndexFile:: ClearMainIndexFiles()
{
   OpenMainIndexFiles ();
   if (GetNumofNodes() != 0)    // if index file already exist 
        DeleteAllIndexFiles();    // delete it
   
}
    

//=====================================================
// replace node 'filename' in the inverted file
// It first delete all entries from old file names,
// then it inserts the new one.
// NOTE: fileName must be full path name, and it must be
//       the same full path name as the one it is replacing
//       other than the contents.
//=====================================================
int MMIndexFile:: ReplaceNode (const char *fileName)
{
   if (readOnly) {
      printf ("\nRead-Only mode, ReplaceNode() is not permitted\n\n");
      exit(-1);
   }

   int OK = 0;
   if (OK=DeleteNode(fileName)<=0)
      printf("ReplaceNode: Error in replacing <%s>\n", fileName);
   OK = AddNode(fileName);
   return OK;
}


// Added by TEO KAH CHOO on May 1995
// for building the color indexes for plib images.
//==============================================================
// Read in a list of directories from a file named fileListName.
// It loads each file in each directory into the inverted file.
//
// Similar to ReadFileList.
//==============================================================
int MMIndexFile::ReadPLibFile (const char *fileListName)
{
   FILE *fileList;
   char passStr[40];   // buffer to skip unwanted characters
   char fileName[40];  // buffer to hold file name
   char dir[150];      // buffer to hold directory name of files
   char fullName[200]; // buffer to hold full path name of files
   char msg[200];

   fileList = fopen(fileListName, "r");
   if (fileList== NULL) {
      printf (" Error in opening file %s\n", fileListName);
      return(0);
   }
   if (feof(fileList)) return (0);

   // Read the first directory name
   fscanf (fileList, "%s", dir);
   printf ("Directory: %s\n", dir);

   struct dirent *dp;
   DIR *dirp;
   int image_count = 0;

   // fileList contains a list of directory names
   // whose images are to be analysed.
   while (!feof(fileList)) {

      // check the existence of the directory
      //=====================================
      if (chdir (dir)) {
         msg[0] = '\0';
         sprintf (msg, "%s directory does not exist!", dir);
         printf ("%s\n", msg);
      }
      else
      {

printf ("Creating index\n");
         // read the directory
	 //===================
         dirp = opendir (dir);
	 if (dirp == NULL) {
	    msg[0] = '\0';
            sprintf (msg, "Error in reading directory %s!", dir);
       	    printf ("%s\n", msg);
         }
         else {
            // Read and index each file
	    //=========================
	    while ((dp = readdir (dirp)) != NULL) {

	       // To overcome a bug which the program hangs
	       // after loading about 90 images
	       // Simply run the program again and it will
	       // continue from the last processed file.
               if (image_count > 200) return (1);

               strcpy (fileName, dp->d_name); // copy image filename
		
	       // Skip file '.' & '..'
	       if ( strcmp (fileName, ".") != 0 &&
		    strcmp (fileName, "..") != 0) {
               	
                  // Construct the full pathname for loading image file 
                  fullName[0] = '\0';
                  sprintf (fullName, "%s/%s",dir, fileName);

                  if (AddNode ((const char*)fullName) == 1)
                     image_count++;
               } // end if strcmp

            } // end while

         } // end inner else read & index each file

      } // end else read the directory

      // Read in the next directory
      printf ("Indexing for directory %s completed.\n", dir);
      fscanf (fileList, "%s", dir);
      
   } // end while

   fclose(fileList);

   return(1);
}
//=====================================================
// Read in a list of files from a file named fileList.
// It loads the contents of each file into the Inverted file
//
// The structure of this file is:
// The 1st string contains the name of directory where the list of
// files are residing; subsequent strings contain the file names.
// A string is defined as a sequence of letters, digits, '/', '-' & '.'.
// All strings are separated by spaces.
// NOTE: This function assumes that the current directory is where
//       the inverted indices are stored.
//
//=====================================================
int MMIndexFile:: ReadFileList(const char *fileListName)
{
   FILE *fileList;
   char passStr[40];   // buffer to skip unwanted characters
   char fileName[40];  // buffer to hold file name
   char dir[100];      // buffer to hold directory name of files
   char fullName[100]; // buffer to hold full path name of files

   fileList = fopen(fileListName, "r");
   if (fileList== NULL) {
      printf (" Error in opening file %s\n", fileListName);
      return(0);
   }
   if (feof(fileList)) return (0);

   fscanf (fileList, "%[^-./A-Za-z0-9]", passStr);
   bool init = true;
   while (!feof(fileList)) {
      if (init) {
         fscanf (fileList, "%[-./A-Za-z0-9]", dir);
         printf ("\nDirectory name is: *%s*\n", dir);
         init = false;
      }
      else {
         fscanf (fileList, "%[-./A-Za-z0-9]", fileName);
         if (strlen(fileName) < 5) {
            printf("\n*** error in fileName *%s*, < 5 chars\n", fileName);
            return(0);
         } 
         else {
            strcpy (fullName, dir);
            strcat (fullName, fileName);
            AddNode ((const char*)fullName);
            printf("<%s> added\n", fullName);
         }
      }
      fscanf (fileList, "%[^-./A-Za-z0-9]", passStr);
   }
   fclose(fileList);
   return(1);
}



//**************************************************************
//
//  Definition of functions for InvertedIndexFile class
//
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
//***************************************************************

//===================================================
// Constructor
//===================================================
InvertedIndexFile:: InvertedIndexFile(const char *name, DIRECTORIES *dirs,
                        bool rdOnly, bool newindex):
                    MMIndexFile (name, dirs, rdOnly, newindex)
{

   // open up main index files
   if (newIndex==TO_REPLACE && readOnly) {
      printf("ERROR in MMIndexFile, Can't replace main indices in READ-ONLY mode!!\n");
      exit (-1);
   }
   if (newIndex == TO_REPLACE) ClearMainIndexFiles();
   OpenMainIndexFiles();
}

//====================================================
// Destructor
//====================================================
InvertedIndexFile:: ~InvertedIndexFile()
{
   CloseMainIndexFiles();
}

//===================================================
// function to open headerDB and fileInforDB
//===================================================
void InvertedIndexFile:: OpenMainIndexFiles()
{
   // open up header and fileInfor HashTables
   chdir(dir->invertedDIR);
   char nameofDB[30];
   strcpy(nameofDB, "headerDB-");
   strcat(nameofDB, IFName);
   headerDB = new HashTable( (const char*)nameofDB, readOnly);
   strcpy(nameofDB, "fileInforDB-");
   strcat(nameofDB, IFName);
   fileInforDB = new HashTable( (const char*)nameofDB, readOnly);
   chdir(dir->mainDIR);
}

//===================================================
// function to close headerDB and fileInforDB
//===================================================
void InvertedIndexFile:: CloseMainIndexFiles()
{
   chdir(dir->invertedDIR);
   if (headerDB != NULL) delete headerDB;
   if (fileInforDB != NULL) delete fileInforDB;
   chdir(dir->mainDIR);
}

//==============================================================================
// function to delete headerDB, fileInforDB, and all the termDB
//==============================================================================
void InvertedIndexFile::DeleteAllIndexFiles()
{
   if (readOnly) {
      printf ("\nRead-Only mode, DeleteHTFiles() is not permitted\n\n");
      exit(-1);
   }
   
   // delete all direct index files first
   chdir(dir->directDIR);
   const char *key;
   for (key=fileInforDB->GetFirstKey(); key!=NULL;
        key=fileInforDB->GetNextKey()) {
      char *justName = util->JustNameExt(key); // extract only the name + ext
      FileStemHT *fileHT = new FileStemHT(justName, RD_ONLY, TO_DELETE);
      delete fileHT;
      delete justName;
   };

   // now delete all inverted indeces
   chdir(dir->invertedDIR);
   for (key=headerDB->GetFirstKey(); key!=NULL;
        key=headerDB->GetNextKey()) {
      HashTable *termDB = new HashTable(key, readOnly, TO_DELETE);
      delete termDB;
   };

   headerDB->ToDeleteOnExit();
   delete headerDB;
   headerDB = NULL;
   fileInforDB->ToDeleteOnExit();
   delete fileInforDB;
   fileInforDB = NULL;
   chdir(dir->mainDIR);
}


//=================================================================
// return the max freq for fileName - stored in HashTable maxFreq
//=================================================================
int InvertedIndexFile:: GetMaxFreqforNode (const char* fileName)
{
   int maxFreq = 0;
   const char* maxFreqStr = fileInforDB->Retrieve(fileName);
   if (maxFreqStr == NULL)
      printf("%s does not exist! \n", fileName);
   else
      maxFreq = atoi(maxFreqStr);
   return maxFreq;
}


//=================================================================
// return the frequency of term in node 'fileName'
//=================================================================
int InvertedIndexFile:: GetTermFreqinNode (const char* term, const char* fileName)
{
   int freq = 0;
   float wt;
   HashTable *termDB = OpenInvertedTermDB (term);
   const char* content = termDB->Retrieve(fileName);
   if (content != NULL)
      util->StringToIntFloat(content, freq, wt);
   else
      printf("%s does not occurs in %s\n", term, fileName);
   delete termDB;    // close term hash-table
   return freq;
}


//======================================================================
// return the weight of term 'term' in node 'filename'
//======================================================================
float InvertedIndexFile:: GetTermWtinNode (const char *term, const char* fileName)
{
   float wt = 0.0;
   int freq;
   HashTable *termDB = OpenInvertedTermDB (term);
   const char *content = termDB->Retrieve(fileName);
   if (content != NULL)
      util->StringToIntFloat(content, freq, wt);
   else
      printf("%s does not occurs in %s\n", term, fileName);
   delete termDB;    // close term hash-table
   return wt;
}

//======================================================================
// return the number of files that is indexed by the index term 'term'
//======================================================================
int InvertedIndexFile:: GetSmallnforTerm (const char *term)
{
   HashTable *termDB = OpenInvertedTermDB (term);
   int n = termDB->GetDBSize();
   delete termDB;
   return n;
}

//======================================================================
// return an opened iinverted file for 'term'
//======================================================================
HashTable* InvertedIndexFile:: OpenInvertedTermDB (const char *term)
{
   chdir(dir->invertedDIR);
   HashTable *termDB = new HashTable(term, readOnly);
   chdir(dir->mainDIR);
   return termDB;
}

//===================================================================
// Adding index term 'term' of frequency 'freq', weight 'wt', and
// file 'filename' to the DB
//===================================================================
int InvertedIndexFile:: AddTerm (const char *term, const char *fileName,
                             int freq, float wt)
{
   if (readOnly) {
      printf ("\nRead-Only mode, AddTerm() is not permitted\n\n");
      exit(-1);
   }

   int OK = 0;
   if ( !headerDB->Contain(term) ) 
      // there is no 'term' in DB, insert it and create a hashtable for term
      headerDB->Insert(term, term);  // key and contents are the same

   //create the term hash-table
   chdir(dir->invertedDIR);
   HashTable *termDB = new HashTable(term, readOnly);
   if ( !termDB->Contain(fileName) ) {
      // fileName not in termDB
      char *str = util->IntFloatToString(freq, wt);
      if (termDB->Insert(fileName, (const char*)str ) != 0) OK = 1;
      delete str;
   }
   delete termDB;
   chdir(dir->mainDIR);
   return OK;
}

//================================================================
// Delete index term 'term' from file 'fileName'
//================================================================
int InvertedIndexFile:: DeleteTerm (const char *term, const char *fileName)
{
   if (readOnly) {
      printf ("\nRead-Only mode, DeleteTerm() is not permitted\n\n");
      exit(-1);
   }

   int OK = 0;
   if ( headerDB->Contain(term) ) {
      chdir(dir->invertedDIR);
      HashTable *termDB = new HashTable(term, readOnly);
      if (termDB->Delete(fileName) != 0) {
	 OK = 1;
	 if (termDB->GetDBSize()==0) {
	    // term list is empty, remove its NDBM files & from headerDB
            termDB->ToDeleteOnExit();
	    headerDB->Delete(term);
	 }
         delete termDB;
      }
      else delete termDB;
      chdir(dir->mainDIR);
   }
   return OK;
}


//
//================================================================
// DisplayContents: print out the contents of InvertedFile for
//                  debugging purposes
//================================================================
void InvertedIndexFile:: DisplayContents()
{
   const char *term, *fileN;
   for (term=headerDB->GetFirstKey(); term!=NULL; term=headerDB->GetNextKey()) {
      printf("\n**term=%s**\n", term);

      // open HashTable for term
      chdir(dir->invertedDIR);
      HashTable* termDB = new HashTable(term, readOnly);
      chdir(dir->mainDIR);
      if (termDB->GetDBSize() == 0)
	 printf("    No ENTRY in TERM!!!\n");
      else {
         int freq; float weight;
         for (fileN=termDB->GetFirstKey(); fileN!=NULL; fileN=termDB->GetNextKey() ) {
            const char* content = termDB->Retrieve (fileN);
            util->StringToIntFloat (content, freq, weight);         
	    printf("   ==%s %d %f\n", fileN, freq, weight);
         }
      }
      delete termDB;
   }
}


//**************************************************************
//
//  Definition of functions for TextIndexFile class
//
//  An implementation of inverted file organization for storing free text.
//  The organization of inverted file is defined in class InvertedIndexFile
//
//***************************************************************

//===================================================
// Constructor
//===================================================
TextIndexFile:: TextIndexFile(const char *name, DIRECTORIES *dirs, bool rdOnly,
                     bool newindex, StopList *stopWords1, Stemming *stemAlgo1):
                InvertedIndexFile (name, dirs, rdOnly, newindex)
{
   stopWords = stopWords1;
   stemAlgo = stemAlgo1;
}

//====================================================
// Destructor
//====================================================
TextIndexFile:: ~TextIndexFile()
{};


//=====================================================
// Add contents of node 'fileName' to the inverted file
// fileName holds the full path name of file to be added
//=====================================================
int TextIndexFile:: AddNode (const char *fileName)
{
   if (readOnly) {
      printf ("\nRead-Only mode, AddNode() is not permitted\n\n");
      exit(-1);
   }

   int OK = 0;
   // First check if fileName has been inserted into DB
   // If it has, there should be an entry in fileInforDB HashTable
   if ( !fileInforDB->Contain(fileName) ) {
      // means that fileName has not been inserted yet
      printf("    from AddNode, the full name is: *%s*\n", fileName);
      chdir(dir->directDIR);
      char *justName = util->JustNameExt(fileName);
      FileStemHT *newStemList = new FileStemHT(justName, RW);
      delete justName;
      chdir(dir->mainDIR);
      newStemList->GenerateStemList (fileName, stopWords, stemAlgo);
      int maxTermFreq = newStemList->GetMaxFreq();

      // store frequency infor in hashTable maxFreq
      float wt2 = 0.0;
      char *content = util->IntFloatToString(maxTermFreq,wt2);
      fileInforDB->Insert(fileName, (const char*)content);
      delete content;

      int freq; float wt;
      const char *key;
      for (key=newStemList->GetFirstKey(); key!=NULL;
           key=newStemList->GetNextKey()) {
         newStemList->RetrieveStemFreqWt(key, freq, wt);
	 wt = (float)freq / (float)maxTermFreq;
	 AddTerm (key, fileName, freq, wt);
      }
      chdir(dir->directDIR);
      delete newStemList;
      chdir(dir->mainDIR);
      OK = 1;
   }
   return OK;
}

//=====================================================
// delete file 'filename' from the database
//=====================================================
int TextIndexFile:: DeleteNode (const char *fileName)
{
   if (readOnly) {
      printf ("\nRead-Only mode, DeleteNode() is not permitted\n\n");
      exit(-1);
   }

   int OK = 0;
   if ( fileInforDB->Contain(fileName) ) {
      // means that fileName is in HT, delete all its entries
      fileInforDB->Delete(fileName);

      chdir(dir->directDIR);
      char *justName = util->JustNameExt(fileName);
      FileStemHT *newStemList = new FileStemHT(justName, RW, TO_DELETE);
      delete justName;
      chdir(dir->mainDIR);

      const char *key;
      for (key=newStemList->GetFirstKey(); key!=NULL;
           key=newStemList->GetNextKey())
	 DeleteTerm(key, fileName);

      chdir(dir->directDIR);
      delete newStemList;
      chdir(dir->mainDIR);

      OK = 1;
   }
   return OK;
}


//=====================================================
// Added by TEO KAH CHOO on May 1995
// to cater for the building of text index files
// for plib text files
// Similar to ReadFileList
//======================================================
int TextIndexFile::ReadPLibFile (const char *fileListName)
{
   FILE *fileList;
   char fileName[40];  // buffer to hold file name
   char dir[100];      // buffer to hold directory name of files
   char fullName[200]; // buffer to hold full path name of files
   char msg[200];

   // Read the file
   //==============
   fileList = fopen(fileListName, "r");
   if (fileList== NULL) {
      printf (" Error in opening file %s\n", fileListName);
      return(0);
   }
   if (feof(fileList)) return (0);

   // Read and Check the existence of directory
   //==========================================
   fscanf (fileList, "%s", dir);
   if (chdir (dir)) {
      fclose (fileList);
      msg[0] = '\0';
      sprintf (msg, "%s directory cannot be opened!", dir);
      printf ("%s\n", msg);
      return 0;
   }

   // Read the first PLib Text filename     
   fscanf (fileList, "%s", fileName);

   while (!feof (fileList)) {

      sprintf(fullName,"%s/%s",dir, fileName);

      //  extract the volume indicator of the text file
      //===============================================
      int volume=1;
      sscanf (fileName, "%*[a-zA-Z]%d", &volume);

      // To process the content of a PLib text file
      ReadPLibText ((const char*)fullName, volume);

      // Read the next PLib Text filename
      fscanf (fileList, "%s", fileName);

   } // end of feof

   fclose (fileList);
   return 1;
}
      /*-----------ASSUMPTIONS MADE------------------------
      1. Filename, Keyword list, Subject List and
         Description List all begins on a new line

      2. Default position for the starting position
         of Image filename, Keyword list and
         Description List is assumed to be fixed
     ---------------------------------------------------*/

// Added by TEO KAH CHOO on May 1995
int TextIndexFile::ReadPLibText (const char* fileName, int volume)
{
    FILE *text_fp;
    text_fp = fopen (fileName, "r");
    if (!text_fp) {
       printf ("%s textual description file does not exist!\n",fileName);
       exit(-1);
    } 

    char sub_dir[DIRNAME_LEN], image_name[IMAGENAME_LEN];
    char image_fullName[FILENAME_LEN];
    char *temp_ptr;
    char line[LINELEN];
    char keyword[LINELEN];
    char subject[LINELEN];

    // Read the first line
    fgets (line, LINELEN, text_fp);

    // Read till end of file
    while (!feof(text_fp)) { 

         //  extract the image filename 
         //============================
         // sub_dir contains the sub-directory eg animals, art etc
         // image_name is the unique code given to each image
         // eg 0647103a.jpg
         
         temp_ptr = nstrcpy (line, IMAGENAME_HEADERPOS);
         sscanf (temp_ptr,"%[.-A-Za-z0-9]", sub_dir);
         sscanf (temp_ptr,"%*[.-A-Za-z0-9]%*1s%[^]]",image_name);

         // construct full path name
         // eg /usr/people/prj/text/vol1/animal/0647103a.jpg

         sprintf (image_fullName,"%s/VOL%d/%s/%s",
		dir->imageBrowseDIR,volume,sub_dir,image_name);
         printf ("Image : %s\n", image_fullName);
 
         //  extract the Keyword List
         fgets (line, LINELEN, text_fp);
         temp_ptr = nstrcpy (line,KEYWORD_HEADERPOS);
         keyword[0] = '\0';
         ConstructStr (keyword, temp_ptr, ';');

         /*  extract the Subject List */
         fgets (line, LINELEN, text_fp);
         temp_ptr = nstrcpy (line, SUBJECT_HEADERPOS);

         subject[0] = '\0';
 	 ConstructStr (subject, temp_ptr, ',');

         /*  skip the Description List and blank lines */
         fgets (line, LINELEN, text_fp);
         //printf ("Description: %s\n", line);

         fgets (line, LINELEN, text_fp);
         fgets (line, LINELEN, text_fp);

         AddNode ((const char*)image_fullName, 
		(const char*)keyword, (const char*)subject);

      } // end while feof

      fclose (text_fp);
      return 1;
}


int TextIndexFile::AddNode (const char *fileName,
			    const char *keyWord,
			    const char *subject)
{
   if (readOnly) {
      printf ("\nRead-Only mode, AddNode() is not permitted\n\n");
      exit(-1);
   }
   int OK = 0;
   // First check if fileName has been inserted into DB
   // If it has, there should be an entry in fileInforDB HashTable
   if ( !fileInforDB->Contain(fileName) ) {
      // means that fileName has not been inserted yet
      printf("    from AddNode, the full name is: *%s*\n", fileName);
      printf ("KEYWORD: %s\n", keyWord);
      printf ("SUBJECT: %s\n", subject);

      chdir(dir->directDIR);
      char *justName = util->JustNameExt(fileName);

      // Note that an instance of StringStemHT is made,
      // Not FileStemHT
      StringStemHT *newStemList = new StringStemHT(justName, RW);
      delete justName;
      chdir(dir->mainDIR);
      newStemList->GenerateStemList (keyWord, stopWords, stemAlgo);
      newStemList->GenerateStemList (subject, stopWords, stemAlgo);

      int maxTermFreq = newStemList->GetMaxFreq();

      // store frequency infor in hashTable maxFreq
      float wt2 = 0.0;
      char *content = util->IntFloatToString(maxTermFreq,wt2);
      fileInforDB->Insert(fileName, (const char*)content);
      delete content;

      int freq; float wt;
      const char *key;
      for (key=newStemList->GetFirstKey(); key!=NULL;
           key=newStemList->GetNextKey()) {
         newStemList->RetrieveStemFreqWt(key, freq, wt);
	 wt = (float)freq / (float)maxTermFreq;
    //printf ("AddTerm: key= %s, freq=%d, wt=%.6f\n", key, freq, wt);
	 AddTerm (key, fileName, freq, wt);
      }
      chdir(dir->directDIR);
      delete newStemList;
      chdir(dir->mainDIR);
      OK = 1;
   }
   return OK;
}

   
