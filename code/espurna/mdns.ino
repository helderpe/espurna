/*

MDNS MODULE

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if MDNS_SUPPORT

#include <ESP8266mDNS.h>

WiFiEventHandler _mdns_wifi_onSTA;
WiFiEventHandler _mdns_wifi_onAP;

#if MQTT_SUPPORT
void mdnsFindMQTT() {
    int count = MDNS.queryService("mqtt", "tcp");
    DEBUG_MSG_P("[MQTT] MQTT brokers found: %d\n", count);
    for (int i=0; i<count; i++) {
        DEBUG_MSG_P("[MQTT] Broker at %s:%d\n", MDNS.IP(i).toString().c_str(), MDNS.port(i));
        mqttSetBrokerIfNone(MDNS.IP(i), MDNS.port(i));
    }
}
#endif

void _mdnsStart() {
    if (MDNS.begin(WiFi.getMode() == WIFI_AP ? APP_NAME : (char *) WiFi.hostname().c_str())) {
        DEBUG_MSG_P(PSTR("[MDNS] OK\n"));
    } else {
        DEBUG_MSG_P(PSTR("[MDNS] FAIL\n"));
    }
}

void mdnsSetup() {

    #if WEB_SUPPORT
        MDNS.addService("http", "tcp", getSetting("webPort", WEB_PORT).toInt());
    #endif

    #if TELNET_SUPPORT
        MDNS.addService("telnet", "tcp", TELNET_PORT);
    #endif

    // Public ESPurna related txt for OTA discovery
    MDNS.addServiceTxt("arduino", "tcp", "app_name", APP_NAME);
    MDNS.addServiceTxt("arduino", "tcp", "app_version", APP_VERSION);
    MDNS.addServiceTxt("arduino", "tcp", "target_board", DEVICE_NAME);

    _mdns_wifi_onSTA = WiFi.onStationModeGotIP([](WiFiEventStationModeGotIP ipInfo) {
        _mdnsStart();
    });
    _mdns_wifi_onAP = WiFi.onSoftAPModeStationConnected([](WiFiEventSoftAPModeStationConnected ipInfo) {
        _mdnsStart();
    });

}

#endif // MDNS_SUPPORT
