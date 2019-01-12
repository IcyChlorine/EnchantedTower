#include<graphics.h>
#include<conio.h>
#include"GameClasses.h"
#include<fstream>
#include<sstream>

#pragma comment( lib, "msimg32.lib")
#pragma comment( lib, "winmm.lib")
#include "mmsystem.h"//��������ͷ�ļ� 

namespace Game {
void Fade();
void InitImages();
void InitGame();
string Menu();
void MenuHelp();
void MenuAbout();
void MainGameLoop(bool new_game_flag=true);
bool Fight(Player& player, Monster& monster);
}

int main() 
{
	using namespace Game;
	
	InitGame();
	
	while (true)
	{
		string str = Menu();
		if(str=="new_game"){ 
			MainGameLoop(); 
		}
		else if(str=="load"){
			try {
				MainGameLoop(false);
			}
			catch (runtime_error& e)
			{
				MessageBox(GetHWnd(), "�浵�ļ������ڻ����𻵣�","", MB_OK);
				continue;
			}
		}
		else if(str=="help"){
			MenuHelp();
		}
		else if(str=="about"){
			MenuAbout(); 
		}
		else if(str=="quit")
		{
			break;
		}
		
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
		for (int i = 0; i < 5 && !img_src[src_name].getwidth(); i++)//����ȡʧ�ܣ��ٴγ��Զ�ȡ������5��
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
	InitImages();                        //����ͼƬ
	PlaySound("EnchantedTower.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	initgraph(1024, 768);            //��ʼ������
	setbkmode(TRANSPARENT);

	settextstyle(35, 0, "����");		//��������
}
string Game::Menu()
{
	Container con_menu{ 700,250 };
	Text menu_start{ 0,0,"New Game" };
	Text menu_load{ 0,50,"Load" };
	Text menu_help{ 0,100,"Help" };
	Text menu_about{ 0,150,"About Us" };
	Text menu_quit{ 0,200,"Quit" };
	Text menu_ptr{ -50,0,">" };
	con_menu.AttachWidget(&menu_start);
	con_menu.AttachWidget(&menu_load);
	con_menu.AttachWidget(&menu_help);
	con_menu.AttachWidget(&menu_about);
	con_menu.AttachWidget(&menu_quit);
	con_menu.AttachWidget(&menu_ptr);
	int selected_button{ 1 };//1-5

	putimage(0, 0, &img_src["menu_background"]);
	con_menu.Render();

	while (true)
	{
		if (_getch())//��ⰴ��
		{	
			if (GetAsyncKeyState(VK_UP))
			{
				_getch();//�Ե�һ���ַ�����Ϊ���¼�����������char
				if (selected_button == 1) {
					selected_button = 5;
				}
				else {
					--selected_button;
				}
			}
			else if (GetAsyncKeyState(VK_DOWN))
			{
				_getch();
				if (selected_button == 5) {
					selected_button = 1;
				}
				else {
					++selected_button;
				}
			}
			else if(GetAsyncKeyState(VK_RETURN)){
				if (selected_button == 1) {
					Fade();
					return string{ "new_game" };
				}
				else if (selected_button == 2)
				{
					Fade();
					return string{ "load" };
				}
				else if (selected_button == 3) {
					Fade();
					return string{ "help" };
				}
				else if (selected_button == 4) {
					Fade();
					return string{ "about" };
				}
				else if (selected_button == 5) { 
					Fade();
					return string{ "quit" }; 
				}
			}
		}
		//�ػ�
		menu_ptr.SetPos(-50, (selected_button - 1) * 50);
		putimage(0, 0, &img_src["menu_background"]);
		con_menu.Render();
	}
}
void Game::MenuAbout() {
	
	Container con_menu { 350, 250 };
	Text about_us{ 0,0,"�����ˣ�" };
	Text producer{ 0,50,"��� ��ѩ�� �� ���� ������ " };
	Text hehe{0,100,"EL PSY CONGROO!" };
	Text version{0,150,"�汾: 3.14"};
	con_menu.AttachWidget(&about_us);
	con_menu.AttachWidget(&producer);
	con_menu.AttachWidget(&hehe);
	con_menu.AttachWidget(&version);
	putimage(0, 0, &img_src["about_background"]);

	con_menu.Render();

	system("pause");
	Fade();
}
void Game::MenuHelp() 
{
	Container con_menu{ 200, 200 };
	Text t1{ 0,0,"���̿��ƣ�" };
	Text t2{ 0,50,"�����пɰ�������Լӿ�" };
	Text t3{ 0,100,"С���̿����������ң��س�ȷ��" };
	Text t4{ 0,150,"H����ʾ�����ֲ�" };
	Text t5{ 0,200,"��Ϸ��S���浵��Q���ص����˵�" };

	con_menu.AttachWidget(&t1);
	con_menu.AttachWidget(&t2);
	con_menu.AttachWidget(&t3);
	con_menu.AttachWidget(&t4);
	con_menu.AttachWidget(&t5);

	putimage(0, 0, &img_src["about_background"]);
	con_menu.Render();

	system("pause");
	Fade();
}
void Game::MainGameLoop(bool new_game_flag)
{
	//��ʼ��
	//��ʼ������
	PlotManager p_mng;
	p_mng.LoadFrom(new_game_flag?"plots.plot":"save.plot");
	if(new_game_flag)p_mng.PlayBeginPlot();//ֻ������Ϸ���п�ͷ�ľ���
	//��ʼ����ͼ
	GameMap theMap{ new_game_flag?"theMap.map":"save.map" };
	theMap.SetPos(300, 50);
	//��ʼ�����״̬��
	PlayerStatBar bar_player_stat{ theMap.GetPlayer() };
	bar_player_stat.SetPos(50, 50);
	//��ʼ����ʾ��
	Text text_save_hint{ 50,550,"S �浵" };
	Text text_quit_hint{ 50,600,"Q �˳�" };

	Container con_main{ 0,0 };
	con_main.AttachWidget((Widget*)&theMap);
	con_main.AttachWidget((Widget*)&bar_player_stat);
	con_main.AttachWidget((Widget*)&text_save_hint);
	con_main.AttachWidget((Widget*)&text_quit_hint);

	con_main.Render();

	while (true)//��ѭ��
	{
		//��ȡ����
		char c{ (char)_getch() };
		static constexpr char direction = 'a';//ħ����������

		int dx{ 0 }, dy{ 0 };
		if (GetAsyncKeyState(VK_UP)) { dy = -1; _getch(); c = direction; }//���ڷ��������������ַ����룬�����Ҫ����getchһ���԰ѵڶ����ַ�����Ե�
		if (GetAsyncKeyState(VK_LEFT)) { dx = -1; _getch(); c = direction;}
		if (GetAsyncKeyState(VK_RIGHT)) { dx = 1; _getch(); c = direction;}
		if (GetAsyncKeyState(VK_DOWN)) { dy = 1; _getch(); c = direction;}

		c = tolower(c);
		if ( c!=direction && c!='h' && c!='s' && c!='q') continue;

		//�鿴�����ֲ������
		if (c == 'h') {
			cleardevice();
			Handbook handbook{ theMap.GetMonster() };
			handbook.Render();
			system("pause"); // Press any key to quit.
			cleardevice();
			con_main.Render();
			continue;
		}
		if (c == 's')//�浵
		{
			theMap.SaveAs("save.map");
			p_mng.SaveAs("save.plot");
			MessageBox(GetHWnd(), "�浵�ɹ���", "", MB_OK);
			continue;
		}
		if (c == 'q' && 
			MessageBox(GetHWnd(), "�˳����ܵ��¶�ʧ��ǰ��Ϸ���ȡ�ȷ����", "", MB_OKCANCEL))
		{
			return;
		}
		
		//else
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
			{//��������ȫ�򲻹����Ǿͱ����
				bool win = Fight(player, *pmonster);
				if (win)
				{
					if (theMap.Win())//�����ǲ��Ǵ�����boss
					{
						Fade();
						p_mng.PlayEndPlot();
						cleardevice();
						return;
					}
					theMap.WipeOut(player.x + dx, player.y + dy);
				}
				else//���˾�����
				{
					MessageBox(GetHWnd(), "YOU LOSE!", "", 0);
					Fade();
					return;
				}
			}
		}
		bar_player_stat.UpdatePlayerStat();
		BeginBatchDraw();
		cleardevice();
		con_main.Render();
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
	Image fire{ "fire","fire_mask" };

	Text text_player_HP{ 0,0,string{"����ֵ"}+str(player.GetStat("HP")) },
		text_player_attack{ 0,50,string{ "������" }+str(player.GetStat("attack")) },
		text_player_defense{ 0,100,string{ "������" }+str(player.GetStat("defense")) },
		text_monster_HP{ 0,0,string{ "����ֵ" }+str(monster.HP) },
		text_monster_attack{ 0,50,string{ "������" }+str(monster.read_atk()) },
		text_monster_defense{ 0,100,string{ "������" }+str(monster.read_dfs()) };
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
		Sleep(350);

		if (player.GetStat("HP") <= 0) return false;//lose
		if (monster.HP <= 0) return true;//win

		monster.Injure(player.GetStat("atk") - monster.read_dfs());

		text_monster_HP.SetText("����ֵ" + str(monster.HP));
		text_player_HP.SetText("����ֵ" + str(player.GetStat("HP")));

		cleardevice();
		monster.RenderAt(500, 300);
		player.RenderAt(300, 300);
		player_stat_bar.Render();
		monster_stat_bar.Render();
		fire.SetPos(500, 290);
		fire.Render();

		cleardevice();
		monster.RenderAt(530, 300);
		player.RenderAt(300, 300);
		player_stat_bar.Render();
		monster_stat_bar.Render();
		fire.Render();
	
		Sleep(100);
		cleardevice();
		monster.RenderAt(500, 300);
		player.RenderAt(300, 300);
		player_stat_bar.Render();
		monster_stat_bar.Render();
		


		Sleep(350);

		if (player.GetStat("HP") <= 0) return false;//lose
		if (monster.HP <= 0) return true;//win

		player.Injure(monster.read_atk() - player.GetStat("dfs"));//fight

		//renew data
		text_monster_HP.SetText("����ֵ" + str(monster.HP));
		text_player_HP.SetText("����ֵ" + str(player.GetStat("HP")));
		
		cleardevice();
		monster.RenderAt(500, 300);
		player.RenderAt(300, 300);
		player_stat_bar.Render();
		monster_stat_bar.Render();
		fire.SetPos(300, 290);
		fire.Render();

		cleardevice();
		monster.RenderAt(500, 300);
		player.RenderAt(270, 300);
		player_stat_bar.Render();
		monster_stat_bar.Render();
		fire.Render();
		Sleep(100);
		cleardevice();
		monster.RenderAt(500, 300);
		player.RenderAt(300, 300);
		player_stat_bar.Render();
		monster_stat_bar.Render();
	
		
	}
	
}