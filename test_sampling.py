from scapy.all import *
import numpy as np
import glob
from os.path import basename
import hashlib
import random

def get_hash(pkt, key):
    while not pkt.name in ['IPv6', 'IP']:
        pkt = pkt.getlayer(1)
    if pkt.name == 'IPv6':
        pkt.setfieldval('hlim', 0)
    elif pkt.name == 'IP':
        pkt.setfieldval('ttl', 0)
    packet_hash = hashlib.sha256(pkt.__str__()[0:len(pkt.original)]).hexdigest()
    return long(packet_hash, base=16)^key


pattern = sys.argv[1]
_id = sys.argv[2]
threshold = float(sys.argv[3])
hash_length = int(sys.argv[4])
mask = 2**16 - 1
interval = float(sys.argv[5])
result_path = sys.argv[6]
key = random.getrandbits(hash_length)

in_pcap_files = sorted(glob.glob('%s_??:??:??:??:??:??_in.pcap' % pattern))
out_pcap_files = sorted(glob.glob('%s_??:??:??:??:??:??_out.pcap' % pattern))
pcaps = [ PcapReader(filename) for filename in in_pcap_files + out_pcap_files ]
pkts = [ pcap.next() for pcap in pcaps ]
is_in = [ True for pcap in in_pcap_files ] + [ False for pcap in out_pcap_files ]
start_time = min([ pkt.time for pkt in pkts ])
next_interval = start_time + interval
samples = [ [] for pcap in pcaps ]

results = []
loop_condition = True
n = 0
max_iter=100

# Each loop iteration is an interval
while loop_condition:
    loop_condition = False
    # Clearing variables:
    for sample in samples:
        sample[:] = []
    in_pkts = 0
    out_pkts = 0
    drop_pkts = 0
    link_pkts = 0
    # Sample incoming traffic
    for (i, pcap) in enumerate(pcaps):
        if pkts[i]:
            loop_condition = True
        while pkts[i] and pkts[i].time < next_interval:
            pkt = pkts[i]
            packet_hash = get_hash(pkt, key)
            if packet_hash & mask < threshold:
                samples[i].append(packet_hash)
            if is_in[i]:
                in_pkts += 1
            else:
                out_pkts += 1
                dst = pkt.getfieldval('dst')
                if dst == '00:00:00:00:00:00':
                    drop_pkts += 1
                elif dst in ['00:00:00:00:00:01','00:00:00:00:00:02'] :
                    link_pkts += 1
            try:
                pkts[i] = pcap.next()
            except StopIteration: pkts[i] = None
    
    # Sum all in samples substract all out samples
    sample_in = []
    sample_out = []
    lengths = []
    for (i, sample) in enumerate(samples):
        lengths.append(len(sample))
        if is_in[i]:
            sample_in.extend(sample)
        else:
            sample_out.extend(sample)
    not_sent = list(sample_in)
    not_received = list(sample_out)
    for pkt in sample_out:
        try:
            not_sent.remove(pkt)
        except ValueError: 
            pass
    for pkt in sample_in:
        try:
            not_received.remove(pkt)
        except ValueError:
            pass
    sample_diff = not_sent + not_received
    
    # Compute every result statistic:
    estimated_sent = len(sample_in) * (2 ** hash_length)/threshold
    estimated_received = len(sample_out) * (2 ** hash_length)/threshold
    estimated_missing_packets = len(sample_diff) * (2 ** hash_length)/threshold
    
    lengths = "\"" + str(lengths) + "\""
    result = (  in_pkts, out_pkts, drop_pkts, link_pkts, estimated_sent, 
                estimated_received, estimated_missing_packets, lengths,
                interval, hash_length, threshold)
    results.append(result)
    n += 1
    next_interval += interval
    if n > max_iter:
        break

results_dtype = [ ('InPackets', 'float'), 
                  ('OutPackets', 'float'), 
                  ('MaliciousDrop', 'float'), 
                  ('LinkDrop', 'float'), 
                  ('EstimatedIn', 'float'), 
                  ('EstimatedOut', 'float'),
                  ('EstimatedDifference', 'float'), 
                  ('NumHashes', '|S96'), 
                  ('Interval', 'float'), 
                  ('HashLength', 'float'), 
                  ('Threshold', 'float')]
result = np.array(results, results_dtype)
np.savetxt("%s/%s_%s_results.csv" % (result_path, basename(pattern), _id), result, delimiter=',', 
            fmt="%s", header=",".join(result.dtype.names), comments='')


