#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
USING_NS_CC;

class HelloWorld : public cocos2d::CCLayer
{
public:
    virtual bool init();  
    static cocos2d::CCScene* scene();
    CREATE_FUNC(HelloWorld);
	void tick(float dt);
	void kick(float dt);
	virtual void ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent);
private:
	b2World* _world;
	b2Body* _body;
	CCSprite* _ball;
};

#endif
