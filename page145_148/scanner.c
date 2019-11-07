#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_KEYWORDS 7
#define ID_LENGTH 12

struct tokenType {
	int number;
	union {
		char id[ID_LENGTH];
		int num;
	} value;
}tokenType;
char *keyword[NO_KEYWORDS] = { "const", "else", "if", "int", "return", "void", "while" };

enum tsymbol {
	tnull = -1, tnot, tnotequ, tmod, tmodAssign, tident, tnumber, tand, tlparen, trparen, tmul, tmulAssign, tplus, tinc, taddAssign, tcomma,
	tminus, tdec, tsubAssign, tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign, tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	tconst, telse, tif, tint, treturn, tvoid, twhile, tlbrace, tor, trbrace, tcomment, tcolon };

enum tsymbol tnum[NO_KEYWORDS] = {
	tconst, telse, tif, tint, treturn, tvoid, twhile };

void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &.\n");
		break;
	case 3: printf("next character must be |.\n");
		break;
	case 4: printf("invalid character!!!\n");
		break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}


int hexValue(char ch)
{
	switch (ch) {
		case '0' : case '1' : case '2' : case '3' : case '4' :
		case '5' : case '6' : case '7' : case '8' : case '9' :
						return (ch-'0');
		case 'A' : case 'B' : case 'C' : case 'D' : case 'E' : case 'F' :
						return (ch-'A'+10);
		case 'a' : case 'b' : case 'c' : case 'd' : case 'e' : case 'f' :
						return (ch - 'a' + 10);
		default: return -1;
	}
}

int getIntNum(char firstCharacter)
{
	int num = 0;
	int value;
	char ch;
	
	if(firstCharacter != '0') {
		ch = firstCharacter;
		do {
			num = 10*num + (int)(ch - '0');
			ch = getchar();
		} while(isdigit(ch));
	} else {
		ch = getchar();
		if((ch >= '0') && (ch <= '7'))
			do {
				num = 8*num + (int)(ch - '0');
				ch = getchar();
			}while((ch >= '0') && (ch <= '7'));
		else if((ch == 'X') || (ch == 'x')) {
			while((value=hexValue(ch=getchar())) != -1)
				num = 16*num + value;
		}
		else num = 0;
	}
	ungetc(ch, stdin);
	return num;
}


struct tokenType scanner(FILE *fp)
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH],num[ID_LENGTH];

	token.number = tnull;

	do {
		while (isspace(ch = fgetc(fp)));
		if (superLetter(ch)) {
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(fp);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			ungetc(ch, stdin);
			for (index = 0; index < NO_KEYWORDS; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORDS) {
				token.number = tnum[index];
				strcpy(token.value.id, id);
			}
			else {
				token.number = tident;
				strcpy(token.value.id, id);
			}
		}
		else if (isdigit(ch)) {
			token.number = tnumber;
			i = 0;
			do {
				if (i < ID_LENGTH) num[i++] = ch;
				ch = fgetc(fp);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			num[i] = '\0';
			token.value.num = atoi(num);
		}
		else switch (ch) {            

		case '/':               
			ch = fgetc(fp);
			if (ch == '*')         
				do {
					token.number = tcomment;
					while (ch != '*') ch = fgetc(fp);
					ch = fgetc(fp);
				} while (ch != '/');
			else if (ch == '/') {      
				token.number = tcomment;
				strcpy(token.value.id, "//");
				break;
			}
			else if (ch == '=') token.number = tdivAssign;
			else {
				token.number = tdiv;
				ungetc(ch, stdin); 
			}
			break;

		case '!':               
			ch = fgetc(fp);
			if (ch == '=') token.number = tnotequ;
			else {
				token.number = tnot;
				ungetc(ch, stdin);   
			}
			break;

		case '%':              
			ch = fgetc(fp);
			if (ch == '=')
				token.number = tmodAssign;
			else {
				token.number = tmod;
				strcpy(token.value.id, "%");
				ungetc(ch, stdin);
			}
			break;

		case '&':               
			ch = fgetc(fp);
			if (ch == '&') token.number = tand;
			else {
				lexicalError(2);
				ungetc(ch, stdin);
			}
			break;

		case '*':             
			ch = fgetc(fp);
			if (ch == '=') token.number = tmulAssign;
			else {
				token.number = tmul;
				ungetc(ch, stdin);  
			}
			break;

		case '+':             
			ch = fgetc(fp);
			if (ch == '+') {
				token.number = tinc;
				strcpy(token.value.id, "++"); 
			}
			else if (ch == '=') token.number = taddAssign;
			else {
				token.number = tplus;
				ungetc(ch, stdin);   
			}
			break;

		case '-':               
			ch = fgetc(fp);
			if (ch == '-') token.number = tdec;
			else if (ch == '=') token.number = tsubAssign;
			else {
				token.number = tminus;
				ungetc(ch, stdin);  
			}
			break;

		case '<':               // state 36
			ch = fgetc(fp);
			if (ch == '=') {
				token.number = tlesse;
				strcpy(token.value.id, "<=");
			}
			else {
				token.number = tless;
				ungetc(ch, stdin); 
			}
			break;

		case '=':             
			ch = fgetc(fp);
			if (ch == '=') {
				token.number = tequal;
				strcpy(token.value.id, "==");
			}
			else {
				token.number = tassign;
				strcpy(token.value.id, "=");
				ungetc(ch, stdin);  
			}
			break;

		case '>':             
			ch = fgetc(fp);
			if (ch == '=') token.number = tgreate;
			else {
				token.number = tgreat;
				ungetc(ch, stdin);  
			}
			break;

		case '|':            
			ch = fgetc(fp);
			if (ch == '|') token.number = tor;
			else {
				lexicalError(3);
				ungetc(ch, stdin);  
			}
			break;

		case '(': token.number = tlparen;
			strcpy(token.value.id, "(");
			break;
		case ':': token.number = tcolon;
			strcpy(token.value.id, ":"); 
			break;
		case ')': token.number = trparen;
			strcpy(token.value.id, ")");
			break;
		case ',': token.number = tcomma;
			strcpy(token.value.id, ",");
			break;
		case ';': token.number = tsemicolon;
			strcpy(token.value.id, ";");
			break;
		case '[': token.number = tlbracket;
			strcpy(token.value.id, "[");
			break;
		case ']': token.number = trbracket;
			strcpy(token.value.id, "]");
			break;
		case '{': token.number = tlbrace;
			strcpy(token.value.id, "{");
			break;
		case '}': token.number = trbrace;
			strcpy(token.value.id, "}");
			break;
		case EOF: token.number = teof;         break;

		default: {
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}

		} // switch end
	} while (token.number == tnull);
	return token;
} // end of scanner

void main()
{
	struct tokenType token;
	FILE *fp;
	if((fp=fopen("prime.mc", "r"))==NULL)
	{
	printf("File open error!");
	}
	
	/* 소스코드(prime.mc)에대한어휘분석기의출력형태
	constintmax = 100;
	*/

	do{
		token = scanner(fp); //pp. 145~148 참조
		if (token.number == tnumber) //토큰이상수일때
			printf("Token: %d: (%d,%d)\n", token.value.num, token.number, token.value.num);
		// (5) 출력형태: Token: 100 : (5, 100)
		else if (token.number == tident) //토큰이명칭일때//page 148쪽(1)참조
			printf("Token: %s: (%d,%s)\n", token.value.id, token.number, token.value.id);
		// (3) 출력형태: Token: max: (4, max)
		else if (token.number == teof)
			break;
		//토큰이Keyword(명령어, 연산자) 일때
		else printf("Token: %s: (%d,%d)\n", token.value.id, token.number, 0);
		// (1) 출력형태: Token: const: (30, 0)
		// (2) 출력형태: Token: int: (33, 0)
		// (4) 출력형태: Token: =: (23, 0)
		// (6) 출력형태: Token: ; (20, 0)
	}while (token.number != teof);
	system("pause");
	getchar();
	fclose(fp);
	system("pause");
}
