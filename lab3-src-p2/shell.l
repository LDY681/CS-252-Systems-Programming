
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

\$\([^\n]*\) {
	/* 2.8: Subshell*/
	//get rid of $(
	std::string str = yytext;
	std::string sstr = str.substr(2);
	//get rid of )
	std::size_t	pos = str.find(")");
	std::string temp = sstr.substr(0,pos-2);
	char *subshell = const_cast<char*> (temp.c_str());
	//printf("the subshell command is %s\n", subshell);

	int tempin = dup(0);
	int tempout = dup(1);

	int fdpipein[2];	//subshell
	pipe(fdpipein);

	int fdpipeout[2];	//outshell
	pipe(fdpipeout);

	//subshell command in
	write(fdpipein[1], subshell, strlen(subshell));
	write(fdpipein[1], "\n", 1);
	close(fdpipein[1]);	//close fdpipein[1]

	//subshell command out
	dup2(fdpipein[0], 0);	//fdpipein[0] to input
	close(fdpipein[0]);	//close fdpipein[0]

	//
	dup2(fdpipeout[1], 1);	//
	close(fdpipeout[1]);	//close fdpipeout[1]

	int ret = fork();

	if (ret == 0) {	//in child process
		execvp("/proc/self/exe", NULL);
		_exit(1);
	} else if (ret < 0) {
		perror("fork");
		exit(1);
	}

	dup2(tempin, 0);
	dup2(tempout, 1);
	close(tempin);
	close(tempout);

	char c;
	char * buffer = (char *) malloc (4096);
	int i = 0;
	
	while (read(fdpipeout[0], &c, 1)) {
		if (c == '\n') {	//if \n then put a space
			buffer[i++] = ' ';
		}
		else {
			buffer [i++] = c;
		}
	}
	buffer[i] = '\0';

	//printf("the output of subshell is %s\n", buffer);
	//read backward
	int j = i - 1;
	for (j = i - 1; j >= 0; j--) {	
		//printf("c = %c\n", buffer[j]);
		myunputc(buffer[j]);
	}
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

	char * str = strdup(yytext);
	char * newstr = (char*) malloc (100);
	//printf("before deescaping, string = %s\n", str);

	int i = 0;
	while (*str){
		if (*str == '\\'){
			if (*(str+1) == '\\'){
				str = str +2;
				newstr[i++] = '\\';
			}else{
				newstr[i++] = *(++str);
			}
		}else{
			newstr[i++] = *str;
		}
		str++;
	}
	newstr[i] = '\0';

	//printf("after deescaping, string = %s\n", newstr);
//	std::string *newstr;

//	int i;
//	for(i = 0; i <yytext->length(); i++){
//		if (yytext[i] == '\\' && i < yytext->length()-1 && yytext[i+1] == '\\'){
//			newstr->append('\\');
//			i = i+1;
//		}else{
//			newstr->append(yytext[i]);
//		}
//	}
	yylval.cpp_string = new std::string(newstr);

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