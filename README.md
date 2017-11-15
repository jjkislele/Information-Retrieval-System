## Information Retrieval System
## ATTENTION!!! 
The whole work IS NOT done by myself. This is a course project. All the codes are provided by my teacher. And maybe the original authors are *T.S. Chua* and *Shen Mo Jun*. 

**All rights reserved by the authors. Should this violate your rights, kindly remind me. I shall delete your document without hesitation and delay.**

--------------------------

This is the original usage of the project

--------------------------

## INSTRUCTIONS TO USE THE PROGRAMS

================================

### To uncompress the program:

```
tar -xvf irs3.tar
```
There will be some files in the main directory and two sub-directories named "data" and "indexdb" respectively.

There are 5 sample documents in the directory of "data". 

### To compile the program:

1. Go to the your main directory where the programs are stored and run:

`make` - to make irmain

and `make index` to make irindex

### To execute the program:

1. Go to sub-directory "indexdb".

Modify the first line of "filelist.dat" to include the full path to the directory where your list of data files are stored

2. Go to sub-directory "indexdb"

run `../irindex` to create the indices: this will take a while when computing weights for vector space model. 

run `../irmain` to try out different queries. 

### For Linux, use the version under irslinux.tar:

It is a little bit different from the Unix version. It works on Linux **Mandrake** which uses gcc-2.95.2 and g++-2.95.2. 

For other linux versions, the location of "ndbm.h" may change. In **Mandrake** we use "#include<db1/ndbm.h>" (in "filesys1.h"). You may probably (maybe, not sure) need to change two lines in Makefile for generating irmain and irindex.  In **Solaris**, the ndbm files are stored as .dir and .pag. But in **Mandrake**, the files are stored only as .db.  After changing the path of including the ndbm.h, the compiling procedure should be similar as mentioned above.
