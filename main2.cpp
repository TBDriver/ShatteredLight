#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <vector>

#include <thread>
#include <mutex>
using namespace std;

// Handle初始化
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

static POINT pForMouse;		     // have been used in: checkMouseStateIn
static POINT cursourXY = {0, 0}; // have been used in: main

static CONSOLE_FONT_INFO fontInfo; // 字体 

mutex printLock; // mutex锁

void gotoxy(short int x, short int y) {
	COORD pos = {x, y};
	SetConsoleCursorPosition(outputHandle, pos);
}
vector<int> generateVec ( initializer_list<int> List ) {
	vector<int> tempVec;
	for (auto beg = List.begin(); beg != List.end(); beg++ ) { tempVec.push_back(*beg); }
	return tempVec;
}
vector< vector<int> > generateVecSquare ( initializer_list<int> List) {
	vector < vector<int> > tempVec;
	vector <int> inTempVec;
	for ( auto beg = List.begin(); beg != List.end(); beg++ ) {
		if (*beg) { // 判断大小
			inTempVec.push_back(*beg);
		}
		else {      // 0则分割
			tempVec.push_back(inTempVec);
			inTempVec.resize(0);
		}
	}
	return tempVec;
}


// prior: < line1: <"2", "3"...>, line2: ... >
vector< vector<char> > initPrintVec() {
	vector< vector<char> > tempVecSqua;
	vector<char> tempVec;
	for (int j = 1; j <= 30; j++) {
		for (int i = 1; i <= 90; i++) {
			tempVec.push_back(' ');
		}
		tempVecSqua.push_back(tempVec);
		tempVec.resize(0);
	}
	return tempVecSqua;
}

/* < prior 1: < line1: <"2", "3"...>, line2: ... >,
*    prior 2: < line2: <"0", "0"...>, line2: ... >
*   > */
vector< vector< vector<char> > > initPriorVec() {
	vector< vector< vector<char> > > tempVecCube;
	for (int i=1; i <= 5; i++) {
		tempVecCube.push_back(initPrintVec());
	}
	return tempVecCube;
}

const char* stringToCharList(string str) {
	const char* chars = str.c_str();
	return chars;
}


static vector< vector< vector<char> > > outputVecInPrior = initPriorVec();
static vector< vector<char> > finalOutput = initPrintVec();


void pushToPrint(short int leftx, short int lefty, short int rightx, short int righty, const char outputStr[], short int priority, short int showMode) {
	// to_string()
	printLock.lock();
	short int localx = leftx;
	short int localy = lefty;
	for (int i = 0; i < strlen(outputStr); i++) {
		if (localx == 90 or localx == rightx) {
			localy++;
			localx = leftx;
		}
		if(localy == 30 or localy == righty) {
			break;
		}
		outputVecInPrior[priority][localx][localy] = outputStr[i];
		// outputVecInPrior[priority][localx][localy].replace; //(to_char(outputStr[i]));
		localx++;
	}
	printLock.unlock();
}
void printQueue(int refreshTime) {
	while (1) {
		if ( outputVecInPrior.capacity() ) { // 防止资源滥用
			printLock.lock();
			for(int i = 0; i < outputVecInPrior[0].size(); ++i) {
				for(int j = 0; j < outputVecInPrior[0][0].size(); ++j) {
					cout << outputVecInPrior[0][i][j] << " ";x
				} 
				cout << "\n";
			}

			printLock.unlock();
		}
		Sleep(refreshTime);
	}
}


int main() {
	// 输出流线程装载
	// std::thread printQueueThread (printQueue, 20);
	// printQueueThread.detach();

	SetConsoleTitleA("Shattering...");
	GetConsoleScreenBufferInfo(outputHandle, &csbi); // 获取窗口大小 90x30

	// 设置控制台模式
	DWORD consoleMode;
	SMALL_RECT winSize = {90, 30} ;
	GetConsoleMode(inputHandle, &consoleMode);
	consoleMode &= ~ENABLE_QUICK_EDIT_MODE;
	consoleMode &= ~ENABLE_INSERT_MODE;
	SetConsoleMode(inputHandle, consoleMode);
	SetConsoleWindowInfo(outputHandle, 1, &winSize);

	pushToPrint(0, 0, 5, 5, stringToCharList("233"), 0, 0);
	std::thread printQueueThread ( printQueue, 100 );
	printQueueThread.detach();

	return 0;
}