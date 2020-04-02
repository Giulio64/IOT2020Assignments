/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating RIOT's MQTT-SN library
 *              emCute
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


#include "shell.h"
#include "msg.h"
#include "net/emcute.h"
#include "net/ipv6/addr.h"


#define EMCUTE_PORT         (1883U)
#define EMCUTE_ID           ("gertrud")
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

static char stack[THREAD_STACKSIZE_DEFAULT];
static msg_t queue[8];

static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];
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


/*
 *Starts a thread in the thread queue
 * Author: https://riot-os.org/
 */
static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

/*
 *Detect a publication to a topic the application is registered to
 * Author: https://riot-os.org/
 */
static void on_pub(const emcute_topic_t *topic, void *data, size_t len)
{
    char *in = (char *)data;

    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");
}

static unsigned get_qos(const char *str)
{
    int qos = atoi(str);
    switch (qos) {
        case 1:     return EMCUTE_QOS_1;
        case 2:     return EMCUTE_QOS_2;
        default:    return EMCUTE_QOS_0;
    }
}

/*
 *Connect to a broker
 * Author: https://riot-os.org/
 */
static int cmd_con(int argc, char **argv)
{
    sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;

    if (argc < 2) {
        printf("usage: %s <ipv6 addr> [port] [<will topic> <will message>]\n",
                argv[0]);
        return 1;
    }

    /* parse address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, argv[1]) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (argc >= 3) {
        gw.port = atoi(argv[2]);
    }
    if (argc >= 5) {
        topic = argv[3];
        message = argv[4];
        len = strlen(message);
    }

    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", argv[1], (int)gw.port);
        return 1;
    }
    printf("Successfully connected to gateway at [%s]:%i\n",
           argv[1], (int)gw.port);

    return 0;
}

/*
 *Disconnect from a broker
 * Author: https://riot-os.org/
 */
static int cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res = emcute_discon();
    if (res == EMCUTE_NOGW) {
        puts("error: not connected to any broker");
        return 1;
    }
    else if (res != EMCUTE_OK) {
        puts("error: unable to disconnect");
        return 1;
    }
    puts("Disconnect successful");
    return 0;
}

/*
 *Publish a value using the MQTT channel
 * Author: https://riot-os.org/
 */
static int cmd_pub(int argc, char **argv)
{
    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;

    if (argc < 3) {
        printf("usage: %s <topic name> <data> [QoS level]\n", argv[0]);
        return 1;
    }

    /* parse QoS level */
    if (argc >= 4) {
        flags |= get_qos(argv[3]);
    }

    printf("pub with topic: %s and name %s and flags 0x%02x\n", argv[1], argv[2], (int)flags);

    /* step 1: get topic id */
    t.name = argv[1];
    if (emcute_reg(&t) != EMCUTE_OK) {
        puts("error: unable to obtain topic ID");
        return 1;
    }

    /* step 2: publish data */
    if (emcute_pub(&t, argv[2], strlen(argv[2]), flags) != EMCUTE_OK) {
        printf("error: unable to publish data to topic '%s [%i]'\n",
                t.name, (int)t.id);
        return 1;
    }

    printf("Published %i bytes to topic '%s [%i]'\n",
            (int)strlen(argv[2]), t.name, t.id);

    return 0;
}

/*
 *Subscribe to a topic using MQTT protocol
 * Author: https://riot-os.org/
 */
static int cmd_sub(int argc, char **argv)
{
    unsigned flags = EMCUTE_QOS_0;

    if (argc < 2) {
        printf("usage: %s <topic name> [QoS level]\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) > TOPIC_MAXLEN) {
        puts("error: topic name exceeds maximum possible size");
        return 1;
    }
    if (argc >= 3) {
        flags |= get_qos(argv[2]);
    }

    /* find empty subscription slot */
    unsigned i = 0;
    for (; (i < NUMOFSUBS) && (subscriptions[i].topic.id != 0); i++) {}
    if (i == NUMOFSUBS) {
        puts("error: no memory to store new subscriptions");
        return 1;
    }

    subscriptions[i].cb = on_pub;
    strcpy(topics[i], argv[1]);
    subscriptions[i].topic.name = topics[i];
    if (emcute_sub(&subscriptions[i], flags) != EMCUTE_OK) {
        printf("error: unable to subscribe to %s\n", argv[1]);
        return 1;
    }

    printf("Now subscribed to %s\n", argv[1]);
    return 0;
}

/*
 *Unsubscribe to a topic using MQTT protocol
 * Author: https://riot-os.org/
 */
static int cmd_unsub(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage %s <topic name>\n", argv[0]);
        return 1;
    }

    /* find subscriptions entry */
    for (unsigned i = 0; i < NUMOFSUBS; i++) {
        if (subscriptions[i].topic.name &&
            (strcmp(subscriptions[i].topic.name, argv[1]) == 0)) {
            if (emcute_unsub(&subscriptions[i]) == EMCUTE_OK) {
                memset(&subscriptions[i], 0, sizeof(emcute_sub_t));
                printf("Unsubscribed from '%s'\n", argv[1]);
            }
            else {
                printf("Unsubscription form '%s' failed\n", argv[1]);
            }
            return 0;
        }
    }

    printf("error: no subscription for topic '%s' found\n", argv[1]);
    return 1;
}

static int cmd_will(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage %s <will topic name> <will message content>\n", argv[0]);
        return 1;
    }

    if (emcute_willupd_topic(argv[1], 0) != EMCUTE_OK) {
        puts("error: unable to update the last will topic");
        return 1;
    }
    if (emcute_willupd_msg(argv[2], strlen(argv[2])) != EMCUTE_OK) {
        puts("error: unable to update the last will message");
        return 1;
    }

    puts("Successfully updated last will topic and message");
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
 * Print the values of all the sensor attached to the board
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static int readEnv(int argc,char **argv){

    (void)argc;
    (void)argv;


    printf("Temperature is: %2.6f C \n", get_Temperature());
    printf("Humidity is: %2.6f perc \n", get_Humidity());
    printf("WindDirection is: %2.6f degrees \n", get_WindDirection());
    printf("WindIntensity is: %2.6f m/s \n", get_WindIntensity());
    printf("Rain height is: %2.6f mm / h \n", get_Rain());

    return 0;
}


/*
 * Initialize the current board as weather station Charlie
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static void initWeatherStationsInformations(void){


    weatherStation charlie;
    weatherStation tango;

    charlie.name = "Charlie";

    sensor tempC = {"2a92abd7-6d09-11ea-b89f-8f444e8fb0fc",get_Temperature(),"temperatureCharlie","temperaturature"};
    sensor humC =  {"2a92abd8-6d09-11ea-b89f-8f444e8fb0fc",get_Humidity(),"humidityCharlie","humidity"};
    sensor wDirC = {"2a92abd9-6d09-11ea-b89f-8f444e8fb0fc",get_WindDirection(),"windDirectionCharlie","WindDirection"};
    sensor wIntC = {"2a92abda-6d09-11ea-b89f-8f444e8fb0fc",get_WindDirection(),"windIntensityCharlie","WindIntensity"};
    sensor rainC = {"2a92abdb-6d09-11ea-b89f-8f444e8fb0fc",get_Rain(),"rainHeightCharlie","rain"};

    charlie.sensors[0] = tempC;
    charlie.sensors[1] = humC;
    charlie.sensors[2] = wDirC;
    charlie.sensors[3] = wIntC;
    charlie.sensors[4] = rainC;


    tango.name = "Tango";

    sensor tempT = {"2a92abd2-6d09-11ea-b89f-8f444e8fb0fc",get_Temperature(),"temperatureTango","temperaturature"};
    sensor humT =  {"2a92abd3-6d09-11ea-b89f-8f444e8fb0fc",get_Humidity(),"humidityTango","humidity"};
    sensor wDirT = {"2a92abd4-6d09-11ea-b89f-8f444e8fb0fc",get_WindDirection(),"windDirectionTango","WindDirection"};
    sensor wIntT = {"2a92abd5-6d09-11ea-b89f-8f444e8fb0fc",get_WindDirection(),"windIntensityTango","WindIntensity"};
    sensor rainT = {"2a92abd6-6d09-11ea-b89f-8f444e8fb0fc",get_Rain(),"rainHeightTango","rain"};

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
        gcvt(currentSensor.value, 6, stringifyValue);

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
* Send the data on mqtt channel from the sensor
* Author: Giulio Serra serra.1904089@gmail.com
*/
static int sendPayload(int argc,char **argv){

    if(!isSensorSelected){
        printf("%s\n","You must first initialize the sensor");
        return 1;
    }

    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;


    char payload[250];
    time_t mytime = time(NULL);

    char stringifyTime[50];
    sprintf (stringifyTime, "%lu", mytime);

    char stringifyValue[50];
    gcvt(currentSensor.value, 6, stringifyValue);

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
        

    if (argc < 2) {
        printf("usage: %s <topic name>  [QoS level]\n", argv[0]);
        return 1;
    }

    /* parse QoS level */
    if (argc >= 3) {
        flags |= get_qos(argv[3]);
    }

    printf("pub with topic: %s and name %s and flags 0x%02x\n", argv[1], payload, (int)flags);

    /* step 1: get topic id */
    t.name = argv[1];
    if (emcute_reg(&t) != EMCUTE_OK) {
        puts("error: unable to obtain topic ID");

        strcpy(payload, ""); 
        strcpy(stringifyTime, "");
        strcpy(stringifyValue, "");

        return 1;
    }

    /* step 2: publish data */
    if (emcute_pub(&t, payload, strlen(payload), flags) != EMCUTE_OK) {

        strcpy(payload, ""); 
        strcpy(stringifyTime, "");
        strcpy(stringifyValue, "");

        printf("error: unable to publish data to topic '%s [%i]'\n",
                t.name, (int)t.id);
        return 1;
    }

    printf("Published %i bytes to topic '%s [%i]'\n",
            (int)strlen(payload), t.name, t.id);

    strcpy(payload, ""); 
    strcpy(stringifyTime, "");
    strcpy(stringifyValue, "");

    return 0;
}




/*------------------------------------------------------------------------------------------------------------------*/

/*
 * Generate all the commands available in the device terminal
 * Author: Giulio Serra serra.1904089@gmail.com
 */
static const shell_command_t shell_commands[] = {
    { "con", "connect to MQTT broker", cmd_con },
    { "discon", "disconnect from the current broker", cmd_discon },
    { "pub", "publish something", cmd_pub },
    { "sub", "subscribe topic", cmd_sub },
    { "unsub", "unsubscribe from topic", cmd_unsub },
    { "will", "register a last will", cmd_will },
    { "readEnv","print all the values from all the sensors on the board",readEnv},
    { "printPay", "show a payload for the current sensor on the board to upload on MQTT", buildPayload },
    { "initSensor", "init the current board as a sensor of a weather station", initSensor},
    {"sendPayload","send the data over MQTT channel",sendPayload},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MQTT-SN example application\n");
    puts("Type 'help' to get started. Have a look at the README.md for more"
         "information.");

    /* the main thread needs a msg queue to be able to run `ping6`*/
    msg_init_queue(queue, (sizeof(queue) / sizeof(msg_t)));

    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /*init of pseudo number generator*/
    srand(time(NULL)); 

    /* should be never reached */
    return 0;
}
