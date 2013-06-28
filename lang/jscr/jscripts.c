#include <stdio.h>


typedef struct _Token
{
	int tp;
	char *tk_str;
}TToken;

void Init(TToken *tk)
{
	tk->tp = 0;
	tk->tk_str = 0;
}

TToken Token;




void main()
{
	Init(&Token);
}
