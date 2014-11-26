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
    "04:f0:21:03:00:64" : (0.05**4, 0.0, 1.5, 1.8),
    #GS-RamblaBadal fe80::215:6dff:fe80:11ce
    "00:15:6d:80:11:ce" : (0.5, 0.5, 33.9, 98.6),
    #GSmVictoria fe80::de9f:dbff:fe08:8da9
    "dc:9f:db:08:8d:a9" : (0.05**4, 0.03**4, 2.5, 4.8),
}

_id = sys.argv[1]
pcap = "pcaps/morning_sagunt.pcap"
local_mac = "dc:9f:db:08:89:71"
local_address = "fe80::de9f:dbff:fe08:8971"
pattern = 'morning_sagunt'
interval = 5.
num_cols = 32
num_rows = 32


# First check which packets are exactly the same:
pkts = PcapReader(pcap)
hashes = collections.defaultdict(list)
for (i,pkt) in enumerate(pkts):
    while not pkt.name in ['IPv6', 'IP']:
            pkt = pkt.getlayer(1)
    # TODO check if pkt.len or len(pkt)
    hashes[hashlib.sha256(str(pkt)[0:len(pkt)]).hexdigest()].append(i)
pkts.close()

index_repeated_pkts = dict()
for pkt_hash, indexes in hashes.iteritems():
    if len(indexes) > 1:
        for i in indexes:
            index_repeated_pkts[i] = pkt_hash


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
# Get the startime
pkts = PcapReader(pcap)
next_interval = pkts.next().time + interval
pkts.close()

pkts = PcapReader(pcap)
last_random = ""
i = 0
for pkt in pkts:
    # New interval?
    if pkt.time > next_interval:
        # Compare every sketch and record difference:
        for neighbor in neighbors:
            # From neighbor to local node
            estimated_neigh = neigh_out[neighbor].second_moment()
            estimated_local = local_in[neighbor].second_moment()
            difference = local_in[neighbor].difference(neigh_out[neighbor])
            result = (neighbor, 'in', estimated_neigh, estimated_local, difference,
                        interval, num_cols, num_rows, str(sketch.__class__))
            results.append(result)
            
            # From local local node to neighbor
            estimated_neigh = neigh_in[neighbor].second_moment()
            estimated_local = local_out[neighbor].second_moment()
            difference = local_out[neighbor].difference(neigh_in[neighbor])
            result = (neighbor, 'out', estimated_neigh, estimated_local, difference,
                        interval, num_cols, num_rows, str(sketch.__class__))
            results.append(result)
            
            # Clear sketches and update next_interval
            neigh_out[neighbor].clear()
            neigh_in[neighbor].clear()
            local_in[neighbor].clear()
            local_out[neighbor].clear()
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
                    last_random = os.urandom(max(len(pkt)-pkt.len-14,0))
                    repeated_random[pkt_hash] = last_random
                complete_pkt = pkt/Raw(last_random)
            else:
                # TODO check if with the new headers still pkt.len - 14 is ok
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
            i = i + 1
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

results_dtype = [ ('Neighbor', '|S96'), 
                  ('Direction', '|S8'), 
                  ('EstimatedNeighbor', 'float'), 
                  ('EstimatedLocal', 'float'),
                  ('EstimatedDifference', 'float'), 
                  ('Interval', 'float'), 
                  ('SketchColumns', 'float'), 
                  ('SketchRows', 'float'), 
                  ('SketchClass', '|S96')] 
result = np.array(results, results_dtype)
np.savetxt("threshold_%s.csv" % _id, result, delimiter=',', fmt="%s", 
            header=",".join(result.dtype.names), comments='')

