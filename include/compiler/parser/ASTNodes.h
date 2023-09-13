// #include <stdlib.h>

// enum ASTNodeType {
// 	NODE_INVALID = 0,
// 	NODE_PROGRAM,
// 	NODE_BLOCK,
// 	NODE_VAR_DECL,
// 	NODE_TYPE
// };


// typedef struct AST {
// 	ASTNode *root;
// 	char *source;
// } AST;

// typedef struct ASTNode {
// 	int type;
// } ASTNode;

// typedef struct ASTNodeProgram {
// 	int type;
// 	ASTNode *block;
// } ASTNodeProgram;

// typedef struct ASTNodeBlock {
// 	int type;
// 	ASTNode **statements;
// } ASTNodeBlock;

// typedef struct ASTNodeVarDecl {
// 	int type;
// 	ASTNode *type;
// 	char *name;
// } ASTNodeVarDecl;

// void myFunc(ASTNode *node) {
// 	switch(node->type) {
// 		case NODE_PROGRAM: {
// 			ASTNodeProgram *program = (ASTNodeProgram*)node;
// 			myFunc(program->block);
// 		} break;

// 		case NODE_BLOCK: {
// 			ASTNodeBlock *block = (ASTNodeBlock*)node;
// 			for(int i = 0; i < block->statementsCount; i++) {
// 				myFunc(block->statements[i]);
// 			}
// 		} break;

// 		case NODE_VAR_DECL: {
// 			ASTNodeVarDecl *varDecl = (ASTNodeVarDecl*)node;
// 			myFunc(varDecl->type);
// 		} break;
// 	}
// }


// int main() {
// 	Extended1 extended1;
// 	extended1.type = TYPE_1;
// 	extended1.myNumber = 10;
// 	myFunc((Base*)&extended1);

// 	Extended2 extended2;
// 	extended2.type = TYPE_2;
// 	strcpy(extended2.myString, "Hello my string!");
// 	myFunc((Base*)&extended2);

// 	return 0;
// }