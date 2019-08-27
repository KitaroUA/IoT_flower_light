/*
Some random cgi routines. Used in the LED example and the page that returns the entire
flash as a binary. Also handles the hit counter on the main page.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */


#include <esp8266.h>
#include "cgi.h"
#include "io.h"
#include "user_config.h"

char v_tempOffMsg[10] = "";
char v_hostname[20] = "";
char v_ntp[20] = "";
char v_timezone[3] = "";
static char v_ntpresult[50] = "";




/*
//cause I can't be bothered to write an ioGetLed()
//static char currLedState=0;

uint8 currLedState[1];
//Cgi that turns the LED on or off according to the 'led' param in the POST data
int ICACHE_FLASH_ATTR cgiLed(HttpdConnData *connData) {
	int len;
	char buff[1024];
	currLedState[0]=0;
	
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len=httpdFindArg(connData->post->buff, "led", buff, sizeof(buff));
	if (len!=0) {
		currLedState[0]=atoi(buff);
//		i2c_PCF8574_Write(0x4c,currLedState,1);

//		ioLed(currLedState);
	}

	httpdRedirect(connData, "/led/led.tpl");
	return HTTPD_CGI_DONE;
}



//Template code for the led page.
int ICACHE_FLASH_ATTR tplLed(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;

	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "ledstate")==0) {
		if (currLedState) {
			os_strcpy(buff, "on");
		} else {
			os_strcpy(buff, "off");
		}
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}


int ICACHE_FLASH_ATTR tplshowTime(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;
	char cj[80];
	if (os_strcmp(token, "showTime")==0)
	{
		os_sprintf(cj, "%02x:%02x:%02x %02x.%02x.20%02x",  DS3231_Time[2], DS3231_Time[1], DS3231_Time[0], DS3231_Date[0], DS3231_Date[1], DS3231_Date[2]);
		os_strcpy(buff, cj);
	}

	memset(&cj[0], 0, sizeof(cj));
	if (os_strcmp(token, "showHum")==0)
	{
		os_sprintf(cj, "%02d%c", (int)i2c_SHT3X_Hum, 0x25);
		os_strcpy(buff, cj);
	}

	memset(&cj[0], 0, sizeof(cj));
	if (os_strcmp(token, "showTemp")==0)
	{
		os_sprintf(cj, "%02dC", (int)i2c_SHT3X_Temp);
		os_strcpy(buff, cj);
	}
	memset(&cj[0], 0, sizeof(cj));



//	��� � ����� ��� �������� ��� ����� ������ ���� ���������
	os_strcpy(cj, "on_t_");
	os_strcat(cj, mFlag.on_time);
	if (os_strncmp(token,cj,6)==0)
	{
		if (os_strcmp(token,cj)==0)
		{
			os_strcpy(buff, "selected");
		}
		else
		{
			os_strcpy(buff, "");
		}
	}
	memset(&cj[0], 0, sizeof(cj));

	os_strcpy(cj, "off_t_");
	os_strcat(cj, mFlag.off_time);
	if (os_strncmp(token,cj,6)==0)
	{
		if (os_strcmp(token,cj)==0)
		{
			os_strcpy(buff, "selected");
		}
		else
		{
			os_strcpy(buff, "");
		}
	}
	memset(&cj[0], 0, sizeof(cj));


	if (os_strcmp(token, "showResult")==0)
	{
		os_strcpy(buff, "");
	}
	memset(&cj[0], 0, sizeof(cj));


	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}
*/



//Template code for the counter on the index page.
int ICACHE_FLASH_ATTR tpl_index(HttpdConnData *connData, char *token, void **arg) {
	char cj[80];
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;

	if (os_strcmp(token, "curTime")==0)
	{
		if (DS3231_Date[0] == 0)
		{	//���� ������� ���� ������� - ������ �� ��������
			os_strcpy(buff, "");
		}
		else
		{
			os_sprintf(cj, "%02x:%02x:%02x %02x.%02x.20%02x",  DS3231_Time[2], DS3231_Time[1], DS3231_Time[0], DS3231_Date[0], DS3231_Date[1], DS3231_Date[2]);
			os_strcpy(buff, cj);
		}
	}
	memset(&cj[0], 0, sizeof(cj));


	if (os_strcmp(token, "working_hr")==0)
	{
		uint16 n16 = atoi (mFlag.on_time);
		uint8 nh = n16/60;
		uint8 nm = n16 - nh*60;
		uint16 f16 = atoi (mFlag.off_time);
		uint8 fh = f16/60;
		uint8 fm = f16 - fh*60;

		if ( (atoi(mFlag.on_time)) < (atoi(mFlag.off_time)))
		{
			os_sprintf(cj,"Режим роботи: %02d:%02d - %02d:%02d",nh,nm,fh,fm);
			os_strcpy(buff, cj);
		}
		else
		{
			os_sprintf(cj,"Режим роботи: 00:00 - %02d:%02d, %02d:%02d - 23:59",fh,fm,nh,nm);
			os_strcpy(buff, cj);
		}


	}
	memset(&cj[0], 0, sizeof(cj));




	if (os_strcmp(token, "Temperature")==0) {
		if (v_tempOffMsg == "" )
		{
			os_strcpy(buff, "");
		}
		else
		{
			os_sprintf(cj, "%02d.%d", (int)i2c_SHT3X_Temp, (int)( (i2c_SHT3X_Temp-((int)i2c_SHT3X_Temp))*10.0) );
			os_strcpy(buff,cj);
		}
	}
	memset(&cj[0], 0, sizeof(cj));

	if (os_strcmp(token, "Humidity")==0) {
		if (v_tempOffMsg == "" )
		{
			os_strcpy(buff, "");
		}
		else
		{
			os_sprintf(cj, "%02d", (int)i2c_SHT3X_Hum);
			os_strcpy(buff,cj);
		}
	}
	memset(&cj[0], 0, sizeof(cj));
/*
	if (os_strcmp(token, "light_status")==0) {
		if (global_light_status == 1 )
		{
			os_strcpy(buff, "\"#00FF00\"");
		}
		else
		{
			os_strcpy(buff,"\"#FF0000\"");
		}
	}
	memset(&cj[0], 0, sizeof(cj));
*/

	if (os_strcmp(token, "checker_1_status")==0) {
		if (temporary_light_off_timer == 0 )
		{
			os_strcpy(buff, "");
		}
		else
		{
			os_strcpy(buff,"checked");
		}
	}
	memset(&cj[0], 0, sizeof(cj));


	if (os_strcmp(token, "checker_2_status")==0) {
		if (temporary_light_on_timer == 0 )
		{
			os_strcpy(buff, "");
		}
		else
		{
			os_strcpy(buff,"checked");
		}
	}
	memset(&cj[0], 0, sizeof(cj));




	if (os_strcmp(token, "checker_1_text")==0) {
		if (temporary_light_off_timer == 0 )
		{
			os_sprintf(buff,"Тимчасово вимкнути світло на: %d хв.", atoi (mFlag.tempOff_time));
//			os_strcpy(buff, "");
		}
		else
		{
			os_sprintf(buff,"Світло тимчасово вимкнено ще: %d хв. %d сек.", temporary_light_off_timer/60, temporary_light_off_timer%60);
//			os_strcpy(buff,"checked");
		}
	}
	memset(&cj[0], 0, sizeof(cj));



	if (os_strcmp(token, "checker_2_text")==0) {
		if (temporary_light_on_timer == 0 )
		{
			os_sprintf(buff,"Тимчасово увімкнути світло на: %d хв.", atoi (mFlag.tempOn_time));
//			os_strcpy(buff, "");
		}
		else
		{
			os_sprintf(buff,"Світло тимчасово увімкнено ще: %dхв. %d сек.", temporary_light_on_timer/60, temporary_light_on_timer%60);
//			os_strcpy(buff,"checked");
		}
	}
	memset(&cj[0], 0, sizeof(cj));



	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}


int ICACHE_FLASH_ATTR tpl_index_js(HttpdConnData *connData, char *token, void **arg) {
	INFO("\r\n\r\njs\r\n\r\n");
	char cj[80];
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;
	if (os_strcmp(token, "light_status")==0) {
		if (global_light_status == 1 )
		{
			os_strcpy(buff, "\"#00FF00\"");
		}
		else
		{
			os_strcpy(buff,"\"#FF0000\"");
		}
	}
	memset(&cj[0], 0, sizeof(cj));

	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}



int ICACHE_FLASH_ATTR cgi_index(HttpdConnData *connData)
{
	int len;
	char buff[1024];
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}
	len=httpdFindArg(connData->post->buff, "slider1", buff, sizeof(buff));
	if (len!=0)
	{
		INFO("\r\n slider 1   ");
		INFO(buff);
		INFO("\r\n");

		if (atoi(buff) == 0)
		{
			temporary_light_off_timer=0;
		}
		if (atoi(buff) == 1)
		{
			temporary_light_off_timer=atoi (mFlag.tempOff_time)*60;;
			temporary_light_on_timer=0;
		}
//		mFlag.minLight = atoi (buff);
//		my_flash_var_write();
	}
	memset(&buff[0], 0, sizeof(buff));


	len=httpdFindArg(connData->post->buff, "slider2", buff, sizeof(buff));
	if (len!=0)
	{
		INFO("\r\n slider 2   ");
		INFO(buff);
		INFO("\r\n");

		if (atoi(buff) == 0)
		{
			temporary_light_on_timer=0;
		}
		if (atoi(buff) == 1)
		{
			temporary_light_on_timer=atoi (mFlag.tempOn_time)*60;;
			temporary_light_off_timer=0;
		}
//		mFlag.minLight = atoi (buff);
//		my_flash_var_write();
	}
	memset(&buff[0], 0, sizeof(buff));



	httpdRedirect(connData, "/index.tpl");
	return HTTPD_CGI_DONE;
}








/*

int ICACHE_FLASH_ATTR cgi_off_time(HttpdConnData *connData)
{
	os_printf("\r\n \r\n Soff\r\n");

return 1;
}

*/





/*

int ICACHE_FLASH_ATTR cgi_on_time(HttpdConnData *connData)
{
	os_printf("\r\n \r\n Son\r\n");
	int len;
	char buff[1024];
currLedState[0]=0;

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len=httpdFindArg(connData->post->buff, "on_time", buff, sizeof(buff));
	if (len!=0) {
		char cj[80];
		os_sprintf(cj,"%s",buff);
		i2c_LCD_Send_String(2, 0, cj);

//		ioLed(currLedState);
	}

	httpdRedirect(connData, "time.tpl");
	return HTTPD_CGI_DONE;
}



int ICACHE_FLASH_ATTR cgi_btn_time(HttpdConnData *connData)
{
	int len;
	char buff[1024];
	char cj[80];
	os_printf("\r\n%s\r\n",buff);
	len=httpdFindArg(connData->post->buff, "on_time", buff, sizeof(buff));
	if (len!=0)
	{
		os_strcpy(mFlag.on_time,buff);
	}
	len=httpdFindArg(connData->post->buff, "off_time", buff, sizeof(buff));
	if (len!=0)
	{
		os_strcpy(mFlag.off_time,buff);
	}



	httpdRedirect(connData, "time.tpl");
	return HTTPD_CGI_DONE;
}


*/
































/*
 * set_min_light page
 */
int ICACHE_FLASH_ATTR tpl_set_min_light(HttpdConnData *connData, char *token, void **arg)
{
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;
	char cj[80];
		if (os_strcmp(token,"v_minLight")==0)
		{
			os_sprintf(cj,"\"%d\"",mFlag.minLight);
			os_sprintf(buff, cj);
		}
		else
		{
			os_strcpy(buff, "");
		}
	memset(&cj[0], 0, sizeof(cj));

	httpdSend(connData, buff, -1);

	return HTTPD_CGI_DONE;
}
int ICACHE_FLASH_ATTR cgi_set_min_light(HttpdConnData *connData)
{
	int len;
	char buff[1024];
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}
	len=httpdFindArg(connData->post->buff, "min_light", buff, sizeof(buff));
	if (len!=0) {
		mFlag.minLight = atoi (buff);
		my_flash_var_write();
	}
	httpdRedirect(connData, "/set_min_light/set_min_light.tpl");
	return HTTPD_CGI_DONE;
}


/*
 * set_ntp page
 */
int ICACHE_FLASH_ATTR tpl_set_ntp(HttpdConnData *connData, char *token, void **arg)
{

	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;
	char cj[80];
	if (os_strcmp(token,"v_hostname")==0)
	{
//		os_sprintf(buff,"\"%s\"", mFlag.hostname);
		os_strcpy(buff,mFlag.hostname);
	}
	memset(&cj[0], 0, sizeof(cj));


	if (os_strcmp(token,"v_ntp")==0)
	{
//		os_sprintf(buff, "\"%s\"", mFlag.ntp);
		os_strcpy(buff, mFlag.ntp);
	}
	memset(&cj[0], 0, sizeof(cj));


	if (os_strcmp(token,"v_timezone")==0)
	{
		os_sprintf(cj,"%d", mFlag.timezone);
		os_sprintf(buff, cj);
	}
	memset(&cj[0], 0, sizeof(cj));

	if (os_strcmp(token,"ntp_type_1")==0)
	{
		if (mFlag.ntp_flag == 0)
		{
		os_sprintf(cj,"checked");
		}
		else
		{
		os_sprintf(cj," ");
		}
		os_sprintf(buff, cj);
	}
	memset(&cj[0], 0, sizeof(cj));


	if (os_strcmp(token,"ntp_type_2")==0)
	{
//		os_strcat(cj, mFlag.tempOff_time);
		if (mFlag.ntp_flag == 1)
		{
		os_sprintf(cj,"checked");
		}
		else
		{
		os_sprintf(cj," ");
		}
		os_sprintf(buff, cj);
	}
	memset(&cj[0], 0, sizeof(cj));



	if (os_strcmp(token,"DST_1")==0)
	{
		if (mFlag.dst_flag == 1)
		{
		os_sprintf(cj,"checked");
		}
		else
		{
		os_sprintf(cj," ");
		}
		os_sprintf(buff, cj);
	}
	memset(&cj[0], 0, sizeof(cj));


	if (os_strcmp(token,"DST_2")==0)
	{
		if (mFlag.dst_flag == 0)
		{
		os_sprintf(cj,"checked");
		}
		else
		{
		os_sprintf(cj," ");
		}
		os_sprintf(buff, cj);
	}
	memset(&cj[0], 0, sizeof(cj));

	if (os_strcmp(token,"v_ntpresult")==0)
	{
//		os_strcat(cj, mFlag.tempOff_time);
		os_sprintf(cj,v_ntpresult);
		os_sprintf(buff, cj);
	}
	os_printf(v_ntpresult, "");
	memset(&cj[0], 0, sizeof(cj));




	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR cgi_set_ntp(HttpdConnData *connData)
{
	int8 timezone = 0;
	uint8 ntp[30] = "";


	int len;
	char buff[1024];
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len=httpdFindArg(connData->post->buff, "hn", buff, sizeof(buff));
	if (len!=0)
	{
		os_strcpy(mFlag.hostname, buff);
	}
	memset(&buff[0], 0, sizeof(buff));

	len=httpdFindArg(connData->post->buff, "ntpip", buff, sizeof(buff));
	if (len!=0)
	{
		os_strcpy(mFlag.ntp, buff);
//		os_strcpy(ntp, buff);
	}
	memset(&buff[0], 0, sizeof(buff));

	len=httpdFindArg(connData->post->buff, "timezone", buff, sizeof(buff));
	if (len!=0)
	{
		mFlag.timezone = atoi(buff);
//		timezone = atoi(buff);
	}
	memset(&buff[0], 0, sizeof(buff));

	len=httpdFindArg(connData->post->buff, "DST", buff, sizeof(buff));
	if (len!=0)
	{
		if(os_strcmp(buff,"1") == 0)
		{
			mFlag.dst_flag = 1;
		}
		else
		{
			mFlag.dst_flag = 0;
		}
	}
	memset(&buff[0], 0, sizeof(buff));

	len=httpdFindArg(connData->post->buff, "ntp_type", buff, sizeof(buff));
	if (len!=0)
	{
		if(os_strcmp(buff,"1") == 0)
		{
			mFlag.ntp_flag = 1;
		}
		else
		{
			mFlag.ntp_flag = 0;
		}
	}
	memset(&buff[0], 0, sizeof(buff));
	/*
			ip_addr_t ntp_addr;
			os_printf("\r\n");
			ntp_addr.addr = ipaddr_addr(mFlag.ntp);
			os_printf("\r\n");
			os_printf(IPSTR,IP2STR(&ntp_addr));
			os_printf("\r\n");
			os_printf("%d",ip4_addr1_16(&ntp_addr));
			os_printf("\r\n");
			os_printf("%d",ip4_addr2_16(&ntp_addr));
			os_printf("\r\n");
			os_printf("%d",ip4_addr3_16(&ntp_addr));
			os_printf("\r\n");
			os_printf("%d",ip4_addr4_16(&ntp_addr));
			os_printf("\r\n");
	*/
	//�������� �������� �����

//	mFlag.timezone = timezone;
//	os_strcpy(mFlag.ntp, ntp);
	my_flash_var_write();

	//������ NTP
	httpdRedirect(connData, "/set_ntp/set_ntp.tpl");
	return HTTPD_CGI_DONE;
}



static void ICACHE_FLASH_ATTR ntpDNSfound(const char *name, ip_addr_t *ip, void *arg) {
	char ntp_ip_from_dns[16] ="";
  static esp_tcp tcp;
  os_printf("\r\n\r\n\r\n");
  struct espconn *conn=(struct espconn *)arg;
  if (ip==NULL) {
    os_printf("Nslookup failed :/ Trying again...\n");
    os_printf("lfai");
    memset(&ntp_ip_from_dns[0], 0, sizeof(ntp_ip_from_dns));
  }
  else{
  os_printf("lokk");
  char page_buffer[20];
  os_sprintf(page_buffer,"DST: %d.%d.%d.%d",
  *((uint8 *)&ip->addr), *((uint8 *)&ip->addr + 1),
  *((uint8 *)&ip->addr + 2), *((uint8 *)&ip->addr + 3));
  os_printf(page_buffer);
  os_sprintf(ntp_ip_from_dns,"%d.%d.%d.%d",
  *((uint8 *)&ip->addr), *((uint8 *)&ip->addr + 1),
  *((uint8 *)&ip->addr + 2), *((uint8 *)&ip->addr + 3));
  }
	ip_addr_t ntp_addr;
	os_printf("\r\n");
	ntp_addr.addr = ipaddr_addr(ntp_ip_from_dns);
	ntp_server[0] = ip4_addr1_16(&ntp_addr);
	ntp_server[1] = ip4_addr2_16(&ntp_addr);
	ntp_server[2] = ip4_addr3_16(&ntp_addr);
	ntp_server[3] = ip4_addr4_16(&ntp_addr);

	ntp_get_time();
}



int ICACHE_FLASH_ATTR cgi_apply_ntp(HttpdConnData *connData)
{
	int8 timezone = 0;
	uint8 ntp[30] = "";
	uint8 v_error[50] = "";

	os_printf("\r\napply_ntp\r\n");

	int len;
	char buff[1024];
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}


	if (mFlag.ntp_flag == 1)
	{// ntp addr - DNS
		os_printf("\r\ntp_flag_1\r\n");

		static struct espconn conn;
		static ip_addr_t ip;
		espconn_gethostbyname(&conn, mFlag.ntp, &ip, ntpDNSfound);
	}
	else
	{
		os_printf("\r\ntp_flag_0\r\n");
		ip_addr_t ntp_addr;
		ntp_addr.addr = ipaddr_addr(mFlag.ntp);
		ntp_server[0] = ip4_addr1_16(&ntp_addr);
		ntp_server[1] = ip4_addr2_16(&ntp_addr);
		ntp_server[2] = ip4_addr3_16(&ntp_addr);
		ntp_server[3] = ip4_addr4_16(&ntp_addr);
		ntp_get_time();
	}
	ntp_get_time();

	httpdRedirect(connData, "/set_ntp/set_ntp.tpl");
	return HTTPD_CGI_DONE;

}



/*
 * set_off_time page
 */
int ICACHE_FLASH_ATTR tpl_set_off_time(HttpdConnData *connData, char *token, void **arg)
{
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;
	char cj[80];
	os_strcpy(cj, "off_t_");
	os_strcat(cj, mFlag.off_time);
	if (os_strncmp(token,cj,6)==0)
	{
		if (os_strcmp(token,cj)==0)
		{
			os_strcpy(buff, "selected");
		}
		else
		{
			os_strcpy(buff, "");
		}
	}
	memset(&cj[0], 0, sizeof(cj));

	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR cgi_set_off_time(HttpdConnData *connData)
{
	int len;
	char buff[1024];
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}
	len=httpdFindArg(connData->post->buff, "off_time", buff, sizeof(buff));
	if (len!=0) {
		os_strcpy(mFlag.off_time,buff);
		my_flash_var_write();
	}
	httpdRedirect(connData, "/set_off_time/set_off_time.tpl");
	return HTTPD_CGI_DONE;
}


/*
 * set_on_time page
 */
int ICACHE_FLASH_ATTR tpl_set_on_time(HttpdConnData *connData, char *token, void **arg)
{
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;
	char cj[80];
	os_strcpy(cj, "on_t_");
	os_strcat(cj, mFlag.on_time);
	if (os_strncmp(token,cj,6)==0)
	{
		if (os_strcmp(token,cj)==0)
		{
			os_strcpy(buff, "selected");
		}
		else
		{
			os_strcpy(buff, "");
		}
	}
	memset(&cj[0], 0, sizeof(cj));

	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR cgi_set_on_time(HttpdConnData *connData)
{
	int len;
	char buff[1024];
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}
	len=httpdFindArg(connData->post->buff, "on_time", buff, sizeof(buff));
	if (len!=0) {
		os_strcpy(mFlag.on_time,buff);
		my_flash_var_write();
	}
	httpdRedirect(connData, "/set_on_time/set_on_time.tpl");
	return HTTPD_CGI_DONE;
}


/*
 * set_temp_off page
 */
int ICACHE_FLASH_ATTR tpl_set_temp_off(HttpdConnData *connData, char *token, void **arg)
{
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;
	char cj[80];
		if (os_strcmp(token,"v_tempOff")==0)
		{
			os_strcpy(cj, "\"");
			os_strcat(cj, mFlag.tempOff_time);
			os_strcat(cj, "\"");
			os_sprintf(buff, cj);
		}
		else
		{
			os_strcpy(buff, "");
		}
	memset(&cj[0], 0, sizeof(cj));

	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR cgi_set_temp_off(HttpdConnData *connData)
{
	int len;
	char buff[1024];
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}
	len=httpdFindArg(connData->post->buff, "temp_off", buff, sizeof(buff));
	if (len!=0) {
//		temporary_light_off_timer = atoi (mFlag.tempOff_time)*60;
		os_strcpy(mFlag.tempOff_time,buff);
		my_flash_var_write();
	}
	httpdRedirect(connData, "/set_temp_off/set_temp_off.tpl");
	return HTTPD_CGI_DONE;
}



