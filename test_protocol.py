from scapy.all import *
import sketches
import numpy as np
from network_sketches import *
import glob
from os.path import basename

# pcap_files = [ '00:0c:42:0a:6c:42_in.pcap', '00:15:6d:f8:32:02_in.pcap', 'net_in.pcap', '00:0c:42:0a:6c:42_out.pcap', '00:15:6d:f8:32:02_out.pcap', 'd4:ca:6d:4c:07:d8_out.pcap', 'net_out.pcap' ]
pattern = sys.argv[1]
_id = sys.argv[2]

in_pcap_files = glob.glob('%s_*:*_in.pcap' % pattern)
out_pcap_files = glob.glob('%s_*:*_out.pcap' % pattern)
pcaps = [ PcapReader(filename) for filename in in_pcap_files + out_pcap_files ]
pkts = [ pcap.next() for pcap in pcaps ]
is_in = [ True for pcap in in_pcap_files ] + [ False for pcap in out_pcap_files ]
sketch = sketches.FastCount32(32,32, 'tab')
net_sketch = NetworkSketch(sketch)
sketches = [ net_sketch.copy() for pcap in pcaps ]
start_time = min([ pkt.time for pkt in pkts ])
interval = float(sys.argv[3])
result_path = sys.argv[4]
desync_dev = float(sys.argv[5])*interval
# Supose there is always in for out and in the same order
desync = [ random.gauss(0, desync_dev) for filename in in_pcap_files ]
desync = desync + desync
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
prevn = 0
while loop_condition:
    min_time = float("inf")
    for pkt in pkts:
        try: min_time = min(min_time, pkt.time)
        except AttributeError: pass
    print n, min_time, next_interval
    loop_condition = False
    for sketch in sketches:
        sketch.clear()
    for (i, pcap) in enumerate(pcaps):
        if pkts[i]:
            loop_condition = True
        while pkts[i] and pkts[i].time < next_interval + desync[i]:
            n += 1
            pkt = pkts[i]
            sketches[i].update(pkt)
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
    max_value = max(sketch_in.sketch.get_max(), sketch_out.sketch.get_max())
    result = (estimated_sent, estimated_received, estimated_missing_packets, 
                intersection_previous_in, intersection_previous_out, 
                intersection_next_in, intersection_next_out, max_value, 
                interval, rows, columns, str(sketch_in.sketch.__class__))
    results.append(result)
    next_interval += interval

results_dtype = [ ('EstimatedIn', 'float'), 
                  ('EstimatedOut', 'float'),
                  ('EstimatedDifference', 'float'), 
                  ('IntersectionPreviousIn', 'float'), 
                  ('IntersectionPreviousOut', 'float'), 
                  ('IntersectionNextIn', 'float'), 
                  ('IntersectionNextOut', 'float'), 
                  ('MaxValue', 'float'), 
                  ('Interval', 'float'), 
                  ('SketchRows', 'float'), 
                  ('SketchColumns', 'float'), 
                  ('SketchClass', '|S96')]
result = np.array(results, results_dtype)
np.savetxt("%s/%s_%s_results.csv" % (result_path, basename(pattern), _id), result, delimiter=',', 
            fmt="%s", header=",".join(result.dtype.names), comments='')


