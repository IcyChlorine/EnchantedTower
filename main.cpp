#include<graphics.h>
#include<conio.h>
#include"GameClasses.h"
#include<fstream>
#include<sstream>


#pragma comment( lib, "msimg32.lib")
#pragma comment( lib, "winmm.lib")
#include "mmsystem.h"//导入声音头文件 

namespace Game {
void InitImages();
void InitGame();
void Menu();
void MainGameLoop();
}

int main() 
{
	using namespace Game;
	
	InitGame();
	
	while (true)
	{
		Menu();
		MainGameLoop();
	}
	closegraph();
	return 0;
}


void Game::InitImages()
{
	ifstream fin{ "src\\src_list.txt" };
	if (!fin) throw runtime_error{ "resources list file not found!" };

	string src_name, src_path;
	while (fin >> src_name>>src_path)
	{
		loadimage(&img_src[src_name], src_path.c_str());
		for (int i = 0; i < 5 && !img_src[src_name].getwidth(); i++)//若读取失败，再次尝试读取，尝试5次
		{
			loadimage(&img_src[src_name], src_path.c_str());
			Sleep(20);
		}
		if (!img_src[src_name].getwidth())
			throw runtime_error{ "fail to load src file:" + src_path };
	}
}
void Game::InitGame()
{
	InitImages();                        //加载图片
	PlaySound("EnchantedTower.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	initgraph(1024, 768);            //初始化画布
	setbkmode(TRANSPARENT);
}
void Game::Menu()
{
	Container con_menu{ 0,0 };
	Text text_EnchantedTower{ 400,200,"魔塔" };
	text_EnchantedTower.SetSize(100);
	Text text_op_hint{ 400,500,"PRESS ANY KEY TO CONTINUE" };
	text_op_hint.SetSize(35);
	con_menu.AttachWidget(&text_EnchantedTower);
	con_menu.AttachWidget(&text_op_hint);
	con_menu.Render();

	system("pause");
	//cleardevice();
}

void Game::MainGameLoop()
{
	//初始化
	//初始化剧情
	PlotManager p_mng;
	p_mng.LoadFrom("plots.txt");
	p_mng.PlayBeginPlot();
	//初始化地图
	GameMap theMap{ "cs_map.txt" };
	theMap.SetPos(300, 50);
	theMap.Render();
	//初始化玩家状态栏
	PlayerStatBar bar_player_stat{ theMap.GetPlayer() };
	bar_player_stat.SetPos(50, 50);
	bar_player_stat.Render();

	while (true)//主循环
	{
		//获取输入
		char c{ (char)_getch() };
		if (GetAsyncKeyState(VK_UP)){getch(); c = 'w';}
		if (GetAsyncKeyState(VK_LEFT)) {getch(); c = 'a';}
		if (GetAsyncKeyState(VK_RIGHT)) {getch(); c = 'd';}
		if (GetAsyncKeyState(VK_DOWN)) {getch(); c = 's';}

		int dx{ 0 }, dy{ 0 };
		c = tolower(c);
		if (c != 'w' && c != 'a' && c != 's' && c != 'd') continue;
		if (c == 'a') dx = -1;
		if (c == 'd') dx = 1;
		if (c == 's') dy = 1;
		if (c == 'w') dy = -1;

		Player& player{ theMap.GetPlayer() };
		try {
			theMap.at(player.x + dx, player.y + dy);
		}
		catch (...)
		{
			continue;
		}

		p_mng.TryPlayPlotAt(player.floor, player.x+dx, player.y+dy);
		GameObject* ptarget{ &theMap.at(player.x + dx, player.y + dy) };
		if (ptarget->GetKind() == "ground")
		{
			theMap.MovePlayer(dx, dy);
		}
		else if (ptarget->GetKind() == "wall")
		{
			//do nothing at ALLLLLLLLLL!
		}
		else if (ptarget->GetKind() == "stair")
		{
			Stair* pstair = (Stair*)ptarget;
			theMap.MovePlayerTo(pstair->GetTargetX(), pstair->GetTargetY(), pstair->GetTargetFloor());
		}
		else if (ptarget->GetKind() == "item")
		{
			Item* pitem = (Item*)ptarget;
			if (pitem->GetMinorKind() == "sword")
			{
				player.AddStat("attack", pitem->GetEffect());
			}
			if (pitem->GetMinorKind() == "shield")
			{
				player.AddStat("defense", pitem->GetEffect());
			}
			if (pitem->GetMinorKind() == "potion" || pitem->GetMinorKind() == "adv_potion")
			{
				player.AddStat("HP", pitem->GetEffect());
			}
			if (pitem->GetMinorKind() == "yellow_key")
			{
				player.AddStat("yellow_key_num", 1);
			}
			if (pitem->GetMinorKind() == "red_key")
			{
				player.AddStat("red_key_num", 1);
			}
			if (pitem->GetMinorKind() == "blue_key")
			{
				player.AddStat("blue_key_num", 1);
			}
			theMap.WipeOut(player.x + dx, player.y + dy);

		}
		else if (ptarget->GetKind() == "door")
		{
			Door* pdoor = (Door*)ptarget;
			string color{ pdoor->GetColor() };
			if (player.GetStat(color + "_key_num"))
			{
				theMap.WipeOut(player.x + dx, player.y + dy);
				player.AddStat(color + "_key_num", -1);
			}
		}
		else if (ptarget->GetKind() == "monster")
		{
			Monster* pmonster = (Monster*)ptarget;
			if (pmonster->try_fight(player.GetStat("HP"), player.GetStat("atk"), player.GetStat("dfs")))
			{//如果真的完全打不过，那就别打了
				bool win = Fight(player, *pmonster);
				if (win)
				{
					if (theMap.Win())//考虑是不是打死了boss
					{
						MessageBox(GetHWnd(), "YOU WIN!", "", 0);
						p_mng.PlayEndPlot();
						cleardevice();
						return;
					}
					theMap.WipeOut(player.x + dx, player.y + dy);
				}
				else//死了就输了
				{
					MessageBox(GetHWnd(), "YOU LOSE!", "", 0);
					cleardevice();
					return;
				}
			}
		}

		BeginBatchDraw();
		cleardevice();

		bar_player_stat.UpdatePlayerStat();
		bar_player_stat.Render();
		theMap.Render();
		EndBatchDraw();
	}
}
string str(int num)
{
	ostringstream ss;
	ss << num;
	return ss.str();
}
bool Game::Fight(Player& player, Monster& monster)
{
	cleardevice();
	//bool finished{ false };

	Text text_player_HP{ 0,0,string{"生命值"}+str(player.GetStat("HP")) },
		text_player_attack{ 0,50,string{ "攻击力" }+str(player.GetStat("attack")) },
		text_player_defense{ 0,100,string{ "防御力" }+str(player.GetStat("defense")) },
		text_monster_HP{ 0,0,string{ "生命值" }+str(monster.HP) },
		text_monster_attack{ 0,50,string{ "攻击力" }+str(monster.read_atk()) },
		text_monster_defense{ 0,100,string{ "防御力" }+str(monster.read_dfs()) };
	Container player_stat_bar{ 300,450 },
		monster_stat_bar{ 500,450 };
	player_stat_bar.AttachWidget(&text_player_HP);
	player_stat_bar.AttachWidget(&text_player_attack);
	player_stat_bar.AttachWidget(&text_player_defense);
	monster_stat_bar.AttachWidget(&text_monster_HP);
	monster_stat_bar.AttachWidget(&text_monster_attack);
	monster_stat_bar.AttachWidget(&text_monster_defense);

	player.RenderAt(300, 300);
	monster.RenderAt(500, 300);
	player_stat_bar.Render();
	monster_stat_bar.Render();
	while (true)
	{
		Sleep(700);

		if (player.GetStat("HP") <= 0) return false;//lose
		if (monster.HP <= 0) return true;//win

		monster.Injure(player.GetStat("atk") - monster.read_dfs());
		player.Injure(monster.read_atk() - player.GetStat("dfs"));//fight

		//renew data
		text_player_HP.SetText("生命值" + str(player.GetStat("HP")));
		text_monster_HP.SetText("生命值" + str(monster.HP));

		cleardevice();
		player.RenderAt(300, 300);
		monster.RenderAt(500, 300);
		player_stat_bar.Render();
		monster_stat_bar.Render();
		
	}
	
}