#include "HelloWorldScene.h"
#include "UILayer.h"
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
    if ( !CCLayer::init() )
    {
        return false;
    }

	ownsize = CCSizeMake(40, 100);
	othersize = CCSizeMake(32, 32);

    CCParallaxNode *voidNode = CCParallaxNode::create();
	CCSprite *bg = CCSprite::create("background.png");
	bg->setScale(1.5f);
	addChild(bg, 0);
	CCTMXTiledMap *map = CCTMXTiledMap::create("ortho-objects.tmx");
	addChild(map, 0, 1);

	ui = new UILayer();
	addChild(ui, 0);

	CCSize s = map->getMapSize();
	int mapWidth = map->getMapSize().width * map->getTileSize().width;
	int mapHeight = map->getMapSize().height * map->getTileSize().height;
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	map->setPosition(ccp(0, 0));

	gameplayer = CCSprite::create("grossini.png");
	map->addChild(gameplayer, map->getChildren()->count());
	gameplayer->setPosition(ccp(winSize.width/2, 32));
	gameplayer->setAnchorPoint(ccp(0.5f, 0));
	enemy = CCSprite::create("SpinningPeas.png");
	map->addChild(enemy, map->getChildren()->count());
	enemy->setPosition(ccp(winSize.width/2 + 80, 32));
	enemy->setAnchorPoint(ccp(0.5f, 0));
	setTouchEnabled(true);
	scheduleUpdate();
	vmove = 0;
	hmove = 0;

    return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {
	CCSetIterator it = pTouches->begin();
	CCTouch *touch = (CCTouch*)(*it);

	CCPoint m_tBeginPos = touch->getLocationInView();
	m_tBeginPos = CCDirector::sharedDirector()->convertToGL(m_tBeginPos);
	CCPoint playerpoint = gameplayer->getPosition();
	CCPoint playersize = gameplayer->getContentSize();

	CCTMXTiledMap *map = (CCTMXTiledMap*) getChildByTag(1);
	CCPoint mappoint = map->getPosition();

	if (m_tBeginPos.x < playerpoint.x - playersize.x/2 + mappoint.x) {
		hmove = -1;
	} else if (m_tBeginPos.x > playerpoint.x + playersize.x/2 + mappoint.x) {
		hmove = 1;
	} else {
		hmove = 0;
	}
	if (m_tBeginPos.y > playersize.y + + playerpoint.y + mappoint.y)
		vmove = 6;
	else
		vmove = 0;
}

void HelloWorld::update(float dt) {
	enemytick();
	playermaphcollision();
	playermapvcollision();
	CCPoint playerpoint = gameplayer->getPosition();
	playerpoint.y += vmove;
	playerpoint.x += hmove;
	gameplayer->setPosition(playerpoint);

	if (! isreduce && iscollision(gameplayer, enemy)) {
		CCActionInterval *action = CCBlink::create(5, 10);
		gameplayer->runAction(action);
		schedule(schedule_selector(HelloWorld::resetreduce), 5.0f);
		isreduce = true;
		hmove = 0;
		ui->setlife(ui->getlife() - 5);
	}
	CCTMXTiledMap *map = (CCTMXTiledMap*) getChildByTag(1);
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int mapWidth = map->getMapSize().width * map->getTileSize().width;
	int mapHeight = map->getMapSize().height * map->getTileSize().height;
	float deltax = playerpoint.x - winSize.width/2;
	float deltay = playerpoint.y - 32;
	if (-deltax > 0) {
		deltax = 0;
	}
	if (-deltax < -mapWidth + winSize.width) {
		deltax = mapWidth - winSize.width;
	}
	if (-deltay > 0) {
		deltay = 0;
	}
	if (-deltay < -mapHeight + winSize.height) {
		deltay = mapHeight - winSize.height;
	}
	map->setPosition(ccp(-deltax, -deltay));	
}

void HelloWorld::playermaphcollision() {
	CCPoint playerpoint = gameplayer->getPosition();
	CCSize playersize = gameplayer->getContentSize();
	CCTMXTiledMap *map = (CCTMXTiledMap*)getChildByTag(1);
	int indexx, indexy;
	CCTMXLayer *layer = map->layerNamed("logic");
	for (int playery = playerpoint.y - playersize.height; 
		playery <= playerpoint.y; playery++) {
			indexx = (playerpoint.x - playersize.width / 2) / map->getTileSize().width;
			indexy = (playerpoint.y) / map->getTileSize().height;
			CCPoint playerindex = ccp(indexx, indexy);
			int tilegid = layer->tileGIDAt(playerindex);
			if (tilegid > 0) {
				CCDictionary *tiledic = map->propertiesForGID(tilegid);
				CCString *mvalue = (CCString*)tiledic->objectForKey("collion");
				int mv = mvalue->intValue();
				if (mv == 2) {
					hmove = 0;
					playerpoint.x = (indexx + 1) * map->getTileSize().width + playersize.width/2;
					gameplayer->setPosition(playerpoint);
				}
				return;
			}
			indexx = (playerpoint.x + playersize.width/2 - 1) / map->getTileSize().width;
			indexy = (playerpoint.y) / map->getTileSize().height;
			playerindex = ccp(indexx, indexy);
			tilegid = layer->tileGIDAt(playerindex);
			if (tilegid > 0) {
				CCDictionary *tiledic = map->propertiesForGID(tilegid);
				CCString *mvalue = (CCString*)tiledic->objectForKey("collion");
				int mv = mvalue->intValue();
				if (mv == 2) {
					hmove = 0;
					playerpoint.x = (indexx) * map->getTileSize().width - playersize.width/2;
					gameplayer->setPosition(playerpoint);
				}
				return;
			}
	}
}

void HelloWorld::playermapvcollision() {
	CCPoint playerpoint = gameplayer->getPosition();
	CCSize playersize = gameplayer->getContentSize();
	CCTMXTiledMap *map = (CCTMXTiledMap*) getChildByTag(1);
	int indexx, indexy;
	CCTMXLayer *layer = map->layerNamed("logic");
	indexx = (playerpoint.x) / map->getTileSize().width;
	indexy = map->getMapSize().height - (playerpoint.y) / map->getTileSize().height;
	CCPoint playerindex = ccp(indexx, indexy);
	int tilegid = layer->tileGIDAt(playerindex);
	if (tilegid > 0) {
		CCDictionary *tiledic = map->propertiesForGID(tilegid);
		CCString *mvalue = (CCString*)tiledic->objectForKey("collion");
		int mv = mvalue->intValue();
		if (mv == 1) {
			if (vmove < 0){
				vmove = 0;
				hmove = 0;
				playerpoint.y = (map->getMapSize().height - indexy) * map->getTileSize().height;
				gameplayer->setPosition(playerpoint);
			}
			return;
		}
	}
	vmove -= 0.2;
}

void HelloWorld::enemytick() {
	CCPoint enemypoint = enemy->getPosition();
	if (enemymovetick >= 0 && enemymovetick <= 20) {
		enemymovetick++;
		enemypoint.x +=2;
	} else if (enemymovetick > 20 && enemymovetick <= 60) {
		enemymovetick++;
		enemypoint.x -= 2;
	} else if (enemymovetick > 60 && enemymovetick <= 80) {
		enemymovetick++;
		enemypoint.x += 2;
	} else if (enemymovetick > 80) {
		enemymovetick = 0;
	}
	enemy->setPosition(enemypoint);
}

bool HelloWorld::iscollision(CCSprite *mysprite, CCSprite *testsprite) {
	CCPoint ownp = mysprite->getPosition();
	CCPoint otherp = testsprite->getPosition();
	if (abs(ownp.x - otherp.x) <= ownsize.width/2 + othersize.width/2
		&& ownp.y - otherp.y <= othersize.height/2
		&& ownp.y - otherp.y >= -(othersize.height/2)) {
			return true;
	}
	return false;
}

void HelloWorld::resetreduce(float dt) {
	isreduce = false;
}
