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
				if((*it1)!='&')
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
			if(isupper(end[i])&&islower(end[i+1])){
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
				for(set<char>::iterator it1=Set.begin();it1!=Set.end();it1++)
				followSet[mid].insert((*it1));
			}
		}
	}
	for(it=pMap.begin();it!=pMap.end();it++){//对所有产生式计算第四种情况
	    char start=(*it).second.start;
	    int sid=xMap[start];
		char end[10];
		strcpy(end,(*it).second.end);
		for(int i=0;i<strlen(end);i++){
			set<char> Set;
			if(i+1<strlen(end))
			Set=getFirstSetByString(end,i+1); 
			if(isupper(end[i])&&((i+1)==strlen(end)||Set.empty())){
				int eid=xMap[end[i]];
				for(set<char>::iterator it1=followSet[sid].begin();it1!=followSet[sid].end();it1++)
				followSet[eid].insert((*it1));
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
					}
				    else if(isupper(p.end[i])){//次简单情况 
				    	int id=xMap[x];//非终结符的编号 
					    set<char> fSet=firstSet[id];
					    if(!fSet.count('&')){
					    	for(set<char>::iterator it1=fSet.begin();it1!=fSet.end();it1++){
					    		pdFirstSet[pid].insert((*it1));
						        firstSet[sid].insert((*it1));//将非终结符的first集添入p.start非终结符first集中 
					        } 
					        vis[pid]=1;
					        break;//计算完成,跳出所在最近for循环 
					    }else{//最复杂情况
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


int main(){
	while((fp=fopen("E://编程代码/compilingPrinciple/0614_LL(1)_Parser/Input.txt","r"))==NULL){
		printf("Don't open the file!");exit(0);
	}
	char s1;
	char s2[10];
	int i=0,j=0,k=0;//分别表示产生式、非终结符和终结符数量 
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
	
	char rm[]="aade#";
	for(int i=strlen(rm);i>=0;i--){
		rmStack.push(rm[i]);
	}
	
	int cnt=0;
	alStack.push('#');
	alStack.push('A');
	while(rmStack.size()>1){
		char a=alStack.top();alStack.pop();
		char r=rmStack.top();rmStack.pop();
		if(isupper(a)){
			int xid=xMap[a],yid=yMap[r];
			int pid=analysisFigure[xid][yid];
			if(pid==-1){
				cout<<"analysis Failed!"<<endl;
				break;
			}
			Production pdt=pMap[pid];
			for(int i=strlen(pdt.end);i>=0;i--){
				alStack.push(pdt.end[i]);
			}
		} else if(a==r)continue;
	}
	cout<<"analysis Successfully!"<<endl;
	
	
	
	
	
	
	
	
	/*cout<<"Analysis Figure:"<<endl;
	for(int i=0;i<xMap.size();i++){
		for(int j=0;j<yMap.size();j++){
			cout<<analysisFigure[i][j]<<" ";
		}
		cout<<endl;
	}
	
	cout<<"First Set:"<<endl;
	for(int i=0;i<pdFirstSet.size();i++){
		for(set<char>::iterator it=pdFirstSet[i].begin();it!=pdFirstSet[i].end();it++){
			cout<<(*it)<<" ";
		}
		cout<<endl;
	}
	
	cout<<"Follow Set:"<<endl;
	for(int i=0;i<followSet.size();i++){
		for(set<char>::iterator it=followSet[i].begin();it!=followSet[i].end();it++){
			cout<<(*it)<<" ";
		}
		cout<<endl;
	}
	
	
	cout<<"Select Set:"<<endl;
	for(int i=0;i<selectSet.size();i++){
	   for(set<char>::iterator it=selectSet[i].begin();it!=selectSet[i].end();it++){
			cout<<(*it)<<" ";
		}
		cout<<endl;
	}*/
	
	/*for(int i=0;i<firstSet.size();i++){
		for(set<char>::iterator it=firstSet[i].begin();it!=firstSet[i].end();it++){
			cout<<(*it)<<" ";
		}
		cout<<endl;
	}
	for(int i=0;i<pMap.size();i++)
	cout<<pMap[i].start<<"->"<<pMap[i].end<<endl;*/
	/*for(map<char,int>::iterator it=yMap.begin();it!=yMap.end();it++)
	cout<<(*it).first<<":"<<(*it).second<<" ";
	cout<<endl;
	for(map<char,int>::iterator it=xMap.begin();it!=xMap.end();it++)
	cout<<(*it).first<<":"<<(*it).second<<" ";*/
	return 0;
} 







 
