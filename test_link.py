import random
import os
import sys
import sketches
import numpy as np
from network_sketches import *
from os.path import basename
from scapy.all import *

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

_id = sys.argv[1]
pcap = "night_sagunt.pcap"
local_mac = "dc:9f:db:08:89:71"
local_address = "fe80::de9f:dbff:fe08:8971"
pattern = 'sagunt'
interval = 5.
num_cols = 32
num_rows = 32
threshold = 0.05

neigh_in = dict()
neigh_out = dict()
local_in = dict()
local_out = dict()
sketch = sketches.FastCount32(num_cols, num_rows, 'cw4')
net_sketch = NetworkSketch(sketch)
for neighbor in neighbors:
    neigh_in[neighbor] = net_sketch.copy()
    neigh_out[neighbor] = net_sketch.copy()
    local_in[neighbor] = net_sketch.copy()
    local_out[neighbor] = net_sketch.copy()

results = []
pkts = PcapReader(pcap)
next_interval = pkts.next().time + interval
pkts.close()
pkts = PcapReader(pcap)
last_random = ""
global_in = net_sketch.copy()
global_out = net_sketch.copy()
for pkt in pkts:
    # New interval?
    error_now = False
    if pkt.time < 1412019186.31 and pkt.time> 1412019181.31:
        error_now=True
    if pkt.time > next_interval:
        # Corrupt sketches and create global:
        detected = False
        for neighbor in neighbors:
            neigh_in[neighbor].corrupt(local_out[neighbor], threshold)
            if local_out[neighbor].detect_link(neigh_in[neighbor], threshold):
                detected = True
            neigh_out[neighbor].corrupt(local_in[neighbor], threshold)
            if local_in[neighbor].detect_link(neigh_out[neighbor], threshold):
                detected = True
            global_in += neigh_out[neighbor]
            global_out += neigh_in[neighbor]
            # Clear sketch for next iteration
            neigh_out[neighbor].clear()
            neigh_in[neighbor].clear()
            local_in[neighbor].clear()
            local_out[neighbor].clear()
        # Compute the statistics:
        estimated_sent = global_in.second_moment()
        estimated_received = global_out.second_moment()
        estimated_missing_packets = global_in.difference(global_out)
        max_value = max(global_in.sketch.get_max(), global_out.sketch.get_max())
        result = (estimated_sent, estimated_received, 
                estimated_missing_packets, max_value, interval, num_rows, num_cols, 
                str(sketch.__class__), detected)
        results.append(result)
        # Clear the sketches
        global_in.clear()
        global_out.clear()
        next_interval += interval
     
    src = pkt.getfieldval('src')
    dst = pkt.getfieldval('dst')
    # Skip pkts for or from local address
    if pkt.getlayer(1).src == local_address or pkt.getlayer(1).dst == local_address:
        continue
    if src in neighbors and dst == local_mac:
        neighbor = neighbors[src]
        # Complete packet
        try:
            last_random = os.urandom(max(len(pkt)-pkt.len-14,0))
            complete_pkt = pkt/Raw(last_random)
        except (TypeError, ValueError, AttributeError):
            print pkt.__repr__()
            continue
        # Compute the packet drop probabilities:
        link_drop = random.random() < neighbor[0]
        
        # Add to neigh out_dump
        complete_pkt.time = pkt.time
        neigh_out[src].update(complete_pkt)
        
        # Add to local in if no drop:
        if not link_drop:
            local_in[src].update(complete_pkt)
        # First drop with p_neigh probability:
        if link_drop:
            # Skip also next:
            pkts.next()
            continue
    elif dst in neighbors and src == local_mac:
        neighbor = neighbors[dst]
        # Complete packet
        try:
            complete_pkt = pkt/Raw(last_random)
        except (TypeError, ValueError, AttributeError):
            print pkt.__repr__()
            continue
        
        # Add to local out
        complete_pkt.time = pkt.time
        local_out[dst].update(complete_pkt)
        
        # If not dropped, add to neigh_in
        if random.random() < neighbor[1]:
            continue
        neigh_in[dst].update(complete_pkt)

results_dtype = [ ('EstimatedIn', 'float'), 
                  ('EstimatedOut', 'float'),
                  ('EstimatedDifference', 'float'), 
                  ('MaxValue', 'float'), 
                  ('Interval', 'float'), 
                  ('SketchRows', 'float'), 
                  ('SketchColumns', 'float'), 
                  ('SketchClass', '|S96'),
                  ('NeighborDetected', 'bool_')]
result = np.array(results, results_dtype)
np.savetxt("test_link_%s.csv" % _id, result, delimiter=',', fmt="%s", 
            header=",".join(result.dtype.names), comments='')

