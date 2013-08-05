#ifndef __HUDLAYER_H__
#define __HUDLAYER_H__

#include "cocos2d.h"

USING_NS_CC;

class HudLayer: public CCLayer {
private:
	CCLabelTTF* _label;
public:
	virtual bool init();
	static CCScene* scene();
	void menuCloseCallBack(CCObject* pSender);
	CREATE_FUNC(HudLayer);
	void numCollectedChanged(int numCollected);
};

#endif