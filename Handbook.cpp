//
//  Handbook.cpp
//  Handbook_of_Monster
//
//  Created by suntop on 2019/1/8.
//  Copyright © 2019年 Li Tianshu. All rights reserved.
//
//  2019/1/9 Changes:
//  2 line display Cards.
//  include <iomanip>.
//
//  2019/1/12 Changes:
//  improve name display codes in SetCard().

#include <vector>
#include <iomanip>
#include "Handbook.h"
#include "GameClasses.h"

namespace Game {



/////////////////////////////////////////////////////////////////////
//  Add below to MainGameloop().

/*
...
bar_player_stat.Render();

Handbook Hb{theMap.GetMonster()};

while (true)
{
	char c{ (char)_getch() };
	int dx{ 0 }, dy{ 0 };
	c = tolower(c);
	if (c != 'w' && c != 'a' && c != 's' && c != 'd' && c != 'h') continue;

	if (c == 'h') {
		cleardevice();
		Hb.Refresh(TheMap.GetMonster());
		Hb.Render();
		system( "pause" ); // Press any key to quit.
		cleardevice();
		TheMap.Render();
		bar_player_stat.Render();
	}

if (c == 'a') dx = -1;
 ...
 */
}