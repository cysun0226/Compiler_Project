#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <iomanip>
#include "symtab.h"
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include "node.h"
string RED_BOLD = "\x1b[;31;1m";
string RED_END = "\033[0m";
string GRN = "\x1b[;32;1m";
string RESET = "\x1b[0;m";
using namespace std;

int scope_id = 0;
int typeErr = 0;

std::vector<Symtab*> symtabStack;
std::vector<Symtab*> symtabList;
std::vector<string> errMsg;
std::string nf = "not func";
std::string PROG = "PROGRAM";
std::string PROG_PARA = "PROG_PARAMETER";
std::string INT = "INTEGER";
map<string, string>::iterator iter;
map<string, bool>::iterator initIter;
map<string, int>::iterator funcIter;


struct Symtab* newSymtab(string func_name, int scope_id) {
  struct Symtab *sym = new Symtab;
  sym->func_name = func_name;
  sym->scope = scope_id;
  return sym;
}

bool searchID(string id)
{
  for (int i = symtabStack.size()-1; i >= 0; i--) {
    iter = symtabStack[i]->symtab.find(id);
    if (iter != symtabStack[i]->symtab.end()) // id exists
      return true;
  }
  return false;
}

bool checkIdInit(string id)
{
  for (int i = symtabStack.size()-1; i >= 0; i--) {
    initIter = symtabStack[i]->symInit.find(id);
    if (initIter != symtabStack[i]->symInit.end()) // id exists
      return true;
  }
  return false;
}

bool searchDupID(string id)
{
  iter = symtabStack.back()->symtab.find(id);
  if (iter != symtabStack.back()->symtab.end()) // id exists
    return true;
  else
    return false;
}

string getIdType(string id)
{
  for (int i = symtabStack.size()-1; i >= 0; i--) {
    iter = symtabStack[i]->symtab.find(id);
    if (iter != symtabStack[i]->symtab.end()) // id exists
      return iter->second;
  }

  return "not found";
}

int getFuncData(string id)
{
  for (int i = symtabStack.size()-1; i >= 0; i--) {
    funcIter = symtabStack[i]->func_data.find(id);
    if (funcIter != symtabStack[i]->func_data.end()) // id exists
      return funcIter->second;
  }

  return 0;
}

void printErrMsg()
{
  cout << "\n----- Semantic check -----\n" << endl;

  if(errMsg.empty()) {
    cout << GRN + "[No Semantic Error]﻿" + RESET << endl << endl;
    return;
  }

  for (size_t i = 0; i < errMsg.size(); i++) {
    cout << errMsg[i] << endl;
  }
  cout << endl;
}

string redeclare(int line_no, string id)
{
  string err;
  err = RED_BOLD + "[error]" + RED_END + " line " + to_string(line_no) + ": redeclaration of " + "\'" + id + "\'";
  return err;
}

string undeclare(int line_no, string id)
{
  string err;
  err = RED_BOLD + "[error]" + RED_END + " line " + to_string(line_no) + ": " + "\'" + id + "\' was not declared in this scope";
  return err;
}

string intTypeError(int line_no, string id)
{
  string err;
  err = RED_BOLD + "[error]" + RED_END + " line " + to_string(line_no) + ": " + "\'" + id + "\' type error (assign floating number to integer)";
  return err;
}

string typeError(int line_no, string id, string id_type, string asg_type )
{
  string err;
  err = RED_BOLD + "[error]" + RED_END + " line " + to_string(line_no) + ": " + "\'" + id + "\' type error (assign " + asg_type + " to " + id_type + ")";
  return err;
}

string unInitialize(int line_no, string id)
{
  string err;
  err = RED_BOLD + "[error]" + RED_END + " line " + to_string(line_no) + ": variable " + "\'" + id + "\' used without having been initialized";
  // local variable 'tauxtrouve' used without having been initialized
  return err;
}

string wrongPara(int line_no, string id)
{
  string err;
  err = RED_BOLD + "[error]" + RED_END + " line " + to_string(line_no) + ": wrong parameters of function " + "\'" + id + "\'";
  return err;
}

void checkInt(Node* node)
{
  if (!node->childs.empty()) {
		for (size_t i = 0; i < node->childs.size(); i++) {
      checkInt(node->childs[i]);
		}
	}

  if(node->nodeType == NODE_NUM) {
    double f = floor(node->number);
    if(f != node->number)
      typeErr = 1;
  }
}




// double calculate(Node* node, double last_val)
// {
//   if (!node->childs.empty()) {
// 		for (size_t i = 0; i < node->childs.size(); i++) {
//       calculate(node->childs[i]);
// 		}
// 	}
//
//   if(node->nodeType == NODE_NUM) {
//     double f = floor(node->)
//   }
//
// }

void printSymtab(Symtab* s)
{
  // header
  if (s->scope == 0)
    cout << "----- global scope symbol table -----" << endl;
  else
    cout << "----- scope " << s->scope << " symbol table -----" << endl;

  if (s->func_name != nf) {
    cout << endl;
    cout << s->func_name << ": current symtab stack = ";
    for (size_t i = 0; i < symtabStack.size(); i++) {
      if(i == 0) cout << "global";
      else cout << symtabStack[i]->scope;
      if(i!=symtabStack.size()-1) cout << ", ";
    }
    cout << endl << endl;
  }


  cout << "----------------------------------------------------" << endl;
  cout << "| " << std::left << setw(15) << "id" << "|  " <<  std::left << setw(18) << "type" << "|  " <<  std::left << setw(10) <<  "scope"  << "|" << endl;
  cout << "----------------------------------------------------" << endl;

  // print content
  map<string, string>::iterator iter;
  for(iter = s->symtab.begin(); iter != s->symtab.end(); iter++) {
    if (s->scope != 0)
      cout << "| " << std::left << setw(15) << iter->first << "|  " << std::left << setw(18) << iter->second << "|  " << std::left << setw(10) << s->scope << "|" << endl;
    else
      cout << "| " << std::left << setw(15) << iter->first << "|  " << std::left << setw(18) << iter->second << "|  " << std::left << setw(10) << "global" << "|" << endl;
  }

  cout << "----------------------------------------------------" << endl << endl;
  // end
  // if (s->scope == 0)
  //   cout << "----- global scope symbol table end -----" << endl;
  // else
  //   cout << "----- scope " << s->scope << " symbol table end -----" << endl << endl;
}

bool divideScope(struct Node *node, int ident) {

    static char blank[1024];
    for(int i=0; i<ident; i++)
        blank[i] = ' ';
    blank[ident] = 0;


    // check parameters num
    // if(node->nodeType == NODE_ID && getIdType(node->strValue) == "PROCEDURE" && node->sibling.size()>2 && node->sibling[0]->nodeType != RE_PROC) {
    //   cout << "use para = " << node->sibling[2]->childs.size() << endl;
    //   cout << "get proc para = " << getFuncData(node->strValue) << endl;
    //   if(node->sibling[2]->childs.size() != getFuncData(node->strValue)) {
    //     errMsg.push_back(wrongPara(node->line_num, node->strValue));
    //   }
    // }
    //
    // if(node->nodeType == NODE_ID && getIdType(node->strValue) == "FUNCTION" && node->sibling.size()>2 && node->sibling[0]->nodeType != RE_FUNC) {
    //   cout << "use para = " << node->sibling[2]->childs.size() << endl;
    //   cout << "get func para = " << getFuncData(node->strValue) << endl;
    //   if(node->sibling[2]->childs.size() != getFuncData(node->strValue)) {
    //     errMsg.push_back(wrongPara(node->line_num, node->strValue));
    //   }
    // }

    // open scope
    if (node->nodeType == RE_PROG) {
      cout << "\n----- global scope -----\n" << endl;
      Symtab* newtab = newSymtab(nf, scope_id);
      newtab->symtab[node->sibling[1]->strValue + "(PROG)"] = PROG;
      symtabStack.push_back(newtab);
      symtabList.push_back(newtab);
      // scope_id++;

      // Symtab* newtab = newSymtab(nf, scope_id);
      // newtab->symtab[node->sibling[1]->strValue] = PROG;

      for (size_t i = 0; i < node->sibling[3]->childs.size(); i++) {
        string para_id = node->sibling[3]->childs[i]->strValue;
        if(!searchDupID(para_id)) {
          newtab->symtab[node->sibling[3]->childs[i]->strValue] = PROG_PARA;
          newtab->symInit[node->sibling[3]->childs[i]->strValue] = true;
        }

        else
          errMsg.push_back(redeclare(node->sibling[3]->childs[i]->line_num,para_id));
      }



    }

    if (node->nodeType == RE_FUNC) {
      scope_id++;
      node->scope_id = scope_id;
      node->parent->parent->childs.back()->childs[2]->scope_id = scope_id; // add scope_id to END

      if(!searchDupID(node->sibling[1]->strValue)) {
        symtabStack.back()->symtab[node->sibling[1]->strValue] = "FUNCTION"; // insert function name into scope
      }
      else {
        errMsg.push_back(redeclare(node->sibling[1]->line_num,node->sibling[1]->strValue));
      }



      Symtab* newtab = newSymtab("function \'" + node->sibling[1]->strValue + "\'", scope_id);
      symtabStack.push_back(newtab);
      symtabList.push_back(newtab);
      newtab->symtab[node->sibling[1]->strValue] = node->sibling[3]->childs[0]->strValue;

      if(!node->sibling[2]->childs[0]->childs.empty()) // if function have parameters
      {
        newtab->func_data[node->sibling[1]->strValue] = node->sibling[2]->childs[0]->childs.size();
        for (size_t i = 0; i < node->sibling[2]->childs[0]->childs[0]->childs.size(); i++) {
          string para_id = node->sibling[2]->childs[0]->childs[0]->childs[i]->strValue;
          string para_type = node->sibling[2]->childs[0]->childs[1]->strValue;
          if(node->sibling[2]->childs[0]->childs[1]->nodeType == RE_ARR) {
              para_type = "ARRAY(" + node->sibling[2]->childs[0]->childs[1]->childs[2]->strValue + ")" ;
          }

          int line_no = node->sibling[2]->childs[0]->childs[0]->childs[i]->line_num;
          if(!searchDupID(para_id)) {
            newtab->symtab[para_id] = para_type;
            newtab->symInit[para_id] = true;
          }
          else
            errMsg.push_back(redeclare(line_no,para_id));
        }
      }

      cout << "\n----- scope " << scope_id << " -----\n" << endl;
    }

    if (node->nodeType == RE_PROC) {
      scope_id++;
      node->scope_id = scope_id;
      node->parent->parent->childs.back()->childs[2]->scope_id = scope_id; // add scope_id to END
      if(!searchDupID(node->sibling[1]->strValue))
        symtabStack.back()->symtab[node->sibling[1]->strValue] = "PROCEDURE"; // insert function name into scope
      else
        errMsg.push_back(redeclare(node->sibling[1]->line_num, node->sibling[1]->strValue));

      Symtab* newtab = newSymtab("procedure \'" + node->sibling[1]->strValue + "\'", scope_id);
      // newtab->symtab[node->sibling[1]->strValue] = "PROCEDURE";
      symtabStack.push_back(newtab);
      symtabList.push_back(newtab);

      if(!node->sibling[2]->childs[0]->childs.empty()) // if have parameters
      {
        symtabStack[0]->func_data[node->sibling[1]->strValue] = node->sibling[2]->childs[0]->childs[0]->childs.size() + node->sibling[2]->childs[0]->childs[2]->childs[0]->childs.size();
        for (size_t i = 0; i < node->sibling[2]->childs[0]->childs[0]->childs.size(); i++) {
          string para_id = node->sibling[2]->childs[0]->childs[0]->childs[i]->strValue;
          if(!searchDupID(para_id)) {
            newtab->symtab[para_id] = node->sibling[2]->childs[0]->childs[1]->strValue;
            if(node->sibling[2]->childs[0]->childs[1]->nodeType == RE_ARR) {
              newtab->symtab[para_id] = "ARRAY(" + node->sibling[2]->childs[0]->childs[1]->childs[2]->strValue + ")";
            }
            newtab->symInit[para_id] = true;
          }
        }
      }



      cout << "\n----- scope " << scope_id << " -----\n" << endl;
    }


    // insert decl symbol
    if (node->nodeType == NODE_DECL && node->childs.size()>2) {
      for (size_t i = 0; i < node->childs[1]->childs.size(); i++) { // decl id list
        string var_id = node->childs[1]->childs[i]->strValue;
        string var_type = node->childs[2]->strValue;

        if(!searchDupID(var_id)) {
          symtabStack.back()->symtab[var_id] = var_type;
          if(node->childs[2]->nodeType == RE_ARR) {
            Array new_arr;
            new_arr.d_num = 0;
            new_arr.start_idx = node->childs[0]->number;
            new_arr.end_idx = node->childs[1]->number;

            // multi-dimension array
            Node* arrNode = node->childs[2];
            while (arrNode->childs[2]->nodeType == RE_ARR) {
              new_arr.d_num++;
              arrNode = arrNode->childs[2];
              Array new_d;
              new_d.start_idx = arrNode->childs[0]->number;
              new_d.end_idx = arrNode->childs[1]->number;
              new_arr.dimension.push_back(new_d);
            }

            std::string arrType = arrNode->childs[2]->strValue;
            new_arr.type = arrType;
            symtabStack.back()->arrTab[node->childs[1]->childs[i]->strValue] = new_arr;

            if (node->childs[2]->childs[2]->nodeType == RE_ARR)
              symtabStack.back()->symtab[node->childs[1]->childs[i]->strValue] = to_string(new_arr.d_num+1) + "d-ARRAY(" + arrType + ")";
            else
              symtabStack.back()->symtab[node->childs[1]->childs[i]->strValue] = "ARRAY(" + arrType + ")";
          }
        }
        else
          errMsg.push_back(redeclare(node->childs[1]->childs[i]->line_num,var_id));
      }
    }

    // Semantic check
    // undeclare
    if (node->nodeType == NODE_ID) {
      bool notDECL = false;
      // check not declare id
      if (node->parent->nodeType == NODE_VAR) notDECL = true;
      if (node->parent->nodeType == NODE_TERM) notDECL = true;
      if (node->parent->nodeType == NODE_SI_EXPR) notDECL = true;
      if (node->parent->nodeType == NODE_PROC_STMT) notDECL = true;
      if (node->parent->nodeType == NODE_FACTOR) notDECL = true;

      if(notDECL)
      {
        string id = node->strValue;
        int line_no = node->line_num;

        // check is declared
        if(!searchID(id))
          errMsg.push_back(undeclare(line_no, id));

        // check is intitialize
        if(searchID(id)) {
          if(node->parent->nodeType != NODE_VAR) { // right value
            if(!checkIdInit(id) && getIdType(id)!="PROCEDURE" && getIdType(id)!="FUNCTION")
              errMsg.push_back(unInitialize(line_no, id));
          }
        }
      }

    }

    // type check
    if (node->nodeType == RE_ASGMNT)
    {
      typeErr = 0;
      Node* leftNode = node->sibling[0]->childs[0];
      string var_id = leftNode->strValue;


      if(getIdType(leftNode->strValue) == "INTEGER") {
        checkInt(node->sibling[2]);
        if(typeErr)
          errMsg.push_back(intTypeError(leftNode->line_num, leftNode->strValue));
      }

      if(node->sibling[2]->nodeType == NODE_ID) {
        string id_type = getIdType(leftNode->strValue);
        string asg_type = getIdType(node->sibling[2]->strValue);
        if(asg_type!="FUNCTION" && id_type != "REAL" && asg_type!="PROCEDURE" && id_type != asg_type) {
          errMsg.push_back(typeError(leftNode->line_num, leftNode->strValue, id_type, asg_type));
          typeErr = 1;
        }
      }

      if(!typeErr) {
        symtabStack.back()->symInit[var_id] = true;
      }

    }





    // print nodes
    switch(node->nodeType) {
        case OP_ADD           : printf("%s|-ADD\n", blank); break;
        case OP_SUB           : printf("%s|-SUB\n", blank); break;
        case NODE_INT         : printf("%s|-%d\n", blank, node->iValue); ident += 3; break;
        case NODE_STMT        : printf("%s|-STMT\n", blank); ident += 3; break;
        case NODE_TERM        : printf("%s|-TERM\n", blank); ident += 3; break;
        case NODE_PROGRAM     : printf("%s|-PROGRAM \n", blank); ident += 3; break;
    		case NODE_ID          : printf("%s|-[ID] ", blank); cout << node->strValue << " (line " << node->line_num << ")" << endl; ident += 3; break;
        case ID_TOK           : printf("%s|-ID_TOK\n", blank); ident += 3; break;
    		case NODE_DECL        : printf("%s|-DECL\n", blank); ident += 3; break;
        case NODE_EXPR        : printf("%s|-EXPR\n", blank); ident += 3; break;
        case NODE_VAR         : printf("%s|-VAR(NODE)\n", blank); ident += 3; break;
        case NODE_ID_LT       : printf("%s|-ID_LIST\n", blank); ident += 3; break;
        case PUC_COMMA        : printf("%s|- ,\n", blank); ident += 3; break;
        case PUC_LBRAC        : printf("%s|- [ \n", blank); ident += 3; break;
        case PUC_RBRAC        : printf("%s|- ]\n", blank); ident += 3; break;
        case PUC_DOTDOT       : printf("%s|- ..\n", blank); ident += 3; break;
        case PUC_COLON        : printf("%s|- :\n", blank); ident += 3; break;
        case PUC_SEMI         : printf("%s|- ;\n", blank); ident += 3; break;
        case TY_INT           : printf("%s|-INTEGER(type)\n", blank);ident += 3;break;
        case NODE_STDTYPE     : printf("%s|-STDTYPE\n", blank);ident += 3;break;
        case NODE_TYPE        : printf("%s|-TYPE\n", blank);ident += 3;break;
        case RE_ARR           : printf("%s|-ARRAY\n", blank);ident += 3;break;
        case RE_OF            : printf("%s|-OF\n", blank);ident += 3;break;
        case NODE_NUM         : printf("%s|-[NUM] ", blank); cout << node->number << endl; ident += 3; break;
        case NUM_TOK          : printf("%s|-NUM_TOK\n", blank); ident += 3;break;
        case NODE_LAMDBA      : printf("%s|- (LAMDBA)\n", blank); ident += 3;break;
        case NODE_STRING      : printf("%s|-STRING\n", blank);ident += 3; break;
        case NODE_IF_STMT     : printf("%s|-IF_STMT\n", blank);ident += 3; break;
        case NODE_OPTVAR      : printf("%s|-OPTVAR\n", blank);ident += 3; break;
        case NODE_RELOP       : printf("%s|-RELOP\n", blank); ident += 3;break;
        case NODE_MULOP       : printf("%s|-MULOP\n", blank); ident += 3;break;
        case NODE_ADDOP       : printf("%s|-ADDOP\n", blank); ident += 3;break;
        case NODE_FACTOR      : printf("%s|-FACTOR\n", blank);ident += 3; break;
        case NODE_TREM        : printf("%s|-TERM\n", blank); ident += 3; break;
        case NODE_SI_EXPR     : printf("%s|-SI_EXPR\n", blank); ident += 3; break;
        case NODE_EXPR_LI     : printf("%s|-EXPR_LI\n", blank); ident += 3; break;
        case NODE_PROC_STMT   : printf("%s|-PROC_STMT\n", blank); ident += 3; break;
        case NODE_TAIL        : printf("%s|-TAIL\n", blank); ident += 3; break;
        case NODE_COMP_STMT   : printf("%s|-COMP_STMT\n", blank); ident += 3; break;
        case NODE_STMT_LI     : printf("%s|-STMT_LI\n", blank); ident += 3;break;
        case NODE_OPT_STMT    : printf("%s|-OPT_STMT\n", blank);ident += 3; break;
        case NODE_PARAM_LI    : printf("%s|-PARAM_LI\n", blank);ident += 3; break;
        case NODE_ARG         : printf("%s|-ARG\n", blank); ident += 3;break;
        case NODE_SPROG_H     : printf("%s|-SPROG\n", blank);ident += 3; break;
        case NODE_SPROG_DECL  : printf("%s|-SPROG_DECL\n", blank);ident += 3; break;
        case NODE_SPROG_DECLS : printf("%s|-SPROG_DECLS\n", blank); ident += 3;break;
        case NODE_START       : printf("%s|-START\n", blank); ident += 3;break;
        case OP_notEQUAL      : printf("%s|-NOTEQUAL\n", blank);ident += 3; break;
        case OP_GE            : printf("%s|- >=\n", blank);ident += 3; break;
        case OP_LE            : printf("%s|- <=\n", blank);ident += 3; break;
        case OP_EQUAL         : printf("%s|- =\n", blank); ident += 3;break;
        case OP_GT            : printf("%s|- >\n", blank); ident += 3;break;
        case OP_LT            : printf("%s|- <\n", blank); ident += 3;break;
        case OP_MUL           : printf("%s|-MUL\n", blank);ident += 3; break;
        case OP_DIV           : printf("%s|-DIV\n", blank);ident += 3; break;
        case OP_PLUS          : printf("%s|-ADD\n", blank);ident += 3; break;
        case OP_MINUS         : printf("%s|-MINUS\n", blank);ident += 3; break;
        case PUC_LPAREN       : printf("%s|- ( \n", blank);ident += 3; break;
        case PUC_RPAREN       : printf("%s|- ) \n", blank);ident += 3; break;
        case PUC_DOT          : printf("%s|- . \n", blank);ident += 3; break;
        case RE_NOT           : printf("%s|-NOT\n", blank);ident += 3; break;
        case RE_VAR           : printf("%s|-VAR\n", blank);ident += 3; break;
        case RE_ASGMNT        : printf("%s|-ASSIGN \n", blank);ident += 3; break;
        case RE_IF            : printf("%s|-IF\n", blank);ident += 3; break;
        case RE_THEN          : printf("%s|-THEN\n", blank); ident += 3;break;
        case RE_ELSE          : printf("%s|-ELSE\n", blank); ident += 3;break;
        case RE_WHILE         : printf("%s|-WHILE\n", blank);ident += 3; break;
        case RE_DO            : printf("%s|-DO\n", blank);ident += 3; break;
        case RE_BEGIN         : printf("%s|-BEGIN\n", blank);ident += 3; break;
        case RE_END           : printf("%s|-END\n", blank);ident += 3; break;
        case RE_FUNC          : printf("%s|-FUNCTION\n", blank);ident += 3; break;
        case RE_PROC          : printf("%s|-PROCEDURE\n", blank); ident += 3;break;
        case RE_PROG          : printf("%s|-PROGRAM\n", blank); ident += 3;break;
        case RE_CONST         : printf("%s|-CONST\n", blank); ident += 3;break;
        case RE_TYPE          : printf("%s|-TYPE\n", blank); ident += 3;break;
        case TY_REAL          : printf("%s|-REAL\n", blank);ident += 3; break;
        case TY_STR           : printf("%s|-STRING\n", blank);ident += 3; break;
        case NODE_DECLS       : printf("%s|-DECLS\n", blank);ident += 3; break;
        case NODE_SPROG_DECLS_LI : printf("%s|-SPROG_DECLS_LI\n", blank);ident += 3; break;


        default:
            printf("%sdefault:%d\n", blank, node->nodeType);
          break;
    }

    if (node->nodeType == PUC_DOT && node->parent->childs[0]->nodeType == RE_PROG) {
      cout << "\n----- global scope end -----\n" << endl;
      printSymtab(symtabStack.back());
      symtabStack.pop_back();
      printErrMsg();
    }


    if (node->nodeType == RE_END && node->parent->parent->nodeType != NODE_START && symtabStack.size()>1 ) {
      cout << "\n----- scope " << node->scope_id << " end -----\n" << endl;
      printSymtab(symtabStack.back());
      symtabStack.pop_back();
    }

    node->scope_id = scope_id;


	if (!node->childs.empty())
	{
		for (size_t i = 0; i < node->childs.size(); i++)
		{
      node->childs[i]->parent = node;
      for (int j = 0; j < node->childs.size(); j++) {
        node->childs[i]->sibling.push_back(node->childs[j]);
      }
      divideScope(node->childs[i], ident);
		}
	}

  return errMsg.empty();
}

std::vector<Symtab*> getSymtab(){
  return symtabList;
}
