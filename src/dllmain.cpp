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

OsuMenu* menu;

std::string getGDPath() {
    char path_c[MAX_PATH + 1];
    GetModuleFileNameA(NULL, path_c, sizeof(path_c));
    size_t pos = std::string::npos;
    std::string path = std::string(path_c);
    while ((pos = path.find("GeometryDash.exe")) != std::string::npos)
    {
        path.erase(pos, std::string("GeometryDash.exe").length());
    }
    return path;
}

CCTransitionFade* __fastcall MenuLayer_createH() {
    menu = OsuMenu::create();
    CCScene* scene = CCScene::create();
    scene->addChild(menu);
    CCTransitionFade* transition = CCTransitionFade::create(0.5f, scene);
    return transition;
}

// set the rank text in the menu
void(__thiscall* ProfilePage_loadPageFromUserInfoO)(gd::ProfilePage* self, gd::GJUserScore* score);
void __fastcall ProfilePage_loadPageFromUserInfoH(gd::ProfilePage* self, void*, gd::GJUserScore* score) {
    ProfilePage_loadPageFromUserInfoO(self, score);
    menu->m_rank->setString((std::string("#") + std::to_string(score->getGlobalRank())).c_str());
}

DWORD WINAPI dll_thread(void* hModule) {
    MH_Initialize();

    MH_CreateHook((PVOID)(gd::base + 0x190550), MenuLayer_createH, NULL);
    MH_CreateHook((PVOID)(gd::base + 0x210040), ProfilePage_loadPageFromUserInfoH, (LPVOID*)&ProfilePage_loadPageFromUserInfoO);

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

