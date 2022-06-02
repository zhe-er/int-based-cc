#include "control-decider.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include <numeric>
#include <fstream>
#include <sstream>
#include <math.h>
#include <fstream>
#include "ns3/trace-source-accessor.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"


namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("ControlDecider");

NS_OBJECT_ENSURE_REGISTERED (ControlDecider);

TypeId
ControlDecider::GetTypeId (void)
{
  	static TypeId tid = TypeId ("ns3::ControlDecider")
	    .SetParent<Object> ()
	    .SetGroupName("Applications")
	    .AddConstructor<ControlDecider> ()
	    ;
	return tid;
}


ControlDecider::ControlDecider()
  : m_rtt_multiplier (1)
{
	NS_LOG_FUNCTION(this);
}

ControlDecider::~ControlDecider()
{
	NS_LOG_FUNCTION(this);
}

void
ControlDecider::SetKey(uint64_t key, uint64_t size)
{
	overallkey[key] = size;
}

void
ControlDecider::SaveData(double u, double thr, double len)
{
	outFile_qlen.open(filename_qlen+".txt", std::ios::app|std::ios::out);
	outFile_qlen << len << "\n";
    outFile_qlen.close();

	outFile_thr.open(filename_thr+".txt", std::ios::app|std::ios::out);
	outFile_thr << thr << "\n";
    outFile_thr.close();

	outFile_uti.open(filename_uti+".txt", std::ios::app|std::ios::out);
	outFile_uti << u << "\n";
    outFile_uti.close();

}

void
ControlDecider::SaveSingleThr(std::vector<double> t, std::vector<double> thr)
{
	std::ofstream fileout(filename_single_thr + "_" + std::to_string(flowNum) + ".txt", std::ios::trunc);
	fileout.close();
	outFile_single_thr.open(filename_single_thr + "_" + std::to_string(flowNum) + ".txt", std::ios::app|std::ios::out);
	for(int i=0; i<thr.size(); i++)
	{
		outFile_single_thr << t[i] << "\t" << thr[i] << "\n";
	}
	outFile_single_thr.close();
}

void
ControlDecider::SaveRtt(double m1, double m2)
{
	outFile_m1.open(filename_m1+".txt", std::ios::app|std::ios::out);
	outFile_m1 << m1 << "\n";
	outFile_m1.close();

	outFile_m2.open(filename_m2+".txt", std::ios::app|std::ios::out);
	outFile_m2 << m2 << "\n";
	outFile_m2.close();

}

void
ControlDecider::SaveBw(uint64_t bw1, uint64_t bw2)
{
	outFile_bw1.open(filename_bw1+".txt", std::ios::app|std::ios::out);
	outFile_bw1 << bw1 << "\n";
	outFile_bw1.close();

	outFile_bw2.open(filename_bw2+".txt", std::ios::app|std::ios::out);
	outFile_bw2 << bw2 << "\n";
	outFile_bw2.close();
}

//to compute the fairness of competition
uint32_t
ControlDecider::CurCC()
{
	uint32_t CCs[4] = {3,7,8,11};
	cc_mode = CCs[cc_round%4];
	return(cc_mode);
}

void
ControlDecider::SaveComp(std::vector<double> t, std::vector<double> thr, uint32_t m_cc_mode)
{
	std::ofstream fileout(filename_comp + "_" + std::to_string(m_cc_mode) + ".txt", std::ios::trunc);
	fileout.close();
	outFile_comp.open(filename_comp + "_" + std::to_string(m_cc_mode) + ".txt", std::ios::app|std::ios::out);
	for(int i=0; i<thr.size(); i++)
	{
		outFile_comp << t[i] << "\t" << thr[i] << "\n";
	}
	outFile_comp.close();
}

}
