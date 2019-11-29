
#include "BaseServer.h"
#include "wsseapi.h"
#include "wsaapi.h"

static GeneratorInitializer generatorInitializer;

IOnvifService::~IOnvifService() {}
IOnvifHandler::~IOnvifHandler() {}

std::string
GenerateToken() {
    std::string str;
    for(unsigned int i = 0; i < TOKEN_LEN; ++i)
        str += alphanum[rand() % stringLength];
    return str;
}

IOnvifServer*
getOnvifServer() {
    return static_cast<IOnvifServer*>( new BaseServer() );
}

void
deleteOnvifServer(IOnvifServer* obj) {
    BaseServer::deleteObject(obj);
}

int http_get(struct soap *soap)
{
   soap_response(soap, SOAP_HTML); // HTTP response header with text/html
   soap_send(soap, "<HTML><img src=\"data:image/png;base64,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\" alt=\"ElvesNeotek\"></HTML>");
   soap_end_send(soap);

   return SOAP_OK;
}

BaseServer::BaseServer():
    soap_(soap_new()) {
}

MedProfile
BaseServer::CreateMediaProfile( const std::string& name,
                                const std::string& token ) {
    return MedProfile(soap_, name , token);
}

MedVideoSource
BaseServer::CreateVideoSource( const std::string &token,
                               int w, int h, double frmRate ) {
    return MedVideoSource(soap_, token, w, h, frmRate);
}

MedVideoAnalyticsConfiguration
BaseServer::CreateVAConf( const std::string& name,
                          const std::string& token,
                          CellDetectionLayout::Fill fill ) {
    return MedVideoAnalyticsConfiguration(soap_, name, token, fill);
}

IOnvifService*
BaseServer::createService(OnvifService::Type type, IOnvifHandler * handler) {
    IOnvifService* service = NULL;
    switch(type) {
    case OnvifService::DEV:
        #ifdef DEV_S
            service = new DeviceServiceImpl( this, dynamic_cast<IOnvifDevMgmt*>(handler), soap_ );
        #endif
        break;
    case OnvifService::DEVIO:
        #ifdef DEVIO_S
            service = new DeviceIOServiceImpl( this, dynamic_cast<IOnvifDevIO*>(handler), soap_ );
        #endif
        break;
    case OnvifService::DISP:
        #ifdef DISP_S
            service = new DisplayServiceImpl( this, dynamic_cast<IOnvifDisplay*>(handler), soap_ );
        #endif
        break;
    case OnvifService::RECV:
        #ifdef RECV_S
            service = new ReceiverServiceImpl( this, dynamic_cast<IOnvifReceiver*>(handler), soap_ );
        #endif
        break;
    case OnvifService::REPLAY:
        #ifdef REPLAY_S
            service = new ReplayServiceImpl(this, soap_);
        #endif
        break;
    case OnvifService::RECORD:
        #ifdef RECORD_S
            service = new RecordingServiceImpl( this, dynamic_cast<IOnvifRecording*>(handler), soap_ );
        #endif
        break;
    case OnvifService::SEARCH:
        #ifdef SEARCH_S
            service = new SearchServiceImpl(this, soap_);
        #endif
        break;
    case OnvifService::MEDIA:
        #ifdef MEDIA_S
            service = new MediaServiceImpl(this, dynamic_cast<IOnvifMedia*>(handler), soap_);
        #endif
        break;
    case OnvifService::ANALY:
        #ifdef ANALY_S
            service = new AnalyticsServiceImpl(this, dynamic_cast<IOnvifAnalytics*>(handler), soap_);
        #endif
        break;
    case OnvifService::EVNT:
        #ifdef EVNT_S
            service = new NotificationProducer(this, soap_);
        #endif
        break;
    };

    return service;
}

int
BaseServer::Init( const OnvifHandlers& data ) {

    soap_init(soap_);
    soap_->fget = http_get;
    soap_->bind_flags |= SO_REUSEADDR;

    for( int i = 0; i < ServicesAmount; ++i ) {
        if( data.h_[i] == NULL )
            continue;
        services_[static_cast<OnvifService::Type>(i)] =  createService( static_cast<OnvifService::Type>(i),
                                       data.h_[i] );
    }

    wsdd_ = new Discoverable();

    if( 0 != RunWsDiscovery() ) {
        SIGRLOG (SIGRWARNING, "BaseServer::Run RunWsDiscovery failed");
        return -1;
    }

    NotificationProducer* prod = dynamic_cast<NotificationProducer*>(services_[OnvifService::EVNT]);

    if( !prod || !prod->init() ) {
        SIGRLOG (SIGRWARNING, "BaseServer::Run NotificationProducer init failed");
        return -1;
    }

    created_ = soap_ && services_[OnvifService::DEV];
    return created_ ? 0 : -1;
}

BaseServer::~BaseServer() {
    SIGRLOG( SIGRDEBUG2, "BaseServer::~BaseServer enter");
    if( wsdd_ ) {
        delete wsdd_;
        wsdd_ = NULL;
    }

    for( std::map<OnvifService::Type, IOnvifService*>::iterator it = services_.begin();
         it != services_.end(); ++it ) {
        deleteService(it->second);
    }

    if( soap_ ) {
        soap_destroy(soap_);
        soap_end(soap_);
        soap_free(soap_);
    }

    SIGRLOG( SIGRDEBUG2, "BaseServer::~BaseServer exit");
}

int
BaseServer::Run(bool &run_flag) {
    if( !created_ ) {
        SIGRLOG(SIGRCRITICAL, "BaseServer::Run Services were not created");
        return -1;
    }

    int iRet = soap_bind(soap_, NULL, m_webservicePort, 100);

    if ( SOAP_INVALID_SOCKET == iRet ) {
        SIGRLOG(SIGRCRITICAL, "BaseServer::Run Binding on %d port failed", m_webservicePort);
        return -1;
    }
    soap_->recv_timeout = 1;
    soap_->accept_timeout = 1;
    soap_->send_timeout = 1;

    soap_register_plugin(soap_, soap_wsa);
    soap_register_plugin(soap_, soap_wsse);

    while(run_flag) {
        if( SOAP_INVALID_SOCKET == (iRet = soap_accept(soap_)) ) {
            if (soap_
                && soap_->fault
                && soap_->fault->SOAP_ENV__Reason
                && soap_->fault->SOAP_ENV__Reason->SOAP_ENV__Text
                && !strcmp(soap_->fault->SOAP_ENV__Reason->SOAP_ENV__Text, "Timeout"))
            {
                continue;
            }
            SIGRLOG(SIGRCRITICAL, "BaseServer::Run accepting failed");
            continue;
        }

        if( SOAP_OK != soap_begin_serve(soap_) ) {
            SIGRLOG(SIGRWARNING, "BaseServer::Run serve %d failed at %s", iRet, soap_->action);
            soap_destroy(soap_);
            continue;
        }

        for( std::map<OnvifService::Type, IOnvifService*>::iterator it = services_.begin();
             it != services_.end(); ++it ) {
            if(NULL == it->second)
                continue;
            iRet = it->second->dispatch();
            if (SOAP_OK == iRet)
            {
                break;
            }
            else if (SOAP_NO_METHOD == iRet || SOAP_DEL_METHOD == iRet)
            {
                continue;
            }
            else
            {
                soap_send_fault(soap_);
                break;
            }
        }

        if( SOAP_OK != iRet )
            SIGRLOG(SIGRWARNING, "BaseServer::Run SOAP_Error= %d at %s", iRet, soap_->action);
        else
            SIGRLOG(SIGRDEBUG2, "BaseServer::Run SOAP_OK at %s", soap_->action);

        soap_closesock(soap_);
        soap_destroy(soap_);
    }

    return 0;
}

int
BaseServer::SetDeviceInfo( OnvifDevice::Type type,
                               const std::string & manufacturer,
                               const std::string & model,
                               const std::string & firmwareVersion,
                               const std::string & serialNumber,
                               const std::string & hardwareId,
                               const std::string & scopes,
                               const std::string & interface,
                               int webservicePort ) {
    return DeviceInfoStorage::SetDeviceInfo( type, manufacturer, model,
                                             firmwareVersion, serialNumber, hardwareId,
                                             scopes, interface, webservicePort );
}

int
BaseServer::RunWsDiscovery() {
    if( !wsdd_ )
		return -1;
    return wsdd_->start(true, this);
}

std::string
BaseServer::GetIp() {
    return DeviceInfoStorage::getInterfaceIp("eth0");
}

void
BaseServer::SendNotification() {
    NotificationProducer* nots = dynamic_cast<NotificationProducer*>( services_[OnvifService::EVNT] );
    if( nots )
        nots->sendNotification();
}
