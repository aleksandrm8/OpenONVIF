
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>

#include <stdio.h>
#include <fstream>
#include <sstream>

#include "sigrlog.h"
#include "stdsoap2.h"
#include "OnvifTestServer.h"

namespace onvif_server {

const std::string OnvifTestServer::kVideoProfileName 		= "video_profile";
const std::string OnvifTestServer::kVideoProfileToken 		= "video_profile_token_content";

const std::string OnvifTestServer::kVideoSourceTokenName	= "video_source_token_name";
const std::string OnvifTestServer::kVideoSourceTokenContent	= "video_source_token_content";

const std::string OnvifTestServer::kAnalyticTokenName		= "analytic_token_name";
const std::string OnvifTestServer::kAnalyticTokenContent	= "analytic_token_content";

const char *OnvifTestServer::kUsersCredentialFiles = "credentials.txt";

const char *OnvifTestServer::kInterfaceName = "eth0";

std::unordered_map<std::string, CUser> DeserializeUsers(const char *file_name)
{
    std::ifstream file(file_name);
    if (!file.good())
        return {};
    std::unordered_map<std::string, CUser> ret;

    while (!file.eof())
    {
        std::string username;
        std::string password;
        std::string role_str;
        for (std::string *element : {&username, &password, &role_str})
        {
            char c;
            file.get(c);
            while ((c == '\r' || c == '\n' || c == ' ') && !file.eof())
                file.get(c);
            while (c != ' ' && c != '\r' && c != '\n' && !file.eof())
            {
                *element += c;
                file.get(c);
            }
        }
        const UsersRoles role = StrToRole(role_str);
        if (Undefined != role)
            ret[username] = {username, password, };
    }
    return ret;
}

std::string getInterfaceIp( const std::string & interface ) {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    getifaddrs(&ifAddrStruct);
    std::string result("");

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa || !ifa->ifa_addr)
            continue;
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            SIGRLOG( SIGRDEBUG3, "%s IP Address %s", ifa->ifa_name, addressBuffer);
            if(! strcmp(ifa->ifa_name, interface.c_str()))
                result = std::string(addressBuffer);
        } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            SIGRLOG( SIGRDEBUG3, "%s IP Address %s", ifa->ifa_name, addressBuffer);
            if( ( !strcmp(ifa->ifa_name, interface.c_str()) ) && result.empty() )
                result = std::string(addressBuffer);
        }
    }

    if( ifAddrStruct != NULL )
        freeifaddrs(ifAddrStruct);

    return result;
}

void SerializeUsers(const char *file_name, const std::unordered_map<std::string, CUser> &users)
{
    std::ofstream file(file_name);
    for (const auto & user : users)
    {
        file << user.second.name << " " << user.second.password << " "
            << RoleToStr(user.second.role) << std::endl;
    }
}

OnvifTestServer::OnvifTestServer(IOnvifServer * server, std::list<CUser> default_users):
    m_users(DeserializeUsers(kUsersCredentialFiles))
{
    for (auto &user : default_users)
    {
        m_users[user.name] = user;
    }
    SerializeUsers(kUsersCredentialFiles, m_users);
    m_media_profile_name = kVideoProfileName;
    m_media_profile_token = kVideoProfileToken;

    m_video_source = server->CreateVideoSource(	kVideoProfileToken,
													kImageWidth,
                                                    kImageHeight,
													kFrmRate);
    m_video_profile 		= server->CreateMediaProfile(kVideoProfileName,
                                                         kVideoProfileToken);

    m_anal_conf = server->CreateVAConf(   kVideoProfileName,
                                            kVideoProfileToken,
                                            CellDetectionLayout::ALL);

    m_video_profile.AddVideoEnc(kImageWidth, kImageHeight);

    m_video_profile.AddVideoSrc(kVideoProfileToken, kImageWidth, kImageHeight);

    m_video_profile.AddVideoAnalytics(m_anal_conf);
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

int OnvifTestServer::SetDateAndTime(DevSetSystemDateAndTime & dt, struct soap &soap)
{
    int year, month, day, hour, min, sec;
    dt.GetUTCDateAndTime(year, month, day, hour, min, sec);

    struct tm tm{sec, min, hour, day, month - 1, year - 1900};

    time_t time = mktime(&tm);
    if (!time || -1 == time)
    {
        SIGRLOG(SIGRWARNING, "%s mktime failed", __FUNCTION__);
        SIGRLOG(SIGRWARNING, "error %s", strerror(errno));
        SIGRLOG(SIGRWARNING, "mktime return %d", time);
        *soap_faultstring(&soap) = "Can not set specified time";
        return SOAP_FAULT;
    }
    SIGRLOG(SIGRWARNING, "time = %d", time );
    struct timeval timeval{time, 0};
    int ret = settimeofday(&timeval, NULL);
    if (0 != ret)
    {
        SIGRLOG(SIGRWARNING, "%s settimeofday failed", __FUNCTION__);
        SIGRLOG(SIGRWARNING, "error %s", strerror(errno));
        SIGRLOG(SIGRWARNING, "settimeofday return %d", ret);
        SIGRLOG(SIGRWARNING, "year %d", year);
        SIGRLOG(SIGRWARNING, "month %d", month);
        SIGRLOG(SIGRWARNING, "day %d", day);
        SIGRLOG(SIGRWARNING, "hour %d", hour);
        SIGRLOG(SIGRWARNING, "min %d", min);
        SIGRLOG(SIGRWARNING, "sec %d", sec);
        *soap_faultstring(&soap) = "Can not set specified time";
        return SOAP_FAULT;
    }

    return 0;
}

int OnvifTestServer::GetUsers( /*out*/ DevGetUsersResponse & resp)
{
    for (auto &user : m_users)
    {
        std::vector<std::string> vec;
        vec.push_back(user.first);
        // do not include password as it noted in ONVIF Core specification
        vec.push_back("");
        vec.push_back(RoleToStr(user.second.role));
        resp.AddUser(vec);
    }

    return 0;
}

int OnvifTestServer::CreateUsers( DevCreateUsers & recv)
{
    std::vector<CUser> new_users;
    if (recv.GetUsers(new_users))
    {
        SIGRLOG(SIGRCRITICAL, "%s: wrong arguments", __FUNCTION__);
        return -1;
    }
    if (m_users.size() + new_users.size() >= kMaxUsers)
    {
        SIGRLOG(SIGRCRITICAL, "%s: Too many users", __FUNCTION__);
        return -1;
    }
    for (auto &user : new_users)
    {
        if (m_users.find(user.name) != m_users.end())
        {
            SIGRLOG(SIGRCRITICAL, "%s: user %s already exist", __FUNCTION__, user.name.c_str());
            return -1;
        }
        if (user.role == Undefined)
        {
            SIGRLOG(SIGRCRITICAL, "%s: undefined user role %s", __FUNCTION__,
                    RoleToStr(user.role).c_str());
            return -1;
        }
    }
    for (auto &user : new_users)
    {
        m_users[user.name] = user;
    }
    SerializeUsers(kUsersCredentialFiles, m_users);
    return 0;
}

int OnvifTestServer::DeleteUsers( DevDeleteUsers & recv)
{
    std::vector<std::string> usernames;
    if (recv.GetUsers(usernames))
    {
        SIGRLOG(SIGRCRITICAL, "%s: wrong arguments", __FUNCTION__);
        return -1;
    }
    for (const std::string &user_to_delete : usernames)
    {
        const auto user_iterator = m_users.find(user_to_delete);
        if (user_iterator == m_users.end())
        {
            SIGRLOG(SIGRWARNING, "%s: can not delete user %s", __FUNCTION__,
                    user_to_delete.c_str());
            continue;
        }
        m_users.erase(user_iterator);
    }
    SerializeUsers(kUsersCredentialFiles, m_users);
    return 0;
}

int OnvifTestServer::SetUser( DevSetUser & recv)
{
    std::vector<CUser> modify_users;
    if (recv.GetUsers(modify_users))
    {
        SIGRLOG(SIGRCRITICAL, "%s: wrong arguments", __FUNCTION__);
        return -1;
    }
    for (const CUser &user_to_modify : modify_users)
    {
        auto user_iterator = m_users.find(user_to_modify.name);
        if (user_iterator == m_users.end())
        {
            SIGRLOG(SIGRWARNING, "%s: can not modify user %s", __FUNCTION__,
                    user_to_modify.name.c_str());
            continue;
        }
        user_iterator->second.password = user_to_modify.password;
        user_iterator->second.role = user_to_modify.role;
    }
    SerializeUsers(kUsersCredentialFiles, m_users);
    return 0;
}

int OnvifTestServer::GetProfile(const std::string & profileToken, MedGetProfileResponse & resp)
{
    resp.SetProfile(m_video_profile);
    return 0;
}
int OnvifTestServer::GetProfiles(MedGetProfilesResponse &t)
{
    t.AddProfile(m_video_profile);
    return 0;
}
int OnvifTestServer::GetVideoSources(MedGetVideoSourcesResponse &t)
{
    t.AddEntry(m_video_source);
    return 0;
}

int OnvifTestServer::GetStreamUri( const std::string& token, std::string & uri)
{
    uri = "rtsp://" + getInterfaceIp(kInterfaceName) + ":8554/test";
    return 0;
}
int OnvifTestServer::GetCompatibleVideoEncoderConfigurations( MedGetCompatibleVideoEncoderConfigurationsResponse& r )
{
    r.AddVideoEnc(kImageWidth, kImageHeight);
    return 0;
}
int OnvifTestServer::GetCompatibleVideoAnalyticsConfigurations( MedGetCompatibleVideoAnalyticsConfigurationsResponse& r )
{
    r.AddVideoAn(m_anal_conf);
    return 0;
}
int OnvifTestServer::AddVideoAnalyticsConfiguration( const std::string& profileToken,
													const std::string& confToken )
{
    return 0;
}

CUser OnvifTestServer::GetUserInfo(const char *username)
{
    if (m_users.find(username) != m_users.end())
    {
        return m_users[username];
    }
    return {"", "", Undefined};
}

bool OnvifTestServer::SecurityMode()
{
    return m_users.empty() ? false : true;
}

}
