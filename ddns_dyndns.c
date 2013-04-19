
// dyndns, noip, 3322

#include "ddns.h"
#include "ddns_util.h"



char* ddns_lists[] =
{
	"members.dyndns.org",
	"members.3322.org",
	"nic.changeip.com",
	"dynupdate.no-ip.com", // no-ip ddns
	"www.pansim.in",
};

char* update_url[] =
{
	"/nic/update?hostname=%s",
	"/dyndns/update?hostname=%s",
	"/nic/update?hostname=%s",
	"/nic/update?hostname=%s", // no-ip ddns
	"/nic/update?hostname=%s",
};

#define HTTP_REQUEST_TEMPLATE "GET %s HTTP/1.1\n" \
"Host:%s\n" \
"User-Agent:juan\n" \
"Authorization:Basic %s\n\n\n"

static int  make_update_request( const char* register_url,
	const char* username, const char* password,const char provider,char *_out)
{
	char _user[256]={0,};
	char _url[256]={0,};
	char _user_base64[256]={0,};
	strcpy(_user, username);
	strcat(_user, ":");
	strcat(_user, password);
	DDNS_base64_enc(_user,_user_base64);
	sprintf(_url, update_url[provider], register_url);
	int len=sprintf(_out, HTTP_REQUEST_TEMPLATE, _url, ddns_lists[provider], _user_base64);
	DDNS_TRACE("update request: %s", _out);
	return len;
}

static void _process(uint32_t* trigger, DDNS_STAT_t* stat, const char* register_url,
	const char* username, const char* password,const char provider)
{
	int ret;
	int sock=0;
	char local_host[20];
	char ddns_host[20];
	char wan_ip[20];
	char buf[1024*2]={0,};
	int len=0;
	*stat=DDNS_GET_LOCAL_HOST;
	DDNS_TRACE("start ...");
	DDNS_TRACE("ddns:%s, host:%s, usr:%s, pwd:%s",ddns_lists[provider],register_url,username,password);
	while(*trigger)
	{
		switch(*stat)
		{
			case DDNS_GET_LOCAL_HOST:
				memset(local_host,0,sizeof(local_host));
				ret=DDNS_get_host(register_url,local_host);
				DDNS_TRACE("get local host:%s",local_host);
				if(ret==0)
				{
					*stat=DDNS_GET_WAN_IP;
				}
				else{
					*stat=DDNS_SUSPEND;
				}
				break;
			case DDNS_GET_WAN_IP:
				memset(wan_ip,0,sizeof(wan_ip));
				ret=UPNP_wan_ip(wan_ip);
				DDNS_TRACE("get wan ip:%s",wan_ip);
				if(ret==NULL)
				{
					*stat=DDNS_SUSPEND;
				}
				else{
					if(!strcmp(wan_ip,local_host))
					{
						DDNS_TRACE("current wan ip equal to local host,no need update.");
						*stat=DDNS_SUSPEND;
					}
					else{
						DDNS_TRACE("current wan ip differ to local host,goto update.");
						*stat=DDNS_GET_DDNS_HOST;
					}
				}
				break;
			case DDNS_GET_DDNS_HOST:
				memset(ddns_host,0,sizeof(ddns_host));
				ret=DDNS_get_host(ddns_lists[provider],ddns_host);
				DDNS_TRACE("get ddns host:%s",ddns_host);
				if(ret==0)
				{
					*stat=DDNS_TRYING_CONNECT;
				}
				else{
					*stat=DDNS_SUSPEND;
				}
				break;
			case DDNS_TRYING_CONNECT:
				sock = socket(AF_INET, SOCK_STREAM, 0);
				struct sockaddr_in my_addr;
				bzero(&my_addr,sizeof(my_addr));
				my_addr.sin_family = AF_INET;
				my_addr.sin_port = htons(80);
				my_addr.sin_addr.s_addr = inet_addr(ddns_host);
				struct timeval timeo = {3, 0};
				setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
				setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
				ret = connect(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr_in));
				if(ret == 0)
				{
					DDNS_TRACE("connect to ddns:%s ok.",ddns_host);
					*stat=DDNS_UPDATE;
				}
				else{
					DDNS_TRACE("connect to ddns:%s failed.",ddns_host);
					close(sock);
					*stat=DDNS_SUSPEND;
				}
				break;
			case DDNS_UPDATE:
				len=make_update_request(register_url,username,password,provider,buf);
				ret=send(sock,buf,len,0);
				if(ret==len){
					ret=recv(sock,buf,sizeof(buf),0);
					buf[ret]=0;
					DDNS_TRACE("update response: %s",buf);
				}
				close(sock);
				*stat=DDNS_SUSPEND;
				break;
			case DDNS_SUSPEND:
				DDNS_TRACE("wait nex time");
				sleep(DDNS_UPDATE_PERIOD);
				*stat=DDNS_GET_LOCAL_HOST;
				break;
		}
	}
}


void ddns_dyndns(uint32_t* trigger, DDNS_STAT_t* stat, const char* register_url,
	const char* username, const char* password)
{
	_process(trigger,stat,register_url,username,password,DDNS_DYNDNS);
}

void ddns_3322(uint32_t* trigger, DDNS_STAT_t* stat, const char* register_url,
	const char* username, const char* password)
{
	_process(trigger,stat,register_url,username,password,DDNS_3322);
}

void ddns_changeip(uint32_t* trigger, DDNS_STAT_t* stat, const char* register_url,
	const char* username, const char* password)
{
	_process(trigger,stat,register_url,username,password,DDNS_CHANGEIP);
}

void ddns_noip(uint32_t* trigger, DDNS_STAT_t* stat, const char* register_url,
	const char* username, const char* password)
{
	_process(trigger,stat,register_url,username,password,DDNS_NOIP);
}


