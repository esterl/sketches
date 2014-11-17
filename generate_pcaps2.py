import random
import os
import sys

from scapy.all import *

############################# MTR output #######################################
#
# > mtr -r -c 1000 -i 0.02 -s 313 fe80::6f0:21ff:fe03:64
# HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#   1.|-- fe80::6f0:21ff:fe03:64     0.0%  1000    1.3   2.8   1.0 177.7  11.2
#
# > mtr -r -c 1000 -i 0.02 -s 313 fe80::215:6dff:fe80:11ce
# HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#  1.|-- fe80::215:6dff:fe80:11ce  16.1%  1000   28.0   1.8   1.1  28.0   1.9
#
# > root@GS_Sagunt66_RK:~# mtr -r -c 1000 -i 0.02 -s 313 fe80::6f0:21ff:fe03:65 
# HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#   1.|-- fe80::6f0:21ff:fe03:65     0.0%  1000    1.9   9.5   1.8 166.1  14.7
#
# > mtr -r -c 1000 -i 0.02 -s 313 fe80::227:22ff:fe68:db37
# HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#
#> x = c(106.489, 128.784,131.978,211.574)
#> mean(x)
#[1] 144.7063
#> sd(x)
#[1] 45.99776
#
# >  mtr -r -c 1000 -i 0.02 -s 313 fe80::de9f:dbff:fe08:8da9
# HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#   1.|-- fe80::de9f:dbff:fe08:8da9  0.0%  1000    1.3   1.4   1.0  11.1   1.1
#
# > mtr -r -c 1000 -i 0.02 -s 313 fe80::6f0:21ff:fe03:52
# HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#   1.|-- fe80::6f0:21ff:fe03:52     0.0%  1000    1.2   1.2   1.0   8.8   0.7
#
# > mtr -r -c 1000 -i 0.02 -s 313 fe80::227:22ff:fed6:21bf
# HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#   1.|-- fe80::227:22ff:fed6:21bf   0.0%  1000    2.3   1.3   1.0   5.9   0.7

#root@GS_Sagunt66_RK:~ bmx6 -c show=links
#links:
#name                            llocalIp                  viaDev   rxRate txRate bestTxLink routes wantsOgms 
#GS_Sagunt62-RS                  fe80::6f0:21ff:fe03:65    wlan0.12 64     74     1          0      0         
#GS_SanBaltassar25-RS            fe80::6f0:21ff:fe03:52    wlan0.12 93     100    1          2      1         
#GS-JocsFlorals-coop-consum-21bf fe80::227:22ff:fed6:21bf  wlan0.12 95     95     1          1      1         
#GSmVictoria-RK8da9              fe80::de9f:dbff:fe08:8da9 wlan0.12 95     97     1          46     1         
#qMp-fe4c                        fe80::6f0:21ff:fe03:64    wlan0.12 95     100    1          1      1         
#GS-RamblaBadal140-NS-11ce       fe80::215:6dff:fe80:11ce  wlan0.12 95     100    1          1      1     
################################################################################
neighbors = {
    #qMp-fe4c fe80::6f0:21ff:fe03:64
    "04:f0:21:03:00:64" : (0.05**4, 0.0, 2.8, 11.2),
    #GS-RamblaBadal fe80::215:6dff:fe80:11ce
    "00:15:6d:80:11:ce" : (0.05**4, 0.0, 1.8, 1.9),
    #GS_Sagunt62-RS fe80::6f0:21ff:fe03:65
    "04:f0:21:03:00:65" : (0.36**4, 0.26**4, 9.5, 14.7),
    #GSgranVia fe80::227:22ff:fe68:db37
    "00:27:22:68:db:37" : (0.04477456, 0.6561, 144.7063, 45.99776),
    #GSmVictoria fe80::de9f:dbff:fe08:8da9
    "dc:9f:db:08:8d:a9" : (0.05**4, 0.03**4, 1.4, 1.1),
    #GS_SanBaltassar fe80::6f0:21ff:fe03:52
    "04:f0:21:03:00:52" : (0.07**4, 0.0, 1.2, 0.7),
    #GS-JocsFlorals fe80::227:22ff:fed6:21bf
    "00:27:22:d6:21:bf" : (0.05**4, 0.05**4, 1.3, 0.7)
}

p_drop = float(sys.argv[1])
pcap = "pcaps/night_sagunt.pcap"
local_mac = "dc:9f:db:08:89:71"
local_address = "fe80::de9f:dbff:fe08:8971"
pattern = 'sagunt'

neigh_in = dict()
neigh_out = dict()
local_in = dict()
local_out = dict()
for neighbor in neighbors:
    in_filename = '%s_%.1f_%s_in.pcap' % (pattern, p_drop, neighbor)
    neigh_in[neighbor] = PcapWriter(in_filename, append=False, sync=True)
    out_filename = '%s_%.1f_%s_out.pcap' % (pattern, p_drop, neighbor)
    neigh_out[neighbor] = PcapWriter(out_filename, append=False, sync=True)
    local_in[neighbor] = PcapWriter('%s_%.1f_%s_local_in.pcap' % (pattern, p_drop, neighbor))
    local_out[neighbor] = PcapWriter('%s_%.1f_%s_local_out.pcap'% (pattern, p_drop, neighbor))

local_in_glob = PcapWriter('%s_%.1f_local_in.pcap' % (pattern, p_drop))
local_out_glob = PcapWriter('%s_%.1f_local_out.pcap'% (pattern, p_drop))

pkts = PcapReader(pcap)
last_random = ""
last_delay = 0.0
last_out_time = 0.0
for pkt in pkts:
    src = pkt.getfieldval('src')
    dst = pkt.getfieldval('dst')
    # Skip pkts for or from local address
    if pkt.getlayer(1).src == local_address or pkt.getlayer(1).dst == local_address:
        continue
    if src in neighbors and dst == local_mac:
        neighbor = neighbors[src]
        # Complete packet
        try:
            last_random = os.urandom(max(pkt.len-len(pkt),0))
            complete_pkt = pkt/Raw(last_random)
        except (TypeError, ValueError, AttributeError):
            print pkt.__repr__()
            continue
        # Compute the packet drop probabilities:
        link_drop = random.random() < neighbor[0]
        malicious_drop = random.random() < p_drop
        
        # Add to neigh out_dump
        complete_pkt.time = pkt.time
        if malicious_drop:
            # Use the eth dst to indicate that the packet is dropped
            complete_pkt.setfieldval('dst', '00:00:00:00:00:00')
        neigh_out[src].write(complete_pkt)
        # First drop with p_neigh probability:
        if link_drop:
            # Skip also next:
            pkts.next()
            continue
        # Add to local in_dump with some delay
        last_delay = max(0.0, random.gauss(neighbor[2], neighbor[3])/2.)/1000
        complete_pkt.time = pkt.time + last_delay
        local_in[dst].write(complete_pkt)
        local_in_glob.write(complete_pkt)
    elif dst in neighbors and src == local_mac:
        neighbor = neighbors[dst]
        # Complete packet
        try:
            complete_pkt = pkt/Raw(last_random)
        except (TypeError, ValueError, AttributeError):
            print pkt.__repr__()
            continue
        # Drop with p_corrupt probability:
        if malicious_drop:
            continue
        complete_pkt.time = pkt.time + last_delay
        local_out[dst].write(complete_pkt)
        local_out_glob.write(complete_pkt)
        # Add to neigh in_dump
        if random.random() < neighbor[1]:
            continue
        delay = max(0.0,random.gauss(neighbor[2], neighbor[3])/2.)/1000
        complete_pkt.time = pkt.time + last_delay + delay
        neigh_in[dst].write(complete_pkt)

# Close pcaps
for pcap in neigh_out.values():
    pcap.close()

for pcap in neigh_in.values():
    pcap.close()

for pcap in local_in.values():
    pcap.close()

for pcap in local_out.values():
    pcap.close()

local_in_glob.close()
local_out.close()
