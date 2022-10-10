#ifndef __PROFILEPAGE_H__
#define __PROFILEPAGE_H__

#include <gd.h>

namespace gd {
    class FLAlertLayer;

    class GDH_DLL ProfilePage : public FLAlertLayer,
        FLAlertLayerProtocol,
        LevelCommentDelegate,
        CommentUploadDelegate,
        UserInfoDelegate,
        UploadActionDelegate,
        UploadPopupDelegate,
        LeaderboardManagerDelegate
    {
        public:
            GJUserScore* m_score;
            int m_accountID;
            PAD(56);
            cocos2d::CCArray* m_buttons;

        public:
            static ProfilePage* create(int accountID, bool idk) {
                return reinterpret_cast<ProfilePage*(__fastcall*)(
                    int, bool
                )>(
                    base + 0x20ee50
                )(
                    accountID, idk
                );
            }
    };
}


#endif
