
#include <stdio.h>
#include <unistd.h>
#include "OnvifSDK.h"

void notsCatcher( const char * msgText) {
    printf("Client code got the Event message: [%s]\n", msgText);
}

int main(const int argc, const char* argv[])
{
    IOnvifClient * pClient = getOnvifClient();
    std::vector<DiscoveryMatch> devices = pClient->DiscoverDevices( OnvifDevice::NVT );
    if(devices.size() == 0) {
        printf("Disappointment. 0 devices were discovered\n");
        return -1;
    } else {
        for(int i = 0; i < devices.size(); i++)
            printf("Discovered device: %s\n", devices[i].xaddr_.c_str());
    }

    if( pClient->Init( devices[0].xaddr_.c_str() ) != 0)
        return -1;
    if( !pClient->SetNotificationCatcher( notsCatcher ) ) {
        return -1;
    }

    soap* pSoap = pClient->GetSoap();
    IOnvifDevMgmt* devMgmt = pClient->getDeviceClient();
    IOnvifMedia* medClient = pClient->getMediaClient();
    DevGetSystemDateAndTimeResponse r(pSoap);
    int iRet = devMgmt->GetDateAndTime(r);
    if(iRet != 0) {
        printf("GetDateAndTime failed\n");
        return -1;
    } else {
         int nYear, nMonth, nDay, nHour, nMin, nSec;
         r.GetUTCDateAndTime(nYear, nMonth, nDay, nHour, nMin, nSec);
         printf("DeviceService response is:\nDate is %d:%d:%d\nTime is %d:%d:%d\n",
                 nYear, nMonth, nDay, nHour, nMin, nSec);
    }

    MedGetProfilesResponse get_profiles_resp(pSoap);
    iRet = medClient->GetProfiles(get_profiles_resp);
    if (iRet != 0) {
        printf("GetProfiles failed\n");
        return -1;
    } else {
        printf("Get profiles num responce is %d\n", get_profiles_resp.GetProfilesNum());
    }

    MedGetMetadataConfigurationsResponse metadata_resp(pSoap);
    iRet = medClient->GetMetadataConfigurations(metadata_resp);
    if (iRet != 0) {
        printf("MetadataConfiguration failed\n");
        return -1;
    } else {
        printf("MetadataConfiguration success\n");
    }

    pClient->Subscribe();

    sleep(15);
    return 0;
}

