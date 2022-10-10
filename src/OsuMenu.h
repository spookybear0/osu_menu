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

USING_NS_CC;

class OsuMenu: public CCLayer {
private:
    bool m_finishedMoving = false;
    gd::CCMenuItemSpriteExtra* m_cookieButton;

    CCMenu* m_cookieMenu;
    CCMenu* m_buttonMenu;
    
    gd::CCMenuItemSpriteExtra* m_playButton;
    gd::CCMenuItemSpriteExtra* m_editButton;
    gd::CCMenuItemSpriteExtra* m_optionsButton;
    gd::CCMenuItemSpriteExtra* m_exitButton;

    bool m_playHovered = false;
    bool m_editHovered = false;
    bool m_optionsHovered = false;
    bool m_exitHovered = false;
public:
    bool init();
    static OsuMenu* create();
    void update(float dt);
    static OsuMenu* switchToOsuMenuButton();

    CCLabelTTF* m_rank;
protected:
    void changeButtonSprite(gd::CCMenuItemSpriteExtra* button, const char* spriteName);
    void keyBackClicked();
    void backButtonCallback(CCObject* object);
    void cookieCallback(CCObject* object);
    void playCallback(CCObject* object);
    void editCallback(CCObject* object);
    void optionsCallback(CCObject* object);
    void exitCallback(CCObject* object);
    void profileCallback(CCObject* object);
};