
#include "DeviceClient.h"

#ifdef DEV_S
DeviceClient::DeviceClient(const char * pchAdress, soap * s):m_proxy(s)
{
	m_proxy.soap_endpoint = pchAdress;
}

int DeviceClient::GetDateAndTime(DevGetSystemDateAndTimeResponse & resp)
{
	DevGetSystemDateAndTime req(m_proxy.soap);

	int nRes = m_proxy.GetSystemDateAndTime(req.d, resp.d);

	CHECKRETURN(nRes, "DeviceClient::GetDateAndTime");
}


int DeviceClient::SetDateAndTime(DevSetSystemDateAndTime & req, struct soap & soap)
{
    DevSetSystemDateAndTimeResponse resp(m_proxy.soap);

	int nRes = m_proxy.SetSystemDateAndTime(req.d, resp.d);

	CHECKRETURN(nRes, "DeviceClient::SetDateAndTime");
}

int DeviceClient::GetUsers(DevGetUsersResponse & resp)
{
	DevGetUsers req(m_proxy.soap);

	int nRes = m_proxy.GetUsers(req.d, resp.d);

	CHECKRETURN(nRes, "DeviceClient::GetUsers");
}

int DeviceClient::CreateUsers(DevCreateUsers & req)
{
	DevCreateUsersResponse resp(m_proxy.soap);

	int nRes = m_proxy.CreateUsers(req.d, resp.d);

	CHECKRETURN(nRes, "DeviceClient::GetUsers");
}

int DeviceClient::DeleteUsers(DevDeleteUsers & req)
{
	DevDeleteUsersResponse resp(m_proxy.soap);

	int nRes = m_proxy.DeleteUsers(req.d, resp.d);

	CHECKRETURN(nRes, "DeviceClient::GetUsers");
}

int DeviceClient::SetUser(DevSetUser & req)
{
	DevSetUserResponse resp(m_proxy.soap);

	int nRes = m_proxy.SetUser(req.d, resp.d);

	CHECKRETURN(nRes, "DeviceClient::GetUsers");
}

#endif //DEV_S
