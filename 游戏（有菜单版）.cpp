#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h> 
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<conio.h>
#define N 1024
//定位箭头
static COORD coord;
void Move(char dir)
{
	COORD originCoord = coord;
	if (dir == 'a' && coord.Y <= 4 && coord.Y > 2)
	{
		coord.Y--;
	}
	else if (dir == 'd' && coord.Y < 4 && coord.Y >= 2)
	{
		coord.Y++;
	}
	else if (dir == '1')
	{
		coord.Y = 2;
	}
	else if (dir == '2')
	{
		coord.Y = 3;
	}
	else if (dir == '3')
	{
		coord.Y = 4;
	}
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), originCoord);
	printf(" ");
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	printf("\033[m<==\033[?251");
}
	static char menuContent[3][N] = {
		{"1.双人对战"},//进入双人游戏
		{"2.人机对战"},//进入人机对战
		{"3.退出游戏"}
	};
	void printMenu()
	{
		coord.X = 10;//列
		coord.Y = 2;//行
		for (int i = coord.Y, j = 0; i <= 4; i++)
		{
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
			coord.Y++;
			printf("%s\n", menuContent[j++]);

		}
		coord.X = 20;
		coord.Y = 2;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
		printf("\033[m<==\033[?251");
	}
int card[53];
const char p[3][8] = { "","Player1","Player2" };
int a1, a2;
void Washcard(int x, int y)
{
	//交换下标为x与y的两张牌 
	int t = card[x];
	card[x] = card[y];
	card[y] = t;
}
void output_num(int x)
{
	//用int型存卡牌点数，用此函数输出点数 
	if (x >= 2 && x <= 10) printf("%d", x);
	else switch (x) {
	case 1:printf("A"); break;
	case 11:printf("J"); break;
	case 12:printf("Q"); break;
	case 13:printf("K"); break;
	default:break;
	}
}
void calc_zds(int& zds, int numA)
{
	//计算最终点数，pts是没算A剩下牌的点数，numA是A的数量 
	int ans = zds;
	bool flag = false;//判断是否存在一种合法情况使得点数小于等于21
	for (int i = 0; i < (1 << numA); i++) {//从0到(1<<numA)-1的数字中，用第x位二进制是否为1来表示第x张A是否为11 
		int tmp = 0;//暂存分数 
		for (int j = 0; j < numA; j++) {//枚举二进制位 
			if (i & (1 << j)) tmp += 11;//如果是1，这个A算11 
			else tmp += 1;//否则这个A算1 
		}
		if (zds + tmp <= 21) flag = true;//存不存在合法情况 
		if (zds + tmp <= 21 && zds + tmp > ans) ans = zds + tmp;//更新答案 
	}
	if (!flag) zds += numA;//如果不存在一种情况使得点数小于等于21，那只能爆了，随便加点最小的 
	else zds = ans;//否则就按小于等于21的情况的最大情况算
}
int main()
{
	 printMenu();
	while (1)
	{
		if (_kbhit())
		{
			char Direction = _getch();
			if (Direction == 'a' || Direction == 'd' || Direction == '1' || Direction == '2' || Direction == '3')
			{
				Move(Direction);
			}
		}
	}
	srand(time(NULL));
	for (int i = 0; i < 13; i++)
		card[i * 4 + 1] = card[i * 4 + 2] = card[i * 4 + 3] = card[i * 4 + 4] = i + 1;//给卡牌赋初值1到13 
	for (int i = 2; i <= 52; i++)
		Washcard(i, rand() % (i - 1) + 1);//洗牌算法 
	puts("双人游戏开始：");

	int top = 52, round = 0;//top记录目前的牌顶，round是回合数 
	while (top)
	{
		printf("Round%d begin:\n", ++round);
		bool flag[3] = { 1,1,1 };//记录当前该选手是否还要选牌 
		int cnt[3] = { 0,0,0 };//记录该选手选了多少牌 
		int c[3][6];//记录该选手手里的牌 
		for (int i = 1; i <= 5 && (flag[1] || flag[2]) && top; i++)
		{
			//模拟要牌过程 
			char opt[3];
			for (int j = 1; j <= 2 && top; j++)
				if (top && flag[j]) {
					printf("%s: 请输入\"y\" 取牌或者 \"n\" 不取牌: ", p[j]);
					scanf("%c", &opt[j]);
					getchar();
					if (opt[j] == 'n') flag[j] = false;
					else {
						c[j][++cnt[j]] = card[top--];//记录手牌 
						printf("%s 现在手中的牌是: ", p[j]);
						for (int k = 1; k <= cnt[j]; k++) {
							output_num(c[j][k]);
							printf(k == cnt[j] ? ";\n" : ",");//格式输出 
						}
					}
				}
		}
		int tot[3] = { 0,0,0 }, numA[3] = { 0,0,0 }, flagB[3] = { 0,0,0 }, ini[3] = { 0,0,0 };
		//tot代表除A外的点数，numA代表A的数量 
		for (int i = 1; i <= 2; i++) {
			if (i == 1) printf("Round%d end: ", round);
			else printf("            ");
			for (int j = 1; j <= cnt[i]; j++)
			{
				//统计结果 
				if (c[i][j] == 1) numA[i]++;
				else tot[i] += c[i][j];
			}
			calc_zds(tot[i], numA[i]);//计算最终结果
			printf("%s的牌是: ", p[i]);
			for (int j = 1; j <= cnt[i]; j++) {
				output_num(c[i][j]);
				printf(j == cnt[i] ? ";" : ",");//格式输出 
			}
			printf(" Total point: %d.\n", tot[i]);//总分 
			if (tot[i] > 21) ini[i] = tot[i], flagB[i] = 1, tot[i] = 0;
		}
		if (flagB[1] && flagB[2]) tot[1] = ini[2], tot[2] = ini[1];
		if (tot[1] == tot[2]) printf("            %s 和 %s 平局.\n", p[1], p[2]);
		//后面是统计得分
		else printf("            %s 获胜and %s 失败.\n", tot[1] > tot[2] ? p[1] : p[2], tot[1] > tot[2] ? p[2] : p[1]);
		a1 += tot[1] > tot[2];
		a2 += tot[2] > tot[1];
		printf("目前的总得分是: %d:%d.\n", a1, a2);
	}
	printf("游戏结束. ");
	if (a1 == a2)
		printf("%s 和 %s 平局. ", p[1], p[2]);
	else printf("祝贺 %s 获得胜利. ", a1 > a2 ? p[1] : p[2]);
	printf("最终的得分比是: %d:%d.\n", a1, a2);
	system("pause");
	return 0;
}
