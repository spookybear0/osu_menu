#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <thread>

#include <cocos2d.h>
#include "MinHook.h"
#include "gd.h"

#include "utilities.h"

USING_NS_CC;

CCScene* pushLayer(CCLayer* layer) {
    CCScene* scene = CCScene::create();
    scene->addChild(layer);
    CCTransitionFade* transition = CCTransitionFade::create(0.5f, scene);
    CCDirector::sharedDirector()->pushScene(transition);
    return scene;
}

class OsuMenu : public CCLayer {
private:
    bool m_finishedMoving = false;
    gd::CCMenuItemSpriteExtra* m_cookieButton;

    CCMenu* m_buttonMenu; 
    
    gd::CCMenuItemSpriteExtra* m_playButton;
    gd::CCMenuItemSpriteExtra* m_editButton;
    gd::CCMenuItemSpriteExtra* m_optionsButton;
    gd::CCMenuItemSpriteExtra* m_exitButton;
public:
    bool init() {
        if (!CCLayer::init()) {
            return false;
        }

        CCSize winSize = CCDirector::sharedDirector()->getWinSize();

        // setup background
        CCSprite* bg_sprite = CCSprite::create("menu-bg.jpg");

        CCSize size = bg_sprite->getContentSize();
        
        bg_sprite->setScaleX(winSize.width / size.width);
        bg_sprite->setScaleY(winSize.height / size.height);
        
        bg_sprite->setAnchorPoint({0, 0});
        
        bg_sprite->setZOrder(-5);
        addChild(bg_sprite);

        // sprite
        CCSprite* cookie_sprite = CCSprite::create("menu-osu.png");
        cookie_sprite->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    
        // button for cookie
        m_cookieButton = gd::CCMenuItemSpriteExtra::create(
            // directly create the sprite here, as its our only use for it
            cookie_sprite,
            this,
            menu_selector(OsuMenu::cookieCallback)
        );

        // click menu
        CCMenu* menu = CCMenu::create();
        menu->setZOrder(5);
        menu->addChild(m_cookieButton);
        addChild(menu);

        // setup buttons
        CCSprite* play_sprite = CCSprite::create("menu-button-play.png");
        CCSprite* edit_sprite = CCSprite::create("menu-button-edit.png");
        CCSprite* options_sprite = CCSprite::create("menu-button-options.png");
        CCSprite* exit_sprite = CCSprite::create("menu-button-exit.png");

        m_playButton = gd::CCMenuItemSpriteExtra::create(play_sprite, this, menu_selector(OsuMenu::playCallback));
        m_editButton = gd::CCMenuItemSpriteExtra::create(edit_sprite, this, menu_selector(OsuMenu::editCallback));
        m_optionsButton = gd::CCMenuItemSpriteExtra::create(options_sprite, this, menu_selector(OsuMenu::optionsCallback));
        m_exitButton = gd::CCMenuItemSpriteExtra::create(exit_sprite, this, menu_selector(OsuMenu::exitCallback));

        m_buttonMenu = CCMenu::create();
        m_buttonMenu->addChild(m_playButton);
        m_buttonMenu->addChild(m_editButton);
        m_buttonMenu->addChild(m_optionsButton);
        m_buttonMenu->addChild(m_exitButton);

        m_buttonMenu->alignItemsVerticallyWithPadding(10);
        m_buttonMenu->setPositionX(winSize.width / 2 + 100);
        m_buttonMenu->setZOrder(2);
        m_buttonMenu->setVisible(false);

        addChild(m_buttonMenu);

        return true;
    }

    static OsuMenu* create() {
        auto ret = new OsuMenu();
        if (ret && ret->init()) {
            ret->autorelease();
        } else {
            delete ret;
            ret = nullptr;
        }
        return ret;
    }

    static void switchToOsuMenuButton() {
        OsuMenu* layer = OsuMenu::create();
        pushLayer(layer);
    }

protected:
    void keyBackClicked() {
        CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

    void backButtonCallback(CCObject* object) {
        keyBackClicked();
    }

    void cookieCallback(CCObject* object) {
        if (!m_finishedMoving) {
            m_finishedMoving = true;
            CCMoveBy* move = CCMoveBy::create(0.75f, ccp(-65, 0));
            CCEaseExponentialOut* ease = CCEaseExponentialOut::create(move);
            m_cookieButton->runAction(ease);
            m_buttonMenu->setVisible(true);
        }
        else {
            gd::LevelSelectLayer* layer = gd::LevelSelectLayer::create(0); // stereo madness
            pushLayer(layer);
        }
    }

    void playCallback(CCObject* object) {
        gd::LevelSelectLayer* layer = gd::LevelSelectLayer::create(0); // stereo madness
        pushLayer(layer);
    }
    void editCallback(CCObject* object) {
        gd::CreatorLayer* layer = gd::CreatorLayer::create();
        pushLayer(layer);
    }
    void optionsCallback(CCObject* object) {
        gd::OptionsLayer* layer = gd::OptionsLayer::create();
        pushLayer(layer);
    }
    void exitCallback(CCObject* object) {
        CCDirector::sharedDirector()->end();
    }
};

bool(__thiscall* MenuLayer_initO)(CCLayer* self);
bool __fastcall MenuLayer_initH(CCLayer* self, void*) {
    if (!MenuLayer_initO(self)) { return false; };
    OsuMenu::switchToOsuMenuButton();
    return true;
}

DWORD WINAPI dll_thread(void* hModule) {
    MH_Initialize();

    MH_CreateHook((PVOID)(utilities::getBase() + 0x1907B0), MenuLayer_initH, (LPVOID*)&MenuLayer_initO);

    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Hello World!" << std::endl;

    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0x1000, dll_thread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

