
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 * Tokens: \n, \t, |, &, <, >, >&, >>, >>&, 2>
 * %token NOTOKEN NEWLINE PIPE AMPERSAND LESS GREAT GREATAMPERSAND GREATGREAT GREATGREATAMPERSAND TWOGREAT
 */

%{

#include <cstring>
#include "y.tab.hh"

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

%%

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

"|" {
	return PIPE;
}

"&"	{
	return AMPERSAND;
}

"<" {
	return LESS;
}

">" {
  return GREAT;
}

">&" {
	return GREATAMPERSAND;
}

">>" {
	return GREATGREAT;
}

">>&" {
	return GREATGREATAMPERSAND;
}

"2>" {
	return TWOGREAT;
}


["][^\n\"]*["] {
	/* 2.4: Quotes */
	
	yylval.cpp_string = new std::string(yytext);
	*yylval.cpp_string = yylval.cpp_string->substr(1, yylval.cpp_string->length()-2);

	/*printf("after dequote, string = %s\n",yylval.cpp_string);*/
	return WORD;
}

[^ \t\n|><&]*\\[^ \t\n]* {
	/* 2.5 Escaping */

	std::string *str = new std::string(yytext);
	char* cstr = new char [str->length()+1];
	std::strcpy (cstr, str->c_str());
	cstr[str->length] = '\0';

	printf("before deescaping , string = %s\n", cstr);

	int i = 0;
	char * newstr = (char*)malloc(100);
	while (*cstr){
		if (*cstr == '\\'){
			if ( *(cstr+1) == '\\' ){
				cstr = cstr + 2;
				newstr[i++] = '\\';
			}else {
				newstr[i++] = *(++cstr);
			}
		}else {
			newstr[i++] = *cstr;
		}
		cstr++;
	}
	newstr[i] = '\0';
	printf("after deescaping , string = %s\n", newstr);

	std::string cpp(newstr);

	//yylval.cpp_string = *cpp;

	return WORD;
}

[^ \t\n|><][^ \t\n|><]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}

.  {
    return NOTOKEN;
}

%%

/*TODO: subshell, escape, quote*/