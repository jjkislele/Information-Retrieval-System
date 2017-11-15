//the code works on sununx
//it need g++ compiler
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "standard.h"
#include "filesys1.h"
#include "irmodel.h"
#include "utility.h"

int main()
{
	VectorSpace *textIR;
	char* currentWD;
	SimNode *retrievedList=NULL;
	StopList *stopWords = new StopList("stoplist.bin");
	Stemming *porter = new Stemming();
	DIRECTORIES *dirs = new DIRECTORIES;
	char inputQuery[200];

	dirs->mainDIR = getcwd((char *)NULL,64);
	currentWD = getcwd((char *)NULL,64);
	dirs->mainDIR=currentWD;
	dirs->directDIR=currentWD;
	dirs->invertedDIR=currentWD;
	printf("mainIDR=<%s>\n",dirs->mainDIR);

	TextIndexFile *text = new TextIndexFile("mainText",dirs,RW,false,
				stopWords,porter);

	textIR = new VectorSpace(text,1,30,0.0);

	//fill in fileInfoDB with the square root of sum of weights
	
	while(1)
	{
		printf("Input your Query:\n");
		fgets(inputQuery,200,stdin);
		if(strncmp(inputQuery,"quit",4)==0) break;
		textIR->GenerateQueryList(inputQuery);
		textIR->ComputeRankList();
		retrievedList = textIR->GetRankList();
		textIR->DisplayRankList(30,retrievedList);

	}
	delete stopWords;
	delete porter;
	delete currentWD;
	delete dirs;
	delete text;
}
