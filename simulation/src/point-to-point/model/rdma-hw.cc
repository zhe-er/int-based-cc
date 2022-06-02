#include <ns3/simulator.h>
#include <ns3/seq-ts-header.h>
#include <ns3/udp-header.h>
#include <ns3/ipv4-header.h>
#include "ns3/ppp-header.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/data-rate.h"
#include "ns3/pointer.h"
#include "rdma-hw.h"
#include "ppp-header.h"
#include "qbb-header.h"
#include "cn-header.h"

#include "ns3/ipv4-priority-tag.h"

namespace ns3{

TypeId RdmaHw::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::RdmaHw")
		.SetParent<Object> ()
		.AddAttribute("MinRate",
				"Minimum rate of a throttled flow",
				DataRateValue(DataRate("100Mb/s")),
				MakeDataRateAccessor(&RdmaHw::m_minRate),
				MakeDataRateChecker())
		.AddAttribute("Mtu",
				"Mtu.",
				UintegerValue(1000),
				MakeUintegerAccessor(&RdmaHw::m_mtu),
				MakeUintegerChecker<uint32_t>())
		.AddAttribute ("CcMode",
				"which mode of DCQCN is running",
				UintegerValue(0),
				MakeUintegerAccessor(&RdmaHw::m_cc_mode),
				MakeUintegerChecker<uint32_t>())
		.AddAttribute("NACK Generation Interval",
				"The NACK Generation interval",
				DoubleValue(500.0),
				MakeDoubleAccessor(&RdmaHw::m_nack_interval),
				MakeDoubleChecker<double>())
		.AddAttribute("L2ChunkSize",
				"Layer 2 chunk size. Disable chunk mode if equals to 0.",
				UintegerValue(0),
				MakeUintegerAccessor(&RdmaHw::m_chunk),
				MakeUintegerChecker<uint32_t>())
		.AddAttribute("L2AckInterval",
				"Layer 2 Ack intervals. Disable ack if equals to 0.",
				UintegerValue(0),
				MakeUintegerAccessor(&RdmaHw::m_ack_interval),
				MakeUintegerChecker<uint32_t>())
		.AddAttribute("L2BackToZero",
				"Layer 2 go back to zero transmission.",
				BooleanValue(false),
				MakeBooleanAccessor(&RdmaHw::m_backto0),
				MakeBooleanChecker())
		.AddAttribute("EwmaGain",
				"Control gain parameter which determines the level of rate decrease",
				DoubleValue(1.0 / 16),
				MakeDoubleAccessor(&RdmaHw::m_g),
				MakeDoubleChecker<double>())
		.AddAttribute ("RateOnFirstCnp",
				"the fraction of rate on first CNP",
				DoubleValue(1.0),
				MakeDoubleAccessor(&RdmaHw::m_rateOnFirstCNP),
				MakeDoubleChecker<double> ())
		.AddAttribute("ClampTargetRate",
				"Clamp target rate.",
				BooleanValue(false),
				MakeBooleanAccessor(&RdmaHw::m_EcnClampTgtRate),
				MakeBooleanChecker())
		.AddAttribute("RPTimer",
				"The rate increase timer at RP in microseconds",
				DoubleValue(1500.0),
				MakeDoubleAccessor(&RdmaHw::m_rpgTimeReset),
				MakeDoubleChecker<double>())
		.AddAttribute("RateDecreaseInterval",
				"The interval of rate decrease check",
				DoubleValue(4.0),
				MakeDoubleAccessor(&RdmaHw::m_rateDecreaseInterval),
				MakeDoubleChecker<double>())
		.AddAttribute("FastRecoveryTimes",
				"The rate increase timer at RP",
				UintegerValue(5),
				MakeUintegerAccessor(&RdmaHw::m_rpgThreshold),
				MakeUintegerChecker<uint32_t>())
		.AddAttribute("AlphaResumInterval",
				"The interval of resuming alpha",
				DoubleValue(55.0),
				MakeDoubleAccessor(&RdmaHw::m_alpha_resume_interval),
				MakeDoubleChecker<double>())
		.AddAttribute("RateAI",
				"Rate increment unit in AI period",
				DataRateValue(DataRate("5Mb/s")),
				MakeDataRateAccessor(&RdmaHw::m_rai),
				MakeDataRateChecker())
		.AddAttribute("RateHAI",
				"Rate increment unit in hyperactive AI period",
				DataRateValue(DataRate("50Mb/s")),
				MakeDataRateAccessor(&RdmaHw::m_rhai),
				MakeDataRateChecker())
		.AddAttribute("VarWin",
				"Use variable window size or not",
				BooleanValue(false),
				MakeBooleanAccessor(&RdmaHw::m_var_win),
				MakeBooleanChecker())
		.AddAttribute("FastReact",
				"Fast React to congestion feedback",
				BooleanValue(true),
				MakeBooleanAccessor(&RdmaHw::m_fast_react),
				MakeBooleanChecker())
		.AddAttribute("MiThresh",
				"Threshold of number of consecutive AI before MI",
				UintegerValue(5),
				MakeUintegerAccessor(&RdmaHw::m_miThresh),
				MakeUintegerChecker<uint32_t>())
		.AddAttribute("TargetUtil",
				"The Target Utilization of the bottleneck bandwidth, by default 95%",
				DoubleValue(0.95),
				MakeDoubleAccessor(&RdmaHw::m_targetUtil),
				MakeDoubleChecker<double>())
		.AddAttribute("UtilHigh",
				"The upper bound of Target Utilization of the bottleneck bandwidth, by default 98%",
				DoubleValue(0.98),
				MakeDoubleAccessor(&RdmaHw::m_utilHigh),
				MakeDoubleChecker<double>())
		.AddAttribute("RateBound",
				"Bound packet sending by rate, for test only",
				BooleanValue(true),
				MakeBooleanAccessor(&RdmaHw::m_rateBound),
				MakeBooleanChecker())
		.AddAttribute("MultiRate",
				"Maintain multiple rates in HPCC",
				BooleanValue(true),
				MakeBooleanAccessor(&RdmaHw::m_multipleRate),
				MakeBooleanChecker())
		.AddAttribute("SampleFeedback",
				"Whether sample feedback or not",
				BooleanValue(false),
				MakeBooleanAccessor(&RdmaHw::m_sampleFeedback),
				MakeBooleanChecker())
		.AddAttribute("TimelyAlpha",
				"Alpha of TIMELY",
				DoubleValue(0.875),
				MakeDoubleAccessor(&RdmaHw::m_tmly_alpha),
				MakeDoubleChecker<double>())
		.AddAttribute("TimelyBeta",
				"Beta of TIMELY",
				DoubleValue(0.8),
				MakeDoubleAccessor(&RdmaHw::m_tmly_beta),
				MakeDoubleChecker<double>())
		.AddAttribute("TimelyTLow",
				"TLow of TIMELY (ns)",
				UintegerValue(50000),
				MakeUintegerAccessor(&RdmaHw::m_tmly_TLow),
				MakeUintegerChecker<uint64_t>())
		.AddAttribute("TimelyTHigh",
				"THigh of TIMELY (ns)",
				UintegerValue(500000),
				MakeUintegerAccessor(&RdmaHw::m_tmly_THigh),
				MakeUintegerChecker<uint64_t>())
		.AddAttribute("TimelyMinRtt",
				"MinRtt of TIMELY (ns)",
				UintegerValue(20000),
				MakeUintegerAccessor(&RdmaHw::m_tmly_minRtt),
				MakeUintegerChecker<uint64_t>())
		.AddAttribute("DctcpRateAI",
				"DCTCP's Rate increment unit in AI period",
				DataRateValue(DataRate("1000Mb/s")),
				MakeDataRateAccessor(&RdmaHw::m_dctcp_rai),
				MakeDataRateChecker())
		.AddAttribute("PintSmplThresh",
				"PINT's sampling threshold in rand()%65536",
				UintegerValue(65536),
				MakeUintegerAccessor(&RdmaHw::pint_smpl_thresh),
				MakeUintegerChecker<uint32_t>())
		;
	return tid;
}

RdmaHw::RdmaHw(){
}

//set controller
void
RdmaHw::MyController (Ptr<ControlDecider> controller)
{
  hw_controller = controller;
}

void RdmaHw::SetNode(Ptr<Node> node){
	m_node = node;
}
void RdmaHw::Setup(QpCompleteCallback cb){
	for (uint32_t i = 0; i < m_nic.size(); i++){
		Ptr<QbbNetDevice> dev = m_nic[i].dev;
		if (dev == NULL)
			continue;
		// share data with NIC
		dev->m_rdmaEQ->m_qpGrp = m_nic[i].qpGrp;
		// setup callback
		dev->m_rdmaReceiveCb = MakeCallback(&RdmaHw::Receive, this);
		dev->m_rdmaLinkDownCb = MakeCallback(&RdmaHw::SetLinkDown, this);
		dev->m_rdmaPktSent = MakeCallback(&RdmaHw::PktSent, this);
		// config NIC
		dev->m_rdmaEQ->m_rdmaGetNxtPkt = MakeCallback(&RdmaHw::GetNxtPacket, this);
	}
	// setup qp complete callback
	m_qpCompleteCallback = cb;
}

uint32_t RdmaHw::GetNicIdxOfQp(Ptr<RdmaQueuePair> qp){
	auto &v = m_rtTable[qp->dip.Get()];
	if (v.size() > 0){
		return v[qp->GetHash() % v.size()];
	}else{
		NS_ASSERT_MSG(false, "We assume at least one NIC is alive");
	}
}
uint64_t RdmaHw::GetQpKey(uint32_t dip, uint16_t sport, uint16_t pg){
	return ((uint64_t)dip << 32) | ((uint64_t)sport << 16) | (uint64_t)pg;
}
Ptr<RdmaQueuePair> RdmaHw::GetQp(uint32_t dip, uint16_t sport, uint16_t pg){
	uint64_t key = GetQpKey(dip, sport, pg);
	auto it = m_qpMap.find(key);
	if (it != m_qpMap.end())
		return it->second;
	return NULL;
}
void RdmaHw::AddQueuePair(uint64_t size, uint16_t pg, Ipv4Address sip, Ipv4Address dip, uint16_t sport, uint16_t dport, uint32_t win, uint64_t baseRtt, Callback<void> notifyAppFinish){
	// create qp
	Ptr<RdmaQueuePair> qp = CreateObject<RdmaQueuePair>(pg, sip, dip, sport, dport);
	qp->SetSize(size);
	qp->SetWin(win);
	qp->SetBaseRtt(baseRtt);
	qp->SetVarWin(m_var_win);
	qp->SetAppNotifyCallback(notifyAppFinish);

	//to compute the fairness of competition
	qp->m_cc_mode = hw_controller->CurCC();

	if(ifCompare)
	{
		m_cc_mode = qp->m_cc_mode;
	}
	

	// add qp
	uint32_t nic_idx = GetNicIdxOfQp(qp);
	m_nic[nic_idx].qpGrp->AddQp(qp);
	uint64_t key = GetQpKey(dip.Get(), sport, pg);
	m_qpMap[key] = qp;
	
	// set init variables
	DataRate m_bps = m_nic[nic_idx].dev->GetDataRate();
	qp->m_rate = m_bps;
	qp->m_max_rate = m_bps;
	if (m_cc_mode == 1){
		qp->mlx.m_targetRate = m_bps;
	}else if (m_cc_mode == 3){
		qp->hp.m_curRate = m_bps;
		if (m_multipleRate){
			for (uint32_t i = 0; i < IntHeader::maxHop; i++)
				qp->hp.hopState[i].Rc = m_bps;
		}
	}else if (m_cc_mode == 7){
		qp->tmly.m_curRate = m_bps;
	}else if (m_cc_mode == 10){
		qp->hpccPint.m_curRate = m_bps;
	}

	// Notify Nic
	m_nic[nic_idx].dev->NewQp(qp);
}

void RdmaHw::DeleteQueuePair(Ptr<RdmaQueuePair> qp){
	// remove qp from the m_qpMap
	uint64_t key = GetQpKey(qp->dip.Get(), qp->sport, qp->m_pg);
	m_qpMap.erase(key);
}

Ptr<RdmaRxQueuePair> RdmaHw::GetRxQp(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport, uint16_t pg, bool create){
	uint64_t key = ((uint64_t)dip << 32) | ((uint64_t)pg << 16) | (uint64_t)dport;
	auto it = m_rxQpMap.find(key);
	if (it != m_rxQpMap.end())
		return it->second;
	if (create){
		// create new rx qp
		Ptr<RdmaRxQueuePair> q = CreateObject<RdmaRxQueuePair>();
		// init the qp
		q->sip = sip;
		q->dip = dip;
		q->sport = sport;
		q->dport = dport;
		q->m_ecn_source.qIndex = pg;
		// store in map
		m_rxQpMap[key] = q;
		return q;
	}
	return NULL;
}
uint32_t RdmaHw::GetNicIdxOfRxQp(Ptr<RdmaRxQueuePair> q){
	auto &v = m_rtTable[q->dip];
	if (v.size() > 0){
		return v[q->GetHash() % v.size()];
	}else{
		NS_ASSERT_MSG(false, "We assume at least one NIC is alive");
	}
}
void RdmaHw::DeleteRxQp(uint32_t dip, uint16_t pg, uint16_t dport){
	uint64_t key = ((uint64_t)dip << 32) | ((uint64_t)pg << 16) | (uint64_t)dport;
	m_rxQpMap.erase(key);
}

int RdmaHw::ReceiveUdp(Ptr<Packet> p, CustomHeader &ch){
	uint8_t ecnbits = ch.GetIpv4EcnBits();

	uint32_t payload_size = p->GetSize() - ch.GetSerializedSize();

	// TODO find corresponding rx queue pair
	Ptr<RdmaRxQueuePair> rxQp = GetRxQp(ch.dip, ch.sip, ch.udp.dport, ch.udp.sport, ch.udp.pg, true);
	if (ecnbits != 0){
		rxQp->m_ecn_source.ecnbits |= ecnbits;
		rxQp->m_ecn_source.qfb++;
	}
	rxQp->m_ecn_source.total++;
	rxQp->m_milestone_rx = m_ack_interval;

	int x = ReceiverCheckSeq(ch.udp.seq, rxQp, payload_size);
	if (x == 1 || x == 2){ //generate ACK or NACK
		qbbHeader seqh;
		seqh.SetSeq(rxQp->ReceiverNextExpectedSeq);
		seqh.SetPG(ch.udp.pg);
		seqh.SetSport(ch.udp.dport);
		seqh.SetDport(ch.udp.sport);
		seqh.SetIntHeader(ch.udp.ih);
		if (ecnbits)
			seqh.SetCnp();

		Ptr<Packet> newp = Create<Packet>(std::max(60-14-20-(int)seqh.GetSerializedSize(), 0));
		newp->AddHeader(seqh);

		Ipv4Header head;	// Prepare IPv4 header
		head.SetDestination(Ipv4Address(ch.sip));
		head.SetSource(Ipv4Address(ch.dip));
		head.SetProtocol(x == 1 ? 0xFC : 0xFD); //ack=0xFC nack=0xFD
		head.SetTtl(64);
		head.SetPayloadSize(newp->GetSize());
		head.SetIdentification(rxQp->m_ipid++);

		newp->AddHeader(head);
		AddHeader(newp, 0x800);	// Attach PPP header

		
		//add priority tag
		uint32_t type = 3;
		uint64_t m_size;
		uint64_t key = ((uint64_t)ch.dip << 32) | ((uint64_t)ch.udp.sport << 16) | (uint64_t)ch.udp.pg;
		auto it = hw_controller->overallkey.find(key);
		if(it != hw_controller->overallkey.end())
		{
			m_size = it->second;
		}

		Ipv4PriorityTag ipv4PriorityackTag;
		if(m_size < 20000)  //20%
		{
			type = 0;
		}
		else if(m_size >=20000 and m_size <50000) //40%
		{
			type = 1;
		}
		else
		{
			type = 2;
		}
		
		ipv4PriorityackTag.SetPriorityTag(type);
		newp->AddByteTag (ipv4PriorityackTag);
		//std::cout << "this is for receiveudp" << std::endl;

		// send
		uint32_t nic_idx = GetNicIdxOfRxQp(rxQp);
		m_nic[nic_idx].dev->RdmaEnqueueHighPrioQ(newp);
		m_nic[nic_idx].dev->TriggerTransmit();
	}
	return 0;
}

int RdmaHw::ReceiveCnp(Ptr<Packet> p, CustomHeader &ch){
	// QCN on NIC
	// This is a Congestion signal
	// Then, extract data from the congestion packet.
	// We assume, without verify, the packet is destinated to me
	uint32_t qIndex = ch.cnp.qIndex;
	if (qIndex == 1){		//DCTCP
		std::cout << "TCP--ignore\n";
		return 0;
	}
	uint16_t udpport = ch.cnp.fid; // corresponds to the sport
	uint8_t ecnbits = ch.cnp.ecnBits;
	uint16_t qfb = ch.cnp.qfb;
	uint16_t total = ch.cnp.total;

	uint32_t i;
	// get qp
	Ptr<RdmaQueuePair> qp = GetQp(ch.sip, udpport, qIndex);
	if (qp == NULL)
		std::cout << "ERROR: QCN NIC cannot find the flow\n";
	
	//whether save multiplier and bw
	if(ifCompare)
	{
		m_cc_mode = qp->m_cc_mode;
	}

	// get nic
	uint32_t nic_idx = GetNicIdxOfQp(qp);
	Ptr<QbbNetDevice> dev = m_nic[nic_idx].dev;

	if (qp->m_rate == 0)			//lazy initialization	
	{
		qp->m_rate = dev->GetDataRate();
		if (m_cc_mode == 1){
			qp->mlx.m_targetRate = dev->GetDataRate();
		}else if (m_cc_mode == 3){
			qp->hp.m_curRate = dev->GetDataRate();
			if (m_multipleRate){
				for (uint32_t i = 0; i < IntHeader::maxHop; i++)
					qp->hp.hopState[i].Rc = dev->GetDataRate();
			}
		}else if (m_cc_mode == 7){
			qp->tmly.m_curRate = dev->GetDataRate();
		}else if (m_cc_mode == 10){
			qp->hpccPint.m_curRate = dev->GetDataRate();
		}
	}
	return 0;
}

int RdmaHw::ReceiveAck(Ptr<Packet> p, CustomHeader &ch){
	uint16_t qIndex = ch.ack.pg;
	uint16_t port = ch.ack.dport;
	uint32_t seq = ch.ack.seq;
	uint8_t cnp = (ch.ack.flags >> qbbHeader::FLAG_CNP) & 1;
	int i;
	Ptr<RdmaQueuePair> qp = GetQp(ch.sip, port, qIndex);
	if (qp == NULL){
		std::cout << "ERROR: " << "node:" << m_node->GetId() << ' ' << (ch.l3Prot == 0xFC ? "ACK" : "NACK") << " NIC cannot find the flow\n";
		return 0;
	}

	//whether save multiplier and bw
	if(ifCompare)
	{
		m_cc_mode = qp->m_cc_mode;
	}

	uint32_t nic_idx = GetNicIdxOfQp(qp);
	
	Ptr<QbbNetDevice> dev = m_nic[nic_idx].dev;
	if (m_ack_interval == 0)
		std::cout << "ERROR: shouldn't receive ack\n";
	else {
		if (!m_backto0){
			qp->Acknowledge(seq);
		}else {
			uint32_t goback_seq = seq / m_chunk * m_chunk;
			qp->Acknowledge(goback_seq);
		}
		if (qp->IsFinished()){
			QpComplete(qp);
			
			
			//write the qp data of throughput into files
			if(calSingleThr)
			{
				hw_controller->SaveSingleThr(m_qpTime[qp], m_qpThr[qp]);
				hw_controller->flowNum = hw_controller->flowNum + 1;
			}

			//to compute the fairness of competition
			if(ifCompare)
			{
				hw_controller->SaveComp(m_qpTime[qp], m_qpThr[qp], qp->m_cc_mode);
			}
		}
	}
	if (ch.l3Prot == 0xFD) // NACK
		RecoverQueue(qp);

	// handle cnp
	if (cnp){
		if (m_cc_mode == 1){ // mlx version
			cnp_received_mlx(qp);
		} 
	}

	//save the send time
	if (m_cc_mode == 3){
		HandleAckHp(qp, p, ch);
		qp->hp.m_pkt_time.push_back(Now());
	}else if (m_cc_mode == 7){
		HandleAckTimely(qp, p, ch);
		qp->tmly.m_pkt_time.push_back(Now());
	}else if (m_cc_mode == 8){
		HandleAckDctcp(qp, p, ch);
		qp->dctcp.m_pkt_time.push_back(Now());
	}else if (m_cc_mode == 10){
		HandleAckHpPint(qp, p, ch);
	}else if (m_cc_mode == 11){
		HandleAckBbr(qp, p, ch);
		qp->bbr.m_pkt_time.push_back(Now());
	}
	// ACK may advance the on-the-fly window, allowing more packets to send
	dev->TriggerTransmit();

	return 0;
}

int RdmaHw::Receive(Ptr<Packet> p, CustomHeader &ch){
	if (ch.l3Prot == 0x11){ // UDP
		ReceiveUdp(p, ch);
	}else if (ch.l3Prot == 0xFF){ // CNP
		ReceiveCnp(p, ch);
	}else if (ch.l3Prot == 0xFD){ // NACK
		ReceiveAck(p, ch);
	}else if (ch.l3Prot == 0xFC){ // ACK
		ReceiveAck(p, ch);
	}
	return 0;
}

int RdmaHw::ReceiverCheckSeq(uint32_t seq, Ptr<RdmaRxQueuePair> q, uint32_t size){
	uint32_t expected = q->ReceiverNextExpectedSeq;
	if (seq == expected){
		q->ReceiverNextExpectedSeq = expected + size;
		if (q->ReceiverNextExpectedSeq >= q->m_milestone_rx){
			q->m_milestone_rx += m_ack_interval;
			return 1; //Generate ACK
		}else if (q->ReceiverNextExpectedSeq % m_chunk == 0){
			return 1;
		}else {
			return 5;
		}
	} else if (seq > expected) {
		// Generate NACK
		if (Simulator::Now() >= q->m_nackTimer || q->m_lastNACK != expected){
			q->m_nackTimer = Simulator::Now() + MicroSeconds(m_nack_interval);
			q->m_lastNACK = expected;
			if (m_backto0){
				q->ReceiverNextExpectedSeq = q->ReceiverNextExpectedSeq / m_chunk*m_chunk;
			}
			return 2;
		}else
			return 4;
	}else {
		// Duplicate. 
		return 3;
	}
}
void RdmaHw::AddHeader (Ptr<Packet> p, uint16_t protocolNumber){
	PppHeader ppp;
	ppp.SetProtocol (EtherToPpp (protocolNumber));
	p->AddHeader (ppp);
}
uint16_t RdmaHw::EtherToPpp (uint16_t proto){
	switch(proto){
		case 0x0800: return 0x0021;   //IPv4
		case 0x86DD: return 0x0057;   //IPv6
		default: NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
	}
	return 0;
}

void RdmaHw::RecoverQueue(Ptr<RdmaQueuePair> qp){
	qp->snd_nxt = qp->snd_una;
}

void RdmaHw::QpComplete(Ptr<RdmaQueuePair> qp){
	NS_ASSERT(!m_qpCompleteCallback.IsNull());
	if (m_cc_mode == 1){
		Simulator::Cancel(qp->mlx.m_eventUpdateAlpha);
		Simulator::Cancel(qp->mlx.m_eventDecreaseRate);
		Simulator::Cancel(qp->mlx.m_rpTimer);
	}

	// This callback will log info
	// It may also delete the rxQp on the receiver
	m_qpCompleteCallback(qp);

	qp->m_notifyAppFinish();

	// delete the qp
	//DeleteQueuePair(qp);
}

void RdmaHw::SetLinkDown(Ptr<QbbNetDevice> dev){
	printf("RdmaHw: node:%u a link down\n", m_node->GetId());
}

void RdmaHw::AddTableEntry(Ipv4Address &dstAddr, uint32_t intf_idx){
	uint32_t dip = dstAddr.Get();
	m_rtTable[dip].push_back(intf_idx);
}

void RdmaHw::ClearTable(){
	m_rtTable.clear();
}

void RdmaHw::RedistributeQp(){
	// clear old qpGrp
	for (uint32_t i = 0; i < m_nic.size(); i++){
		if (m_nic[i].dev == NULL)
			continue;
		m_nic[i].qpGrp->Clear();
	}

	// redistribute qp
	for (auto &it : m_qpMap){
		Ptr<RdmaQueuePair> qp = it.second;
		uint32_t nic_idx = GetNicIdxOfQp(qp);
		m_nic[nic_idx].qpGrp->AddQp(qp);
		// Notify Nic
		m_nic[nic_idx].dev->ReassignedQp(qp);
	}
}

Ptr<Packet> RdmaHw::GetNxtPacket(Ptr<RdmaQueuePair> qp){
	uint32_t payload_size = qp->GetBytesLeft();
	if (m_mtu < payload_size)
		payload_size = m_mtu;
	Ptr<Packet> p = Create<Packet> (payload_size);
	// add SeqTsHeader
	SeqTsHeader seqTs;
	seqTs.SetSeq (qp->snd_nxt);
	seqTs.SetPG (qp->m_pg);
	p->AddHeader (seqTs);
	// add udp header
	UdpHeader udpHeader;
	udpHeader.SetDestinationPort (qp->dport);
	udpHeader.SetSourcePort (qp->sport);
	p->AddHeader (udpHeader);
	// add ipv4 header
	Ipv4Header ipHeader;
	ipHeader.SetSource (qp->sip);
	ipHeader.SetDestination (qp->dip);
	ipHeader.SetProtocol (0x11);
	ipHeader.SetPayloadSize (p->GetSize());
	ipHeader.SetTtl (64);
	ipHeader.SetTos (0);
	ipHeader.SetIdentification (qp->m_ipid);
	p->AddHeader(ipHeader);
	// add ppp header
	PppHeader ppp;
	ppp.SetProtocol (0x0021); // EtherToPpp(0x800), see point-to-point-net-device.cc
	p->AddHeader (ppp);

	// update state
	qp->snd_nxt += payload_size;
	qp->m_ipid++;

	// return
	return p;
	
}

void RdmaHw::PktSent(Ptr<RdmaQueuePair> qp, Ptr<Packet> pkt, Time interframeGap){
	qp->lastPktSize = pkt->GetSize();
	
	//add priority tag
    Ipv4PriorityTag ipv4PriorityTag;
	uint32_t type=0;
	if(qp->m_size < 20000)  //20%
	{
		type = 0;
	}
	else if(qp->m_size >=20000 and qp->m_size <50000) //40%
	{
		type = 1;
	}
	else
	{
		type = 2;
	}
	
	ipv4PriorityTag.SetPriorityTag(type);
    pkt->AddByteTag (ipv4PriorityTag);

	UpdateNextAvail(qp, interframeGap, pkt->GetSize());
}

void RdmaHw::UpdateNextAvail(Ptr<RdmaQueuePair> qp, Time interframeGap, uint32_t pkt_size){
	Time sendingTime;
	if (m_rateBound)
		sendingTime = interframeGap + Seconds(qp->m_rate.CalculateTxTime(pkt_size));
	else
		sendingTime = interframeGap + Seconds(qp->m_max_rate.CalculateTxTime(pkt_size));
	qp->m_nextAvail = Simulator::Now() + sendingTime;
}

void RdmaHw::ChangeRate(Ptr<RdmaQueuePair> qp, DataRate new_rate){
	#if 1
	Time sendingTime = Seconds(qp->m_rate.CalculateTxTime(qp->lastPktSize));
	Time new_sendintTime = Seconds(new_rate.CalculateTxTime(qp->lastPktSize));
	qp->m_nextAvail = qp->m_nextAvail + new_sendintTime - sendingTime;
	// update nic's next avail event
	uint32_t nic_idx = GetNicIdxOfQp(qp);
	m_nic[nic_idx].dev->UpdateNextAvail(qp->m_nextAvail);
	#endif

	// change to new rate
	qp->m_rate = new_rate;
}

#define PRINT_LOG 0
/******************************
 * Mellanox's version of DCQCN
 *****************************/
void RdmaHw::UpdateAlphaMlx(Ptr<RdmaQueuePair> q){
	#if PRINT_LOG
	//std::cout << Simulator::Now() << " alpha update:" << m_node->GetId() << ' ' << q->mlx.m_alpha << ' ' << (int)q->mlx.m_alpha_cnp_arrived << '\n';
	//printf("%lu alpha update: %08x %08x %u %u %.6lf->", Simulator::Now().GetTimeStep(), q->sip.Get(), q->dip.Get(), q->sport, q->dport, q->mlx.m_alpha);
	#endif
	if (q->mlx.m_alpha_cnp_arrived){
		q->mlx.m_alpha = (1 - m_g)*q->mlx.m_alpha + m_g; 	//binary feedback
	}else {
		q->mlx.m_alpha = (1 - m_g)*q->mlx.m_alpha; 	//binary feedback
	}
	#if PRINT_LOG
	//printf("%.6lf\n", q->mlx.m_alpha);
	#endif
	q->mlx.m_alpha_cnp_arrived = false; // clear the CNP_arrived bit
	ScheduleUpdateAlphaMlx(q);
}
void RdmaHw::ScheduleUpdateAlphaMlx(Ptr<RdmaQueuePair> q){
	q->mlx.m_eventUpdateAlpha = Simulator::Schedule(MicroSeconds(m_alpha_resume_interval), &RdmaHw::UpdateAlphaMlx, this, q);
}

void RdmaHw::cnp_received_mlx(Ptr<RdmaQueuePair> q){
	q->mlx.m_alpha_cnp_arrived = true; // set CNP_arrived bit for alpha update
	q->mlx.m_decrease_cnp_arrived = true; // set CNP_arrived bit for rate decrease
	if (q->mlx.m_first_cnp){
		// init alpha
		q->mlx.m_alpha = 1;
		q->mlx.m_alpha_cnp_arrived = false;
		// schedule alpha update
		ScheduleUpdateAlphaMlx(q);
		// schedule rate decrease
		ScheduleDecreaseRateMlx(q, 1); // add 1 ns to make sure rate decrease is after alpha update
		// set rate on first CNP
		q->mlx.m_targetRate = q->m_rate = m_rateOnFirstCNP * q->m_rate;
		q->mlx.m_first_cnp = false;
	}
}

void RdmaHw::CheckRateDecreaseMlx(Ptr<RdmaQueuePair> q){
	ScheduleDecreaseRateMlx(q, 0);
	if (q->mlx.m_decrease_cnp_arrived){
		#if PRINT_LOG
		printf("%lu rate dec: %08x %08x %u %u (%0.3lf %.3lf)->", Simulator::Now().GetTimeStep(), q->sip.Get(), q->dip.Get(), q->sport, q->dport, q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
		#endif
		bool clamp = true;
		if (!m_EcnClampTgtRate){
			if (q->mlx.m_rpTimeStage == 0)
				clamp = false;
		}
		if (clamp)
			q->mlx.m_targetRate = q->m_rate;
		q->m_rate = std::max(m_minRate, q->m_rate * (1 - q->mlx.m_alpha / 2));
		// reset rate increase related things
		q->mlx.m_rpTimeStage = 0;
		q->mlx.m_decrease_cnp_arrived = false;
		Simulator::Cancel(q->mlx.m_rpTimer);
		q->mlx.m_rpTimer = Simulator::Schedule(MicroSeconds(m_rpgTimeReset), &RdmaHw::RateIncEventTimerMlx, this, q);
		#if PRINT_LOG
		printf("(%.3lf %.3lf)\n", q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
		#endif
	}
}
void RdmaHw::ScheduleDecreaseRateMlx(Ptr<RdmaQueuePair> q, uint32_t delta){
	q->mlx.m_eventDecreaseRate = Simulator::Schedule(MicroSeconds(m_rateDecreaseInterval) + NanoSeconds(delta), &RdmaHw::CheckRateDecreaseMlx, this, q);
}

void RdmaHw::RateIncEventTimerMlx(Ptr<RdmaQueuePair> q){
	q->mlx.m_rpTimer = Simulator::Schedule(MicroSeconds(m_rpgTimeReset), &RdmaHw::RateIncEventTimerMlx, this, q);
	RateIncEventMlx(q);
	q->mlx.m_rpTimeStage++;
}
void RdmaHw::RateIncEventMlx(Ptr<RdmaQueuePair> q){
	// check which increase phase: fast recovery, active increase, hyper increase
	if (q->mlx.m_rpTimeStage < m_rpgThreshold){ // fast recovery
		FastRecoveryMlx(q);
	}else if (q->mlx.m_rpTimeStage == m_rpgThreshold){ // active increase
		ActiveIncreaseMlx(q);
	}else { // hyper increase
		HyperIncreaseMlx(q);
	}
}

void RdmaHw::FastRecoveryMlx(Ptr<RdmaQueuePair> q){
	#if PRINT_LOG
	printf("%lu fast recovery: %08x %08x %u %u (%0.3lf %.3lf)->", Simulator::Now().GetTimeStep(), q->sip.Get(), q->dip.Get(), q->sport, q->dport, q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
	#endif
	q->m_rate = (q->m_rate / 2) + (q->mlx.m_targetRate / 2);
	#if PRINT_LOG
	printf("(%.3lf %.3lf)\n", q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
	#endif
}
void RdmaHw::ActiveIncreaseMlx(Ptr<RdmaQueuePair> q){
	#if PRINT_LOG
	printf("%lu active inc: %08x %08x %u %u (%0.3lf %.3lf)->", Simulator::Now().GetTimeStep(), q->sip.Get(), q->dip.Get(), q->sport, q->dport, q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
	#endif
	// get NIC
	uint32_t nic_idx = GetNicIdxOfQp(q);
	Ptr<QbbNetDevice> dev = m_nic[nic_idx].dev;
	// increate rate
	q->mlx.m_targetRate += m_rai;
	if (q->mlx.m_targetRate > dev->GetDataRate())
		q->mlx.m_targetRate = dev->GetDataRate();
	q->m_rate = (q->m_rate / 2) + (q->mlx.m_targetRate / 2);
	#if PRINT_LOG
	printf("(%.3lf %.3lf)\n", q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
	#endif
}
void RdmaHw::HyperIncreaseMlx(Ptr<RdmaQueuePair> q){
	#if PRINT_LOG
	printf("%lu hyper inc: %08x %08x %u %u (%0.3lf %.3lf)->", Simulator::Now().GetTimeStep(), q->sip.Get(), q->dip.Get(), q->sport, q->dport, q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
	#endif
	// get NIC
	uint32_t nic_idx = GetNicIdxOfQp(q);
	Ptr<QbbNetDevice> dev = m_nic[nic_idx].dev;
	// increate rate
	q->mlx.m_targetRate += m_rhai;
	if (q->mlx.m_targetRate > dev->GetDataRate())
		q->mlx.m_targetRate = dev->GetDataRate();
	q->m_rate = (q->m_rate / 2) + (q->mlx.m_targetRate / 2);
	#if PRINT_LOG
	printf("(%.3lf %.3lf)\n", q->mlx.m_targetRate.GetBitRate() * 1e-9, q->m_rate.GetBitRate() * 1e-9);
	#endif
}

/***********************
 * High Precision CC
 ***********************/
void RdmaHw::HandleAckHp(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch){
	uint32_t ack_seq = ch.ack.seq;
	// update rate
	if (ack_seq > qp->hp.m_lastUpdateSeq){ // if full RTT feedback is ready, do full update
		UpdateRateHp(qp, p, ch, false);
	}else{ // do fast react
		FastReactHp(qp, p, ch);
	}
}

void RdmaHw::UpdateRateHp(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch, bool fast_react){

	uint32_t next_seq = qp->snd_nxt;
	bool print = !fast_react || true;
	if (qp->hp.m_lastUpdateSeq == 0){ // first RTT
		qp->hp.m_lastUpdateSeq = next_seq;
		// store INT
		IntHeader &ih = ch.ack.ih;
		NS_ASSERT(ih.nhop <= IntHeader::maxHop);
		for (uint32_t i = 0; i < ih.nhop; i++)
			qp->hp.hop[i] = ih.hop[i];
		#if PRINT_LOG
		if (print){
			printf("%lu %s %08x %08x %u %u [%u,%u,%u]", Simulator::Now().GetTimeStep(), fast_react? "fast" : "update", qp->sip.Get(), qp->dip.Get(), qp->sport, qp->dport, qp->hp.m_lastUpdateSeq, ch.ack.seq, next_seq);
			for (uint32_t i = 0; i < ih.nhop; i++)
				printf(" %u %lu %lu", ih.hop[i].GetQlen(), ih.hop[i].GetBytes(), ih.hop[i].GetTime());
			printf("\n");
		}
		#endif
	}else {
		//get the throughput of single flows
		if(calSingleThr || ifCompare)
		{
			std::vector<uint64_t> tmp_t_thr = getSingleThr(qp);
			if(tmp_t_thr[1] != 0)
			{
				std::vector<double> tmp_time = m_qpTime[qp];
				std::vector<double> tmp_thr = m_qpThr[qp];
				tmp_time.push_back(tmp_t_thr[0]);
				tmp_thr.push_back(double(tmp_t_thr[1]/1e9)); //in Gbps
				m_qpTime[qp] = tmp_time;
				m_qpThr[qp] = tmp_thr;
			}
		}

		//for rtt change
		uint64_t m_tmpRtt = qp->m_baseRtt;

		// check packet INT
		IntHeader &ih = ch.ack.ih;
		if (ih.nhop <= IntHeader::maxHop){
			double max_c = 0;
			bool inStable = false;
			#if PRINT_LOG
			if (print)
				printf("%lu %s %08x %08x %u %u [%u,%u,%u]", Simulator::Now().GetTimeStep(), fast_react? "fast" : "update", qp->sip.Get(), qp->dip.Get(), qp->sport, qp->dport, qp->hp.m_lastUpdateSeq, ch.ack.seq, next_seq);
			#endif
			// check each hop
			double U = 0;

			//test myu
			double myU = 0;
			double myrate = 0;
			double myqlen = 0;

			uint64_t dt = 0;
			bool updated[IntHeader::maxHop] = {false}, updated_any = false;
			NS_ASSERT(ih.nhop <= IntHeader::maxHop);
			for (uint32_t i = 0; i < ih.nhop; i++){
				if (m_sampleFeedback){
					if (ih.hop[i].GetQlen() == 0 and fast_react)
						continue;
				}
				updated[i] = updated_any = true;
				#if PRINT_LOG
				if (print)
					printf(" %u(%u) %lu(%lu) %lu(%lu)", ih.hop[i].GetQlen(), qp->hp.hop[i].GetQlen(), ih.hop[i].GetBytes(), qp->hp.hop[i].GetBytes(), ih.hop[i].GetTime(), qp->hp.hop[i].GetTime());
				#endif
				uint64_t tau = ih.hop[i].GetTimeDelta(qp->hp.hop[i]);
				double duration = tau * 1e-9;
				double txRate = (ih.hop[i].GetBytesDelta(qp->hp.hop[i])) * 8 / duration;

				double u;

				//test myu
				double myu;

				//rtt change 
				uint64_t my_minRTT;
				double multiplier = 1;
				if(hw_controller->myrchange == 1)
				{
					//for ccdfs
					if(hw_controller->myrdyna == 1)
					{
						u = txRate / ih.hop[i].GetLineRate() + (double)std::min(ih.hop[i].GetQlen(), qp->hp.hop[i].GetQlen()) * qp->m_max_rate.GetBitRate() / ih.hop[i].GetLineRate() /qp->m_win/hw_controller->m_rtt_multiplier;
						qp->m_baseRtt = m_tmpRtt*hw_controller->m_rtt_multiplier;
						myu = txRate / ih.hop[i].GetLineRate() ;

						int t_size = qp->myRTTs.size();
						if(t_size>0)
						{	
							my_minRTT = qp->myRTTs[0];
							for(int i=0; i<t_size; i++)
							{
								if(qp->myRTTs[i] < my_minRTT)
								{
									my_minRTT = qp->myRTTs[i];
								}
							}
							multiplier = double(my_minRTT)/m_tmpRtt;
						}
						//compare the multiplier
						if(ifSaveMul)
						{
							hw_controller->SaveRtt(multiplier, hw_controller->m_rtt_multiplier);
						}
						multiplier = hw_controller->m_rtt_multiplier;
					}
				}

				qp->m_baseRtt = m_tmpRtt * multiplier;

				//bw change
				uint64_t m_curB = ih.hop[i].GetLineRate();
				if(hw_controller->mybchange == 1)
				{
					//for hpcc and ccdsf bcase>4
					if((hw_controller->mybdyna == 0) || ((hw_controller->mybdyna == 1) and (hw_controller->mybcase > 4)))
					{
						m_curB = 100000000000; //100Gbps
						qp->m_max_rate = m_curB;
					}
					else
					{
						//test probeBW
						int curSize = (qp->dynaU).size();
						bool changeB = false;
						if(curSize == 5)
						{
							double u1 = (qp->dynaU)[0][1];
							double u2 = (qp->dynaU)[1][1];
							double u3 = (qp->dynaU)[2][1];
							double u4 = (qp->dynaU)[3][1];
							double u5 = (qp->dynaU)[4][1];
							double diffu1 = (qp->dynaU)[0][2];
							double diffu2 = (qp->dynaU)[1][2];
							double diffu3 = (qp->dynaU)[2][2];
							double diffu4 = (qp->dynaU)[3][2];
							double diffu5 = (qp->dynaU)[4][2];
							if((u1<0.8) and (u2<0.8) and (u3<0.8) and (u4<0.8) and (u5<0.8)\
							and (diffu1<0.05) and (diffu2<0.05) and (diffu3<0.05) and (diffu4<0.05) and (diffu5<0.05))
							{
								changeB = true;
							}
							//if((u1>1.2) and (u2>1.2) and (u3>1.2) and (u4>1.2) and (u5>1.2)\
							and (diffu1>0.05) and (diffu2>0.05) and (diffu3>0.05) and (diffu4>0.05) and (diffu5>0.05))
							if((u1>1.2) and (u2>1.2) and (u3>1.2) and (u4>1.2) and (u5>1.2))
							{
								changeB = true;
							}
						}
						double sum_u = 0;
						double sum_diff = 0;
						double avg_u;
						double avg_diff;
						
						if(ih.hop[i].GetLineRate() != 100000000000)
						{
							if(changeB)
							{
								uint64_t sum = 0;
								uint64_t max = 0;
								uint64_t max_curB = 0;
								uint64_t curB = 0;
								for(int i=0; i<curSize; i++)
								{
									curB = uint64_t((qp->dynaU)[i][0]);
									sum = sum + curB;
								}

								max = sum/curSize;
								qp->curB.push_back(max);

								int size = qp->curB.size();

								//erase early data
								if(size > 100)
								{
									for(int i=0; i<size-100; i++)
									{
										(qp->curB).erase((qp->curB).begin()+i);
									}
								}
								if(size>1)
								{
									for(int i=0; i<size; i++)
									{
										if(qp->curB[i] > max_curB)
										{
											max_curB = qp->curB[i];
										}
									}
									curB = max_curB;
								}
								else
								{
									curB = max;
								}

								qp->lowerB = curB;
								m_curB = curB;
							}
							else
							{
								if(qp->lowerB == 0)
								{
									qp->lowerB = ih.hop[i].GetLineRate();
								}
								m_curB = qp->lowerB;
							}

							//compare the predicted bw and real bw 
							if(ifSaveBw)
							{
								hw_controller->SaveBw(m_curB, ih.hop[i].GetLineRate());
							}

						}
						else
						{
							m_curB = ih.hop[i].GetLineRate();
						}
					}
				}

				u = txRate / m_curB + (double)std::min(ih.hop[i].GetQlen(), qp->hp.hop[i].GetQlen()) * qp->m_max_rate.GetBitRate() / m_curB /qp->m_win/multiplier;
				myu = txRate / m_curB;

	
				#if PRINT_LOG
				if (print)
					printf(" %.3lf %.3lf", txRate, u);
				#endif

				if (!m_multipleRate){
					// for aggregate (single R)
					if (u > U){
						U = u;
						dt = tau;
					}
					if(myu > myU){
						myU = myu;
						myrate = txRate;
						myqlen = std::min(ih.hop[i].GetQlen(), qp->hp.hop[i].GetQlen());
					}
					//check whether store base RTT
					if(std::min(ih.hop[i].GetQlen(), qp->hp.hop[i].GetQlen())>10)
					{
						qp->ifSaveRtt = false;
					}
				}else {
					// for per hop (per hop R)
					if (tau > qp->m_baseRtt)
						tau = qp->m_baseRtt;
					qp->hp.hopState[i].u = (qp->hp.hopState[i].u * (qp->m_baseRtt - tau) + u * tau) / double(qp->m_baseRtt);
				}
				qp->hp.hop[i] = ih.hop[i];
			}

			//save the data of qlen, u
			hw_controller->SaveData(U, myrate/1e9, myqlen);

			//check whether store base RTT
			uint64_t rtt = Simulator::Now().GetTimeStep() - ch.ack.ih.ts;
			if(qp->ifSaveRtt and hw_controller->myrchange == 1 and hw_controller->myrdyna == 1)
			{
				qp->myRTTs.push_back(rtt);
				int size = qp->myRTTs.size();
				if(size>200)
				{
					for(int i=0; i<size-200; i++)
					{
						(qp->myRTTs).erase((qp->myRTTs).begin()+i);
					}
				}
			}

			//check whether save U for bw change
			int mysize = (qp->dynaU).size();
			std::vector<double> mytmp;
			if(hw_controller->mybchange == 1 and hw_controller->mybdyna== 1)
			{
				if(mysize == 0)
				{
					mytmp.push_back(myrate);
					mytmp.push_back(myU);
					//mytmp.push_back(U);
					mytmp.push_back(0);
					(qp->dynaU).push_back(mytmp);
				}
				else
				{
					double lastU = (qp->dynaU)[mysize-1][1];
					if(lastU != 0)
					{
						double new_diff_u = (myU-lastU)/lastU;
						mytmp.push_back(myrate);
						mytmp.push_back(myU);
						//mytmp.push_back(U);
						mytmp.push_back(new_diff_u);
						(qp->dynaU).push_back(mytmp);
						if(mysize == 5)
						{
							(qp->dynaU).erase((qp->dynaU).begin());
						}
					}
				}
			}
			

			DataRate new_rate;
			int32_t new_incStage;
			DataRate new_rate_per_hop[IntHeader::maxHop];
			int32_t new_incStage_per_hop[IntHeader::maxHop];
			if (!m_multipleRate){
				// for aggregate (single R)
				if (updated_any){
					if (dt > qp->m_baseRtt)
						dt = qp->m_baseRtt;
					qp->hp.u = (qp->hp.u * (qp->m_baseRtt - dt) + U * dt) / double(qp->m_baseRtt);

					//varied target U
					if(varU)
					{
						//for multi-queues
						//get priority tag
						int prioTag;
						Ipv4PriorityTag ipv4PriorityTag;
						if (p->FindFirstMatchingByteTag (ipv4PriorityTag)) 
						{
							prioTag = ipv4PriorityTag.GetPriorityTag()+1;
						}
						if(prioTag == 1)
						{
							max_c = qp->hp.u / small_u;
						}
						if(prioTag == 2)
						{
							max_c = qp->hp.u / medium_u;
						}
						else
						{
							max_c = qp->hp.u / large_u;
						}
					}
					else
					{
						max_c = qp->hp.u / m_targetUtil;
					}

					if (max_c >= 1 || qp->hp.m_incStage >= m_miThresh){
						new_rate = qp->hp.m_curRate / max_c + m_rai;	
						new_incStage = 0;
					}else{
						new_rate = qp->hp.m_curRate + m_rai;
						new_incStage = qp->hp.m_incStage+1;
					}
					if (new_rate < m_minRate)
						new_rate = m_minRate;
					if (new_rate > qp->m_max_rate)
						new_rate = qp->m_max_rate;
					#if PRINT_LOG
					if (print)
						printf(" u=%.6lf U=%.3lf dt=%u max_c=%.3lf", qp->hp.u, U, dt, max_c);
					#endif
					#if PRINT_LOG
					if (print)
						printf(" rate:%.3lf->%.3lf\n", qp->hp.m_curRate.GetBitRate()*1e-9, new_rate.GetBitRate()*1e-9);
					#endif
				}
			}else{
				// for per hop (per hop R)
				new_rate = qp->m_max_rate;
				for (uint32_t i = 0; i < ih.nhop; i++){
					if (updated[i]){

						//varied U
						double c;
						if(varU)
						{
							//for multi-queues
							//get priority tag
							int prioTag;
							Ipv4PriorityTag ipv4PriorityTag;
							if (p->FindFirstMatchingByteTag (ipv4PriorityTag)) 
							{
								prioTag = ipv4PriorityTag.GetPriorityTag()+1;
							}
							if(prioTag == 1)
							{
								c = qp->hp.hopState[i].u / small_u;
							}
							if(prioTag == 2)
							{
								c = qp->hp.hopState[i].u / medium_u;
							}
							else
							{
								c = qp->hp.hopState[i].u / large_u;
							}
						}
						else
						{
							c = qp->hp.hopState[i].u / m_targetUtil;
						}

						if (c >= 1 || qp->hp.hopState[i].incStage >= m_miThresh){
							new_rate_per_hop[i] = qp->hp.hopState[i].Rc / c + m_rai;
							new_incStage_per_hop[i] = 0;
						}else{
							new_rate_per_hop[i] = qp->hp.hopState[i].Rc + m_rai;
							new_incStage_per_hop[i] = qp->hp.hopState[i].incStage+1;
						}
						// bound rate
						if (new_rate_per_hop[i] < m_minRate)
							new_rate_per_hop[i] = m_minRate;
						if (new_rate_per_hop[i] > qp->m_max_rate)
							new_rate_per_hop[i] = qp->m_max_rate;
						// find min new_rate
						if (new_rate_per_hop[i] < new_rate)
							new_rate = new_rate_per_hop[i];
						#if PRINT_LOG
						if (print)
							printf(" [%u]u=%.6lf c=%.3lf", i, qp->hp.hopState[i].u, c);
						#endif
						#if PRINT_LOG
						if (print)
							printf(" %.3lf->%.3lf", qp->hp.hopState[i].Rc.GetBitRate()*1e-9, new_rate.GetBitRate()*1e-9);
						#endif
					}else{
						if (qp->hp.hopState[i].Rc < new_rate)
							new_rate = qp->hp.hopState[i].Rc;
					}
				}
				#if PRINT_LOG
				printf("\n");
				#endif
			}
			if (updated_any)
				ChangeRate(qp, new_rate);
			if (!fast_react){
				if (updated_any){
					qp->hp.m_curRate = new_rate;
					qp->hp.m_incStage = new_incStage;
				}
				if (m_multipleRate){
					// for per hop (per hop R)
					for (uint32_t i = 0; i < ih.nhop; i++){
						if (updated[i]){
							qp->hp.hopState[i].Rc = new_rate_per_hop[i];
							qp->hp.hopState[i].incStage = new_incStage_per_hop[i];
						}
					}
				}
			}
		}
		if (!fast_react){
			if (next_seq > qp->hp.m_lastUpdateSeq)
				qp->hp.m_lastUpdateSeq = next_seq; //+ rand() % 2 * m_mtu;
		}
	    
		//for rtt change
		qp->m_baseRtt = m_tmpRtt;
	}
}

void RdmaHw::FastReactHp(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch){
	if (m_fast_react)
		UpdateRateHp(qp, p, ch, true);
}

/**********************
 * TIMELY
 *********************/
void RdmaHw::HandleAckTimely(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch){
	uint32_t ack_seq = ch.ack.seq;
	// update rate
	if (ack_seq > qp->tmly.m_lastUpdateSeq){ // if full RTT feedback is ready, do full update
		UpdateRateTimely(qp, p, ch, false);
	}else{ // do fast react
		FastReactTimely(qp, p, ch);
	}
}
void RdmaHw::UpdateRateTimely(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch, bool us){
	uint32_t next_seq = qp->snd_nxt;
	uint64_t rtt = Simulator::Now().GetTimeStep() - ch.ack.ih.ts;
	bool print = !us;

	if (qp->tmly.m_lastUpdateSeq == 0){ // first RTT
		qp->tmly.m_lastUpdateSeq = next_seq;

		//to get the queue length, throughput, utilization ratio
		IntHeader &ih = ch.ack.ih;
		for (uint32_t i = 0; i < ih.nhop; i++)
		{
			qp->tmly.hop[i] = ih.hop[i];
		}
		return;
	}

	if (qp->tmly.m_lastUpdateSeq != 0){ // not first RTT
		//get the throughput of single flows
		if(calSingleThr || ifCompare)
		{
			std::vector<uint64_t> tmp_t_thr = getSingleThr(qp);
			if(tmp_t_thr[1] != 0)
			{
				std::vector<double> tmp_time = m_qpTime[qp];
				std::vector<double> tmp_thr = m_qpThr[qp];
				tmp_time.push_back(tmp_t_thr[0]);
				tmp_thr.push_back(double(tmp_t_thr[1]/1e9)); //in Gbps
				m_qpTime[qp] = tmp_time;
				m_qpThr[qp] = tmp_thr;
				//std::cout << double(tmp_t_thr[1]/1e9) << std::endl;
			}
		}

		//to get the queue length, throughput, utilization ratio
		IntHeader &ih = ch.ack.ih;
		for (uint32_t i = 0; i < ih.nhop; i++)
		{
			if (ih.hop[i].GetQlen() == 0)
			{
				continue;
			}
			uint64_t tau = ih.hop[i].GetTimeDelta(qp->tmly.hop[i]);
			double duration = tau * 1e-9;
			double txRate = (ih.hop[i].GetBytesDelta(qp->tmly.hop[i])) * 8 / duration;
			double u = txRate / ih.hop[i].GetLineRate() + (double)std::min(ih.hop[i].GetQlen(), qp->tmly.hop[i].GetQlen()) * qp->m_max_rate.GetBitRate() / ih.hop[i].GetLineRate() /qp->m_win;
			//std::cout << txRate << std::endl;
			qp->tmly.hop[i] = ih.hop[i];
			hw_controller->SaveData(txRate / ih.hop[i].GetLineRate(), txRate/1e9, ih.hop[i].GetQlen());
		}

		int64_t new_rtt_diff = (int64_t)rtt - (int64_t)qp->tmly.lastRtt;
		double rtt_diff = (1 - m_tmly_alpha) * qp->tmly.rttDiff + m_tmly_alpha * new_rtt_diff;
		double gradient = rtt_diff / m_tmly_minRtt;
		bool inc = false;
		double c = 0;
		#if PRINT_LOG
		if (print)
			printf("%lu node:%u rtt:%lu rttDiff:%.0lf gradient:%.3lf rate:%.3lf", Simulator::Now().GetTimeStep(), m_node->GetId(), rtt, rtt_diff, gradient, qp->tmly.m_curRate.GetBitRate() * 1e-9);
		#endif
		if (rtt < m_tmly_TLow){
			inc = true;
		}else if (rtt > m_tmly_THigh){
			c = 1 - m_tmly_beta * (1 - (double)m_tmly_THigh / rtt);
			inc = false;
		}else if (gradient <= 0){
			inc = true;
		}else{
			c = 1 - m_tmly_beta * gradient;
			if (c < 0)
				c = 0;
			inc = false;
		}
		if (inc){
			if (qp->tmly.m_incStage < 5){
				qp->m_rate = qp->tmly.m_curRate + m_rai;
			}else{
				qp->m_rate = qp->tmly.m_curRate + m_rhai;
			}
			if (qp->m_rate > qp->m_max_rate)
				qp->m_rate = qp->m_max_rate;
			if (!us){
				qp->tmly.m_curRate = qp->m_rate;
				qp->tmly.m_incStage++;
				qp->tmly.rttDiff = rtt_diff;
			}
		}else{
			qp->m_rate = std::max(m_minRate, qp->tmly.m_curRate * c); 
			if (!us){
				qp->tmly.m_curRate = qp->m_rate;
				qp->tmly.m_incStage = 0;
				qp->tmly.rttDiff = rtt_diff;
			}
		}
		#if PRINT_LOG
		if (print){
			printf(" %c %.3lf\n", inc? '^':'v', qp->m_rate.GetBitRate() * 1e-9);
		}
		#endif
	}
	if (!us && next_seq > qp->tmly.m_lastUpdateSeq){
		qp->tmly.m_lastUpdateSeq = next_seq;
		// update
		qp->tmly.lastRtt = rtt;
	}
}
void RdmaHw::FastReactTimely(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch){
}

/**********************
 * DCTCP
 *********************/
void RdmaHw::HandleAckDctcp(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch){
	uint32_t ack_seq = ch.ack.seq;
	uint8_t cnp = (ch.ack.flags >> qbbHeader::FLAG_CNP) & 1;
	bool new_batch = false;

	//get the throughput of single flows
	if(calSingleThr || ifCompare)
	{
		std::vector<uint64_t> tmp_t_thr = getSingleThr(qp);
		if(tmp_t_thr[1] != 0)
		{
			std::vector<double> tmp_time = m_qpTime[qp];
			std::vector<double> tmp_thr = m_qpThr[qp];
			tmp_time.push_back(tmp_t_thr[0]);
			tmp_thr.push_back(double(tmp_t_thr[1]/1e9)); //in Gbps
			m_qpTime[qp] = tmp_time;
			m_qpThr[qp] = tmp_thr;
			//std::cout << double(tmp_bw/1e9)<< std::endl;
		}
	}

	// update alpha
	qp->dctcp.m_ecnCnt += (cnp > 0);
	if (ack_seq > qp->dctcp.m_lastUpdateSeq){ // if full RTT feedback is ready, do alpha update
		#if PRINT_LOG
		printf("%lu %s %08x %08x %u %u [%u,%u,%u] %.3lf->", Simulator::Now().GetTimeStep(), "alpha", qp->sip.Get(), qp->dip.Get(), qp->sport, qp->dport, qp->dctcp.m_lastUpdateSeq, ch.ack.seq, qp->snd_nxt, qp->dctcp.m_alpha);
		#endif
		new_batch = true;
		if (qp->dctcp.m_lastUpdateSeq == 0){ // first RTT
			qp->dctcp.m_lastUpdateSeq = qp->snd_nxt;
			qp->dctcp.m_batchSizeOfAlpha = qp->snd_nxt / m_mtu + 1;

			//to get the queue length, throughput, utilization ratio
			IntHeader &ih = ch.ack.ih;
			for (uint32_t i = 0; i < ih.nhop; i++)
			{
				qp->dctcp.hop[i] = ih.hop[i];
			}
		}else {
			
			//to get the queue length, throughput, utilization ratio
			IntHeader &ih = ch.ack.ih;
			for (uint32_t i = 0; i < ih.nhop; i++)
			{
				if (ih.hop[i].GetQlen() == 0)
				{
					continue;
				}
				uint64_t tau = ih.hop[i].GetTimeDelta(qp->dctcp.hop[i]);
				double duration = tau * 1e-9;
				double txRate = (ih.hop[i].GetBytesDelta(qp->dctcp.hop[i])) * 8 / duration;
				double u = txRate / ih.hop[i].GetLineRate() + (double)std::min(ih.hop[i].GetQlen(), qp->dctcp.hop[i].GetQlen()) * qp->m_max_rate.GetBitRate() / ih.hop[i].GetLineRate() /qp->m_win;
				//std::cout << txRate << std::endl;
				qp->dctcp.hop[i] = ih.hop[i];
				hw_controller->SaveData(txRate / ih.hop[i].GetLineRate(), txRate/1e9, ih.hop[i].GetQlen());
			}
			

			double frac = std::min(1.0, double(qp->dctcp.m_ecnCnt) / qp->dctcp.m_batchSizeOfAlpha);
			qp->dctcp.m_alpha = (1 - m_g) * qp->dctcp.m_alpha + m_g * frac;
			qp->dctcp.m_lastUpdateSeq = qp->snd_nxt;
			qp->dctcp.m_ecnCnt = 0;
			qp->dctcp.m_batchSizeOfAlpha = (qp->snd_nxt - ack_seq) / m_mtu + 1;
			#if PRINT_LOG
			printf("%.3lf F:%.3lf", qp->dctcp.m_alpha, frac);
			#endif
		}
		#if PRINT_LOG
		printf("\n");
		#endif
	}

	// check cwr exit
	if (qp->dctcp.m_caState == 1){
		if (ack_seq > qp->dctcp.m_highSeq)
			qp->dctcp.m_caState = 0;
	}

	// check if need to reduce rate: ECN and not in CWR
	if (cnp && qp->dctcp.m_caState == 0){
		#if PRINT_LOG
		printf("%lu %s %08x %08x %u %u %.3lf->", Simulator::Now().GetTimeStep(), "rate", qp->sip.Get(), qp->dip.Get(), qp->sport, qp->dport, qp->m_rate.GetBitRate()*1e-9);
		#endif
		qp->m_rate = std::max(m_minRate, qp->m_rate * (1 - qp->dctcp.m_alpha / 2));
		#if PRINT_LOG
		printf("%.3lf\n", qp->m_rate.GetBitRate() * 1e-9);
		#endif
		qp->dctcp.m_caState = 1;
		qp->dctcp.m_highSeq = qp->snd_nxt;
	}

	// additive inc
	if (qp->dctcp.m_caState == 0 && new_batch)
		qp->m_rate = std::min(qp->m_max_rate, qp->m_rate + m_dctcp_rai);
}

/*********************
 * BBR
 ********************/
void RdmaHw::HandleAckBbr(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch){
	
	//to get the queue length, throughput, utilization ratio
	uint32_t next_seq = qp->snd_nxt;
	if (qp->bbr.m_lastUpdateSeq == 0){ // first RTT
		qp->bbr.m_lastUpdateSeq = next_seq;
		IntHeader &ih = ch.ack.ih;
		for (uint32_t i = 0; i < ih.nhop; i++)
		{
			qp->bbr.hop[i] = ih.hop[i];
		}
	}
	else
	{
		IntHeader &ih = ch.ack.ih;
		for (uint32_t i = 0; i < ih.nhop; i++)
		{
			if (ih.hop[i].GetQlen() == 0)
			{
				continue;
			}
			uint64_t tau = ih.hop[i].GetTimeDelta(qp->bbr.hop[i]);
			double duration = tau * 1e-9;
			double txRate = (ih.hop[i].GetBytesDelta(qp->bbr.hop[i])) * 8 / duration;
			double u = txRate / ih.hop[i].GetLineRate() + (double)std::min(ih.hop[i].GetQlen(), qp->bbr.hop[i].GetQlen()) * qp->m_max_rate.GetBitRate() / ih.hop[i].GetLineRate() /qp->m_win;
			//std::cout << txRate/1e9 << std::endl;
			qp->bbr.hop[i] = ih.hop[i];
			hw_controller->SaveData(txRate / ih.hop[i].GetLineRate(), txRate/1e9, ih.hop[i].GetQlen());
		}
		if (next_seq > qp->bbr.m_lastUpdateSeq)
		{
			qp->bbr.m_lastUpdateSeq = next_seq; //+ rand() % 2 * m_mtu;
		}
	}	


	uint32_t ack_seq = ch.ack.seq;
	uint8_t cnp = (ch.ack.flags >> qbbHeader::FLAG_CNP) & 1;
	

	Time now = Simulator::Now();
	// check cwr exit
	if (qp->bbr.m_caState == 1){
		if (ack_seq > qp->bbr.m_highSeq)
			qp->bbr.m_caState = 0;
	}

	// check if need to reduce rate: ECN and not in CWR
	if (cnp && qp->bbr.m_caState == 0){
		qp->m_rate = std::max(m_minRate, qp->m_rate/2);
		qp->bbr.m_caState = 1;
		qp->bbr.m_highSeq = qp->snd_nxt;
	}
	else
	{
		uint64_t bdp = updateTargetCwnd(qp);
		if(bdp>0)
		{
			uint32_t low_bdp = bdp * qp->bbr.m_cwnd_gain;
			qp->m_win = std::max(low_bdp, uint32_t(10000));
			//m_rate is not used
			DataRate tmp_bw = qp->m_max_rate;
			qp->m_rate = tmp_bw;  
		}
		else
		{
			qp->m_rate = m_minRate;
		}
	}

	//save rtt
	uint64_t rtt = now.GetTimeStep() - ch.ack.ih.ts;
	//in nano seconds
	if(rtt < getRTT(qp))
	{
		 qp->bbr.m_min_rtt_change = now;  
	}
	qp->bbr.m_rtt_window[now] = rtt;
	if(qp->bbr.m_rtt_window.size() == 1)
	{
		//to finish: whether to use rtt-based update or ack-based update
		update(qp);
	}

	//get the throughput of single flows
	if(calSingleThr || ifCompare)
	{
		std::vector<uint64_t> tmp_t_thr = getSingleThr(qp);
		if(tmp_t_thr[1] != 0)
		{
			std::vector<double> tmp_time = m_qpTime[qp];
			std::vector<double> tmp_thr = m_qpThr[qp];
			tmp_time.push_back(tmp_t_thr[0]);
			tmp_thr.push_back(double(tmp_t_thr[1]/1e9)); //in Gbps
			m_qpTime[qp] = tmp_time;
			m_qpThr[qp] = tmp_thr;
			//std::cout << double(tmp_bw/1e9)<< std::endl;
		}
	}
	else
	{
		//save bw
		int tmp_size = qp->bbr.m_pkt_time.size();
		if(tmp_size==0 || tmp_size==1)
		{
			return;
		}
		int tmp_count = 0;
		int i;
		for(i=tmp_size-1; ((now-qp->bbr.m_pkt_time[i]).GetNanoSeconds()<=rtt) and i >= 0; i--)
		{	
			tmp_count++;
		}
		if(i+1 == tmp_size-1)
		{
			return;
		}
	
		Time diff_time = qp->bbr.m_pkt_time[tmp_size-1]-qp->bbr.m_pkt_time[i+1];

		//remove rebundant data of before 3 rtts
		int tmp_remove = tmp_size - 3*tmp_count; 
		if(tmp_remove > 1)
		{
			for(int j=0; j<tmp_remove; j++)
			{
				qp->bbr.m_pkt_time.erase(qp->bbr.m_pkt_time.begin()+j);
			}
		}

		uint64_t bw_est = tmp_count * 8 * 1000 * 1e9/diff_time.GetNanoSeconds();//in bits
		bw_struct bw;
		bw.bw_est = bw_est;
		bw.time = now;
		qp->bbr.m_bw_window.push_back(bw);
	}
}

//save throughput of single flows
std::vector<uint64_t>
RdmaHw::getSingleThr(Ptr<RdmaQueuePair> qp){
	//whether save multiplier and bw
	if(ifCompare)
	{
		m_cc_mode = qp->m_cc_mode;
	}

	Time now = Simulator::Now();
	//save bw
	int tmp_size;
	std::vector<Time> tmp_pkt_time;
	std::vector<uint64_t> tmp;
	//hpcc or ccdfs
	if(m_cc_mode == 3)
	{
		tmp_size = qp->hp.m_pkt_time.size();
		tmp_pkt_time = qp->hp.m_pkt_time;
	}
	if(m_cc_mode == 7)
	{
		tmp_size = qp->tmly.m_pkt_time.size();
		tmp_pkt_time = qp->tmly.m_pkt_time;
	}
	if(m_cc_mode == 8)
	{
		tmp_size = qp->dctcp.m_pkt_time.size();
		tmp_pkt_time = qp->dctcp.m_pkt_time;
	}
	if(m_cc_mode == 11)
	{
		tmp_size = qp->bbr.m_pkt_time.size();
		tmp_pkt_time = qp->bbr.m_pkt_time;
	}

	if(tmp_size==0 || tmp_size==1)
	{
		tmp.push_back(0);
		tmp.push_back(0);
		return(tmp);
	}
	int tmp_count = 0;
	int i;
	for(i=tmp_size-1; i >= 0; i--)
	{	
		if((now - tmp_pkt_time[i]).GetNanoSeconds()>20*qp->m_baseRtt)
		{
			break;
		}
		tmp_count++;
	}
	if(i+1 == tmp_size-1)
	{
		tmp.push_back(0);
		tmp.push_back(0);
		return(tmp);
	}

	Time diff_time = tmp_pkt_time[tmp_size-1]-tmp_pkt_time[i+1];
	uint64_t bw_est = tmp_count * 8 * 1000 * 1e9/diff_time.GetNanoSeconds();//in bits
	tmp.push_back(now.GetNanoSeconds());
	tmp.push_back(bw_est);
	
	return(tmp);
}


uint64_t RdmaHw::getBDP(Ptr<RdmaQueuePair> qp) const{
	uint64_t rtt = getRTT(qp);
	uint64_t tmp_bw = getBW(qp);
	uint64_t bw = std::max(m_minRate.GetBitRate(), (tmp_bw));
	return (rtt*bw/(8*1e9));  //in bytes
}

uint64_t RdmaHw::getBW(Ptr<RdmaQueuePair> qp) const{
	uint64_t max_bw = 0;
	if(qp->bbr.m_bw_window.size()==0)
	{
		max_bw = 1;
	}
	else
	{
		for (auto it = qp->bbr.m_bw_window.begin(); it != qp->bbr.m_bw_window.end(); it++)
		{
			max_bw = std::max(max_bw, it->bw_est);
		}
	}
	return(max_bw);
}

uint64_t RdmaHw::getRTT(Ptr<RdmaQueuePair> qp) const{
	uint64_t min_rtt = 0;
	if(qp->bbr.m_rtt_window.size() == 0)
	{
		min_rtt = -1;
	}
	else
	{
		for(auto it = qp->bbr.m_rtt_window.begin(); it != qp->bbr.m_rtt_window.end(); it++)
		{
			if(it == qp->bbr.m_rtt_window.begin())
			{
				min_rtt = it->second;
			}
			min_rtt = std::min(min_rtt, it->second);
		}
	}
	return(min_rtt);
}

void RdmaHw::cullBWwindow(Ptr<RdmaQueuePair> qp){
	uint64_t bw = getBW(qp);
	if(bw == 1)
	{
		return;
	}
	uint64_t rtt = getRTT(qp);
	if(rtt < 0)
	{
		return;
	}
	Time now = Simulator::Now();
	Time time_delta = now - NanoSeconds(rtt * BW_WINDOW_TIME);
		// Erase any values that are too old.
  	auto it = qp->bbr.m_bw_window.begin();
  	while (it != qp->bbr.m_bw_window.end()) 
	{
		// Configured with either WALLCLOCK or PACKET time.                         // Use packet time.
		if (it -> time < time_delta)
		{
			it = qp->bbr.m_bw_window.erase(it);
		}
		else
		{
			it++;
		}
  }
}

void RdmaHw::cullRTTwindow(Ptr<RdmaQueuePair> qp){
	uint64_t rtt = getRTT(qp);
	if(rtt < 0)
	{
		return;
	}
	Time now = Simulator::Now();
	uint64_t delta = now.GetNanoSeconds() - rtt * 10; // Units are nanoseconds.


	// Erase any values that are too old.
  	auto it = qp->bbr.m_rtt_window.begin();
	while (it != qp->bbr.m_rtt_window.end()) 
	{
    	if ((it -> first).GetNanoSeconds() < delta)
		{
			it = qp->bbr.m_rtt_window.erase(it);
		}
    	else
		{
			it++;
		}
  	}
}

uint64_t RdmaHw::updateTargetCwnd(Ptr<RdmaQueuePair> qp){
	uint64_t bdp = getBDP(qp);
	return(bdp);
}

bool RdmaHw::checkProbeRTT(Ptr<RdmaQueuePair> qp){
	Time now = Simulator::Now();
	if(qp->bbr.bbr_state_ == PROBE_BW_STATE &&
	(now.GetMicroSeconds() - qp->bbr.m_min_rtt_change.GetMicroSeconds()) >
      RTT_NOCHANGE_LIMIT)
	  {
		  qp->bbr.m_min_rtt_change = now;
		  return true;
	  }
	return false;
}

bbr_state RdmaHw::getStateType(Ptr<RdmaQueuePair> qp){
	return qp->bbr.bbr_state_;
}

void RdmaHw::update(Ptr<RdmaQueuePair> qp){
	if(checkProbeRTT(qp))
	{
		changeState(qp, PROBE_RTT_STATE);
	}
	execute(qp);
	cullRTTwindow(qp);
	cullBWwindow(qp);

	uint64_t rtt = getRTT(qp);
	if(rtt > 0)
	{
		uint32_t tmp = Now().GetNanoSeconds() + rtt;
		Simulator::Schedule(NanoSeconds(tmp), &RdmaHw::update, this, qp);
	}
}

void RdmaHw::changeState(Ptr<RdmaQueuePair> qp, bbr_state new_bbr_state)
{
	qp->bbr.bbr_state_ = new_bbr_state;
	enter(qp);
}

void RdmaHw::enter(Ptr<RdmaQueuePair> qp)
{
	if(qp->bbr.bbr_state_ == STARTUP_STATE)
	{
		qp->bbr.m_cwnd_gain = STARTUP_GAIN;
	}
	if(qp->bbr.bbr_state_ == DRAIN_STATE)
	{
		qp->bbr.m_cwnd_gain = 1/STARTUP_GAIN; // Slow cwnd if no pacing.
		uint64_t bdp = getBDP(qp);
		qp->bbr.m_inflight_limit = bdp;
		qp->bbr.m_round_count = 0;
	}
	if(qp->bbr.bbr_state_ == PROBE_BW_STATE)
	{
		// Pick random start cycle phase (except "low") to avoid synch of
  		// flows that enter PROBE_BW simultaneously.
 		qp->bbr.m_gain_cycle = rand() % 8;
		if(qp->bbr.m_gain_cycle == 0)
		{
			qp->bbr.m_cwnd_gain = STEADY_FACTOR + PROBE_FACTOR;
		}
		if(qp->bbr.m_gain_cycle == 1)
		{
			qp->bbr.m_cwnd_gain = STEADY_FACTOR - PROBE_FACTOR;
		}
		else
		{
			qp->bbr.m_cwnd_gain = STEADY_FACTOR;
		}
	}
	if(qp->bbr.bbr_state_ == PROBE_RTT_STATE)
	{
		qp->bbr.m_cwnd_gain = STEADY_FACTOR;
		uint64_t rtt = getRTT(qp);
		qp->bbr.m_probe_rtt_time = NanoSeconds(rtt);
		qp->bbr.m_probe_rtt_time = qp->bbr.m_probe_rtt_time + Simulator::Now();
	}

}

void RdmaHw::execute(Ptr<RdmaQueuePair> qp)
{
	if(qp->bbr.bbr_state_ == STARTUP_STATE)
	{
		uint64_t new_bw = getBW(qp); //in Gbps
		if(new_bw<0)
		{
			return;
		}

		if(new_bw > qp->bbr.m_full_bw * STARTUP_THRESHOLD)
		{
			qp->bbr.m_full_bw = new_bw;
			qp->bbr.m_full_bw_count = 0;
			return;
		}

		qp->bbr.m_full_bw_count++;

		// If 3+ rounds w/out much growth, STARTUP --> DRAIN.
  		if (qp->bbr.m_full_bw_count > 2) 
		{
    		changeState(qp, DRAIN_STATE);
  		}
	}

	if(qp->bbr.bbr_state_ == DRAIN_STATE)
	{
		qp->bbr.m_round_count++;
		//to finish: no inflight data limitation
		if (qp->bbr.m_round_count == 5)
	  	{
		  changeState(qp, PROBE_BW_STATE);
	  	}
	}

	if(qp->bbr.bbr_state_ == PROBE_BW_STATE)
	{
		if(qp->bbr.m_gain_cycle == 0)
		{
			qp->bbr.m_cwnd_gain = STEADY_FACTOR + PROBE_FACTOR;
		}
		else if(qp->bbr.m_gain_cycle == 1)
		{
			qp->bbr.m_cwnd_gain = STEADY_FACTOR - PROBE_FACTOR;
		}
		else
		{
			qp->bbr.m_cwnd_gain = STEADY_FACTOR;
		}
		qp->bbr.m_gain_cycle++;
		if(qp->bbr.m_gain_cycle > 7)
		{
			qp->bbr.m_gain_cycle = 0;
		}
	}

	if(qp->bbr.bbr_state_ == PROBE_RTT_STATE)
	{
		qp->m_rate = m_minRate;
		Time now = Simulator::Now();
		if(now > qp->bbr.m_probe_rtt_time)
		{
			changeState(qp, PROBE_BW_STATE);
		}
	}
}

/*********************
 * HPCC-PINT
 ********************/
void RdmaHw::SetPintSmplThresh(double p){
       pint_smpl_thresh = (uint32_t)(65536 * p);
}
void RdmaHw::HandleAckHpPint(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch){
       uint32_t ack_seq = ch.ack.seq;
       if (rand() % 65536 >= pint_smpl_thresh)
               return;
       // update rate
       if (ack_seq > qp->hpccPint.m_lastUpdateSeq){ // if full RTT feedback is ready, do full update
               UpdateRateHpPint(qp, p, ch, false);
       }else{ // do fast react
               UpdateRateHpPint(qp, p, ch, true);
       }
}

void RdmaHw::UpdateRateHpPint(Ptr<RdmaQueuePair> qp, Ptr<Packet> p, CustomHeader &ch, bool fast_react){
       uint32_t next_seq = qp->snd_nxt;
       if (qp->hpccPint.m_lastUpdateSeq == 0){ // first RTT
               qp->hpccPint.m_lastUpdateSeq = next_seq;
       }else {
               // check packet INT
               IntHeader &ih = ch.ack.ih;
               double U = Pint::decode_u(ih.GetPower());

               DataRate new_rate;
               int32_t new_incStage;
               double max_c = U / m_targetUtil;

               if (max_c >= 1 || qp->hpccPint.m_incStage >= m_miThresh){
                       new_rate = qp->hpccPint.m_curRate / max_c + m_rai;
                       new_incStage = 0;
               }else{
                       new_rate = qp->hpccPint.m_curRate + m_rai;
                       new_incStage = qp->hpccPint.m_incStage+1;
               }
               if (new_rate < m_minRate)
                       new_rate = m_minRate;
               if (new_rate > qp->m_max_rate)
                       new_rate = qp->m_max_rate;
               ChangeRate(qp, new_rate);
               if (!fast_react){
                       qp->hpccPint.m_curRate = new_rate;
                       qp->hpccPint.m_incStage = new_incStage;
               }
               if (!fast_react){
                       if (next_seq > qp->hpccPint.m_lastUpdateSeq)
                               qp->hpccPint.m_lastUpdateSeq = next_seq; //+ rand() % 2 * m_mtu;
               }
       }
}

}
