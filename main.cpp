#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <string>

// 按键按下函数预定义 
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1: 0)

// 命名空间
using namespace std;



/*
*	变量定义组 
*/
// Handle初始化 
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

// 你这point太假了
static POINT pForMouse; 		 // usedIn: checkMouseState 
static POINT cursourXY = {0, 0}; // usedIn: GLOBAL



/*
*	基础功能组 
*	包括：光标移动gotoxy 检测鼠标按下范围checkMouseState
*	author: 闰土 
*/

// 光标移动
void gotoxy(int x, int y) {
	COORD pos = {x, y};
	SetConsoleCursorPosition(outputHandle, pos);
}

// 检测鼠标按下范围
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
*	复合功能组 
*	现已加入大道磨灭套餐
*	包括：分割线printDevideLineWithInfo
*		  按钮结构体 
*	author: 闰土 
*/

// 分割线 
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



// 主函数 
int main(int argc, char* argv[]) {
	SetConsoleTitleA("Shattering...");
	// 获取窗口大小 90x30 
	GetConsoleScreenBufferInfo(outputHandle, &csbi);
	// 设置控制台模式 
	DWORD consoleMode;
	GetConsoleMode(inputHandle, &consoleMode);
	consoleMode &= ~ENABLE_QUICK_EDIT_MODE;
	consoleMode &= ~ENABLE_INSERT_MODE;
	SetConsoleMode(inputHandle, consoleMode);
	
	// 加载动画[To do] 
	
	gotoxy(0, 0);
	printDevideLineWithInfo("正在加载...", 2);
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
	printDevideLineWithInfo("加载成功", 2);
	SetConsoleTitleA("Shattered Light");
	Sleep(1000);
	gotoxy(0, 0);
	printDevideLineWithInfo("Shattered Light - 破碎的光芒", 2);
	
	buttonTUI
	
	// 游戏主循环 
	while (1) {
		if (KEY_DOWN(VK_LBUTTON)) {
			if(checkMouseState(0, 10, 0, 10)) {
				gotoxy(6, 5);
				cout << "部署";
			}
		}
		Sleep(90);
	}
	return 0;
}	
