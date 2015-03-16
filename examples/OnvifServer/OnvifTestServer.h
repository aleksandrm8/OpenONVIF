#ifndef ONVIFTESTSERVER_H__
#define ONVIFTESTSERVER_H__

#include <thread>
#include "OnvifSDK.h"

class OnvifTestServer :
        public IOnvifDevMgmt,
        public IOnvifMedia
{
static const std::string kVideoProfileName;
static const std::string kVideoProfileToken;

static const std::string kRadarProfileName;
static const std::string kRadarProfileToken;

	static const std::string kVideoSourceTokenName;
	static const std::string kVideoSourceTokenContent;

	static const std::string kAnalyticTokenName;
	static const std::string kAnalyticTokenContent;

	static const int kImageWidth	= 352;
	static const int kImageHeight	= 288;
    static const int kFrmRate		= 25;



public:
    OnvifTestServer(IOnvifServer * server);
    virtual int GetDateAndTime( DevGetSystemDateAndTimeResponse & );
    virtual int SetDateAndTime( DevSetSystemDateAndTime & ) { return -1; }
    virtual int GetUsers( DevGetUsersResponse & );


virtual int GetProfile(const std::string & profileToken, MedGetProfileResponse & resp);
virtual int GetProfiles(MedGetProfilesResponse &);
virtual int GetVideoSources(MedGetVideoSourcesResponse &);
virtual int GetStreamUri( const std::string& token, std::string & uri);
virtual int GetCompatibleVideoEncoderConfigurations( MedGetCompatibleVideoEncoderConfigurationsResponse& r );
virtual int GetCompatibleVideoAnalyticsConfigurations( MedGetCompatibleVideoAnalyticsConfigurationsResponse& r );
virtual int AddVideoAnalyticsConfiguration( const std::string& profileToken,
                                            const std::string& confToken );

	std::string							m_media_profile_name;
	std::string							m_media_profile_token;
	MedVideoSource 						m_video_source;
    MedVideoSource 						m_radar_source;
    MedProfile 							m_video_profile;
    MedProfile 							m_radar_data_profile;
    MedVideoAnalyticsConfiguration 		m_anal_conf;
    std::thread                         m_thread;


};

#endif	// ONVIFTESTSERVER_H__
