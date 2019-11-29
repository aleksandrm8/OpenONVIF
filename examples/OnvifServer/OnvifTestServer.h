#ifndef ONVIFTESTSERVER_H__
#define ONVIFTESTSERVER_H__

#include <list>
#include <map>
#include <string.h>
#include <unordered_map>
#include "OnvifSDK.h"

namespace onvif_server {

static const int kMaxUsers = 10;
static std::map<const char*, int> g_method_classes;

class OnvifTestServer :
        public IOnvifDevMgmt,
        public IOnvifMedia
{
public:
    static const char *kInterfaceName;

    OnvifTestServer(IOnvifServer * server, std::list<CUser> default_users);
    virtual int GetDateAndTime( DevGetSystemDateAndTimeResponse & );
    virtual int SetDateAndTime( DevSetSystemDateAndTime &, struct soap &soap );
    virtual int GetUsers( DevGetUsersResponse & );
    virtual int CreateUsers( DevCreateUsers & );
    virtual int DeleteUsers( DevDeleteUsers & );
    virtual int SetUser( DevSetUser & );


    virtual int GetProfile(const std::string & profileToken, MedGetProfileResponse & resp);
    virtual int GetProfiles(MedGetProfilesResponse &);
    virtual int GetVideoSources(MedGetVideoSourcesResponse &);
    virtual int GetStreamUri( const std::string& token, std::string & uri);
    virtual int GetCompatibleVideoEncoderConfigurations( MedGetCompatibleVideoEncoderConfigurationsResponse& r );
    virtual int GetCompatibleVideoAnalyticsConfigurations( MedGetCompatibleVideoAnalyticsConfigurationsResponse& r );
    virtual int AddVideoAnalyticsConfiguration( const std::string& profileToken,
                                                const std::string& confToken );
    CUser GetUserInfo(const char *username) override;
    bool SecurityMode() override;
private:
    static const std::string kVideoProfileName;
    static const std::string kVideoProfileToken;

	static const std::string kVideoSourceTokenName;
	static const std::string kVideoSourceTokenContent;

	static const std::string kAnalyticTokenName;
	static const std::string kAnalyticTokenContent;

    static const char *kUsersCredentialFiles;

	static const int kImageWidth	= 3840;
	static const int kImageHeight	= 2160;
    static const int kFrmRate		= 20;


	std::string							m_media_profile_name;
	std::string							m_media_profile_token;
	MedVideoSource 						m_video_source;
    MedVideoSource 						m_radar_source;
    MedProfile 							m_video_profile;
    MedProfile 							m_radar_data_profile;
    MedVideoAnalyticsConfiguration 		m_anal_conf;

    std::unordered_map<std::string, CUser> m_users;

};

}

#endif	// ONVIFTESTSERVER_H__
