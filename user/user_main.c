/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */

/*
This is example code for the esphttpd library. It's a small-ish demo showing off 
the server, including WiFi connection management capabilities, some IO and
some pictures of cats.
*/


#include <esp8266.h>
#include "httpd.h"
#include "io.h"
#include "httpdespfs.h"
#include "cgi.h"
#include "cgiwifi.h"
#include "cgiflash.h"
#include "stdio.h"
#include "auth.h"
#include "espfs.h"
#include "captdns.h"
#include "webpages-espfs.h"
#include "cgiwebsocket.h"



#include "user_config.h"



#define HI(X) (X>>8)
#define LO(X) (X & 0xFF)




//The example can print out the heap use every 3 seconds. You can use this to catch memory leaks.
//#define SHOW_HEAP_USE

//Function that tells the authentication system what users/passwords live on the system.
//This is disabled in the default build; if you want to try it, enable the authBasic line in
//the builtInUrls below.
int ICACHE_FLASH_ATTR myPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen) {
	if (no==0) {
		os_strcpy(user, "admin");
		os_strcpy(pass, "s3cr3t");
		return 1;
//Add more users this way. Check against incrementing no for each user added.
//	} else if (no==1) {
//		os_strcpy(user, "user1");
//		os_strcpy(pass, "something");
//		return 1;
	}
	return 0;
}

static ETSTimer websockTimer;

//Broadcast the uptime in seconds every second over connected websockets
static void ICACHE_FLASH_ATTR websockTimerCb(void *arg) {
	static int ctr=0;
	char buff[128];
	ctr++;
	os_sprintf(buff, "Up for %d minutes %d seconds!\n", ctr/60, ctr%60);
	cgiWebsockBroadcast("/websocket/ws.cgi", buff, os_strlen(buff), WEBSOCK_FLAG_NONE);
}

//On reception of a message, send "You sent: " plus whatever the other side sent
void myWebsocketRecv(Websock *ws, char *data, int len, int flags) {
	int i;
	char buff[128];
	os_sprintf(buff, "You sent: ");
	for (i=0; i<len; i++) buff[i+10]=data[i];
	buff[i+10]=0;
	cgiWebsocketSend(ws, buff, os_strlen(buff), WEBSOCK_FLAG_NONE);
}

//Websocket connected. Install reception handler and send welcome message.
void ICACHE_FLASH_ATTR myWebsocketConnect(Websock *ws) {
	ws->recvCb=myWebsocketRecv;
	cgiWebsocketSend(ws, "Hi, Websocket!", 14, WEBSOCK_FLAG_NONE);
}

//On reception of a message, echo it back verbatim
void ICACHE_FLASH_ATTR myEchoWebsocketRecv(Websock *ws, char *data, int len, int flags) {
	os_printf("EchoWs: echo, len=%d\n", len);
	cgiWebsocketSend(ws, data, len, flags);
}

//Echo websocket connected. Install reception handler.
void ICACHE_FLASH_ATTR myEchoWebsocketConnect(Websock *ws) {
	os_printf("EchoWs: connect\n");
	ws->recvCb=myEchoWebsocketRecv;
}


#ifdef ESPFS_POS
CgiUploadFlashDef uploadParams={
	.type=CGIFLASH_TYPE_ESPFS,
	.fw1Pos=ESPFS_POS,
	.fw2Pos=0,
	.fwSize=ESPFS_SIZE,
};
#define INCLUDE_FLASH_FNS
#endif
#ifdef OTA_FLASH_SIZE_K
CgiUploadFlashDef uploadParams={
	.type=CGIFLASH_TYPE_FW,
	.fw1Pos=0x1000,
	.fw2Pos=((OTA_FLASH_SIZE_K*1024)/2)+0x1000,
	.fwSize=((OTA_FLASH_SIZE_K*1024)/2)-0x1000,
	.tagName=OTA_TAGNAME
};
#define INCLUDE_FLASH_FNS
#endif

/*
This is the main url->function dispatching data struct.
In short, it's a struct with various URLs plus their handlers. The handlers can
be 'standard' CGI functions you wrote, or 'special' CGIs requiring an argument.
They can also be auth-functions. An asterisk will match any url starting with
everything before the asterisks; "*" matches everything. The list will be
handled top-down, so make sure to put more specific rules above the more
general ones. Authorization things (like authBasic) act as a 'barrier' and
should be placed above the URLs they protect.
*/
HttpdBuiltInUrl builtInUrls[]={
	{"/", cgiRedirect, "/index.tpl"},
	{"/index.tpl", cgiEspFsTemplate, tpl_index},
	{"/index_3.js", cgiEspFsTemplate, tpl_index_js},
	{"/index.cgi", cgi_index, NULL},

/*
 * 	Options page
 */

	{"/options/options.tpl", cgiEspFsTemplate, tpl_options},
	{"/options/options.cgi", cgi_options, NULL},




/*
 * set_min_light page
 */
//	{"/set_min_light", cgiRedirect, "/set_min_light/set_min_light.tpl"},
//	{"/set_min_light/", cgiRedirect, "/set_min_light/set_min_light.tpl"},
	{"/set_min_light/set_min_light.tpl", cgiEspFsTemplate, tpl_set_min_light},
	{"/set_min_light/set_min_light.cgi", cgi_set_min_light, NULL},

/*
 * set_ntp page
 */
//	{"/set_ntp", cgiRedirect, "/set_ntp/set_ntp.tpl"},
//	{"/set_ntp/", cgiRedirect, "/set_ntp/set_ntp.tpl"},
	{"/set_ntp/set_ntp.tpl", cgiEspFsTemplate, tpl_set_ntp},
	{"/set_ntp/set_ntp.cgi", cgi_set_ntp, NULL},
	{"/set_ntp/apply_ntp.cgi", cgi_apply_ntp, NULL},



/*
 * set_temp_off page
 */
//	{"/set_temp_off", cgiRedirect, "/set_temp_off/set_temp_off.tpl"},
//	{"/set_temp_off/", cgiRedirect, "/set_temp_off/set_temp_off.tpl"},
	{"/set_temp_off/set_temp_off.tpl", cgiEspFsTemplate, tpl_set_temp_off},
	{"/set_temp_off/set_temp_off.cgi", cgi_set_temp_off, NULL},


/*
 * set_temp_on page
 */
	//	{"/set_temp_off", cgiRedirect, "/set_temp_off/set_temp_off.tpl"},
	//	{"/set_temp_off/", cgiRedirect, "/set_temp_off/set_temp_off.tpl"},
		{"/set_temp_on/set_temp_on.tpl", cgiEspFsTemplate, tpl_set_temp_on},
		{"/set_temp_on/set_temp_on.cgi", cgi_set_temp_on, NULL},



/*
 * Time Range selector page
 */
	//	{"/set_temp_off", cgiRedirect, "/set_temp_off/set_temp_off.tpl"},
	//	{"/set_temp_off/", cgiRedirect, "/set_temp_off/set_temp_off.tpl"},
		{"/working_time_selector/working_time_selector.tpl", cgiEspFsTemplate, tpl_working_time_selector},
		{"/working_time_selector/working_time_selector_1.cgi", cgi_working_time_selector_1, NULL},
//		{"/working_time_selector/working_time_selector_1.js", cgiEspFsTemplate, js_working_time_selector_1},


/*
 * set_mqtt page
 */
	//	{"/set_ntp", cgiRedirect, "/set_ntp/set_ntp.tpl"},
	//	{"/set_ntp/", cgiRedirect, "/set_ntp/set_ntp.tpl"},
		{"/set_mqtt/set_mqtt.tpl", cgiEspFsTemplate, tpl_set_mqtt},
		{"/set_mqtt/set_mqtt.cgi", cgi_set_mqtt, NULL},


	{"/flash/download", cgiReadFlash, NULL},
#ifdef INCLUDE_FLASH_FNS
	{"/flash/next", cgiGetFirmwareNext, &uploadParams},
	{"/flash/upload", cgiUploadFirmware, &uploadParams},
#endif
	{"/flash/reboot", cgiRebootFirmware, NULL},


/*
 * set_temp_off page
 */
//Routines to make the /wifi URL and everything beneath it work.
//Enable the line below to protect the WiFi configuration with an username/password combo.
//	{"/wifi/*", authBasic, myPassFn},
	{"/wifi", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/wifiscan.cgi", cgiWiFiScan, NULL},
	{"/wifi/wifi.tpl", cgiEspFsTemplate, tplWlan},
	{"/wifi/connect.cgi", cgiWiFiConnect, NULL},
	{"/wifi/connstatus.cgi", cgiWiFiConnStatus, NULL},
	{"/wifi/setmode.cgi", cgiWiFiSetMode, NULL},

	{"*", cgiEspFsHook, NULL}, //Catch-all cgi function for the filesystem
	{NULL, NULL, NULL}
};


#ifdef SHOW_HEAP_USE
static ETSTimer prHeapTimer;

static void ICACHE_FLASH_ATTR prHeapTimerCb(void *arg) {
	os_printf("Heap: %ld\n", (unsigned long)system_get_free_heap_size());
}
#endif






// ==============================================================
// MQTT Part
MQTT_Client mqttClient;

/*
LOCAL void ICACHE_FLASH_ATTR setup_wifi_st_mode(void)
{
	wifi_set_opmode(STATION_MODE);
	struct station_config stconfig;
	wifi_station_disconnect();
	wifi_station_dhcpc_stop();
	if(wifi_station_get_config(&stconfig))
	{
		os_memset(stconfig.ssid, 0, sizeof(stconfig.ssid));
		os_memset(stconfig.password, 0, sizeof(stconfig.password));
		os_sprintf((char*)stconfig.ssid, "%s", "Power AP");
		os_sprintf((char*)stconfig.password, "%s", "shall we dance");
		if(!wifi_station_set_config(&stconfig))
		{
			os_printf("ESP8266 not set station config!\r\n");
		}
	}
	wifi_station_connect();
	wifi_station_dhcpc_start();
	wifi_station_set_auto_connect(1);
	os_printf("ESP8266 in STA mode configured.\r\n");
}
*/





void ICACHE_FLASH_ATTR wifiConnectCb(uint8_t status)
{
	if(status == STATION_GOT_IP){
		MQTT_Connect(&mqttClient);
	} else {
		MQTT_Disconnect(&mqttClient);
	}
}
void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args)
{
	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Connected\r\n");
	MQTT_Subscribe(client, "/esp1/LED", 0);
	MQTT_Subscribe(client, "/esp1/NTP", 1);

	MQTT_Publish(client, "/esp1/Temp", "0", 1, 0, 0);
	MQTT_Publish(client, "/esp1/Hum",  "0", 1, 1, 0);
	MQTT_Publish(client, "/esp1/LED",  "0", 1, 1, 0);
	MQTT_Publish(client, "/esp1/NTP",  "0", 1, 1, 0);

}

void ICACHE_FLASH_ATTR mqttDisconnectedCb(uint32_t *args)
{
//	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Disconnected\r\n");
}

void ICACHE_FLASH_ATTR mqttPublishedCb(uint32_t *args)
{
//	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Published\r\n");
}

void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
	char *topicBuf = (char*)os_zalloc(topic_len+1),
			*dataBuf = (char*)os_zalloc(data_len+1);

//	MQTT_Client* client = (MQTT_Client*)args;

	os_memcpy(topicBuf, topic, topic_len);
	topicBuf[topic_len] = 0;

	os_memcpy(dataBuf, data, data_len);
	dataBuf[data_len] = 0;

	INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);

	char if_op[80];
	os_sprintf(if_op, "Get topic:%s with data:%s \r\n", topicBuf, dataBuf);
	INFO(topicBuf);
	if (strcmp(topicBuf, "/esp1/LED") == 0)
	{

	}

	if (strcmp(topicBuf, "/esp1/NTP") == 0)
	{

		uint8 triger = atoi (dataBuf);

		if (triger == 1)
		{
			ntp_get_time();
		}

	}


	os_free(topicBuf);
	os_free(dataBuf);



}

// END MQTT Part
// ==============================================================


/*
 * FLASH part
 */

void ICACHE_FLASH_ATTR my_flash_var_write (void)
{
	spi_flash_erase_sector((CFG_LOCATION - 1));
	spi_flash_write((CFG_LOCATION - 1) * SPI_FLASH_SEC_SIZE,(uint32 *)&mFlag, sizeof(MY_FLASH_STR));

}



void ICACHE_FLASH_ATTR my_flash_var_read  (void)
{
	 spi_flash_read((CFG_LOCATION - 1) * SPI_FLASH_SEC_SIZE,
	                   (uint32 *)&mFlag, sizeof(MY_FLASH_STR));
}
/*
 * End FLASH part
 */


static void ICACHE_FLASH_ATTR networkServerFoundCb(const char *name, ip_addr_t *ip, void *arg) {
  static esp_tcp tcp;
  os_printf("\r\n\r\n\r\n");
  struct espconn *conn=(struct espconn *)arg;
  if (ip==NULL) {
    os_printf("Nslookup failed :/ Trying again...\n");
    os_printf("lfai");
  }
  else{
  os_printf("lokk");
  char page_buffer[20];
  os_sprintf(page_buffer,"DST: %d.%d.%d.%d",
  *((uint8 *)&ip->addr), *((uint8 *)&ip->addr + 1),
  *((uint8 *)&ip->addr + 2), *((uint8 *)&ip->addr + 3));
  os_printf(page_buffer);
  }
}

uint8 one_sec_task_state =0;

static uint8 ICACHE_FLASH_ATTR bcdToDec(uint8 bcd) {
  return(((bcd / 16) * 10) + (bcd % 16));
}

void ICACHE_FLASH_ATTR one_sec_task(void *arg)
{

	// �������� ���� ���������
	// -------------------------
		uint16 cur_time = (bcdToDec(DS3231_Time[2]))*60+bcdToDec(DS3231_Time[1]);
//		os_printf("%X:%X=%d \r\n",DS3231_Time[2], DS3231_Time[1], (bcdToDec(DS3231_Time[2]))*60+bcdToDec(DS3231_Time[1]));
//		os_printf("%d %d %d\r\n",cur_time, atoi(mFlag.on_time),atoi(mFlag.off_time));
//		os_printf("%d - _%d_ - %d \r\n", atoi(mFlag.on_time), cur_time, atoi(mFlag.off_time));

		if ( (atoi(mFlag.on_time)) < (atoi(mFlag.off_time)))
		{ // light control template - off-on-off
//			INFO ("          1          ");
			if (    (
					(cur_time >= atoi(mFlag.on_time))
					&(cur_time<=atoi(mFlag.off_time))
					&(temporary_light_off_timer == 0)
					&((light_avg <= mFlag.minLight)|(mFlag.minLight ==0))
					)
					|(temporary_light_on_timer != 0)
				)
			{
				GPIO_OUTPUT_SET(RELAY_PIN, 1);
				global_light_status=1;
//				os_printf("  ON1 \r\n");
			}
			else
			{
				GPIO_OUTPUT_SET(RELAY_PIN, 0);
				global_light_status=0;
//				os_printf("  OFF1 \r\n");
			}
		}
		else
		{// light control template - on-of-on
//			INFO ("          2          ");
			if (    (
					((cur_time >= atoi(mFlag.on_time)|(cur_time < atoi(mFlag.off_time))))
					&(temporary_light_off_timer == 0)
					&((light_avg <= mFlag.minLight)|(mFlag.minLight ==0))
					)
					|(temporary_light_on_timer != 0)
				)
			{
				GPIO_OUTPUT_SET(RELAY_PIN, 1);
				global_light_status=1;
//				os_printf("  ON2 \r\n");
			}
			else
			{
				GPIO_OUTPUT_SET(RELAY_PIN, 0);
				global_light_status=0;
//				os_printf("  OFF2 \r\n");
			}

		}

		if ( (temporary_light_off_timer != 0))
		{
			temporary_light_off_timer = temporary_light_off_timer-1;
//			os_printf("                                        temp_off_timer = %d \r\n", temp_off_timer);
		}
		if ( (temporary_light_on_timer != 0))
		{
			temporary_light_on_timer = temporary_light_on_timer-1;
//			os_printf("                                        temp_off_timer = %d \r\n", temp_off_timer);
		}


	if ((one_sec_task_state == 5))
	{

	}

	if (one_minute_timer == 0)
	{
	// 	MQTT tasks
		MQTT_Client* client_publish = &mqttClient;
		if ((client_publish->connState == 15)& (mFlag.mqtt_task_enabled == 1))
		{
				char tc[80];
				//	os_sprintf (tc, "connState -> %d \r\n", client_publish->connState);
//				os_sprintf(tc,"%02d", (int)i2c_SHT3X_Temp);
				//	os_printf(tc);
				MQTT_Publish(client_publish, "/esp1/Temp", tc, strlen(tc), 0, 0);
//				os_sprintf(tc,"%02d", (int)i2c_SHT3X_Hum);
				//	os_printf(tc);
				MQTT_Publish(client_publish, "/esp1/Hum", tc, strlen(tc), 0, 0);
		}


	}








	one_sec_task_state++;
	if (one_sec_task_state >=10)
	{
		one_sec_task_state = 0;
	}

	one_minute_timer ++;
	if (one_minute_timer >=60)
	{
		one_minute_timer = 0;
	}

	one_sec_timer_xor1 ^= 0x01;
	SetTimerTask (one_sec_timer, one_sec_task, 1000, 0);
}






















/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 8;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}



void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}


void ICACHE_FLASH_ATTR my_flash_init(void)
{
	os_sprintf(mFlag.on_time, "%s", "0540");
	os_sprintf(mFlag.off_time, "%s", "1320");
	os_sprintf(mFlag.tempOff_time, "%s", "30");
	os_sprintf(mFlag.tempOn_time, "%s", "45");
	mFlag.minLight = 25000;
	os_sprintf(mFlag.hostname, "ESP_IoT_04001");
	os_sprintf(mFlag.ntp, "0.ua.pool.ntp.org");
//	os_sprintf(mFlag.ntp, "10.10.10.10");
	mFlag.timezone = 2;
	mFlag.ntp_flag=1;
	mFlag.dst_flag=1;
	mFlag.dst_active=1;

	os_strcpy(mFlag.mqtt_server,"192.168.137.1");
	 mFlag.mqtt_task_enabled = 0;

	my_flash_var_write();
}




//Main routine. Initialize stdout, the I/O, filesystem and the webserver and we're done.
void ICACHE_FLASH_ATTR user_init(void)
{

	temporary_light_off_timer = 0;
	temporary_light_on_timer = 0;
	one_minute_timer = 0;
	memset(&light_array[0], 0, sizeof(light_array));

//!!!!!!!!!!!!!!!!!!!! for test
//	light_avg = 16000;


	//	stdoutInit();
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
//	system_set_os_print(1);
//	ioInit();


//	my_flash_init();


	my_flash_var_read();
	global_light_status = 0;

	captdnsInit();

	// 0x40200000 is the base address for spi flash memory mapping, ESPFS_POS is the position
	// where image is written in flash that is defined in Makefile.
#ifdef ESPFS_POS
	espFsInit((void*)(0x40200000 + ESPFS_POS));
#else
	espFsInit((void*)(webpages_espfs_start));
#endif
	httpdInit(builtInUrls, 80);
#ifdef SHOW_HEAP_USE
	os_timer_disarm(&prHeapTimer);
	os_timer_setfn(&prHeapTimer, prHeapTimerCb, NULL);
	os_timer_arm(&prHeapTimer, 3000, 1);
#endif
	os_timer_disarm(&websockTimer);
	os_timer_setfn(&websockTimer, websockTimerCb, NULL);
	os_timer_arm(&websockTimer, 1000, 1);
	os_printf("\nReady\n");


	CFG_Load();

	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);
	//MQTT_InitConnection(&mqttClient, "192.168.11.122", 1880, 0);

	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);
	//MQTT_InitClient(&mqttClient, "client_id", "user", "pass", 120, 1);
	char cj[6] = "/lwt";
	char ci[9] = "offline";
	MQTT_InitLWT(&mqttClient, (uint8_t*)cj, (uint8_t*)ci, 0, 0);
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);

	WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);
	INFO("\r\nSystem started ...\r\n");

	i2c_init();
	LogC("Start \r\n");

	DS3231_Init();
	SetTimerTask (one_sec_timer, one_sec_task, 1000, 0);
	i2c_SHT3X_Test();

	PIN_FUNC_SELECT(RELAY_MUX, RELAY_FUNC);

}










