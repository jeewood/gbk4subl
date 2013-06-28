

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"




void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		wprintf(L"%s\n",la->val);
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::JScr() {
		while (StartOf(1)) {
			Statement();
		}
}

void Parser::Statement() {
		switch (la->kind) {
		case _identifier: case _number: case _string: case _char: case 9 /* "(" */: case 11 /* "::" */: case 33 /* "&" */: case 42 /* "+" */: case 43 /* "-" */: case 44 /* "*" */: case 47 /* "++" */: case 48 /* "--" */: case 49 /* "[" */: case 63 /* "!" */: case 64 /* "~" */: {
			AssignmentExpression();
			break;
		}
		case 20 /* "break" */: {
			BreakStatement();
			break;
		}
		case 21 /* "continue" */: {
			ContinueStatement();
			break;
		}
		case 22 /* "do" */: {
			DoStatement();
			break;
		}
		case 24 /* "for" */: {
			ForStatement();
			break;
		}
		case 25 /* "if" */: {
			IfStatement();
			break;
		}
		case 6 /* ";" */: {
			Get();
			break;
		}
		case 27 /* "return" */: {
			ReturnStatement();
			break;
		}
		case 28 /* "switch" */: {
			SwitchStatement();
			break;
		}
		case 23 /* "while" */: {
			WhileStatement();
			break;
		}
		case 14 /* "{" */: {
			BlockStatement();
			break;
		}
		case 7 /* "def" */: {
			DefineStatement();
			break;
		}
		default: SynErr(66); break;
		}
}

void Parser::AssignmentExpression() {
		Expression();
		Expect(6 /* ";" */);
}

void Parser::BreakStatement() {
		Expect(20 /* "break" */);
		Expect(6 /* ";" */);
}

void Parser::ContinueStatement() {
		Expect(21 /* "continue" */);
		Expect(6 /* ";" */);
}

void Parser::DoStatement() {
		Expect(22 /* "do" */);
		Statement();
		Expect(23 /* "while" */);
		Expect(9 /* "(" */);
		Expression();
		Expect(10 /* ")" */);
		Expect(6 /* ";" */);
}

void Parser::ForStatement() {
		Expect(24 /* "for" */);
		Expect(9 /* "(" */);
		if (StartOf(2)) {
			Expression();
		}
		Expect(6 /* ";" */);
		if (StartOf(2)) {
			Expression();
		}
		Expect(6 /* ";" */);
		if (StartOf(2)) {
			Expression();
		}
		Expect(10 /* ")" */);
		Statement();
}

void Parser::IfStatement() {
		Expect(25 /* "if" */);
		Expect(9 /* "(" */);
		Expression();
		Expect(10 /* ")" */);
		Statement();
		if (la->kind == 26 /* "else" */) {
			Get();
			Statement();
		}
}

void Parser::ReturnStatement() {
		Expect(27 /* "return" */);
		if (StartOf(3)) {
			if (StartOf(2)) {
				ActualParameters();
			}
			Expect(6 /* ";" */);
		} else if (la->kind == 7 /* "def" */) {
			DefineStatement();
		} else SynErr(67);
}

void Parser::SwitchStatement() {
		Expect(28 /* "switch" */);
		Expect(9 /* "(" */);
		Expression();
		Expect(10 /* ")" */);
		Expect(14 /* "{" */);
		while (la->kind == 16 /* "case" */ || la->kind == 19 /* "default" */) {
			Label();
			if (StartOf(1)) {
				Statement();
			}
		}
		Expect(15 /* "}" */);
}

void Parser::WhileStatement() {
		Expect(23 /* "while" */);
		Expect(9 /* "(" */);
		Expression();
		Expect(10 /* ")" */);
		Statement();
}

void Parser::BlockStatement() {
		Expect(14 /* "{" */);
		while (StartOf(1)) {
			Statement();
		}
		Expect(15 /* "}" */);
}

void Parser::DefineStatement() {
		Expect(7 /* "def" */);
		if (la->kind == _identifier) {
			Get();
			while (la->kind == 8 /* "." */) {
				Get();
				Expect(_identifier);
			}
		}
		Expect(9 /* "(" */);
		if (la->kind == _identifier || la->kind == 13 /* "..." */) {
			ParamExpression();
		}
		Expect(10 /* ")" */);
		BlockStatement();
}

void Parser::ParamExpression() {
		if (la->kind == _identifier) {
			Get();
			while (la->kind == 12 /* "," */) {
				Get();
				if (la->kind == _identifier) {
					Get();
				} else if (la->kind == 13 /* "..." */) {
					Get();
				} else SynErr(68);
			}
		} else if (la->kind == 13 /* "..." */) {
			Get();
		} else SynErr(69);
}

void Parser::NameList() {
		if (la->kind == 11 /* "::" */) {
			Get();
		}
		Expect(_identifier);
		while (la->kind == 12 /* "," */) {
			Get();
			if (la->kind == 11 /* "::" */) {
				Get();
			}
			Expect(_identifier);
		}
}

void Parser::Label() {
		if (la->kind == 16 /* "case" */) {
			Get();
			Expression();
			if (la->kind == 12 /* "," */ || la->kind == 18 /* ":" */) {
				while (la->kind == 12 /* "," */) {
					Get();
					Expression();
				}
			} else if (la->kind == 17 /* ".." */) {
				Get();
				Expression();
			} else SynErr(70);
			Expect(18 /* ":" */);
		} else if (la->kind == 19 /* "default" */) {
			Get();
			Expect(18 /* ":" */);
		} else SynErr(71);
}

void Parser::Expression() {
		LogORExp();
		while (StartOf(4)) {
			AssignmentOperator();
			ActualParameters();
		}
}

void Parser::ActualParameters() {
		Expression();
		while (la->kind == 12 /* "," */) {
			Get();
			Expression();
		}
}

void Parser::LogORExp() {
		LogANDExp();
		while (la->kind == 29 /* "||" */) {
			Get();
			LogANDExp();
		}
}

void Parser::AssignmentOperator() {
		switch (la->kind) {
		case 52 /* "=" */: {
			Get();
			break;
		}
		case 53 /* "*=" */: {
			Get();
			break;
		}
		case 54 /* "/=" */: {
			Get();
			break;
		}
		case 55 /* "%=" */: {
			Get();
			break;
		}
		case 56 /* "+=" */: {
			Get();
			break;
		}
		case 57 /* "-=" */: {
			Get();
			break;
		}
		case 58 /* "&=" */: {
			Get();
			break;
		}
		case 59 /* "^=" */: {
			Get();
			break;
		}
		case 60 /* "|=" */: {
			Get();
			break;
		}
		case 61 /* "<<=" */: {
			Get();
			break;
		}
		case 62 /* ">>=" */: {
			Get();
			break;
		}
		default: SynErr(72); break;
		}
}

void Parser::LogANDExp() {
		InclORExp();
		while (la->kind == 30 /* "&&" */) {
			Get();
			InclORExp();
		}
}

void Parser::InclORExp() {
		ExclORExp();
		while (la->kind == 31 /* "|" */) {
			Get();
			ExclORExp();
		}
}

void Parser::ExclORExp() {
		ANDExp();
		while (la->kind == 32 /* "^" */) {
			Get();
			ANDExp();
		}
}

void Parser::ANDExp() {
		EqualExp();
		while (la->kind == 33 /* "&" */) {
			Get();
			EqualExp();
		}
}

void Parser::EqualExp() {
		RelationExp();
		while (la->kind == 34 /* "==" */ || la->kind == 35 /* "!=" */) {
			if (la->kind == 34 /* "==" */) {
				Get();
			} else {
				Get();
			}
			RelationExp();
		}
}

void Parser::RelationExp() {
		ShiftExp();
		while (StartOf(5)) {
			if (la->kind == 36 /* "<" */) {
				Get();
			} else if (la->kind == 37 /* ">" */) {
				Get();
			} else if (la->kind == 38 /* "<=" */) {
				Get();
			} else {
				Get();
			}
			ShiftExp();
		}
}

void Parser::ShiftExp() {
		AddExp();
		while (la->kind == 40 /* "<<" */ || la->kind == 41 /* ">>" */) {
			if (la->kind == 40 /* "<<" */) {
				Get();
			} else {
				Get();
			}
			AddExp();
		}
}

void Parser::AddExp() {
		MultExp();
		while (la->kind == 42 /* "+" */ || la->kind == 43 /* "-" */) {
			if (la->kind == 42 /* "+" */) {
				Get();
			} else {
				Get();
			}
			MultExp();
		}
}

void Parser::MultExp() {
		UnaryExp();
		while (la->kind == 44 /* "*" */ || la->kind == 45 /* "/" */ || la->kind == 46 /* "%" */) {
			if (la->kind == 44 /* "*" */) {
				Get();
			} else if (la->kind == 45 /* "/" */) {
				Get();
			} else {
				Get();
			}
			UnaryExp();
		}
}

void Parser::UnaryExp() {
		if (StartOf(6)) {
			PostFixExp();
		} else if (StartOf(7)) {
			if (la->kind == 47 /* "++" */) {
				Get();
			} else if (la->kind == 48 /* "--" */) {
				Get();
			} else {
				UnaryOperator();
			}
			UnaryExp();
		} else SynErr(73);
}

void Parser::PostFixExp() {
		Primary();
		while (StartOf(8)) {
			switch (la->kind) {
			case 49 /* "[" */: {
				Get();
				if (StartOf(2)) {
					ActualParameters();
				}
				Expect(50 /* "]" */);
				break;
			}
			case 9 /* "(" */: {
				FunctionCall();
				break;
			}
			case 8 /* "." */: {
				Get();
				Expect(_identifier);
				break;
			}
			case 51 /* "->" */: {
				Get();
				Expect(_identifier);
				break;
			}
			case 47 /* "++" */: {
				Get();
				break;
			}
			case 48 /* "--" */: {
				Get();
				break;
			}
			}
		}
}

void Parser::UnaryOperator() {
		switch (la->kind) {
		case 42 /* "+" */: {
			Get();
			break;
		}
		case 43 /* "-" */: {
			Get();
			break;
		}
		case 44 /* "*" */: {
			Get();
			break;
		}
		case 63 /* "!" */: {
			Get();
			break;
		}
		case 33 /* "&" */: {
			Get();
			break;
		}
		case 64 /* "~" */: {
			Get();
			break;
		}
		default: SynErr(74); break;
		}
}

void Parser::Primary() {
		switch (la->kind) {
		case _identifier: case 11 /* "::" */: {
			NameList();
			break;
		}
		case _string: {
			Get();
			break;
		}
		case _char: {
			Get();
			break;
		}
		case _number: {
			Get();
			break;
		}
		case 9 /* "(" */: {
			Get();
			Expression();
			Expect(10 /* ")" */);
			break;
		}
		case 49 /* "[" */: {
			Get();
			if (StartOf(2)) {
				ActualParameters();
			}
			Expect(50 /* "]" */);
			break;
		}
		default: SynErr(75); break;
		}
}

void Parser::FunctionCall() {
		Expect(9 /* "(" */);
		if (StartOf(2)) {
			ActualParameters();
		}
		Expect(10 /* ")" */);
}



void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	JScr();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 65;

	//ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[9][67] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,x, T,T,T,T, x,T,x,T, x,x,T,x, x,x,x,x, T,T,T,T, T,T,x,T, T,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x},
		{x,T,T,x, T,T,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x},
		{x,T,T,x, T,T,T,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, T,T,T,T, T,T,T,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,x, T,T,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x},
		{x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	//ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"identifier expected"); break;
			case 2: s = coco_string_create(L"number expected"); break;
			case 3: s = coco_string_create(L"HexNumber expected"); break;
			case 4: s = coco_string_create(L"string expected"); break;
			case 5: s = coco_string_create(L"char expected"); break;
			case 6: s = coco_string_create(L"\";\" expected"); break;
			case 7: s = coco_string_create(L"\"def\" expected"); break;
			case 8: s = coco_string_create(L"\".\" expected"); break;
			case 9: s = coco_string_create(L"\"(\" expected"); break;
			case 10: s = coco_string_create(L"\")\" expected"); break;
			case 11: s = coco_string_create(L"\"::\" expected"); break;
			case 12: s = coco_string_create(L"\",\" expected"); break;
			case 13: s = coco_string_create(L"\"...\" expected"); break;
			case 14: s = coco_string_create(L"\"{\" expected"); break;
			case 15: s = coco_string_create(L"\"}\" expected"); break;
			case 16: s = coco_string_create(L"\"case\" expected"); break;
			case 17: s = coco_string_create(L"\"..\" expected"); break;
			case 18: s = coco_string_create(L"\":\" expected"); break;
			case 19: s = coco_string_create(L"\"default\" expected"); break;
			case 20: s = coco_string_create(L"\"break\" expected"); break;
			case 21: s = coco_string_create(L"\"continue\" expected"); break;
			case 22: s = coco_string_create(L"\"do\" expected"); break;
			case 23: s = coco_string_create(L"\"while\" expected"); break;
			case 24: s = coco_string_create(L"\"for\" expected"); break;
			case 25: s = coco_string_create(L"\"if\" expected"); break;
			case 26: s = coco_string_create(L"\"else\" expected"); break;
			case 27: s = coco_string_create(L"\"return\" expected"); break;
			case 28: s = coco_string_create(L"\"switch\" expected"); break;
			case 29: s = coco_string_create(L"\"||\" expected"); break;
			case 30: s = coco_string_create(L"\"&&\" expected"); break;
			case 31: s = coco_string_create(L"\"|\" expected"); break;
			case 32: s = coco_string_create(L"\"^\" expected"); break;
			case 33: s = coco_string_create(L"\"&\" expected"); break;
			case 34: s = coco_string_create(L"\"==\" expected"); break;
			case 35: s = coco_string_create(L"\"!=\" expected"); break;
			case 36: s = coco_string_create(L"\"<\" expected"); break;
			case 37: s = coco_string_create(L"\">\" expected"); break;
			case 38: s = coco_string_create(L"\"<=\" expected"); break;
			case 39: s = coco_string_create(L"\">=\" expected"); break;
			case 40: s = coco_string_create(L"\"<<\" expected"); break;
			case 41: s = coco_string_create(L"\">>\" expected"); break;
			case 42: s = coco_string_create(L"\"+\" expected"); break;
			case 43: s = coco_string_create(L"\"-\" expected"); break;
			case 44: s = coco_string_create(L"\"*\" expected"); break;
			case 45: s = coco_string_create(L"\"/\" expected"); break;
			case 46: s = coco_string_create(L"\"%\" expected"); break;
			case 47: s = coco_string_create(L"\"++\" expected"); break;
			case 48: s = coco_string_create(L"\"--\" expected"); break;
			case 49: s = coco_string_create(L"\"[\" expected"); break;
			case 50: s = coco_string_create(L"\"]\" expected"); break;
			case 51: s = coco_string_create(L"\"->\" expected"); break;
			case 52: s = coco_string_create(L"\"=\" expected"); break;
			case 53: s = coco_string_create(L"\"*=\" expected"); break;
			case 54: s = coco_string_create(L"\"/=\" expected"); break;
			case 55: s = coco_string_create(L"\"%=\" expected"); break;
			case 56: s = coco_string_create(L"\"+=\" expected"); break;
			case 57: s = coco_string_create(L"\"-=\" expected"); break;
			case 58: s = coco_string_create(L"\"&=\" expected"); break;
			case 59: s = coco_string_create(L"\"^=\" expected"); break;
			case 60: s = coco_string_create(L"\"|=\" expected"); break;
			case 61: s = coco_string_create(L"\"<<=\" expected"); break;
			case 62: s = coco_string_create(L"\">>=\" expected"); break;
			case 63: s = coco_string_create(L"\"!\" expected"); break;
			case 64: s = coco_string_create(L"\"~\" expected"); break;
			case 65: s = coco_string_create(L"??? expected"); break;
			case 66: s = coco_string_create(L"invalid Statement"); break;
			case 67: s = coco_string_create(L"invalid ReturnStatement"); break;
			case 68: s = coco_string_create(L"invalid ParamExpression"); break;
			case 69: s = coco_string_create(L"invalid ParamExpression"); break;
			case 70: s = coco_string_create(L"invalid Label"); break;
			case 71: s = coco_string_create(L"invalid Label"); break;
			case 72: s = coco_string_create(L"invalid AssignmentOperator"); break;
			case 73: s = coco_string_create(L"invalid UnaryExp"); break;
			case 74: s = coco_string_create(L"invalid UnaryOperator"); break;
			case 75: s = coco_string_create(L"invalid Primary"); break;

		default:
		{
			wchar_t format[20];
			//coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}


