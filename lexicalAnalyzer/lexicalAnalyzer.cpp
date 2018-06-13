//词法分析器
#include<iostream>
#include<cstdio> 
#include<ctype.h>
#include<cstdlib>
#include<cstring>
#include<map>
using namespace std;
map<string,int> reMap;
map<string,int> odMap;
//保留字表
static char reserveWord[32][20] = {
"auto", "break", "case", "char", "const", "continue",
"default", "do", "double", "else", "enum", "extern",
"float", "for", "goto", "if", "int", "long",
"register", "return", "short", "signed", "sizeof", "static",
"struct", "switch", "typedef", "union", "unsigned", "void",
"volatile", "while"
};

//界符运算符表
static char operatorOrDelimiter[37][10] = {
"+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==",
"!=", ";", "(", ")", "^", ",", "\"", "\'", "#", "&",
"&&", "|", "||", "%", "~", "<<", ">>", "[", "]", "{",
"}", "\\", ".", "\?", ":", "!","\\n"
};

const int MAXN=10000;
char file[MAXN],tempFile[MAXN];
FILE *fp,*fp1;

void Input(){
	if((fp=fopen("E://编程代码/compilingPrinciple/0611_lexicalAnalyzer/origin.txt","r"))==NULL){
		printf("The file can't open!");exit(0);
	}
	int n=0;
	while((tempFile[n++]=fgetc(fp))!=EOF);
}
//对输入程序进行预处理,删除注释和非必要的空格 
void Preprocess(){
	int n=sizeof(tempFile)/sizeof(char);
	int k=0;
	for(int i=0;i<n;){
		switch (tempFile[i]){
			case ' '://删除空格 
				if(tempFile[i+1]==' '){
					i++;
				}else file[k++]=tempFile[i++];
				break;
			case '/':
				if(tempFile[i+1]=='*'){//删除/**/内的内容 
					while(!(tempFile[i]=='*'&&tempFile[i+1]=='/'))i++;
					i++;
				} else if (tempFile[i+1]=='/'){//删除//后的一行注释 
					while(tempFile[++i]!='\n');
				}
				i++;
				break;
			case '\n':
				i++;
				break;
			default:
				file[k++]=tempFile[i++];
				break;
		}	
	}
}

/*
1、系统保留字（系统中） 
2、用户标识符（用户定义） 
3、数字（用户定义） 
4、运算符或操作符（系统中） 
*/ 
void lexicalAnalyzer(){
//	fp1=fopen("E://编程代码/compilingPrinciple/0611_lexicalAnalyzer/result.txt","w+");
	int n=sizeof(file)/sizeof(char);
	char token[20];
	int k=0;//token字符串的位置 
	for(int i=0;i<n;i++){//读入程序的位置指针
		if(isspace(file[i]))continue;
		else if(isalpha(file[i])){
			token[k=0]=file[i];
			for(i++;i<n&&isalnum(file[i]);){
				token[++k]=file[i++];
			}
			i--;token[k+1]='\0';
			if(reMap.count(token))printf("(%d,%s)\n",reMap[token],token);//输出关键字 
			else printf("(100,%s)\n",token);//否则为标识符 
		}
		else if(isdigit(file[i])){//如果为数字 
			token[k=0]=file[i];
			for(i++;i<n&&isdigit(file[i]);){
				token[++k]=file[i++];
			}
			i--;
			printf("(101,%s)\n",token);
		}
		else if(ispunct(file[i])){//如果参数是出字母、数字或空格以外的可打印字符 
			token[k=0]=file[i];
			token[++k]=file[++i];
			if(odMap.count(token)){
				printf("(%d,%s)\n",odMap[token],token);
			}else{
				token[k]='\0';
				i--;k--;
				if(odMap.count(token))
					printf("(%d,%s)\n",odMap[token],token);
			}
		}
		memset(token,0,sizeof(token));	
	}
}

int main(){
	Input();
	Preprocess();
	printf("%s\n",tempFile);
	printf("\n%s\n",file);
	string s1,s2;
	int n=sizeof(operatorOrDelimiter)/10*sizeof(char); 
	int m=sizeof(reserveWord)/20*sizeof(char);
	for(int i=0;i<n;i++){
		s1=operatorOrDelimiter[i];
		odMap.insert(make_pair(s1,i));
		if(i<m){
			s2=reserveWord[i];
			reMap.insert(make_pair(s2,i+n)); 
		}
	}
	lexicalAnalyzer();
	return 0;
}

