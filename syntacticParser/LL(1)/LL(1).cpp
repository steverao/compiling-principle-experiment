//LL(1)自顶向下语法分析器

/*算法步骤 
*1)读入输入的文法所有参生式
*2)依据产生式计算非终结符的First、Follow集以及产生式的Select集
*3)依据产生式的Select集构造分析表
*4)依据构造的分析表，对句子进行语法分析
*/
#include<iostream>
#include<cstdio>
#include<cstdlib> 
#include<map>
#include<set>
#include<iterator>
#include<vector>
#include<cstring>
#include<stack>
using namespace std;
FILE* fp;
typedef struct Node{
	char start;
	char end[10]; 
}Production;
const int MAXN=20;//大致估计的非终结符或终结符数量 
int analysisFigure[MAXN][MAXN];//分析表结构
map<char,int> xMap,yMap;//分别用来存储非终结符和终结符 
map<int,Production> pMap;//存储参生式 
map<int,set<char> > pdFirstSet;//存储每一个产生式的first集，int项表示产生式编号 
map<int,set<char> > firstSet;//int 项表示非终结符编号,set集合存储对应first集合
map<int,set<char> > followSet;//同上
map<int,set<char> > selectSet;//int 项表示产生式的编号,set集合存储对应select集合  
stack<char> alStack;//分析栈
stack<char> rmStack;//余留分析栈 

void buildAnalysisGraph(){
	for(int i=0;i<pMap.size();i++){
		char s=pMap[i].start;
		int xid=xMap[s];
		set<char>::iterator it;
		for(it=selectSet[i].begin();it!=selectSet[i].end();it++){
			int yid=yMap[(*it)];
			analysisFigure[xid][yid]=i;
		} 
	}
}

set<char> getFirstSetByString(char end[],int s){
	set<char> Set;
	for(int i=s;i<strlen(end);i++){
		if(!isupper(end[i])){
			Set.insert(end[i]);
			break;
		}
		int sid=xMap[end[i]];
		if(!firstSet[sid].count('&')){
			set<char> fSet=firstSet[sid];
			for(set<char>::iterator it1=fSet.begin();it1!=fSet.end();it1++){
				Set.insert((*it1));
			}
			break;//计算结束 
		}else if(firstSet[sid].count('&')){
			set<char> fSet=firstSet[sid];
			for(set<char>::iterator it1=fSet.begin();it1!=fSet.end();it1++){
				//if((*it1)!='&')
				char c=(*it1);
				Set.insert((*it1));//将非终结符的添加 
		    } 
		}
	}
	return Set;
}

void getSelectSet(){
	for(int i=0;i<pMap.size();i++){
		char end[10];
		strcpy(end,pMap[i].end);
		set<char> Set=getFirstSetByString(end,0);
		char start=pMap[i].start;
		int xid=xMap[start];
		if(end[0]=='&'){//SelectSet等于FollowSet 	
			selectSet[i]=followSet[xid];
		}else if(strlen(end)>0&&end[0]!='&'&&Set.empty()){//SelectSet等于FollowSet交FirstSet 
		    set<char> Set;
		    set<char>::iterator it;
		    for(it=pdFirstSet[i].begin();it!=pdFirstSet[i].end();it++)
		    Set.insert((*it));
		    for(it=followSet[xid].begin();it!=followSet[xid].end();it++)
		    Set.insert((*it));
		    selectSet[i]=Set;
		
		}else if(strlen(end)>0&&end[0]!='&'&&!Set.empty()){//等于FirstSet
			selectSet[i]=pdFirstSet[i];
		} 
	}
}


void getFollowSet(){
	/*算法: 
	*1、若A是文法开始符号，则#属于Follow(A)。 
	*2、若存在产生式B->..Aa..，则把a加入到Follow(A)。
	*3、若存在产生式B->..AB..,则把First(B..)加入到Follow(A)。
	*4、若存在产生式B->..A&,则把Follow(B)加入到Follow(A) 
	*/
	char s=pMap[0].start;
	int sid=xMap[s];followSet[sid].insert('#');//算法中第一种情况 
	map<int,Production>::iterator it;
	for(it=pMap.begin();it!=pMap.end();it++){//对所有产生式计算第二种情况 
		char end[10];
		strcpy(end,(*it).second.end);
		for(int i=0;i<strlen(end);i++){
			if(isupper(end[i])&&(islower(end[i+1])||(isprint(end[i+1])&&!isalpha(end[i+1])))){
				int eid=xMap[end[i]];
				followSet[eid].insert(end[i+1]);
				i++;
			}
		}
	} 
	for(it=pMap.begin();it!=pMap.end();it++){//对所有产生式计算第三种情况
		char end[10];
		strcpy(end,(*it).second.end);
		for(int i=0;i+1<strlen(end);i++){
			if(isupper(end[i])&&isupper(end[i+1])){
				set<char> Set=getFirstSetByString(end,i+1);
				int mid=xMap[end[i]];
				for(set<char>::iterator it1=Set.begin();it1!=Set.end();it1++){
				    if((*it1)!='&')
				    followSet[mid].insert((*it1));
				}
			}
		}
	}
	for(it=pMap.begin();it!=pMap.end();it++){//对所有产生式计算第四种情况
	    char start=(*it).second.start;
	    int sid=xMap[start];
		char end[10];
		strcpy(end,(*it).second.end);
		for(int i=strlen(end)-1;i>=0;i--){
			set<char> Set;
			Set=getFirstSetByString(end,i);
			if(!isupper(end[i]))break;
			if(isupper(end[i])&&!Set.count('&')){
				int eid=xMap[end[i]];
				for(set<char>::iterator it1=followSet[sid].begin();it1!=followSet[sid].end();it1++){
				    followSet[eid].insert((*it1));
				}
				break;
			} 
			else if(i>=0&&isupper(end[i])&&Set.count('&')){
				int eid=xMap[end[i]];
				for(set<char>::iterator it1=followSet[sid].begin();it1!=followSet[sid].end();it1++){
					followSet[eid].insert((*it1));
				}
			
			}
		} 
	}	
}

void getFirstSet(){
	/*
	*算法：
	*1、首先计算最简单的情况A->aB,将a加入到First(A)
	*2、对于A->Ba的情况,且First(B)不包括空集,则将First(B)加入到First(A) 
	*3、对于第二种情况，如First(B)包含空集,将除空集的First(B)加入到First(A)，在将a也加入First(A)
	*4、重复以上过程直到总的数量不再增加 
	*/ 
	int vis[MAXN]; 
	memset(vis,0,sizeof(vis));
	int l=0,n=0;//初始的所有非终结符的first集元素数量 
	while(l==0||l!=n){
		l=n;
		for(map<int,Production>::iterator it=pMap.begin();it!=pMap.end();it++){
			int pid=(*it).first;
			if(!vis[pid]){//如果该产生式的头部first没有计算完 
			    Production p=(*it).second;//取出产生式 
			    char start=p.start;
			    int sid=xMap[start];n-=firstSet[sid].size();
			    int k=strlen(p.end);
			    for(int i=0;i<k;i++){//计算一个产生式的头部非终结符的first集 
			    	char x=p.end[i];
				
					if(i==0&&!isupper(x)){//最简单情况 
					    pdFirstSet[pid].insert(x);
						firstSet[sid].insert(x);
						vis[pid]=1;
					    break;//计算完成,跳出所在最近for循环 
					}else if(isupper(p.end[i])&&firstSet[xMap[x]].size()==0){
					    	break;
					}
				    else if(isupper(p.end[i])&&firstSet[xMap[x]].size()!=0){//次简单情况 
				    	int id=xMap[x];//非终结符的编号 
					    set<char> fSet=firstSet[id];
					    if(!fSet.count('&')){
					    	for(set<char>::iterator it1=fSet.begin();it1!=fSet.end();it1++){
					    		pdFirstSet[pid].insert((*it1));
						        firstSet[sid].insert((*it1));//将非终结符的first集添入p.start非终结符first集中 
					        } 
					        vis[pid]=1;
					        break;//计算完成,跳出所在最近for循环 
					    }
						else if(isupper(p.end[i])&&firstSet[xMap[x]].size()!=0&&firstSet[xMap[x]].count('&')){//最复杂情况
					    	for(set<char>::iterator it1=fSet.begin();it1!=fSet.end();it1++){
				    		    if((*it1)!='&'){
				    		    	firstSet[sid].insert((*it1));//将非终结符的first集添入p.start非终结符first集中 
				    		    	pdFirstSet[pid].insert((*it1));
				    		    }
						        
					        } 
					    }
				    }
			    } 
			n+=firstSet[sid].size(); 
			}					
	    }
	}	
}

char rmStackArray[MAXN];
int rmIndex;
int cnt=0,alIndex=0;
char alStackArray[MAXN]; 
char a,r;
Production pdt;


void Print(int pid=-1){
	printf("%d\t",++cnt);
	for(int i=0;i<alIndex;i++)printf("%c",alStackArray[i]);
	printf("\t");
	for(int i=rmIndex-1;i>=0;i--)printf("%c",rmStackArray[i]);
	if(pid!=-1&&isupper(a)){
		pdt=pMap[pid];
		printf("\t\t%c->%s\n",pdt.start,pdt.end);
	}else printf("\n");
} 



int main(){
	while((fp=fopen("E://编程代码/compilingPrinciple/0614_LL(1)_Parser/Input.txt","r"))==NULL){
		printf("Don't open the file!");exit(0);
	}
	char s1;
	char s2[10];
	int i=0,j=0,k=0;//分别表示产生式、非终结符和终结符数量 
	fscanf(fp,"%s\n",rmStackArray);
	rmIndex=strlen(rmStackArray);
	while(fscanf(fp,"%c->%s\n",&s1,&s2)!=EOF){
		Production pro;pro.start=s1;strcpy(pro.end,s2);//初始化产生式并将其存储 
		pMap[i++]=pro;
		if(!xMap.count(s1))
		xMap[s1]=j++;
		int n=strlen(s2);
		for(int t=0;t<n;t++)
		if(!yMap.count(s2[t])&&isgraph(s2[t])&&!isupper(s2[t]))
		yMap[s2[t]]=k++;
		memset(s2,0,sizeof(s2));//其实初始化s2避免字符问题; 
	}
	yMap['#']=k; 
	
	getFirstSet();
	getFollowSet();
	getSelectSet();
	memset(analysisFigure,-1,sizeof(analysisFigure));
	buildAnalysisGraph();
	
	
	for(int i=0;i<rmIndex;i++){
		rmStack.push(rmStackArray[i]);
	}
	char start=pMap[0].start;
	alStack.push('#');alStack.push(start);
    alStackArray[alIndex++]='#';alStackArray[alIndex++]=start;
	int pid,xid,yid,flag=0;
	printf("步骤\t分析栈\t余留输入串\t所用产生式\n");
	
	    
	while(rmStack.size()>0){
		//输出分析步骤 
		
		a=alStack.top();
		r=rmStack.top();
		
		if(isupper(a)){
			
			xid=xMap[a];yid=yMap[r];
			pid=analysisFigure[xid][yid];
			if(pid==-1){
				flag=1;
			    break;
			}
			Print(pid); 
			pdt=pMap[pid];
			alStack.pop();alIndex--;//将大写字母弹出栈 
			for(int i=strlen(pdt.end)-1;i>=0;i--){
				if(pdt.end[i]!='&'){
					alStack.push(pdt.end[i]);
				    alStackArray[alIndex++]=pdt.end[i];
				}
			}
		    //Print();
			a=alStack.top();
		    r=rmStack.top();
		    if(a==r){
		    	Print();
		    	alStack.pop();alIndex--;
			    rmStack.pop();rmIndex--;
		    }
			continue;
		} else if(a==r){//将两个栈内相同的小写字母同时弹出 
			alStack.pop();alIndex--;
			rmStack.pop();rmIndex--;
			Print();
			continue;
		}else{
			flag=1;
			break;
		}
		
	}
	if(flag)printf("\nanalysis Failed!\n");
	else printf("\nanalysis Successfully!\n");
	return 0;
} 







 
