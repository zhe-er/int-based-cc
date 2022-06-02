import subprocess
import argparse

#write into excel
import xlwt
import xlrd

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

	ofile = xlwt.Workbook(encoding='utf-8', style_compression=0)
	sheet = ofile.add_sheet("fct_data")

	f = open("../simulation/mix/fct.txt")
	lines = f.readlines()
	row = 0
	for line in lines:
		curline = line.strip().split(" ")
		flowsize = curline[4] 
		fct = curline[6]
		if(int(flowsize) < 20000):
			type = 0
		elif(int(flowsize)>=20000 and int(flowsize)<50000):
			type = 1
		else:
			type = 2
		sheet.write(row,0,type)
		sheet.write(row,1,flowsize)
		sheet.write(row,2,fct)
		row = row+1
	
	ofile.save("fct_analysis_file.xls")



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


