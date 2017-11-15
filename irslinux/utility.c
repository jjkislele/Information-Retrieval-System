//***********************************************************
// Last Modified Date: July 1995
// Modified by: Shen Mo Jun
//
//**********************************************************
  
#include <string.h>
#include "utility.h"

//**********************************************************
//
//  Definition of functions for class StopList
//
//***********************************************************
//
//===================================
// Constructor
//===================================
StopList:: StopList (const char *swFile)
{
   FILE *stopFile = fopen (swFile, "rb");
   if ((stopPtr = (char *) malloc (STOPNUM*WDSIZE)) == NULL) {
      printf ("not enough memory\n");
      exit(-1);
   }
   if (stopFile)
      numStopWords = fread(stopPtr, WDSIZE, STOPNUM, stopFile);
   else {
      printf("\n\n Stop file <%s> not opened!!\n\n Check file name\n", swFile);
      numStopWords = 0;
   }
   fclose (stopFile);
}

//===================================
// Destructor
//===================================
StopList :: ~StopList ()
{
   free ((char *) stopPtr);
}

//===================================
// Non-Class Function
//===================================
int Compare (const void *key, const void *stopPtr)
{
   return memcmp ((char *)key, (char*)stopPtr, WDSIZE);
}

//===================================
// Return true is 'key' is a stop word
//===================================
bool StopList:: IsStopWord (char *key)
{
   char *bs = NULL;
   bs = (char *)bsearch(key, stopPtr, numStopWords, WDSIZE, Compare);

/*
//   *** Test Stop List Codes
   static int count = 1;
   if (count<6) {
      printf("\n%dst call to StopWords: N=%d, Key=%s", count,numStopWords, key);
      if (bs == NULL) printf(" Outcome=false\n\n");
      else            printf(" OutCome=true\n\n");
      count++;
   }
//   ***
*/
   if (bs == NULL )
      return false;
   else
      return true;
}



//****************************************************************
//
//  Definition of functions for HTUtilities class
//
//****************************************************************
//
//==========================================================
// convert a floating value into a string
//===========================================================
char* HTUtilities:: FloatToString (float value)
{
   char buffer[80];
   char *ptr;
   sprintf (buffer, "%f", value);
   ptr = new char[strlen(buffer) + 1];
   strcpy(ptr, buffer);
   return ptr;
}

//
//==========================================================
// convert an integer value into a string
//===========================================================
char* HTUtilities:: IntToString (int value)
{
   char buffer[80];
   char *ptr;
   sprintf (buffer, "%d", value);
   ptr = new char[strlen(buffer) + 1];
   strcpy(ptr, buffer);
   return ptr;
}

//
//================================================================
// IntFloatToString: pack freq and wt infor into a string separated by
//             a space for storing in HashTable
//================================================================
char* HTUtilities:: IntFloatToString(int freq, float wt)
{
   char* freqStr = IntToString(freq);
   char* wtStr = FloatToString(wt);
   char* str = new char[strlen(freqStr) + strlen(wtStr) + 3];
   strcpy(str, freqStr);
   strcat(str, " ");
   strcat(str, wtStr);
   delete freqStr;
   delete wtStr;
   return str;
}

//
//================================================================
// IntStringArrayToString: pack n follow by n strings into a long
//             string. All items are separated by spaces.
//================================================================
char* HTUtilities:: IntStringArrayToString(int n, const char **str)
{
   char* intStr = IntToString(n);

   // calculate size of string buffer required
   int size = 0;
   int i;
   for (i=0; i<n; i++) size += strlen(str[i]);
   size += strlen(intStr) + n + 1;

   char* packedStr = new char[size];
   strcpy(packedStr, intStr);
   for (i=0; i<n; i++) {
      strcat(packedStr, " ");
      strcat(packedStr, str[i]);
   }
   delete intStr;
   return packedStr;
}

//
//================================================================
// StringToIntFloat: unpack a string packed using IntFloatToString into its
//               constituent parts (ie freq and wt)
//================================================================
void HTUtilities:: StringToIntFloat(const char *str, int &freq, float &wt)
{
   freq = atoi(str);
   char *strWtName = strchr(str, ' ') + 1; // points to wt part of str
   wt = atof(strWtName);
}

//
//================================================================
// StringToIntStringArray: unpack a long string created using
//             IntStringArrayToString to its constituent parts of 
//             an integer n and an array of string str.
//================================================================
char** HTUtilities:: StringToIntStringArray(const char *packedStr, int &n)
{
   n = atoi(packedStr);
   char *strings = strchr(packedStr,' ')+1; //points to rest of strings

   char** str = new char*[n];
   char *strToken;
   bool init = true;
   for (int i=0; i<n; i++) {
      // get the next string token from strings
      if (init) {
         strToken = strtok(strings, " ");
         init = false;
      }
      else strToken = strtok(NULL, " ");

      str[i] = strdup(strToken);
//    delete strToken;
   }
   return str;
}

//==========================================================
// convert a floating value into a string with certain length
// no zero brfore the point. The format of string is ".xxxx".
//===========================================================
char* HTUtilities:: FloatToFormatString (float value)
{
   char buffer[80];
   char *ptr;
   sprintf (buffer, "%.6f", value);
   ptr = new char[strlen(buffer)];

   for (int i=0; i<strlen(buffer); i++)
     ptr[i]= buffer[i+1];   
    
   return ptr;
}

//
//================================================================
// FloatArrayToString: pack n follow by n floats into a long
//             string. All items are separated by spaces.
// ---- Shen Mojun
//================================================================
char* HTUtilities:: FloatArrayToString(int n, float *value)
{
   
   // convert float array to string array
    char **str = new char*[n];
    int i;
    for (i=0; i<n; i++) str[i]=FloatToFormatString(value[i]);   


   // calculate size of string buffer required
   int size = 0;
   for (i=0; i<n; i++) size += strlen(str[i]);
   size += n + 1;

   char* packedStr = new char[size]; 
  
   strcpy(packedStr, str[0]);
   for (i=1; i<n; i++) {
      strcat(packedStr, " ");
      strcat(packedStr, str[i]);      
   }

   delete str;
   return packedStr;
}

//
//================================================================
// StringToFloatArray: unpack a long string created using
//              FlaotArrayToString to its constituent parts of 
//              an integer n and an array of string str.
// --- Shen Mojun
//================================================================
float *HTUtilities::StringToFloatArray(const char *packedStr, int n)
{

   float *str = new float[n];
   char *strToken = new char[15];
   for (int i=0; i<n; i++)
    {
       int k=0;
       while ((*packedStr) != ' ')
         { strToken[k]=(*packedStr);
           k++; 
           packedStr++;
          }
       strToken[k]='\0';
       packedStr++;   
     str[i] = (float)atof(strToken);
   }
   delete strToken;
   return(str);
}


//
//================================================================
// GetFullImageName: extract the corresponding full-path image name for
// the fullTextName. This function assume that the text is stored in 
// ../parent/TEXT sub-directory, and the image is in ../parent/IMAGE
// sub-directory. Both corresponding text and image files have the 
// same name.
//================================================================
char* HTUtilities::GetFullImageName(const char *fullTextName)
{
   char* name1 = strdup(fullTextName);  // make a copy of fullTextName
   char* imageName=new char[strlen(name1)+3];
   strcpy(imageName, "\0");   // copy a NULL string to it first
   if (name1[0]=='/') {       // first character is /, copy it to imageName
      strcat(imageName, "/");
      name1++;
   }
   char *strToken;
   if (strchr(name1,'/')!=NULL) {    // name1 contains '/'
      strToken = strtok(name1, "/");  // extract the first token
      while (strcmp(strToken,"TEXT")!=0 && strToken!=NULL) {
         strcat(imageName, strToken);
         strcat(imageName, "/");
//       delete strToken;
         strToken=strtok(NULL,"/");  // get next token until TEXT is found
      }
      strcat(imageName, "IMAGE/");
   }


   // now add in the name follow by image extension
   char* justNameExt = JustNameExt(fullTextName); // extract the name part
   char* justName = JustName(justNameExt);
   strcat(imageName, justName);
   strcat(imageName, ".tif");
   delete justNameExt;
   delete justName;

// delete name1;

   return imageName;
}


//
//================================================================
// GetFullTextName: extract the corresponding full-path text name for
// the fullImageName. This function assume that the text is stored in 
// ../parent/TEXT sub-directory, and the image is in ../parent/IMAGE
// sub-directory. see GetFullImageName().
//================================================================
char* HTUtilities::GetFullTextName(const char *fullImageName)
{
   char* name1 = strdup(fullImageName);  // make a copy of fullImageName
   char* textName=new char[strlen(name1)+3];
   strcpy(textName, "\0");   // copy a NULL string to it first
   if (name1[0]=='/') {       // first character is /, copy it to textName
      strcat(textName, "/");
      name1++;
   }
   char *strToken;
   if (strchr(name1,'/')!=NULL) {       // name contains '/'
      strToken = strtok(name1, "/");    // extract the first token
      while (strcmp(strToken,"IMAGE")!=0 && strToken!=NULL) {
         strcat(textName, strToken);
         strcat(textName, "/");
//       delete strToken;
         strToken=strtok(NULL,"/");  // get next token until TEXT is found
      }
      strcat(textName, "TEXT/");
   }

   // now add in the name follow by text extension
   char* justNameExt= JustNameExt(fullImageName); // extract the name part
   char* justName= JustName(justNameExt);  
   strcat(textName, justName);
   strcat(textName, ".txt");
   delete justNameExt;
   delete justName;

// delete name1;

   return textName;
}

//
//================================================================
// JustNameExt: extract the name with file extension by removing
// only its path component.
//================================================================
char* HTUtilities::JustNameExt(const char *fullName)
{
   char* name= strrchr(fullName, '/');
   if (name==NULL)
      // string does not contain '/'
      return (strdup(fullName));
   else
      // advance pass '/' to point to actual name
      return (strdup(++name));
}

//
//================================================================
// JustName: extract just the name part (without extension).
//================================================================
char* HTUtilities::JustName(const char *fullName)
{
   // First remove the file path
   char* name= strrchr(fullName, '/');
   if (name==NULL)
      // string does not contain '/'
      name = (char*) fullName;
   else
      // advance pass '/' to point to actual name
      ++name;

   // Now remove the file extension
   char* justName=strchr(name, '.');
   if (justName==NULL)
      // string does not contain '.'
      return (strdup(name));
   else {
      char *name1 = strdup(name); // prevent strtok writing '\0' to input fullName
      char *strToken = strtok(name1, ".");
      return (strdup(strToken));
   }   
}

//
//================================================================
// SortList(): sorts an input linked list of type SImNode in descending
// order based on similarity value
//================================================================
SimNode* HTUtilities:: SortList (SimNode *list)
{
   SimNode *prevNode, *currNode, *nextNode;

   if (list == NULL) return list;
   bool done = false;
   while (!done) {
      done = true;
      prevNode = currNode = list;
      nextNode = list->next;
      while (nextNode != NULL) {
         if (nextNode->similarity > currNode->similarity) {
            // to exchange positions between nextNode and currNode
            done = false;
            if (currNode == list) {
               // To exchange the header
               list = prevNode = nextNode;
               currNode->next = nextNode->next;
               nextNode->next = currNode;
               // to setup currNode & nextNode properly
               currNode = list;
               nextNode = currNode->next;
            }
            else {
               // To exchange non-header node
               prevNode->next = currNode->next;
               currNode->next = nextNode->next;
               nextNode->next = currNode;
               // to setup currNode & nextNode properly
               currNode = prevNode->next;
               nextNode = currNode->next;
            }
         }
         prevNode = currNode;
         currNode = nextNode;
         nextNode = nextNode->next;
      }
   }
   return list;
}


//**********************************************************
//
//  Definition of functions for class Stemming
//  It implements the M.F.Porter's algorithm for suffix stripping.
//  Refer to notes for details of Porter's Algorithm
//
//***********************************************************
//
//===================================
// Constructor
//===================================
Stemming:: Stemming () {}


//===================================
// Destructor
//===================================
Stemming:: ~Stemming () {}


//=============================================================
//  Stem
//  ----
//  This function takes in a word key and returns the
//  stem form of word using the same pointer key
//  It implements the 5 major steps of the Porter's algorithm
//
//=============================================================
int Stemming:: Stem(char *word)
{
  int i = strlen(word) - 1;
  int retvalue = i;

  // Step 1 a 

  if (i >= 0 && word[i] == 's') {
    if (i >= 1) {
      switch (word[i-1]) {
      case 'e' :
	if (i >= 2 && word[i-2] == 'i') {
	  word[i-1] = word[i] = '\0';
	  i -= 2;
	} else {
	  if (i >= 3 && word[i-2] == 's' && word[i-3] == 's') {
	    word[i] = word[i-1] = '\0';
	    i -= 2;
	  } else {
	    word[i] = '\0';
	    i--;
	  }
	}
	break;
      case 's' :
	break;
      default :
	word[i] = '\0';
	i--;
	break;
      }
    } else {
      word[i] = '\0';
      i--;
    }
  }

  // Step 1 b 

  if (i >= 1) {
    int flag = 0;
    if (word[i] == 'd' && word[i-1] == 'e') {
      if (i >= 2 && word[i-2] == 'e') {
	if (VCNum(word, i-3) > 0) {
	  word[i] = '\0';
	  i--;
	}
      } else {
	if (Vowel(word, i-2) == 1) {
	word[i] = word[i-1] = '\0';
	i -= 2;
	flag = 1;
	}
      }
    } else {
      if (i >= 2 && word[i] == 'g' && word[i-1] == 'n' && 
	  word[i-2] == 'i') {
	if (Vowel(word, i-3) == 1) {
	  word[i] = word[i-1] = word[i-2] = '\0';
	  i -= 3;
	  flag = 1;
	}
      }
    }
    if (flag == 1) {
      if (i >= 1) {
	if ((word[i-1] == 'a' && word[i] == 't') ||
	    (word[i-1] == 'b' && word[i] == 'l') ||
	    (word[i-1] == 'i' && word[i] == 'z')) {
	  i++;
	  word[i] = 'e';
	} else {
	  if (DoubleLetter(word, i) == 1 &&
	      !(word[i] == 'l' || word[i] == 's' || word[i] == 'z')) {
	    word[i] = '\0';
	    i--;
	  } else {
	    if (VCNum(word, i) == 1 && Endcvc(word, i) == 1) {
	      i++;
	      word[i] = 'e';
	    }
	  }
	}
      }
    }
  }

  // Step 1 c 

  if (word[i] == 'y') {
    if (Vowel(word, i-1) == 1) {
      word[i] = 'i';
    }
  }

  // Step 2 

  if (i >= 1) {
    switch (word[i-1]) {
    case 'a' :
      if (word[i] == 'l') {
	if (i >= 5 && word[i-2] == 'n' && word[i-3] == 'o' &&
	    word[i-4] == 'i' && word[i-5] == 't') {
	  if (i >= 6 && word[i-6] == 'a') {
	    if (VCNum(word, i-7) > 0) {
	      word[i] = word[i-1] = word[i-2] = word[i-3] = '\0';
	      word[i-4] = 'e';
	      i -= 4;
	    }
	  } else {
	    if (VCNum(word, i-6) > 0) {
	      word[i] = word[i-1] = '\0';
	      i -= 2;
	    }
	  }
	}
      }
      break;
    case 'c' :
      if (word[i] == 'i') {
	if (i >= 3 && word[i-2] == 'n' &&
	    (word[i-3] == 'a' || word[i-3] == 'e')) {
	  if (VCNum(word, i-4) > 0) {
	    word[i] = 'e';
	  }
	}
      }
      break;
    case 'e' :
      if (word[i] == 'r') {
	if (i >= 3 && word[i-2] == 'z' && word[i-3] == 'i') {
	  if (VCNum(word, i-4) > 0) {
	    word[i] = '\0';
	    i--;
	  }
	}
      }
      break;
    case 'l' :
      if (word[i] == 'i') {
	if (i >= 2 && word[i-2] == 'e') {
	  if (VCNum(word, i-3) > 0) {
	    word[i] = word[i-1] = '\0';
	    i -= 2;
	  }
	} else {
	  if (i >= 3 && word[i-2] == 'b' && word[i-3] == 'a') {
	    if (VCNum(word, i-4) > 0) {
	      word[i] = 'e';
	    }
	  } else {
	    if (i >= 3 && word[i-2] == 'l' && word[i-3] == 'a') {
	      if (VCNum(word, i-4) > 0) {
		word[i] = word[i-1] = '\0';
		i -= 2;
	      }
	    } else {
	      if (i >= 4 &&
		  ((word[i-2]=='t' && word[i-3]=='n' && word[i-4]=='e') ||
		   (word[i-2]=='s' && word[i-3]=='u' && word[i-4]=='o'))) {
		if (VCNum(word, i-5) > 0) {
		  word[i] = word[i-1] = '\0';
		  i -= 2;
		}
	      }
	    }
	  }
	}
      }
      break;
    case 'o' :
      switch (word[i]) {
      case 'n' :
	if (i >= 4 && word[i-2] == 'i' && word[i-3] == 't' &&
	    word[i-4] == 'a') {
	  if (i >= 6 && word[i-5] == 'z' && word[i-6] == 'i') {
	    if (VCNum(word, i-7) > 0) {
	      word[i] = word[i-1] = word[i-2] = word[i-3] = '\0';
	      i -= 4;
	      word[i] = 'e';
	    }
	  } else {
	    if (VCNum(word, i-5) > 0) {
	      word[i] = word[i-1] = '\0';
	      i -= 2;
	      word[i] = 'e';
	    }
	  }
	}
	break;
      case 'r' :
	if (i >= 3 && word[i-2] == 't' && word[i-3] == 'a') {
	  if (VCNum(word, i-4) > 0) {
	    word[i] = '\0';
	    i--;
	    word[i] = 'e';
	  }
	}
	break;
      default :
	break;
      }
      break;
    case 's' :
      switch (word[i]) {
      case 's' :
	if (i >= 6 && word[i-2] == 'e' && word[i-3] == 'n' &&
	    ((word[i-4] == 'e' && word[i-5] == 'v' && word[i-6] == 'i') ||
	     (word[i-4] == 'l' && word[i-5] == 'u' && word[i-6] == 'f') ||
	     (word[i-4] == 's' && word[i-5] == 'u' && word[i-6] == 'o'))) {
	  if (VCNum(word, i-7) > 0) {
	    word[i] = word[i-1] = word[i-2] = word[i-3] = '\0';
	    i -= 4;
	  }
	}
	break;
      case 'm' :
	if (i>=4 && word[i-2]=='i' && word[i-3]=='l' && word[i-4]=='a') {
	  if (VCNum(word, i-5) > 0) {
	    word[i] = word[i-1] = word[i-2] = '\0';
	    i -= 3;
	  }
	}
	break;
      default :
	break;
      }
      break;
    case 't' :
      if (word[i] == 'i') {
	if (i >= 2 && word[i-2] == 'i') {
	  if (i >= 3) {
	    switch (word[i-3]) {
	    case 'l' :
	      if (i >= 4) {
		switch (word[i-4]) {
		case 'a' :
		  if (VCNum(word, i-5) > 0) {
		    word[i] = word[i-1] = word[i-2] = '\0';
		    i -= 3;
		  }
		  break;
		case 'i' :
		  if (i >= 5 && word[i-5] == 'b') {
		    if (VCNum(word, i-6) > 0) {
		      word[i] = word[i-1] = word[i-2] = '\0';
		      i -= 3;
		      word[i] = 'e';
		      word[i-1] = 'l';
		    }
		  }
		  break;
		default :
		  break;
		}
	      }
	      break;
	    case 'v' :
	      if (i >= 4 && word[i-4] == 'i') {
		if (VCNum(word, i-5) > 0) {
		  word[i] = word[i-1] = '\0';
		  i -= 2;
		  word[i] = 'e';
		}
	      }
	      break;
	    default :
	      break;
	    }
	  }
	}
      }
      break;
    default :
      break;
    }
  }

  // Step 3 

  if (i >= 0) {
    switch (word[i]) {
    case 'e' :
      if (i >= 4) {
	if ((word[i-1] == 't' && word[i-2] == 'a' &&
	     word[i-3] == 'c' && word[i-4] == 'i') ||
	    (word[i-1] == 'z' && word[i-2] == 'i' &&
	     word[i-3] == 'l' && word[i-4] == 'a')) {
	  if (VCNum(word, i-5) > 0) {
	    word[i] = word[i-1] = word[i-2] = '\0';
	    i -= 3;
	  }
	} else {
	  if (word[i-1] == 'v' && word[i-2] == 'i' &&
	      word[i-3] == 't' && word[i-4] == 'a') {
	    if (VCNum(word, i-5) > 0) {
	      word[i] = word[i-1] = word[i-2] = word[i-3] = word[i-4] = '\0';
	      i -= 5;
	    }
	  }
	}
      }
      break;
    case 'i' :
      if (i >= 4) {
	if (word[i-1] == 't' && word[i-2] == 'i' &&
	    word[i-3] == 'c' && word[i-4] == 'i') {
	  if (VCNum(word, i-5) > 0) {
	    word[i] = word[i-1] = word[i-2] = '\0';
	    i -= 3;
	  }
	}
      }
      break;
    case 'l' :
      if (i >= 1) {
	switch (word[i-1]) {
	case 'a' :
	  if (i >= 3) {
	    if (word[i-2] == 'c' && word[i-3] == 'i') {
	      if (VCNum(word, i-4) > 0) {
		word[i] = word[i-1] = '\0';
		i -= 2;
	      }
	    }
	  }
	  break;
	case 'u' :
	  if (i >= 2) {
	    if (word[i-2] == 'f') {
	      if (VCNum(word, i-3) > 0) {
		word[i] = word[i-1] = word[i-2] = '\0';
		i -= 3;
	      }
	    }
	  }
	  break;
	default :
	  break;
	}
      }
      break;
    case 's' :
      if (i >= 3) {
	if (word[i-1] == 's' && word[i-2] == 'e' && word[i-3] == 'n') {
	  if (VCNum(word, i-4) > 0) {
	    word[i] = word[i-1] = word[i-2] = word[i-3] = '\0';
	    i -= 4;
	  }
	}
      }
      break;
    default :
      break;
    }
  }

  // Step 4

  if (i >= 1) {
    switch (word[i-1]) {
    case 'a' :
      if (word[i] == 'l') {
	if (VCNum(word, i-2) > 1) {
	  word[i] = word[i-1] = '\0';
	  i -= 2;
	}
      }
      break;
    case 'c' :
      if (word[i] == 'e') {
	if (i >= 3) {
	  if (word[i-2] == 'n' && (word[i-3] == 'a' || word[i-3] == 'e')) {
	    if (VCNum(word, i-4) > 1) {
	      word[i] = word[i-1] = word[i-2] = word[i-3] = '\0';
	      i -= 4;
	    }
	  }
	}
      }
      break;
    case 'e' :
      if (word[i] == 'r') {
	if (VCNum(word, i-2) > 1) {
	  word[i] = word[i-1] = '\0';
	  i -= 2;
	}
      }
      break;
    case 'i' :
      if (word[i] == 'c') {
	if (VCNum(word, i-2) > 1) {
	  word[i] = word[i-1] = '\0';
	  i -= 2;
	}
      }
      break;
    case 'l' :
      if (word[i] == 'e') {
	if (i >= 3) {
	  if (word[i-2] == 'b' && (word[i-3] == 'a' || word[i-3] == 'i')) {
	    if (VCNum(word, i-4) > 1) {
	      word[i] = word[i-1] = word[i-2] = word[i-3] = '\0';
	      i -= 4;
	    }
	  }
	}
      }
      break;
    case 'n' :
      if (word[i] == 't') {
	if (i >= 2) {
	  switch (word[i-2]) {
	  case 'a' :
	    if (VCNum(word,i-3) > 1) {
	      word[i] = word[i-1] = word[i-2] = '\0';
	      i -= 3;
	    }
	    break;
	  case 'e' :
	    if (i >= 3  && word[i-3] == 'm') {
	      if (i >= 4 && word[i-4] == 'e') {
		if (VCNum(word, i-5) > 1) {
		  word[i] = word[i-1] = word[i-2] =
		  word[i-3] = word[i-4] = '\0';
		  i -= 5;
		}
	      } else {
		if (VCNum(word, i-4) > 1) {
		  word[i] = word[i-1] = word[i-2] = word[i-3] = '\0';
		  i -= 4;
		}
	      }
	    } else {
	      word[i] = word[i-1] = word[i-2] = '\0';
	      i -= 3;
	    }
	    break;
	  default :
	    break;
	  }
	}
      }
      break;
    case 'o' :
      if (word[i] == 'u') {
	if (VCNum(word, i-2) > 1) {
	  word[i] = word[i-1] = '\0';
	  i -= 2;
	}
      } else {
	if (i >= 2 && word[i] == 'n' && word[i-2] == 'i') {
	  if ((word[i-3] == 's' || word[i-3] == 't') &&
	      VCNum(word, i-3) > 1) {
	    word[i] = word[i-1] = word[i-2] = '\0';
	    i -= 3;
	  }
	}
      }
      break;
    case 's' :
      if (i >= 2 && word[i] == 'm' && word[i-2] == 'i') {
	if (VCNum(word, i-3) >1) {
	  word[i] = word[i-1] = word[i-2] = '\0';
	  i -= 3;
	}
      }
      break;
    case 't' :
      if (i >= 2 && ((word[i] == 'e' && word[i-2] == 'a') ||
	   (word[i] == 'i' && word[i-2] == 'i'))) {
	if (VCNum(word, i-3) > 1) {
	  word[i] = word[i-1] = word[i-2] = '\0';
	  i -= 3;
	}
      }
      break;
    case 'u' :
      if (i>=2 && word[i-2]=='o' && word[i-1]=='u' && word[i]=='s') {
	if (VCNum(word, i-3) > 1) {
	  word[i] = word[i-1] = word[i-2] = '\0';
	  i -= 3;
	}
      }
      break;
    case 'v' :
      if (i >= 2 && word[i] == 'e' && word[i-2] == 'i') {
	if (VCNum(word, i-3) > 1) {
	  word[i] = word[i-1] = word[i-2] = '\0';
	  i -= 3;
	}
      }
      break;
    case 'z' :
      if (i >= 2 && word[i] == 'e' && word[i-2] == 'i') {
	if (VCNum(word, i-3) > 1) {
	  word[i] = word[i-1] = word[i-2] = '\0';
	  i -= 3;
	}
      }
      break;
    default :
      break;
    }
  }

  // Step 5 a 

  if (i >= 0) {
    if (word[i] == 'e') {
      if ((VCNum(word, i-1) > 1) ||
	  (VCNum(word, i-1) == 1 && !Endcvc(word, i-1))) {
	word[i] = '\0';
	i--;
      }
    }
  }

  // Step 5 b

  if (VCNum(word, i) > 1 && DoubleLetter(word, i) == 1 && word[i] == 'l') {
    word[i] = '\0';
    i--;
  }

  return (retvalue > i);
}


//*******************************************************************
//  VCNum
//  -----
//  The function VCNum returns the measure of any word or word part.
//  Any word, or part of a word, has the following form :
//          m
//  [C] (VC) [V]
//    where C is a series of consonant and
//          V is a series of vowel.
//  Hence, the function returns the number of repetition of VC in
//  the word, or the part of a word.
//
//********************************************************************

int Stemming:: VCNum(char *word, int i)
{
  int vcnum = 0;
  int flag;
  int consonant = 1;
  for (int k = 0; k <= i; k++) {
    if (word[k] != 'a' && word[k] != 'e' && word[k] != 'i' &&
	word[k] != 'o' && word[k] != 'u') {
      if (word[k] == 'y') {
	if (k == 0) {
	  flag = 1;
	} else {
	  if (consonant == 1) {
	    flag = 0;
	  } else {
	    flag = 1;
	  }
	}
      } else {
	flag = 1;
      }
    } else {
      flag = 0;
    }
    if (flag == 1) {
      if (consonant == 0) {
	vcnum++;
      }
    }
    consonant = flag;
  }
  return vcnum;
}


//*************************************************************
//  Vowel
//  -----
//  The function determines if the stem contains a vowel.
//
//*************************************************************

int Stemming:: Vowel(char *word, int i)
{
  int consonant = 0;
  for (int k = 0; k <= i; k++) {
    if (word[k]  != 'a' && word[k] != 'e' && word[k] != 'i' &&
	word[k] != 'o' && word[k] != 'u') {
      if (word[k] == 'y') {
	if (consonant == 1) {
	  return 1;
	}
      }
      consonant = 1;
    } else {
      return 1;
    }
  }
  return 0;
}


//*************************************************************
//  DoubleLetter
//  ------------
//  The function determines if the stem ends with a double consonant.
//
//*************************************************************

int Stemming:: DoubleLetter(char *word, int i)
{
  if (i >= 1)
    if (word[i] == word[i-1])
      if (word[i] != 'a' && word[i] != 'e' && word[i] != 'i' &&
	  word[i] != 'o' && word[i] != 'u' && word[i] != 'y')
	return 1;
  return 0;
}

//*************************************************************
//  Endcvc
//  ------
//  The function checks if the stem end with cvc, where the second
//  c is not w, x or y.
//
//*************************************************************

int Stemming:: Endcvc(char *word, int i)
{
  if (i >= 2) {
    if (word[i] != 'a' && word[i] != 'e' && word[i] != 'i' &&
	word[i] != 'o' && word[i] != 'u' && word[i] != 'y' &&
	word[i] != 'w' && word[i] != 'x') {
      if (word[i-1] == 'a' || word[i-1] == 'e' || word[i-1] == 'i' ||
	  word[i-1] == 'o' || word[i-1] == 'u' || word[i-1] == 'y') {
	if (word[i-2] != 'a' && word[i-2] != 'e' && word[i-2] != 'i' &&
	    word[i-2] != 'o' && word[i-2] != 'u') {
	  if (word[i-2] == 'y') {
	    int consonant = 0;
	    for (int j = 0; j < i-2; j++) {
	      if (word[j] != 'a' && word[j] != 'e' && word[j] != 'i' &&
		  word[j] != 'o' && word[j] != 'u') {
		if (word[j] == 'y') {
		  if (consonant == 1) {
		    consonant = 0;
		  } else {
		    consonant = 1;
		  }
		} else {
		  consonant = 1;
		}
	      } else {
		consonant = 0;
	      }
	    }
	    if (consonant == 0) {
	      return 1;
	    }
	  } else {
	    return 1;
	  }
	}
      }
    }
  }
  return 0;
}
