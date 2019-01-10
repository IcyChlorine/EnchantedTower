//
//  monster.cpp
//  monster
//
//  Created by 李天舒 on 2018/12/9.
//  Copyright © 2018 李天舒. All rights reserved.
//
// select kinds to initialize monsters :
// F: frj
// s: skeleton "mini-skelly"
// b: bat "batty-cutie"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "Monster.h"
#include "GameClasses.h"

using std::cout;
//player_Combat_Data p_CD;
// I recommend creating sth like this for the player too ( like what I did in 'player_Combat_Data.h').
// However it's up to you. All I need are HP, attack and defense data.
namespace Game {
/*Monster::Monster(string k) : GameObject{ "monster" }
{
	char _k = k[0];
	switch (_k) {
	case 'F': {
		set_combat_data(frj[0], frj[1], frj[2]);
		name = "frj";
		break;
	}
	case 's': {
		set_combat_data(mini_skelly[0], mini_skelly[1], mini_skelly[2]);
		name = "mini-skelly";
		break;
	}
	case 'b': {
		set_combat_data(batty_cutie[0], batty_cutie[1], batty_cutie[2]);
		name = "batty-cutie";
		break;
	}
	default: throw Monster_errors{};
	}
}*/

void Monster::set_combat_data(int a, int d, int H)
{
	if (HP > 0)
		throw Monster_errors{};
	atk = a;
	dfs = d;
	HP = H;
}

double Monster::read_atk()
{
	return atk;
}

double Monster::read_dfs()
{
	return dfs;
}

bool Monster::try_fight(int p_HP, int p_atk,int p_dfs)
//return 1 for a triumph, 0 for a defeat
{
	//able =  ( (p_atk > dfs) && ((HP * (atk - p_dfs)) <= (p_HP * (p_atk - dfs)))) ? true : false; // player can harm monster; monster die first
	// I am also wondering about lowering the fighting requirements to 'causing harm' only. Then the player could die and the game process
	// would be saved. So if the player have something like 'a 2nd life', he could resume his game.
	able = (p_atk > dfs);
	return able;
}

bool Monster::fight(int& p_HP, int p_atk, int p_dfs)
// return 1 for a tiumph, 0 for a defeat
{
	if (!able)
		throw Monster_errors{};

	// draw fighting page. Here I print the fighting process out

	cout << "Start fighting " << name << "!\n";
	cout << "PlayerHP: " << p_HP << "\tMonsterHP: " << HP << '\n';
	while (p_HP > 0) {
		HP -= p_atk - dfs; // animation here; sleep(1000)
		cout << "PlayerHP: " << p_HP << "\tMonsterHP: " << HP << '\n';
		if (HP <= 0) {
			alive = false; // monster died
			cout << "Remarkable job!\n";
			return 1;
		}
		p_HP -= (atk - p_dfs >= 0) ? atk - p_dfs : 0; // animation here; sleep(1000)
		cout << "PlayerHP: " << p_HP << "\tMonsterHP: " << HP << '\n';
	}
	cout << "Ahhhh! I am dead.\n";
	return 0; // player died
}

}