#include "stdafx.h"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <windows.h>
#include <codecvt>
#include <fstream>
#include <regex>
#include <stdlib.h>
using namespace std;
//-----------------------------------------
typedef struct DAT
{
	int base;
	int check;
	wstring content;
};
//-----------------------------------------
typedef struct node
{
	wstring WS;
	bool IsEndChar;
};
//-----------------------------------------
vector<DAT>  m_dat;
vector<node> m_ReInsertString;
//-----------------------------------------
string wstring_to_utf8 (const wstring& str)
{
	wstring_convert<codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}
//-----------------------------------------
void print()  //for debug
{
	ofstream  myfile;
	myfile.open("D:\\MyLog.txt", fstream::app);
	for(int i=1;i<m_dat.size();i++)
	{
		if(m_dat[i].content.length()>0)
		{
			myfile<<i<<"\t";
			myfile<<m_dat[i].base<<"\t";
			myfile<<m_dat[i].check<<"\t";
			myfile<<wstring_to_utf8(m_dat[i].content)<<"\t";
			myfile<<endl;
		}
	}
	myfile.close();
}
//-----------------------------------------
int tableSizeFor(int cap) 
{
	int n = cap - 1;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	return (n < 0) ? 1 :  n + 1;
}
//-----------------------------------------
void recursive_reset(int id)
{
	for(int i=0;i<m_dat.size();i++)
	{
		if(m_dat[i].check==id)
		{
			node *record = new node;
			record->WS=m_dat[i].content;

			if(m_dat[i].base<0)
				record->IsEndChar=true;
			else
				record->IsEndChar=false;

			m_ReInsertString.push_back(*record);
			delete record;
			m_dat[i].check=0;
			m_dat[i].base=0;
			m_dat[i].content=L"";
			recursive_reset(i);
		}
	}
}
//-----------------------------------------
struct compareNodeString
{
	bool operator()(const node& first, const node& second) 
	{
		return first.WS > second.WS; 
	}
};
//-----------------------------------------
struct compareNodeStringLen
{
	bool operator()(const node& first, const node& second) 
	{
		return first.WS.length() < second.WS.length(); 
	}
};
//-----------------------------------------
struct compareStringLen
{
	bool operator()(const wstring& first, const wstring& second) 
	{
		return first.length() > second.length(); 
	}
};
//-----------------------------------------
void Expand(vector<DAT> &dat_, int base)
{
	int ori_datsize = dat_.size();
	if(base>ori_datsize-1)
	{
		cout<<"resizing array..."<<endl;
		dat_.resize(tableSizeFor(base+1));
		for(int m=ori_datsize;m<m_dat.size();m++)
		{
			dat_[m].base=0;
			dat_[m].check=0;
			dat_[m].content=L"";
		}
	}
}
//-----------------------------------------
int Search(wstring str, bool bExactWord) //return room id or -1
{
	int base=str[0]+abs(m_dat[0].base);
	for(int j=0;j<str.length();j++)
	{
		Expand(m_dat,base);
		if(str.substr(0,j+1) ==m_dat[base].content)
		{
			if(j==str.length()-1)
			{
				if(m_dat[base].base<0)
				{
					return base;
				}
				else
				{
					if(bExactWord)
						return -1;
					else
						return base;
				}
			}
			base=abs(m_dat[base].base)+str[j+1];
		}
		else
		{
			return -1;
		}
	}
}
//-----------------------------------------
int Get_LandLordID_From_Tenant(wstring ws)
{
	if(ws.length()==1)
		return 0;

	wstring LandLord=ws.substr(0,ws.size()-1);

	return Search(LandLord,false);
}
//-----------------------------------------
 void InsertBase(vector<node> &vNodes)
{
	int nLandLordID = Get_LandLordID_From_Tenant(vNodes[0].WS);
	
	int *ids = new int[vNodes.size()]; 

	for(int i=0;i<vNodes.size();i++)
		ids[i] = vNodes[i].WS[vNodes[i].WS.size()-1];

	for(int k=1;k<m_dat.size();k+=rand()/100+1)
	{
		int i=0;
		for(;i<vNodes.size();i++)
		{
			Expand(m_dat,k+ids[i]);
			if(m_dat[k+ids[i]].content.length()!=0)
				break;        //continue loop
		}
		if(i!=vNodes.size())  //continue loop
			continue;
		
		if(m_dat[nLandLordID].base<0)   //if is end char
			m_dat[nLandLordID].base=-k;
		else
			m_dat[nLandLordID].base=k;

		for(int n=0;n<vNodes.size();n++)
		{
			m_dat[k+ids[n]].check=nLandLordID;
			m_dat[k+ids[n]].content=vNodes[n].WS;

			if(vNodes[n].IsEndChar)
				m_dat[k+ids[n]].base=-(k+ids[n]);
			else
				m_dat[k+ids[n]].base=0;

		}
		break;
	}
	delete ids;
}
//-----------------------------------------
void Insert(wstring str)
{
	if(str.length()<=1)
		return;

	if(Search(str,true)!=-1)
		return;

	int base=str[0]+abs(m_dat[0].base);
	int base_pre=0;

	for(int i=0;i<str.length();i++)
	{
		Expand(m_dat,base);

		if(i!=str.length()-1 && base>0 && str.substr(0,i+1) ==m_dat[base].content)
		{
		}
		else
		{
			m_ReInsertString.clear();   
			recursive_reset(base_pre);
			wstring wsTemp = str; 

			for(int m=i;m<str.length();m++)
			{
				node *record = new node;

				record->WS=wsTemp;

				if(m==i)
					record->IsEndChar=true;
				else
					record->IsEndChar=false;

				m_ReInsertString.push_back(*record);
				wsTemp=wsTemp.substr(0,wsTemp.size()-1);

				delete record;
			}
			break;
		}
		base_pre = base;
		base=m_dat[base].base+str[i+1];
	}

	compareNodeStringLen c1;
	sort(m_ReInsertString.begin(), m_ReInsertString.end(),c1);

	int m_nMinLenForReInsert=m_ReInsertString.front().WS.length();
	int m_nMaxLenForReInsert=m_ReInsertString.back().WS.length();

	compareNodeString c2;
	sort(m_ReInsertString.begin(), m_ReInsertString.end(),c2);

	vector<node> m_ReInsertStringForEachLen;
	for(int i=m_nMinLenForReInsert;i<m_nMaxLenForReInsert+1;i++)
	{
		m_ReInsertStringForEachLen.clear();

		for(int j=0;j<m_ReInsertString.size();j++)
		{
			if(m_ReInsertString[j].WS.length()==i)
			{
				m_ReInsertStringForEachLen.push_back(m_ReInsertString[j]);
				m_ReInsertString[j].WS=L"";
			}
			else if(m_ReInsertString[j].WS==L"" && m_ReInsertStringForEachLen.size()>0)
			{
				InsertBase(m_ReInsertStringForEachLen);
				m_ReInsertStringForEachLen.clear();
			}
		}

		if(m_ReInsertStringForEachLen.size()>0)
		{
			InsertBase(m_ReInsertStringForEachLen);
			m_ReInsertStringForEachLen.clear();
		}
	}
	
	m_ReInsertStringForEachLen.clear();
}
//-----------------------------------------
void GroupInsert(vector<node> &vNodes)
{
	wstring temp = vNodes.back().WS.substr(0,vNodes.back().WS.length()-1);
	vector<node> vNodesInserting;

	while(vNodes.size()>0 && vNodes.back().WS.substr(0,vNodes.back().WS.length()-1) == temp)
	{
		vNodesInserting.push_back(vNodes.back());
		vNodes.pop_back();
	}
	InsertBase(vNodesInserting);
		vNodesInserting.clear();
}
//-----------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	locale::global(locale(""));

	Expand(m_dat,65530);

	typedef codecvt_utf8<wchar_t> converter_type;	
	const converter_type* converter = new converter_type;
	string dicPath = "D:\\Dictionary.txt";
	cout<<"Please check dictionary path: "<<dicPath<<endl;  
	system("pause");
	wifstream stream(dicPath);
	stream.imbue(locale(locale::empty(), converter));

	vector<wstring> vWords; // collect words from dictionary
	ifstream inFile(dicPath);    
	int countline = count(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>(), '\n');
	vWords.resize(countline+1);// resizing to possible space needed
	
	wstring line;
	int count = -1;
	while (getline(stream, line))
	{
		if(line.length()<1)
			continue;

		count++;
		for(int i=0;i<line.length();i++) 
			line[i]= towlower(line[i]);
		vWords[count]=line; // collect words from dictionary
	}
	
	compareStringLen c;
	sort(vWords.begin(), vWords.end(), c);

	for(int i=vWords.size()-1;i>=0;i--) // resizing to exact space
	{
		if(vWords[i].length()!=0)
		{
			vWords.resize(i+1);
			break;
		}
	}
	
	int minLen = vWords.back().length();  //min len word in dictionary
	int maxLen = vWords.front().length(); //max len word in dictionary

	
	//insert every words from dictionay to double-array trie
	for(int k=1; k<=maxLen; k++) 
	{
		cout<<"Reading dictionary file, remaining: "<<vWords.size()<<endl;

		vector<node> vWords_Inserting;

		for(int i=vWords.size()-1;i>=0;i--)
		{
			node *record = new node;
			if(vWords[i].length()==k)
			{
				record->WS=vWords[i];     

				record->IsEndChar=true;   

				vWords_Inserting.push_back(*record);
				vWords.pop_back();
			}
			else
			{
				record->WS=vWords[i].substr(0,k);   

				record->IsEndChar=false;   

				vWords_Inserting.push_back(*record);
			}
			delete 	record;
		}

		compareNodeString c;
		sort(vWords_Inserting.begin(), vWords_Inserting.end(),c);

		for(int i=vWords_Inserting.size()-1;i>=1;i--)
		{
			if(vWords_Inserting[i].WS==vWords_Inserting[i-1].WS)
			{
				if( vWords_Inserting[i].IsEndChar!=vWords_Inserting[i-1].IsEndChar)
				{
						vWords_Inserting[i].WS=L"";
						vWords_Inserting[i-1].IsEndChar=true;
				}
				else
				{
						vWords_Inserting[i].WS=L"";
				}
			}
		}

		compareNodeString c1;
		sort(vWords_Inserting.begin(), vWords_Inserting.end(),c1);
		
		for(int i=vWords_Inserting.size()-1;i>=1;i--)
		{
			if(vWords_Inserting[i].WS!=L"")
			{
				vWords_Inserting.resize(i+1);
				break;
			}
		}

		while(vWords_Inserting.size() != 0)
			GroupInsert(vWords_Inserting);
		
		vWords_Inserting.clear();
	}

	wstring newWrod;
	while(true)
	{
		cout<<endl<<"請輸入新增字囊"<<endl;
		getline(wcin, newWrod);
		if(newWrod.length()>0) 
			Insert(newWrod);

		wstring seq;
		cout<<endl<<"請輸入過濾對話"<<endl;
		getline(wcin, seq);
		wstring seq_ = seq; //seq_ will convert to lower case, seq will replace as output

		//timer start
		LARGE_INTEGER nFreq,nBeginTime,nEndTime;
		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&nBeginTime);
		
		//covert input to lower case
		for(int i=0;i<seq_.length();i++) 
			seq_[i]= towlower(seq_[i]);
		
		//extract every whole dialog without any mark from input
		wregex rgx(L"\\w+");
		for( wsregex_iterator it(seq_.begin(), seq_.end(), rgx), it_end; it != it_end; ++it )
		{
			wstring line = (*it)[0];
			if(line.length()<minLen)
				continue;

			//extrct every possible word from dialog
			for(int wordLen=minLen;wordLen<=maxLen;wordLen++)
			{
				if(line.length()<wordLen)
					break;

				for(int i=0;i<line.length()+1-wordLen;i++)
				{
					wstring line0=line.substr(i,wordLen);

					if(line0.find(L"*") != string::npos)
						continue;

					if(Search(line0,true)>0)
					{
						wstring ws(line0.length(), L'*');
						line.replace(i, wordLen, ws);
						seq.replace(it->position()+i, wordLen, ws);
					}
				}
			}
		}

		//output
		cout<<endl<<"過濾後:"<<endl;
		wcout<<seq<<endl;
		
		//timer end
		QueryPerformanceCounter(&nEndTime);
		double timeT = (double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
		printf("\n耗時:%f\n",timeT);
	}

	system("pause");
	return 0;
}
//-----------------------------------------
