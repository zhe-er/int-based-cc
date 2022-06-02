#bbr
'''
#exp1
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --bcase=0' 
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --bcase=1'
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --bcase=2'
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --bcase=3'
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --bcase=4'
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --bcase=5'
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --bcase=6'      

#exp2
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1' 

#exp3
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --rchange=1 --rdyna=1' 
'''
#exp4
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --rchange=1 --rdyna=1'
'''
#exp5 (only four flows)
sudo ./waf --run 'scratch/third mix/config.txt --cc=11 --bchange=1 --bdyna=1 --rchange=1 --rdyna=1 --single=true'
'''