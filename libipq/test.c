#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/netfilter.h>
#include <libipq.h>

#include <libnet.h>
#define BUFSIZE 2048 

static void die(struct ipq_handle *h)
{
 ipq_perror("passer");
 ipq_destroy_handle(h);
 exit(1);
}

int main()
{
 int status;
 unsigned char buf[BUFSIZE];
 struct ipq_handle *h;
printf("!@#");
 char *ip_dst_s="202.108.22.5";
 h = ipq_create_handle(0, PF_INET);
 if (!h)
  die(h);
                
 status = ipq_set_mode(h, IPQ_COPY_PACKET, BUFSIZE);
 if (status < 0)
  die(h);
                
 do{

  status = ipq_read(h, buf, BUFSIZE, 0);
printf("!@#");
/*  if (status < 0)
   die(h);
                        
  switch (ipq_message_type(buf)) {
   case NLMSG_ERROR:
    fprintf(stderr, "Received error message %d/n",
      ipq_get_msgerr(buf));
    break;
                                
   case IPQM_PACKET: {
    ipq_packet_msg_t *m = ipq_get_packet(buf);
    struct libnet_ipv4_hdr* iph;
    iph=(struct libnet_ipv4_hdr*)m->payload;
    if(m->data_len<20){
     fprintf(stderr, "ip header is too short/n");
     die(h);
    }

    char *dst = libnet_addr2name4(iph->ip_dst.s_addr, LIBNET_DONT_RESOLVE);
    
    if(!strcmp(dst, ip_dst_s)){
      status = ipq_set_verdict(h, m->packet_id, NF_ACCEPT, 0, NULL);
     printf("dst ip:%s state: accept/n", dst);
    }
    else{
     status = ipq_set_verdict(h, m->packet_id, NF_DROP, 0, NULL);
     printf("dst ip:%s state: drop/n", dst);
    }
 
    if (status < 0)
     die(h);
    break;
   }
   default:
    fprintf(stderr, "Unknown message type!/n");
    break;
  }*/
 } while (1);

 ipq_destroy_handle(h);
 return 0;
}
