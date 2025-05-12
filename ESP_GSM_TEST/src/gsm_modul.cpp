#include "gsm_modul_util.h"


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif

void setupModem() {
#ifdef MODEM_RST
    // Keep reset high
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
#endif

    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);

    // Turn on the Modem power first
    digitalWrite(MODEM_POWER_ON, HIGH);

    // Pull down PWRKEY for more than 1 second according to manual requirements
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(100);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);

    // Initialize the indicator as an output
    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LED_OFF);
}

void gsm_setup(){
    SerialMon.println("Setting up modem");

    // Set GSM module baud rate
    // Set GSM module baud rate and UART pins
    SerialAT.begin(GSM_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);

    setupModem();
    delay(6000);

    SerialMon.println("Initializing modem...");
    modem.init();

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

}


//Bearing set
bool Bearing_set()
{

    modem.sendAT(GF("+HTTPTERM"));//Configuring Bearer Scenarios
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPTERM"));
        //return false;
    }


    modem.sendAT(GF("+SAPBR=0,1"));//Configuring Bearer Scenarios
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+SAPBR=0,1"));
        return false;
    }
    delay(1000);

    modem.sendAT(GF("+SAPBR=3,1,\"Contype\",\"GPRS\""));//Configuring Bearer Scenarios
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+SAPBR=3,1,\"Contype\",\"GPRS\""));
        return false;
    }

    modem.sendAT(GF("+SAPBR=1,1"));//Activate a GPRS context
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+SAPBR=1,1"));
        //return false;
    }

    modem.sendAT(GF("+SAPBR=2,1"));//Query the GPRS context
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+SAPBR=2,1"));
        //return false;
    }
    delay(2000);
    return true;
}

bool Https_get(String url)
{

    modem.sendAT(GF("+HTTPINIT"));// Initialize the HTTP service
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPINIT"));
        return false;
    }
    modem.sendAT(GF("+HTTPPARA=\"CID\",1"));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"CID\",1"));
        return false;
    }
    modem.sendAT(GF("+HTTPPARA=\"URL\",\"" + url + "\""));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"URL\",\"" + url + "\""));
        return false;
    }

    modem.sendAT(GF("+HTTPPARA=\"REDIR\",1"));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"REDIR\",1"));
        return false;
    }
    modem.sendAT(GF("+HTTPSSL=1"));//Enabling the HTTPS function
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPSSL=1"));
        return false;
    }
    modem.sendAT(GF("+HTTPACTION=0"));//Get
    if (modem.waitResponse(60000L) != 1) {
        DBG(GF("+HTTPACTION=0"));
        return false;
    }
    delay(10000);

    modem.sendAT(GF("+HTTPREAD"));//Read data from the HTTP server
    if (modem.waitResponse(60000L) != 1) {
        DBG(GF("+HTTPREAD"));
        return false;
    }
    return true;
}

bool https_close()
{

    modem.sendAT(GF("+HTTPTERM"));//close https
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPTERM"));
        return false;
    }

    modem.sendAT(GF("+SAPBR=0,1"));//close GPRS
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+SAPBR=0,1"));
        return false;
    }

    return true;
}



void perform_get_https(String url)
{
    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected()) {
        SerialMon.println("Network connected");
    }

    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) {
        SerialMon.println("GPRS connected");
    }

    SerialMon.print(F("Performing HTTPS GET request... "));

    if (Bearing_set() == false) SerialMon.println("Bearing set fail");

    if (Https_get(url) == false) {
        SerialMon.println("https get fail");
    }

    https_close();

    modem.gprsDisconnect();
    SerialMon.println(F("GPRS disconnected"));

}

String regStatusToString(int regStatus) {
    switch (regStatus) {
        case REG_NO_RESULT:
            return "No result";
        case REG_UNREGISTERED:
            return "Unregistered";
        case REG_SEARCHING:
            return "Searching";
        case REG_DENIED:
            return "Denied";
        case REG_OK_HOME:
            return "Registered home";
        case REG_OK_ROAMING:
            return "Registered roaming";
        case REG_UNKNOWN:
            return "Unknown";
        default:
            return "Unknown status";
    }
}


String getSignalQuality(int SignalQuality) {
    // Parameters
    // <rssi>
    // 0 -115 dBm or less
    // 1 -111 dBm
    // 2...30 -110... -54 dBm
    // 31 -52 dBm or greater
    // 99 not known or not detectable
    // <ber> (in percent):
    // 0...7 As RXQUAL values in the table in GSM 05.08 [20]
    // subclause 7.2.4
    // 99 Not known or not detectable



    if (SignalQuality == 0)
        return "< -115 dBm";
    if (SignalQuality > 0 && SignalQuality < 31)
        return String(map(SignalQuality, 1, 30, -111, -54)) + " dBm";
    
    if (SignalQuality == 31)
        return "> -52 dBm";

    if (SignalQuality == 99)
        return "Not known or not detectable";

    return "Unknown";
}

statusInfo getStatusInfo() {
    statusInfo statusInfo;
    statusInfo.isNetworkConnected = modem.isNetworkConnected();
    statusInfo.isGprsConnected = modem.isGprsConnected();
    statusInfo.signalQuality = getSignalQuality((int) modem.getSignalQuality());
    statusInfo.batt   = modem.getBattVoltage();
    statusInfo.regStatus = regStatusToString((int) modem.getRegistrationStatus());
    statusInfo.loc = modem.getGsmLocationRaw();
    statusInfo.operatorName = modem.getOperator();
    statusInfo.modemInfo = modem.getModemInfo();
    return statusInfo;
}