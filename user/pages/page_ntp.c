/*
 * ntp.c
 *
 *  Created on: 30 жовт. 2017 р.
 *      Author: Kitaro
 */

#include "pages/page_ntp.h"
#include <esp8266.h>
#include "io.h"
#include "user_config.h"


char v_ntp[20] = "";
char v_hostname[20] = "";
char v_timezone[3] = "";
static char v_ntpresult[50] = "";


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
