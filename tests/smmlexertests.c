#include "../compiler/smmcommon.h"
#include "CuTest.h"
#include "../compiler/smmlexer.h"
#include "smmmsgsMockup.h"

static PSmmAllocator allocator;
static SmmMsgType lastGotMsg;

static void lexOnPostMessageCalled(SmmMsgType msgType) {
	lastGotMsg = msgType;
}

static void TestParseIdent(CuTest *tc) {
	char buf[] = "whatever and something or whatever again";
	PSmmLexer lex = smmCreateLexer(buf, "TestParseIdent", allocator);
	PSmmToken token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmIdent, token->kind);
	CuAssertStrEquals(tc, "whatever", token->repr);
	const char* whatever = token->repr;
	
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmAndOp, token->kind);
	CuAssertStrEquals(tc, "and", token->repr);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmIdent, token->kind);
	CuAssertStrEquals(tc, "something", token->repr);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmOrOp, token->kind);
	CuAssertStrEquals(tc, "or", token->repr);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmIdent, token->kind);
	CuAssertPtrEquals(tc, whatever, token->repr);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmIdent, token->kind);
	CuAssertStrEquals(tc, "again", token->repr);
}

static void TestParseHexNumber(CuTest *tc) {
	char buf[] = "0x0 0x1234abcd 0x567890ef 0xffffffff 0x100000000 0xFFFFFFFFFFFFFFFF "
		"0x10000000000000000 0xxrg 0x123asd 0x123.324 ";
	PSmmLexer lex = smmCreateLexer(buf, "TestParseHexNumber", allocator);
	PSmmToken token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 0, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 0x1234abcd, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 0x567890ef, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 0xffffffff, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertTrue(tc, 0x100000000 == token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertTrue(tc, 0xFFFFFFFFFFFFFFFF == token->uintVal);

	onPostMessageCalled = lexOnPostMessageCalled;
	// 0x10000000000000000
	lastGotMsg = 0;
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmErr, token->kind);
	CuAssertIntEquals_Msg(tc, "Expected message not received", errSmmIntTooBig, lastGotMsg);

	// 0xxrg
	lastGotMsg = 0;
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmErr, token->kind);
	CuAssertIntEquals_Msg(tc, "Expected message not received", errSmmInvalidHexDigit, lastGotMsg);

	// 0x123asd
	lastGotMsg = 0;
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmErr, token->kind);
	CuAssertIntEquals_Msg(tc, "Expected message not received", errSmmInvalidHexDigit, lastGotMsg);

	// 0x123.324
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 0x123, token->uintVal);
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, '.', token->kind);

	onPostMessageCalled = NULL;
}

static void TestParseNumber(CuTest *tc) {
	char buf[] = "0 1 1234567890 4294967295 4294967296 18446744073709551615 18446744073709551616 02342 43abc "
		"123.321 4.2 456E2 789E-2  901.234E+123 56789.01235E-456 234.3434E-234.34 37.b "
		"1111111111111111111111111111111.456 1.12345678901234567890";
	PSmmLexer lex = smmCreateLexer(buf, "TestParseNumber", allocator);
	PSmmToken token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 0, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 1, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 1234567890, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 4294967295, token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertTrue(tc, 4294967296 == token->uintVal);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertTrue(tc, 18446744073709551615U == token->uintVal);

	onPostMessageCalled = lexOnPostMessageCalled;
	// 18446744073709551616 MAX_UINT64 + 1
	lastGotMsg = 0;
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmErr, token->kind);
	CuAssertIntEquals_Msg(tc, "Expected message not received", errSmmIntTooBig, lastGotMsg);

	// 02342
	lastGotMsg = 0;
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmErr, token->kind);
	CuAssertIntEquals_Msg(tc, "Expected message not received", errSmmInvalid0Number, lastGotMsg);

	// 43abc
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertUIntEquals(tc, 43, token->uintVal);
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmIdent, token->kind);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 123.321, token->floatVal, 0);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 4.2, token->floatVal, 0);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 456E2, token->floatVal, 0);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 789E-2, token->floatVal, 0);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 901.234E+123, token->floatVal, 0);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
#pragma GCC diagnostic push // Clang understands both GCC and clang pragmas
#pragma GCC diagnostic ignored "-Woverflow"
#pragma clang diagnostic ignored "-Wliteral-range"
	CuAssertDblEquals(tc, 56789.01235E-456, token->floatVal, 0);
#pragma GCC diagnostic pop

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 234.3434E-234, token->floatVal, 0);

	// . (dot)
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, '.', token->kind);

	// 34
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);

	// 37.b
	lastGotMsg = 0;
	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmUInt, token->kind);
	CuAssertIntEquals_Msg(tc, "Expected message not received", errSmmInvalidNumber, lastGotMsg);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 1111111111111111111111111111111.456, token->floatVal, 0);

	token = smmGetNextToken(lex);
	CuAssertIntEquals(tc, tkSmmFloat, token->kind);
	CuAssertDblEquals(tc, 1.12345678901234567890, token->floatVal, 0);

	onPostMessageCalled = NULL;
}

static void TestTokenToString(CuTest *tc) {
	struct SmmToken token = { 0, 0, 0, "repr" };
	char buf[4] = { 0 };
	const char* res = smmTokenToString(&token, buf);
	CuAssertStrEquals(tc, "repr", res);
	CuAssertStrEquals(tc, "", buf);

	token.kind = tkSmmIdent;
	res = smmTokenToString(&token, buf);
	CuAssertStrEquals(tc, "identifier", res);
	CuAssertStrEquals(tc, "", buf);

	token.kind = '+';
	res = smmTokenToString(&token, buf);
	CuAssertStrEquals(tc, "'+'", res);
	CuAssertPtrEquals(tc, buf, res);
}

CuSuite* SmmLexerGetSuite() {
	allocator = smmCreatePermanentAllocator("lexerTest", 64 * 1024 * 1024);
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestParseIdent);
	SUITE_ADD_TEST(suite, TestParseHexNumber);
	SUITE_ADD_TEST(suite, TestParseNumber);
	SUITE_ADD_TEST(suite, TestTokenToString);

	return suite;
}
