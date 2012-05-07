#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h> 
#include <errno.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <signal.h> 
#include <netinet/ip.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <netinet/if_ether.h> 
#include <netinet/ip_icmp.h> 
#include <time.h> 
#include <netinet/igmp.h> 
#define __FAVOR_BSD 
#include <netinet/tcp.h> 
#include <netdb.h> 
#include <netinet/udp.h> 
#define PACKET_SIZE 4096 
#define ETH_HW_ADDR_LEN 6 
#define IP_ADDR_LEN 4 
#define LOWCOUNTER 10 
#define MAX(a,b) ((a)>(b)?(a):(b)) 
struct arp_packet 
　　　　{ 
　　　　u_char targ_hw_addr[ETH_HW_ADDR_LEN]; 
　　　　u_char src_hw_addr[ETH_HW_ADDR_LEN]; 
　　　　u_short frame_type; 
　　　　u_short hw_type; 
　　　　u_short prot_type; 
　　　　u_char hw_addr_size; 
　　　　u_char prot_addr_size; 
　　　　u_short op; 
　　　　u_char sndr_hw_addr[ETH_HW_ADDR_LEN]; 
　　　　u_char sndr_ip_addr[IP_ADDR_LEN]; 
　　　　u_char rcpt_hw_addr[ETH_HW_ADDR_LEN]; 
　　　　u_char rcpt_ip_addr[IP_ADDR_LEN]; 
　　　　u_char padding[18]; 
　　　　} ; 
void leave(); 
char * hwaddr (unsigned char　*,char *); 
void writearpbuf(FILE *,char *,int);//亟匯倖arpbuf欺fd嶄 
void writetcpbuf(FILE *,char *,int); 
void writeudpbuf(FILE *,char *,int); 
void writeigmpbuf(FILE *,char *,int); 
void writeicmpbuf(FILE *,char *,int); 
int fd_recv = -1 ; 
time_t one,two,temptime; 
char arpbufone[PACKET_SIZE]; 
char tcpbufone[PACKET_SIZE]; 
char udpbufone[PACKET_SIZE]; 
char igmpbufone[PACKET_SIZE]; 
char icmpbufone[PACKET_SIZE]; 
int arpcounter=0,tcpcounter=0,igmpcounter=0,icmpcounter=0,udpcounter=0; 
char writebuf[PACKET_SIZE]; 
FILE * waringfd; 
unsigned int total=0,totalarp=0,disarp=0,totaltcp=0,distcp=0, 
totaludp=0,disudp=0,totalicmp=0,disicmp=0,totaligmp=0,disigmp=0; 
struct ifreq ifr,ifr_old; 
main(int argc, char *argv[]) 
{ 
char device[] = "eth0"; // ethernet device name 
char protocol[16]; 
u_char buf_recv[PACKET_SIZE]; // buffer for receive 
int ihl; 
int ethprotol; 
int ipfragoff,df,mf; 
int protol; 
char buf1[20],buf2[20],buf3[20],buf4[20]; 
char strptr[255]; 
char *srcaddr,*dstaddr; 
struct ethhdr *eth ; 
struct arp_packet *arphead; 
struct udphdr *udp; 
struct icmp *icmphdr; 
struct iphdr * ip; 
struct tcphdr * tcp1; 
char *flagsmes; 
int flags; 
struct igmp * igmp1; 
struct in_addr in1; 
struct in_addr in2; 
int from_len, datalen; 
struct sockaddr from; 
int arpenable=0; 
int tcpenable=0; 
int icmpenable=0; 
int igmpenable=0; 
int loenable=0; 
int udpenable=0; 
char *tcpflag=NULL; 
int c; 
char *dstip=NULL; 
char *srcip=NULL; 
extern char *optarg; 
extern int optind; 
const char 
message[]= 
"-a 
display arp packet 
-t 
display tcp packet 
-f　　　one of RST FIN SYN PUSH ACK URG 
display only tcpflag tcp packet 
-u 
display udp packet 
-i 
display icmp packet 
-g 
display igmp packet 
-A 
display all packet 
-l 
also display lo layer packet 
-s srcip 
display srcip packet 
-d dstip 
display dstip packet 
Welcome to use myxray which writed by sztcww Good Luck"; 
if (argc==1) { 
p r i n tf("%s\n",message); 
e x i t(-1); 
} 
while ((c = getopt(argc, argv, "Aatugil?s:d:f:")) != EOF) 
　　　　　　　 switch (c) { 
　　　　case 'A': 
　 tcpenable=1; 
　 udpenable=1; 
　 icmpenable=1; 
　 igmpenable=1; 
　 arpenable=1; 
　 break; 
case 'd': 
　 dstip=optarg; 
break; 
　　　　case 's': 
srcip=optarg; 
break; 
　　　　case 'f': 
tcpflag=optarg; 
break; 
case 'a': 
　　　　　　　　 arpenable=1; 
　　　　　　　　 break; 
　　　　case 't': 
　　　　　　　　 tcpenable=1; 
　　　　　　　　 break; 
　　　 case 'i': 
　　　　　　　　 icmpenable=1; 
　　　　　　　　 break; 
　　　 case 'g': 
igmpenable=1; 
　　　　　　　 break; 
　　　 case 'u': 
　　　　　　　　 udpenable=1; 
　　　　　　　　 break; 
　　　 case 'l': 
loenable=1; 
break; 
　　　 case '?': 
　　　　　　　　 p r i n tf("%s\n",message); 
　　　　　　　　 e x i t(-1); 
　　　　　　　 } 
if ( (waringfd=fo p e n("myxray.waring","a+"))==NULL) 
{ 
perror("fo p e n"); 
e x i t(-1); 
} 
fd_recv = socket(AF_INET, SOCK_PACKET, htons(0x0003)); 
if (fd_recv < 0) { perror( "packet socket error"); e x i t(-1); } 
strcpy(ifr.ifr_name, device); 
if (ioctl(fd_recv, SIOCGIFFLAGS, &ifr) < 0 ) 
{ 
　 perror("ioctl SIOCGIFFLAGS error"); 
　 if (fd_recv >= 0) c l o s e(fd_recv); 
　 e x i t(-1); 
} 
ifr_old = ifr; 
ifr.ifr_flags |= IFF_PROMISC; 
if (ioctl(fd_recv, SIOCSIFFLAGS, &ifr) < 0 ) 
{ 
　 perror("ioctl SIOCSIFFLAGS error"); 
　 if (fd_recv >= 0) c l o s e(fd_recv); 
　 e x i t(-1); 
} 
signal(SIGINT, leave); 
signal(SIGTERM, leave); 
one=time(NULL); 
AGAIN: 
bzero(&from, sizeof(from)); 
from_len = sizeof(from); 
bzero(buf_recv, PACKET_SIZE); 
datalen = recvfrom(fd_recv, (char *)buf_recv, 4096, 0,&from, &from_len); 
if (datalen < 0) 
{ 
perror("recvfrom error"); 
　 e x i t(-1); 
} 
total++; 
buf_recv[datalen] = '\0'; 
if (loenable==0) 
if (strcmp(device, from.sa_data) != 0) goto AGAIN; 
eth=(struct ethhdr *)buf_recv; 
ethprotol=ntohs(eth->h_proto); 
if (ethprotol==0x0806) 
{ 
arphead=(struct arp_packet *)buf_recv; 
　　　　memcpy(&in1,arphead->sndr_ip_addr, IP_ADDR_LEN); 
　　　　memcpy(&in2,arphead->rcpt_ip_addr, IP_ADDR_LEN); 
　　　　srcaddr=inet_ntoa(in1); 
　　　　dstaddr=(char*)inet_ntop(AF_INET,&in2,strptr,sizeof(strptr)); 
totalarp++; 
　　　 if ( (!memcmp(srcaddr,dstaddr,MAX(strlen(srcaddr),strlen(dstaddr))))&& 
(memcmp(hwaddr(arphead->rcpt_hw_addr,buf1),"00:00:00:00:00:00",17)) ) 
　writearpbuf(waringfd,buf_recv,1); 
/*射村arp淫嶄srcaddr,dstaddr�猴�議方象淫*/ 
else 
{ 
if ( memcmp(buf_recv,arpbufone,datalen)==0 ) arpcounter++; 
else 
{ 
if　(arpcounter>LOWCOUNTER) 
/*射村銭偬廛欺議,arpcounter>LOWCOUNTER議arp淫 */ 
　 { 
writearpbuf(waringfd,arpbufone,arpcounter+1); 
} 
memcpy(arpbufone,buf_recv,datalen); 
arpcounter=0; 
}//end else 
}//end else 
} 
if ((ethprotol==0x0806)&&(arpenable)) 
{//arp 
if ( ( (dstip==NULL)||(strcmp(dstaddr,dstip)==0) )&& 
((srcip==NULL)||(strcmp(srcaddr,srcip)==0) ) ) 
　　　 { 
temptime=time(NULL); 
　 p r i n tf("Recorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH\n", 
　 ctime(&temptime),hwaddr(eth->h_source,buf1),hwaddr(eth->h_dest,buf2),ethprotol); 
　 p r i n tf("arphdr\nhwtype:%d protol:%xH hw_size:%d pro_size:%d op:%d\ns_ha:% s s_ip:%s\nd_ha:%s d_ip:%s\n", 
ntohs(arphead->hw_type),ntohs(arphead->prot_type), 
arphead->hw_addr_size, arphead->prot_addr_size, 
ntohs(arphead->op), hwaddr(arphead->sndr_hw_addr,buf1), 
srcaddr, hwaddr(arphead->rcpt_hw_addr,buf2),dstaddr); 
　 p r i n tf("-----------------------------------------------------------\n"); 
disarp++; 
　 } 
　 goto AGAIN; 
}//end arp 
if (ethprotol==0x0800) 
{//ip 
ip = (struct iphdr *)&buf_recv[14]; 
　 ihl = (int)ip->ihl << 2; 
in1.s_addr = ip->saddr; 
　 in2.s_addr = ip->daddr; 
　 srcaddr=inet_ntoa(in1); 
dstaddr=(char *)inet_ntop(AF_INET,&in2,strptr,sizeof(strptr)); 
　 //iphdr 
　 protol=ip->protocol; 
　 ipfragoff=ntohs(ip->frag_off); 
　 df=ipfragoff&IP_DF; 
　 mf=ipfragoff&IP_MF; 
　　 if (df!=0) df=1; 
　 if (mf!=0) mf=1; 
　 protol=ip->protocol; 
switch (protol) 
{ 
case 6 :totaltcp++; 
　 tcp1 = (struct tcphdr *)&buf_recv[14 + ihl]; 
　 flags= tcp1->th_flags; 
　 if (flags&TH_PUSH) flagsmes="PUSH"; 
　 if (flags&TH_ACK) flagsmes="ACK"; 
　 if (flags&TH_URG) flagsmes="URG"; 
　 if (flags&TH_FIN) flagsmes="FIN"; 
　 if (flags&TH_SYN) flagsmes="SYN"; 
　 if (flags&TH_RST) flagsmes="RST"; 
if ( !memcmp(buf_recv,tcpbufone,datalen)) 
tcpcounter++; 
else 
{ 
if (tcpcounter>LOWCOUNTER) 
{ 
　　　　　　　　 writetcpbuf(waringfd,tcpbufone,tcpcounter); 
} 
tcpcounter=0; 
memcpy(tcpbufone,buf_recv,datalen); 
　　　　 } 
break; 
case 1 : totalicmp++; 
　　 icmphdr=(struct icmp *)&buf_recv[14 + ihl]; 
　　　　　　　　　　　 if (!memcmp(buf_recv,icmpbufone,datalen)) 
icmpcounter++; 
else 
{ 
if (icmpcounter>LOWCOUNTER) 
　　　　　　　　 　　　　{ 
writeicmpbuf(waringfd,icmpbufone,icmpcounter); 
　　　　　　　　　　　　 } 
icmpcounter=0; 
　　　　　　　　　　　　 memcpy(icmpbufone,buf_recv,datalen); 
} 
break; 
case 17:totaludp++; 
　 udp= (struct udphdr *)&buf_recv[14 + ihl]; 
　　　　　　　　　　　　if ( !memcmp(buf_recv,udpbufone,datalen)) 
udpcounter++; 
else 
{ 
　　 if (udpcounter>LOWCOUNTER) 
　　　　　　　　　　　　　　 { 
　　　　　writeudpbuf(waringfd,udpbufone,udpcounter); 
　　　　　　　　　　　　　　　} 
　　 udpcounter=0; 
　　　　　　　　　　　　　　 memcpy(udpbufone,buf_recv,datalen); 
} 
break; 
case 2 :totaligmp++; 
　 　　　 igmp1=(struct igmp *)&buf_recv[14+ihl]; 
　　　　　　　　　　　 if (!memcmp(inet_ntoa(igmp1->igmp_group),"0.0.0.0",7)) 
writeigmpbuf(waringfd,buf_recv,1); 
else 
{ 
　 if (!memcmp(buf_recv,igmpbufone,datalen)) 
igmpcounter++; 
　 else 
　 { 
　　 if (igmpcounter>LOWCOUNTER) 
　　　　　　　　　　　　 { 
writeigmpbuf(waringfd,igmpbufone,igmpcounter); } 
　　　igmpcounter=0; 
　　　　　　　　　　　　　　　memcpy(igmpbufone,buf_recv,datalen); 
　 } 
} 
break; 
} 
if (((dstip==NULL)||(strcmp(dstaddr,dstip)==0))&& 
((srcip==NULL)||(strcmp(srcaddr,srcip)==0))) 
{ 
if ((protol==6)&&(tcpenable)) 
{//tcp 
　　 if (tcpflag!=NULL) 
if (strcmp(tcpflag,flagsmes)) goto AGAIN; 
　 //do with ethhdr data 
temptime=time(NULL); 
　 p r i n tf("Recorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH\n", 
ctime(&temptime),hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
ethprotol); 
　 //iphdr 
p r i n tf("iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s\n", 
ip->version,ip->ihl, ip->tos,ntohs(ip->tot_len), 
ntohs(ip->id),df,mf,ipfragoff&IP_OFFMASK,ip->ttl, 
protol,srcaddr,dstaddr); 
　 //tcphdr 
　 p r i n tf("tcphdr\nsrcport:%d dstport:%d seq:%u ack:%u off:%d flag:%s win:%d\n", 
ntohs(tcp1->th_sport),ntohs(tcp1->th_dport), 
ntohl(tcp1->th_seq),ntohl(tcp1->th_ack),tcp1->th_off, 
flagsmes,ntohs(tcp1->th_win)); 
p r i n tf("-----------------------------------------------------------\n"); 
distcp++; 
　 }//end tcp 
if ((protol==1)&&(icmpenable)) 
{//icmp 
temptime=time(NULL); 
　 p r i n tf("Recorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH\n", 
ctime(&temptime),hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
ethprotol); 
p r i n tf("iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s\n", 
ip->version,ip->ihl, ip->tos,ntohs(ip->tot_len), 
ntohs(ip->id),df,mf,ipfragoff&IP_OFFMASK,ip->ttl, 
protol,srcaddr,dstaddr); 
　　 p r i n tf("icmp\ntype:%d code:%d\n",icmphdr->icmp_type, 
icmphdr->icmp_code); 
p r i n tf("-----------------------------------------------------------\n"); 
disicmp++; 
　 }//end icmp 
if ((protol==17)&&(udpenable)) 
{//udphdr 
temptime=time(NULL); 
　 p r i n tf("Recored %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH\n", 
ctime(&temptime),hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
ethprotol); 
p r i n tf("iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s\n", 
ip->version,ip->ihl, ip->tos,ntohs(ip->tot_len), 
ntohs(ip->id),df,mf,ipfragoff&IP_OFFMASK,ip->ttl, 
protol,srcaddr,dstaddr); 
　 p r i n tf("udphdr\nsport:%d dsport:%d len:%d\n", 
ntohs(udp->uh_sport), ntohs(udp->uh_dport), 
ntohs(udp->uh_ulen)); 
p r i n tf("-----------------------------------------------------------\n"); 
disudp++; 
　 }//end udp 
if ((protol==2)&&(igmpenable)) 
{//igmp 
temptime=time(NULL); 
　 p r i n tf("Recored %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH\n", 
ctime(&temptime),hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
ethprotol); 
p r i n tf("iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s\n", 
ip->version,ip->ihl, ip->tos,ntohs(ip->tot_len), 
ntohs(ip->id),df,mf,ipfragoff&IP_OFFMASK,ip->ttl, 
protol,srcaddr,dstaddr); 
　 p r i n tf("igmphdr\ntype:%d code:%d group_addr:%s\n", 
igmp1->igmp_type,igmp1->igmp_code, 
inet_ntoa(igmp1->igmp_group)); 
p r i n tf("-----------------------------------------------------------\n"); 
disigmp++; 
}//end igmp 
　 
fflush(stdout); 
} //endif strcmp 
}//endif ip 
　goto AGAIN; 
}//end main 
char * 
hwaddr (unsigned char * s,char *d) 
{ 
sp r i n tf (d, "%02X:%02X:%02X:%02X:%02X:%02X", s[0], s[1], s[2], s[3], 
s[4], s[5]); 
return d; 
} 
void leave() 
{ 
if (ioctl(fd_recv, SIOCSIFFLAGS, &ifr_old) < 0) { 
perror("ioctl SIOCSIFFLAGS error"); 
} 
if (fd_recv > 0) c l o s e(fd_recv); 
two=time(NULL); 
fc l o s e(waringfd); 
p r i n tf("total received %u packets\n",total); 
p r i n tf("tcp　packets %u, display %u, speed is %u /second\n",totaltcp,distcp,totaltcp/(two-one)); 
p r i n tf("udp　packets %u, display %u, speed is %u /second\n",totaludp,disudp,totaludp/(two-one)); 
p r i n tf("arp　packets %u, display %u, speed is %u /second\n",totalarp,disarp,totalarp/(two-one)); 
p r i n tf("icmp packets %u, display %u, speed is %u /second\n",totalicmp,disicmp,totalicmp/(two-one)); 
p r i n tf("igmp packets %u, display %u, speed is %u /second\n",totaligmp,disigmp,totaligmp/(two-one)); 
p r i n tf("process terminamted.\n"); 
e x i t(0); 
} 
void writearpbuf(FILE * fd,char *p,int counter) 
{ 
struct arp_packet *arphead; 
struct in_addr in2,in1; 
char *srcaddr,*dstaddr; 
time_t temptime; 
char strptr[255]; 
char buf1[20],buf2[20],buf3[20],buf4[20]; 
int ethprotol; 
temptime=time(NULL); 
　　　　　　　　arphead=(struct arp_packet *)p; 
ethprotol=ntohs(arphead->frame_type); 
　　　　　　　　memcpy(&in1,arphead->sndr_ip_addr, IP_ADDR_LEN); 
　　　　　　　　memcpy(&in2,arphead->rcpt_ip_addr, IP_ADDR_LEN); 
　　　　　　　　srcaddr=inet_ntoa(in1); 
　　　　　　　　dstaddr=(char*)inet_ntop(AF_INET,&in2,strptr,sizeof(strptr)); 
　　　　　　　　snp r i n tf(writebuf,PACKET_SIZE,"This arp packet catch %d times continuous\nrecorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH 
arphdr\nhwtype:%d protol:%xH hw_size:%d pro_size:%d op:%d\ns_ha:% s s_ip:%s\nd_ha:%s d_ip:%s 
-----------------------------------------------------------\n",counter,ctime(&temptime), hwaddr(arphead->src_hw_addr,buf1),hwaddr(arphead->targ_hw_addr,buf2),ethprotol, 
　　　　　　　　　　　　ntohs(arphead->hw_type),ntohs(arphead->prot_type), 
　　　　　　　　　　　　arphead->hw_addr_size, arphead->prot_addr_size, 
　　　　　　　　　　　 ntohs(arphead->op), hwaddr(arphead->sndr_hw_addr,buf3),　　　　　　　　　　　　srcaddr, hwaddr(arphead->rcpt_hw_addr,buf4),dstaddr); 
　　　　　　　　fwrite(writebuf,strlen(writebuf),1,fd); 
} 

void writetcpbuf(FILE * fd,char *p,int counter) 
{ 
struct ethhdr *eth ; 
struct iphdr * ip; 
struct tcphdr * tcp1; 
char *flagsmes; 
int flags; 
struct in_addr in1,in2; 
int ethprotol; 
int ihl,df,mf,ipfragoff; 
char * srcaddr,*dstaddr; 
int protol; 
char strptr[255]; 
char buf1[20],buf2[20]; 
eth=(struct ethhdr *)p; 
ethprotol=ntohs(eth->h_proto); 
ip = (struct iphdr *)(p+14); 
　　　　 ihl = (int)ip->ihl << 2; 
　　　　 in1.s_addr = ip->saddr; 
　　　　 in2.s_addr = ip->daddr; 
　　　　 srcaddr=inet_ntoa(in1); 
　　　 dstaddr=(char *)inet_ntop(AF_INET,&in2,strptr,sizeof(strptr)); 
　　　　　　　　//iphdr 
　　　　　　　　protol=ip->protocol; 
　　　　　　　　ipfragoff=ntohs(ip->frag_off); 
　　　　　　　　df=ipfragoff&IP_DF; 
　　　　　　　　mf=ipfragoff&IP_MF; 
　　　　　　　　if (df!=0) df=1; 
　　　　　　　　if (mf!=0) mf=1; 
　　　　 protol=ip->protocol; 
tcp1 = (struct tcphdr *)(p+14 + ihl); 
　　　　　　　　flags= tcp1->th_flags; 
　　　　　　　　if (flags&TH_PUSH)　　　flagsmes="PUSH"; 
　　　　　　　　if (flags&TH_ACK)　　　 flagsmes="ACK"; 
　　　　　　　　if (flags&TH_URG)　　　 flagsmes="URG"; 
　　　　　　　　if (flags&TH_FIN)　　　 flagsmes="FIN"; 
　　　　　　　　if (flags&TH_SYN)　　　 flagsmes="SYN"; 
　　　　　　　　if (flags&TH_RST)　　　 flagsmes="RST"; 

temptime=time(NULL); 
　　　　　　　　snp r i n tf(writebuf,PACKET_SIZE,"This tcp packet catch %d times continuous\nrecorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH 
iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s 
tcphdr\nsrcport:%d dstport:%d seq:%u ack:%u off:%d flag:%s win:%d 
-----------------------------------------------------------\n",counter+1,ctime(&temptime), 
　　　　　　　　 hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
　　　　　　　　 ethprotol,ip->version,ip->ihl, 
　　　　　　　　 ip->tos,ntohs(ip->tot_len), ntohs(ip->id),df,mf, 
　　　　　　　　 ipfragoff&IP_OFFMASK,ip->ttl,protol,srcaddr,dstaddr, 
　　　　　　　　 ntohs(tcp1->th_sport),ntohs(tcp1->th_dport), 
　　　　　　　　 ntohl(tcp1->th_seq),ntohl(tcp1->th_ack),tcp1->th_off, 
　　　　　　　　 flagsmes,ntohs(tcp1->th_win)); 
　　　　　　　　 
fwrite(writebuf,strlen(writebuf),1,fd); 

} 
void writeigmpbuf(FILE *fd,char *p,int counter) 
{ 
struct ethhdr *eth ; 
struct iphdr * ip; 
struct in_addr in1,in2; 
int ethprotol; 
int ihl,df,mf,ipfragoff; 
char * srcaddr,*dstaddr; 
int protol; 
char strptr[255]; 
char buf1[20],buf2[20]; 
struct igmp * igmp1; 
　　　　　　　　eth=(struct ethhdr *)p; 
　　　　　　　　ethprotol=ntohs(eth->h_proto); 
　　　　　　　　ip = (struct iphdr *)(p+14); 
　　　　　　　　ihl = (int)ip->ihl << 2; 
　　　　　　　　in1.s_addr = ip->saddr; 
　　　　　　　　in2.s_addr = ip->daddr; 
　　　　　　　　srcaddr=inet_ntoa(in1); 
　　　　　　　　dstaddr=(char *)inet_ntop(AF_INET,&in2,strptr,sizeof(strptr)); 
　　　　　　　　//iphdr 
　　　　　　　　protol=ip->protocol; 
　　　　　　　　ipfragoff=ntohs(ip->frag_off); 
　　　　　　　　df=ipfragoff&IP_DF; 
mf=ipfragoff&IP_MF; 
　　　　　　　　if (df!=0) df=1; 
　　　　　　　　if (mf!=0) mf=1; 
　　　　　　　　protol=ip->protocol; 
igmp1=(struct igmp *)(p+14+ihl); 
temptime=time(NULL); 
　　　　　　　　　　　　snp r i n tf(writebuf,PACKET_SIZE,"This packet catch %d times continuous\nrecorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH 
iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s 
igmphdr\ntype:%d code:%d group_addr:%s 
-----------------------------------------------------------\n",counter+1,ctime(&temptime), 
　　　　　　　　 hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
　　　　　　　　 ethprotol,ip->version,ip->ihl, 
　　　　　　　　 ip->tos,ntohs(ip->tot_len), ntohs(ip->id),df,mf, 
　　　　　　　　 ipfragoff&IP_OFFMASK,ip->ttl,protol,srcaddr,dstaddr, 
　　　　　　　　igmp1->igmp_type,igmp1->igmp_code,inet_ntoa(igmp1->igmp_group)); 
　　　　　　　　 fwrite(writebuf,strlen(writebuf),1,fd); 

} 
void writeudpbuf(FILE * fd,char *p,int counter) 
{ 
struct ethhdr *eth ; 
struct iphdr * ip; 
struct in_addr in1,in2; 
int ethprotol; 
int ihl,df,mf,ipfragoff; 
char * srcaddr,*dstaddr; 
int protol; 
char strptr[255]; 
char buf1[20],buf2[20]; 
struct udphdr *udp; 
eth=(struct ethhdr *)p; 
　　　　　　　　ethprotol=ntohs(eth->h_proto); 
　　　　　　　　ip = (struct iphdr *)(p+14); 
　　　　　　　　ihl = (int)ip->ihl << 2; 
　　　　　　　　in1.s_addr = ip->saddr; 
　　　　　　　　in2.s_addr = ip->daddr; 
　　　　　　　　srcaddr=inet_ntoa(in1); 
　　　　　　　　dstaddr=(char *)inet_ntop(AF_INET,&in2,strptr,sizeof(strptr)); 
　　　　　　　　//iphdr 
　　　　　　　　protol=ip->protocol; 
　　　　　　　　ipfragoff=ntohs(ip->frag_off); 
　　　　　　　　df=ipfragoff&IP_DF; 
　　　　　　　　mf=ipfragoff&IP_MF; 
　　　　　　　　if (df!=0) df=1; 
　　　　　　　　if (mf!=0) mf=1; 
　　　　　　　　protol=ip->protocol; 
　　　　　　　　udp= (struct udphdr *)(p+14 + ihl); 
temptime=time(NULL); 
　　　　　　　　snp r i n tf(writebuf,PACKET_SIZE,"This udp packet catch %d times continuous\nrecorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH 
iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s 
udphdr\nsport:%d dsport:%d len:%d 
-----------------------------------------------------------\n",counter+1,ctime(&temptime), 
　　　　　　　　hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
　　　　　　　　ethprotol,ip->version,ip->ihl, 
　　　　　　　　ip->tos,ntohs(ip->tot_len), ntohs(ip->id),df,mf, 
　　　　　　　　ipfragoff&IP_OFFMASK,ip->ttl,protol,srcaddr,dstaddr, 
　　　　　　　　ntohs(udp->uh_sport), ntohs(udp->uh_dport),ntohs(udp->uh_ulen)); 
　　　　　　　　 
fwrite(writebuf,strlen(writebuf),1,fd); 
} 
void writeicmpbuf(FILE * fd,char *p,int counter) 
{ 
struct ethhdr *eth ; 
struct iphdr * ip; 
struct in_addr in1,in2; 
int ethprotol; 
int ihl,df,mf,ipfragoff; 
char * srcaddr,*dstaddr; 
int protol; 
char strptr[255]; 
char buf1[20],buf2[20]; 
struct icmp *icmphdr; 
　　　　　eth=(struct ethhdr *)p; 
　　　　　　　　ethprotol=ntohs(eth->h_proto); 
　　　　　　　　ip = (struct iphdr *)(p+14); 
　　　　　　　　ihl = (int)ip->ihl << 2; 
　　　　　　　　in1.s_addr = ip->saddr; 
　　　　　　　　in2.s_addr = ip->daddr; 
　　　　　　　　srcaddr=inet_ntoa(in1); 
　　　　　　　　dstaddr=(char *)inet_ntop(AF_INET,&in2,strptr,sizeof(strptr)); 
　　　　　　　　//iphdr 
　　　　　　　　protol=ip->protocol; 
　　　　　　　　ipfragoff=ntohs(ip->frag_off); 
　　　　　　　　df=ipfragoff&IP_DF; 
　　　　　　　　mf=ipfragoff&IP_MF; 
　　　　　　　　if (df!=0) df=1; 
　　　　　　　　if (mf!=0) mf=1; 
　　　　　　　　protol=ip->protocol; 
icmphdr=(struct icmp *)(p+14 + ihl); 
temptime=time(NULL); 
　　　　　　　　　　　　snp r i n tf(writebuf,PACKET_SIZE,"This icmp packet catch %d times continuous\nrecorded %sethhdr\nsrchw:%s--->dsthw:%s proto:%xH 
iphdr\nver:%d ihl:%d tos:%d tot_len:%d id:%d df:%d mf:%d fragoff:%d TTL:%d proto:%d\nsrcaddr:%s dstaddr:%s 
icmp\ntype:%d code:%d 
-----------------------------------------------------------\n",counter+1,ctime(&temptime), 
　　　　　　　　　　　　hwaddr(eth->h_source,buf1), hwaddr(eth->h_dest,buf2), 
　　　　　　　　　　　　ethprotol,ip->version,ip->ihl, 
　　　　　　　　　　　　ip->tos,ntohs(ip->tot_len), ntohs(ip->id),df,mf, 
　　　　　　　　　　　　ipfragoff&IP_OFFMASK,ip->ttl,protol,srcaddr,dstaddr, 
　　　　　　　　　　　　icmphdr->icmp_type,icmphdr->icmp_code); 
　　　　　　　　　　　　fwrite(writebuf,strlen(writebuf),1,fd); 
} 
