//=========================================================================
//
//  irmodel.h
//  ---------
//  Class definitions for:
//  IRModel: It defines the class prototype for all retrieval models
//  VectorSpace: an implementation of the vector space model developed
//               by Salton of Cornell University.
//
//                     RetrievalModel
//                       |                              
//                       |                              
//                 TextRetrieval   
//                       |    
//                       |  
//                       |
//                   VectorSpace 
//
//
//  Implemented by:  T.S. Chua
//     Date of creation: June 1992
//     Last Modified:    3rd May, 1995
//
// Modified by Shen Mojun
//    Sept. 1995
//
//=========================================================================

#ifndef IRMODEL_H
#define IRMODEL_H

#include "standard.h"
#include "utility.h"
#include "filesys1.h"

#define MAXRANKLISTR  100 //max no of nodes in text ranked list resepectivelt
#define MAXRANKLIST   60 // max no of nodes in ranked list
#define MINRANKLIST   10 // min no of nodes with non-zero similarity values
#define MINSIM_TEXT  0.1 // if there are more than MINRANKLIST nodes, only
                         // those with sim-value > MINSIM will be displayed
#define MAXNEWQUERYTERMS  15  // max no of new query terms to be added 

// define names of temporary files used during retrieval
#define origTextQueryHTName   "tmp-origTextQuery"
#define currTextQueryHTName   "tmp-currTextQuery"
#define seenListHTName    "tmp-seenList-"      // seen and seenRel list names will be
#define seenRelListHTName "tmp-seenRelList-"   // appended by appropriate type name
                                               // such as "COLOR" to make them unique

#define MaxDminClr 5

//************************************************************************
//
//  class RetrievalModel
//  --------------------
//  a virtual class used to define the prototype of all retrieval models
//
//  Local variables:
//  indexFile: stores the index structures of respective MM data
//  NRankList: number of items in ranked list.
//  rankList: contains a ranked list of items retrieved based on query
//  seenList: contains list of items seen by users. It will not be displayed
//            in subsequent retrieval iterations
//  seenRelList: contains the list of relevant items found so far
//
//
//************************************************************************
class RetrievalModel
{
  protected:
    int  NRankList;              // no of entries in Rank list
    SimNode *rankList;	         // linked list to store results

    HashTable *seenList;         // list of items seen by users
    HashTable *seenRelList;      // list of relevant items seen by users
    DIRECTORIES *dir;
    HTUtilities *util;
    int minNRankList;            // min no of nodes in ranked list
    int maxNRankList;            // max no of nodes in ranked list
    float minSimilarity;         // min similarity of nodes in ranked list
    char *typeName;     // set to type TEXT, COLOR, COARSENESS, CONTRAST or 
                        // DIRECTIONALITY depending on the retrieval type. It is
                        // set in specialised class. Used mainly to create different
                        // seenList and seenRelList for different retrieval model.  
       
  public:
    RetrievalModel (int minNR, int maxNR, float minSim);
    virtual ~RetrievalModel();

    // Added by TEO KAH CHOO in June 1995
    void WriteToFile (char *fName);

    virtual void ComputeRankList () =0;
    virtual int *DisplayNJudgeRankList(int NRankList, SimNode *rankList);
    virtual void DisplayRankList(int NRankList, SimNode *rankList);
    virtual void ProcessRelevantJudgement(int *relevant)=0;
    virtual void DisplayNSelectNewQueryTerms () =0;
    virtual void GenerateNewQueryList () =0;
    virtual void ComputeWeights () = 0;
    virtual SimNode *GetRankList () { return rankList; }
    virtual int GetNRankList () { return NRankList; }
    virtual void ProcessRankList ();

  protected:
   // virtual void ProcessRankList ();
    virtual void DeleteTmpRelFiles();
    virtual void DeleteQueryFiles() =0;
    virtual void RemoveSimNodeList(SimNode *list);
};

//************************************************************************
//
//  class TextRetrieval <- RetrievalModel
//  -------------------------------------
//  class for retrieving free-text data
//
//
//************************************************************************
class TextRetrieval : public RetrievalModel
{   
  protected:
    TextIndexFile *indexFile;        // indices of free-text data
    StringStemHT *origQueryStemList; // original query list specified by users
    StringStemHT *currQueryStemList; // new (expanded) query list
    SimNode *newQueryTerms;       // holding new query terms derived from user feedback 
    StopList *stopWords;
    Stemming *stemAlgo;

  public:
    TextRetrieval (TextIndexFile *indexfile, int minNR, int maxNR, float minSim);
    virtual ~TextRetrieval();
    virtual void ComputeRankList ();
    virtual void GenerateQueryList(const char *query);
    virtual void ProcessRelevantJudgement (int *relevant) {};
    virtual void DisplayNSelectNewQueryTerms (){};
    virtual void GenerateNewQueryList (){};
    virtual void ComputeWeights() = 0;

  public://Shen protected:
    virtual void DeleteQueryFiles();
    virtual void SetupTmpRankList();  // setup possible nodes in a temp ranked list
    virtual void AddDocToRankList(const char *fileName, float Wq, float Wd);
    virtual StringStemHT* ComputeQueryWeights(StringStemHT *queryStemHT);
    virtual float ComputeDocTermWt(const char *fileName, int tf, float idf) = 0;
    virtual float ComputeQueTermWt(int maxtf, int tf, float idf) = 0;
    virtual float ComputeIDFWeight(const char* termName, int NTerms) = 0;
    virtual void ComputeSim() = 0;
};


//************************************************************************
//
//  class VectorSpace <- TextRetrieval 
//  ----------------------------------
//  it implements the vector-space model developed by Salton.
//
//  Local functions:
//  (only the function ComputeWeights() and its associated functions are
//   modified here. The rest are identical to class IRModel.
//  ComputeWeights(): it compute the tf.idf weights and the square root of
//      the sum of weight square. These quantities are required for the
//      vector-space model.
// 
//  The Term weight scheme used is:
//  
//  Doc term   wt, Wd = (0.5 + 0.5*tf/maxtf) * log(N/n)
//  Query term wt, Wq = (0.5 + 0.5*tf/maxtf) * log(N/n)
//
//  Similarity Wt = Sum_of_Wd*Wq / [sqrt(Sum_of_Wd**2) * sqrt(Sum_of_Wq**2)]
// 
//
//************************************************************************

class VectorSpace : public TextRetrieval
{
  public:
    VectorSpace (TextIndexFile *indexfile, int minNR, int maxNR, float minSim):
        TextRetrieval (indexfile, minNR, maxNR, minSim) {};
    virtual ~VectorSpace() {};
    virtual void ComputeWeights();

  protected:
    virtual float ComputeDocTermWt(const char *fileName, int tf, float idf);
    virtual float ComputeQueTermWt(int maxtf, int tf, float idf);
    virtual float ComputeIDFWeight(const char* termName, int NTerms);
    virtual void ComputeSim();
    virtual void AddSumWtSq(const char *fileName, float wt);
};


#endif
