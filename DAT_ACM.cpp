#include "stdafx.h"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <windows.h>
#include <codecvt>
#include <fstream>
#include <map>
using namespace std;
//-----------------------------------------
typedef struct DAT
{
	//int id;  //equal to array index

	int base; 
	//base<0: whole word,might also be mid node    
	//base>0: mid node   
	//base=0: empty node

	int check;
	//check=-1: root, len=0      
	//check= 0: root-child, len=1    or   empty node   
	//check> 0: can't define

	int failId;
	//failId=0: no fail node
	//failId>0: has fail node

	wstring content;
};
//-----------------------------------------
typedef struct Node
{
	wstring content;
	bool bIsCase;
};
//-----------------------------------------
vector<DAT>  m_dat;
vector<Node> m_ReInsert;
multimap <int, int> m_RecordCheck;
//-----------------------------------------
int TableSizeFor(int cap) 
{
	int n = cap - 1;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	return (n < 0) ? 1 :  n + 1;
}
//-----------------------------------------
void RecursiveMove(int id, const wstring& exclude)
{
	multimap<int, int>::iterator iter = m_RecordCheck.find(id);
	while(iter != m_RecordCheck.end())//instead of loop "m_dat" to find which "i" match "m_dat[i].check = id", use mapping to do quick search
	{
		int target  = iter->second;
		if(m_dat[target].base !=0 )
		{
			if(m_dat[target].content != exclude)
			{
				Node *node = new Node;
				node->content=m_dat[target].content;
				node->bIsCase=(m_dat[target].base<0);

				m_ReInsert.push_back(*node);

				delete node;
			}

			m_dat[target].check=0;
			m_dat[target].base=0;
			m_dat[target].failId=0;
			m_dat[target].content=L"";
			RecursiveMove(target,exclude); //recursive to move all the relative node from their IDs

			m_RecordCheck.erase(iter);
		}
		iter = m_RecordCheck.find(id);
	}
}
//-----------------------------------------
struct CompareNodeReverse
{
	bool operator()(const Node& first, const Node& second) 
	{
		return first.content > second.content; 
	}
};
//-----------------------------------------
struct CompareNodeLen
{
	bool operator()(const Node& first, const Node& second) 
	{
		return first.content.length() < second.content.length(); 
	}
};
//-----------------------------------------
struct CompareStringLenReverse
{
	bool operator()(const wstring& first, const wstring& second) 
	{
		return first.length() > second.length(); 
	}
};
//-----------------------------------------
void ResizingDAT(int reach)
{
	int ori_size = m_dat.size();
	if(reach>ori_size-1)
	{
		int new_size = TableSizeFor(reach+1);

		cout<<"resizing array..."<<ori_size<<" to "<<new_size<<endl;
		m_dat.resize(new_size);
		for(int m=ori_size;m<m_dat.size();m++)
		{
			m_dat[m].base=0;
			m_dat[m].check=0;
			m_dat[m].failId=0;
		}
		m_dat[0].check=-1;
	}
}
//-----------------------------------------
int Search(const wstring& str, bool bFindNode = false) //return id or -1 
{
	int base=str[0]+abs(m_dat[0].base);
	int base_pre=0;

	for(int j=0;j<str.length();j++)
	{
		if(base>m_dat.size()-1)
			return -1;

		if(base_pre ==m_dat[base].check  &&   m_dat[base].base!=0/*for first char*/  &&   str[j] ==m_dat[base].content.back())
		{
			if(j==str.length()-1)
			{
				if(m_dat[base].base<0 || bFindNode) 
					return base;
				else
					return -1;
			}

			base_pre = base;
			base=abs(m_dat[base].base)+str[j+1];
		}
		else
		{
			return -1;
		}
	}
}
//-----------------------------------------
int GetTargetID(wstring ws)
{
	if(ws.length()==1)
		return 0;

	ws.pop_back();

	return Search(ws,true);
}
//-----------------------------------------
void InsertBase(vector<Node> &vNodes) //vNodes all have same length and same target: only differ in last char
{
	int nTarget = GetTargetID(vNodes[0].content);

	int *ids = new int[vNodes.size()]; 

	for(int i=0;i<vNodes.size();i++)
		ids[i] = vNodes[i].content.back();

	for(int k=1;k<m_dat.size();k+=rand()/100+1)//if k=1 not fit, then random move forward to fit
	{
		int i=0;
		for(;i<vNodes.size();i++) //try to fit vNodes one by one
		{
			ResizingDAT(k+ids[i]);
			if(m_dat[k+ids[i]].content.length()!=0)
				break;        //not fit, continue loop
		}
		if(i!=vNodes.size())  //not fit, continue loop
			continue;

		//fit ok, base is k
		if(m_dat[nTarget].base<0)   //if is end char
			m_dat[nTarget].base=-k;
		else
			m_dat[nTarget].base=k;

		for(int n=0;n<vNodes.size();n++) //inserting
		{
			int id = k+ids[n];

			m_RecordCheck.insert(pair<int, int>(nTarget,id));



			m_dat[id].check=nTarget;
			m_dat[id].content=vNodes[n].content;

			m_dat[id].failId=0;
			wstring temp = vNodes[n].content;
			while(m_dat[id].failId<=0 && temp.length()>0)
			{
				temp.erase(temp.begin());
				m_dat[id].failId = Search(temp,true);
			}
			if(m_dat[id].failId<0)
				m_dat[id].failId=0;


			if(vNodes[n].bIsCase)
				m_dat[id].base=-id;
			else
				m_dat[id].base=id;
		}
		break;
	}
	delete ids;
}
//-----------------------------------------
void InsertSingle(wstring str)// for single insert
{
	if(Search(str)!=-1) //already exist
		return;

	int base=str[0]+abs(m_dat[0].base);
	int base_pre=0;

	for(int i=0;i<str.length();i++) //search down the trie
	{
		ResizingDAT(base);

		if(Search(str.substr(0,i+1),true)==-1  ||  str.length()==i+1 )
		{
			m_ReInsert.clear();   
			RecursiveMove(GetTargetID(str.substr(0,i+1)),str); //move all the nodes have same "check" and those nodes' "check" are theirs ID as well, all to m_ReInsert
			wstring wsTemp = str; 

			for(int m=i;m<str.length();m++)
			{
				Node *node = new Node;

				node->content=wsTemp;
				node->bIsCase=(m==i);

				m_ReInsert.push_back(*node); //insert all relative node from input string,  ex: insert "hello", "he" already exist, so push_back "hel"(mid)、"hell"(mid)、"hello"(end)
				wsTemp=wsTemp.substr(0,wsTemp.size()-1);

				delete node;
			}
			break;
		}
	}

	sort(m_ReInsert.begin(), m_ReInsert.end(),CompareNodeLen());
	int nMinLen=m_ReInsert.front().content.length();
	int nMaxLen=m_ReInsert.back().content.length();

	sort(m_ReInsert.begin(), m_ReInsert.end(),CompareNodeReverse());
	vector<Node> m_ReInsertEachLen;
	for(int len=nMinLen;len<=nMaxLen;len++)
	{
		m_ReInsertEachLen.clear();

		for(int j=0;j<m_ReInsert.size();j++)
		{
			if(m_ReInsert[j].content.length()==len)
			{
				m_ReInsertEachLen.push_back(m_ReInsert[j]);
				m_ReInsert[j].content=L"";
			}
			else if(m_ReInsert[j].content==L"" && m_ReInsertEachLen.size()>0)
			{
				InsertBase(m_ReInsertEachLen); //group insert all the nodes that have same "check"
				m_ReInsertEachLen.clear();
			}
		}

		if(m_ReInsertEachLen.size()>0)
		{
			InsertBase(m_ReInsertEachLen);     //group insert all the nodes that have same "check"
			m_ReInsertEachLen.clear();
		}
	}

	m_ReInsertEachLen.clear();
}
//-----------------------------------------
void InsertGroup(vector<Node> &vNodes,int len)//for group insert from dictionary, vNodes have same length
{
	wstring sTarget = vNodes.back().content.substr(0,len-1);
	vector<Node> vNodesInserting;
	vNodes.back().content.substr(0,len-1);
	while(vNodes.size()>0 && vNodes.back().content.substr(0,len-1) == sTarget) //collect the nodes that have the same "check"
	{
		vNodesInserting.push_back(vNodes.back());
		vNodes.pop_back();
	}
	InsertBase(vNodesInserting);//insert all nodes that have the same "check"
}
//-----------------------------------------
int main()
{
	string dicPath = "D:\\Dictionary.txt";
	cout<<"Please check dictionary path: "<<dicPath<<endl;  
	system("pause");

	//define
	locale::global(locale(""));	
	wifstream stream(dicPath);
	stream.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
	int countline = count(istreambuf_iterator<char>(ifstream(dicPath)), istreambuf_iterator<char>(), '\n');
	vector<wstring> vWords; // collect words from dictionary
	vector<wstring>::iterator itws;
	vector<Node>::iterator itnd;
	int maxLen = INT_MIN;

	ResizingDAT(65530);//init to unicode size
	vWords.resize(countline+1);// resizing to possible space needed

	itws = vWords.begin();
	wstring line;
	while (getline(stream, line))
	{
		int len = line.length();

		if(len==0)
			continue;

		if(len>maxLen)
			maxLen = len;

		transform(line.begin(), line.end(), line.begin(), ::towlower);
		*itws++=line; // collect words from dictionary
	}

	for(int i=vWords.size()-1;i>=0;i--) // resizing to exact space
	{
		if(vWords[i].length()!=0)
		{
			vWords.resize(i+1);
			break;
		}
	}

	sort(vWords.begin(), vWords.end(),CompareStringLenReverse());//sorting as reverse length order(inserting order: the shorter the prior, so pop-back the shortest for vector efficiency)


	//group insert every words from dictionay to double-array trie (faster then insert one-by-one)
	for(int k=1; k<=maxLen; k++) //start from 1 not minLen, because middle node always start from length 1
	{
		cout<<"constructing trie, remaining words: "<<vWords.size()<<endl;

		vector<Node> vNodes;

		int count = vWords.size()-1;
		for(int i=count;i>=0;i--)
		{
			bool bIsCase = (vWords[i].length()==k);

			Node node = {
				vWords[i].substr(0,k),
				bIsCase,    // "Node.bIsCase" indicate whether string is a completely word or a middle node
			};

			vNodes.push_back(node);

			if(bIsCase)
				vWords.pop_back();
		}

		//remove same strings, leave "bIsCase" as true if has
		sort(vNodes.begin(), vNodes.end(),CompareNodeReverse());
		for(itnd=vNodes.begin() ; itnd!=vNodes.end()-1 ; itnd++)
		{
			if((itnd+1)->content==itnd->content)
			{
				(itnd)->content=L"";
				(itnd+1)->bIsCase=((itnd+1)->bIsCase!=itnd->bIsCase);
			}
		}

		//resizing to exact space
		sort(vNodes.begin(), vNodes.end(),CompareNodeReverse());
		for(int i=vNodes.size()-1;i>=0;i--)
		{
			if(vNodes[i].content!=L"")
			{
				vNodes.resize(i+1);
				break;
			}
		}

		while(vNodes.size() != 0) //group insert all the nodes that have same target id
			InsertGroup(vNodes,k);

		vNodes.clear();
	}
	//print();

	wstring newWord; //insert word
	wstring input;   //input for filter, will replaced as output
	wstring inputL;  //lower case of "input"
	while(true)
	{
		cout<<endl<<"請輸入新增字囊"<<endl;
		getline(wcin, newWord);
		if(newWord.length()>0) 
		{
			transform(newWord.begin(), newWord.end(), newWord.begin(), ::towlower);
			InsertSingle(newWord);
		}


		cout<<endl<<"請輸入過濾對話"<<endl;
		getline(wcin, input);
		int searchTime=0; 
		if(input.length()>0)
		{
			//timer start
			LARGE_INTEGER nFreq,nBeginTime,nEndTime;
			QueryPerformanceFrequency(&nFreq);
			QueryPerformanceCounter(&nBeginTime);

			inputL = input;
			transform(inputL.begin(), inputL.end(), inputL.begin(), ::towlower);

			int base = inputL[0]+abs(m_dat[0].base);
			int base_pre = 0;
			int start = 0;
			int failId = 0;
			
			for(int n = 0; n < inputL.length();)
			{
				if(base>m_dat.size()-1)
					break;
				
				int nMatchLen=-1;
				while(base_pre ==m_dat[base].check  &&  m_dat[base].content.length()>0   &&  inputL[n] == m_dat[base].content.back()) //search trie
				{
					searchTime++;

					if(m_dat[base].base<0)
						nMatchLen = m_dat[base].content.length();

					base_pre = base;

					if(inputL.length()-1 < n+1)
						break;

					base=abs(m_dat[base].base)+inputL[n+1];
					n++;
				}
				searchTime++;
				//----------------- here: inputL[n]       is the char that terminate current trie search  ex: hell"z" 
				//----------------- here: inputL[start]   is the current first char(a root-child),        ex: "h"ello
				//----------------- here: m_dat[base_pre] is the deepest node that can reach before terminate  ex: hel"l"
				
				//replace if have found
				if(nMatchLen>0)
				{
					input.replace(start, nMatchLen, wstring(nMatchLen, L'*'));
					if(input.length()==start+nMatchLen) // //end
						break;
				}
				
				
				if(start>inputL.length()-1)//end
					break;

				failId = m_dat[base_pre].failId;
				int failStrLen = m_dat[failId].content.length();
				if(failStrLen==0) //no fail node
				{
					if(base == inputL[n]+abs(m_dat[0].base)) //char inputL[n] not start char, skip it.
						n++;

					base_pre = 0;			      //start from new char
					base = inputL[n]+abs(m_dat[0].base); //start from new char
					start = n;			      //start from new char
				}
				else //move to fail node
				{
					base_pre = failId;
					base = abs(m_dat[failId].base)+inputL[n];
					start += (m_dat[base_pre].content.length()- failStrLen);
				}

			}

			cout<<endl<<"搜尋次數:"<<searchTime<<endl;
			cout<<endl<<"過濾後:"<<endl;
			wcout<<input<<endl;

			//timer end
			QueryPerformanceCounter(&nEndTime);
			double timeT = (double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
			printf("\n耗時:%f\n",timeT);
		}
	}
	return 0;
}
