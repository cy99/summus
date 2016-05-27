#pragma once

#ifndef SMM_LEXER_H
#define SMM_LEXER_H

#include <stdint.h>
#include <stdbool.h>

#include "smmmsgs.h"
#include "smmutil.h"

// Disable warning that we are using anonimous unions in structs
#pragma warning(disable : 4201)

/********************************************************
Type Definitions
*********************************************************/

typedef enum { smmLexTypeFile, smmLexTypeStdIn } SmmLexTypeEnum;

typedef enum {
	tkSmmErr,
	tkSmmIdent = 256, // Because first 255 values are reserved for existing chars
	tkSmmIntDiv, tkSmmIntMod, tkSmmAndOp, tkSmmOrOp, tkSmmXorOp,
	tkSmmUInt32, tkSmmUInt64, tkSmmFloat64, tkSmmBool,
	tkSmmEof
} SmmTokenKind;

struct SmmLexer {
	char* fileName; // Used only for error messages
	char* buffer;
	char* curChar;
	uint64_t scanCount;

	struct SmmFilePos filePos;
};
typedef struct SmmLexer* PSmmLexer;

struct SmmSymbol {
	char* name;
	int kind;
};
typedef struct SmmSymbol* PSmmSymbol;

struct SmmToken {
	int kind;
	char* repr;
	uint64_t pos;
	bool isFirstOnLine;
	struct SmmFilePos filePos;
	union {
		uint64_t intVal;
		double floatVal;
		bool boolVal;
		uint32_t hash;
	};
};
typedef struct SmmToken* PSmmToken;

/**
Returns a new instance of SmmLexer that will scan the given buffer or stdin
if given buffer is null. When scanning stdin end of file is signaled using
"Enter, CTRL+Z, Enter" on Windows and CTRL+D on *nix systems
*/
PSmmLexer smmCreateLexer(char* buffer, char* filename, PSmmAllocator allocator);

PSmmToken smmGetNextToken(PSmmLexer lex);

char* smmTokenToString(PSmmToken token, char* buf);

#endif