#include <core.p4>


#if __TARGET_TOFINO__ == 2
#include <t2na.p4>
#else
#include <tna.p4>
#endif

#include "common/util.p4"
#include "common/headers.p4"

/*************************************************************************
 ************* C O N S T A N T S    A N D   T Y P E S  *******************
**************************************************************************/
const bit<8>  TYPEOPT= 145;
const bit<8>  EPV= 143;
const bit<8>  RFL= 144;
#define CPU_PORT 192
typedef bit<9>  egressSpec_t;


/*************************************************************************
 **************  I N G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/


    /******  G L O B A L   I N G R E S S   M E T A D A T A  *********/

struct metadata_t {
    bit<16> checksum_tcp_tmp;
    bit<32> pkgcount;
    bit<32> pkgcount2;
}

    /***********************  P A R S E R  **************************/
parser IngressParser(packet_in        pkt,
    /* User */
    out header_t          hdr,
    out metadata_t         meta,
    /* Intrinsic */
    out ingress_intrinsic_metadata_t  ig_intr_md)
{
    /* This is a mandatory state, required by Tofino Architecture */
    state start {
        pkt.extract(ig_intr_md);
        pkt.advance(PORT_METADATA_SIZE);
        transition meta_init;
    }
	
	state meta_init {
		transition parse_ethernet;
	}

    state parse_ethernet {
        pkt.extract(hdr.ethernet);
        transition parse_ipv4;
    }

    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        transition  accept;
    }

}

 /***************** M A T C H - A C T I O N  *********************/
control Ingress(
    /* User */
    inout header_t                       hdr,
    inout metadata_t                      meta,
    /* Intrinsic */
    in    ingress_intrinsic_metadata_t               ig_intr_md,
    in    ingress_intrinsic_metadata_from_parser_t   ig_prsr_md,
    inout ingress_intrinsic_metadata_for_deparser_t  ig_dprsr_md,
    inout ingress_intrinsic_metadata_for_tm_t        ig_tm_md
     )
{
    action set_egress_port(egressSpec_t port) {                                  
        ig_tm_md.ucast_egress_port = port; //设置出端口
    }                                                        
    table set_out_port { 
         key = {
            ig_intr_md.ingress_port: exact;
        }	
        actions = { set_egress_port; 
		}                                        
    }

    action set_dst(egressSpec_t port) {                                  
        ig_tm_md.ucast_egress_port = port; //设置出端口
    }                                                        
    table set_dst_port { 
         key = {
            hdr.ipv4.dst_addr: exact;
        }	
        actions = { set_dst; 
		}                                        
    }
    
    apply {
         set_out_port.apply();
         set_dst_port.apply();
    }
}

    /*********************  D E P A R S E R  ************************/

control IngressDeparser(packet_out pkt,
    /* User */
    inout header_t                       hdr,
    in    metadata_t                      meta,
    /* Intrinsic */
    in    ingress_intrinsic_metadata_for_deparser_t  ig_dprsr_md)
{
    apply{
        pkt.emit(hdr);
    }
}


/*************************************************************************
 ****************  E G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/


    /********  G L O B A L   E G R E S S   M E T A D A T A  *********/



    /***********************  P A R S E R  **************************/

parser EgressParser(packet_in        pkt,
    /* User */
    out header_t          hdr,
    out metadata_t         meta,
    /* Intrinsic */
    out egress_intrinsic_metadata_t  eg_intr_md)
{
    /* This is a mandatory state, required by Tofino Architecture */
    TofinoEgressParser() tofino_parser;
    Checksum<bit<16>>(HashAlgorithm_t.CSUM16) tcp_checksum;

    state start {
        tofino_parser.apply(pkt, eg_intr_md);
        transition parse_ethernet;
    }

    state parse_ethernet {
        pkt.extract(hdr.ethernet);
        transition select (hdr.ethernet.ether_type) {
            ETHERTYPE_IPV4 : parse_ipv4;
            default : reject;
        }
    }

    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        transition  parse_tcp;
    }

    state parse_tcp {
        pkt.extract(hdr.tcp);
        tcp_checksum.subtract({hdr.tcp.checksum});
        meta.checksum_tcp_tmp = tcp_checksum.get();
        transition parse_option;
    }

    state parse_option{
        pkt.extract(hdr.opts);
        transition accept;
    }

}

    /***************** M A T C H - A C T I O N  *********************/

control Egress(
    /* User */
    inout header_t                          hdr,
    inout metadata_t                         meta,
    /* Intrinsic */
    in    egress_intrinsic_metadata_t                  eg_intr_md,
    in    egress_intrinsic_metadata_from_parser_t      eg_prsr_md,
    inout egress_intrinsic_metadata_for_deparser_t     eg_dprsr_md,
    inout egress_intrinsic_metadata_for_output_port_t  eg_oport_md)
{
    
    //use register to get txbytes
    Register<bit<32>, bit<32>>(3) test_reg;
    RegisterAction<bit<32>, bit<32>, bit<32>>(test_reg) test_reg_action = {
        void apply(inout bit<32> value, out bit<32> read_value){
            read_value = value;
            value = value + 1;
        }
    };

    action register_action(bit<32> idx) {
        meta.pkgcount = test_reg_action.execute(idx);
    }

    table reg_match {
        key = {
            eg_intr_md.egress_port : exact;
        }
        actions = {
            register_action;
        }
        size = 3;
    }


    //use register to get txbytes
    Register<bit<32>, bit<32>>(3) test_reg2;
    RegisterAction<bit<32>, bit<32>, bit<32>>(test_reg2) test_reg2_action = {
        void apply(inout bit<32> value, out bit<32> read_value){
            read_value = value;
            value = value + 1;
        }
    };

    action register_action2(bit<32> idx) {
        meta.pkgcount2 = test_reg2_action.execute(idx);
    }

    table reg_match2 {
        key = {
            eg_intr_md.egress_port : exact;
        }
        actions = {
            register_action2;
        }
        size = 3;
    }

    // --- new options ----------------------------------------------------
    
    action set_options() {
        // get queue length
        
        hdr.opts.sw1 = (qdepth_t)eg_intr_md.enq_qdepth;
        hdr.opts.sw2 = (qdepth_t)(eg_intr_md.deq_timedelta);
        hdr.opts.sw3 = meta.pkgcount - meta.pkgcount2;
        
        //hdr.opts.sw1 = 32w1;
        //hdr.opts.sw2 = 32w2;
        //hdr.opts.sw3 = 32w3;
    }

    table options_table {
        key = {
          hdr.opts.nop4_kind : exact;
        }
        actions = {
          set_options;
        }
    }
    
    
    apply {
        reg_match.apply();
        reg_match2.apply();
        options_table.apply();
    }
}

    /*********************  D E P A R S E R  ************************/

control EgressDeparser(packet_out pkt,
    /* User */
    inout header_t                       hdr,
    in    metadata_t                      meta,
    /* Intrinsic */
    in    egress_intrinsic_metadata_for_deparser_t  eg_dprsr_md)
{
    Checksum<bit<16>>(HashAlgorithm_t.CSUM16) tcp_checksum;

    apply {
        hdr.tcp.checksum = tcp_checksum.update({
                meta.checksum_tcp_tmp,
                hdr.opts
            });

         pkt.emit(hdr);
    }
}


/************ F I N A L   P A C K A G E ******************************/
Pipeline(
    IngressParser(),
    Ingress(),
    IngressDeparser(),
    EgressParser(),
    Egress(),
    EgressDeparser()
) pipe;

Switch(pipe) main;
