
#include <stdio.h>
#include <chrono>
#include "sigrlog.h"
#include "OnvifTestServer.h"

const std::string OnvifTestServer::kVideoProfileName 		= "video_profile";
const std::string OnvifTestServer::kVideoProfileToken 		= "video_profile_token_content";

const std::string OnvifTestServer::kRadarProfileName 		= "radar_profile";
const std::string OnvifTestServer::kRadarProfileToken 		= "radar_profile_token_content";

const std::string OnvifTestServer::kVideoSourceTokenName	= "video_source_token_name";
const std::string OnvifTestServer::kVideoSourceTokenContent	= "video_source_token_content";

const std::string OnvifTestServer::kAnalyticTokenName		= "analytic_token_name";
const std::string OnvifTestServer::kAnalyticTokenContent	= "analytic_token_content";


void thread_fun()
{
    std::chrono::seconds sec(1);
    while(1)
    {
        std::this_thread::sleep_for(sec);
    }
}

OnvifTestServer::OnvifTestServer(IOnvifServer * server)
{
    m_media_profile_name = kVideoProfileName;
    m_media_profile_token = kVideoProfileToken;

    m_video_source = server->CreateVideoSource(	kVideoProfileToken,
													kImageWidth,
                                                    kImageHeight,
													kFrmRate);
    m_radar_source = server->CreateVideoSource(	kRadarProfileToken,
                                                    kImageWidth,
                                                    kImageHeight,
                                                    kFrmRate);
    m_video_profile 		= server->CreateMediaProfile(kVideoProfileName,
                                                         kVideoProfileToken);
    m_radar_data_profile 	= server->CreateMediaProfile(kRadarProfileName,
                                                         kRadarProfileToken);

    m_anal_conf = server->CreateVAConf(   kVideoProfileName,
                                            kVideoProfileToken,
                                            CellDetectionLayout::ALL);

    m_video_profile.AddVideoEnc(kImageWidth, kImageHeight);
    m_radar_data_profile.AddVideoEnc(kImageWidth, kImageHeight);

    m_video_profile.AddVideoSrc(kVideoProfileToken, kImageWidth, kImageHeight);
    m_radar_data_profile.AddVideoSrc(kRadarProfileToken, kImageWidth, kImageHeight);

    m_video_profile.AddVideoAnalytics(m_anal_conf);
    m_thread = std::thread(thread_fun);
}

int OnvifTestServer::GetDateAndTime(DevGetSystemDateAndTimeResponse & dt)
{
    time_t t = time(0);
    struct tm * now = localtime( & t );

    if(now == NULL)
    {
        SIGRLOG(SIGRWARNING, "OnvifTestServer::GetDateAndTime Getting localtime failed");
        return -1;
    }

    dt.SetUTCDateAndTime(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

    return 0;
}

int OnvifTestServer::GetUsers( /*out*/ DevGetUsersResponse & resp) {
    std::vector<std::string> vec;
    vec.push_back("admin");
    vec.push_back("admin");
    vec.push_back("Administrator");
    resp.AddUser(vec);
    return 0;
}

int OnvifTestServer::GetProfile(const std::string & profileToken, MedGetProfileResponse & resp)
{
    printf("OnvifTestServer::GetProfile\n");
    resp.SetProfile(m_video_profile);
    return 0;
}
int OnvifTestServer::GetProfiles(MedGetProfilesResponse &t)
{
    printf("OnvifTestServer::GetProfiles\n");
    t.AddProfile(m_video_profile);
    t.AddProfile(m_radar_data_profile);
    return 0;
}
int OnvifTestServer::GetVideoSources(MedGetVideoSourcesResponse &t)
{
    t.AddEntry(m_video_source);
    t.AddEntry(m_radar_source);
    printf("OnvifTestServer::GetVideoSources\n");
    return 0;
}

std::string GetRadarUrl()
{
    return "rtsp://10.176.11.4:9554/radar_data";
}

std::string GetVideoUrl()
{
    return "rtsp://10.176.11.4:8554/testStream";
}

int OnvifTestServer::GetStreamUri( const std::string& token, std::string & uri)
{
    printf("OnvifTestServer::GetStreamUri\n");
    //uri = "http://10.176.11.4:8181/stream_video_from_orwr_server";
    //uri = "rtsp://10.176.11.4:8554/test.mpg";

    if (!token.compare(kRadarProfileToken))
    {
        uri = GetRadarUrl();
    }
    else if (!token.compare(kVideoProfileToken))
    {
        uri = GetVideoUrl();
    }
    else
    {
        // wrong token
        uri = GetRadarUrl();
    }

    return 0;
}
int OnvifTestServer::GetCompatibleVideoEncoderConfigurations( MedGetCompatibleVideoEncoderConfigurationsResponse& r )
{
    printf("OnvifTestServer::GetCompatibleVideoEncoderConfigurations\n");
    r.AddVideoEnc(kImageWidth, kImageHeight);
    return 0;
}
int OnvifTestServer::GetCompatibleVideoAnalyticsConfigurations( MedGetCompatibleVideoAnalyticsConfigurationsResponse& r )
{
    printf("OnvifTestServer::GetCompatibleVideoAnalyticsConfigurations\n");
    r.AddVideoAn(m_anal_conf);
    return 0;
}
int OnvifTestServer::AddVideoAnalyticsConfiguration( const std::string& profileToken,
													const std::string& confToken )
{
    printf("OnvifTestServer::AddVideoAnalyticsConfiguration\n");
    return 0;
}

