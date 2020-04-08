/*
 * Copyright (C) 2017 Inria
 *               2017 Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       Semtech LoRaMAC test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Jose Alamos <jose.alamos@inria.cl>
 * @author      Giulio Serra <serra.1904089@studenti.uniroma1.it>
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include "msg.h"
#include "shell.h"
#include "fmt.h"

#include "net/loramac.h"
#include "semtech_loramac.h"

semtech_loramac_t loramac;

static bool isSensorSelected = false;

/**
*Struct declaration that rappresent all the five sensors mounted on the board(weather station)
*/
typedef struct 
{
  char *ID;
  float value;
  char *sensorName;
  char *sensorType;

}sensor; 

/**
*Struct declaration thta rappresents the station
*/
typedef struct station{
    sensor sensors[5];
    char *name;
}weatherStation;

static weatherStation stations[2];
static sensor currentSensor;



/* Application key is 16 bytes long (e.g. 32 hex chars), and thus the longest
   possible size (with application session and network session keys) */
static char print_buf[LORAMAC_APPKEY_LEN * 2 + 1];

static void _loramac_usage(void)
{
    puts("Usage: loramac <get|set|join|tx|link_check"
#ifdef MODULE_PERIPH_EEPROM
         "|save|erase"
#endif
         ">");
}

static void _loramac_join_usage(void)
{
    puts("Usage: loramac join <otaa|abp>");
}

static void _loramac_tx_usage(void)
{
    puts("Usage: loramac tx <payload> [<cnf|uncnf>] [port]");
}

static void _loramac_set_usage(void)
{
    puts("Usage: loramac set <deveui|appeui|appkey|appskey|nwkskey|devaddr|"
         "class|dr|adr|public|netid|tx_power|rx2_freq|rx2_dr> <value>");
}

static void _loramac_get_usage(void)
{
    puts("Usage: loramac get <deveui|appeui|appkey|appskey|nwkskey|devaddr|"
         "class|dr|adr|public|netid|tx_power|rx2_freq|rx2_dr>");
}

static int _cmd_loramac(int argc, char **argv)
{
    if (argc < 2) {
        _loramac_usage();
        return 1;
    }

    if (strcmp(argv[1], "get") == 0) {
        if (argc < 3) {
            _loramac_get_usage();
            return 1;
        }

        if (strcmp("deveui", argv[2]) == 0) {
            uint8_t deveui[LORAMAC_DEVEUI_LEN];
            semtech_loramac_get_deveui(&loramac, deveui);
            fmt_bytes_hex(print_buf, deveui, LORAMAC_DEVEUI_LEN);
            print_buf[LORAMAC_DEVEUI_LEN * 2] = '\0';
            printf("DEVEUI: %s\n", print_buf);
        }
        else if (strcmp("appeui", argv[2]) == 0) {
            uint8_t appeui[LORAMAC_APPEUI_LEN];
            semtech_loramac_get_appeui(&loramac, appeui);
            fmt_bytes_hex(print_buf, appeui, LORAMAC_APPEUI_LEN);
            print_buf[LORAMAC_APPEUI_LEN * 2] = '\0';
            printf("APPEUI: %s\n", print_buf);
        }
        else if (strcmp("appkey", argv[2]) == 0) {
            uint8_t appkey[LORAMAC_APPKEY_LEN];
            semtech_loramac_get_appkey(&loramac, appkey);
            fmt_bytes_hex(print_buf, appkey, LORAMAC_APPKEY_LEN);
            print_buf[LORAMAC_APPKEY_LEN * 2] = '\0';
            printf("APPKEY: %s\n", print_buf);
        }
        else if (strcmp("appskey", argv[2]) == 0) {
            uint8_t appskey[LORAMAC_APPSKEY_LEN];
            semtech_loramac_get_appskey(&loramac, appskey);
            fmt_bytes_hex(print_buf, appskey, LORAMAC_APPSKEY_LEN);
            print_buf[LORAMAC_APPSKEY_LEN * 2] = '\0';
            printf("APPSKEY: %s\n", print_buf);
        }
        else if (strcmp("nwkskey", argv[2]) == 0) {
            uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];
            semtech_loramac_get_nwkskey(&loramac, nwkskey);
            fmt_bytes_hex(print_buf, nwkskey, LORAMAC_NWKSKEY_LEN);
            print_buf[LORAMAC_NWKSKEY_LEN * 2] = '\0';
            printf("NWKSKEY: %s\n", print_buf);
        }
        else if (strcmp("devaddr", argv[2]) == 0) {
            uint8_t devaddr[LORAMAC_DEVADDR_LEN];
            semtech_loramac_get_devaddr(&loramac, devaddr);
            fmt_bytes_hex(print_buf, devaddr, LORAMAC_DEVADDR_LEN);
            print_buf[LORAMAC_DEVADDR_LEN * 2] = '\0';
            printf("DEVADDR: %s\n", print_buf);
        }
        else if (strcmp("class", argv[2]) == 0) {
            printf("Device class: ");
            switch(semtech_loramac_get_class(&loramac)) {
                case LORAMAC_CLASS_A:
                    puts("A");
                    break;
                case LORAMAC_CLASS_B:
                    puts("B");
                    break;
                case LORAMAC_CLASS_C:
                    puts("C");
                    break;
                default:
                    puts("Invalid");
                    break;
            }
        }
        else if (strcmp("dr", argv[2]) == 0) {
            printf("DATARATE: %d\n",
                   semtech_loramac_get_dr(&loramac));
        }
        else if (strcmp("adr", argv[2]) == 0) {
            printf("ADR: %s\n",
                   semtech_loramac_get_adr(&loramac) ? "on" : "off");
        }
        else if (strcmp("public", argv[2]) == 0) {
            printf("Public network: %s\n",
                   semtech_loramac_get_public_network(&loramac) ? "on" : "off");
        }
        else if (strcmp("netid", argv[2]) == 0) {
            printf("NetID: %" PRIu32 "\n", semtech_loramac_get_netid(&loramac));
        }
        else if (strcmp("tx_power", argv[2]) == 0) {
            printf("TX power index: %d\n", semtech_loramac_get_tx_power(&loramac));
        }
        else if (strcmp("rx2_freq", argv[2]) == 0) {
            printf("RX2 freq: %" PRIu32 "\n", semtech_loramac_get_rx2_freq(&loramac));
        }
        else if (strcmp("rx2_dr", argv[2]) == 0) {
            printf("RX2 dr: %d\n", semtech_loramac_get_rx2_dr(&loramac));
        }
        else {
            _loramac_get_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "set") == 0) {
        if (argc < 3) {
            _loramac_set_usage();
            return 1;
        }

        if (strcmp("deveui", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_DEVEUI_LEN * 2)) {
                puts("Usage: loramac set deveui <16 hex chars>");
                return 1;
            }
            uint8_t deveui[LORAMAC_DEVEUI_LEN];
            fmt_hex_bytes(deveui, argv[3]);
            semtech_loramac_set_deveui(&loramac, deveui);
        }
        else if (strcmp("appeui", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPEUI_LEN * 2)) {
                puts("Usage: loramac set appeui <16 hex chars>");
                return 1;
            }
            uint8_t appeui[LORAMAC_APPEUI_LEN];
            fmt_hex_bytes(appeui, argv[3]);
            semtech_loramac_set_appeui(&loramac, appeui);
        }
        else if (strcmp("appkey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPKEY_LEN * 2)) {
                puts("Usage: loramac set appkey <32 hex chars>");
                return 1;
            }
            uint8_t appkey[LORAMAC_APPKEY_LEN];
            fmt_hex_bytes(appkey, argv[3]);
            semtech_loramac_set_appkey(&loramac, appkey);
        }
        else if (strcmp("appskey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPSKEY_LEN * 2)) {
                puts("Usage: loramac set appskey <32 hex chars>");
                return 1;
            }
            uint8_t appskey[LORAMAC_APPSKEY_LEN];
            fmt_hex_bytes(appskey, argv[3]);
            semtech_loramac_set_appskey(&loramac, appskey);
        }
        else if (strcmp("nwkskey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_NWKSKEY_LEN * 2)) {
                puts("Usage: loramac set nwkskey <32 hex chars>");
                return 1;
            }
            uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];
            fmt_hex_bytes(nwkskey, argv[3]);
            semtech_loramac_set_nwkskey(&loramac, nwkskey);
        }
        else if (strcmp("devaddr", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_DEVADDR_LEN * 2)) {
                puts("Usage: loramac set devaddr <8 hex chars>");
                return 1;
            }
            uint8_t devaddr[LORAMAC_DEVADDR_LEN];
            fmt_hex_bytes(devaddr, argv[3]);
            semtech_loramac_set_devaddr(&loramac, devaddr);
        }
        else if (strcmp("class", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set class <A,B,C>");
                return 1;
            }
            loramac_class_t cls;
            if (strcmp(argv[3], "A") == 0) {
                cls = LORAMAC_CLASS_A;
            }
            else if (strcmp(argv[3], "B") == 0) {
                cls = LORAMAC_CLASS_B;
            }
            else if (strcmp(argv[3], "C") == 0) {
                cls = LORAMAC_CLASS_C;
            }
            else {
                puts("Usage: loramac set class <A,B,C>");
                return 1;
            }
            semtech_loramac_set_class(&loramac, cls);
        }
        else if (strcmp("dr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set dr <0..16>");
                return 1;
            }
            uint8_t dr = atoi(argv[3]);
            if (dr > LORAMAC_DR_15) {
                puts("Usage: loramac set dr <0..16>");
                return 1;
            }
            semtech_loramac_set_dr(&loramac, dr);
        }
        else if (strcmp("adr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set adr <on|off>");
                return 1;
            }
            bool adr;
            if (strcmp("on", argv[3]) == 0) {
                adr = true;
            }
            else if (strcmp("off", argv[3]) == 0) {
                adr = false;
            }
            else {
                puts("Usage: loramac set adr <on|off>");
                return 1;
            }
            semtech_loramac_set_adr(&loramac, adr);
        }
        else if (strcmp("public", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set public <on|off>");
                return 1;
            }
            bool public;
            if (strcmp("on", argv[3]) == 0) {
                public = true;
            }
            else if (strcmp("off", argv[3]) == 0) {
                public = false;
            }
            else {
                puts("Usage: loramac set public <on|off>");
                return 1;
            }
            semtech_loramac_set_public_network(&loramac, public);
        }
        else if (strcmp("netid", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set netid <integer value>");
                return 1;
            }
            semtech_loramac_set_netid(&loramac, strtoul(argv[2], NULL, 0));
        }
        else if (strcmp("tx_power", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set tx_power <0..16>");
                return 1;
            }
            uint8_t power = atoi(argv[3]);
            if (power > LORAMAC_TX_PWR_15) {
                puts("Usage: loramac set tx_power <0..16>");
                return 1;
            }
            semtech_loramac_set_tx_power(&loramac, power);
        }
        else if (strcmp("rx2_freq", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set rx2_freq <frequency>");
                return 1;
            }
            uint32_t freq = atoi(argv[3]);
            semtech_loramac_set_rx2_freq(&loramac, freq);
        }
        else if (strcmp("rx2_dr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set rx2_dr <0..16>");
                return 1;
            }
            uint8_t dr = atoi(argv[3]);
            if (dr > LORAMAC_DR_15){
                puts("Usage: loramac set rx2_dr <0..16>");
                return 1;
            }
            semtech_loramac_set_rx2_dr(&loramac, dr);
        }
        else {
            _loramac_set_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "join") == 0) {
        if (argc < 3) {
            _loramac_join_usage();
            return 1;
        }

        uint8_t join_type;
        if (strcmp(argv[2], "otaa") == 0) {
            join_type = LORAMAC_JOIN_OTAA;
        }
        else if (strcmp(argv[2], "abp") == 0) {
            join_type = LORAMAC_JOIN_ABP;
        }
        else {
            _loramac_join_usage();
            return 1;
        }

        switch (semtech_loramac_join(&loramac, join_type)) {
            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot join: dutycycle restriction");
                return 1;
            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot join: mac is busy");
                return 1;
            case SEMTECH_LORAMAC_JOIN_FAILED:
                puts("Join procedure failed!");
                return 1;
            case SEMTECH_LORAMAC_ALREADY_JOINED:
                puts("Warning: already joined!");
                return 1;
            case SEMTECH_LORAMAC_JOIN_SUCCEEDED:
                puts("Join procedure succeeded!");
                break;
            default: /* should not happen */
                break;
        }
        return 0;
    }
    else if (strcmp(argv[1], "tx") == 0) {
        if (argc < 3) {
            _loramac_tx_usage();
            return 1;
        }

        uint8_t cnf = LORAMAC_DEFAULT_TX_MODE;  /* Default: confirmable */
        uint8_t port = LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
        /* handle optional parameters */
        if (argc > 3) {
            if (strcmp(argv[3], "cnf") == 0) {
                cnf = LORAMAC_TX_CNF;
            }
            else if (strcmp(argv[3], "uncnf") == 0) {
                cnf = LORAMAC_TX_UNCNF;
            }
            else {
                _loramac_tx_usage();
                return 1;
            }

            if (argc > 4) {
                port = atoi(argv[4]);
                if (port == 0 || port >= 224) {
                    printf("error: invalid port given '%d', "
                           "port can only be between 1 and 223\n", port);
                    return 1;
                }
            }
        }

        semtech_loramac_set_tx_mode(&loramac, cnf);
        semtech_loramac_set_tx_port(&loramac, port);

        switch (semtech_loramac_send(&loramac,
                                     (uint8_t *)argv[2], strlen(argv[2]))) {
            case SEMTECH_LORAMAC_NOT_JOINED:
                puts("Cannot send: not joined");
                return 1;

            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot send: dutycycle restriction");
                return 1;

            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot send: MAC is busy");
                return 1;

            case SEMTECH_LORAMAC_TX_ERROR:
                puts("Cannot send: error");
                return 1;
        }

        /* wait for receive windows */
        switch (semtech_loramac_recv(&loramac)) {
            case SEMTECH_LORAMAC_DATA_RECEIVED:
                loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
                printf("Data received: %s, port: %d\n",
                       (char *)loramac.rx_data.payload, loramac.rx_data.port);
                break;

            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot send: dutycycle restriction");
                return 1;

            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot send: MAC is busy");
                return 1;

            case SEMTECH_LORAMAC_TX_ERROR:
                puts("Cannot send: error");
                return 1;

            case SEMTECH_LORAMAC_TX_DONE:
                puts("TX complete, no data received");
                break;
        }

        if (loramac.link_chk.available) {
            printf("Link check information:\n"
                   "  - Demodulation margin: %d\n"
                   "  - Number of gateways: %d\n",
                   loramac.link_chk.demod_margin,
                   loramac.link_chk.nb_gateways);
        }

        return 0;
    }
    else if (strcmp(argv[1], "link_check") == 0) {
        if (argc > 2) {
            _loramac_usage();
            return 1;
        }

        semtech_loramac_request_link_check(&loramac);
        puts("Link check request scheduled");
    }
#ifdef MODULE_PERIPH_EEPROM
    else if (strcmp(argv[1], "save") == 0) {
        if (argc > 2) {
            _loramac_usage();
            return 1;
        }

        semtech_loramac_save_config(&loramac);
    }
    else if (strcmp(argv[1], "erase") == 0) {
        if (argc > 2) {
            _loramac_usage();
            return 1;
        }

        semtech_loramac_erase_config();
    }
#endif
    else {
        _loramac_usage();
        return 1;
    }

    return 0;
}

/*------------------------------------------------------------------------------------------------------------------*/
// Here starts the new code


/*
 * Get the current temperaturature value detected by the device
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static float get_Temperature(void){

        int MAX_TEMP = 100;
        float coeff = ((float)rand()/(float)(RAND_MAX));
        return MAX_TEMP * coeff;
}

/*
 * Get the current humidity value detected by the device
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static float get_Humidity(void){

    int MAX_HUMIDITY = 100;
        float coeff = ((float)rand()/(float)(RAND_MAX));

        return MAX_HUMIDITY * coeff;
   
}

/*
 * Get the current rain height value detected by the device
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static float get_Rain(void){

    int MAX_RAIN_HEIGHT = 50;
    float coeff = ((float)rand()/(float)(RAND_MAX));

    return MAX_RAIN_HEIGHT * coeff;
}

/*
 * Get the current wind intensity value detected by the device
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static float get_WindIntensity(void){
 
    int MAX_WIND_INTENSITY = 100;
    float coeff = ((float)rand()/(float)(RAND_MAX));

    return MAX_WIND_INTENSITY * coeff;
}

/*
 * Get the current wind direction value detected by the device
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static float get_WindDirection(void){

    int MAX_WIND_DIRECTION = 360;
    float coeff = ((float)rand()/(float)(RAND_MAX));

    return MAX_WIND_DIRECTION * coeff;
}


/*
 * Initialize the current board as weather station Charlie
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static void initWeatherStationsInformations(void){


    weatherStation charlie;
    weatherStation tango;

    charlie.name = "Charlie";

    sensor tempC = {"2c107530-743b-11ea-9072-737364a53ef5",get_Temperature(),"temperatureCharlie","temperaturature"};
    sensor humC =  {"2c107531-743b-11ea-9072-737364a53ef5",get_Humidity(),"humidityCharlie","humidity"};
    sensor wDirC = {"2c107532-743b-11ea-9072-737364a53ef5",get_WindDirection(),"windDirectionCharlie","WindDirection"};
    sensor wIntC = {"2c107533-743b-11ea-9072-737364a53ef5",get_WindDirection(),"windIntensityCharlie","WindIntensity"};
    sensor rainC = {"2c107534-743b-11ea-9072-737364a53ef5",get_Rain(),"rainHeightCharlie","rain"};

    charlie.sensors[0] = tempC;
    charlie.sensors[1] = humC;
    charlie.sensors[2] = wDirC;
    charlie.sensors[3] = wIntC;
    charlie.sensors[4] = rainC;


    tango.name = "Tango";

    sensor tempT = {"2c107535-743b-11ea-9072-737364a53ef5",get_Temperature(),"temperatureTango","temperaturature"};
    sensor humT =  {"2c107536-743b-11ea-9072-737364a53ef5",get_Humidity(),"humidityTango","humidity"};
    sensor wDirT = {"2c107537-743b-11ea-9072-737364a53ef5",get_WindDirection(),"windDirectionTango","WindDirection"};
    sensor wIntT = {"2c107538-743b-11ea-9072-737364a53ef5",get_WindDirection(),"windIntensityTango","WindIntensity"};
    sensor rainT = {"2c107539-743b-11ea-9072-737364a53ef5",get_Rain(),"rainHeightTango","rain"};

    tango.sensors[0] = tempT;
    tango.sensors[1] = humT;
    tango.sensors[2] = wDirT;
    tango.sensors[3] = wIntT;
    tango.sensors[4] = rainT;

    stations[0] = charlie;
    stations[1] = tango;

     /* now print the whole condiguration */

    for(int i=0; i<2; i++){

        printf("Weather station: %s\n",stations[i].name);
        printf("%s\n","");

        for(int j=0; j<5; j++){

            printf("%s",stations[i].sensors[j].ID);
            printf("%s"," ");
            printf("%s",stations[i].sensors[j].sensorName);
            printf("%s"," ");
            printf("%s",stations[i].sensors[j].sensorType);
            printf("%s"," ");
            printf("%.6f",stations[i].sensors[j].value);
            printf("%s\n","");
            printf("%s\n","");
        }
    }
   
}


/**
* Generate a random string
* Author: Giulio Serra serra.1904089@gmail.com
*/
char *randstring(int length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#?!";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}


/**
*Build a payload that will be used to comunicate over MQTT channel
* Author: Giulio Serra serra.1904089@gmail.com
*/
static int buildPayload(int argc,char **argv){

    (void)argc;
    (void)argv;

    if(!isSensorSelected){
        printf("%s\n", "sensor not initialized, please run the initStation command");
    }
    else{

        char payload[250];
        time_t mytime = time(NULL);

        char stringifyTime[50];
        sprintf (stringifyTime, "%lu", mytime);

        char stringifyValue[50];
        sprintf(stringifyValue , "%.3f", currentSensor.value);

        strcat(payload, "{\"timestamp\":");
        strcat(payload,stringifyTime);
        strcat(payload, "\n");

        strcat(payload, "\"sensorName\":");
        strcat(payload,currentSensor.sensorName);
        strcat(payload,"\",");
        strcat(payload, "\n");

        strcat(payload, "\"sensorType\":");
        strcat(payload,currentSensor.sensorType);
        strcat(payload,"\",");
        strcat(payload, "\n");

        strcat(payload, "\"origin\":\"physical Device\",");
        strcat(payload, "\n");

        strcat(payload, "\"sensorID\":");
        strcat(payload,currentSensor.ID);
        strcat(payload,"\",");
        strcat(payload, "\n");

        strcat(payload, "\"value\":");
        strcat(payload,stringifyValue);
        strcat(payload,"\",");
        strcat(payload, "\n");

        strcat(payload, "\"ID\":");
        strcat(payload,randstring(32));
        strcat(payload,"\"}");


        printf("%s\n",payload);

            // empty the strings once the payload is printed
        strcpy(payload, ""); 
        strcpy(stringifyTime, "");
        strcpy(stringifyValue, "");
  
    }

    return 0;
    
}

/**
* Configure the current sensor type
* Author: Giulio Serra serra.1904089@gmail.com
*/
static int initSensor(int argc,char **argv){

    (void)argc;
    (void)argv;

    if(argc < 3){
        printf("%s\n", "You should specify <StationName> and <SensorName>");
        return 0;
    }

    initWeatherStationsInformations();

    for(int i=0; i<2; i++){

        printf("Weather station: %s\n",stations[i].name);
        printf("%s\n","");

        if(strcmp(stations[i].name,argv[1]) == 0){

            printf("Weather station: %s found",argv[1]);
            printf("%s\n","");

            for(int j=0; j<5; j++){

                if(strcmp(stations[i].sensors[j].sensorName,argv[2]) == 0){

                    printf("sensor: %s found",argv[2]);
                    printf("%s\n","");
                    currentSensor = stations[i].sensors[j];

                    if(strcmp(currentSensor.sensorType,"temperature") == 0){
                        currentSensor.value = get_Temperature();
                    }else if(strcmp(currentSensor.sensorType,"rain") == 0){
                        currentSensor.value = get_Rain();
                    }else if(strcmp(currentSensor.sensorType,"humidity") == 0){
                        currentSensor.value = get_Humidity();
                    }else if(strcmp(currentSensor.sensorType,"windDirection") == 0){
                        currentSensor.value = get_WindDirection();
                    }else{
                        currentSensor.value = get_WindIntensity();
                    }

                    isSensorSelected = true;
                    return 0;


                }
            }

            printf("sensor %s not found.\n", argv[2]);
            return 1;
        }
     
    }

    printf("WeatherStation %s not found.\n", argv[1]);
    return 1;
}


/**
* Send the data from the sensor over the lora channel
* Author: Giulio Serra serra.1904089@gmail.com
*/
static int sendPayload(int argc,char **argv){

    (void)argc;
    (void)argv;

    if(!isSensorSelected){
        printf("%s\n","You must first initialize the sensor");
        return 1;
    }

    char payload[250];
    time_t mytime = time(NULL);

    char stringifyTime[50];
    sprintf (stringifyTime, "%lu", mytime);

    char stringifyValue[50];
    sprintf(stringifyValue , "%.3f", currentSensor.value);


    strcat(payload, "{\"timestamp\":");
    strcat(payload,stringifyTime);
    strcat(payload, "\n");

    strcat(payload, "\"sensorName\":");
    strcat(payload,currentSensor.sensorName);
    strcat(payload,"\",");
    strcat(payload, "\n");

    strcat(payload, "\"sensorType\":");
    strcat(payload,currentSensor.sensorType);
    strcat(payload,"\",");
    strcat(payload, "\n");

    strcat(payload, "\"origin\":\"physical Device\",");
    strcat(payload, "\n");

    strcat(payload, "\"sensorID\":");
    strcat(payload,currentSensor.ID);
    strcat(payload,"\",");
    strcat(payload, "\n");

    strcat(payload, "\"value\":");
    strcat(payload,stringifyValue);
    strcat(payload,"\",");
    strcat(payload, "\n");

    strcat(payload, "\"ID\":");
    strcat(payload,randstring(32));
    strcat(payload,"\"}");


    printf("%s\n",payload);

    // now it sends the data over the LORA

    uint8_t cnf = LORAMAC_DEFAULT_TX_MODE;  /* Default: confirmable */
    uint8_t port = LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
        

    semtech_loramac_set_tx_mode(&loramac, cnf);
    semtech_loramac_set_tx_port(&loramac, port);

    switch (semtech_loramac_send(&loramac,
                                     (uint8_t *)payload, strlen(payload))) {
        case SEMTECH_LORAMAC_NOT_JOINED:
            puts("Cannot send: not joined");
            strcpy(payload, ""); 
            strcpy(stringifyTime, "");
            strcpy(stringifyValue, "");
            return 1;

        case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
            puts("Cannot send: dutycycle restriction");
            strcpy(payload, ""); 
            strcpy(stringifyTime, "");
            strcpy(stringifyValue, "");
            return 1;

        case SEMTECH_LORAMAC_BUSY:
            puts("Cannot send: MAC is busy");
            strcpy(payload, ""); 
            strcpy(stringifyTime, "");
            strcpy(stringifyValue, "");
            return 1;

        case SEMTECH_LORAMAC_TX_ERROR:
            puts("Cannot send: error");
            strcpy(payload, ""); 
            strcpy(stringifyTime, "");
            strcpy(stringifyValue, "");
            return 1;
    }

    // empty the strings once the payload is printed

    strcpy(payload, ""); 
    strcpy(stringifyTime, "");
    strcpy(stringifyValue, "");

    return 0;
}


/*------------------------------------------------------------------------------------------------------------------*/



static const shell_command_t shell_commands[] = {
    { "loramac", "control the loramac stack", _cmd_loramac },
    { "printPay", "show a payload for the current sensor on the board to upload on MQTT", buildPayload },
    { "initSensor", "init the current board as a sensor of a weather station", initSensor},
    { "sendPayload","send the data using lora channel",sendPayload},
    { NULL, NULL, NULL }
};

int main(void)
{
    semtech_loramac_init(&loramac);

    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
