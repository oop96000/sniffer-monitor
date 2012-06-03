/*
Exercise
Author: Will(Wang Lulin).
*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <net/ethernet.h>

#define ETH_HDL (size_t)14
#define UDP_HDL (size_t)8

typedef u_int16_t PROTO;

#define UNKWN	(PROTO)0xffff

#define ETH 	(PROTO)0xfffe
#define HTTP	(PROTO)0xfffd
#define FTP		(PROTO)0xfffc

typedef struct {
	int sockfd;
} easy_sniffer;

typedef struct data_gram {
	size_t		size;		// 包的大小
	PROTO		prot;		// 包的协议类型
	char		*buff;		// 包的完整数据
} dgram;

/*
 * 初始化sniffer
 */
int initiate(easy_sniffer *);

/*
 * 抓包
 */
ssize_t capture(easy_sniffer *, dgram *);

/*
 * 分析包
 */
int anal_dg(dgram *a_dg, dgram *dg);

/*
 * 去协议头
 * 第一个参数是去掉头之后的串，第二个参数是源串，第三个参数是协议类型
 */
size_t cut_proto_head(char **p_buff_lef, char *buff, PROTO proto_t);

/*
 * 获得上层协议
 * 后面的一组函数是get_upper_proto的辅助函数
 */
PROTO get_upper_proto(dgram *dg);

PROTO eth_upper_proto(char *buff);
PROTO ip_upper_proto(char *buff);
PROTO tcp_or_udp_upper_proto(char *buff);

int print_mac_addr(char *buff);
int print_ip_addr(char *buff);
int print_tcp_udp_port(char *buff);




