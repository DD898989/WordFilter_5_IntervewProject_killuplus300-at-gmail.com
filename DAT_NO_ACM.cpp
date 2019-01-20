#include "stdafx.h"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <windows.h>
#include <codecvt>
#include <fstream>
#include <random>
using namespace std;
//------------------------------------------------------------------------------------------------------------------------------------------------
class ExampleFilter
{
public: 
	//------------------------------------------------
	vector<wstring> m_vDictionary;
	//------------------------------------------------
	wstring FilterDialog(wstring wsInput)
	{
		locale::global(locale(""));
		int* aryMark = new int[wsInput.size()]();

		for(int n = 0; n<m_vDictionary.size(); n++)
		{
			wstring nCurrentWord = m_vDictionary[n];
			size_t start_pos = 0;
			while((start_pos = wsInput.find(m_vDictionary[n], start_pos)) != string::npos)
			{
				for(int i =0 ;i<m_vDictionary[n].length();i++)
					aryMark[start_pos+i]=1;
				start_pos += 1;//from.length(); 
			}
		}

		for(int i=0;i<wsInput.length();i++)
			if(aryMark[i])
				wsInput[i] = L'*';
		delete aryMark;
		return wsInput;
	}
	//------------------------------------------------
};
//------------------------------------------------------------------------------------------------------------------------------------------------
class DAT_ACM
{
public:
	//------------------------------------------------
	typedef struct DAT
	{
		//int id;  //equal to array index


		wstring content;


		int base; 
		//base<0: whole word,might also be mid node      negative base most <=-33 because first visible ascii code is 32(space)
		//base>0: mid node   
		//base=0: empty node

		int parent;
		//parent=-1: root, len=0      
		//parent= 0: root-child, len=1    or   empty node   
		//parent> 0: can't define
		
		vector<int> children;
	};
	//------------------------------------------------
	vector<DAT>  m_dat;
	//------------------------------------------------
	void PrintTrie(wstring ws=L"")   //for debug
	{
		wofstream  myfile;
		myfile.open("D:\\MyLog.txt", fstream::app);
		wstring_convert<codecvt_utf8<wchar_t>> myconv;
		myfile<<endl<<endl<<endl;
		if(ws!=L"")
			myfile<<L"Insert:\t"<<ws<<endl;

		myfile
			<<"ID"<<"\t"
			<<"base"<<"\t"
			<<"parent"<<"\t"
			<<"content"<<"\t"
			<<"children"
			<<endl;

		for(int i=0;i<m_dat.size();i++)
		{
			if(m_dat[i].base!=0 ||
				m_dat[i].parent!=0 ||
				m_dat[i].content.length() !=0 
				)
			{
				myfile<<i<<"\t";
				myfile<<m_dat[i].base<<"\t";
				myfile<<m_dat[i].parent<<"\t";
				myfile<<m_dat[i].content<<"\t";
				for (vector<int>::iterator it=m_dat[i].children.begin(); it != m_dat[i].children.end(); ++it){myfile<<*it<<",";}	myfile<<"\t";
				myfile<<endl;
			}
		}
		myfile.close();
	}
	//------------------------------------------------
	typedef struct Node
	{
		wstring content;
		bool bIsWord;

        Node(wstring ws, bool b) : content(ws), bIsWord(b) { }
		Node(){}

		bool operator==(const Node& a) const
		{
			return ( a.bIsWord==bIsWord && a.content ==content);
		}
	};
	//------------------------------------------------
	struct CompareNodeReverse
	{
		bool operator()(const Node& first, const Node& second) 
		{
			if(first.content.length()==second.content.length())
				if(first.content != second.content)
					return first.content < second.content; 
				else
					return first.bIsWord;
			else
				return first.content.length() > second.content.length(); 
		}
	};
	//------------------------------------------------
	struct CompareStringLenReverse
	{
		bool operator()(const wstring& first, const wstring& second) 
		{
			return first.length() > second.length(); 
		}
	};
	//------------------------------------------------
	void ResizingDAT(int reach)
	{
		int Old = m_dat.size();
		if(reach>Old-1)
		{
			int New = reach;
			New |= New >> 1;  New |= New >> 2;  New |= New >> 4;  New |= New >> 8;  New |= New >> 16;
			New = (New < 0) ? 1 :  New + 1;

			cout<<"resizing array..."<<Old<<" to "<<New<<endl;
			m_dat.resize(New);

			for(int m=Old;m<m_dat.size();m++)
			{
				m_dat[m].base=0;
				m_dat[m].parent=0;
				m_dat[m].content=L"";
			}  m_dat[0].parent=-1;
		}
	}
	//------------------------------------------------
	DAT_ACM()
	{
		ResizingDAT(0XFFFF);//0XFFFF=65535
	}
	//------------------------------------------------
	int Search(const wstring& str, bool bFindNode = false, bool bFindNearestParent = false) //return id or -1 
	{
		int base=str[0]+abs(m_dat[0].base);
		int base_pre=0;

		for(int j=0;j<str.length();j++)
		{
			if(base>m_dat.size()-1)
				return (bFindNearestParent) ? base_pre : -1;

			if(base_pre ==m_dat[base].parent  &&   m_dat[base].base!=0/*for first char*/  &&   str[j] ==m_dat[base].content.back())
			{
				if(j==str.length()-1)
				{
					if(m_dat[base].base<0 || bFindNode) 
						return base;
					else
						return (bFindNearestParent) ? base_pre : -1;
				}

				base_pre = base;
				base=abs(m_dat[base].base)+str[j+1];
			}
			else
				return (bFindNearestParent) ? base_pre : -1;
		}

		return base_pre;// str length = 0
	}
	//------------------------------------------------
	void InsertSingle(wstring str)// for single insert
	{
		int id = Search(str,true,true);
		vector<Node>  vReInsert;

		if(id>0)
		{
			if(m_dat[id].content.length() == str.length())
		{
				if(m_dat[id].base<0)
				{
					return;//is word
		}
				else
		{
					//assign: base
			m_dat[id].base = -m_dat[id].base;
			return;
		}
			}
		}
		
		int startFrom = m_dat[id].content.length();
		for(int k=1+startFrom;k<str.length()+1;k++)
			vReInsert.push_back(Node(str.substr(0,k),false));
		vReInsert.back().bIsWord=true;
		
		RecursiveMove(id,false,vReInsert);

		//assign: all
		InsertGroup(vReInsert);

	}
	//------------------------------------------------
	void RecursiveMove(int id, bool moveSelf,vector<Node>  &vReInsert)
	{
		for (vector<int>::iterator it=m_dat[id].children.begin(); it != m_dat[id].children.end(); ++it)
		{
			RecursiveMove(*it,true,vReInsert);
		}

		if(moveSelf)
		{
			vReInsert.push_back(Node(m_dat[id].content,m_dat[id].base<0));


			m_dat[id].base=0;
			m_dat[id].parent=0;
			m_dat[id].children.clear();
			m_dat[id].content=L"";
		}
	}
	//------------------------------------------------
	void InsertGroup(vector<Node> &vNodes)
	{
		sort(vNodes.begin(), vNodes.end(),CompareNodeReverse());

		while(vNodes.size() != 0)
		{
		int len = vNodes.back().content.length();
		wstring sTarget = vNodes.back().content.substr(0,len-1);
		
		vector<Node> vNodesInsert;
		while(
			vNodes.size()>0 && 
			vNodes.back().content.length() == len && 
				vNodes.back().content.substr(0,len-1) == sTarget) //collect the nodes that have the same "parent"
		{
			vNodesInsert.push_back(vNodes.back());
			vNodes.pop_back();
		}

		wstring ws = vNodesInsert[0].content;   ws.pop_back();
		int nTarget = Search(ws,true);

			vector<int> ids;
		for(int i=0;i<vNodesInsert.size();i++)
				ids.push_back(vNodesInsert[i].content.back());
		

			int k=1;
			while(true)
		{
			int i=0;
			for(;i<vNodesInsert.size();i++) //try to fit vNodesInsert one by one
			{
				ResizingDAT(k+ids[i]);
				if(m_dat[k+ids[i]].content.length()!=0)
					{
						k+=rand()/100+1;//if k=1 not fit, then random move forward to fit
						break;
					}
			}
				if(i==vNodesInsert.size())
					break;
			}



			m_dat[nTarget].children.clear();

			//assign: base
			m_dat[nTarget].base = (m_dat[nTarget].base<0) ? -k : k;

			for(int n=0;n<vNodesInsert.size();n++) //inserting
			{
				int id = k+ids[n];

				//assign: parent
				m_dat[id].parent=nTarget;

				//assign: content
				m_dat[id].content=vNodesInsert[n].content;

				//assign: children
				if(nTarget>=0)
				{
					vector<int> temp = m_dat[nTarget].children;
					if (find(temp.begin(), temp.end(), id) == temp.end())
						m_dat[nTarget].children.push_back(id);
				}


				//assign: base
				m_dat[id].base = (vNodesInsert[n].bIsWord) ? -id : id;
			}
		}
	}
	//------------------------------------------------
	void AddDicBase(vector<wstring> vWords)
	{
		sort( vWords.begin(), vWords.end() );
		vWords.erase( unique( vWords.begin(), vWords.end() ), vWords.end() );

		sort(vWords.begin(), vWords.end(),CompareStringLenReverse());//sorting as reverse length order(inserting order: the shorter the prior, so pop-back the shortest for vector efficiency)
		int maxLen = vWords.front().size();

		//group insert every words from dictionay to double-array trie (faster then insert one-by-one)
		vector<Node> vNodes;
		for(int k=1; k<=maxLen; k++) //start from 1 not minLen, because middle node always start from length 1
		{
			//cout<<"constructing trie, remaining words: "<<vWords.size()<<endl;
			int count = vWords.size()-1;
			for(int i=count;i>=0;i--)
			{
				bool bIsWord = (vWords[i].length()==k);

				vNodes.push_back(Node(vWords[i].substr(0,k),bIsWord));


				if(bIsWord)
					vWords.pop_back();
			}
		}
		
		//resizing to exact space
		for(int i=vNodes.size()-1;i>=0;i--)
		{
			if(vNodes[i].content!=L"")
			{
				vNodes.resize(i+1);
				break;
			}
		}
		
		InsertGroup(vNodes);
	}
	//------------------------------------------------
	void AddDicFromFile(string dicPath)
	{
		//define
		locale::global(locale(""));	
		wifstream stream(dicPath);
		stream.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
		int countline = count(istreambuf_iterator<char>(ifstream(dicPath)), istreambuf_iterator<char>(), '\n');
		vector<wstring> vWords; // collect words from dictionary
		vector<wstring>::iterator itws;

		vWords.resize(countline+1);// resizing to possible space needed
		itws = vWords.begin();
		wstring line;
		while (getline(stream, line))
		{
			int len = line.length();

			if(len==0)
				continue;

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

		AddDicBase(vWords);
	}
	//------------------------------------------------
	void AddNewWord()
	{
		wstring newWord; //insert word
		cout<<endl<<"請輸入新增字囊"<<endl;
		getline(wcin, newWord);
		if(newWord.length()>0) 
			InsertSingle(newWord);
	}
	//------------------------------------------------
	void FilterDialog(wstring input=L"",wstring example=L"",vector<wstring> m_vDictionary = vector<wstring>())
	{
		if(input==L"")
		{
			cout<<endl<<"請輸入過濾對話"<<endl;
			getline(wcin, input);
		}

		if(input.length()>0)
		{
			wstring in_cpy = input;
			//-----------------------
			//-----------------------
			//-----------------------
			int base = in_cpy[0]+abs(m_dat[0].base);
			int base_pre = 0, From = 0, To = 1, failId = 0, nMatchLen = -1;
			bool bTestOK=true;

			while(true)
			{
				if(base > m_dat.size()-1)
					break;

				while(in_cpy.substr(From,To-From) == m_dat[base].content) //search trie
				{
					if(m_dat[base].base<0)
						nMatchLen = m_dat[base].content.length();

					base_pre = base;

					if(To>=in_cpy.length())
					{
						To++;
						break;
					}

					base=abs(m_dat[base].base)+in_cpy[To];
					To++;

					if(base > m_dat.size()-1)
						break;
				}
				
				//replace if have found
				if(nMatchLen>0)
				{
					input.replace(From, nMatchLen, wstring(nMatchLen, L'*'));

					if(From+nMatchLen == input.length())
						break;

					nMatchLen=-1;
				}
					From++;
					
					//assing To
					To = From+1; //"To" must greater than "From" for "in_cpy.substr(From,To-From)"

					if(From>=in_cpy.length())
					{
						if(example!=L""  && example != input)
							bTestOK = false;
						break;
					}
					else
					{
						//assing two bases
						base_pre = 0;
						base = in_cpy[From]+abs(m_dat[0].base); //From from new char
					}
			}

			if(bTestOK)// test ok  OR  not testing now
			{
				cout<<endl<<"過濾後:"<<endl;
				wcout<<input<<endl;
			}
			else
			{
				cout<<"\n\ntest fail, pls check:"<<endl;
				wcout<<L"input  : "<<in_cpy  <<endl;
				wcout<<L"Example: "<<example <<endl;    
				wcout<<L"DAT    : "<<input   <<endl;

				
				if(m_vDictionary.size()>0)
				{
				wofstream  myfile;
				myfile.open("D:\\MyLog.txt", fstream::app);
					for (vector<wstring>::const_iterator i = m_vDictionary.begin(); i != m_vDictionary.end(); ++i)
					myfile << *i << endl;
				myfile.close();
				}

				system("pause");
			}
		}
		return;
	}
	//------------------------------------------------
};
//------------------------------------------------------------------------------------------------------------------------------------------------
wstring RandomString(int minLen, int maxLen,  wstring charPool)
{
	size_t len = rand()%maxLen+minLen;
    wstring ustr=L"";
	int range = charPool.length()-1;//0-base

    srand(time(NULL));
    for (auto i = 0; i < len; i++) 
	{
		random_device rd;
		mt19937 eng(rd());
		uniform_int_distribution<> distr(0,range);
		int rdn = distr(eng);
        ustr += charPool[rdn];
    }
    return ustr;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
void main()
{
	if(true)//TRUE for test,  if test fail, change to FALSE to degug
	{
		//--------------------------------------------------------------------------------------------------------- test setting
		int testWords = 200;
		int testCount = 10000;
		int maxWordLen = 8;
		int maxDialogLen = 110;
		wstring dialogPool = L"@abcdefghijklmnopq，測試";
		wstring wordPool = L"abcdefghijklmnopq";
		//--------------------------------------------------------------------------------------------------------- test case start
		vector<wstring> vWords;
		for(int i=0;i<testWords;i++)
		{
			wstring temp = RandomString(1,maxWordLen,wordPool);
			vWords.push_back(temp);//////////////
		}

		ExampleFilter *exa_test = new ExampleFilter();
		DAT_ACM       *dat_test = new DAT_ACM();

		exa_test->m_vDictionary = vWords; //default dictionay 
		dat_test->AddDicBase(vWords);     //default dictionay 

		for(int i=0;i<testCount;i++)
		{
			//cout<<i<<",";
			wstring randomDialog = RandomString(1,maxDialogLen,dialogPool);

			wstring wsExample = exa_test->FilterDialog(randomDialog);
			/*                */dat_test->FilterDialog(randomDialog,wsExample,exa_test->m_vDictionary);

			wstring newWord = RandomString(3,maxWordLen,wordPool);//////////////

			exa_test->m_vDictionary.push_back(newWord);//insert word
			dat_test->InsertSingle(newWord);           //insert word
		}
		delete exa_test;
		delete dat_test;
		//--------------------------------------------------------------------------------------------------------- test case over
	}
	else
	{
		//--------------------------------------------------------------------------------------------------------- debug case start
		ExampleFilter *exa_debug = new ExampleFilter();
		DAT_ACM       *dat_debug = new DAT_ACM();


		wifstream infile("D:\\MyLog.txt");
		wstring line;
		while (getline(infile, line))
		{
			exa_debug->m_vDictionary.push_back(line);//insert word
			dat_debug->InsertSingle(line);           //insert word
			//dat_debug->PrintTrie(line);//set break point
		}

		wstring randomDialog = L"O";
		wstring wsExample = exa_debug->FilterDialog(randomDialog);
		/*                */dat_debug->FilterDialog(randomDialog,wsExample);
		delete exa_debug;
		delete dat_debug;
		//--------------------------------------------------------------------------------------------------------- debug case over
	}


	//------------------------------------------------------------------------------------------------------------- main case start
	string dicPath = "D:\\Dictionary.txt";
	cout<<"Please check dictionary path: "<<dicPath<<endl;  
	system("pause");

	DAT_ACM *dat = new DAT_ACM();
	dat->AddDicFromFile(dicPath);

	while(true)
	{
		dat->AddNewWord(); 
		dat->FilterDialog();
	}
	delete dat;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
