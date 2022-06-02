#ccdfs

#exp1
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --bcase=0' 
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --bcase=1'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --bcase=2'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --bcase=3'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --bcase=4'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --bcase=5'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --bcase=6'    

#exp2
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1' 

#exp3
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --rchange=1 --rdyna=1' 

#exp4
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --rchange=1 --rdyna=1'

#exp5 (only four flows)--fairness
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --rchange=1 --rdyna=1 --single=true'

#exp6 (only four flows)--competing
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --rchange=1 --rdyna=1 --comp=true'

'''Sensitivity Analysis'''

#exp7 probebw precision (the recording size: 50, 100, 150, 200)--default:100
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --saveBw=true'

#exp8 probertt precision (the recording size: 50, 100, 150, <qlen: 10, 5, 0)--default:100
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --rchange=1 --rdyna=1 --mul=true'

#exp9 varied U --default:0.95,0.95,0.95
'''
exp1: 0.95 0.95 0.9
exp2: 0.95 0.95 0.93
exp3: 0.95 0.95 0.96

exp4: 0.9 0.95 0.95
exp5: 0.93 0.95 0.95
exp6: 0.96 0.95 0.95

exp7: 0.95 0.90 0.95
exp8: 0.95 0.93 0.95
exp9: 0.95 0.96 0.95
'''
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --rchange=1 --rdyna=1 --varu=true --smallu=0.8 --mediumu=0.95 --largeu=0.8'

#exp10 varied weight--default: 0.5,0.3,0.2
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=1 --rchange=1 --rdyna=1'
'''
exp1: 0.5 0.3 0.2
exp2: 0.6 0.24 0.16
exp3: 0.7 0.18 0.12
exp4: 0.8 0.12 0.08
'''

