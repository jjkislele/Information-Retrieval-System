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

	char* currentWD;
	StopList *stopWords = new StopList("stoplist.bin");
	Stemming *porter = new Stemming();
	DIRECTORIES *dirs = new DIRECTORIES;
	VectorSpace *textIR;


	dirs->mainDIR = getcwd((char *)NULL,64);
	currentWD = getcwd((char *)NULL,64);
	dirs->mainDIR=currentWD;
	dirs->directDIR=currentWD;
	dirs->invertedDIR=currentWD;
	printf("mainIDR=<%s>\n",dirs->mainDIR);

	TextIndexFile *text = new TextIndexFile("mainText",dirs,RW,true,
				stopWords,porter);
//the first line of the filelist.dat is the full path name of the directory that contains the input files
	text->ReadFileList("filelist.dat");


	textIR = new VectorSpace(text,1,30,0.0);

	//fill in fileInfoDB with the square root of sum of weights
	printf("Computing Weights, please wait...\n");
	textIR->ComputeWeights();

	delete dirs;
	delete stopWords;
	delete porter;
	delete text;
	delete currentWD;
}
