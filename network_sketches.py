import hashlib

def record_results(sketch_in, sketch_out):
    est_difference_1st = sketch_in.estimate_first_moment(sketch_out)
    est_difference_2nd = sketch_in.estimate_second_moment(sketch_out)
    est_input_1st = sketch_in.sketch.first_moment()
    est_output_1st = sketch_out.sketch.second_moment()
    est_input_2nd = sketch_in.sketch.second_moment()
    est_output_2nd = sketch_out.sketch.second_moment()
    return (est_difference_1st, est_difference_2nd, est_input_1st, 
                est_output_1st, est_input_2nd, est_output_2nd)
    
    
    
    
# TODO: Find a name for the block
class NetworkSketch():
    #2-way hash
    #sketch
    #buffer
    
    def __init__(self,sketch, key=0L, mask=None):
        self.buffer = []
        self.sketch = sketch
        self.key=key
        if mask is None:
            self.mask = (1L << sketch.get_key_size() ) -1
        else:
            self.mask = mask
    
    def change_key(self,new_key):
        self.key = new_key
    
    def clear(self):
        self.buffer = []
        self.sketch.clear()
    
    def get_sketch(self):
        for packet in self.buffer:
            self.sketch.update(str((packet^self.key)&self.mask),1)
        self.buffer=[]
        return self.sketch
    
    def get_columns(self):
        return self.sketch.get_columns()
    
    def get_rows(self):
        return self.sketch.get_rows()
    
    def buffer_packet(self, pkt):
        self.buffer.append(long(hashlib.md5(str(pkt['IP'])).hexdigest(),base=16))
    
    def update(self, pkt):
	if pkt.name == 'Ethernet':
            pkt = pkt.getlayer(1)
        if pkt.name == 'IPv6':
            pkt.setfieldval('hlim', 0)
        elif pkt.name == 'IP':
            pkt.setfieldval('ttl', 0)
        packet_hash = long(hashlib.md5(str(pkt)).hexdigest(),base=16)
        self.sketch.update(str((packet_hash^self.key)&self.mask),1.)
    
    def second_moment(self):
        return self.sketch.second_moment()
    
    def estimate_first_moment(self, other):
        return self.sketch.difference(other.sketch).first_moment()
    
    def difference(self, other):
        return self.sketch.difference(other.sketch).second_moment()
    
    def __iadd__(self, other):
        self.sketch = self.sketch.__iadd__(other.sketch)
        return self
    
    def copy(self):
        result = NetworkSketch(self.sketch.copy(), self.key)
        return result
    
    def test(self, pcap, drop_probability, is_random=True, time_interval=None, 
                num_packets=None, max_iter=10):
        import numpy as np
        import random
        from scapy.all import *
        import os
        if time_interval is not None:
            if not is_random: return
            time=True
        elif num_packets is not None: time=False
        else: return
        rows = self.sketch.get_rows()
        columns = self.sketch.get_columns()
        pkts = PcapReader(pcap)
        start_time = pkts.next().time
        pkts.close()
        pkts = PcapReader(pcap)
        if time_interval is not None: max_t = start_time + time_interval
        sketch_out = self.copy()
        input_packets = 0.
        output_packets = 0.
        results = []
        iters = 0
        for pkt in pkts:
            try:
                complete_pkt = pkt/Raw(os.urandom(max(pkt.len-len(pkt),0)))
            except (TypeError, ValueError):
                continue
            # Check test condition
            if time:
                if pkt.time>max_t:
                    iters += 1
                    # Record results:
                    result = record_results(self, sketch_out)
                    result = (input_packets, output_packets) + result + (time_interval, rows, columns, str(self.sketch.__class__))
                    results.append(result)
                    self.clear()
                    sketch_out.clear()
                    input_packets = 0
                    output_packets = 0
                    while pkt.time>max_t:
                        max_t += time_interval
            else:
                if input_packets >= num_packets:
                    iters += 1
                    result = record_results(self, sketch_out)
                    result = (input_packets, output_packets) + result + (time_interval, rows, columns, str(self.sketch.__class__))
                    results.append(result)
                    self.clear()
                    sketch_out.clear()
                    input_packets = 0
                    output_packets = 0
            # Enough iterations?
            if iters >= max_iter:
                break
            # Update sketches:
            self.update(complete_pkt)
            input_packets += 1
            # Drop the packet?
            if not is_random:
                # Drop the first num_packets*drop_probability:
                if input_packets > num_packets*drop_probability:
                    output_packets += 1
                    sketch_out.update(complete_pkt)
            else:
                if random.random() > drop_probability:
                    output_packets += 1
                    sketch_out.update(complete_pkt)
        results_dtype = [ ('InputPackets', 'float'), 
                          ('OutputPackets', 'float'),
                          ('EstimatedDifference1', 'float'), 
                          ('EstimatedDifference2', 'float'), 
                          ('EstimatedInput1', 'float'), 
                          ('EstimatedOutput1', 'float'), 
                          ('EstimatedInput2', 'float'),
                          ('EstimatedOutput2', 'float'),
                          ('TimeInterval', 'float'), 
                          ('SketchRows', 'float'), 
                          ('SketchColumns', 'float'), 
                          ('SketchClass','|S96')]
        return np.array(results, results_dtype)

