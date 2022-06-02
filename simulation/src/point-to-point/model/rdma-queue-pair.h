#ifndef RDMA_QUEUE_PAIR_H
#define RDMA_QUEUE_PAIR_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ipv4-address.h>
#include <ns3/data-rate.h>
#include <ns3/event-id.h>
#include <ns3/custom-header.h>
#include <ns3/int-header.h>
#include <vector>
#include <map>

namespace ns3 {

	// Structure for storing BW estimates.
	struct bw_struct {
  	Time time;               // Time stored.
  	int round;               // Virtual time stored.
  	uint64_t bw_est;           // Bandwidth estimate.
	};

	// Defined BBR' states.
	enum bbr_state {
	UNDEFINED_STATE=-1,
	STARTUP_STATE,
	DRAIN_STATE,
	PROBE_BW_STATE,
	PROBE_RTT_STATE,
	};

class RdmaQueuePair : public Object {
public:


	Time startTime;
	Ipv4Address sip, dip;
	uint16_t sport, dport;
	uint64_t m_size;
	uint64_t snd_nxt, snd_una; // next seq to send, the highest unacked seq
	uint16_t m_pg;
	uint16_t m_ipid;
	uint32_t m_win; // bound of on-the-fly packets
	uint64_t m_baseRtt; // base RTT of this qp
	DataRate m_max_rate; // max rate
	bool m_var_win; // variable window size
	Time m_nextAvail;	//< Soonest time of next send
	uint32_t wp; // current window of packets
	uint32_t lastPktSize;
	Callback<void> m_notifyAppFinish;

	//test probeBw
	std::vector<std::vector<double>> dynaU; 
	std::vector<uint64_t> curB;
	uint64_t lowerB;

	//test probeRTT
	std::vector<uint64_t> myRTTs;
	bool ifSaveRtt = true;

	//to compute the fairness of competition
	uint32_t m_cc_mode;


	/******************************
	 * runtime states
	 *****************************/
	DataRate m_rate;	//< Current rate
	struct {
		DataRate m_targetRate;	//< Target rate
		EventId m_eventUpdateAlpha;
		double m_alpha;
		bool m_alpha_cnp_arrived; // indicate if CNP arrived in the last slot
		bool m_first_cnp; // indicate if the current CNP is the first CNP
		EventId m_eventDecreaseRate;
		bool m_decrease_cnp_arrived; // indicate if CNP arrived in the last slot
		uint32_t m_rpTimeStage;
		EventId m_rpTimer;
	} mlx;
	struct {
		std::vector<Time> m_pkt_time;         //store the received time of ack
		uint32_t m_lastUpdateSeq;
		DataRate m_curRate;
		IntHop hop[IntHeader::maxHop];
		uint32_t keep[IntHeader::maxHop];
		uint32_t m_incStage;
		double m_lastGap;
		double u;
		struct {
			double u;
			DataRate Rc;
			uint32_t incStage;
		}hopState[IntHeader::maxHop];
	} hp;
	struct{
		uint32_t m_lastUpdateSeq;
		IntHop hop[IntHeader::maxHop];
		std::vector<Time> m_pkt_time;         //store the received time of ack
		uint32_t keep[IntHeader::maxHop];
		DataRate m_curRate;
		uint32_t m_incStage;
		uint64_t lastRtt;
		double rttDiff;
	} tmly;
	struct{
		uint32_t m_lastUpdateSeq;
		IntHop hop[IntHeader::maxHop];
		std::vector<Time> m_pkt_time;         //store the received time of ack
		uint32_t keep[IntHeader::maxHop];
		uint32_t m_caState;
		uint32_t m_highSeq; // when to exit cwr
		double m_alpha;
		uint32_t m_ecnCnt;
		uint32_t m_batchSizeOfAlpha;
	} dctcp;
	struct{
		uint32_t m_lastUpdateSeq;
		DataRate m_curRate;
		uint32_t m_incStage;
	}hpccPint;
	struct{
		uint32_t m_lastUpdateSeq;
		IntHop hop[IntHeader::maxHop];
		uint32_t keep[IntHeader::maxHop];
		uint32_t m_caState;
		uint32_t m_highSeq; // when to exit cwr
		double m_cwnd_gain;  // Scale estimated BDP for cwnd.
		bbr_state bbr_state_;
		std::map<Time, uint64_t> m_rtt_window;       // For computing min RTT.
		std::vector<bw_struct> m_bw_window; // For computing max BW.
		uint64_t m_inflight_limit; // Target bytes in flight to exit DRAIN state.
		uint32_t m_round_count;    // Number of rounds in DRAIN state.
		int m_gain_cycle;                        // For cycling gain in PROBE_BW.
		std::vector<Time> m_pkt_time;         //store the received time of ack
		double m_full_bw;                        // Max prev BW in STARTUP.
		int m_full_bw_count;                     // Times BW not grown in STARTUP.
		Time m_min_rtt_change;                   // Last time min RTT changed.
		Time m_probe_rtt_time;     // Time to remain in PROBE_RTT.

	}bbr;

	/***********
	 * methods
	 **********/
	static TypeId GetTypeId (void);
	RdmaQueuePair(uint16_t pg, Ipv4Address _sip, Ipv4Address _dip, uint16_t _sport, uint16_t _dport);
	void SetSize(uint64_t size);
	void SetWin(uint32_t win);
	void SetBaseRtt(uint64_t baseRtt);
	void SetVarWin(bool v);
	void SetAppNotifyCallback(Callback<void> notifyAppFinish);

	uint64_t GetBytesLeft();
	uint32_t GetHash(void);
	void Acknowledge(uint64_t ack);
	uint64_t GetOnTheFly();
	bool IsWinBound();
	uint64_t GetWin(); // window size calculated from m_rate
	bool IsFinished();
	uint64_t HpGetCurWin(); // window size calculated from hp.m_curRate, used by HPCC

};

class RdmaRxQueuePair : public Object { // Rx side queue pair
public:
	struct ECNAccount{
		uint16_t qIndex;
		uint8_t ecnbits;
		uint16_t qfb;
		uint16_t total;

		ECNAccount() { memset(this, 0, sizeof(ECNAccount));}
	};
	ECNAccount m_ecn_source;
	uint32_t sip, dip;
	uint16_t sport, dport;
	uint16_t m_ipid;
	uint32_t ReceiverNextExpectedSeq;
	Time m_nackTimer;
	int32_t m_milestone_rx;
	uint32_t m_lastNACK;
	EventId QcnTimerEvent; // if destroy this rxQp, remember to cancel this timer

	static TypeId GetTypeId (void);
	RdmaRxQueuePair();
	uint32_t GetHash(void);
};

class RdmaQueuePairGroup : public Object {
public:
	std::vector<Ptr<RdmaQueuePair> > m_qps;
	//std::vector<Ptr<RdmaRxQueuePair> > m_rxQps;

	static TypeId GetTypeId (void);
	RdmaQueuePairGroup(void);
	uint32_t GetN(void);
	Ptr<RdmaQueuePair> Get(uint32_t idx);
	Ptr<RdmaQueuePair> operator[](uint32_t idx);
	void AddQp(Ptr<RdmaQueuePair> qp);


	//void AddRxQp(Ptr<RdmaRxQueuePair> rxQp);
	void Clear(void);
};

}

#endif /* RDMA_QUEUE_PAIR_H */
