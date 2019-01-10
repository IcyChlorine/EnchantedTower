//
//  Monster.h
//  monster
//
//  Created by 李天舒 on 2018/12/9.
//  Copyright © 2018 李天舒. All rights reserved.
//
// 	Modified by StarSky on 2018/12/17.
//
/*#ifndef Monster_h
#define Monster_h

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "GameClasses.h"
using namespace std;

namespace Game
{
class Monster :public GameObject
{
public:
	//char kind{ ' ' };
	string name{ ' ' };
	int HP{ 0 };
	bool alive{ true };

	bool fight(int& p_HP, const int p_atk, const int p_dfs);

	double read_atk();
	double read_dfs();

	bool try_fight(int p_HP, const int p_atk, const int p_dfs);

	Monster(string k);
	Monster(int HP, int attack, int defense)
		:GameObject{ "monster" }, HP{ HP }, atk{ attack }, dfs{ defense } {}
	void set_combat_data(int atk, int dfs, int HP);

	class Monster_errors {};

private:
	int atk{ 0 };
	int dfs{ 0 };

	bool able{ false }; // able to fight agianst
};

const vector<int> frj{ 10000, 10000, 99999999 };
const vector<int> mini_skelly{ 1, 1, 5 };
const vector<int> batty_cutie{ 2, 2, 10 };
// ready to be added
}
#endif /* Monster_h */
