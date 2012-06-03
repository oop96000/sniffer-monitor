#include "easysniffer.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//#define DEBUG

int
initiate(easy_sniffer *es) {
	if((es->sockfd = socket(PF_PACKET,
							SOCK_RAW,
							htons(ETH_P_ALL))) < 0) {
		fprintf(stderr, "Failed to get a socket.\n%s\n",
						strerror(errno));
		return 0;
	}
	return 1;
}

/*
 * 抓指定大小的包
 * The value of *size is recommonded bigger than 54.
 */
ssize_t
capture(easy_sniffer * es, dgram *dg) {
	if (es->sockfd < 0) {
		fprintf(stderr, "The sniffer hasn\'t been initiated.\n");
		return -1;
	}
	if (NULL == dg->buff) {
		fprintf(stderr, "Your should initiate the dg buff, using func like 'malloc'.\n");
		return -1;
	}

	ssize_t recv;

	bzero(dg->buff, dg->size);
	fprintf(stdout, "Now capturing packet...\n");
	if((recv = recvfrom(es->sockfd,
						dg->buff,
						dg->size,
						0, NULL, NULL)) < 0) {
		fprintf(stderr, "Failed to capture a bag.\n");
		return 0;
	}
	#ifdef DEBUG
	printf("原始包大小：%d\n", recv);
	#endif
	return (dg->size = recv);
}

/*
 * 分析包
 */
int
anal_dg(dgram *a_dg, dgram *dg) {
	if ((NULL == dg) || (NULL == a_dg)) {
		return -1;
	}
	if (UNKWN == dg->prot) {
		a_dg->size = dg->size;
		a_dg->prot = dg->prot;
		a_dg->buff = dg->buff;
	#ifdef DEBUG
		printf("line 67:未知包.\n");
	#endif
		return 0;
	}
	int hdl;
	hdl = cut_proto_head(&(a_dg->buff), dg->buff, dg->prot);
	a_dg->size = dg->size - hdl;
	a_dg->prot = get_upper_proto(dg);
	return 1;
}

size_t
cut_proto_head(char **p_buff_lef, char *buff, PROTO proto_t) {
	size_t hdl;		// 头长
	switch (proto_t) {
	case UNKWN:
		*p_buff_lef = buff;
		return (hdl = 0);
	case ETH:
	#ifdef DEBUG
		printf("line 87:ETHer包.\n");
	#endif
		hdl = ETH_HDL;
		break;
	case ETHERTYPE_IP:
		hdl = (size_t)((buff[0] & 0xF) * 4);// printf("ip hdl:%d\n", hdl);
		break;
	case ETHERTYPE_ARP:
		*p_buff_lef = buff;
		return (hdl = 0);
	case IPPROTO_TCP:
		hdl = (size_t)(buff[12] & 0xF0);
		break;
	case IPPROTO_UDP:
		hdl = UDP_HDL;
		break;
	case IPPROTO_ICMP:
	#ifdef DEBUG
		printf("line 109:未知包.\n");
		*p_buff_lef = buff;
		return (hdl = 0);
	#endif
	default:
	#ifdef DEBUG
		printf("line 111:未知包.\n");
	#endif
		*p_buff_lef = buff;
		return (hdl = 0);
	}
	*p_buff_lef = buff + hdl;
	return hdl;
}

PROTO
get_upper_proto(dgram *dg) {
	switch (dg->prot) {
	case UNKWN:
		return UNKWN;
	case ETH:
		return eth_upper_proto(dg->buff);
	case ETHERTYPE_IP:
		return ip_upper_proto(dg->buff);
	case ETHERTYPE_ARP:
		return UNKWN;
	case IPPROTO_TCP:
	case IPPROTO_UDP:
		return tcp_or_udp_upper_proto(dg->buff);
	default:
		return UNKWN;
	}
}

PROTO
eth_upper_proto(char *buff) {
	u_int16_t eth_t;
	eth_t = (u_int16_t )buff[12];
	eth_t <<= 8;
	eth_t |= (u_int16_t)buff[13];

	switch (eth_t) {
	case ETHERTYPE_IP:
		return ETHERTYPE_IP;
#ifndef DEBUG
	case ETHERTYPE_ARP:
		return ETHERTYPE_ARP;
#endif
	/*
	 * 添加其他协议
	 */
	default:
		return UNKWN;
	}
}

PROTO
ip_upper_proto(char *buff) {
	switch (buff[9]) {		// 取ip报头的协议字段
	case IPPROTO_ICMP:
		return IPPROTO_ICMP;
	case IPPROTO_TCP:
		return IPPROTO_TCP;
	case IPPROTO_UDP:
		return IPPROTO_UDP;
	default:
		return UNKWN;
	}
}

PROTO
tcp_or_udp_upper_proto(char *buff) {
	u_int16_t port;
	port = (u_int16_t)buff[2]; // 获得tcp报头的源端口
	port <<= 8;
	port |= (u_int16_t)buff[3];

	switch (port) {
	case 0x50:	// 80端口，http
		return HTTP;
	case IPPORT_FTP:
		return FTP; // 21端口， ftp
	default:
		return UNKWN;
	}
}

int
print_mac_addr(char *buff) {
	printf("The src mac addr is: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\nThe dest mac addr is: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			buff[6]&0xff, buff[7]&0xff, buff[8]&0xff, buff[9]&0xff, buff[10]&0xff, buff[11]&0xff,
			buff[0]&0xff, buff[1]&0xff, buff[2]&0xff, buff[3]&0xff, buff[4]&0xff, buff[5]&0xff);
	return 1;
}

int
print_ip_addr(char *buff) {
	printf("The src ip addr is: %d.%d.%d.%d\nThe dest ip addr is: %d.%d.%d.%d\n",
			buff[12]&0xff,buff[13]&0xff,buff[14]&0xff,buff[15]&0xff,
			buff[16]&0xff,buff[17]&0xff,buff[18]&0xff,buff[19]&0xff);
	return 1;
}

int
print_tcp_udp_port(char *buff) {
	u_int16_t dst_port, src_port;
	dst_port = buff[2];
	dst_port <<= 8;
	dst_port |= buff[3];

	src_port = (u_int16_t)buff[0];
	src_port <<= 8;
	src_port |= (u_int16_t)buff[1];

	printf("The src port is: %d\nThe dest port is:%d\n",
			src_port, dst_port);
	return 1;
}
