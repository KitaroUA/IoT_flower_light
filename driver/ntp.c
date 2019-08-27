//////////////////////////////////////////////////
// Simple NTP client for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt for license terms.
//////////////////////////////////////////////////



#include "driver\ntp.h"
#include "driver\IIC_DS3231.h"

// list of major public servers http://tf.nist.gov/tf-cgi/servers.cgi
//uint8 ntp_server[] = {131, 107, 13, 100}; // microsoft
uint8 ntp_server[4] = {193, 106, 144, 7}; // microsoft


static os_timer_t ntp_timeout;
static struct espconn *pCon = 0;

// convert normal decimal to binary coded decimal
static uint8 ICACHE_FLASH_ATTR decToBcd(uint8 dec) {
  return(((dec / 10) * 16) + (dec % 10));
}

// convert binary coded decimal to normal decimal
static uint8 ICACHE_FLASH_ATTR bcdToDec(uint8 bcd) {
  return(((bcd / 16) * 10) + (bcd % 16));
}



static void ICACHE_FLASH_ATTR ntp_udp_timeout(void *arg) {
	
	os_timer_disarm(&ntp_timeout);
	uart0_tx_buffer("ntp timout\r\n", 12);
	os_printf("ntp timeout");
	// clean up connection
	if (pCon) {
		espconn_delete(pCon);
		os_free(pCon->proto.udp);
		os_free(pCon);
		pCon = 0;
	}
}

static void ICACHE_FLASH_ATTR ntp_udp_recv(void *arg, char *pdata, unsigned short len) {
	struct tm *dt;
	time_t timestamp;
	ntp_t *ntp;

	os_timer_disarm(&ntp_timeout);
	// extract ntp time
	ntp = (ntp_t*)pdata;
	timestamp = ntp->trans_time[0] << 24 | ntp->trans_time[1] << 16 |ntp->trans_time[2] << 8 | ntp->trans_time[3];
	// convert to unix time
	timestamp -= 2208988800UL;
	// create tm struct
	dt = gmtime(&timestamp);

	if (dt->tm_wday == 0)
	{
		dt->tm_wday = 7; //� NTP ���� ����� �� 0 �� 6 ��������� � �����, ��������� � ����� �� 1 �� 7
	}
	// do something with it, like setting an rtc
	//ds1307_setTime(dt);
	// or just print it out

	os_printf("\r\n");
	os_printf("\r\n");
	os_printf("\r\n");
	char timestr[11];
	os_sprintf(timestr, "%02d:%02d:%02d\r\n", dt->tm_hour, dt->tm_min, dt->tm_sec);
	uart0_tx_buffer(timestr, 10);
	os_printf("\r\n");
	char cj[80];
	os_sprintf(cj, "%02d:%02d:%02d %02d.%02d.%04d, %d",  dt->tm_hour + dt->tm_isdst + mFlag.timezone, dt->tm_min, dt->tm_sec, dt->tm_mday, dt->tm_mon + 1, dt->tm_year + 1900, dt->tm_wday);
	os_printf(cj);
	os_printf("\r\n");
	DS3231_Date_to_Write[0] = decToBcd(dt->tm_sec);
	DS3231_Date_to_Write[1] = decToBcd(dt->tm_min);
	DS3231_Date_to_Write[2] = decToBcd(dt->tm_hour + dt->tm_isdst + mFlag.timezone);
	DS3231_Date_to_Write[3] = decToBcd(dt->tm_wday);
	DS3231_Date_to_Write[4] = decToBcd(dt->tm_mday);
	DS3231_Date_to_Write[5] = decToBcd(dt->tm_mon + 1);
	DS3231_Date_to_Write[6] = decToBcd(dt->tm_year - 100);
	DS3231_Set_Date();

	// clean up connection
	if (pCon) {
		espconn_delete(pCon);
		os_free(pCon->proto.udp);
		os_free(pCon);
		pCon = 0;
	}
}

void ICACHE_FLASH_ATTR ntp_get_time() {

	ntp_t ntp;
	os_printf("\r\n");
	os_printf("ntp start");
	os_printf("%d.%d.%d.%d", ntp_server[0], ntp_server[1], ntp_server[2], ntp_server[3]);

	// set up the udp "connection"
	pCon = (struct espconn*)os_zalloc(sizeof(struct espconn));
	pCon->type = ESPCONN_UDP;
	pCon->state = ESPCONN_NONE;
	pCon->proto.udp = (esp_udp*)os_zalloc(sizeof(esp_udp));
	pCon->proto.udp->local_port = espconn_port();
	pCon->proto.udp->remote_port = 123;
	os_memcpy(pCon->proto.udp->remote_ip, ntp_server, 4);

	// create a really simple ntp request packet
	os_memset(&ntp, 0, sizeof(ntp_t));
	ntp.options = 0b00100011; // leap = 0, version = 4, mode = 3 (client)

	// set timeout timer
	os_timer_disarm(&ntp_timeout);
	os_timer_setfn(&ntp_timeout, (os_timer_func_t*)ntp_udp_timeout, pCon);
	os_timer_arm(&ntp_timeout, NTP_TIMEOUT_MS, 0);

	// send the ntp request
	espconn_create(pCon);
	espconn_regist_recvcb(pCon, ntp_udp_recv);
	espconn_sent(pCon, (uint8*)&ntp, sizeof(ntp_t));
}



