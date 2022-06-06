
/*define new TCP option in tcp_options_write and in tcp_output.c*/
static void tcp_options_write(__be32 *ptr, struct tcp_sock *tp,
 			       struct tcp_out_options *opts){
if(likely(OPTION_MYOPT & options)){
    *ptr++ = htonl((TCPOPT_NOP << 24) |
                    (TCPOPT_NOP << 16) |
                    (TCPOPT_MYOPT << 8) |
                    (TCPOLEN_MYOPT));
    *ptr++ = htonl(opts->qlen);
    *ptr++ = htonl(opts->timestamp);
    *ptr++ = htonl(opts->pkg_count);
}
static unsigned int tcp_established_options
		(struct sock *sk, struct sk_buff *skb
 			opts->num_sack_blocks * TCPOLEN_SACK_PERBLOCK）{
	if(sock_net(sk)->ipv4.sysctl_tcp_myopt==1){
			opts->options|=OPTION_MYOPT;
			opts->qlen=0;
			opts->timestamp=0;
			opts->pkg_count=0;
			size += TCPOLEN_MYOPT_ALIGNED;
		}
 }
/*parse new TCP option in tcp_parse_options and in tcp_input.c*/
tcp_parse_options(const struct net *net,
		       const struct sk_buff *skb,
		       struct tcp_options_received *opt_rx, int estab,
		       struct tcp_fastopen_cookie *foc){
case TCPOPT_QLEN:
				if ((opsize == TCPOLEN_QLEN) &&
					(net->ipv4.sysctl_tcp_qlen)) {
						opt_rx->qlen = get_unaligned_be32(ptr);
						opt_rx->timedelta = get_unaligned_be32(ptr+4);
						opt_rx->txbytes = get_unaligned_be32(ptr+8);
						cur_qlen = opt_rx->qlen;
						cur_timedelta = opt_rx->timestamp;
						cur_pkg_count = opt_rx->pkg_count;
				}
				break;
}