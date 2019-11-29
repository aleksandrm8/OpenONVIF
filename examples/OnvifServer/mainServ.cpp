
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "OnvifSDK.h"
#include "OnvifTestServer.h"
#include "EventsEmitter.h"

const std::string scopes =
"onvif://www.onvif.org/name/VisorJet720 "
"onvif://www.onvif.org/hardware/Vip1 "
"onvif://www.onvif.org/type/NetworkVideoTransmitter "
"onvif://www.onvif.org/type/video_encoder "
"onvif://www.onvif.org/type/audio_encoder "
"onvif://www.onvif.org/type/ptz "
"onvif://www.onvif.org/location/Anywhere";

IOnvifServer * srv;
EventsEmitter evEmm;

void sig_handler(int signo)
{
    evEmm.stop();
    deleteOnvifServer(srv);
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGKILL, sig_handler);
    signal(SIGSTOP, sig_handler);
    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    srv = getOnvifServer();
    if( srv->SetDeviceInfo( OnvifDevice::NVT, "ELVEES",
                            "Model No 007", "1.0", "000.000.001",
                            "1.1", scopes,
                            argc > 1 ? argv[1] : onvif_server::OnvifTestServer::kInterfaceName, 8081 ) != 0 )
        return -1;

    if( !evEmm.run() )
        return -1;

    OnvifHandlers providedHandlers;
    onvif_server::OnvifTestServer handler(srv, {});
    memset( providedHandlers.h_, 0, sizeof(providedHandlers.h_) );
    providedHandlers.h_[OnvifService::DEV] = &handler;
    providedHandlers.h_[OnvifService::EVNT] = &handler;
    providedHandlers.h_[OnvifService::MEDIA] = &handler;


    if( srv->Init( providedHandlers ) != 0 )
        return -1;
    bool run_flag = true;
    if( srv->Run(run_flag) != 0 )
        return -1;

    return 0;
}
