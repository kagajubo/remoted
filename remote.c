#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
//#include <netdb.h>
#include <string.h>
#include <pthread.h>
//#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

#include "upnp.h"
#include "ddns.h"

#include "IniFile.h"

#define CONF_FILE	"remote.conf"

int upnp_load_conf(const char *name,UPNP_PARA_t *_conf)
{
	char tmp[32];
	memset(_conf,0,sizeof(UPNP_PARA_t));
	IniFileInitialize(name);
	_conf->num=IniFileGetInteger("upnp-conf","num",0);
	if(_conf->num == 0){
		return -1;
	}
	strcpy(_conf->ip_gw,IniFileGetStr("upnp-conf","gateway","192.168.1.1"));
	strcpy(_conf->ip_me,IniFileGetStr("upnp-conf","ip","192.168.1.114"));
	printf("Request port map num %d gw:%s ip:%s\n",_conf->num,
		_conf->ip_gw,_conf->ip_me);

	int i;
	for(i=0;i<_conf->num;i++){
		char secname[16];
		sprintf(secname,"upnp%d",i+1);
		strcpy(tmp,IniFileGetStr(secname,"protocal","tcp"));
		if(strcmp(tmp,"tcp")==0){
			_conf->port_maps[i].protocal=PROTOCAL_TCP;
		}else if(strcmp(tmp,"udp")==0){
			_conf->port_maps[i].protocal=PROTOCAL_UDP;
		}
		_conf->port_maps[i].inPort=IniFileGetInteger(secname,"internal_port",80);
		_conf->port_maps[i].exPort=IniFileGetInteger(secname,"external_port",10080);
		printf("request map %d : %s %d -> %d\n",i+1,tmp,_conf->port_maps[i].inPort,
			_conf->port_maps[i].exPort);
	}
	IniFileCleanUp();
	return 0;
}

int ddns_load_conf(const char *name,DDNS_PARA_t *_conf)
{
	char tmp[32];
	memset(_conf,0,sizeof(UPNP_PARA_t));
	IniFileInitialize(name);
	strcpy(tmp,IniFileGetStr("ddns-conf","server","none"));
	if(strcmp(tmp,"none") == 0){
		return -1;
	} else if(strcmp(tmp,"changeip") == 0){
		_conf->provider = DDNS_CHANGEIP;
	}else if(strcmp(tmp,"3322") == 0){
		_conf->provider = DDNS_3322;
	}else if(strcmp(tmp,"dyndns") == 0){
		_conf->provider = DDNS_DYNDNS;
	}else if(strcmp(tmp,"noip") == 0){
		_conf->provider = DDNS_NOIP;
	}else{
		printf("no vaild provider :%s\n",tmp);
		exit(0);
	}
	strcpy(_conf->changeip.register_url,IniFileGetStr("ddns-conf","domain","test.changeip.org"));
	strcpy(_conf->changeip.username,IniFileGetStr("ddns-conf","username","test"));
	strcpy(_conf->changeip.password,IniFileGetStr("ddns-conf","password","123456"));
	printf("[DDNS Load Config Done.]\n");
	printf("\tprovider : %s\n",tmp);
	printf("\tdomain : %s\n",_conf->changeip.register_url);
	printf("\tusername : %s\n",_conf->changeip.username);
	printf("\tpassword : %s\n",_conf->changeip.password);
	IniFileCleanUp();
	return 0;
}

int main(int argc , char *argv[])
{
	UPNP_PARA_t _upnp;
	DDNS_PARA_t _ddns;
	if(argc != 2){
		printf("usage : ./app-name [conf-file]\n");
		exit(0);
	}
	printf("configure file is : %s \n",argv[1]);
	if(upnp_load_conf(argv[1],&_upnp) == 0){
		printf("start upnpd.....\n");
		UPNP_start(&_upnp);
	}
	if(ddns_load_conf(argv[1],&_ddns) == 0){
		printf("start ddnsd....\n");
		DDNS_start(_ddns);
	}
	while(1);
	return 0;
}
