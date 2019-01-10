#include"GameClasses.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
using std::ifstream;
using std::stringstream;
using std::map;

namespace Game {
map<string, IMAGE> img_src;
/*GameObject::GameObject(std::string kind, IMAGE* pimg) :kind{ kind } 
{
	this->pimg = new IMAGE;
	SetImage(pimg); 
}
GameObject::GameObject(string kind, string src_filepath) :kind{ kind } 
{
	pimg = new IMAGE;
	SetImage(src_filepath);
}*/
GameObject& GameMap::at(int x, int y,int floor)
{
	if (floor == -1)floor = cur_floor;
	if (floor<0 || floor>=floors ||
		x<0 || x>=width ||
		y<0 || y>=height)
		throw out_of_range();
	return *(data[_GetHashedIndex(x, y, floor)]);
}
GameMap::GameMap(string filename)
{
	LoadFrom(filename);
}
GameMap::~GameMap()
{
	//for (auto x : data)
		//delete x;
	for (int i = 0; i < data.size(); i++)
		delete data[i];
}
void GameMap::LoadFrom(string filename)
{
	//初始化输入流
	ifstream fin{ filename };
	if (!fin) throw runtime_error{ "无效的地图文件！" };
	string line;

	while (true)//segment 1: basic data of the map
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line=="") continue;

		stringstream ss{ line };
		ss >> floors >> width >> height;
		GameObject* m = new GameObject{ "ground" };
		
		for (int i = 0; i < width*height*floors; i++)
		{
			GameObject* p = new GameObject{ *m };
			data.push_back(p);
		}
	}

	while (true)//segment 2: player's data
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line == "") continue;

		stringstream ss{ line };
		int HP{ 0 }, att{ 0 }, dfs{ 0 }, n1{ 0 }, n2{ 0 }, n3{ 0 };
		ss >> player_f>> player_x >> player_y >> HP >> att >> dfs >> n1 >> n2 >> n3;
		cur_floor = player_f;
		Player* pplayer = new Player{ player_f,player_x,player_y,
												HP,att,dfs,
												n1,n2,n3 };
		delete data.at(_GetHashedIndex(player_x, player_y, player_f));
		data[_GetHashedIndex(player_x, player_y, player_f)] = pplayer;
	}
	
	map<string, Monster> monster_m;
	while (true)//segment 3: monster's stat
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line == "") continue;

		stringstream ss{ line };
		string name;
		int HP{ 0 }, atk{ 0 }, dfs{ 0 };
		ss >> name>>HP>>atk>>dfs;
		loadimage(&img_src[name], ("src\\" + name + ".jpg").c_str());
		monster_m[name] = Monster{ name,HP,atk,dfs };
	}

	while (true)//segment 4:walls
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line == "") continue;

		stringstream ss{ line };
		int f{ 0 }, x{ 0 }, y{ 0 };
		ss >> f >> x >> y;
		at(x, y, f).SetKind("wall");
	}

	while (true)//segment 5:stairs
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line == "") continue;

		stringstream ss{ line };
		int f{ 0 }, x{ 0 }, y{ 0 };
		int f_to{ 0 }, x_to{ 0 }, y_to{ 0 };
		ss >> f >> x >> y >> f_to >> x_to >> y_to;

		//init object
		Stair* pstair = new Stair{ x_to,y_to,f_to,f_to<f };

		//substitutey
		delete data[_GetHashedIndex(x, y, f)];
		data[_GetHashedIndex(x, y, f)] = pstair;
	}

	while (true)//segment 6:doors
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line == "") continue;

		stringstream ss{ line };
		int f{ 0 }, x{ 0 }, y{ 0 };
		string color;
		ss >> f >> x >> y >> color;

		//init object
		Door* pdoor = new Door{ color };

		//substitute
		delete data[_GetHashedIndex(x, y, f)];
		data[_GetHashedIndex(x, y, f)] = pdoor;
	}

	while (true)//segment 7:items
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line.empty()) continue;

		int f{ 0 }, x{ 0 }, y{ 0 }, effect{ 0 };
		string kind;
		stringstream ss{ line };
		ss >> f >> x >> y >> kind >> effect;

		//init object
		Item* pitem = new Item{ kind ,effect };


		//substitute
		delete data[_GetHashedIndex(x, y, f)];
		data[_GetHashedIndex(x, y, f)] = pitem;
	}

	Monster* temp{ nullptr };
	while (true)//segment 8: monsters
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line == "") continue;

		stringstream ss{ line };
		int f{ 0 }, x{ 0 }, y{ 0 };
		string name;
		ss >> f >> x >> y >> name;

		//init object
		Monster* pmonster = new Monster{ monster_m[name] };

		//substitute
		delete data[_GetHashedIndex(x, y, f)];
		data[_GetHashedIndex(x, y, f)] = pmonster;
	}

	while (true)//segment 9: boss data
	{
		getline(fin, line);
		if (line == "*") break;
		if (line.substr(0, 2) == "//" || line == "") continue;
		
		stringstream ss{ line };
		ss >> boss_f >> boss_x >> boss_y;
	}

	fin.close();
}
void Player::SetStat(string name, int value)
{
	if (name == "HP") HP = value;
	else if (name == "attack") attack = value;
	else if (name == "defense") defense = value;
	else if (name == "yellow_key_num") yellow_key_num = value;
	else if (name == "red_key_num") red_key_num = value;
	else if (name == "blue_key_num") blue_key_num = value;
	else throw runtime_error{ "Player::AddStat(): unknown Stat" };
	return;
}
void Player::AddStat(string name, int dx)
{
	if (name == "HP") HP += dx;
	else if (name == "attack") attack += dx;
	else if (name == "defense") defense += dx;
	else if (name == "yellow_key_num") yellow_key_num += dx;
	else if (name == "red_key_num") red_key_num += dx;
	else if (name == "blue_key_num") blue_key_num += dx;
	else throw runtime_error{ "Player::AddStat(): unknown Stat" };
	return;
}
int Player::GetStat(string name)
{
	if (name == "HP") return HP;
	else if (name == "attack" || name=="atk") return attack;
	else if (name == "defense" || name=="dfs") return defense;
	else if (name == "yellow_key_num") return yellow_key_num;
	else if (name == "red_key_num") return red_key_num;
	else if (name == "blue_key_num") return blue_key_num;
	else throw runtime_error{ "Player::GetStat(): unknown Stat" };
}
void Fade()
{
	setrop2(R2_MASKPEN);
	for (double i = 1.0f; i >= 0.0f; i -= 0.05f) {
		setfillcolor(HSVtoRGB(0.0f, 0.0f, sqrt(i)));
		solidrectangle(0, 0, 1024, 768);
		//system("pause");
		Sleep(80);
	}
	setrop2(R2_COPYPEN);
}
void Plot::Play()
{
	//cleardevice();
	Fade();

	plots.push_back(string{ " " });
	plots.push_back(string{ "按任意键以继续......" });
	settextstyle(35, 0, "楷体");

	int y{ 65 };
	for (auto str:plots)
	{
		if (str == "cls")
		{
			Fade();
			y = 65;
			continue;
		}
		for (double t = 0; t < 1; t += 0.05)
		{
			settextcolor(HSVtoRGB(0.0f, 0.0f, t));
			outtextxy(75, y, str.c_str());
			Sleep(80);
		}
		y += 50;
		Sleep(400);
	}
	system("pause");
}
istream& operator >> (istream& in, Plot& p)
{
	in >> p.kind;
	if (p.kind == "game")
		in >> p.f >> p.x >> p.y;
	string str;
	p.plots.clear();
	while (in >> str && str != "*")
	{
		p.plots.push_back(str);
	}
	return in;
}
}