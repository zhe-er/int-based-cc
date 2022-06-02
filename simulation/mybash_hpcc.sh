#hpcc

#exp1
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --bcase=0' 
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --bcase=1'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --bcase=2'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --bcase=3'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --bcase=4'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --bcase=5'
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --bcase=6'      
'''
#exp2
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0' 

#exp3
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --rchange=1 --rdyna=0' 

#exp4
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --rchange=1 --rdyna=0'

#exp5 (only four flows)
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --rchange=1 --rdyna=0 --single=true'
'''

#exp9 test probeBW
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --bchange=1 --bdyna=0 --saveBw=true' 

#probe test probeRTT
sudo ./waf --run 'scratch/third mix/config.txt --cc=3 --rchange=1 --rdyna=0 --mul=true' 


