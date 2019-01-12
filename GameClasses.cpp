#include"GameClasses.h"
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<map>
#include<set>
#include<conio.h>

namespace Game {

map<string, IMAGE> img_src;

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

		//substitute
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

//zq编写 必属精品
void GameMap::SaveAs(string filename) {

	ofstream fout{ filename };

	fout << floors << " " << width << " " << height << endl;
	fout << "*" << endl;

	Player player = GetPlayer();
	fout << player_f << " " << player_x << " " << player_y << " ";
	fout << player.GetStat("HP") << " " << player.GetStat("attack") << " " << player.GetStat("defense") << " ";
	fout << player.GetStat("yellow_key_num") << " " << player.GetStat("red_key_num") << " " << player.GetStat("blue_key_num") << endl;
	fout << "*" << endl;


	set<string> s;
	for (int k = 0; k < floors*height*width; ++k)
	{
		int h = k / (height*width);
		int a1 = k % (height*width);
		int y = a1 / width;
		int x = a1 % width;
		GameObject&Q = *data[_GetHashedIndex(x, y, h)];
		if (Q.GetKind() == "monster")
		{
			Monster* pmonster = (Monster*)data[_GetHashedIndex(x, y, h)];
			if (!s.count(pmonster->name)) {
				fout << pmonster->name << " " << pmonster->HP << " " << pmonster->read_atk() << " " << pmonster->read_dfs() << " " << endl;
				
				s.insert(pmonster->name);
			}
		}
	}
	fout << "*" << endl;

	for (int k = 0; k < floors*height*width; ++k)
	{
		int h = k / (height*width);
		int a1 = k % (height*width);
		int y = a1 / width;
		int x = a1 % width;
		GameObject&Q = *data[_GetHashedIndex(x, y, h)];
		if (Q.GetKind() == "wall")
		{
			fout << h << " " << x << " " << y << endl;	
		}
	}
	fout << "*" << endl;

	for (int k = 0; k < floors*height*width; ++k)
	{
		int h = k / (height*width);
		int a1 = k % (height*width);
		int y = a1 / width;
		int x = a1 % width;
		GameObject&Q = *data[_GetHashedIndex(x, y, h)];
		if (Q.GetKind() == "stair")
		{
			Stair* pstair = (Stair*)data[_GetHashedIndex(x, y, h)];
			fout << h << " " << x << " " << y << " " << pstair->GetTargetFloor() << " " << pstair->GetTargetX() << " " << pstair->GetTargetY() << endl;
		}
	}
	fout << "*" << endl;

	for (int k = 0; k < floors*height*width; ++k)
	{
		int h = k / (height*width);
		int a1 = k % (height*width);
		int y = a1 / width;
		int x = a1 % width;
		GameObject&Q = *data[_GetHashedIndex(x, y, h)];
		if (Q.GetKind() == "door")
		{
			Door* pdoor = (Door*)data[_GetHashedIndex(x, y, h)];
			fout << h << " " << x << " " << y << " " << pdoor->GetColor() << endl;
		}
	}
	fout << "*" << endl;

	for (int k = 0; k < floors*height*width; ++k)
	{
		int h = k / (height*width);
		int a1 = k % (height*width);
		int y = a1 / width;
		int x = a1 % width;
		GameObject&Q = *data[_GetHashedIndex(x, y, h)];
		if (Q.GetKind() == "item")
		{
			Item* pitem = (Item*)data[_GetHashedIndex(x, y, h)];
			fout << h << " " << x << " " << y << " " << pitem->GetMinorKind() << " " << pitem->GetEffect() << endl;
		}
	}
	fout << "*" << endl;

	for (int k = 0; k < floors*height*width; ++k)
	{
		int h = k / (height*width);
		int a1 = k % (height*width);
		int y = a1 / width;
		int x = a1 % width;
		GameObject&Q = *data[_GetHashedIndex(x, y, h)];
		if (Q.GetKind() == "monster")
		{
			Monster* pmonster = (Monster*)data[_GetHashedIndex(x, y, h)];
			fout << h << " " << x << " " << y << " " << pmonster->name << endl;
		}
	}
	fout << "*" << endl;

	fout << boss_f << " " << boss_x << " " << boss_y << endl;
	fout << "*" << endl;
}


bool is_new_card(const vector<Card *> &Cardlist, Game::Monster *pmonster) {
	for (Card *loadedCard : Cardlist) {
		if (loadedCard->name == pmonster->name) return false;
	}
	return true;
}
vector<Card*> GameMap::GetMonster() {
	vector<Card *> Cardlist;
	// Go through this floor.
	for (int x = 0; x != width; ++x)
	{
		for (int y = 0; y != height; ++y)
		{
			Game::GameObject* piterator{ &at(x, y,cur_floor) };
			if (piterator->GetKind() == "monster")
			{
				Game::Monster* pmonster{ dynamic_cast<Monster*>(piterator) };// COPY ITERATOR
				if (!is_new_card(Cardlist, pmonster)) continue;
				// A new monster
				Card *pcard{ new Card{} }; // default status: cannot fight
				if (pmonster->try_fight(GetPlayer().GetStat("HP"),
					GetPlayer().GetStat("atk"), GetPlayer().GetStat("dfs")))
					// Able to fight. Show its data.
				{
					pcard->SetCard(pmonster);
				}
				else
					pcard->SetName(pmonster->name);
				Cardlist.push_back(pcard);
			}
		}
	}
	return Cardlist;
}

void Card::SetCard(Game::Monster *pmonster) {
	name = pmonster->name;
	{
		ostringstream ss;
		ss << "攻击力 " << setw(display_wide) << pmonster->read_atk();
		text_attack.SetText(ss.str());
	}
	{
		ostringstream ss;
		ss << "防御力 " << setw(display_wide) << pmonster->read_dfs();
		text_defense.SetText(ss.str());
	}
	{
		ostringstream ss;
		ss << "生命值 " << setw(display_wide) << pmonster->HP;
		text_HP.SetText(ss.str());
	}
	text_name.SetText(name);
	img_monster.SetImage(name);
	img_monster.SetMask(name + "_mask");
}

Handbook::Handbook(vector<Game::Card *>& initCardlist) : Cardlist{ initCardlist }
{
	text_title.SetSize(40);
	AttachWidget(&text_title);
	int on_left{ 0 };
	for (auto card : initCardlist)
	{// 2 Card per row. x change via %, y change via row width multiples a half on_left.
		dynamic_cast<Widget*>(card)->SetPos(50 + 512 * (on_left % 2), 100 + 100 * (on_left / 2));
		AttachWidget(dynamic_cast<Widget*>(card));
		++on_left;
	}
}
void Handbook::Refresh(vector<Game::Card *>& newCardlist)
{
	/*for (auto oldcard : Cardlist) delete (oldcard);
	Cardlist = newCardlist;
	widgets = {};
	AttachWidget(&text_title);
	int on_left{ 0 };
	for (auto card : newCardlist)
	{
		dynamic_cast<Widget*>(card)->SetPos(50 + 512 * (on_left % 2), 100 + 100 * (on_left / 2));
		AttachWidget(dynamic_cast<Widget*>(card));
		++on_left;
	}*/
	throw runtime_error{ "this function is not implemented" };
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
		if(!kbhit())Sleep(20);
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
			if (!kbhit()) Sleep(kind=="game"?20:80);
		}
		y += 50;
		if (!kbhit()) Sleep(400);
	}
	system("pause");
	Fade();
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
ostream& operator << (ostream& out, Plot& p)
{
	out << p.kind;
	if (p.kind == "game")
		out << p.f << p.x << p.y;
	for (string str : p.plots)
	{
		out << str << endl;
	}
	return out;
}
}