#include<iostream>
#include<thread>
#include<queue>
#include<vector>
#include<unordered_set>
#include<boost/filesystem.hpp>
#include<mutex>
#include<string>
#include<chrono>
#include<climits>
#include<condition_variable>

using namespace std;
namespace fs=boost::filesystem;

struct Timer{
	chrono::time_point<chrono::system_clock> start,stop;
	chrono::duration<float> dur;
	Timer(){
		start=chrono::high_resolution_clock::now();
	}
	~Timer(){
		stop=chrono::high_resolution_clock::now();
		dur=stop-start;
		cout<<"Time taken to fetch the results in seconds : "<<dur.count()<<endl;
	}
};

mutex mut;//for filePath queues
mutex mut2;//for stored filepath unordered_set
mutex mut3;//for result vector
mutex mut4;//for condition variable

condition_variable cv;

queue<fs::path>qu;
string target;
vector<string>result;
unordered_set<string>stored;
int isDoneFlag=1;

bool threadSafeIsEmpty(){
	lock_guard<mutex>lg(mut);
	return qu.empty();
}
void threadSafePush(const fs::path& p){
	lock_guard<mutex>lg(mut);
	qu.push(p);

}
const fs::path threadSafeFront(){
	lock_guard<mutex>lg(mut);
	if(!qu.empty())
		return qu.front();
	const fs::path p("");
	return p;
}
void threadSafePop(){
	lock_guard<mutex>lg(mut);
	if(!qu.empty())
		qu.pop();
}
bool threadSafeCheck(const string& path){
	lock_guard<mutex>lg(mut2);
	if(stored.find(path)==stored.end())
		return true;
	return false;
}
void threadSafeInsert(const fs::path& p){
	lock_guard<mutex>lg(mut2);
	stored.insert(p.string());
}
void threadSafePushBack(const fs::path& p){
	lock_guard<mutex>lg(mut3);
	result.push_back(p.string());
}
bool isHiddenPath(const fs::path& p){
	const char* s=p.filename().string().c_str();
	if(s[0]=='.')
		return true;
	else
		return false;
}
int threadSafeSize(){
	lock_guard<mutex>lg(mut3);
	return result.size();
}

void searchInsidePath(const fs::path p){
	if(!isDoneFlag)
		return;
	try{
	for(fs::directory_entry& e:fs::directory_iterator(p)){
		if(fs::is_directory(e.path())&&!isHiddenPath(e.path())){
			if(threadSafeCheck(e.path().string())){
				threadSafeInsert(e.path());
				threadSafePush(e.path());
			}
		}
		else if(e.path().filename().string()==target)
			if(threadSafeCheck(e.path().string())){
				threadSafePushBack(e.path());
				threadSafeInsert(e.path().string());
			}
		}
	}catch(fs::filesystem_error& ex){
		cout<<ex.what()<<" "<<p.string()<<endl;
	}
}

int main(){
//	Timer t;
	string initialPath;
	cout<<"Enter the starting directory to be searched"<<endl;
	cin>>initialPath;
	cout<<"Enter the target file"<<endl;
	cin>>target;
	int estimateResults;
	cout<<"Enter the number of estimate number of files you want"<<endl;
	cin>>estimateResults;
	fs::path p(initialPath);
	if(!fs::exists(p)){
		cout<<"Error not a valid path is given to initiate search"<<endl;
		return 0;
	}
	qu.push(p);
	vector<thread> threadPool;
	int cnt=0;
	Timer time;
	while(isDoneFlag&&cnt<INT_MAX/10000){
		if(threadSafeIsEmpty()){
			++cnt;
		}
		else{
			const fs::path p=threadSafeFront();
			try{
			threadPool.emplace_back(thread (searchInsidePath,p));
			if(threadSafeSize()>=estimateResults)
				isDoneFlag=0;
			threadSafePop();
			}catch(exception& ex){
				++cnt;
			}
		}
	}
	for(auto& i:threadPool)
		i.join();
	for(const auto& i:result)
		cout<<i<<endl;
	cout<<endl;
	cout<<"- - - - - - - - - - - - - - - - - -"<<endl;
}
