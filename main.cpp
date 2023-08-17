/*

	Shattered Light - 消逝的光芒
	by TBDriver
	
	Version 0.1


    No BUG ANYTIME pls!
    
*/

// 功能实现
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

// 并发编程
#include <thread>
#include <mutex>

// 文件操作
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "json\json.h"

// 彩色文字
#define OS_TYPE_WINDOWS_CC
#include "ColorfulConsole.h"

// 按键按下函数预定义
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1: 0)
// 吐槽一下 大写就别惦记你内驼峰了 

// 命名空间
using namespace std;
using std::wcout; // LOVE from ColorfulConsole.h
using std::endl;

/*
*	变量定义组
*/

// 函数预定义
// 生成二维向量组，以0为分割元素
// 接受整型
vector< vector <int> > generateVecs ( initializer_list< int > List);
void pushPrintTask(string pStr, int priority);

// Handle初始化
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

// 你这point太假了
static POINT pForMouse;		 // usedIn: checkMouseStateIn
static POINT cursourXY = {0, 0};// usedIn: GLOBAL

// 字体大小全局变量建立 
static CONSOLE_FONT_INFO fontInfo;

// mutex锁
mutex mutexPushLock;
// %HOMEPATH%

// 将领信息
/*
整型向量矩阵opearatorsValue 数据说明
======================================
0: 将领唯一码  1: 体力  2: ATK 攻击力  3: DEF 防御力  4: 机动性(闪避%)  5: 牵制力(阻挡)  6: 自制力((1-再部署%) * 80s)

7: 主兵种
1.车 (近+远,攻击速度略快)   2.步 (近/远,攻击速度中等)   3.骑 (近,攻击速度略快,攻击力较高,防御力略低)   4.舟
8: 兵种分支
车 1.战车(消耗军费略高,防御力略高)
步 1.轻装步兵 (机动性略高(5)) 2.重装步兵 (体力与防御力高,牵制力高,自制力略低,消耗军费高) 3.奇兵(攻击力高,防御力略低,自制力高,牵制力低,消耗军费略低) 4.弓箭手(高台,攻击速度略快,牵制力低)
骑 1.骑士(消耗军费略高,攻击力略高,攻击速度快)
舟 1.后勤(回血) 2.战鼓(加攻击/速度/防御/机动性)

8: 可部署位置 1.地面 2.高台 3.both  10: 攻击速度(间隔/ms) 11: 消耗军费 12: 稀有度
*/
static vector< vector<int> > opearatorsValue = generateVecs({1, 1274, 276, 168, 5, 2, 30, 2, 1, 1, 1500, 15, 7, 0, \
															 2, 1356, 232, 175, 1, 1, 40, 3, 1, 1, 1000, 18, 6, 0});
/*
字符串向量矩阵opearatorsFiles 数据说明
======================================
0: 姓字  1: 最终所属势力  2: 性别  3: 种族
======================================
id:1 刘玄德
id:2 张翼德
*/
static string opearatorsFiles[][4] = {{"刘玄德", "蜀", "男", "汉族"},\
									   "张翼德", "蜀", "男", "汉族"};

/*
## 整型数组矩阵nowInformation 数据说明
## ======================================
0: 鼠标所在点类型
1.地图  2.待部署区 

1: 鼠标所在点状态
1-> 1.可部署地面  2.不可部署地面  3.可部署山地  4.不可部署山地  5.敌军攻入点  6.保护本营点  7.山地
2-> %将领唯一码%
*/
static int nowInformation[] = {1, 1};

/*
输出队列向量组 数据说明
======================================
printQueStr -> 字符串类型vector,存储字符串

那么为什么用队列呢
因为咱在处理元素的时候可能会有其他元素介入
为防止出现元素顺序紊乱就只能用队列了
*/
static vector< vector<string> > printQueStr;
static bool isQueBusy = false;


/*
*	基础功能组
*	包括：光标移动gotoxy 检测鼠标按下范围checkMouseStateIn 数组转vectorarrayToVec
*	author: 闰土
*/
// 光标移动
void gotoxy(short int x, short int y) {
	COORD pos = {x, y};
	SetConsoleCursorPosition(outputHandle, pos);
}
// 检测鼠标按下范围
int checkMouseStateIn(int x, int xRound, int y, int yRound) {
	int pointX, pointY;
	GetCursorPos(&pForMouse);
	// 获取鼠标坐标
	ScreenToClient(GetForegroundWindow(), &pForMouse);
	GetCurrentConsoleFont(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fontInfo);
	pointX = pForMouse.x / fontInfo.dwFontSize.X;
	pointY = pForMouse.y / fontInfo.dwFontSize.Y;
	// 判断范围
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
// 记得，这是你花整整半天时间12个小时查资料写的
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
*	复合功能组
*	现已加入大道磨灭套餐
*	包括：分割线 打印 拓展-按钮结构操作
*	author: 闰土
*
*	Base - 基本功能
*
*/
// 分割线打印
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
// 清除屏幕
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
*	Extended - 按钮结构操作
*	include: 基础按钮数据体 按钮初始化函数
*	         按钮文本覆盖式输出
*/
struct Button {
	int leftX[2], leftY[2], rightX[2], rightY[2];
	bool isPressed;
	void (*func)();
	string label;
};
void buttonInit(struct Button &button, int leftXY[2], int rightXY[2], string buttonLabel, void (*func)()) {
	button.func = func;
	// 位置赋值
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
* UI - 基础界面定义及刷新
* include: coutMap coutGenerals checkInformation play
*/
void coutMap(vector< vector<int> > deployUI) {
	/* 
	 接受vector容器矩阵 
	 限定类型为int 
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
					case 1: // 可部署地面
						pushPrintTask("| _ |", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						break;
					case 2: // 无法部署地面
						pushPrintTask("| X |", 2);
						break;
					case 3: // 可部署山地
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::White;
						pushPrintTask(" - ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 4: // 不可部署山地
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::White;
						pushPrintTask(" X ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 5: // 敌军攻入点
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::Red;
						pushPrintTask(" ! ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 6: // 保护本营点
						pushPrintTask("|", 2);
						wcout << ConsoleBackgroundColor::Cyan;
						pushPrintTask(" ! ", 2);
						wcout << ConsoleBackgroundColor::None << ConsoleColor::None;
						pushPrintTask("|", 2);
						break;
					case 7: // 深渊
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
	// for效果:打印将领信息
	for (int i = 0; i < opearatorsFormations.size(); i++) {
		gotoxy(10 * i + 1, 26);
		wcout << ConsoleBackgroundColor::Yellow;
		pushPrintTask(" Ex " + to_string(opearatorsValue[(opearatorsFormations[i][0]-1)][11]) + " ", 2);
		switch (opearatorsFormations[i][2]){
			case 1:
				pushPrintTask("士 ", 2);
				break;
			case 2:
				pushPrintTask("尉 ", 2);
				break;
			case 3:
				pushPrintTask("校 ", 2);
				break;
			case 4:
				pushPrintTask("将 ", 2);
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
		 					pushPrintTask("可部署地面  ", 1);
		 					break;
		 				case 2:
							pushPrintTask("不可部署地面", 1);
							break;
						case 3:
							pushPrintTask("可部署山地  ", 1);
							break;
						case 4:
							pushPrintTask("不可部署山地", 1);
							break;
						case 5:
							pushPrintTask("敌军攻入点  ", 1);
							break;
						case 6:
							pushPrintTask("保护本营点  ", 1);
							break;
						case 7:
							pushPrintTask("深渊        ", 1);
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
	printDevideLineWithInfo("Command Cyperspace - 演算准备中...| ", 1);
	coutMap(mapVec);
	Sleep(700);
	gotoxy(0, 0);
	printDevideLineWithInfo("Command Cyperspace - 线程装载...\\ ", 1);
	std::thread checkInformationThread (checkInformation, mapVec, opearatorsFormations);
	checkInformationThread.detach();	
	Sleep(200);
	gotoxy(0, 0);
	printDevideLineWithInfo("Command Cyperspace - 将领模拟中...- ", 1);
	coutGenerals(opearatorsFormations);
	Sleep(100);
	gotoxy(0, 0);
	printDevideLineWithInfo("Command Cyperspace - 演算正常运行中...", 1);
	gotoxy(75, 0);
	pushPrintTask("\bInformation", 2);
	while (1) {
		// 进入游玩
		/*
		for (int i = mapVec.begin(); i != mapVec.end(); i++) {
			
		}*/
		// todo:显示信息框
		// coutGenerals(opearatorsFormations);
		Sleep(50);
	}	
}
/*
* Extended - 输出队列PrintQueue
* include: printQueue pushPrintTask
*/
void printQueue(short int refreshTime) {
	while (1) {
		if ( printQueStr.size() ) { // 判断队列是否为空 节省资源
			// 输出存储的元素
			for (int i = 0; i <= 4; i++) {
				for(int j = 0; j <= printQueStr[i].size(); j++) {
					printf((char[])printQueStr[i][-1]);
					printQueStr[i].pop_back();
				}
			}
		}
		Sleep(refreshTime); // 黄忠二技能cd
	}
}
// √finish-> TODO: taskQue 并发编程 mutex锁资源?
void pushPrintTask(string pStr, int priority) {
	/*
	* string pStr 用于添加至输出队列
	* int    priority 打印优先级
	* 0: 置顶 需要置于顶层的数据
	* 1: 最重要 需要即时刷新的数据
	* 2: 较为重要 适用于正在移动或正在在攻击的单位
	* 3: 正常 适用于特效或边界
	* 4: 劣重要 适用于不常刷新或无所谓的数据
	*/
	mutexPushLock.lock(); // 锁定防止数据紊乱
	printQueStr[priority].push_back(pStr); // 加入队列
	mutexPushLock.unlock();// 解锁
}


/*
整型向量矩阵opearatorsFormations 数据说明
======================================
0: 将领唯一码  1: 将领等级 2.将领军衔 (1.士  2.尉  3.校  4.将)
======================================
*/



// 主函数
int main(int argc, char* argv[]) {
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

	// TODO: 更为细节的加载动画 -> 进度条式
	printDevideLineWithInfo("正在加载...", 2);
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
	printDevideLineWithInfo("加载成功", 2);

	// 完成加载
	SetConsoleTitleA("Shattered Light");
	gotoxy(0, 0);
	printDevideLineWithInfo("Shattered Light - 破碎的光芒", 2);
	Sleep(1000);

	Button aButton;
	string tempLabel = "进行部署";
	int a[] = {1, 1}, b[] = {3, 2};
	buttonInit(aButton, a, b, tempLabel, eraseExpectFrame);
	printButtonLabel(aButton);	
	Sleep(1000);
	
	vector< vector<int> > opearatorsFormations = generateVecs({1, 40, 1, 0, \
															   2, 40, 4, 0});
	
	play(generateVecs({ 3, 2, 1, 0, 1, 1, 4, 0, 2, 4, 2, 1, 1, 1, 0, 5, 1, 1, 1, 2, 1, 6, 0, 7, 0 }), opearatorsFormations);
	
	return 0;
}