#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include <vector>
#include <string>
#include <map>
#include "node.h"
using namespace std;

typedef struct Symtab {
	map<string, string> symtab; // ID, type
	string func_name;
	int scope;
} Symtab;

struct Symtab* newSymtab(string func_name, int scope_id);
void divideScope(Node* node);	
// void addChild(struct Node *node, struct Node *child);
// void printTree(struct Node *node, int ident);
// Node* buildAstTree(struct Node *node);

#endif