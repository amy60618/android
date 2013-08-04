#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "MyContactListener.h"

#define PTM_RATIO 32
USING_NS_CC;

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	b2World* _world;
	b2Body* _groundBody;
	b2Fixture* _bottomFixture;
	b2Fixture* _ballFixture;
	b2Body* _paddleBody;
	b2Fixture* _paddleFixture;
	b2MouseJoint* _mouseJoint;
	MyContactListener* _contactListener;

	void tick(float dt);
	void dealloc();
	virtual void ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent);
	virtual void ccTouchesMoved(CCSet* pTouches, CCEvent* pEvent);
//	void ccTouchesCancelled(CCSet* pTouches, CCEvent* pEvent);
	virtual void ccTouchesEnded(CCSet* pTouches, CCEvent* pEvent);
};

#endif // __HELLOWORLD_SCENE_H__
