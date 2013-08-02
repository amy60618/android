#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "UILayer.h"
USING_NS_CC;

class HelloWorld : public cocos2d::CCLayer
{
public:
	HelloWorld():enemymovetick(0),isreduce(false) {};
	CCSprite *gameplayer;
	CCSprite *enemy;
	double vmove;
	int hmove;
	int enemymovetick;
	CCSize ownsize;
	CCSize othersize;
	bool isreduce;
	UILayer *ui;
	

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	void update(float dt);
	void playermaphcollision();
	void playermapvcollision();
	void enemytick();
	bool iscollision(CCSprite *mysprite, CCSprite *testsprite);
	void resetreduce(float dt);
};


#endif // __HELLOWORLD_SCENE_H__
