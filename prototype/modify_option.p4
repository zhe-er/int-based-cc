
/*tcp option header*/
header opts_h {
    bit<8> nop1;
    bit<8> nop2;
    bit<8> kind;
    bit<8> opt_length;
    bit<32> qlen;
    bit<32> timestamp;
    bit<32> pkg_count;
}
/*use metadata to save the unmodified tcp checksum*/
struct metadata_t {
    bit<16> checksum_tcp_tmp;
}
/*parse tcp option*/
state parse_option{
    pkt.extract(hdr.opts);
    transition accept;
}
/*write the queue length, the timestamp, the number of packets 
processed in uplink*/
typedef bit<32> qdepth_t;
typedef bit<32> timestamp_t;
action write_option_action() {
    hdr.opts.qlen = (qdepth_t)eg_intr_md.enq_qdepth;
    hdr.opts.timestamp = (timestamp_t)(eg_intr_md.enq_tstamp);
    hdr.opts.pkg_count = meta.pkgcount_overall - meta.pkgcount_downlink;
    }
table write_option {
    key = {
        hdr.opts.kind : exact;
    }
    actions = {
        write_option_action;
    }
}
/*deparse tcp option*/
Checksum<bit<16>>(HashAlgorithm_t.CSUM16) tcp_checksum;
apply {
    hdr.tcp.checksum = tcp_checksum.update({
            meta.checksum_tcp_tmp,
            hdr.opts
        });
        pkt.emit(hdr);
}



/*modify tcp option*/
/*use metadata to save the data of register counts*/
struct metadata_t {
    bit<32> pkgcount_overall;
    bit<32> pkgcount_downlink;
}
//use register to get the  number of packets both in uplink and downlink
Register<bit<32>, bit<32>>(3) reg_overall;
RegisterAction<bit<32>, bit<32>, bit<32>>(reg_overall) reg_overall_action = {
    void apply(inout bit<32> value, out bit<32> read_value){
        value = value + 1;
        read_value = value;
    }
};
action register_overall_action(bit<32> idx) {
    meta.pkgcount_overall = reg_overall_action.execute(idx);
}
table reg_overall_match {
    key = {
        eg_intr_md.egress_port : exact;
    }
    actions = {
        register_overall_action;
    }
}
//use register to get the  number of packets both in downlink
Register<bit<32>, bit<32>>(3) reg_downlink;
RegisterAction<bit<32>, bit<32>, bit<32>>(reg_downlink) reg_downlink_action = {
    void apply(inout bit<32> value, out bit<32> read_value){
        value = value + 1;
        read_value = value;
    }
};
action register_downlink_action(bit<32> idx) {
    meta.pkgcount_downlink = reg_downlink_action.execute(idx);
}
table reg_downlink_match {
    key = {
        eg_intr_md.egress_port : exact;
    }
    actions = {
        register_downlink_action;
    }
}







