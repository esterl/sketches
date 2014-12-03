from scapy.all import *
import sketches
import numpy as np
from network_sketches import *
import glob
from os.path import basename

def get_sketch(hash_length, columns, rows, random_generator):
    if hash_length==8:
        sketch = sketches.FastCount8(columns, rows, random_generator)
    elif hash_length==16:
        sketch = sketches.FastCount16(columns, rows, random_generator)
    elif hash_length==32:
        sketch = sketches.FastCount32(columns, rows, random_generator)
    elif hash_length==64:
        sketch = sketches.FastCount64(columns, rows, random_generator)
    elif hash_length==128:
        sketch = sketches.FastCount128(columns, rows, random_generator)
    else:
        raise AttributeError('Hash length not valid: %i' % hash_length)
    return sketch

# pcap_files = [ '00:0c:42:0a:6c:42_in.pcap', '00:15:6d:f8:32:02_in.pcap', 'net_in.pcap', '00:0c:42:0a:6c:42_out.pcap', '00:15:6d:f8:32:02_out.pcap', 'd4:ca:6d:4c:07:d8_out.pcap', 'net_out.pcap' ]
pattern = sys.argv[1]
_id = sys.argv[2]
num_cols = int(sys.argv[3])
num_rows = int(sys.argv[4])
hash_length = int(sys.argv[5])

in_pcap_files = glob.glob('%s_??:??:??:??:??:??_in.pcap' % pattern)
out_pcap_files = glob.glob('%s_??:??:??:??:??:??_out.pcap' % pattern)
pcaps = [ PcapReader(filename) for filename in in_pcap_files + out_pcap_files ]
pkts = [ pcap.next() for pcap in pcaps ]
is_in = [ True for pcap in in_pcap_files ] + [ False for pcap in out_pcap_files ]
sketch = get_sketch(hash_length, num_cols, num_rows, 'tab')
net_sketch = NetworkSketch(sketch)
sketches = [ net_sketch.copy() for pcap in pcaps ]
start_time = min([ pkt.time for pkt in pkts ])
interval = float(sys.argv[6])
result_path = sys.argv[7]
desync_dev = float(sys.argv[8])*interval
# Supose there is always in for out and in the same order
desync = [ random.gauss(0, desync_dev) for filename in in_pcap_files ]
desync = desync + desync
max_desync = max(desync) - min(desync)
next_interval = start_time + interval
previous_in = net_sketch.copy()
previous_out = net_sketch.copy()
sketch_in = net_sketch.copy()
sketch_out = net_sketch.copy()
next_in = net_sketch.copy()
next_out = net_sketch.copy()

columns = sketch.get_columns()
rows = sketch.get_rows()
results = []
loop_condition = True
n = 0
in_pkts = [0, 0, 0]
out_pkts = [0, 0, 0]
drop_pkts = [0, 0, 0]
link_pkts = [0, 0, 0]
max_iter=31
while loop_condition:
    loop_condition = False
    for sketch in sketches:
        sketch.clear()
    for (i, pcap) in enumerate(pcaps):
        if pkts[i]:
            loop_condition = True
        while pkts[i] and pkts[i].time < next_interval + desync[i]:
            pkt = pkts[i]
            sketches[i].update(pkt)
            if is_in[i]:
                in_pkts[2] += 1
            else:
                out_pkts[2] += 1
                dst = pkt.getfieldval('dst')
                if dst == '00:00:00:00:00:00':
                    drop_pkts[2] += 1
                elif dst in ['00:00:00:00:00:01','00:00:00:00:00:02'] :
                    link_pkts[2] += 1
            try: 
                pkts[i] = pcap.next()
            except StopIteration: pkts[i] = None
    
    # Move the sketches
    previous_in = sketch_in
    previous_out = sketch_out
    sketch_in = next_in
    sketch_out = next_out
    # Sum all in sketches substract all out sketches
    next_in = net_sketch.copy()
    next_out = net_sketch.copy()
    for (i, sketch) in enumerate(sketches):
        if is_in[i]:
            next_in += sketch
        else:
            next_out += sketch
    
    # Compute every result statistic:
    estimated_sent = sketch_in.second_moment()
    estimated_received = sketch_out.second_moment()
    estimated_missing_packets = sketch_in.difference(sketch_out)
    intersection_previous_in = sketch_out.inner_product(previous_in)
    intersection_previous_out = sketch_in.inner_product(previous_out)
    intersection_next_in = sketch_out.inner_product(next_in)
    intersection_next_out = sketch_in.inner_product(next_out)
    dup_previous_in = sketch_in.inner_product(previous_in)
    dup_next_in = sketch_in.inner_product(next_in)
    dup_previous_out = sketch_out.inner_product(next_out)
    dup_next_out = sketch_out.inner_product(next_out)
    max_value = max(sketch_in.sketch.get_max(), sketch_out.sketch.get_max())
    result = (  in_pkts[1], out_pkts[1], drop_pkts[1], link_pkts[1], estimated_sent, 
                estimated_received, estimated_missing_packets, 
                intersection_previous_in, intersection_previous_out, 
                intersection_next_in, intersection_next_out, dup_previous_in, 
                dup_next_in, dup_previous_out, dup_next_out, max_value, 
                interval, rows, columns, str(sketch_in.sketch.__class__), desync_dev, max_desync)
    results.append(result)
    n += 1
    next_interval += interval
    in_pkts.pop(0)
    in_pkts.append(0)
    out_pkts.pop(0)
    out_pkts.append(0)
    drop_pkts.pop(0)
    drop_pkts.append(0)
    link_pkts.pop(0)
    link_pkts.append(0)
    if n > max_iter:
        break

results_dtype = [ ('InPackets', 'float'), 
                  ('OutPackets', 'float'), 
                  ('MaliciousDrop', 'float'), 
                  ('LinkDrop', 'float'), 
                  ('EstimatedIn', 'float'), 
                  ('EstimatedOut', 'float'),
                  ('EstimatedDifference', 'float'), 
                  ('IntersectionPreviousIn', 'float'), 
                  ('IntersectionPreviousOut', 'float'), 
                  ('IntersectionNextIn', 'float'), 
                  ('IntersectionNextOut', 'float'), 
                  ('DupPreviousIn', 'float'), 
                  ('DupNextIn', 'float'), 
                  ('DupPreviousOut', 'float'), 
                  ('DupNextOut', 'float'), 
                  ('MaxValue', 'float'), 
                  ('Interval', 'float'), 
                  ('SketchRows', 'float'), 
                  ('SketchColumns', 'float'), 
                  ('SketchClass', '|S96'),
                  ('DesyncDev', 'float'), 
                  ('MaxDesync', 'float')]
result = np.array(results, results_dtype)
np.savetxt("%s/%s_%s_results.csv" % (result_path, basename(pattern), _id), result, delimiter=',', 
            fmt="%s", header=",".join(result.dtype.names), comments='')


