import subprocess
import argparse

#write into excel
import xlwt
import xlrd
import os

def get_pctl(a, p):
	i = int(len(a) * p)
	return a[i]

if __name__=="__main__":
	parser = argparse.ArgumentParser(description='')
	parser.add_argument('-p', dest='prefix', action='store', default='fct', help="Specify the prefix of the fct file. Usually like fct_<topology>_<trace>")
	parser.add_argument('-s', dest='step', action='store', default='1')
	parser.add_argument('-t', dest='type', action='store', type=int, default=0, help="0: normal, 1: incast, 2: all")
	parser.add_argument('-T', dest='time_limit', action='store', type=int, default=30000000000, help="only consider flows that finish before T")
	parser.add_argument('-b', dest='bw', action='store', type=int, default=25, help="bandwidth of edge link (Gbps)")
	args = parser.parse_args()

	type = args.type
	time_limit = args.time_limit

	# Please list all the cc (together with parameters) that you want to compare.
	# For example, here we list two CC: 1. HPCC-PINT with utgt=95,AI=50Mbps,pint_log_base=1.05,pint_prob=1; 2. HPCC with utgt=95,ai=50Mbps.
	# For the exact naming, please check ../simulation/mix/fct_*.txt output by the simulation.
	CCs = [
		'hpccPint95ai50log1.05p1.000',
		#'hp95ai50',
	]


	changes = ["bchange_", "rchange_", "rtchange_", ""]
	ccs = ["hpcc_", "timely_", "dctcp_", "bbr_", "ccdfs_"]
	btvalues = ["12.5Gbps", "25Gbps", "50Gbps", "100Gbps", "200Gbps", "400Gbps","800Gbps", "0us", "1us", "4us", "8us", "2us", ""]


	#overall average data
	overall_avg = xlwt.Workbook(encoding='utf-8', style_compression=0)
	overall_sheet = overall_avg.add_sheet("overall")

	overall_count = 0
	for change in changes:
		for cc in ccs:
			for btvalue in btvalues:
				if(os.path.exists("results/fct_" + change + cc + btvalue + ".txt")):
					fct = open("results/fct_" + change + cc + btvalue + ".txt")
					uti = open("results/uti_" + change + cc + btvalue + ".txt")
					thr = open("results/thr_" + change + cc + btvalue + ".txt")
					qlen = open("results/qlen_" + change + cc + btvalue + ".txt")

					#................for fct data...............................
					ofct = xlwt.Workbook(encoding='utf-8', style_compression=0)
					sheet = ofct.add_sheet("fct_data")
					ofct_classify = xlwt.Workbook(encoding='utf-8', style_compression=0)
					sheet0 = ofct_classify.add_sheet("0")
					sheet1 = ofct_classify.add_sheet("1")
					sheet2 = ofct_classify.add_sheet("2")

					lines = fct.readlines()
					row = 0
					row0 = 0
					row1 = 0
					row2 = 0
					total_flowsize = 0
					total_flowsize0 = 0
					total_flowsize1 = 0
					total_flowsize2 = 0
					total_fct = 0
					total_fct0 = 0
					total_fct1 = 0
					total_fct2 = 0
					for line in lines:
						curline = line.strip().split(" ")
						flowsize = float(curline[4]) 
						fct = float(curline[6])
						if(int(flowsize) < 20000):
							type = 0
							sheet0.write(row0,0,type)
							sheet0.write(row0,1,flowsize)
							sheet0.write(row0,2,fct)
							row0 = row0 + 1
							total_flowsize0 = total_flowsize0 + flowsize
							total_fct0 = total_fct0 + fct
						elif(int(flowsize)>=20000 and int(flowsize)<50000):
							type = 1
							sheet1.write(row1,0,type)
							sheet1.write(row1,1,flowsize)
							sheet1.write(row1,2,fct)
							row1 = row1 + 1
							total_flowsize1 = total_flowsize1 + flowsize
							total_fct1 = total_fct1 + fct
						else:
							type = 2
							sheet2.write(row2,0,type)
							sheet2.write(row2,1,flowsize)
							sheet2.write(row2,2,fct)
							row2 = row2 + 1
							total_flowsize2 = total_flowsize2 + flowsize
							total_fct2 = total_fct2 + fct

						sheet.write(row,0,type)
						sheet.write(row,1,flowsize)
						sheet.write(row,2,fct)
						row = row+1
						total_flowsize = total_flowsize + flowsize
						total_fct = total_fct + fct

					avg_flowsize = total_flowsize/float(row)
					avg_fct = total_fct/float(row)

					avg_flowsize0 = total_flowsize0/float(row0)
					avg_fct0 = total_fct0/float(row0)

					avg_flowsize1 = total_flowsize1/float(row1)
					avg_fct1 = total_fct1/float(row1)

					avg_flowsize2 = total_flowsize2/float(row2)
					avg_fct2 = total_fct2/float(row2)

					ofct.save("trans_results/fct/fct_" + change + cc + btvalue+".xls")
					ofct_classify.save("trans_results/fct/fct_classify_" + change + cc + btvalue + ".xls")

					#................for uti data...............................
					#outi = xlwt.Workbook(encoding='utf-8', style_compression=0)
					#sheet = outi.add_sheet("uti_data")
					lines = uti.readlines()
					row = 0
					total_uti = 0
					totalNum = 0
					for line in lines:
						curline = line.strip().split(" ")
						utivalue = float(curline[0]) 
						#sheet.write(row,0,utivalue)
						row = row+1
						total_uti = total_uti + utivalue
						totalNum = totalNum + 1
						if(totalNum > 20000):
							break

					avg_uti = total_uti/float(row)
					print(total_uti)
					print(row)
					print(avg_uti)
					print("stop.................")
					
					#outi.save("trans_results/uti/uti_"+change+dyna+btvalue+".xls")

					#................for qlen data...............................
					#oqlen = xlwt.Workbook(encoding='utf-8', style_compression=0)
					#sheet = outi.add_sheet("qlen_data")
					lines = qlen.readlines()
					row = 0
					total_qlen = 0
					for line in lines:
						curline = line.strip().split(" ")
						qlenvalue = float(curline[0]) 
						#sheet.write(row,0,qlenvalue)
						row = row+1
						total_qlen = total_qlen + qlenvalue

					avg_qlen = total_qlen/float(row)
					#ofct.save("trans_results/qlen/qlen_"+change+dyna+btvalue+".xls")


					#................for thr data...............................
					#oqlen = xlwt.Workbook(encoding='utf-8', style_compression=0)
					#sheet = outi.add_sheet("qlen_data")
					lines = thr.readlines()
					row = 0
					total_thr = 0
					for line in lines:
						curline = line.strip().split(" ")
						thrvalue = float(curline[0]) 
						#sheet.write(row,0,qlenvalue)
						row = row+1
						total_thr = total_thr + thrvalue

					avg_thr = total_thr/float(row)
					#ofct.save("trans_results/qlen/qlen_"+change+dyna+btvalue+".xls")

					#write into overall_avg
					overall_sheet.write(overall_count,0,change)
					overall_sheet.write(overall_count,1,cc)
					overall_sheet.write(overall_count,2,btvalue)
					overall_sheet.write(overall_count,3,avg_fct)
					overall_sheet.write(overall_count,4,avg_fct0)
					overall_sheet.write(overall_count,5,avg_fct1)
					overall_sheet.write(overall_count,6,avg_fct2)
					overall_sheet.write(overall_count,7,avg_uti)
					overall_sheet.write(overall_count,8,avg_thr)
					overall_sheet.write(overall_count,9,avg_qlen)
					overall_count = overall_count + 1

	overall_avg.save("trans_results/overall.xls")



	'''
	for cc in CCs:
		#file = "%s_%s.txt"%(args.prefix, cc)
		file = "../simulation/mix/%s.txt"%(args.prefix)
		if type == 0:
			cmd = "cat %s"%(file)+" | awk '{if ($4==100 && $6+$7<"+"%d"%time_limit+") {slow=$7/$8;print slow<1?1:slow, $5}}' | sort -n -k 2"
			# print cmd
			output = subprocess.check_output(cmd, shell=True)
		elif type == 1:
			cmd = "cat %s"%(file)+" | awk '{if ($4==200 && $6+$7<"+"%d"%time_limit+") {slow=$7/$8;print slow<1?1:slow, $5}}' | sort -n -k 2"
			#print cmd
			output = subprocess.check_output(cmd, shell=True)
		else:
			cmd = "cat %s"%(file)+" | awk '{$6+$7<"+"%d"%time_limit+") {slow=$7/$8;print slow<1?1:slow, $5}}' | sort -n -k 2"
			#print cmd
			output = subprocess.check_output(cmd, shell=True)

		# up to here, `output` should be a string of multiple lines, each line is: fct, size
		a = output.split('\n')[:-2]
		n = len(a)
		print("n is: ", n)
		step = int(args.step)
		res = [[i/n] for i in range(0, n, step)]
		for i in range(0,n,step):
			l = i * n / n
			r = (i+step) * n / n
			d = map(lambda x: [float(x.split()[0]), int(x.split()[1])], a[l:r])
			fct=sorted(map(lambda x: x[0], d))
			res[i/step].append(d[-1][1]) # flow size
			#res[i/step].append(sum(fct) / len(fct)) # avg fct
			res[i/step].append(get_pctl(fct, 0.5)) # mid fct
			#res[i/step].append(get_pctl(fct, 0.95)) # 95-pct fct
			#res[i/step].append(get_pctl(fct, 0.99)) # 99-pct fct
	
	#ofile = open("fct_analysis_file.txt", "w")
	ofile = xlwt.Workbook(encoding='utf-8', style_compression=0)
	sheet = ofile.add_sheet("fct_data")
	row = 0
	for item in res:
		line = "%.3f %d"%(item[0], item[1])
		i = 1
		for cc in CCs:
			line += "\t%.3f"%(item[i+1])
			i += 4
		print line
		#ofile.write(line)
		#ofile.write("\n")
		for col in range(3):
			sheet.write(row,col,item[col])
		row = row+1
	#ofile.close()
	ofile.save("fct_analysis_file.xls")
	'''


