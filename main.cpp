/*

	Shattered Light - ���ŵĹ�â
	by TBDriver
	
	Version 0.1


    No BUG ANYTIME pls!
    
*/

// ����ʵ��
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

// �������
#include <thread>
#include <mutex>

// �ļ�����
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "json\json.h"

// ��ɫ����
#define OS_TYPE_WINDOWS_CC
#include "ColorfulConsole.h"

// �������º���Ԥ����
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1: 0)
// �²�һ�� ��д�ͱ��������շ��� 

// �����ռ�
using namespace std;
using std::wcout; // LOVE from ColorfulConsole.h
using std::endl;

/*
*	����������
*/

// ����Ԥ����
// ���ɶ�ά�����飬��0Ϊ�ָ�Ԫ��
// ��������
vector< vector <int> > generateVecs ( initializer_list< int > List);
void pushPrintTask(string pStr, int priority);

// Handle��ʼ��
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

// ����point̫����
static POINT pForMouse;		 // usedIn: checkMouseStateIn
static POINT cursourXY = {0, 0};// usedIn: GLOBAL

// �����Сȫ�ֱ������� 
static CONSOLE_FONT_INFO fontInfo;

// mutex��
mutex mutexPushLock;
// %HOMEPATH%

// ������Ϣ
/*
������������opearatorsValue ����˵��
======================================
0: ����Ψһ��  1: ����  2: ATK ������  3: DEF ������  4: ������(����%)  5: ǣ����(�赲)  6: ������((1-�ٲ���%) * 80s)

7: ������
1.�� (��+Զ,�����ٶ��Կ�)   2.�� (��/Զ,�����ٶ��е�)   3.�� (��,�����ٶ��Կ�,�������ϸ�,�������Ե�)   4.��
8: ���ַ�֧
�� 1.ս��(���ľ����Ը�,�������Ը�)
�� 1.��װ���� (�������Ը�(5)) 2.��װ���� (�������������,ǣ������,�������Ե�,���ľ��Ѹ�) 3.���(��������,�������Ե�,��������,ǣ������,���ľ����Ե�) 4.������(��̨,�����ٶ��Կ�,ǣ������)
�� 1.��ʿ(���ľ����Ը�,�������Ը�,�����ٶȿ�)
�� 1.����(��Ѫ) 2.ս��(�ӹ���/�ٶ�/����/������)

8: �ɲ���λ�� 1.���� 2.��̨ 3.both  10: �����ٶ�(���/ms) 11: ���ľ��� 12: ϡ�ж�
*/
static vector< vector<int> > opearatorsValue = generateVecs({1, 1274, 276, 168, 5, 2, 30, 2, 1, 1, 1500, 15, 7, 0, \
															 2, 1356, 232, 175, 1, 1, 40, 3, 1, 1, 1000, 18, 6, 0});
/*
�ַ�����������opearatorsFiles ����˵��
======================================
0: ����  1: ������������  2: �Ա�  3: ����
======================================
id:1 ������
id:2 �����
*/
static string opearatorsFiles[][4] = {{"������", "��", "��", "����"},\
									   "�����", "��", "��", "����"};

/*
## �����������nowInformation ����˵��
## ======================================
0: ������ڵ�����
1.��ͼ  2.�������� 

1: ������ڵ�״̬
1-> 1.�ɲ������  2.���ɲ������  3.�ɲ���ɽ��  4.���ɲ���ɽ��  5.�о������  6.������Ӫ��  7.ɽ��
2-> %����Ψһ��%
*/
static int nowInformation[] = {1, 1};

/*
������������� ����˵��
======================================
printQueStr -> �ַ�������vector,�洢�ַ���

��ôΪʲô�ö�����
��Ϊ���ڴ���Ԫ�ص�ʱ����ܻ�������Ԫ�ؽ���
Ϊ��ֹ����Ԫ��˳�����Ҿ�ֻ���ö�����
*/
static vector< vector<string> > printQueStr;
static bool isQueBusy = false;


/*
*	����������
*	����������ƶ�gotoxy �����갴�·�ΧcheckMouseStateIn ����תvectorarrayToVec
*	author: ����
*/
// ����ƶ�
void gotoxy(short int x, short int y) {
	COORD pos = {x, y};
	SetConsoleCursorPosition(outputHandle, pos);
}
// �����갴�·�Χ
int checkMouseStateIn(int x, int xRound, int y, int yRound) {
	int pointX, pointY;
	GetCursorPos(&pForMouse);
	// ��ȡ�������
	ScreenToClient(GetForegroundWindow(), &pForMouse);
	GetCurrentConsoleFont(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fontInfo);
	pointX = pForMouse.x / fontInfo.dwFontSize.X;
	pointY = pForMouse.y / fontInfo.dwFontSize.Y;
	// �жϷ�Χ
	if ( (pointX <= x + xRound) && (pointX >= x) && (pointY <= y + yRound) && (pointY >= y) ) {
		return 1;
	}
	return 0;
}
int checkMousePoint() {
	int pointX, pointY;
	GetCursorPos(&pForMouse);
	ScreenToClient(GetForegroundWindow(), &pForMouse);
	GetCurrentConsoleFont(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fontInfo);
	pointX = pForMouse.x / fontInfo.dwFontSize.X;
	pointY = pForMouse.y / fontInfo.dwFontSize.Y;
	int pointXY[] = {pointX, pointY};
	return *pointXY;
}
// �ǵã������㻨��������ʱ��12��Сʱ������д��
vector< vector <int> > generateVecs ( initializer_list< int > List) {
	vector < vector<int> > tempVec;
	vector <int> inTempVec;
	for ( auto beg = List.begin(); beg != List.end(); beg++ ) {
		if (*beg) {
			inTempVec.push_back(*beg);
		}
		else {
			tempVec.push_back(inTempVec);
			inTempVec.resize(0);
		}
	}
	return tempVec;
}
/*
*	���Ϲ�����
*	���Ѽ�����ĥ���ײ�
*	�������ָ��� ��ӡ ��չ-��ť�ṹ����
*	author: ����
*
*	Base - ��������
*
*/
// �ָ��ߴ�ӡ
void printDevideLineWithInfo(string printTitle, int offset) {
	GetConsoleScreenBufferInfo(outputHandle, &csbi);
	for(int i = 0; i < (csbi.srWindow.Right + 1) / 2 - printTitle.length() / 2 - offset; i++) {
		pushPrintTask("-", 3);
	}
	pushPrintTask(printTitle, 3);
	for(int i = 0; i < (csbi.srWindow.Right + 1) / 2 - printTitle.length() / 2; i++) {
		pushPrintTask("-", 3);
	}
	pushPrintTask("\n", 3);
}
// �����Ļ
void eraseExpectFrame() {
	GetConsoleScreenBufferInfo(outputHandle, &csbi);
	for(int i = 0; i < (csbi.srWindow.Bottom - 3); i++) {
		gotoxy(1, i + 1);
		for(int j = 0; j < (csbi.srWindow.Right - 4); i++) {
			pushPrintTask(" ", 3);
		}
	}
	pushPrintTask("\n", 3);	
}
/*
*	Extended - ��ť�ṹ����
*	include: ������ť������ ��ť��ʼ������
*	         ��ť�ı�����ʽ���
*/
struct Button {
	int leftX[2], leftY[2], rightX[2], rightY[2];
	bool isPressed;
	void (*func)();
	string label;
};
void buttonInit(struct Button &button, int leftXY[2], int rightXY[2], string buttonLabel, void (*func)()) {
	button.func = func;
	// λ�ø�ֵ
	button.leftX[0] = leftXY[0];
	button.leftX[1] = leftXY[1];
	button.leftY[0] = leftXY[0];
	button.leftY[1] = rightXY[1];
	button.rightX[0] = rightXY[0];
	button.rightX[1] = leftXY[1];
	button.rightY[0] = rightXY[0];
	button.rightY[0] = rightXY[1];
	button.label = buttonLabel;
	button.isPressed = false;
}
void printButtonLabel(struct Button button) {
	gotoxy(button.leftX[0], button.leftX[1]);
	pushPrintTask(button.label, 2);
}
void eraseButtonLabel(struct Button button, int offset) {
	gotoxy(button.leftX[0], button.leftX[1]);
	for(int i = 0; i < sizeof(button.label) / sizeof(string) + offset; i++) {
		pushPrintTask(" ", 3);
	}
}
/*
* UI - �������涨�弰ˢ��
* include: coutMap coutGenerals checkInformation play
*/
void coutMap(vector< vector<int> > deployUI) {
	/* 
	 ����vector�������� 
	 �޶�����Ϊint 
	*/
	int lineSize = deployUI.size();
	gotoxy(1, 1);
	pushPrintTask("  \b\b", 2);
	for(int i = 0; i < lineSize; i++) {
		int rowSize = deployUI[i].size();
		for(int j = 0; j < rowSize; j++) {
			for(int k = 0; k < lineSize; k++) {
				gotoxy(4 * j + 2, 2 * i + 1);
				pushPrintTask("-----", 2);
				gotoxy(4 * j + 2, 2 * i + 2);
				switch ( deployUI[i][j] ){
					case 1: // �ɲ������
						pushPrintTask("| _ |", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						break;
					case 2: // �޷��������
						pushPrintTask("| X |", 2);
						break;
					case 3: // �ɲ���ɽ��
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::White;
						pushPrintTask(" - ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 4: // ���ɲ���ɽ��
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::White;
						pushPrintTask(" X ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 5: // �о������
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::Red;
						pushPrintTask(" ! ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 6: // ������Ӫ��
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::Cyan;
						pushPrintTask(" ! ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 7: // ��Ԩ
						pushPrintTask("\\", 2);
						wcout << ConsoleBackgroundColor::Red;
						pushPrintTask("www", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("/", 2);
						break;
				} 
				gotoxy(4 * j + 2, 2 * i + 3);
				pushPrintTask("-----", 2);
			}
		}
	}
}
void coutGenerals(vector< vector<int> > opearatorsFormations){
	gotoxy(1, 25);
	pushPrintTask("---------------------------------------------------------------------------------------", 2);
	gotoxy(1, 26);
	pushPrintTask("                                                                                       ", 2);
	gotoxy(1, 27);
	pushPrintTask("                                                                                       ", 2);
	gotoxy(1, 28);
	pushPrintTask("                                                                                       ", 2);
	gotoxy(1, 25);
	pushPrintTask("---------------------------------------------------------------------------------------", 2);
	// forЧ��:��ӡ������Ϣ
	for (int i = 0; i < opearatorsFormations.size(); i++) {
		gotoxy(10 * i + 1, 26);
		wcout << ConsoleBackgroundColor::Yellow;
		pushPrintTask(" Ex " + to_string(opearatorsValue[(opearatorsFormations[i][0]-1)][11]) + " ", 2);
		switch (opearatorsFormations[i][2]){
			case 1:
				pushPrintTask("ʿ ", 2);
				break;
			case 2:
				pushPrintTask("ξ ", 2);
				break;
			case 3:
				pushPrintTask("У ", 2);
				break;
			case 4:
				pushPrintTask("�� ", 2);
				break;
		}
		wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
		
		gotoxy(10 * i + 1, 27);
		wcout << ConsoleBackgroundColor::White;
		pushPrintTask(" " += (char)opearatorsFiles[opearatorsFormations[i][0]-1][0] + "  ", 2);
		wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
		
		gotoxy(10 * i + 1, 28);
		pushPrintTask(" ", 2);
		if (opearatorsFormations[i][2] < 4) {for( int j = 0; j < i + 1; j++) {pushPrintTask("I", 2);}}
		else {pushPrintTask("IV", 2);}
		wcout << ConsoleColor::WhiteIntensity;
		pushPrintTask(" LV." + (char)opearatorsFormations[i][1], 2);
		wcout << ConsoleColor::None;
		
		for (int j = 1; j <= 3; j++){ 
			gotoxy(10 * i + 10, 25 + j);
			pushPrintTask("| ", 2);
			gotoxy(10 * i + 4, 25);
			pushPrintTask("/\\", 2);
		}
		
	}
}
void checkInformation(vector < vector<int> > mapVec, vector < vector <int> > opearatorsFormations) {
	Sleep(1000);
	bool isInformationChanged = false;
	while (1) {
		for (int i = 0; i < mapVec.size(); i++) {
			for (int j = 0; j < mapVec[i].size(); j++) {
				if (checkMouseStateIn( j*4+3, 3, i*2+2, 3 ) ) {
					nowInformation[0] = 1;
					nowInformation[1] = mapVec[i][j];
					isInformationChanged = true;
				}
			}
		}
		if (isInformationChanged) {
			isInformationChanged = false;
			gotoxy(70, 1);
			switch (nowInformation[0]) {
				case 1:
					switch (nowInformation[1]) {
						case 1:	
		 					pushPrintTask("�ɲ������  ", 1);
		 					break;
		 				case 2:
							pushPrintTask("���ɲ������", 1);
							break;
						case 3:
							pushPrintTask("�ɲ���ɽ��  ", 1);
							break;
						case 4:
							pushPrintTask("���ɲ���ɽ��", 1);
							break;
						case 5:
							pushPrintTask("�о������  ", 1);
							break;
						case 6:
							pushPrintTask("������Ӫ��  ", 1);
							break;
						case 7:
							pushPrintTask("��Ԩ        ", 1);
							break;
						default:
							pushPrintTask("            ", 1);
					}
					break;
				case 2:
					break;
			}
			nowInformation[0] = 0;
			nowInformation[1] = 0;
		}
		Sleep(50);
	}
	
}
void play(vector < vector<int> > mapVec, vector < vector <int> > opearatorsFormations) {
	gotoxy(0, 0);
	printDevideLineWithInfo("Command Cyperspace - ����׼����...| ", 1);
	coutMap(mapVec);
	Sleep(700);
	gotoxy(0, 0);
	printDevideLineWithInfo("Command Cyperspace - �߳�װ��...\\ ", 1);
	std::thread checkInformationThread (checkInformation, mapVec, opearatorsFormations);
	checkInformationThread.detach();	
	Sleep(200);
	gotoxy(0, 0);
	printDevideLineWithInfo("Command Cyperspace - ����ģ����...- ", 1);
	coutGenerals(opearatorsFormations);
	Sleep(100);
	gotoxy(0, 0);
	printDevideLineWithInfo("Command Cyperspace - ��������������...", 1);
	gotoxy(75, 0);
	pushPrintTask("\bInformation", 2);
	while (1) {
		// ��������
		/*
		for (int i = mapVec.begin(); i != mapVec.end(); i++) {
			
		}*/
		// todo:��ʾ��Ϣ��
		// coutGenerals(opearatorsFormations);
		Sleep(50);
	}	
}
/*
* Extended - �������PrintQueue
* include: printQueue pushPrintTask
*/
void printQueue(short int refreshTime) {
	while (1) {
		if ( printQueStr.size() ) { // �ж϶����Ƿ�Ϊ�� ��ʡ��Դ
			// ����洢��Ԫ��
			for (int i = 0; i <= 4; i++) {
				for(int j = 0; j <= printQueStr[i].size(); j++) {
					printf((char[])printQueStr[i][-1]);
					printQueStr[i].pop_back();
				}
			}
		}
		Sleep(refreshTime); // ���Ҷ�����cd
	}
}
// ��finish-> TODO: taskQue ������� mutex����Դ?
void pushPrintTask(string pStr, int priority) {
	/*
	* string pStr ����������������
	* int    priority ��ӡ���ȼ�
	* 0: �ö� ��Ҫ���ڶ��������
	* 1: ����Ҫ ��Ҫ��ʱˢ�µ�����
	* 2: ��Ϊ��Ҫ �����������ƶ��������ڹ����ĵ�λ
	* 3: ���� ��������Ч��߽�
	* 4: ����Ҫ �����ڲ���ˢ�»�����ν������
	*/
	mutexPushLock.lock(); // ������ֹ��������
	printQueStr[priority].push_back(pStr); // �������
	mutexPushLock.unlock();// ����
}


/*
������������opearatorsFormations ����˵��
======================================
0: ����Ψһ��  1: ����ȼ� 2.������� (1.ʿ  2.ξ  3.У  4.��)
======================================
*/



// ������
int main(int argc, char* argv[]) {
	SetConsoleTitleA("Shattering...");
	GetConsoleScreenBufferInfo(outputHandle, &csbi); // ��ȡ���ڴ�С 90x30

	// ���ÿ���̨ģʽ
	DWORD consoleMode;
	SMALL_RECT winSize = {90, 30} ;
	GetConsoleMode(inputHandle, &consoleMode);
	consoleMode &= ~ENABLE_QUICK_EDIT_MODE;
	consoleMode &= ~ENABLE_INSERT_MODE;
	SetConsoleMode(inputHandle, consoleMode);
	SetConsoleWindowInfo(outputHandle, 1, &winSize);

	// TODO: ��Ϊϸ�ڵļ��ض��� -> ������ʽ
	printDevideLineWithInfo("���ڼ���...", 2);
	gotoxy(0, 1);
	for(int i = 0; i < (csbi.srWindow.Bottom - 1); i++) {
		pushPrintTask("| ", 1);
		for (int j = 0; j < (csbi.srWindow.Right - 4); j++) {
			pushPrintTask(" ", 1);
		}
		pushPrintTask(" |\n", 1);
	}
	printDevideLineWithInfo("", 1);
	gotoxy(0, 0);
	printDevideLineWithInfo("���سɹ�", 2);

	// ��ɼ���
	SetConsoleTitleA("Shattered Light");
	gotoxy(0, 0);
	printDevideLineWithInfo("Shattered Light - ����Ĺ�â", 2);
	Sleep(1000);

	Button aButton;
	string tempLabel = "���в���";
	int a[] = {1, 1}, b[] = {3, 2};
	buttonInit(aButton, a, b, tempLabel, eraseExpectFrame);
	printButtonLabel(aButton);	
	Sleep(1000);
	
	vector< vector<int> > opearatorsFormations = generateVecs({1, 40, 1, 0, \
															   2, 40, 4, 0});
	
	play(generateVecs({ 3, 2, 1, 0, 1, 1, 4, 0, 2, 4, 2, 1, 1, 1, 0, 5, 1, 1, 1, 2, 1, 6, 0, 7, 0 }), opearatorsFormations);
	
	return 0;
}