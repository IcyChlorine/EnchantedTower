#pragma once
#ifndef _WIDGET_CLASSES_H
#define _WIDGET_CLASSES_H

#include<string>
#include<vector>
#include<map>
#include<graphics.h>
using namespace std;

namespace Game {

extern map<string, IMAGE> img_src;
struct point
{
	int x{ 0 };
	int y{ 0 };
	point(int x = 0, int y = 0) :x{ x }, y{ y } {}
};

class Widget
{
protected:
	point pos{ 0,0 };
public:
	Widget(int x = 0, int y = 0) :pos{ x,y } {}
	void SetPos(int x_new, int y_new) { pos.x = x_new, pos.y = y_new; }
	virtual void Render() {}
};
class Text : public Widget
{
private:
	string text;
	int size{ 35 };
public:
	Text(const string& str) :text{ str } {}
	Text(int x, int y, const string& str) : Widget{ x,y }, text{ str } {}

	void SetText(string& new_text) { text = new_text; }
	void SetSize(int size_new) { size = size_new; }
	void Render() override
	{
		settextstyle(size, 0, nullptr);
		outtextxy(pos.x, pos.y, text.c_str());
		settextstyle(20, 0, nullptr);
	}
};
class Image :public Widget
{
private:
	string src_name, src_mask_name;
public:
	Image(const string& src_name, const string& src_mask_name = "") :
		src_name(src_name), src_mask_name(src_mask_name) {}

	void SetImage(const string& src_name_new)
	{
		src_name = src_name_new;
	}
	void SetMask(const string& src_mask_name_new)
	{
		src_mask_name = src_mask_name_new;
	}
	void Render() override
	{
		if (!src_mask_name.empty())
		{
			putimage(pos.x, pos.y, &img_src[src_mask_name], SRCPAINT);
			putimage(pos.x, pos.y, &img_src[src_name], SRCAND);
		}
		else
			putimage(pos.x, pos.y, &img_src[src_name]);
	}
};
class Container :public Widget
{
private:
	vector<Widget*> widgets;
public:
	Container(int x = 0, int y = 0) :Widget{ x,y } {}
	void AttachWidget(Widget* pWidget)
	{
		widgets.push_back(pWidget);
	}
	void Render() override
	{
		setorigin(pos.x, pos.y);
		for (auto x : widgets)
			x->Render();
		setorigin(0, 0);
		//把相对坐标移回去
	}
};

}


#endif // !_WIDGET_CLASSES_H
