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

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
class ExampleFilter
{
public: 
	//------------------------------------------------
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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
class DAT_ACM
{
public:
	//------------------------------------------------
	void print(wstring ws=L"")   //for debug
	{
		wofstream  myfile;
		myfile.open("D:\\MyLog.txt", fstream::app);
		wstring_convert<codecvt_utf8<wchar_t>> myconv;
		if(ws!=L"")
			myfile<<L"Insert:\t"<<ws<<endl;
		myfile<<"ID"<<"\t"<<"base"<<"\t"<<"check"<<"\t"<<"failId"<<"\t"<<"failFrom"<<"\t"<<"content"<<endl;
		for(int i=0;i<m_dat.size();i++)
		{
			if(m_dat[i].base!=0 ||
				m_dat[i].check!=0 ||
				m_dat[i].failId!=0 ||
				m_dat[i].failFrom!=0 ||
				m_dat[i].content.length() !=0 
				)
			{
				myfile<<i<<"\t";
				myfile<<m_dat[i].base<<"\t";
				myfile<<m_dat[i].check<<"\t";
				myfile<<m_dat[i].failId<<"\t";
				myfile<<m_dat[i].failFrom<<"\t";
				myfile<<m_dat[i].content<<"\t";
				myfile<<endl;
			}
		}
		myfile.close();
	}
	//------------------------------------------------
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

		int failFrom;//"hello" fail to "llo",  failFrom = 2

		wstring content;
	};
	//------------------------------------------------
	typedef struct Node
	{
		wstring content;
		bool bIsWord;

		bool operator==(const Node& a) const
		{
			return ( a.bIsWord==bIsWord && a.content ==content);
		}
	};
	//------------------------------------------------
	vector<DAT>  m_dat;
	vector<Node>  m_SortedDic;
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
				m_dat[m].check=0;
				m_dat[m].failId=0;
				m_dat[m].failFrom=0;
				m_dat[m].content=L"";
			}  m_dat[0].check=-1;
		}
	}
	//------------------------------------------------
	DAT_ACM()
	{
		ResizingDAT(65530);
	}
	//------------------------------------------------
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
				return -1;
		}
	}
	//------------------------------------------------
	void InsertSingle(wstring str)// for single insert
	{
		int id = Search(str);

		if(id>0)//mid node or case
			return;

		for(int i=0;i<m_dat.size();i++)
		{
			m_dat[i].base=0;
			m_dat[i].check=0;
			m_dat[i].failId=0;
			m_dat[i].failFrom=0;
			m_dat[i].content=L"";
		}  m_dat[0].check=-1;


		for(int k=1;k<str.length()+1;k++)
		{
			Node node = {
				str.substr(0,k),
				false, 
			};
			m_SortedDic.push_back(node);
		}m_SortedDic.back().bIsWord=true;

		sort(m_SortedDic.begin(), m_SortedDic.end(),CompareNodeReverse());
		
		m_SortedDic.erase( unique( m_SortedDic.begin(), m_SortedDic.end() ), m_SortedDic.end() );

		vector<Node> m_SortedDic_Copy(m_SortedDic);

		while(m_SortedDic_Copy.size() != 0)
			InsertGroup(m_SortedDic_Copy);
	}
	//------------------------------------------------
	void InsertGroup(vector<Node> &vNodes)
	{
		int len = vNodes.back().content.length();
		wstring sTarget = vNodes.back().content.substr(0,len-1);
		
		vector<Node> vNodesInsert;
		while(
			vNodes.size()>0 && 
			vNodes.back().content.length() == len && 
			vNodes.back().content.substr(0,len-1) == sTarget) //collect the nodes that have the same "check"
		{
			vNodesInsert.push_back(vNodes.back());
			vNodes.pop_back();
		}

		wstring ws = vNodesInsert[0].content;   ws.pop_back();
		int nTarget = Search(ws,true);

		int *ids = new int[vNodesInsert.size()]; 
		for(int i=0;i<vNodesInsert.size();i++)
			ids[i] = vNodesInsert[i].content.back();
		
		for(int k=1;k<m_dat.size();k+=rand()/100+1)//if k=1 not fit, then random move forward to fit
		{
			int i=0;
			for(;i<vNodesInsert.size();i++) //try to fit vNodesInsert one by one
			{
				ResizingDAT(k+ids[i]);
				if(m_dat[k+ids[i]].content.length()!=0)
					break;        //not fit, continue loop
			}
			if(i!=vNodesInsert.size())  //not fit, continue loop
				continue;

			//fit ok, base is k
			if(m_dat[nTarget].base<0)   //if is end char
				m_dat[nTarget].base=-k;
			else
				m_dat[nTarget].base=k;

			for(int n=0;n<vNodesInsert.size();n++) //inserting
			{
				int id = k+ids[n];

				m_dat[id].check=nTarget;
				m_dat[id].content=vNodesInsert[n].content;

				wstring temp = vNodesInsert[n].content;

				m_dat[id].failId=0;
				if(temp.length()>0)
				{
					for(int i=1;i<temp.length();i++)
					{
						for(int j=temp.length()-i;j>0;j--)
						{
							int failId = Search(temp.substr(i,j),true);  //ex:"hello"    temp.substr(i,j) -->  "ello","ell","el","e","llo","lo", ...
							if(failId>0)
							{
								if(m_dat[id].failId == 0) //about == 0, if found node, only assing longest length if there's no exact word, ex:"ello" 
								{
									m_dat[id].failId = failId;
									m_dat[id].failFrom = i;
								}

								if(m_dat[failId].base<0) // assing longest exact word if found
								{
									m_dat[id].failId = failId;
									break;
								}
							}
						}
						if(m_dat[id].failId>0) // only assign fail case that is closest to first char, ex: "e..."
							break;
					}
				}

				if(vNodesInsert[n].bIsWord)
					m_dat[id].base=-id;
				else
					m_dat[id].base=id;
			}
			break;
		}
		delete ids;
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

				Node node = {
					vWords[i].substr(0,k),
					bIsWord,    // "Node.bIsWord" indicate whether string is a completely word or a middle node
				};

				vNodes.push_back(node);

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
		
		sort(vNodes.begin(), vNodes.end(),CompareNodeReverse());

		m_SortedDic = vNodes;
		
		while(vNodes.size() != 0) 
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
	void FilterDialog(wstring input=L"",wstring example=L"")
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
				}
				
				//replace if have found
				if(nMatchLen>0)
				{
					input.replace(From, nMatchLen, wstring(nMatchLen, L'*'));

					if(From+nMatchLen == input.length())
						break;

					nMatchLen=-1;
				}

				failId = m_dat[base_pre].failId;

				DAT dat_now = m_dat[base_pre];
				DAT dat_fail =  m_dat[failId];

				if(failId==0) //no fail node
				{
					//assing From
					From++;
					
					//assing To
					To = From+1; //"To" must greater than "From" for "in_cpy.substr(From,To-From)"

					if(From>=in_cpy.length())
					{
						if(example != input && example!=L"")
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
				else //move to fail node
				{
					//assing To
					To -= (
						dat_now.content.length()  - (dat_now.failFrom  + dat_fail.content.length()) 
						);
					

					if(To>in_cpy.length())
					{
						while(true) //ending check.  ex: "good to see David" ~~  "David",is word? -> fail to "vid",is word? -> ...
						{
							From += dat_now.failFrom;
							if(dat_fail.base<0)
							{
								nMatchLen = dat_fail.content.length();
								input.replace(From, nMatchLen, wstring(nMatchLen, L'*'));

								if(From+nMatchLen == input.length())
									break;
							}
							if(dat_fail.failId>0)
							{
								dat_now = dat_fail;
								dat_fail = m_dat[dat_fail.failId];
							}
							else
								break;
						}

						if(example != input && example!=L"")
							bTestOK = false;
						break; 
					}
					
					//assing From
					From += dat_now.failFrom;
					if(dat_now.failFrom<1)
						From++;
					
					//if itself is word before add in_cpy[To-1]
					if(dat_fail.base<0)
						nMatchLen = dat_fail.content.length();

					//assign two bases
					base = abs(dat_fail.base)+in_cpy[To-1];
					base_pre = failId;
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
				system("pause");
			}
		}
		return;
	}
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
wstring RandomString(int minLen, int maxLen, wstring charPool)
{
	size_t length = rand()%maxLen+minLen;
	static const wstring alphabet = charPool;
	static default_random_engine rng( time(nullptr) ) ;
	static uniform_int_distribution<size_t> distribution( 0, alphabet.size() - 1 ) ;
	wstring str ;
	while( str.size() < length ) str += alphabet[ distribution(rng) ] ;
	return str;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void main()
{
	//--------------------------------------------- test setting
	int testWords = 20;
	int testCount = 1000;
	int maxWordLen = 5;
	int maxDialogLen = 30;
	wstring dialogPool = L"@abcdefghijklmnopqrstuvwxyzABCDEFGHIJKL";
	wstring wordPool = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKL";
	//--------------------------------------------- test case start
	vector<wstring> vWords;
	for(int i=0;i<testWords;i++)
		vWords.push_back(RandomString(1,maxWordLen,wordPool));//////////////
	
	ExampleFilter *exa_test = new ExampleFilter();
	DAT_ACM       *dat_test = new DAT_ACM();

	exa_test->m_vDictionary = vWords; //default dictionay 
	dat_test->AddDicBase(vWords);     //default dictionay 

	for(int i=0;i<testCount;i++)
	{
		wstring randomDialog = RandomString(1,maxDialogLen,dialogPool);

		wstring wsExample = exa_test->FilterDialog(randomDialog);
		/*                */dat_test->FilterDialog(randomDialog,wsExample);

		wstring newWord = RandomString(3,maxWordLen,wordPool);//////////////
		
		exa_test->m_vDictionary.push_back(newWord);//insert word
		dat_test->InsertSingle(newWord);           //insert word
	}

	delete exa_test;
	delete dat_test;
	//--------------------------------------------- test case over


	//--------------------------------------------- main case start
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
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
