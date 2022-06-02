#ifndef CONTROL_DECIDER_H
#define CONTROL_DECIDER_H

#include "ns3/application.h"
#include <vector>
#include <numeric>
#include <map>
#include <fstream>
#include <unordered_map>
#include <fstream>
#include <iostream>


namespace ns3

{
class ControlDecider : public Object
{
public:
	static TypeId GetTypeId (void);

	ControlDecider();
	~ControlDecider();

	std::unordered_map<uint64_t, uint64_t> overallkey;

	void SetKey(uint64_t key, uint64_t size);

	double m_rtt_multiplier;

	//save qlen and utilization value
	void SaveData(double u, double thr, double len);
	void SaveSingleThr(std::vector<double> t, std::vector<double> thr);

	//to compute the fairness of competition
	int cc_round = 0;
	uint32_t cc_mode = 0;
	uint32_t CurCC();
	void SaveComp(std::vector<double> t, std::vector<double> thr, uint32_t m_cc_mode);
	std::string filename_comp;
	std::ofstream outFile_comp;

	//save multiplier and bw
	void SaveRtt(double m1, double m2);
	void SaveBw(uint64_t bw1, uint64_t bw2);
	std::string filename_m1;
	std::string filename_m2;
	std::string filename_bw1;
	std::string filename_bw2;
	std::ofstream outFile_m1;
	std::ofstream outFile_m2;
	std::ofstream outFile_bw1;
	std::ofstream outFile_bw2;

	std::string filename_qlen;
	std::string filename_uti;
	std::string filename_thr;
	std::string filename_single_thr;
	int flowNum=0;
	std::ofstream outFile_qlen;
	std::ofstream outFile_uti;
	std::ofstream outFile_thr;
	std::ofstream outFile_single_thr;  //the throughput of single flows
	int mybchange,myrchange,mybdyna,myrdyna,mybcase,myrcase;


private:
	int m;

};

} // namespace ns3


#endif

