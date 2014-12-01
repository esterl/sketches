import random
import os
import argparse
import sketches
import numpy as np
from network_sketches import *
from os.path import basename
from scapy.all import *

def get_packet_length(pkt):
    n = 0
    while not pkt.name in ['IPv6', 'IP']:
        header = pkt.copy()
        header.remove_payload()
        n += len(header)
        pkt = pkt.getlayer(1)
    ip_len = pkt.len
    header = pkt.copy()
    header.remove_payload()
    n += len(header)
    return n + ip_len

neighbors = {
    #qMp-fe4c fe80::6f0:21ff:fe03:64
    "04:f0:21:03:00:64" : (0.05**4, 0.0, 1.5, 1.8),
    #GS-RamblaBadal fe80::215:6dff:fe80:11ce
    "00:15:6d:80:11:ce" : (0.5, 0.5, 33.9, 98.6),
    #GSmVictoria fe80::de9f:dbff:fe08:8da9
    "dc:9f:db:08:8d:a9" : (0.05**4, 0.03**4, 2.5, 4.8),
}

parser = argparse.ArgumentParser()
parser.add_argument("_id")
parser.add_argument("threshold", type=float)
parser.add_argument("nmax", default=1, nargs='?', type=int)

globals().update(vars(parser.parse_args()))

pcap = "pcaps/morning_sagunt.pcap"
local_mac = "dc:9f:db:08:89:71"
local_address = "fe80::de9f:dbff:fe08:8971"
pattern = 'morning_sagunt'
interval = 5.
num_cols = 32
num_rows = 32
captured_length = 94

# First check which packets are exactly the same:
pkts = PcapReader(pcap)
hashes = collections.defaultdict(list)
for (i,pkt) in enumerate(pkts):
    while not pkt.name in ['IPv6', 'IP']:
            pkt = pkt.getlayer(1)
    # TODO check if pkt.len or len(pkt)
    hashes[hashlib.sha256(pkt.original).hexdigest()].append(i)
pkts.close()

index_repeated_pkts = dict()
for pkt_hash, indexes in hashes.iteritems():
    if len(indexes) > 1:
        for i in indexes:
            index_repeated_pkts[i] = pkt_hash
repeated_random = dict()

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
i = -1
for pkt in pkts:
    i += 1
    if pkt.time > next_interval:
        # Corrupt sketches and create global:
        detected = False
        for neighbor in neighbors:
            loss_probability = neighbors[neighbor][1]
            neigh_in[neighbor].corrupt(local_out[neighbor], threshold, loss_probability, False, nmax)
            if local_out[neighbor].detect_link(neigh_in[neighbor], threshold, 
                                                loss_probability, False, nmax):
                detected = True
            loss_probability = neighbors[neighbor][0]
            neigh_out[neighbor].corrupt(local_in[neighbor], threshold, loss_probability, True, nmax)
            if local_in[neighbor].detect_link(neigh_out[neighbor], threshold, loss_probability, True, nmax):
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
            if i in index_repeated_pkts:
                pkt_hash = index_repeated_pkts[i]
                if pkt_hash in repeated_random:
                    last_random = repeated_random[pkt_hash]
                else:
                    last_random = os.urandom(max(get_packet_length(pkt) - captured_length,0))
                    repeated_random[pkt_hash] = last_random
                complete_pkt = pkt/Raw(last_random)
                complete_pkt.original = pkt.original + last_random
            else:
                # TODO check if with the new headers still pkt.len - 14 is ok
                last_random = os.urandom(max(get_packet_length(pkt) - captured_length,0))
                complete_pkt = pkt/Raw(last_random)
                complete_pkt.original = pkt.original + last_random
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
            i += 1
            continue
    elif dst in neighbors and src == local_mac:
        neighbor = neighbors[dst]
        # Complete packet
        try:
            complete_pkt = pkt/Raw(last_random)
            complete_pkt.original = pkt.original + last_random
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

