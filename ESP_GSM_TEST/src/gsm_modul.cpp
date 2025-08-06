#include "gsm_module.h"


void gsm_module::begin(){
    Serial.println("Setting up GSM module...");

    // Set GSM module baud rate
    // Set GSM module baud rate and UART pins
    SerialAT.begin(GSM_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);

    // Reset Modem
    // resetModem();
    // delay(6000);

    Serial.println("Initializing modem...");
    init = modem.init();
    if (!init) {
        Serial.println("GSM module failed to initialize");
        return;
    }
    Serial.println("GSM initialized.");    
    String modemInfo = modem.getModemInfo();
    Serial.print("GSM Info: ");
    Serial.println(modemInfo);
}


// TODO: WIRE RST PIN TO GPIO
// void resetModem() {
// #ifdef MODEM_RST
//     // Keep reset high
//     pinMode(MODEM_RST, OUTPUT);
//     digitalWrite(MODEM_RST, HIGH);
// #endif

//     // pinMode(MODEM_PWRKEY, OUTPUT);
//     // pinMode(MODEM_POWER_ON, OUTPUT);

//     // Turn on the Modem power first
//     // digitalWrite(MODEM_POWER_ON, HIGH);

//     // Pull down PWRKEY for more than 1 second according to manual requirements
//     // digitalWrite(MODEM_PWRKEY, HIGH);
//     // delay(100);
//     // digitalWrite(MODEM_PWRKEY, LOW);
//     // delay(1000);
//     // digitalWrite(MODEM_PWRKEY, HIGH);

//     // Initialize the indicator as an output
//     // pinMode(LED_GPIO, OUTPUT);
//     // digitalWrite(LED_GPIO, LED_OFF);
// }


//Bearing set
bool gsm_module::set_gprs_bearer()
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

int gsm_module::http_get_raw(String url){
    int status = -1;

    modem.sendAT(GF("+HTTPINIT"));// Initialize the HTTP service
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPINIT"));
        return status;
    }
    // Set Bearer profile ID
    modem.sendAT(GF("+HTTPPARA=\"CID\",1"));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"CID\",1"));
        return status;
    }
    modem.sendAT(GF("+HTTPPARA=\"URL\",\"" + url + "\""));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"URL\",\"" + url + "\""));
        return status;
    }

    // Accept redirects
    modem.sendAT(GF("+HTTPPARA=\"REDIR\",1"));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"REDIR\",1"));
        return status;
    }

    modem.sendAT(GF("+HTTPSSL=0"));//Disable the HTTPS function
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPSSL=0"));
        return status;
    }
    modem.sendAT(GF("+HTTPACTION=0"));//Get
    if (modem.waitResponse(60000L) != 1) {
        DBG(GF("+HTTPACTION=0"));
        return status;
    }
    delay(10000);

    String response = modem.stream.readString();
    Serial.printf("Response: %s\n", response.c_str());
    if (response.indexOf("+HTTPACTION: 0,") != -1) {
        int start = response.indexOf(",") + 1;
        int end = response.indexOf(",", start);
        String statusCode = response.substring(start, end);
        status = statusCode.toInt();
        Serial.printf("Status code: %d\n", status);
    } else {
        Serial.println("Failed to parse response");
    }

    modem.sendAT(GF("+HTTPREAD"));//Read data from the HTTP server
    if (modem.waitResponse(60000L) != 1) {
        DBG(GF("+HTTPREAD"));
        return status;
    }

    return status;
}

int gsm_module::http_post_raw(String url, String data){
    int status = -1;

    modem.sendAT(GF("+HTTPINIT"));// Initialize the HTTP service
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPINIT"));
        return status;
    }
    // Set Bearer profile ID
    modem.sendAT(GF("+HTTPPARA=\"CID\",1"));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"CID\",1"));
        return status;
    }

    modem.sendAT(GF("+HTTPPARA=\"URL\",\"" + url + "\""));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"URL\",\"" + url + "\""));
        return status;
    }

    // Content type JSON
    modem.sendAT(GF("+HTTPPARA=\"CONTENT\",\"text/plain\""));//Set HTTP session parameters
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPPARA=\"CONTENT\",\"text/plain\""));
        return status;
    }

    modem.sendAT(GF("+HTTPSSL=0"));//Disable the HTTPS function
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPSSL=0"));
        return status;
    }

    // Set length of the data to be sent
    modem.sendAT(GF("+HTTPDATA=" + String(data.length()) + ",10000"));
    delay(10);
    SerialAT.println(data);
    if (modem.waitResponse(10000L) != 1) {
        DBG(GF("+HTTPDATA=" + String(data.length()) + ",10000"));
        return status;
    }

    modem.sendAT(GF("+HTTPACTION=1"));//Post
    if (modem.waitResponse(60000L) != 1) {
        DBG(GF("+HTTPACTION=1"));
        return status;
    }

    delay(10000);
    String response = modem.stream.readString();
    Serial.printf("Response: %s\n", response.c_str());
    if (response.indexOf("+HTTPACTION: 1,") != -1) {
        int start = response.indexOf(",") + 1;
        int end = response.indexOf(",", start);
        String statusCode = response.substring(start, end);
        status = statusCode.toInt();
        Serial.printf("Status code: %d\n", status);
    } else {
        Serial.println("Failed to parse response");
    }

    return status;
}

bool gsm_module::http_close()
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


int gsm_module::get_req_http(String url){
    int status = -1;

    Serial.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        Serial.println(" fail");
        delay(10000);
        return status;
    }
    Serial.println(" success");

    if (modem.isNetworkConnected()) {
        Serial.println("Network connected");
    }

    // GPRS connection parameters are usually set after network registration
    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" fail");
        delay(10000);
        return status;
    }
    Serial.println(" success");

    if (modem.isGprsConnected()) {
        Serial.println("GPRS connected");
    }

    Serial.println(F("Performing HTTPS GET request... "));

    if (set_gprs_bearer() == false)
        Serial.println("Bearing set fail");

    status = http_get_raw(url);
    if (status != 200) {
        Serial.println("https get fail");
    }

    http_close();

    modem.gprsDisconnect();
    Serial.println(F("GPRS disconnected"));

    return status;
}


int gsm_module::post_req_http(String url, String data){
    int status = -1;
    Serial.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        Serial.println(" fail");
        delay(10000);
        return status;
    }
    Serial.println(" success");

    if (modem.isNetworkConnected()) {
        Serial.println("Network connected");
    }

    // GPRS connection parameters are usually set after network registration
    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" fail");
        delay(10000);
        return status;
    }
    Serial.println(" success");

    if (modem.isGprsConnected()) {
        Serial.println("GPRS connected");
    }

    Serial.print(F("Performing HTTPS GET request... "));

    if (set_gprs_bearer() == false) Serial.println("Bearing set fail");

    status = http_post_raw(url, data);
    if (status != 200) {
        Serial.println("https post fail");
    }

    http_close();

    modem.gprsDisconnect();
    Serial.println(F("GPRS disconnected"));

    return status;
}

String gsm_module::getRegStatus(int regStatus) {
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


String gsm_module::getSignalQuality(int SignalQuality) {
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

statusInfo gsm_module::getStatusInfo() {
    if (!init)
        return statusInfo();    
    statusInfo statusInfo;
    statusInfo.isNetworkConnected = modem.isNetworkConnected();
    statusInfo.isGprsConnected = modem.isGprsConnected();
    statusInfo.signalQuality = getSignalQuality((int)modem.getSignalQuality());
    String battVoltageString = String(modem.getBattVoltage())+" mV";
    switch (modem.getBattChargeState()) {
        case 0:
            battVoltageString += " - not Charging";
            break;
        case 1:
            battVoltageString += " - Charging";
            break;
        case 2:
            battVoltageString += " - Charging finished";
            break;
        default:
            battVoltageString += " - " + String(modem.getBattChargeState());
            break;
    }
    statusInfo.batt = battVoltageString;
    statusInfo.regStatus = getRegStatus((int) modem.getRegistrationStatus());
    statusInfo.loc = modem.getGsmLocation();
    statusInfo.operatorName = modem.getOperator();
    statusInfo.modemInfo = modem.getModemInfo();
    statusInfo.networkTime = modem.getGSMDateTime(TinyGSMDateTimeFormat::DATE_FULL);
    return statusInfo;
}