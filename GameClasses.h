#pragma once
#ifndef _GAME_CLASSES_H
#define _GAME_CLASSES_H

#include<graphics.h>
#include<sstream>
#include<fstream>
#include"WidgetClasses.h"
using namespace std;

namespace Game {

#include"Monster.h"

extern map<string, IMAGE> img_src;
//-----------------classes------------------//

class GameObject
{
protected:
	string  kind;

public:
	class GameObjectError {};
	GameObject(){ }
	GameObject(string kind) :kind{ kind } { }

	void SetKind(string kind_new) { kind = kind_new; }
	virtual string GetKind() const { return kind; }

	virtual void RenderAt(int x, int y)
	{
		putimage(x, y, &img_src[kind + "_mask"], SRCPAINT);
		putimage(x, y, &img_src[kind], SRCAND);
	}
};
class Player :public GameObject
{
public:
	int x { 0 }, y{ 0 };
	int floor{ 0 };
private:
	int HP{ 1000 };
	int attack{ 20 };
	int defense{ 15 };

	int yellow_key_num{ 3 };
	int red_key_num{ 1 };
	int blue_key_num{ 0 };

public:
	Player(int f, int x, int y, int HP, int attack, int defense, int n1, int n2, int n3)
		:GameObject{ "player" },
		floor{ f }, x{ x }, y{ y },
		HP{ HP }, attack{ attack }, defense{ defense },
		yellow_key_num{ n1 },
		red_key_num{ n2 },
		blue_key_num{ n3 } {}
	string GetKind() { return "player"; }

	void SetStat(string name, int value);//改变name属性的值
	void AddStat(string name, int dx);//同上。因为属性较多，分别写Set/Get会有很多函数，所以合并到一个函数里
	void Injure(int damage) 
	{
		if (damage < 0)damage = 0;
		HP -= damage; 
		if (HP < 0)HP = 0;
	}
	int GetStat(string name);
	void Move(int dx, int dy,int df=0) { x += dx, y += dy; return; }
	void MoveTo(int x_new, int y_new, int floor_new = -1)
	{
		x = x_new;
		y = y_new;
		if (floor_new >= 0) floor = floor_new;
		//floor_new=-1是floor不需要修改的标志
	}
	bool Dead() { return HP<=0; }
	
};
class Stair:public GameObject
{
public:
	int floor_to{ 0 };
	int x_to{ 0 };
	int y_to{ 0 };
	bool down{ false };
public:
	Stair(int x_to, int y_to, int floor_to,bool down=false) :GameObject{ "stair" },
		x_to{ x_to }, y_to{ y_to }, floor_to{ floor_to }, down{ down } {}
	void RenderAt(int x, int y)
	{
		if(!down)
			putimage(x, y, &img_src["stair"]);
		else
			putimage(x, y, &img_src["stair_down"]);

	}
	int GetTargetX() { return x_to; }
	int GetTargetY() { return y_to; }
	int GetTargetFloor() { return floor_to; }
};
class Door :public GameObject
{
private:
	string color;
public:
	Door(string color) :
		GameObject{ "door" }, color{ color } {}
	void SetColor(string color_new) { color = color_new; }
	string GetColor() const { return color; }
	void RenderAt(int x, int y)
	{
		putimage(x, y, &img_src[color + "_door"]);
	}
};
class Item:public GameObject
{
public:
	Item(string kind, int effect) :GameObject{ "item" }, 
		minor_kind{ kind }, effect{ effect } {}

	void SetMinorKind(string new_kind) { minor_kind = new_kind; }
	void SetEffect(int new_effect) { effect = new_effect; }
	string GetMinorKind() { return minor_kind; }
	int GetEffect() { return effect; }
	void RenderAt(int x, int y)
	{
		putimage(x, y, &img_src[minor_kind+"_mask"],SRCPAINT);
		putimage(x, y, &img_src[minor_kind],SRCAND);
	}
private:
	string minor_kind;
	int effect;
};
class Monster :public GameObject
{
public:
	string name;
	int HP{ 0 };
	bool alive{ true };

	bool fight(int& p_HP, const int p_atk, const int p_dfs);
	void Injure(int damage) 
	{
		if (damage < 0)damage = 0;
		HP -= damage;
		if (HP < 0)HP = 0;
	}

	double read_atk();
	double read_dfs();
	
	bool try_fight(int p_HP, const int p_atk, const int p_dfs);

	Monster() {}
	Monster(string k);
	Monster(string name, int HP, int attack, int defense)
		:GameObject{ "monster" },
		name{ name }, HP{ HP }, atk{ attack }, dfs{ defense } {}
	void set_combat_data(int atk, int dfs, int HP);

	void RenderAt(int x, int y)
	{
		putimage(x, y, &img_src[name + "_mask"], SRCPAINT);
		putimage(x, y, &img_src[name], SRCAND);
	}

	class Monster_errors {};
private:
	int atk{ 0 };
	int dfs{ 0 };

	bool able{ false }; // able to fight agianst
};

class GameMap:public Widget
{
private:
	vector<GameObject*> data;
	int width, height, floors;//地图的宽度，长度和层数
	int player_x, player_y, player_f;//玩家的位置
	int boss_x, boss_y, boss_f;//boss的位置
	int cur_floor{ 0 };//当前楼层
	int _GetHashedIndex(int x, int y, int h) { return h*width*height + y*width + x; }

	const int cell_width{ 50 };
public:
	GameMap(string finename);
	~GameMap();
	void LoadFrom(string filename);

	Player& GetPlayer() {
		Player* p = (Player*)data[_GetHashedIndex(player_x, player_y, player_f)];
		return *p;
	}
	void MovePlayer(int dx, int dy, int df = 0) 
	{
		if (player_x+dx < 0 || player_x+dx >= width ||
			player_y + dy < 0 || player_y + dy >= height ||
			player_f + df < 0 || player_f + df >= floors)
			return;//越界检查
		GetPlayer().Move(dx, dy, df);
		swap(data[_GetHashedIndex(player_x, player_y, player_f)],
			data[_GetHashedIndex(player_x + dx, player_y + dy, player_f + df)]);
		//交换对象位置
		player_x += dx;
		player_y += dy;
		player_f += df;//维护map类里的数据
		cur_floor += df; }
	void MovePlayerTo(int x_new, int y_new, int f_new = -1) 
	{ 
		if (x_new < 0 || x_new >= width ||
			y_new < 0 || y_new >= height ||
			f_new < 0 || f_new >= floors)
			return;
		GetPlayer().MoveTo(x_new, y_new, f_new);
		swap(data[_GetHashedIndex(player_x, player_y, player_f)],
			data[_GetHashedIndex(x_new, y_new, f_new)]);
		player_x = x_new;
		player_y = y_new;
		player_f = f_new;
		cur_floor = f_new; }

	void WipeOut(int x, int y, int f = -1)//把某个格子上的东西抹除掉，只剩下ground
	{
		if (f == -1) f = cur_floor;
		GameObject* p_new = new GameObject{ "ground" };
		delete data[_GetHashedIndex(x, y, f)];
		data[_GetHashedIndex(x, y, f)] = p_new;
	}

	bool Win()
	{
		if (data[_GetHashedIndex(boss_x, boss_y, boss_f)]->GetKind() == "ground") return true;
		Monster* p = (Monster*)data[_GetHashedIndex(boss_x, boss_y, boss_f)];
		return p->HP <= 0;
	}

	class out_of_range {};
	GameObject& at(int x, int y,int floor=-1);

	void Render()
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				putimage(pos.x + j*cell_width, pos.y + i*cell_width, &img_src["ground"]);
				data[_GetHashedIndex(j, i, cur_floor)]
					->RenderAt(pos.x + j*cell_width, pos.y + i*cell_width);
			}
		}
	}
		
};

class PlayerStatBar:public Container
{
private:
	Player& player;
	Text text_HP{ 0,0,"生命值 1000" };
	Text text_attack{ 0,50,"攻击力 20" };
	Text text_defense{ 0,100,"防御力 15" };
	Image img_yellow_key{ "yellow_key","yellow_key_mask" };
	Image img_red_key{ "red_key","red_key_mask" };
	Image img_blue_key{ "blue_key","blue_key_mask" };
	
	Text text_yellow_key_num{ 50,150,"x3" };
	Text text_red_key_num{ 50,200,"x1" };
	Text text_blue_key_num{ 50,250,"x0" };
public:
	PlayerStatBar(Player& player) :player{ player }
	{
		img_yellow_key.SetPos(0, 150);
		img_red_key.SetPos(0, 200);
		img_blue_key.SetPos(0, 250);
		AttachWidget(&text_HP);
		AttachWidget(&text_attack);
		AttachWidget(&text_defense);
		AttachWidget(&img_yellow_key);
		AttachWidget(&img_red_key);
		AttachWidget(&img_blue_key);
		AttachWidget(&text_yellow_key_num);
		AttachWidget(&text_red_key_num);
		AttachWidget(&text_blue_key_num);
		UpdatePlayerStat();
	}
	void UpdatePlayerStat()
	{
		{
			ostringstream ss;
			ss << "攻击力 " << player.GetStat("attack");
			text_attack.SetText(ss.str());
		}
		{
			ostringstream ss;
			ss << "防御力 " << player.GetStat("defense");
			text_defense.SetText(ss.str());
		}
		{
			ostringstream ss;
			ss << "生命值 " << player.GetStat("HP");
			text_HP.SetText(ss.str());
		}
		{
			ostringstream ss;
			ss << "x" << player.GetStat("yellow_key_num");
			text_yellow_key_num.SetText(ss.str());
		}
		{
			ostringstream ss;
			ss << "x" << player.GetStat("red_key_num");
			text_red_key_num.SetText(ss.str());
		}
		{
			ostringstream ss;
			ss << "x" << player.GetStat("blue_key_num");
			text_blue_key_num.SetText(ss.str());
		}
	}
};

class Plot
{
	friend istream& operator >> (istream& in, Plot& p);
	friend class PlotManager;
private:
	//触发条件
	string kind{ "game" };		//start/begin代表游戏开始时触发剧情，end/finish代表游戏结束时触发，game代表在游戏中触发
									//只有当kind为game时，下面的坐标才是有意义的
	int x{ -1 }, y{ -1 }, f{ -1 };	//当玩家到这个位置的时候触发
	vector<string> plots;		//剧情内容。每一个string一行。string="cls"代表清屏。
public:
	string GetKind() { return kind; }
	void Play();
};
class PlotManager
{
private:
	Plot begin_plot;
	Plot end_plot;
	vector<Plot> plots;
public:
	void LoadFrom(string filename)
	{
		ifstream fin{ filename };
		if (!fin) throw runtime_error{ "无效的剧情文件！" };
		Plot p;
		while (fin >> p)
		{
			if (p.GetKind() == "begin" || p.GetKind() == "start")begin_plot = p;
			else if (p.GetKind() == "end" || p.GetKind() == "finish")end_plot = p;
			else plots.push_back(p);
		}
	}
	void PlayBeginPlot() { begin_plot.Play(); }
	void PlayEndPlot() { end_plot.Play(); }
	void TryPlayPlotAt(int f,int x,int y)
	{
		for (auto& p : plots)
		{
			if (p.x == x && p.y == y && p.f == f)
			{
				p.Play();
				swap(p, plots.back());
				plots.pop_back();
				return;
			}
		}
	}
};
}



#endif // !_GAME_CLASSES_H

