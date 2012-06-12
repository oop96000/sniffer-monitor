/*
抓包模块的测设程序     Jan 23, 2010.
Author: Will(Wang Lulin).
Produce: 命令行可以指定抓包个数，如果该个数为0，则不停的抓包直到意外。如果不指定，则默认为5包。
*/
#include "easysniffer.c"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define DEFAULT_PORT 9001

int dispose(dgram *);
int output_dgram(dgram *);

int
main(int argc, char **argv) {
	int dgram_no = 5;	/* 抓多少包，默认5包 */
	size_t dgram_size = 0x400;	/* 包的大小， 默认1kb */
/*
 * 处理命令行参数
 */	
	int optc;
	while((optc = getopt(argc, argv, ":n:s:")) != -1) {
		switch (optc) {
		case 'n':
			dgram_no = (int)strtol(optarg, NULL, 10);
			break;
		case 's':
			dgram_size = (size_t)strtod(optarg, NULL);
			break;
		case ':':
			fprintf(stderr, "option '-%c' requires an arg.\n", optopt);
			return 1;
		case '?':
		default:
			fprintf(stderr, "option '-%c' no defined.\n", optopt);
			return 1;
		}
	}
	/*
	 * 创建抓捕器
	 */
	easy_sniffer es, *esp;
	esp = &es;
	if(initiate(esp) == 0) {
		return 1;
	}

	int i;
	for(i = 0; (0 == dgram_no ? 1 : i < dgram_no); i++) {
		printf("第--%d--包:\n", i+1);

		dgram dg, *dgp;		// 存储抓来的包
		/*
		 * 以下几行初始化dg
		 */
		dgp = &dg;
		dg.size = dgram_size;
		dg.prot = ETH;
		char *buff = (char *)malloc(dg.size);
		dg.buff = buff;
		bzero(dg.buff, dg.size);

		if(capture(esp, dgp) <= 0) {					/* 抓包 */
			printf("capture failed.\n");
			continue;
		}
		dispose(dgp);

		dgram a_dg, *a_dgp;		// 用来存储分析之后的包
		a_dgp = &a_dg;
		a_dg.size = 0;
		a_dg.prot = ETH;
		a_dg.buff = NULL;

		int anal_result;		
		while (UNKWN != dgp->prot) {
			anal_result = anal_dg(a_dgp, dgp);
			if( anal_result < 0) {							/* 分析 */
				fprintf(stderr, "Failed to analyze.\n");
				return 1;
			} else if (0 == anal_result) {
				fprintf(stdout, "Unkown packet, cannot be analized.\n");
			}
			dgp->size = a_dgp->size;
			dgp->prot = a_dgp->prot;
			dgp->buff = a_dgp->buff;
			if (0 == dispose(a_dgp)) {
				fprintf(stderr, "Failed to dispose.\n");
			}
		} // while
		printf("---------------------------------------\n");

		free(buff);
	} // for
	return 0;
}

int
dispose(dgram *dg) {
	int result;
	int dstport;
	switch (dg->prot) {
	case ETH:
		result = print_mac_addr(dg->buff);
		break;
	case ETHERTYPE_IP:
		result = print_ip_addr(dg->buff);
		break;
	case IPPROTO_TCP:
		printf("tcp包\n");
		result = print_tcp_udp_port(dg->buff);
		break;
	case IPPROTO_UDP:
		printf("udp包\n");
		result = print_tcp_udp_port(dg->buff);
		dstport = dg->buff[2];
		dstport <<= 8;
		dstport |= dg->buff[3];
		break;
	case IPPROTO_ICMP:
		printf("icmp包\n");
		result = 1;
		break;
	case HTTP:
		printf("http包\n");
		result = 1;
		break;
	default:
		printf("Unkown protocol.\n");
		result = 0;
		break;
	}
	if ((IPPROTO_UDP == dg->prot) && DEFAULT_PORT == dstport) {
		printf("%d端口来访!!!\n", DEFAULT_PORT);
		if(output_dgram(dg) == 0) {
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
	}
	return result;
}

int
output_dgram(dgram *dg) {
	if (IPPROTO_UDP != dg->prot) {
		return 0;
	}
	FILE *fp;
	if ((fp = fopen("bags.txt", "a")) == NULL) {
		printf("line 148: %s\n", strerror(errno));
		return 0;
	}
	time_t *tp; time(tp);
	fprintf(fp, "\n>>>>>>>>>>>>%s\t\t\t>>>>>>>>>>>>>\n", ctime(tp));
	int count;
	for (count = 0; count < UDP_HDL; count++) {
		fprintf(fp, "%.2x ", dg->buff[count]&0xff);
	}
	fprintf(fp, "---------\n");
	for (; count < dg->size; count++) {
		fprintf(fp, "%.2x ", dg->buff[count]&0xff);
	}
	fclose(fp);
	return 1;
}
