#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "MyContactListener.h"

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

	HelloWorld() {};
	~HelloWorld();
	void tick(float dt);
	void ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent);
	void ccTouchesMoved(CCSet* pTouches, CCEvent* pEvent);
	void ccTouchesEnded(CCSet* pTouches, CCEvent* pEvent);
	void createBullets(int count);
	bool attachBullet();
	void resetGame();
	void createTarget(char   *imageName,
					  CCPoint position,
					  float   rotation,
					  bool	  isCircle,
					  bool    isStatic,
					  bool    isEnemy);
	void createTargets();
	void resetBullet();
private:
	b2World* m_world;
	b2Body* m_groundBody;
	b2RevoluteJoint *m_armJoint;
	b2MouseJoint *m_mouseJoint;
	b2Fixture* m_armFixture;
	b2Body* m_armBody;
	std::vector<b2Body*> m_bullets;
	int m_currentBullet;
	b2Body* m_bulletBody;
	b2WeldJoint* m_bulletJoint;
	bool m_releasingArm;
	vector<b2Body*> *targets;
	vector<b2Body*> *enemies;
	MyContactListener* contactListener;
};

#endif // __HELLOWORLD_SCENE_H__
