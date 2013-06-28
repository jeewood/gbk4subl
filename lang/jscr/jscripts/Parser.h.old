

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__



#include "Scanner.h"



class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_identifier=1,
		_number=2,
		_HexNumber=3,
		_string=4,
		_char=5
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void JScr();
	void Statement();
	void AssignmentExpression();
	void BreakStatement();
	void ContinueStatement();
	void DoStatement();
	void ForStatement();
	void IfStatement();
	void ReturnStatement();
	void SwitchStatement();
	void WhileStatement();
	void BlockStatement();
	void DefineStatement();
	void ParamExpression();
	void NameList();
	void Label();
	void Expression();
	void ActualParameters();
	void LogORExp();
	void AssignmentOperator();
	void LogANDExp();
	void InclORExp();
	void ExclORExp();
	void ANDExp();
	void EqualExp();
	void RelationExp();
	void ShiftExp();
	void AddExp();
	void MultExp();
	void UnaryExp();
	void PostFixExp();
	void UnaryOperator();
	void Primary();
	void FunctionCall();

	void Parse();

}; // end Parser



#endif

