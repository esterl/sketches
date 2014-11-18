# Run using:
# for DROP in 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7; do python split_pcaps.py $DROP; done

import random
import os
import sys

from scapy.all import *

############################# MTR output #######################################
#
# mtr fe80::6f0:21ff:fe03:64 --interval 0.1 -s 806 -c 500 -r
#HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#  1.|-- fe80::6f0:21ff:fe03:64     0.0%   500    1.3   1.5   1.1  31.5   1.8
#
# mtr fe80::215:6dff:fe80:11ce --interval 0.1 -s 806 -c 500 -r
#HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#  1.|-- fe80::215:6dff:fe80:11ce  74.8%   500    1.6  33.9   1.2 505.7  98.6
#
# mtr fe80::6f0:21ff:fe03:65 --interval 0.1 -s 806 -c 500 -r
#HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#  1.|-- fe80::6f0:21ff:fe03:65    94.2%   500  1383.  98.7   4.3 1383. 255.5
#
#mtr fe80::de9f:dbff:fe08:8da9 --interval 0.1 -s 806 -c 500 -r 
#HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#  1.|-- fe80::de9f:dbff:fe08:8da9  0.0%   500    1.4   2.5   1.1  65.9   4.8
#mtr fe80::6f0:21ff:fe03:52 --interval 0.1 -s 806 -c 500 -r
#HOST: GS_Sagunt66_RK              Loss%   Snt   Last   Avg  Best  Wrst StDev
#  1.|-- fe80::6f0:21ff:fe03:52    30.6%   500    1.4   2.5   1.3  43.5   3.8
################################################################################
neighbors = {
    #qMp-fe4c fe80::6f0:21ff:fe03:64
    "04:f0:21:03:00:64" : (0.05**4, 0.0, 1.5, 1.8),
    #GS-RamblaBadal fe80::215:6dff:fe80:11ce
    "00:15:6d:80:11:ce" : (0.5, 0.5, 33.9, 98.6),
    #GSmVictoria fe80::de9f:dbff:fe08:8da9
    "dc:9f:db:08:8d:a9" : (0.05**4, 0.03**4, 2.5, 4.8),
}

p_drop = float(sys.argv[1])
pcap = "pcaps/morning_sagunt.pcap"
local_mac = "dc:9f:db:08:89:71"
local_address = "fe80::de9f:dbff:fe08:8971"
pattern = 'morning_sagunt'

neigh_in = dict()
neigh_out = dict()
local_in = dict()
local_out = dict()
for neighbor in neighbors:
    in_filename = 'pcaps/%s_%.1f_%s_in.pcap' % (pattern, p_drop, neighbor)
    neigh_in[neighbor] = PcapWriter(in_filename, append=False, sync=True)
    out_filename = 'pcaps/%s_%.1f_%s_out.pcap' % (pattern, p_drop, neighbor)
    neigh_out[neighbor] = PcapWriter(out_filename, append=False, sync=True)
    local_in[neighbor] = PcapWriter('pcaps/%s_%.1f_%s_local_in.pcap' % (pattern, p_drop, neighbor))
    local_out[neighbor] = PcapWriter('pcaps/%s_%.1f_%s_local_out.pcap'% (pattern, p_drop, neighbor))

local_in_glob = PcapWriter('pcaps/%s_%.1f_local_in.pcap' % (pattern, p_drop))
local_out_glob = PcapWriter('pcaps/%s_%.1f_local_out.pcap'% (pattern, p_drop))

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
        in_pkt = pkt
        out_pkt = pkts.next()
        if out_pkt.getfieldval('src')!=local_mac or not out_pkt.getfieldval('dst') in neighbors:
            out_pkt = pkts.next()
        from_neighbor = src
        to_neighbor = out_pkt.getfieldval('dst')
        
        # Complete packets:
        try:
            random_bytes = os.urandom(max(len(pkt)-pkt.len-14,0))
            in_complete = in_pkt/Raw(random_bytes)
            out_complete = out_pkt/Raw(random_bytes)
        except (TypeError, ValueError, AttributeError):
            print pkt.__repr__()
            continue
        
        # Compute drop probabilities:
        link1_drop = random.random() < neighbors[from_neighbor][0]
        link2_drop = random.random() < neighbors[to_neighbor][1]
        malicious_drop = random.random() < p_drop
        
        # Add to from_neigh out pcap
        in_complete.time = in_pkt.time
        # Use the eth dst to indicate drop
        if link1_drop:
            in_complete.dst = '00:00:00:00:00:01'
        elif malicious_drop:
            in_complete.dst = '00:00:00:00:00:00'
        elif link2_drop:
            in_complete.dst = '00:00:00:00:00:02'
        neigh_out[from_neighbor].write(in_complete)
        
        # If the packet was dropped by the first link, skip to next iteration
        if link1_drop:
            continue
        
        # Otherwise, add to local_in with some delay
        link1_delay = max(0.0, random.gauss(neighbors[from_neighbor][2], 
                                                neighbors[from_neighbor][3])/2.)/1000
        in_complete.time += link1_delay
        local_in[from_neighbor].write(in_complete)
        local_in_glob.write(in_complete)
        
        # If the packet was dropped by the node, skip to next iteration:
        if malicious_drop:
            continue
        
        # Otherwise, add to local_out the out packet:
        out_complete.time = out_pkt.time + link1_delay
        local_out[to_neighbor].write(out_complete)
        local_out_glob.write(out_complete)
        
        # If the packet was dropped by the second link, skip to next interation
        if link2_drop:
            continue
        
        # Otherwise add to the neighbor in pcap with some delay
        link2_delay = max(0.0, random.gauss(neighbors[to_neighbor][2], 
                                                neighbors[to_neighbor][3])/2.)/1000
        out_complete.time += link2_delay
        neigh_in[to_neighbor].write(out_complete)


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
local_out_glob.close()
