#ifndef __UILAYER_H__
#define __UILAYER_H__

#include "cocos2d.h"
USING_NS_CC;

class UILayer: public CCLayer {
private:
	int life;
public:
	UILayer():life(100) {};
	int getlife();
	void setlife(int tmp);
	void draw();
};

#endif