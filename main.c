#include <stdbool.h>
#include <stdint.h>

#include "mongoose/mongoose.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/flash.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "httpserver_raw/httpd.h"
#include "drivers/pinout.h"
#include "initialFunctions.h"

#include "mongoose/MongooseEssentials.h"
#include "initialFunctions.h"
#include "ProgrammHandler.h"

#include "fsConfig/fs.h"
#include "mbedtls/debug.h"
#include "io.h"

// Defines for the system clock.
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)

#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

//current IP address.
uint32_t g_ui32IPAddress;

// The system clock frequency.
uint32_t g_ui32SysClock;

// mongoose Event Handler
static struct mg_mgr mgr;
// 172.20.146.158
static const char *s_address = "169.254.82.238:1883";
static const char *s_user_name = "";
static const char *s_password = "";
static const char *s_topic = "test";
static struct mg_mqtt_topic_expression s_topic_expr = {NULL, 0};
int flag = 0;

void ev_handler(struct mg_connection *nc, int ev, void *ev_data){
    struct mg_mqtt_message *msg = (struct mg_mqtt_message *) ev_data;
     (void) nc;

     if (ev != MG_EV_POLL) printf("USER HANDLER GOT EVENT %d\n", ev);

     switch (ev) {
       case MG_EV_CONNECT: {
         struct mg_send_mqtt_handshake_opts opts;
         memset(&opts, 0, sizeof(opts));
         opts.user_name = s_user_name;
         opts.password = s_password;
         opts.keep_alive = 0;
         mg_set_protocol_mqtt(nc);
         mg_send_mqtt_handshake_opt(nc, "Daniel client", opts);
         break;
       }
       case MG_EV_MQTT_CONNACK:
         if (msg->connack_ret_code != MG_EV_MQTT_CONNACK_ACCEPTED) {
           UARTprintf("Got mqtt connection error: %d\n", msg->connack_ret_code);
           exit(1);
         }
         s_topic_expr.topic = s_topic;
         UARTprintf("Subscribing to '%s'\n", s_topic);
         mg_mqtt_subscribe(nc, &s_topic_expr, 1, 42);
         break;
       case MG_EV_MQTT_PUBACK:
         UARTprintf("Message publishing acknowledged (msg_id: %d)\n", msg->message_id);
         break;
       case MG_EV_MQTT_SUBACK:
         UARTprintf("Subscription acknowledged, forwarding to '/test'\n");
         break;
       case MG_EV_MQTT_PUBLISH: {
   #if 0
           char hex[1024] = {0};
           mg_hexdump(nc->recv_mbuf.buf, msg->payload.len, hex, sizeof(hex));
           UARTprintf("Got incoming message %.*s:\n%s", (int)msg->topic.len, msg->topic.p, hex);
   #else
           // Save topic in char array
           char topic[msg->topic.len];
           int i = 0;
           for(i=0; i< msg->topic.len;i++){
               topic[i] = msg->topic.p[i];
           }
           // Save message in char array
           char message[msg->payload.len];
           i = 0;
           for(i=0; i< msg->payload.len;i++){
               message[i] = msg->payload.p[i];
           }
           UARTprintf("\nReceived Message:\n");
           UARTprintf("Topic: ");
           UARTprintf(topic);
           UARTprintf("\nMessage: ");
           UARTprintf(message);
           UARTprintf("\n");

           if(strcmp(topic,"test") == 0){
               if(strcmp(message,"on") == 0){
                   io_set_led(true);
               }else if(strcmp(message,"off") == 0){
                   io_set_led(false);
               }
           }

   #endif

         UARTprintf("\nForwarding to /test\n");
         mg_mqtt_publish(nc, "/test", 65, MG_MQTT_QOS(0), msg->payload.p,
                         msg->payload.len);
         break;
       }
       case MG_EV_CLOSE:
           UARTprintf("Connection closed\n");
           if (mg_connect(&mgr, s_address, ev_handler) == NULL) {
             UARTprintf("\nConnection Failed");
             exit(EXIT_FAILURE);
           }else{
               UARTprintf("\nConnected");
           }
           //exit(1);
     }
}
// Display an lwIP type IP Address.
void
DisplayIPAddress(uint32_t ui32Addr)
{
    char pcBuf[16];
    // Convert the IP Address into a string.
    usprintf(pcBuf, "%d.%d.%d.%d", ui32Addr & 0xff, (ui32Addr >> 8) & 0xff,
            (ui32Addr >> 16) & 0xff, (ui32Addr >> 24) & 0xff);
    // Display the string.
    UARTprintf(pcBuf);
    CFAF128128B0145T_text(10,60,pcBuf ,CFAF128128B0145T_color_white,CFAF128128B0145T_color_black, 1, 1);
}

// Required by lwIP library to support any host-related timer functions.
void
lwIPHostTimerHandler(void)
{
    uint32_t ui32NewIPAddress;
    // Get the current IP address.
    ui32NewIPAddress = lwIPLocalIPAddrGet();
    // See if the IP address has changed.
    if(ui32NewIPAddress != g_ui32IPAddress)
    {
        // See if there is an IP address assigned.
        if(ui32NewIPAddress == 0xffffffff)
        {
            // Indicate that there is no link.
            UARTprintf("Waiting for link.\n");
        }
        else if(ui32NewIPAddress == 0)
        {
            // There is no IP address, so indicate that the DHCP process is running.
            UARTprintf("Waiting for IP address.\n");
        }
        else
        {
            // Display the new IP address.
            UARTprintf("IP Address: https://");
            DisplayIPAddress(ui32NewIPAddress);
        }
        // Save the new IP address.
        g_ui32IPAddress = ui32NewIPAddress;
    }

    // If there is not an IP address.
    if((ui32NewIPAddress == 0) || (ui32NewIPAddress == 0xffffffff))
    {
        // Do nothing and keep waiting.
    }else{
        if(flag == 0){

            UARTprintf("Adresse: %s\n", s_address);

            if (mg_connect(&mgr, s_address, ev_handler) == NULL) {
              UARTprintf("\nConnection Failed");
              exit(EXIT_FAILURE);
            }else{
                UARTprintf("\nConnected");
            }
            flag = 1;
        }

        //Calls the mongoose event Handler
         mg_mgr_poll(&mgr, 1000);
    }
}


// The interrupt handler for the SysTick interrupt.
void SysTickIntHandler(void){
    // Call the lwIP timer handler.
    lwIPTimer(SYSTICKMS);
}


//Main
int
main(void)
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACArray[8];

    // Make sure the main oscillator is enabled because this is required by
    // the PHY.  The system must have a 25MHz crystal attached to the OSC
    // pins. The SYSCTL_MOSC_HIGHFREQ parameter is used when the crystal
    // frequency is 10MHz or higher.
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

    // Run from the PLL at 120 MHz.
    // Note: SYSCTL_CFG_VCO_240 is a new setting provided in TivaWare 2.2.x and
    // later to better reflect the actual VCO speed due to SYSCTL#22.
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240), 120000000);

    // Configure the device pins.
    PinoutSet(true, false);

    // Configure UART.
    UARTStdioConfig(0, 115200, g_ui32SysClock);

    // Clear the terminal and print banner.
    UARTprintf("\033[2J\033[H");
    UARTprintf("MQTT Project\n");
    UARTprintf("MQTT Subscriber");
    UARTprintf("Group 8\n\n");

    CFAF128128B0145T_init(2, g_ui32SysClock, 20000000);
    uint16_t color = CFAF128128B0145T_color_white;
    CFAF128128B0145T_text(10, 20, "MQTT Project ", color, CFAF128128B0145T_color_black, 1, 1);
    CFAF128128B0145T_text(10, 30, "  Subscriber ", color, CFAF128128B0145T_color_black, 1, 1);
    CFAF128128B0145T_text(10, 40, "IP Address:  ", color, CFAF128128B0145T_color_black, 1, 1);

    // Configure SysTick for a periodic interrupt.
    MAP_SysTickPeriodSet(g_ui32SysClock / SYSTICKHZ);
    MAP_SysTickEnable();
    MAP_SysTickIntEnable();

    //configures Timer2 and starts the timer
    ConfigureTimer1(g_ui32SysClock);

    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.  The MAC address will be stored in the non-volatile
    // USER0 and USER1 registers.
    MAP_FlashUserGet(&ui32User0, &ui32User1);

    if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
    {
        // Let the user know there is no MAC address and exit Program-m
        UARTprintf("No MAC programmed!\n");
        return 1;
    }

    // Tell the user what we are doing just now.
    UARTprintf("Waiting for IP.\n");


    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    pui8MACArray[0] = ((ui32User0 >>  0) & 0xff);
    pui8MACArray[1] = ((ui32User0 >>  8) & 0xff);
    pui8MACArray[2] = ((ui32User0 >> 16) & 0xff);
    pui8MACArray[3] = ((ui32User1 >>  0) & 0xff);
    pui8MACArray[4] = ((ui32User1 >>  8) & 0xff);
    pui8MACArray[5] = ((ui32User1 >> 16) & 0xff);

    // enable mbedTLS log in console
    mbedtls_debug_set_threshold(5);

    // Initialize the lwIP library, using DHCP.
    lwIPInit(g_ui32SysClock, pui8MACArray, 0, 0, 0, IPADDR_USE_DHCP);

    mg_mgr_init(&mgr, 0);

    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    MAP_IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
    MAP_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);
    // Loop forever
    while(1){
    }
}
