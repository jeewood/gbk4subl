#include <stdio.h>
#include "Parser.h"
#include "Scanner.h"

void main(int argc, char** argv)
{
		wchar_t *fileName = coco_string_create(argv[1]);
		Scanner *scanner = new Scanner(fileName);
		Parser *parser = new Parser(scanner);
		parser->Parse();

		coco_string_delete(fileName);
		delete parser;
		delete scanner;
}
