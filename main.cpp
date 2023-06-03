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

// ���߳�
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
// ���ɶ�ά��������0Ϊ�ָ�Ԫ��
// ��������
vector< vector <int> > generateVecs ( initializer_list< int > List);

// Handle��ʼ��
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

// ����point̫����
static POINT pForMouse;		 // usedIn: checkMouseStateIn
static POINT cursourXY = {0, 0};// usedIn: GLOBAL

// �����Сȫ�ֱ������� 
static CONSOLE_FONT_INFO fontInfo;
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
�����������nowInformation ����˵��
======================================
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
printQuePos -> ������vector����,�洢λ��

��ôΪʲô�ö�����
��Ϊ���ڴ���Ԫ�ص�ʱ����ܻ�������Ԫ�ؽ���
Ϊ��ֹ����Ԫ��˳�����Ҿ�ֻ���ö�����
*/
static vector< string > printQueStr;
static vector< vector<int> > printQuePos;
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
		cout << "-";
	}
	cout << printTitle;
	for(int i = 0; i < (csbi.srWindow.Right + 1) / 2 - printTitle.length() / 2; i++) {
		cout << "-";
	}
	cout << endl;
}
// �����Ļ
void eraseExpectFrame() {
	GetConsoleScreenBufferInfo(outputHandle, &csbi);
	for(int i = 0; i < (csbi.srWindow.Bottom - 3); i++) {
		gotoxy(1, i + 1);
		for(int j = 0; j < (csbi.srWindow.Right - 4); i++) {
			cout << " ";
		}
	}
	cout << endl;
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
	cout << button.label;
}
void eraseButtonLabel(struct Button button, int offset) {
	gotoxy(button.leftX[0], button.leftX[1]);
	for(int i = 0; i < sizeof(button.label) / sizeof(string) + offset; i++) {
		cout << " ";
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
	cout << "  \b\b";
	for(int i = 0; i < lineSize; i++) {
		int rowSize = deployUI[i].size();
		for(int j = 0; j < rowSize; j++) {
			for(int k = 0; k < lineSize; k++) {
				gotoxy(4 * j + 2, 2 * i + 1);
				cout << "-----";
				gotoxy(4 * j + 2, 2 * i + 2);
				switch ( deployUI[i][j] ){
					case 1: // �ɲ������
						cout << "| _ |";
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						break;
					case 2: // �޷��������
						cout << "| X |";
						break;
					case 3: // �ɲ���ɽ��
						cout << "|";
						wcout << ConsoleBackgroundColor::White;
						cout << " - ";
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						cout << "|";
						break;
					case 4: // ���ɲ���ɽ��
						cout << "|";
						wcout << ConsoleBackgroundColor::White;
						cout << " X ";
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						cout << "|";
						break;
					case 5: // �о������
						cout << "|";
						wcout << ConsoleBackgroundColor::Red;
						cout << " ! ";
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						cout << "|";
						break;
					case 6: // ������Ӫ��
						cout << "|";
						wcout << ConsoleBackgroundColor::Cyan;
						cout << " ! ";
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						cout << "|";
						break;
					case 7: // ��Ԩ
						cout << "\\";
						wcout << ConsoleBackgroundColor::Red;
						cout << "www";
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						cout << "/";
						break;
				} 
				gotoxy(4 * j + 2, 2 * i + 3);
				cout << "-----";                               
			}
		}
	}
}
void coutGenerals(vector< vector<int> > opearatorsFormations){
	gotoxy(1, 25);
	cout << "---------------------------------------------------------------------------------------";
	gotoxy(1, 26);
	cout << "                                                                                       ";
	gotoxy(1, 27);
	cout << "                                                                                       ";
	gotoxy(1, 28);
	cout << "                                                                                       ";
	gotoxy(1, 25);
	cout << "---------------------------------------------------------------------------------------";
	for (int i = 0; i < opearatorsFormations.size(); i++) {
		gotoxy(10 * i + 1, 26);
		wcout << ConsoleBackgroundColor::Yellow;
		cout << " Ex "<< opearatorsValue[ opearatorsFormations[i][0] - 1 ][11] << " ";
		switch (opearatorsFormations[i][2]){
			case 1:
				cout << "ʿ ";
				break;
			case 2:
				cout << "ξ ";
				break;
			case 3:
				cout << "У ";
				break;
			case 4:
				cout << "�� ";
				break;
		}
		wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
		
		gotoxy(10 * i + 1, 27);
		wcout << ConsoleBackgroundColor::White;
		cout << " " << opearatorsFiles[ opearatorsFormations[i][0] - 1 ][0] << "  ";
		wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
		
		gotoxy(10 * i + 1, 28);
		cout << " ";
		if (opearatorsFormations[i][2] < 4) {for( int j = 0; j < i + 1; j++) {cout << "I";}}
		else {cout << "IV";}
		wcout << ConsoleColor::WhiteIntensity;
		cout << " LV." << opearatorsFormations[i][1];
		wcout << ConsoleColor::None;
		
		for (int j = 1; j <= 3; j++){ 
			gotoxy(10 * i + 10, 25 + j);
			cout << "| ";
			gotoxy(10 * i + 4, 25);
			cout << "/\\";
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
		 					cout << "�ɲ������  ";
		 					break;
		 				case 2:
							cout << "���ɲ������";
							break;
						case 3:
							cout << "�ɲ���ɽ��  ";
							break;
						case 4:
							cout << "���ɲ���ɽ��";
							break;
						case 5:
							cout << "�о������  ";
							break;
						case 6:
							cout << "������Ӫ��  ";
							break;
						case 7:
							cout << "��Ԩ        ";
							break;
						default:
							cout << "            ";
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
	cout << "\bInformation";
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
		if ( !printQuePos.size() ) {
			// ����Ԫ��
			gotoxy(printQuePos[0][0], printQuePos[0][1]);
			cout << printQueStr[0];
			printQuePos.erase[0];
		}
		Sleep(refreshTime);
	}
}
void pushPrintTask(short int px, short int py, string pStr) {
	while (!isQueBusy){
		isQueBusy = true;
		printQuePos.push_back(generateVecs(px, py));
		printQueStr.push_back(pStr);
		isQueBusy = false;
	}
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
	// ��ȡ���ڴ�С 90x30
	GetConsoleScreenBufferInfo(outputHandle, &csbi);
	// ���ÿ���̨ģʽ
	DWORD consoleMode;
	SMALL_RECT winSize = {90, 30} ;
	GetConsoleMode(inputHandle, &consoleMode);
	consoleMode &= ~ENABLE_QUICK_EDIT_MODE;
	consoleMode &= ~ENABLE_INSERT_MODE;
	SetConsoleMode(inputHandle, consoleMode);
	SetConsoleWindowInfo(outputHandle, 1, &winSize);
	// CONSOLE_CURSOR_INFO cursor_info={0,0}; 
	// SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);

	// ��Ϊϸ�ڵļ��ض��� -> ������ʽ[To do]
	printDevideLineWithInfo("���ڼ���...", 2);
	gotoxy(0, 1);
	for(int i = 0; i < (csbi.srWindow.Bottom - 1); i++) {
		cout << "| ";
		for (int j = 0; j < (csbi.srWindow.Right - 4); j++) {
			cout << " ";
		}
		cout << " |" << endl;
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