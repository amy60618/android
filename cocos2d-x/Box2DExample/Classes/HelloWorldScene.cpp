#include "HelloWorldScene.h"

USING_NS_CC;
#define PTM_RATIO 32

CCScene* HelloWorld::scene()
{
    CCScene *scene = CCScene::create();
    HelloWorld *layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
	setTouchEnabled(true);
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    _ball = CCSprite::create("ball2.png");
	this->addChild(_ball);
	
	b2Vec2 gravity = b2Vec2(0.0f, -8.0f);
	_world = new b2World(gravity);
	
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(winSize.width/2/PTM_RATIO, winSize.height/2/PTM_RATIO);
	ballBodyDef.userData = _ball;
	_body = _world->CreateBody(&ballBodyDef);

	b2CircleShape circle;
	circle.m_radius - 26.0/PTM_RATIO;

	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &circle;
	ballShapeDef.density = 1.0f;
	ballShapeDef.friction = 0.2f;
	ballShapeDef.restitution = 0.8f;
	_body->CreateFixture(&ballShapeDef);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);

	b2Body* groundBody = _world->CreateBody(&groundBodyDef);
	b2EdgeShape groundEdge;
	b2FixtureDef boxShapeDef;
	boxShapeDef.shape = &groundEdge;

	groundEdge.Set(b2Vec2(0,0), b2Vec2(0,winSize.height/PTM_RATIO));
    groundBody->CreateFixture(&boxShapeDef); 
    groundEdge.Set(b2Vec2(0, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO));
    groundBody->CreateFixture(&boxShapeDef);
	groundEdge.Set(b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, 0));
    groundBody->CreateFixture(&boxShapeDef);
	groundEdge.Set(b2Vec2(winSize.width/PTM_RATIO, 0), b2Vec2(0,0));
	groundBody->CreateFixture(&boxShapeDef);

	schedule(schedule_selector(HelloWorld::tick));
//	schedule(schedule_selector(HelloWorld::kick), 5.0f);

	return true;
}

void HelloWorld::tick(float dt) {
	_world->Step(dt, 10, 10);
	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != NULL) {
			CCSprite* ballData = (CCSprite*)b->GetUserData();
			ballData->setPosition(ccp(b->GetPosition().x*PTM_RATIO, b->GetPosition().y*PTM_RATIO));
			ballData->setRotation(-1*CC_RADIANS_TO_DEGREES(b->GetAngle()));
		}
	}
}

void HelloWorld::kick(float dt) {
	b2Vec2 force = b2Vec2(3, 3);
	_body->ApplyLinearImpulse(force, _body->GetPosition());
}

void HelloWorld::ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent) {
	CCTouch* touch = (CCTouch*)pTouches->anyObject();
	CCPoint tp = touch->getLocation();
	CCPoint _ballPoint = _ball->getPosition();
	if (_ballPoint.x > tp.x) {
		if (_ballPoint.y > tp.y) {
			b2Vec2 force = b2Vec2(3, 3);
			_body->ApplyLinearImpulse(force, _body->GetPosition());
		} else {
			b2Vec2 force = b2Vec2(3, -3);
			_body->ApplyLinearImpulse(force, _body->GetPosition());
		}
	} else {
		if (_ballPoint.y > tp.y) {
			b2Vec2 force = b2Vec2(-3, 3);
			_body->ApplyLinearImpulse(force, _body->GetPosition());
		} else {
			b2Vec2 force = b2Vec2(-3, -3);
			_body->ApplyLinearImpulse(force, _body->GetPosition());
		}
	}
	
}