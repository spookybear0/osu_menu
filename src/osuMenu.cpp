#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <thread>

#include <cocos2d.h>
#include "MinHook.h"
#include "gd.h"
#include <CCEGLView.h>

#include "OsuMenu.h"

USING_NS_CC;

CCScene* pushLayer(CCLayer* layer) {
    CCScene* scene = CCScene::create();
    scene->addChild(layer);
    CCTransitionFade* transition = CCTransitionFade::create(0.5f, scene);
    CCDirector::sharedDirector()->pushScene(transition);
    return scene;
}

bool OsuMenu::init() {
    if (!CCLayer::init()) {
        return false;
    }

    scheduleUpdate();

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    gd::GameManager* gm = gd::GameManager::sharedState();
    gd::GJAccountManager* am = gd::GJAccountManager::sharedState();
    gd::GameStatsManager* gsm = gd::GameStatsManager::sharedState();

    // setup background
    CCSprite* bg_sprite = CCSprite::create("menu-bg.jpg");

    CCSize size = bg_sprite->getContentSize();
    
    bg_sprite->setScaleX(winSize.width / size.width);
    bg_sprite->setScaleY(winSize.height / size.height);
    
    bg_sprite->setAnchorPoint({0, 0});
    addChild(bg_sprite, -20);

    // sprite
    CCSprite* cookie_sprite = CCSprite::create("menu-osu.png");
    cookie_sprite->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    cookie_sprite->setScale(0.85f);

    // button for cookie
    m_cookieButton = gd::CCMenuItemSpriteExtra::create(
        // directly create the sprite here, as its our only use for it
        cookie_sprite,
        this,
        menu_selector(OsuMenu::cookieCallback)
    );

    // click menu
    m_cookieMenu = CCMenu::create();
    m_cookieMenu->addChild(m_cookieButton);
    addChild(m_cookieMenu, 3);

    // setup buttonsprites
    CCSprite* play_sprite = CCSprite::create("menu-button-play.png");
    CCSprite* edit_sprite = CCSprite::create("menu-button-edit.png");
    CCSprite* options_sprite = CCSprite::create("menu-button-options.png");
    CCSprite* exit_sprite = CCSprite::create("menu-button-exit.png");

    // sprite scale
    play_sprite->setScale(0.85f);
    edit_sprite->setScale(0.85f);
    options_sprite->setScale(0.85f);
    exit_sprite->setScale(0.85f);

    // setup buttons
    m_playButton = gd::CCMenuItemSpriteExtra::create(play_sprite, this, menu_selector(OsuMenu::playCallback));
    m_editButton = gd::CCMenuItemSpriteExtra::create(edit_sprite, this, menu_selector(OsuMenu::editCallback));
    m_optionsButton = gd::CCMenuItemSpriteExtra::create(options_sprite, this, menu_selector(OsuMenu::optionsCallback));
    m_exitButton = gd::CCMenuItemSpriteExtra::create(exit_sprite, this, menu_selector(OsuMenu::exitCallback));

    // add buttons to menu
    m_buttonMenu = CCMenu::create();
    m_buttonMenu->addChild(m_playButton);
    m_buttonMenu->addChild(m_editButton);
    m_buttonMenu->addChild(m_optionsButton);
    m_buttonMenu->addChild(m_exitButton);

    // button alignment
    m_buttonMenu->alignItemsVerticallyWithPadding(10);
    m_buttonMenu->setPositionX(winSize.width / 2 + 50);
    m_buttonMenu->setZOrder(2);
    m_buttonMenu->setOpacity(0);
    m_buttonMenu->setVisible(false);

    addChild(m_buttonMenu);

    // setup tip text

    CCLabelTTF* tip = CCLabelTTF::create(gd::LoadingLayer::getLoadingString(), "Aller.ttf", 10);
    tip->setPosition(ccp(winSize.width / 2, 30));
    addChild(tip);

    // setup user card

    // user card top background
    CCSprite* rec = CCSprite::create();
    rec->setColor(ccColor3B{0, 0, 0});
    rec->setPosition(ccp(winSize.width / 2, winSize.height - 5));
    rec->setTextureRect(CCRect(0, 0, winSize.width, 70));
    rec->setOpacity(120);
    addChild(rec, -10);

    // user card bottom background
    CCSprite* rec2 = CCSprite::create();
    rec2->setColor(ccColor3B{0, 0, 0});
    rec2->setPosition(ccp(winSize.width / 2, 5));
    rec2->setTextureRect(CCRect(0, 0, winSize.width, 70));
    rec2->setOpacity(120);
    addChild(rec2, -10);

    // player icon
    gd::SimplePlayer* player = gd::SimplePlayer::create(gm->getPlayerFrame());

    player->setColor(gm->colorForIdx(gm->getPlayerColor()));
    player->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
    player->setGlowOutline(gm->getPlayerGlow());

    player->setPosition(ccp(20, winSize.height - 20));
    player->setScale(1.05f);

    addChild(player);

    // player name
    CCLabelTTF* name = CCLabelTTF::create("spookybear0", "Aller.ttf", 10);//am->getUsername()
    name->setAnchorPoint({0, 0});
    name->setPosition(ccp(40, winSize.height - 15));
    addChild(name);

    // player stats
    int stars = gsm->m_pPlayerStats->valueForKey("6")->intValue();
    CCLabelTTF* stars_label = CCLabelTTF::create((std::string("Total Stars: ") + std::to_string(stars)).c_str(), "Aller.ttf", 6);
    stars_label->setAnchorPoint({0, 0});
    stars_label->setPosition(ccp(40, winSize.height - 22));
    addChild(stars_label);

    int attempts = gsm->m_pPlayerStats->valueForKey("2")->intValue();
    CCLabelTTF* attempts_label = CCLabelTTF::create((std::string("Attempts: ") + std::to_string(attempts)).c_str(), "Aller.ttf", 6);
    attempts_label->setAnchorPoint({0, 0});
    attempts_label->setPosition(ccp(40, winSize.height - 29));
    addChild(attempts_label);
    
    int jumps = gsm->m_pPlayerStats->valueForKey("1")->intValue();
    CCLabelTTF* jumps_label = CCLabelTTF::create((std::string("Jumps: ") + std::to_string(jumps)).c_str(), "Aller.ttf", 6);
    jumps_label->setAnchorPoint({0, 0});
    jumps_label->setPosition(ccp(40, winSize.height - 36));
    addChild(jumps_label);

    // rank
    m_rank = CCLabelTTF::create("", "exo.ttf", 20);
    m_rank->setPosition(ccp(80, winSize.height - 36));
    m_rank->setAnchorPoint({0, 0});
    m_rank->setHorizontalAlignment(kCCTextAlignmentRight);
    m_rank->setOpacity(86);
    addChild(m_rank, -1);

    // profile button
    CCMenu* profileButtonMenu = CCMenu::create();
    profileButtonMenu->setPosition(ccp(0, winSize.height - 5));

    CCSprite* profileButton = CCSprite::create();
    profileButton->setTextureRect(CCRect(0, 0, 320, 70));
    profileButton->setOpacity(0);
    
    profileButtonMenu->addChild(gd::CCMenuItemSpriteExtra::create(profileButton, this, menu_selector(OsuMenu::profileCallback)));
    addChild(profileButtonMenu, 5);

    /*CCArray* allkeys = gd::GameLevelManager::sharedState()->m_storedUserInfo->allKeys();
    for(int i = 0; i < allkeys->count(); i++)
    {
        CCString* child = reinterpret_cast<CCString*>(allkeys->objectAtIndex(i));
        std::cout << child->getCString() << " " << gd::GameLevelManager::sharedState()->m_storedUserInfo->valueForKey(child->m_sString)->intValue() << std::endl;
    }*/

    return true;
}

OsuMenu* OsuMenu::create() {
    auto ret = new OsuMenu();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void OsuMenu::update(float dt) {
    CCEGLView* view = CCDirector::sharedDirector()->getOpenGLView();
    CCPoint mousepos = view->getMousePosition();

    CCSize framesize = view->getFrameSize();
    CCSize winsize = CCDirector::sharedDirector()->getWinSize();

    if (m_cookieButton->boundingBox().containsPoint(
        m_cookieMenu->convertToNodeSpace(
            ccp(mousepos.x / framesize.width * winsize.width, (1 - mousepos.y / framesize.height) * winsize.height)
        )
    )) {
        // reset all buttons
        if (m_playHovered) {
            changeButtonSprite(m_playButton, "menu-button-play.png");
            m_playHovered = false;
            CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
            m_playButton->runAction(move_btn);
        }
        if (m_editHovered) {
            changeButtonSprite(m_editButton, "menu-button-edit.png");
            m_editHovered = false;
            CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
            m_editButton->runAction(move_btn);
        }
        if (m_optionsHovered) {
            changeButtonSprite(m_optionsButton, "menu-button-options.png");
            m_optionsHovered = false;
            CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
            m_optionsButton->runAction(move_btn);
        }
        if (m_exitHovered) {
            changeButtonSprite(m_exitButton, "menu-button-exit.png");
            m_exitHovered = false;
            CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
            m_exitButton->runAction(move_btn);
        }
        return;
    }

    // check if hovering, set sprite, if not, reset sprite to normal
    // god forgive me
    if (m_playButton->boundingBox().containsPoint(m_buttonMenu->convertToNodeSpace(ccp(mousepos.x / framesize.width * winsize.width, (1 - mousepos.y / framesize.height) * winsize.height)))) {
        if (m_playHovered) return;
        changeButtonSprite(m_playButton, "menu-button-play-over.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(25, 0));
        m_playButton->runAction(move_btn);
        m_playHovered = true;
    }
    else if (m_playHovered) {
        changeButtonSprite(m_playButton, "menu-button-play.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
        m_playButton->runAction(move_btn);
        m_playHovered = false;
    }
    if (m_editButton->boundingBox().containsPoint(m_buttonMenu->convertToNodeSpace(ccp(mousepos.x / framesize.width * winsize.width, (1 - mousepos.y / framesize.height) * winsize.height)))) {
        if (m_editHovered) return;
        changeButtonSprite(m_editButton, "menu-button-edit-over.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(25, 0));
        m_editButton->runAction(move_btn);
        m_editHovered = true;
    }
    else if (m_editHovered) {
        changeButtonSprite(m_editButton, "menu-button-edit.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
        m_editButton->runAction(move_btn);
        m_editHovered = false;
    }
    if (m_optionsButton->boundingBox().containsPoint(m_buttonMenu->convertToNodeSpace(ccp(mousepos.x / framesize.width * winsize.width, (1 - mousepos.y / framesize.height) * winsize.height)))) {
        if (m_optionsHovered) return;
        changeButtonSprite(m_optionsButton, "menu-button-options-over.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(25, 0));
        m_optionsButton->runAction(move_btn);
        m_optionsHovered = true;
    }
    else if (m_optionsHovered) {
        changeButtonSprite(m_optionsButton, "menu-button-options.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
        m_optionsButton->runAction(move_btn);
        m_optionsHovered = false;
    }
    if (m_exitButton->boundingBox().containsPoint(m_buttonMenu->convertToNodeSpace(ccp(mousepos.x / framesize.width * winsize.width, (1 - mousepos.y / framesize.height) * winsize.height)))) {
        if (m_exitHovered) return;
        changeButtonSprite(m_exitButton, "menu-button-exit-over.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(25, 0));
        m_exitButton->runAction(move_btn);
        m_exitHovered = true;
    }
    else if (m_exitHovered) {
        changeButtonSprite(m_exitButton, "menu-button-exit.png");
        CCMoveBy* move_btn = CCMoveBy::create(0.1f, ccp(-25, 0));
        m_exitButton->runAction(move_btn);
        m_exitHovered = false;
    }

    if (!getChildByTag(5)) { // if not profile layer exists
        m_cookieButton->setOpacity(255);
    }
}

OsuMenu* OsuMenu::switchToOsuMenuButton() {
    OsuMenu* menu = OsuMenu::create();
    pushLayer(menu);
    return menu;
}

// utility function
void OsuMenu::changeButtonSprite(gd::CCMenuItemSpriteExtra* button, const char* spritename) {
    CCSprite* img = CCSprite::create(spritename);
    button->setNormalImage(img);
    button->setSelectedImage(img);
    button->setScale(0.85f);
}

void OsuMenu::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void OsuMenu::backButtonCallback(CCObject* object) {
    keyBackClicked();
}

void OsuMenu::cookieCallback(CCObject* object) {
    if (!m_finishedMoving) {
        m_finishedMoving = true;
        CCMoveBy* move = CCMoveBy::create(0.75f, ccp(-65, 0));
        CCEaseExponentialOut* ease = CCEaseExponentialOut::create(move);
        m_cookieButton->runAction(ease);

        // buttons

        m_buttonMenu->setVisible(true);

        CCFadeIn* fadeIn = CCFadeIn::create(0.25f);
        m_buttonMenu->runAction(fadeIn);

        CCMoveBy* move_btns = CCMoveBy::create(1.25f, ccp(25, 0));
        CCEaseExponentialOut* ease_btns = CCEaseExponentialOut::create(move_btns);
        m_buttonMenu->runAction(ease_btns);
    }
    else {
        pushLayer(gd::LevelSelectLayer::create(0)); // stereo madness
    }
}

void OsuMenu::playCallback(CCObject* object) {
    pushLayer(gd::LevelSelectLayer::create(0)); // stereo madness
}

void OsuMenu::editCallback(CCObject* object) {
    pushLayer(gd::CreatorLayer::create());
}

void OsuMenu::optionsCallback(CCObject* object) {
    gd::OptionsLayer::addToCurrentScene();
}

void OsuMenu::exitCallback(CCObject* object) {
    CCDirector::sharedDirector()->end();
}

void OsuMenu::profileCallback(CCObject* object) {
    m_cookieButton->setOpacity(0);
    gd::ProfilePage* layer = gd::ProfilePage::create(gd::GJAccountManager::sharedState()->m_nPlayerAccountID, true);
    addChild(layer, 2, 5);
}