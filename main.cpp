#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <string>

// �������º���Ԥ���� 
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1: 0)

// �����ռ�
using namespace std;



/*
*	���������� 
*/
// Handle��ʼ�� 
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

// ����point̫����
static POINT pForMouse; 		 // usedIn: checkMouseState 
static POINT cursourXY = {0, 0}; // usedIn: GLOBAL



/*
*	���������� 
*	����������ƶ�gotoxy �����갴�·�ΧcheckMouseState
*	author: ���� 
*/

// ����ƶ�
void gotoxy(int x, int y) {
	COORD pos = {x, y};
	SetConsoleCursorPosition(outputHandle, pos);
}

// �����갴�·�Χ
int checkMouseState(int x, int xRound, int y, int yRound) {
	int pointX, pointY;
	CONSOLE_FONT_INFO fontInfo; 
	GetCursorPos(&pForMouse);
	ScreenToClient(GetForegroundWindow(), &pForMouse);
	GetCurrentConsoleFont(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fontInfo);
	pointX = pForMouse.x / fontInfo.dwFontSize.X;
	pointY = pForMouse.y / fontInfo.dwFontSize.Y;
	if(((pointX <= x + xRound) and (pointX >= x)) and ((pointY <= y + yRound) and (pointY >= y))){
		return 1;
	}
	else{
		return 0;
	}
}



/*
*	���Ϲ����� 
*	���Ѽ�����ĥ���ײ�
*	�������ָ���printDevideLineWithInfo
*		  ��ť�ṹ�� 
*	author: ���� 
*/

// �ָ��� 
void printDevideLineWithInfo(string printTitle, int offset) {
	GetConsoleScreenBufferInfo(outputHandle, &csbi);
	for(int i = 0; i < (csbi.srWindow.Right + 1) / 2 - printTitle.length() / 2 - offset; i++){
		cout << "-";
	}
	cout << printTitle;
	for(int i = 0; i < (csbi.srWindow.Right + 1) / 2 - printTitle.length() / 2; i++){
		cout << "-";
	}
	cout << endl;
}

struct buttonTUI{
	int leftX;
	int RightY;
	
	void temp(){
		printf("intoTemp");
	}
}; 



// ������ 
int main(int argc, char* argv[]) {
	SetConsoleTitleA("Shattering...");
	// ��ȡ���ڴ�С 90x30 
	GetConsoleScreenBufferInfo(outputHandle, &csbi);
	// ���ÿ���̨ģʽ 
	DWORD consoleMode;
	GetConsoleMode(inputHandle, &consoleMode);
	consoleMode &= ~ENABLE_QUICK_EDIT_MODE;
	consoleMode &= ~ENABLE_INSERT_MODE;
	SetConsoleMode(inputHandle, consoleMode);
	
	// ���ض���[To do] 
	
	gotoxy(0, 0);
	printDevideLineWithInfo("���ڼ���...", 2);
	gotoxy(0, 1);
	for(int i = 0; i < (csbi.srWindow.Bottom - 3); i++){
		cout << "| ";
		for (int j = 0; j < (csbi.srWindow.Right - 4); j++){
			cout << " ";	
		}
		cout << " |" << endl;;
	}
	
	printDevideLineWithInfo("", 1);
	
	gotoxy(0, 0);
	printDevideLineWithInfo("���سɹ�", 2);
	SetConsoleTitleA("Shattered Light");
	Sleep(1000);
	gotoxy(0, 0);
	printDevideLineWithInfo("Shattered Light - ����Ĺ�â", 2);
	
	buttonTUI
	
	// ��Ϸ��ѭ�� 
	while (1) {
		if (KEY_DOWN(VK_LBUTTON)) {
			if(checkMouseState(0, 10, 0, 10)) {
				gotoxy(6, 5);
				cout << "����";
			}
		}
		Sleep(90);
	}
	return 0;
}	
