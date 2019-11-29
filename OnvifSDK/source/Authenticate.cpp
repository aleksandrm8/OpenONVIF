#include "Authenticate.h"

#include "wsseapi.h"
#include "sigrlog.h"

#include <map>
#include <set>

enum AccessClass
{
    PRE_AUTH,
    READ_SYSTEM,
    READ_SYSTEM_SENSITIVE,
    READ_SYSTEM_SECRET,
    WRITE_SYSTEM,
    UNRECOVERABLE,
    READ_MEDIA,
    ACTUATE
};

std::map<UsersRoles, std::set<AccessClass> > GetUsersRoles()
{
    std::map<UsersRoles, std::set<AccessClass> > res;
    res[Administrator].insert(PRE_AUTH);
    res[Administrator].insert(READ_SYSTEM);
    res[Administrator].insert(READ_SYSTEM_SENSITIVE);
    res[Administrator].insert(READ_SYSTEM_SECRET);
    res[Administrator].insert(WRITE_SYSTEM);
    res[Administrator].insert(UNRECOVERABLE);
    res[Administrator].insert(READ_MEDIA);
    res[Administrator].insert(ACTUATE);

    res[Operator].insert(PRE_AUTH);
    res[Operator].insert(READ_SYSTEM);
    res[Operator].insert(READ_SYSTEM_SENSITIVE);
    res[Operator].insert(READ_MEDIA);
    res[Operator].insert(ACTUATE);

    res[User].insert(PRE_AUTH);
    res[User].insert(READ_SYSTEM);
    res[User].insert(READ_MEDIA);

    res[Anonymous].insert(PRE_AUTH);

    return res;
}

std::map<UsersRoles, std::set<AccessClass> > g_users_role = GetUsersRoles();

struct cmp_str
{
    bool operator()(char const *a, char const *b)
    {
        return strcmp(a, b) < 0;
    }
};

std::map<const char*, AccessClass, cmp_str> GetMethodClasses()
{
    std::map<const char*, AccessClass, cmp_str> res;
    res["GetServices"]          = PRE_AUTH;
    res["GetSystemDateAndTime"] = PRE_AUTH;
    res["SetSystemDateAndTime"] = WRITE_SYSTEM;
    res["GetUsers"]             = READ_SYSTEM_SECRET;
    res["CreateUsers"]          = WRITE_SYSTEM;
    res["DeleteUsers"]          = WRITE_SYSTEM;
    res["SetUser"]              = WRITE_SYSTEM;
    res["GetProfiles"]          = READ_MEDIA;
    res["GetProfile"]          = READ_MEDIA;
    res["GetVideoSources"]          = READ_MEDIA;
    res["GetCapabilities"]          = PRE_AUTH;
    res["GetScopes"]          = READ_SYSTEM;
    res["GetNetworkInterfaces"]          = READ_SYSTEM;
    res["GetDeviceInformation"]          = READ_SYSTEM;
    res["GetDNS"]          = READ_SYSTEM;
    res["GetSnapshotUri"]          = READ_MEDIA;
    return res;
}

std::map<const char*, AccessClass, cmp_str> g_method_classes = GetMethodClasses();

int Verify(IOnvifHandler *handler, soap *soap)
{
    const char *method_name = strrchr(soap->action, '/') + 1;
    if (method_name[0] == '\0')
    {
        std::string m(soap->action);
        m[m.size() - 1] = '\0';
        method_name = strrchr(m.c_str(), '/') + 1;
        char *wsdl = "wsdl";
        if (strstr(method_name, wsdl) == method_name)
            method_name += 4;
    }
    if (g_method_classes.find(method_name) == g_method_classes.end())
    {
        SIGRLOG(SIGRCRITICAL, "%s can not find method access class for method %s",
                __FUNCTION__, method_name);
        return SOAP_ERR;
    }
    AccessClass method_access = g_method_classes.at(method_name);
    if (handler->SecurityMode() && method_access != PRE_AUTH)
    {
        const char *username = soap_wsse_get_Username(soap);
        if (!username)
        {
            soap_wsse_delete_Security(soap); // remove old security headers
            return soap->error; // no username: return FailedAuthentication (from soap_wsse_get_Username)
        }


        if (soap && soap->header && soap->header->wsse__Security && soap->header->wsse__Security->UsernameToken
                && soap->header->wsse__Security->UsernameToken->wsu__Created)
        {
            time_t expired, now = time(NULL);
            soap_s2dateTime(soap, soap->header->wsse__Security->UsernameToken->wsu__Created, &expired);
            char *created = soap->header->wsse__Security->UsernameToken->wsu__Created;
            /*if (expired > now + 300)
            {
                created[strlen(created) - 1] = '\0';
            }*/
        }


        CUser user = handler->GetUserInfo(username);
        if (soap_wsse_verify_Password(soap, user.password.c_str()))
        {
            int err = soap->error;
            soap_wsse_delete_Security(soap); // remove old security headers
            return err;
        }
        if (g_users_role.find(user.role) == g_users_role.end())
        {
		    SIGRLOG(SIGRCRITICAL, "%s can not find role for user %s", __FUNCTION__, user.name.c_str());
		    return SOAP_ERR;
        }

        std::set<AccessClass> users_access = g_users_role[user.role];
        if (users_access.find(method_access) == users_access.end())
        {
		    SIGRLOG(SIGRCRITICAL, "%s user %s have no rights for method %s",
                    __FUNCTION__, user.name.c_str(), method_name);
		    return SOAP_ERR;
        }
    }
	return SOAP_OK;
}
