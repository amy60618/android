#include "HelloWorldScene.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
	_world = new b2World(gravity);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);
	_groundBody = _world->CreateBody(&groundBodyDef);

	b2EdgeShape groundBox;
	b2FixtureDef groundBoxDef;
	groundBoxDef.shape = &groundBox;

	groundBox.Set(b2Vec2(0, 0), b2Vec2(winSize.width/PTM_RATIO, 0));
	_bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(0, 0), b2Vec2(0, winSize.height/PTM_RATIO));
	_groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(0, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, 
		winSize.height/PTM_RATIO));
	_groundBody->CreateFixture(&groundBoxDef);

	groundBox.Set(b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO),
		b2Vec2(winSize.width/PTM_RATIO, 0));
	_groundBody->CreateFixture(&groundBoxDef);

	CCSprite *ball = CCSprite::create("ball.png");
	ball->setTag(1);
	ball->setPosition(ccp(100, 100));
	addChild(ball);

	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(100/PTM_RATIO, 100/PTM_RATIO);
	ballBodyDef.userData = ball;
	b2Body* ballBody = _world->CreateBody(&ballBodyDef);

	b2CircleShape circle;
	circle.m_radius = 26.0/PTM_RATIO;

	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &circle;
	ballShapeDef.density = 1.0f;
	ballShapeDef.friction = 0.0f;
	ballShapeDef.restitution = 1.0f;
	_ballFixture = ballBody->CreateFixture(&ballShapeDef);

	b2Vec2 force = b2Vec2(10, 10);
	ballBody->ApplyLinearImpulse(force, ballBodyDef.position);

	CCSprite* paddle = CCSprite::create("paddle.png");
	paddle->setPosition(ccp(winSize.width/2, 50));
	addChild(paddle);

	b2BodyDef paddleBodyDef;
	paddleBodyDef.type = b2_dynamicBody;
	paddleBodyDef.position.Set(winSize.width/2/PTM_RATIO, 50/PTM_RATIO);
	paddleBodyDef.userData = paddle;
	_paddleBody = _world->CreateBody(&paddleBodyDef);

	b2PolygonShape paddleShape;
	paddleShape.SetAsBox(paddle->getContentSize().width/PTM_RATIO/2, paddle->getContentSize().height/PTM_RATIO/2);

	b2FixtureDef paddleShapeDef;
	paddleShapeDef.shape = &paddleShape;
	paddleShapeDef.density = 10.0f;
	paddleShapeDef.friction = 0.4f;
	paddleShapeDef.restitution = 0.1f;
	_paddleFixture = _paddleBody->CreateFixture(&paddleShapeDef);

	setTouchEnabled(true);
	_mouseJoint = NULL;
	
	b2PrismaticJointDef jointDef;
	b2Vec2 worldAxis(1.0f, 0.0f);
	jointDef.collideConnected = true;
	jointDef.Initialize(_paddleBody, _groundBody,
		_paddleBody->GetWorldCenter(), worldAxis);
	_world->CreateJoint(&jointDef);

	_contactListener = new MyContactListener();
	_world->SetContactListener(_contactListener);

	for (int i = 0; i < 4; i++) {
		static int padding=20;
		CCSprite* block = CCSprite::create("block.png");
		int xOffset = padding + block->getContentSize().width/2 +
			((block->getContentSize().width + padding)*i);
		block->setPosition(ccp(xOffset, 250));
		block->setTag(2);
		addChild(block);

		b2BodyDef blockBodyDef;
		blockBodyDef.type = b2_dynamicBody;
		blockBodyDef.position.Set(xOffset/PTM_RATIO, 250/PTM_RATIO);
		blockBodyDef.userData = block;
		b2Body* blockBody = _world->CreateBody(&blockBodyDef);

		b2PolygonShape blockShape;
		blockShape.SetAsBox(block->getContentSize().width/PTM_RATIO/2,
			block->getContentSize().height/PTM_RATIO/2);

		b2FixtureDef blockShapeDef;
		blockShapeDef.shape = &blockShape;
		blockShapeDef.density = 10.0;
		blockShapeDef.friction = 0.0;
		blockShapeDef.restitution = 0.1f;
		blockBody->CreateFixture(&blockShapeDef);
	}

	schedule(schedule_selector(HelloWorld::tick));

//	dealloc();
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::tick(float dt) {
	bool blockFound = false;
	_world->Step(dt, 10, 10);
	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != NULL) {
			CCSprite* sprite = (CCSprite*)b->GetUserData();
			if (sprite->getTag() == 2) {
				blockFound = true;
			}
			sprite->setPosition(ccp(b->GetPosition().x*PTM_RATIO, b->GetPosition().y*PTM_RATIO));
			sprite->setRotation(-1*CC_RADIANS_TO_DEGREES(b->GetAngle()));
			if (sprite->getTag() == 1) {
				static int maxSpeed = 10;
				b2Vec2 velocity = b->GetLinearVelocity();
				float32 speed = velocity.Length();
				if (speed > maxSpeed) {
					b->SetLinearDamping(0.5);
				} else if (speed < maxSpeed) {
					b->SetLinearDamping(0.0);
				}
			}
		
		}
	}
	std::vector<b2Body*> toDestroy;
	std::vector<MyContact>::iterator pos;
	for (pos = _contactListener->_contacts.begin();
		 pos != _contactListener->_contacts.end(); ++pos) {
			MyContact contact = *pos;
			if ((contact.fixtureA == _bottomFixture && contact.fixtureB == _ballFixture)
				|| (contact.fixtureA == _ballFixture && contact.fixtureB == _bottomFixture)) {
					CCLog("You Lose :[");
			}

			b2Body* bodyA = contact.fixtureA->GetBody();
			b2Body* bodyB = contact.fixtureB->GetBody();
			if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL) {
				CCSprite* spriteA = (CCSprite*)bodyA->GetUserData();
				CCSprite* spriteB = (CCSprite*)bodyB->GetUserData();

				if (spriteA->getTag() == 1 && spriteB->getTag() == 2) {
					if (std::find(toDestroy.begin(),  toDestroy.end(), bodyB) == toDestroy.end()) {
						toDestroy.push_back(bodyB);
					}
				} else if (spriteA->getTag() == 2 && spriteB->getTag() == 1) {
					if (std::find(toDestroy.begin(), toDestroy.end(), bodyA) == toDestroy.end()) {
						toDestroy.push_back(bodyA);
					}
				}
			}
	}
	std::vector<b2Body*>::iterator pos2;
	for (pos2 = toDestroy.begin(); pos2 != toDestroy.end(); ++pos2) {
		b2Body* body = *pos2;
		if (body->GetUserData() != NULL) {
			CCSprite* sprite = (CCSprite*)body->GetUserData();
			this->removeChild(sprite);
		}
		_world->DestroyBody(body);
	}
	
	if (!blockFound) {
		CCLog("You Won!");
	}
	
}

void HelloWorld::dealloc() {
	delete _world;
	_groundBody = NULL;
	delete _contactListener;
}

void HelloWorld::ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent) {
	if (_mouseJoint != NULL) return;

	CCTouch* myTouch = (CCTouch*)pTouches->anyObject();
	CCPoint location = myTouch->getLocation();
	location = CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

//	if (_paddleFixture->TestPoint(locationWorld)) {
	if (1) {
		b2MouseJointDef md;
		md.bodyA = _groundBody;
		md.bodyB = _paddleBody;
		md.target = locationWorld;
		md.collideConnected = true;
		md.maxForce = 1000.0f*_paddleBody->GetMass();

		_mouseJoint = (b2MouseJoint*)_world->CreateJoint(&md);
		_paddleBody->SetAwake(true);
	}
}

void HelloWorld::ccTouchesMoved(CCSet* pTouches, CCEvent* pEvent) {
	if (_mouseJoint == NULL) return;

	CCTouch* myTouch = (CCTouch*)pTouches->anyObject();
	CCPoint location = myTouch->getLocation();
	location = CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

	_mouseJoint->SetTarget(locationWorld);
}
/*
void HelloWorld::ccTouchesCancelled(CCSet* pTouches, CCEvent* pEvent) {
	if (_mouseJoint) {
		_world->DestroyJoint(_mouseJoint);
		_mouseJoint = NULL;
	}
}
*/
void HelloWorld::ccTouchesEnded(CCSet* pTouches, CCEvent* pEvent) {
	if (_mouseJoint) {
		_world->DestroyJoint(_mouseJoint);
		_mouseJoint = NULL;
	}
}