#ifndef _UPNP_H_
#define _UPNP_H_

//upnp status
 //
enum{
 UPNP_STATE_INIT,
 UPNP_STATE_UPNP_DISCOVERY,
 
 UPNP_STATE_REQUEST_XML,
 UPNP_STATE_PARSE_XML,
 
 UPNP_STATE_REQUEST_WAN_IP,
 UPNP_STATE_PARSE_RESPONSE_WAN_IP,

 UPNP_SATE_REQUEST_ROUTER_STATUS,
 UPNP_STATE_PARSE_RESPONSE_ROUTER_STATUS,
 UPNP_STATE_REQUEST_ADD_PORT_MAP,
 UPNP_STATE_PARSE_RESPONSE_ADD_PORT_MAP,

 UPNP_STATE_REQUEST_QUERY_PORT,
 UPNP_STATE_PARSE_RESPONSE_QUERY_PORT,
 
 UPNP_STATE_WAIT_NEXT_TIME,
 UPNP_STATE_END
};

enum{
	PROTOCAL_TCP,
	PROTOCAL_UDP,
	PROTOCAL_TCP_UDP
};

#define EXPORT_MIN	5500
#define EXPORT_MAX	50000

#define UPNP_MAX_BUF_SIZE	(5*1024)
#define UPNP_READ_TIMEOUT	(5)
#define UPNP_UPDATE_PERIOD	(20*1)

typedef struct __upnp_port_map{
    uint16_t exPort;
    uint16_t inPort;
	uint16_t protocal;
    uint16_t isMapped;
}stPortMap;

#define MAX_PORT_MAP	(10)
typedef struct _upnp_para{
	char ip_me[20];
	char ip_gw[20];
	char ip_wan[20];
	uint16_t num;	
	stPortMap port_maps[MAX_PORT_MAP];
}UPNP_PARA_t;

//#define DEBUG_UPNP

#ifdef DEBUG_UPNP
#define UPNP_DBG(fmt...)	\
	do{printf("\033[1;33mUPNP->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#define UPNP_MSG(fmt...)	\
			do{printf("\033[1;34mUPNP->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#define UPNP_ERR(fmt...)	\
			do{printf("\033[7;35mUPNP->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#define UPNP_ASSERT(exp,fmt...)	\
	do{\
		if(exp==0){\
		printf("\033[1;33mUPNP->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");\
		assert(exp);\
		}\
	}while(0)
#else
#define UPNP_DBG(fmt...)
#define UPNP_MSG(fmt...)	\
			do{printf("\033[1;34mUPNP->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#define UPNP_ERR(fmt...)	\
			do{printf("\033[1;33mUPNP->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[7m\r\n");}while(0)
#define UPNP_ASSERT(exp,fmt...)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////FUNCTION 

extern void UPNP_start(void *para);
extern void UPNP_stop();
extern int UPNP_external_port(uint16_t in_port,uint16_t protocal /* 0 for tcp , 1 for udp */);
extern char *UPNP_wan_ip(char *ip_str);
extern int UPNP_done();
extern int UPNP_started();
#endif
