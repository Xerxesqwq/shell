#include "proj.h"
#ifndef __TRIE__
#define __TRIE__ 1

void trieInsert(const char*);
void dfs(int, char*, int);
void trieFind(struct Prompt*);
void trieInsertBuiltIn(const char*, int);
int trieGetBuiltIn(const char*);
int trieExist(const char*);
void saveDicToDisk();
void readDicFromDisk();
#endif