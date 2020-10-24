#include <bits/stdc++.h>
using namespace std;

vector<pair<char, string> > productions;
vector<vector<pair<char, string> > > itemsets;
map<pair<int, char>, int> goto1;
map<pair<int, char>, string> action;
set<char> terms;
map<char, set<char> > follows;

ofstream fout("output.txt", ios::out);

vector<pair<char, string> > closure(vector<pair<char, string> > kernel_item){
	vector<pair<char, string> > ret;

	stack<pair<char, string> > s;
	
	for(int i=0; i<kernel_item.size(); i++)s.push(kernel_item[i]);

	vector<bool> added(productions.size(), false);
	for(int i=0; i<productions.size(); i++){
		for(int j=0; j<kernel_item.size(); j++){
			if(kernel_item[j]==productions[i])added[i]=true;
		}
	}

	while(!s.empty()){
		pair<char, string> curr = s.top(); s.pop();
		ret.push_back(curr);

		int pos = -1;
		for(int i=0; i<curr.second.size(); i++){
			if(curr.second[i]=='.'){
				pos = i; break;
			}
		}

		if(pos==curr.second.size()-1)continue;

		char lhs = curr.second[pos+1];

		for(int i=0; i<productions.size(); i++){
			if(productions[i].first==lhs && !added[i]){
				pair<char, string> curr_prod = productions[i];
				added[i] = true;
				curr_prod.second.insert(curr_prod.second.begin(), '.');
				s.push(curr_prod);
			}
		}
	}

	return ret;
}

void build_items(){
	pair<char, string> curr_production = productions[0];
	curr_production.second.insert(curr_production.second.begin(), '.');
	
	vector<pair<char, string> > init; init.push_back(curr_production); itemsets.push_back(closure(init)); 
	stack<vector<pair<char, string> > > s; s.push(closure(init)); 

	while(!s.empty()){
		vector<pair<char, string> > now; now = s.top(); s.pop();

		int index = 0;

		for(int i=0; i<itemsets.size(); i++){
			if(now==itemsets[i]){
				index = i; break;
			}
		}

		map<char, vector<pair<char, string> > > kernels;

		for(int i=0; i<now.size(); i++){
			string rhs = now[i].second;
			for(int j=0; j<rhs.size(); j++){
				if(rhs[j]=='.' && j!=rhs.size()-1){
					swap(now[i].second[j], now[i].second[j+1]);
					kernels[now[i].second[j]].push_back(now[i]);
				}
			}
		}

		map<char, vector<pair<char, string> > >::iterator it;

		for(it=kernels.begin(); it!=kernels.end(); it++){
			vector<pair<char, string> > curr_close = closure(it->second);
			int flag = 0;
			for(int i=0; i<itemsets.size(); i++){
				if(itemsets[i]==curr_close){
					goto1[make_pair(index, it->first)] = i; flag = 1; break;
				}
			}

			if(flag==0){
				goto1[make_pair(index, it->first)] = itemsets.size();
				itemsets.push_back(curr_close);
				s.push(curr_close);
			}
		}
	}
}

void find_first(string arg, set<char> &first_set){
	if(arg[0]=='0'){
		first_set.insert(arg[0]); return;
	}
	else{
		for(int i=0; i<arg.size(); i++){
			int flag=0;
			if(!(arg[i]<='Z' && arg[i]>='A')){
				first_set.insert(arg[0]); return;
			}
			else{
				set<char> tmp_first_vec;
				vector<pair<char, string> > lhs_matched;

				for(int j=0; j<productions.size(); j++){
					if(productions[j].first==arg[i] && productions[j].second!=arg){
						lhs_matched.push_back(productions[j]);
					}
				}

				for(int j=0; j<lhs_matched.size(); j++){
					set<char> tmp_first_set;
					find_first(lhs_matched[j].second, tmp_first_set);
					set<char>::iterator it1;
					for(it1=tmp_first_set.begin(); it1!=tmp_first_set.end(); it1++)tmp_first_vec.insert(*it1);
				}

				if(tmp_first_vec.find('0')==tmp_first_vec.end())flag=1;

				set<char>::iterator it;

				for(it=tmp_first_vec.begin(); it!=tmp_first_vec.end(); it++){
					first_set.insert(*it);
				}
			}
			if(flag==1)break;
		}
	}
}

void build_first(){
	fout<<"\n\n";
	for(int i=0; i<productions.size(); i++){
		set<char> first_set;
		find_first(productions[i].second, first_set);
		fout<<"First for: "<<productions[i].second<<" - ";
		set<char>::iterator it;
		for(it=first_set.begin(); it!=first_set.end(); it++){
			fout<<" "<<*it;
		}
		fout<<endl;
	}
}

void find_follow(char nont, set<char> &follow_set, vector<bool> &visited){
	if(nont==productions[0].first){
		follow_set.insert('$'); 
	}

	for(int i=0; i<productions.size(); i++){
		if(!visited[i]){
			visited[i]=true;
			for(int j=0; j<productions[i].second.size(); j++){
				if(nont==productions[i].second[j]){
					if(j==productions[i].second.size()-1){
						if(nont!=productions[i].first){
							if(follows.find(productions[i].first)==follows.end()){
								set<char> tmp_follow_set;
								find_follow(productions[i].first, tmp_follow_set, visited);
								follows[productions[i].first] = tmp_follow_set;
								set<char>::iterator it;
								for(it=tmp_follow_set.begin(); it!=tmp_follow_set.end(); it++){
									follow_set.insert(*it);
								}
							}
							else{
								set<char>::iterator it;
								for(it=follows[productions[i].first].begin(); it!=follows[productions[i].first].end(); it++){
									follow_set.insert(*it);
								}
							}
						}
					}
					else{
						set<char> first_set;
						string beta;
						for(int k=j+1; k<productions[i].second.size(); k++)beta+=productions[i].second[k];
						find_first(beta, first_set);

						set<char>::iterator it;
						for(it=first_set.begin(); it!=first_set.end(); it++)if(*it!='0')follow_set.insert(*it);

						if(first_set.find('0')!=first_set.end()){
							if(nont!=productions[i].first){
								if(follows.find(productions[i].first)==follows.end()){
									set<char> tmp_follow_set;
									find_follow(productions[i].first, tmp_follow_set, visited);
									follows[productions[i].first] = tmp_follow_set;
									for(it=tmp_follow_set.begin(); it!=tmp_follow_set.end(); it++){
										follow_set.insert(*it);
									}
								}
								else{
									for(it=follows[productions[i].first].begin(); it!=follows[productions[i].first].end(); it++){
										follow_set.insert(*it);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void build_follow(){
	fout<<"\n\n";
	for(int i=0; i<productions.size(); i++){
		set<char> follow_set;
		vector<bool> visited(productions.size(), false);
		find_follow(productions[i].first, follow_set, visited);
		follows[productions[i].first] = follow_set;
		fout<<"Follow for: "<<productions[i].first<<" - ";
		set<char>::iterator it;
		for(it=follow_set.begin(); it!=follow_set.end(); it++){
			fout<<" "<<*it;
		}
		fout<<endl;
	}
}

void build_actions(){
	for(int i=0; i<itemsets.size(); i++){
		set<char> termcpy = terms;

		for(int j=0; j<itemsets[i].size(); j++){
			string tmp = itemsets[i][j].second;
			for(int k=0; k<tmp.size(); k++){
				if(tmp[k]=='.' && k!=tmp.size()-1){
					if(!(tmp[k+1]>='A'&&tmp[k+1]<='Z')){
						stringstream ss; ss<<goto1[make_pair(i, tmp[k+1])];
						action[make_pair(i, tmp[k+1])] = "shift "+ss.str();
						terms.erase(tmp[k+1]);
					}
				}
				else if(tmp[k]=='.' && k==tmp.size()-1){
					if(itemsets[i][j].first==productions[0].first){
                        action[make_pair(i, '$')] = "accept"; break;
                    }

					pair<char, string> reduce_this = itemsets[i][j];
					reduce_this.second.erase(reduce_this.second.find('.'));
					set<char>::iterator it;
					for(it=follows[reduce_this.first].begin(); it!=follows[reduce_this.first].end(); it++){
						stringstream ss; ss<<"reduce "<<reduce_this.first<<"->"<<reduce_this.second;
						action[make_pair(i, *it)] = ss.str();
					}
				}
			}
		} 
	}
}

void parse(){
	string w;
	cin>>w; w+='$';
	int i=0;
	stack<int> s; s.push(0);

	fout<<"\n\nParsing actions:\n\n";
	fout<<"String is:"<<w<<"\n\n";

	while(1){
		string acc = action[make_pair(s.top(), w[i])];
		fout << "\nACTION[" << s.top() << ", " <<w[i]<<"]"<<endl;
		if(acc[0]=='s'){
			fout<<acc<<endl;
			string tmp; 
			for(int j=6; j<acc.size(); j++)tmp+=acc[j]; 
			s.push(atoi(tmp.c_str())); i++;
		}
		else if(acc[0]=='r'){
			fout<<acc;
			char lhs=acc[7]; string rhs;
			for(int j=10; j<acc.size(); j++)rhs+=acc[j];
			int popoff=rhs.size();

			while(popoff--)s.pop();
			s.push(goto1[make_pair(s.top(), lhs)]);
			fout<<endl;
		}
		else if(acc=="accept"){
			fout<<"Accepted"<<endl; break;
		}
		else{
			fout<<"Error()\n"; break;
		}
	}
}

int main(){
	int n; 
	cout<<"Enter number of productions:"; cin>>n;

	while(n--){
		char lhs; string rhs; 
		cout<<"Enter lhs:"; cin>>lhs;
		cout<<"Enter rhs:"; cin>>rhs;
		productions.push_back(make_pair(lhs, rhs));
		cout<<endl;
		for(int i=0; i<rhs.size(); i++)if(!(rhs[i]>='A' && rhs[i]<='Z'))terms.insert(rhs[i]);
	}
	terms.insert('$');
	build_first();
	build_follow();
	build_items();
	build_actions();

	fout<<"\nItemsets:\n\n";

	for(int i=0; i<itemsets.size(); i++){
		fout<<"\n   ---------------------------------"<<endl;
		fout << "\tI-" << i <<":\n\n";

		for(int j=0; j<itemsets[i].size(); j++){
			fout<<"\t  "<<itemsets[i][j].first<<"->"<<itemsets[i][j].second<<endl;
		}
		fout<<"   ---------------------------------"<<endl;
		fout<<endl;
	}

	fout<<"\nGo to computations:\n\n";

	map<pair<int, char>, int>::iterator it;

	for(it=goto1.begin(); it!=goto1.end(); it++){
		fout<<"\tGOTO["<<(it->first).first<<", "<<(it->first).second<<"] = "<<it->second<<endl;
	}

	fout<<"\nActions in parsing table:\n\n";

	map<pair<int, char>, string>::iterator it2;

	for(it2=action.begin(); it2!=action.end(); it2++){
		fout<<"\tACTION["<<(it2->first).first<<", "<<(it2->first).second<<"] = "<<it2->second<<endl;
	}

	fout<<"\n\n";
	parse();
	fout.close();
	return 0;
}

/*
c167256@uselab168:~/LPlab/SLR_parsing$ ./a.out
Enter number of productions:7
Enter lhs:A
Enter rhs:E

Enter lhs:E
Enter rhs:T+E

Enter lhs:E
Enter rhs:T

Enter lhs:T
Enter rhs:F*T

Enter lhs:T
Enter rhs:F

Enter lhs:F
Enter rhs:(E)

Enter lhs:F
Enter rhs:d

*/
