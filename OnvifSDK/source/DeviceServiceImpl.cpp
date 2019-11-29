#include "DeviceServiceImpl.h"

#ifdef DEV_S

#include <set>
#include <string.h>

#include "sigrlog.h"
#include "wsseapi.h"
#include "BaseServer.h"
#include "Authenticate.h"

int
DeviceServiceImpl::GetSystemDateAndTime( _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime,
                                         _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse ) {
    int nRes = Verify(handler_, soap);
    if (SOAP_OK != nRes)
        return nRes;

	DevGetSystemDateAndTimeResponse dt(tds__GetSystemDateAndTimeResponse);
    nRes = handler_->GetDateAndTime(dt);
	CHECKRETURN(nRes, "DeviceServiceImpl::GetSystemDateAndTime");
}

// for while not supporting copy
DeviceBindingService* DeviceServiceImpl::copy() {
	return NULL;
}

int
DeviceServiceImpl::SetSystemDateAndTime( _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime,
                                         _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse ) {
    int nRes = Verify(handler_, soap);
    if (SOAP_OK != nRes)
        return nRes;
    if(tds__SetSystemDateAndTime->DateTimeType != tt__SetDateTimeType__Manual) {
		SIGRLOG(SIGRWARNING, "DeviceServiceImpl::SetSystemDateAndTime Time is not Manual");
		return SOAP_ERR;
	}

	DevSetSystemDateAndTime dt(tds__SetSystemDateAndTime);
    nRes = handler_->SetDateAndTime(dt, *soap);
	CHECKRETURN(nRes, "DeviceServiceImpl::SetSystemDateAndTime");
}

int
DeviceServiceImpl::GetCapabilities( _tds__GetCapabilities *tds__GetCapabilities,
                                    _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse ) {
	DevGetCapabilitiesResponse resp(tds__GetCapabilitiesResponse);
    resp.SetDeviceCapabilities( baseServer_->m_endpoint );
    if( baseServer_->m_deviceType == OnvifDevice::NVT ) {
        resp.SetEventsCapabilities( baseServer_->m_endpoint );
        resp.SetMediaCapabilities( baseServer_->m_endpoint );
        resp.SetAnalyticsCapabilities( baseServer_->m_endpoint );
    }

	return SOAP_OK;
}

int
DeviceServiceImpl::GetDeviceInformation( _tds__GetDeviceInformation *tds__GetDeviceInformation,
                                         _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse ) {
	DevGetDeviceInformationResponse resp(tds__GetDeviceInformationResponse);
    resp.SetDeviceInfo( baseServer_->m_manufacturer, baseServer_->m_model,
                        baseServer_->m_firmwareVersion, baseServer_->m_serialNumber, baseServer_->m_hardwareId );

	return SOAP_OK;
}

int
DeviceServiceImpl::GetUsers( _tds__GetUsers *tds__GetUsers,
                             _tds__GetUsersResponse *tds__GetUsersResponse) {
    int nRes = Verify(handler_, soap);
    if (SOAP_OK != nRes)
        return nRes;

	DevGetUsersResponse resp(tds__GetUsersResponse);
    nRes = handler_->GetUsers(resp);
	CHECKRETURN(nRes, "DeviceServiceImpl::GetUsers");
}

int
DeviceServiceImpl::CreateUsers( _tds__CreateUsers *tds__CreateUsers,
                             _tds__CreateUsersResponse *tds__CreateUsersResponse) {
    int nRes = Verify(handler_, soap);
    if (SOAP_OK != nRes)
        return nRes;

	DevCreateUsers recv(tds__CreateUsers);
    nRes = handler_->CreateUsers(recv);
	CHECKRETURN(nRes, "DeviceServiceImpl::CreateUsers");
}

int
DeviceServiceImpl::DeleteUsers(_tds__DeleteUsers *tds__DeleteUsers,
                                _tds__DeleteUsersResponse *tds__DeleteUsersResponse) {
    int nRes = Verify(handler_, soap);
    if (SOAP_OK != nRes)
        return nRes;

	DevDeleteUsers recv(tds__DeleteUsers);
    nRes = handler_->DeleteUsers(recv);
	CHECKRETURN(nRes, "DeviceServiceImpl::DeleteUsers");
}

int
DeviceServiceImpl::SetUser(_tds__SetUser *tds__SetUser,
                            _tds__SetUserResponse *tds__SetUserResponse) {
    int nRes = Verify(handler_, soap);
    if (SOAP_OK != nRes)
        return nRes;

	DevSetUser recv(tds__SetUser);
    nRes = handler_->SetUser(recv);
	CHECKRETURN(nRes, "DeviceServiceImpl::SetUser");
}

int
DeviceServiceImpl::GetServices( _tds__GetServices *tds__GetServices,
                                _tds__GetServicesResponse *tds__GetServicesResponse ) {

    int nRes = Verify(handler_, soap);
    if (SOAP_OK != nRes)
        return nRes;
	DevGetServices req(tds__GetServices);
    DevGetServicesResponse resp(tds__GetServicesResponse);
    if( req.d->IncludeCapability )
        SIGRLOG(SIGRWARNING, "DeviceServiceImpl::GetServices 'Including Capabilities' still not implemented");

    resp.AddService( "http://www.onvif.org/ver10/events/wsdl",  baseServer_->m_endpoint, 1 );
    resp.AddService( "http://www.onvif.org/ver20/analytics/wsdl",  baseServer_->m_endpoint, 2 );
	return 0;
}

int
DeviceServiceImpl::GetScopes( _tds__GetScopes *tds__GetScopes,
                              _tds__GetScopesResponse *tds__GetScopesResponse ) {
    DevGetScopesResponse resp(tds__GetScopesResponse);
    resp.AddItems( baseServer_->m_scopes );
    return SOAP_OK;
}

#endif //DEV_S
