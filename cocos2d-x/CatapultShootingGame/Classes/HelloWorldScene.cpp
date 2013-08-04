#include "HelloWorldScene.h"
#define PTM_RATIO 32
#define FLOOR_HEIGHT 62.0f

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
    //////////////////////////////
    // 1. super init first
	targets = new vector<b2Body*>;
	enemies = new vector<b2Body*>;
    CCSprite* sprite1 = CCSprite::create("bg.png");
	sprite1->setAnchorPoint(CCPointZero);
	addChild(sprite1, -1);

	CCSprite* sprite2 = CCSprite::create("catapult_base_2.png");
	sprite2->setAnchorPoint(CCPointZero);
	sprite2->setPosition(CCPointMake(181.0, FLOOR_HEIGHT));
	addChild(sprite2, 0);

	CCSprite* sprite3 = CCSprite::create("squirrel_1.png");
	sprite3->setAnchorPoint(CCPointZero);
	sprite3->setPosition(CCPointMake(11.0, FLOOR_HEIGHT));
	addChild(sprite3, 0);

	CCSprite* sprite4 = CCSprite::create("catapult_base_1.png");
	sprite4->setAnchorPoint(CCPointZero);
	sprite4->setPosition(CCPointMake(181.0, FLOOR_HEIGHT));
	addChild(sprite4, 9);

	CCSprite* sprite5 = CCSprite::create("squirrel_2.png");
	sprite5->setAnchorPoint(CCPointZero);
	sprite5->setPosition(CCPointMake(240.0, FLOOR_HEIGHT));
	addChild(sprite5, 9);

	CCSprite* sprite6 = CCSprite::create("fg.png");
	sprite6->setAnchorPoint(CCPointZero);
	addChild(sprite6, 10);

	b2Vec2 gravity;
	gravity.Set(0.0f, -10.0f);
	m_world = new b2World(gravity);
	m_world->SetAllowSleeping(true);
	m_world->SetContinuousPhysics(true);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);
	m_groundBody = m_world->CreateBody(&groundBodyDef);

	b2EdgeShape groundBox;
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

	 groundBox.Set(b2Vec2(0,FLOOR_HEIGHT/PTM_RATIO), b2Vec2(screenSize.width*2.0f/PTM_RATIO,FLOOR_HEIGHT/PTM_RATIO)); 
    m_groundBody->CreateFixture(&groundBox, 0); 
     
    // top 
    groundBox.Set(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width*2.0f/PTM_RATIO,screenSize.height/PTM_RATIO)); 
    m_groundBody->CreateFixture(&groundBox, 0); 
     
    // left 
    groundBox.Set(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(0,0)); 
    m_groundBody->CreateFixture(&groundBox, 0); 
     
    // right 
//    groundBox.Set(b2Vec2(screenSize.width*1.5f/PTM_RATIO,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width*1.5f/PTM_RATIO,0)); 
//    m_groundBody->CreateFixture(&groundBox, 0); 

    
	CCSprite *arm = CCSprite::create("catapult_arm.png");
	addChild(arm, 1);
	b2BodyDef armBodyDef;
	armBodyDef.type = b2_dynamicBody;
	armBodyDef.linearDamping = 1;
	armBodyDef.angularDamping = 1;
	armBodyDef.position.Set(230.0f/PTM_RATIO, (FLOOR_HEIGHT+91.0f)/PTM_RATIO);
	armBodyDef.userData = arm;
	m_armBody = m_world->CreateBody(&armBodyDef);

	b2PolygonShape armBox;
	b2FixtureDef armBoxDef;
	armBoxDef.shape = &armBox;
	armBoxDef.density = 0.3f;
	armBox.SetAsBox(11.0f/PTM_RATIO, 91.0f/PTM_RATIO);
	m_armFixture = m_armBody->CreateFixture(&armBoxDef);


	b2RevoluteJointDef armJointDef;
	armJointDef.Initialize(m_groundBody, m_armBody,
		b2Vec2(233.0f / PTM_RATIO, 50/PTM_RATIO));
	armJointDef.enableMotor = true;
	armJointDef.enableLimit = true;
	armJointDef.motorSpeed = -10;
	armJointDef.lowerAngle = CC_DEGREES_TO_RADIANS(9);
	armJointDef.upperAngle = CC_DEGREES_TO_RADIANS(75);
	armJointDef.maxMotorTorque = 700;
	m_armJoint = (b2RevoluteJoint*) m_world->CreateJoint(&armJointDef);
	
	m_mouseJoint = NULL;

	resetGame();
	m_releasingArm = false;
	createTargets();

	contactListener = new MyContactListener();
	m_world->SetContactListener(contactListener);

	setTouchEnabled(true);
	schedule(schedule_selector(HelloWorld::tick));
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
	int velocityIterations = 8;
	int positionIterations = 1;
	
	m_world->Step(dt, velocityIterations, positionIterations);
	for (b2Body* b = m_world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != NULL) {
			CCSprite* sprite = (CCSprite*)(b->GetUserData());
			sprite->setPosition(ccp(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
			sprite->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
			
		}
	}
	if (m_releasingArm && m_bulletJoint != NULL) {
		if (m_armJoint->GetJointAngle() <= CC_DEGREES_TO_RADIANS(10)) {
			m_releasingArm = false;
			m_world->DestroyJoint(m_bulletJoint);
			m_bulletJoint = NULL;

			CCDelayTime* delayAction = CCDelayTime::create(5.0f);
			CCCallFunc* callSelectorAction = CCCallFunc::create(this, callfunc_selector(HelloWorld::resetBullet));
			this->runAction(CCSequence::create(delayAction, callSelectorAction, NULL));
		}
	}
	if (m_bulletBody && m_bulletJoint == NULL) {
		b2Vec2 position = m_bulletBody->GetPosition();
		CCPoint myPosition = this->getPosition();
		CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
		if (position.x > screenSize.width/2.0f/PTM_RATIO) {
			myPosition.x = -MIN(screenSize.width*2.0f - screenSize.width,
				position.x*PTM_RATIO - screenSize.width/2.0f);
			this->setPosition(myPosition);
		}
	}
    std::set<b2Body*>::iterator pos;
	for (pos = contactListener->contacts.begin(); pos != contactListener->contacts.end(); ++pos) {
		b2Body* body = *pos;
		for (vector<b2Body*>::iterator iter = targets->begin(); iter != targets->end(); ++iter) {
			if (body == *iter) {
				iter = targets->erase(iter);
				break;
			}
		}
		for (vector<b2Body*>::iterator iter = enemies->begin(); iter != enemies->end(); ++iter) {
			if (body == *iter) {
				iter = enemies->erase(iter);
				break;
			}
		}
		CCNode* contactNode = (CCNode*)body->GetUserData();
		CCPoint position = contactNode->getPosition();
		removeChild(contactNode, true);
		m_world->DestroyBody(body);

		CCParticleSun *explosion = CCParticleSun::create();
		explosion->retain();
		explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
//		explosion->initWithTotalParticles(200);
		explosion->setAutoRemoveOnFinish(true);
		explosion->setStartSizeVar(10.0f);
		explosion->setSpeed(70.0f);
		explosion->setAnchorPoint(ccp(0.5f, 0.5f));
		explosion->setPosition(position);
		explosion->setDuration(1.0f);
		addChild(explosion, 11);
		explosion->release();
	}
	contactListener->contacts.clear();
}

void HelloWorld::ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent) {
	if (m_mouseJoint != NULL) { return; }
	CCTouch* touch = (CCTouch*)pTouches->anyObject();
	CCPoint location = touch->getLocation();
	location = CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

	if (locationWorld.x < m_armBody->GetWorldCenter().x + 150.0/PTM_RATIO) {
		b2MouseJointDef md;
		md.bodyA = m_groundBody;
		md.bodyB = m_armBody;
		md.target = locationWorld;
		md.maxForce = 2000;

		m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
	}
}

void HelloWorld::ccTouchesMoved(CCSet* pTouches, CCEvent* pEvent) {
	if (m_mouseJoint == NULL) { return; }

	CCTouch* touch = (CCTouch*)pTouches->anyObject();
	CCPoint location = touch->getLocation();
	location = CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

	m_mouseJoint->SetTarget(locationWorld);
}

void HelloWorld::ccTouchesEnded(CCSet* pTouches, CCEvent* pEvent) {
	if (m_mouseJoint != NULL) {
		if (m_armJoint->GetJointAngle() >= CC_DEGREES_TO_RADIANS(5)) {
			m_releasingArm = true;
		}
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
		return;
	}
}

void HelloWorld::createBullets(int count) {
	m_currentBullet = 0;
	float pos = 62.0f;

	if (count > 0) {
		float delta = (count > 1) ? ((165.0f - 62.0f - 30.0f) / (count - 1)):0.0f;
		for (int i = 0; i < count; i++, pos += delta) {
			CCSprite* sprite = CCSprite::create("acorn.png");
			addChild(sprite, 1);

			b2BodyDef bulletBodyDef;
			bulletBodyDef.type = b2_dynamicBody;
			bulletBodyDef.bullet = true;
			bulletBodyDef.position.Set(pos/PTM_RATIO, (FLOOR_HEIGHT+15.0f)/PTM_RATIO);
			bulletBodyDef.userData = sprite;
			b2Body* bullet = m_world->CreateBody(&bulletBodyDef);
			bullet->SetActive(false);

			b2CircleShape circle;
			circle.m_radius = 15.0/PTM_RATIO;

			b2FixtureDef ballShapeDef;
			ballShapeDef.shape = &circle;
			ballShapeDef.density = 0.8f;
			ballShapeDef.restitution = 0.2f;
			ballShapeDef.friction = 0.99f;
			bullet->CreateFixture(&ballShapeDef);
			m_bullets.push_back(bullet);
		}
	}
}

bool HelloWorld::attachBullet() {
	if (m_currentBullet < m_bullets.size()) {
		m_bulletBody = (b2Body*)m_bullets.at(m_currentBullet++);
		m_bulletBody->SetTransform(b2Vec2(230.0f/PTM_RATIO, (156.0f+FLOOR_HEIGHT)/PTM_RATIO), 0.0f);
		m_bulletBody->SetActive(true);
		b2WeldJointDef weldJointDef;
		weldJointDef.Initialize(m_bulletBody, m_armBody, b2Vec2(230.0f/PTM_RATIO, (150.0f+FLOOR_HEIGHT)/PTM_RATIO));
		weldJointDef.collideConnected = false;
		m_bulletJoint = (b2WeldJoint*)m_world->CreateJoint(&weldJointDef);
		return true;
	}
	return false;
}

void HelloWorld::resetGame() {
	if (m_bullets.size() != 0) {
		for (vector<b2Body*>::iterator bulletPointer = m_bullets.begin(); bulletPointer != m_bullets.end(); ++bulletPointer) {
			b2Body* bullet = (b2Body*)*bulletPointer;
			CCNode* node = (CCNode*)bullet->GetUserData();
			m_world->DestroyBody(bullet);
		}
		m_bullets.clear();
	}
	if (targets->end() - targets->begin() != 0) {
		for (vector<b2Body*>::iterator targetPointer = (*targets).begin(); targetPointer != (*targets).end(); targetPointer++) {
			b2Body* target = (b2Body*)*targetPointer;
			CCNode* node = (CCNode*)target->GetUserData();
			removeChild(node, true);
			m_world->DestroyBody(target);
		}
		(*targets).clear();
		(*enemies).clear();
	}

	this->createBullets(3);
	this->createTargets();
	this->attachBullet();

	CCFiniteTimeAction *action1 = CCMoveTo::create(1.5f, ccp(-480.0f, 0.0f));
//	CCCallFuncN *action2 = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::attachBullet));
	CCDelayTime *action3 = CCDelayTime::create(1.0f);
	CCFiniteTimeAction* action4 = CCMoveTo::create(1.5f, CCPointZero);
	runAction(CCSequence::create(action1,action3, action4, NULL));

	
}

HelloWorld::~HelloWorld()
{
	delete m_world;
	m_world = NULL;
}

void HelloWorld::resetBullet() {
	if (enemies->size() == 0) {
		CCDelayTime* delayAction = CCDelayTime::create(2.0f);
		CCCallFunc* callSelectorAction = CCCallFunc::create(this, callfunc_selector(HelloWorld::resetGame));
		this->runAction(CCSequence::create(delayAction, callSelectorAction, NULL));
	} else if (attachBullet()) {
		CCAction *action = CCMoveTo::create(0.2f, CCPointZero);
		runAction(action);
	} else {
		CCDelayTime* delayAction = CCDelayTime::create(2.0f);
		CCCallFunc* callSelectorAction = CCCallFunc::create(this, callfunc_selector(HelloWorld::resetGame));
		this->runAction(CCSequence::create(delayAction, callSelectorAction, NULL));
	}
}

void HelloWorld::createTarget(char   *imageName,
					  CCPoint position,
					  float   rotation,
					  bool	  isCircle,
					  bool    isStatic,
					  bool    isEnemy) {

						  CCSprite *sprite = CCSprite::create(imageName); 
    this->addChild(sprite, 1); 
 
    b2BodyDef bodyDef; 
    bodyDef.type = isStatic ? b2_staticBody : b2_dynamicBody; 
    bodyDef.position.Set((position.x+sprite->getContentSize().width/2.0f)/PTM_RATIO, 
                                     (position.y+sprite->getContentSize().height/2.0f)/PTM_RATIO); 
    bodyDef.angle = CC_DEGREES_TO_RADIANS(rotation); 
    bodyDef.userData = sprite; 
 
    b2Body *body = m_world->CreateBody(&bodyDef); 
 
    b2FixtureDef boxDef; 
    b2Fixture *fixtureTemp; 
     
    if (isCircle){ 
        b2CircleShape circle; 
        boxDef.shape = &circle; 
        circle.m_radius = sprite->getContentSize().width/2.0f/PTM_RATIO; 
         
        fixtureTemp = body->CreateFixture(&circle, 0.5f); 
        targets->push_back(body); 
    } 
    else{ 
        b2PolygonShape box; 
        boxDef.shape = &box; 
        box.SetAsBox(sprite->getContentSize().width/2.0f/PTM_RATIO, sprite->getContentSize().height/2.0f/PTM_RATIO); 
        body->CreateFixture(&box, 0.5f); 
        targets->push_back(body); 
    } 
 
    if (isEnemy){ 
        fixtureTemp->SetUserData((void*)1);     //  boxDef.userData = (void*)1; 
        enemies->push_back(body); 
    } 
}

void HelloWorld::createTargets() {
	createTarget("brick_2.png", CCPointMake(675.0, FLOOR_HEIGHT), 0.0f, false, false, false); 
    createTarget("brick_1.png", CCPointMake(741.0, FLOOR_HEIGHT), 0.0f, false, false, false); 
    createTarget("brick_1.png", CCPointMake(741.0, FLOOR_HEIGHT+23.0), 0.0f, false, false, false); 
    createTarget("brick_3.png", CCPointMake(673.0, FLOOR_HEIGHT+46.0), 0.0f, false, false, false); 
    createTarget("brick_1.png", CCPointMake(707.0, FLOOR_HEIGHT+58.0), 0.0f, false, false, false); 
    createTarget("brick_1.png", CCPointMake(707.0, FLOOR_HEIGHT+81.0), 0.0f, false, false, false); 
 
    createTarget("head_dog.png", CCPointMake(702.0, FLOOR_HEIGHT), 0.0f, true, false, true); 
    createTarget("head_cat.png", CCPointMake(680.0, FLOOR_HEIGHT+58.0), 0.0f, true, false, true); 
    createTarget("head_dog.png", CCPointMake(740.0, FLOOR_HEIGHT+58.0), 0.0f, true, false, true); 
 
    // 2 bricks at the right of the first block 
    createTarget("brick_2.png", CCPointMake(770.0, FLOOR_HEIGHT), 0.0f, false, false, false); 
    createTarget("brick_2.png", CCPointMake(770.0, FLOOR_HEIGHT+46.0), 0.0f, false, false, false); 
 
    // The dog between the blocks 
    createTarget("head_dog.png", CCPointMake(830.0, FLOOR_HEIGHT), 0.0f, true, false, true); 
 
    // Second block 
    createTarget("brick_platform.png", CCPointMake(839.0, FLOOR_HEIGHT), 0.0f, false, true, false); 
    createTarget("brick_2.png", CCPointMake(854.0, FLOOR_HEIGHT+28.0), 0.0f, false, false, false); 
    createTarget("brick_2.png", CCPointMake(854.0, FLOOR_HEIGHT+28.0+46.0), 0.0f, false, false, false); 
    createTarget("head_cat.png", CCPointMake(881.0, FLOOR_HEIGHT+28.0), 0.0f, true, false, true); 
    createTarget("brick_2.png", CCPointMake(909.0, FLOOR_HEIGHT+28.0), 0.0f, false, false, false); 
    createTarget("brick_1.png", CCPointMake(909.0, FLOOR_HEIGHT+28.0+46.0), 0.0f, false, false, false); 
    createTarget("brick_1.png", CCPointMake(909.0, FLOOR_HEIGHT+28.0+46.0+23.0), 0.0f, false, false, false); 
    createTarget("brick_2.png", CCPointMake(882.0, FLOOR_HEIGHT+108.0), 90.0f, false, false, false); 
	
}
