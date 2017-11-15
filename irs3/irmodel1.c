//=========================================================================
//
//  irmodel1.c
//  ----------
//  Function definitions for classes: 
//  1) RetrievalModel
//  2) TextRetrieval
//  3) VectorSpace
//
//  Implemented by:  T.S. Chua
//     Date of creation: June 1992
//     Last Modified:    13th May 1995
//
//  Modified by: Shen Mo Jun
//               September 1995
//=========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <strings.h>
#include <fstream.h>
#include <fcntl.h>
#include <math.h>
#include "irmodel.h"

int  rn[100]; //defined in comb.c
//***************************************************************************
//
//  Definition of functions for RetrievalModel class
//
//  a virtual class used to define the prototype of all retrieval models
//
//***************************************************************************

//==============================================================================
// Constructor
//==============================================================================
RetrievalModel:: RetrievalModel (int minNR, int maxNR, float minSim)
{
   NRankList = 0;
   rankList = NULL;
   seenList = seenRelList = NULL;
   util = new HTUtilities();
   minNRankList = minNR;
   maxNRankList = maxNR;
   minSimilarity = minSim;
      // dir and typeName are to be setup in appropriate specialized class 
   dir = NULL;
   typeName = NULL; 
}

//==============================================================================
// Destructor
//==============================================================================
RetrievalModel:: ~RetrievalModel()
{
   if (rankList != NULL) RemoveSimNodeList(rankList);
   if (seenList != NULL) delete seenList;
   if (seenRelList != NULL) delete seenRelList;
   delete util;
   if (typeName != NULL) delete typeName;
   // do not delete dir as it is shared by other classes
}

//==============================================================================
// This function is called within ComputeRankList to post-process the list.
// It assumes that there is a ranked list sorted in ascending order of similarities.
// It ensures that there are > minNR items, and for nodes above minNR,
// similarity > minSim. It also updates the seenList
//==============================================================================
void RetrievalModel:: ProcessRankList()
{

   if (rankList == NULL) return;  // nothing to do

   // now select up to maxNRankList nodes and assign status to each node
   printf("Process and trim ranked list\n");
   SimNode *aNode=rankList, *prevNode=rankList;
   int N = 0;
   while (aNode != NULL) {
      int status = UNRANKED;
      if ( seenRelList->Contain((const char*)aNode->name) )
          status = RELEVANT;
      else if ( seenList->Contain((const char*)aNode->name) ) status = SEEN;
      aNode->status = status;
      if (status==UNRANKED) N++; // count only those not seen by users
      prevNode = aNode;          // set aNode to next node in list
      aNode = aNode->next;
      if (aNode != NULL) {
         if ((N==maxNRankList) ||
             (N>=minNRankList && aNode->similarity < minSimilarity) ) {
            // remove rest of nodes in ranked list starting from aNode
            RemoveSimNodeList(aNode);
            aNode = NULL;
            prevNode->next = NULL;
         }
      }
   }

   // Add nodes in ranked list into seen list
   NRankList=0; //Add by Shen Mojun
   for (aNode=rankList; aNode != NULL; aNode=aNode->next) {
      NRankList++;
      seenList->Insert((const char*)aNode->name, (const char*)aNode->name);
   }
} 


// Added by TEO KAH CHOO  in May 95
// to write the ranklist to an output file
void RetrievalModel:: WriteToFile (char *fName)
{
   FILE *fp;
   SimNode *currNode = rankList;

   fp =fopen (fName, "w");
   if (fp == NULL) 
       printf ("error in writing to file %s\n", fName);
   else {

       while (currNode) {
          fprintf (fp,"%s\n", currNode->name);
          currNode = currNode->next;
       }
       fclose (fp);
   }
}    

void RetrievalModel:: DisplayRankList(int NR, SimNode *rList)
{

   // **** Display ranked list ****
   SimNode *currNode = rList;
   printf("\n\nTop %i document list: \n", NR);
   int i = 1;
   while (currNode != NULL) {
      printf("  %2i)  %12s  %f", i, currNode->name, currNode->similarity);
      printf("  **\n");
      currNode = currNode->next;
      i++;
   };

}
//==============================================================================
// function to display Ranked List of results and obtain relevance judgements
// from the users
//==============================================================================
int* RetrievalModel:: DisplayNJudgeRankList(int NR, SimNode *rList)
{
   int *relevant = new int[NR];
   for (int j=0; j<NR; j++) relevant[j] = UNRANKED;

   // **** Display ranked list ****
   SimNode *currNode = rList;
   printf("\n\nTop %i document list: \n", NR);
   int i = 1;
   while (currNode != NULL) {
      printf("  %2i)  %12s  %f", i, currNode->name, currNode->similarity);
      if (currNode->status == RELEVANT) {
         printf("  REL\n");
         relevant[i-1] = RELEVANT;
      }
      else if (currNode->status == NONRELEVANT) {
         printf("  NON-REL\n");
         relevant[i-1] = NONRELEVANT;
      }
      else if (currNode->status == SEEN) {
         printf("  SEEN\n");
         relevant[i-1] = SEEN;
      }
      else
         printf("  **\n");

      currNode = currNode->next;
      i++;
   };

   // **** Relevance Judgement ****
 /* printf("\nList positions of new relevant nodes, terminate by typing -1:\n");
   scanf("%d",&i);
   while (i != -1) {
      if (i>0 && i<=NR) relevant[i-1] = RELEVANT;
      scanf("%d", &i);
   }

 Commented by Shen Mojun */
  
  for (i=0; i<=NR; i++)
    { if (rn[i]== 1) relevant[i] = RELEVANT;} 
  
   printf("\nResult of Relevant judgement is:\n");
   for (i=0; i<NR; i++)
      if (relevant[i]==RELEVANT)    printf("  %2i) RELEVANT\n", i+1);
      else
      if (relevant[i]==NONRELEVANT) printf("  %2i) NON-RELEVANT\n", i+1);
      else
      if (relevant[i]==SEEN)        printf("  %2i) SEEN\n", i+1);
      else                          printf("  %2i) UNRANKED\n", i+1);

   return relevant;
}

//==============================================================================
// function to delete all temporary files used to maintain seen list
// and seen-Relevant list 
//==============================================================================
void RetrievalModel:: DeleteTmpRelFiles()
{
   // setup full name for seenList & seenRekList
   char name[30];
   strcpy (name, seenListHTName);
   strcat (name, typeName);
   char relName[30];
   strcpy (relName, seenRelListHTName);
   strcat (relName, typeName);

   if (seenList == NULL) 
      seenList = new HashTable(name, RW);
   seenList->ToDeleteOnExit();
   delete seenList;

   if (seenRelList == NULL)
      seenRelList = new HashTable(relName, RW);
   seenRelList->ToDeleteOnExit();
   delete seenRelList;

   if (rankList != NULL) RemoveSimNodeList(rankList);
   rankList = NULL;
   NRankList = 0;

   // create two temporary hash-tables for seenList & seenRekList
   seenList = new HashTable(name, RW);
   seenRelList = new HashTable(relName, RW);
}


//================================================================
// RemoveSimNodeList: Free all nodes in the ranked list of type SimData
//================================================================
void RetrievalModel:: RemoveSimNodeList(SimNode* list)
{
   SimNode* currList;

   while (list != NULL) {
      currList = list;
      list = list->next;
      if (currList->name != NULL) delete currList->name;
      delete currList;
   }
}



//***************************************************************************
//
//  Definition of functions for TextRetrieval class
//
//  TextRetrieval <- RetrievalModel
//  class for retrieving free-text data
//
//***************************************************************************

//==============================================================================
// Constructor
//==============================================================================
TextRetrieval:: TextRetrieval (TextIndexFile *indexfile,
                               int minNR, int maxNR, float minSim):
           RetrievalModel (minNR, maxNR, minSim)
{
   indexFile = indexfile;
   dir = indexFile->GetDIRS();
   origQueryStemList = currQueryStemList = NULL;
   newQueryTerms = NULL;
   stopWords = indexFile->GetStopWords();
   stemAlgo  = indexFile->GetStemAlgo();
   // setup typeName here
   typeName = strdup ("TEXT");
}

//==============================================================================
// Destructor
//==============================================================================
TextRetrieval:: ~TextRetrieval()
{
   if (origQueryStemList != NULL) delete origQueryStemList;
   if (currQueryStemList != NULL) delete currQueryStemList;
   if (newQueryTerms != NULL) RemoveSimNodeList (newQueryTerms);
   // do not delete indexFile, dir, stopWords and stemAlgo
}


//==============================================================================
// function to calculate similarity and rank them
//==============================================================================
void TextRetrieval:: ComputeRankList()
{
   
   NRankList = 0;
   if (rankList != NULL) RemoveSimNodeList(rankList);
   rankList = NULL;

   printf("\nText Retrieval in process ...\n");

   // set up a temp list of nodes containing at least one query term
   SetupTmpRankList();

   // Compute sim values and sort them
   if (rankList != NULL) {
      ComputeSim();
      rankList = util->SortList(rankList);
   }

   // Post-process the ranked list

  //ProcessRankList(); //Commented by Shen Mojun for MMretrieval feedback use

 /*  SimNode *curr=rankList;

   while (curr != NULL ) 
      { printf (" fileN=<%s> sim=%f \n", curr->name, curr->similarity);
        curr=curr->next;
      }     
 */  
   printf("Text Retrieval completed.\n");
} 


//==============================================================================
//  function to generate stem list for query and store as currQueryStemList
//==============================================================================
void TextRetrieval:: GenerateQueryList(const char* query)
{
   printf("\n***Text Processing A New Query ...\n");

   // Reset seenList, seenRelList, rankList, origQueryList & currQueryList
   DeleteTmpRelFiles();
   DeleteQueryFiles();
   // setup temp list for query
   origQueryStemList = new StringStemHT(origTextQueryHTName, RW);
   origQueryStemList->GenerateStemList(query, stopWords, stemAlgo);

   // go through the query terms and remove those not in inverted file
   const char *queryTerm;
   HashTable *headerDB = indexFile->GetHeaderDB();

// 
//  for (queryTerm=origQueryStemList->GetFirstKey(); queryTerm!=NULL;
//        queryTerm=origQueryStemList->GetNextKey() )
//      if ( !headerDB->Contain(queryTerm) ) {
//         printf("  IRModel::GenerateQueryList, queryTerm <%s> ", queryTerm);
//         printf("not in DB! Try other terms\n");
//         origQueryStemList->Delete(queryTerm);
//      }
//
//  Commented out by Dennis 250996
//
//  Cannot use for loop becos, after a Delete(), the deleted record will be removed and
//  hash table is moved up.  And therefore, we cannot use GetNextKey() in for loop.
//
//  soln.  use while loop & check whether it is necessary to re-get the first key
//	   after a deletion.
//

// added by Dennis
   
   int m_fGetFirstKey = 1;
   int m_fCurrIsFirstKey = 1;
   queryTerm=origQueryStemList->GetFirstKey(); 
   while (queryTerm!=NULL) {

      if ( !headerDB->Contain(queryTerm) ) {
         printf("  IRModel::GenerateQueryList, queryTerm [%s] ", queryTerm);
         printf("not in DB! Try other terms\n");

         //
         // no need to move to next key as Delete moves automatically
         //
         origQueryStemList->Delete(queryTerm);
         if (m_fCurrIsFirstKey = 1) {
           m_fGetFirstKey = 1;
         }
      }
      
      if (m_fGetFirstKey) {
         queryTerm=origQueryStemList->GetFirstKey(); 
         m_fGetFirstKey = 0;
         m_fCurrIsFirstKey = 1;
      }
      else {
         queryTerm=origQueryStemList->GetNextKey(); 
      }
      
   } // while

// added till here


   // now compute weight of query terms and copy it to currQueryStemList
   origQueryStemList = ComputeQueryWeights(origQueryStemList);
   currQueryStemList = origQueryStemList->Duplicate(currTextQueryHTName);
   currQueryStemList->DisplayContents();

}





//==============================================================================
// function to delete all temporary files used for queries
//==============================================================================
void TextRetrieval:: DeleteQueryFiles()
{
   if (newQueryTerms != NULL) RemoveSimNodeList (newQueryTerms);
   newQueryTerms = NULL;

   if (origQueryStemList == NULL) 
      origQueryStemList = new StringStemHT(origTextQueryHTName, RW);
   origQueryStemList->ToDeleteOnExit();
   delete origQueryStemList;

   if (currQueryStemList == NULL) 
      currQueryStemList = new StringStemHT(currTextQueryHTName, RW);
   currQueryStemList->ToDeleteOnExit();
   delete currQueryStemList;

   // set pointers to NULL
   origQueryStemList = currQueryStemList = NULL;
}


//==============================================================================
// function to calculate temporary ranked list containing all possible nodes
//==============================================================================
void TextRetrieval:: SetupTmpRankList()
{
   // set up list of files containing at least one query term
   int freq; float Wq, Wd;
   const char *queryTerm, *fileN;
   for (queryTerm=currQueryStemList->GetFirstKey(); queryTerm!=NULL;
        queryTerm=currQueryStemList->GetNextKey() ) {

      currQueryStemList->RetrieveStemFreqWt(queryTerm, freq, Wq);
      // open HashTable for queryTerm
      HashTable* termDB = indexFile->OpenInvertedTermDB (queryTerm);
      for (fileN=termDB->GetFirstKey(); fileN!=NULL; fileN=termDB->GetNextKey() ) {
         const char* content = termDB->Retrieve (fileN);
         util->StringToIntFloat (content, freq, Wd);         
         AddDocToRankList(fileN, Wq, Wd);
      }
      delete termDB;
   }
} 


//==============================================================================
// Add file to rank list and accumulate WqWd for each file
//==============================================================================
void TextRetrieval:: AddDocToRankList(const char *fileName, float Wq, float Wd)
{
   bool found = false;
   SimNode *aNode = rankList;
   while (aNode!=NULL && !found) {
      if (strcmp(aNode->name, fileName)==0) {
         // fileName already in ranked list
         aNode->sumWqWd += Wq*Wd;
         found = true;
      }
      aNode = aNode->next;
   }

   if (!found) {
      aNode = new SimNode;
      aNode->name = strdup(fileName);
      aNode->sumWqWd = Wq*Wd;
      aNode->next = rankList;   // insert aNode at beginning of ranked list
      rankList = aNode;
   }
}


//==============================================================================
// Compute weights of all query terms stored in a Hash-Table
// The frequncy of each query term is also given
//==============================================================================
StringStemHT* TextRetrieval:: ComputeQueryWeights (StringStemHT *queryStemHT)
{
   int freq; float wt;
   int maxFreq = queryStemHT->GetMaxFreq();

   // copy contents of queryStemHT to a temporary queryHT before updating
   // because can't access key sequence of HT and do updating at the same time
   HashTable *queryTmpHT = queryStemHT->Duplicate ("TEMP_DB");

   // Now update weights of query terms
   int NFiles = indexFile->GetNumofNodes();
   const char *key;
   for (key=queryTmpHT->GetFirstKey(); key!=NULL; key=queryTmpHT->GetNextKey()) {
      queryStemHT->RetrieveStemFreqWt (key, freq, wt);
      float idf = ComputeIDFWeight (key, NFiles);
      wt = ComputeQueTermWt (maxFreq, freq, idf);
      queryStemHT->UpdateStemFreqWt (key, freq, wt);
   }

   queryTmpHT->ToDeleteOnExit();
   delete queryTmpHT;

   return queryStemHT;
}
    
    

//***************************************************************************
//
//  Definition of functions for VectorSpace <- TextRetrieval
//
//  it implements the vector-space model developed by Salton.
//
//***************************************************************************
  	
//==============================================================================
//  Function to compute the new weights of all terms in Inverted File
//  it also comptes the sum of weight square for each file
//==============================================================================
void VectorSpace:: ComputeWeights()
{
   const char *term, *fileN;
   int NFiles = indexFile->GetNumofNodes();
   HashTable *headerDB = indexFile->GetHeaderDB();

   for (term=headerDB->GetFirstKey(); term!= NULL; term=headerDB->GetNextKey()) {
      float idf = ComputeIDFWeight(term, NFiles);
      HashTable* termDB = indexFile->OpenInvertedTermDB (term);
      // Duplicate termDB as tempDB for use to access all keys in termDB
      // otherwise the Retrieve and Replace operations on termDB will upset its keys
      HashTable *tempDB = termDB->DuplicateHT("TEMP_DB");

      int freq;
      float weight; 
      for (fileN=tempDB->GetFirstKey(); fileN!= NULL; fileN=tempDB->GetNextKey()) {
         const char* content = termDB->Retrieve (fileN);
         util->StringToIntFloat (content, freq, weight);
         weight = ComputeDocTermWt(fileN, freq, idf);
         char* str = util->IntFloatToString(freq, weight);
         // store new weight in term HT
         termDB->Replace(fileN, (const char*)str);
         // also accumulate wt**(2) in fileInforDB HT
         AddSumWtSq (fileN, weight); 
         delete str;
      }
      
      tempDB->ToDeleteOnExit();   // to remove tempDB permenently from file system
      delete tempDB;
      delete termDB;
   }
}
    

//==============================================================================
//  function to compute the  weight of a term in a Document
//  where Wt = (0.5 + 0.5*tf/maxtf) * log (N/n)
//==============================================================================
float VectorSpace:: ComputeDocTermWt(const char * fileName, int tf, float idf)
{
   float maxtf = (float) indexFile->GetMaxFreqforNode (fileName);
   float normalizedWt = 0.5 + 0.5 * (float)tf / maxtf;
   return (normalizedWt * idf);
}


//==============================================================================
//  function to compute the  weight of a term in Query 
//  where Wt = (0.5 + 0.5*tf/maxtf) * log (N/n)
//==============================================================================
float VectorSpace:: ComputeQueTermWt(int maxtf, int tf, float idf)
{
   float normalizedWt = 0.5 + 0.5 * (float)tf / (float)maxtf;
   return (normalizedWt * idf);
}

    
//==============================================================================
//  function to compute the IDF weight of a term
//  where idfWeight = log10(N/n)
//==============================================================================
float VectorSpace:: ComputeIDFWeight(const char *termName, int NFiles)
{
   int n = indexFile->GetSmallnforTerm (termName);
   float idfWt;
   if (n == 0)
      idfWt = 0.0;
   else  {
      idfWt = (float) NFiles / (float) n;
      idfWt = log10(idfWt);
   }
   return idfWt;
}

  	
//==============================================================================
//  function to compute the similarity between the query vector and
//  all documents in the ranked list
//==============================================================================
void VectorSpace:: ComputeSim()
{
   SimNode *aNode;
   int maxFreq=0;
   float sumWd2=0.0;

   // compute sumWq2
   float sumWq2 = 0.0;
   int freq; float weight;
   const char *queryTerm;
   for (queryTerm=currQueryStemList->GetFirstKey(); queryTerm!=NULL;
        queryTerm=currQueryStemList->GetNextKey() ) {
      currQueryStemList->RetrieveStemFreqWt(queryTerm, freq, weight);
      sumWq2 += weight * weight;
   }

   HashTable *fileInforDB = indexFile->GetFileInforDB();
   for (aNode=rankList; aNode!=NULL; aNode=aNode->next) {
      const char *maxFreqWt2 = fileInforDB->Retrieve((const char*)aNode->name);
      util->StringToIntFloat(maxFreqWt2, maxFreq, sumWd2);
      // Modify by TEO KAH CHOO to fix a bug of
      // sim = infinity when sumWq2 = 0
      // This occurs when a text term appears in all documents
      if (sumWq2 == 0)
         aNode->similarity = 0;
      else
         aNode->similarity = aNode->sumWqWd / sqrt( sumWq2 * sumWd2);
   }
}
      
  	
//==============================================================================
//  function to compute SumWtsq of a term and accumulate it in hashtable
//  fileInforDB
//==============================================================================
void VectorSpace:: AddSumWtSq (const char *fileName, float wt)
{
   HashTable *fileInforDB = indexFile->GetFileInforDB();
   const char *maxFreqWt2 = fileInforDB->Retrieve(fileName);
   if (maxFreqWt2 == NULL) {    //file is not in hashTable - ERROR
      printf("Error in AddSumSq(): fileName=%s\n", fileName);
      exit(-1);
   }
   else {
      int maxFreq=0;
      float sum2=0.0;
      util->StringToIntFloat(maxFreqWt2, maxFreq, sum2);
      sum2 += wt*wt;
      char *str = util->IntFloatToString(maxFreq, sum2);
      fileInforDB->Replace(fileName, (const char*)str);
      delete str;
   }
}

