#include <stdio.h>
#include <string.h>
#include "Absyn.h"
#include "parser.hpp"
#include "debug.h"
#include <assert.h>

class LineBuffer
{
public:
	LineBuffer(FILE *fp);
	~LineBuffer();

	char getNextChar();
	bool isEOF();
	int getPos();
	int getLength();
	int getRow();
	void printLine();
private:
	char *buf;
	int length;
	int pos;
	bool eof;
	static int row;
};
LineBuffer::LineBuffer(FILE *fp)
	: buf(NULL)
	, length(0)
	, pos(0)
	, eof(false)
{
	buf = new char[4096];
	char *p = fgets(buf, 4096, fp);
	if (p == NULL) {
		if (ferror(fp)) {
			ERROR("Failed to read line");
		}
		eof = true;
		VDBG("EOF");
		return;
	} else {
		VDBG("%s", buf);
		length = strlen(buf);
		pos = 0;
	}
	row++;
}

LineBuffer::~LineBuffer()
{
	delete [] buf;
}

char
LineBuffer::getNextChar()
{
	if (pos < length) {
		char c = buf[pos];
		++pos;
		return c;
	} else {
		return 0;
	}
}

bool
LineBuffer::isEOF()
{
	return eof;
}

int
LineBuffer::getPos()
{
	return pos;
}

int
LineBuffer::getLength()
{
	return length;
}

int
LineBuffer::getRow()
{
	return row;
}

void
LineBuffer::printLine()
{
	fprintf(stdout, "%6d |%.*s", row, length, buf);
}

int
LineBuffer::row;

extern FILE *yyin;
LineBuffer *lineBuffer = NULL;
int tokenStartPos;
int tokenLength;
int tokenNextStartPos;
bool eof = false;

int getNextChar(char *b, int maxBuffer) 
{
 retry:
	if (lineBuffer == NULL) {
		lineBuffer = new LineBuffer(yyin);
		tokenNextStartPos = 1;
	}

	if (lineBuffer->isEOF()) {
		return 0;
	}

	b[0] = lineBuffer->getNextChar();
	VDBG("getNextChar() => '%c'0x%02x at %d",
		b[0], b[0], lineBuffer->getPos());
	
	if (b[0] != 0) {
		return 1;
	} else {
		delete lineBuffer;
		lineBuffer = NULL;
		goto retry;
	}
}

void beginToken(const char *t) 
{
	assert(lineBuffer);
	tokenStartPos = tokenNextStartPos;
	tokenLength = strlen(t);
	tokenNextStartPos = lineBuffer->getPos(); // + 1;
	
	yylloc.first_line = lineBuffer->getRow();
	yylloc.first_column = tokenStartPos;
	yylloc.last_line = lineBuffer->getRow();
	yylloc.last_column = tokenStartPos + tokenLength - 1;
	VDBG("beginToken %s (line[%d]:%d-%d)", t, yylloc.first_line, yylloc.first_column, yylloc.last_column);
}


void printError(const char *errorstring, ...) 
{
	static char errmsg[10000];
	va_list args;

	int start = tokenStartPos;
	int end = start + tokenLength - 1;
	int i;

	/*================================================================*/
	/* simple version ------------------------------------------------*/
	/*
	  fprintf(stdout, "...... !");
	  for (i=0; i<nBuffer; i++)
      fprintf(stdout, ".");
	  fprintf(stdout, "^\n");
	*/

	/*================================================================*/
	/* a bit more complicate version ---------------------------------*/
	/* */
	assert(lineBuffer);
	lineBuffer->printLine();
	if (lineBuffer->isEOF()) {
		fprintf(stdout, "...... !");
		for (int i = 0; i < lineBuffer->getLength(); ++i) {
			fprintf(stdout, ".");
		}
		fprintf(stdout, "^-EOF\n");
	}
	else {
		fprintf(stdout, "...... !");
		for (int i = 1; i < start; ++i) {
			fprintf(stdout, ".");
		}
		for (int i = start; i <= end; ++i) {
			fprintf(stdout, "^");
		}
		for (int i = end+1; i < lineBuffer->getLength(); ++i) {
			fprintf(stdout, ".");
		}
		fprintf(stdout, "   token%d:%d\n", start, end);
	}
	/* */
	
	/*================================================================*/
	/* print it using variable arguments -----------------------------*/
	va_start(args, errorstring);
	vsprintf(errmsg, errorstring, args);
	va_end(args);
	
	fprintf(stdout, "Error: %s\n", errmsg);
}

void
parseComplete()
{
	if (lineBuffer) {
		delete lineBuffer;
	}
}
