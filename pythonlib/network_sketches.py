import hashlib
import random
from scipy.stats import binom
import warnings
from scapy.all import *

def record_results(sketch_in, sketch_out):
    est_difference_1st = sketch_in.estimate_first_moment(sketch_out)
    est_difference_2nd = sketch_in.difference(sketch_out)
    est_input_1st = sketch_in.sketch.first_moment()
    est_output_1st = sketch_out.sketch.first_moment()
    est_input_2nd = sketch_in.sketch.second_moment()
    est_output_2nd = sketch_out.sketch.second_moment()
    return (est_difference_1st, est_difference_2nd, est_input_1st, 
                est_output_1st, est_input_2nd, est_output_2nd)
    
    
    
# TODO: Find a name for the block
class NetworkSketch():
    #2-way hash
    #sketch
    #buffer
    
    def __init__(self,sketch, keys=[0L], mask=None):
        self.buffer = []
        self.sketch = sketch
        self.keys=keys
        if mask is None:
            self.mask = (1L << sketch.get_key_size() ) -1
        else:
            self.mask = mask
    
    def change_key(self,new_keys):
        self.keys = new_keys
    
    def clear(self):
        self.buffer = []
        self.sketch.clear()
    
    def get_sketch(self):
        for packet in self.buffer:
            self.update(packet)
        self.buffer=[]
        return self.sketch
    
    def get_columns(self):
        return self.sketch.get_columns()
    
    def get_rows(self):
        return self.sketch.get_rows()
    
    def buffer_packet(self, pkt):
        self.buffer.append(long(hashlib.md5(str(pkt['IP'])).hexdigest(),base=16))
    
    def update(self, pkt):
        while not pkt.name in ['IPv6', 'IP']:
            pkt = pkt.getlayer(1)
        if pkt.name == 'IPv6':
            pkt.setfieldval('hlim', 0)
        elif pkt.name == 'IP':
            pkt.setfieldval('ttl', 0)
        try:
            packet_hash = hashlib.sha256(pkt.__str__()[0:len(pkt.original)]).hexdigest()
        except TypeError:
            warnings.warn('Packet error')
            packet_hash = hashlib.sha256(pkt.original).hexdigest()
        slice_len = 256/len(self.keys)
        # Since the string its in HEX each char is 4 bits
        ini_range = xrange(0, 256/4, slice_len/4)
        end_range = xrange(slice_len/4, 256/4+1, slice_len/4)
        for ini,end,key in zip(ini_range, end_range, self.keys):
            hash_slice = long(packet_hash[ini:end], base=16)
            self.sketch.update(str((hash_slice^key)&self.mask), 1.)
    
    # Shorter version that doesn't care about fixing TTL and so on
    def lazy_update(self, pkt):
        str_random = os.urandom(max(pkt.len-len(pkt.original),0))
        packet_hash = hashlib.sha256(pkt.original + str_random).hexdigest()
        slice_len = 256/len(self.keys)
        # Since the string its in HEX each char is 4 bits
        ini_range = xrange(0, 256/4, slice_len/4)
        end_range = xrange(slice_len/4, 256/4+1, slice_len/4)
        for ini,end,key in zip(ini_range, end_range, self.keys):
            hash_slice = long(packet_hash[ini:end], base=16)
            self.sketch.update(str((hash_slice^key)&self.mask), 1.)
    
    def second_moment(self):
        return self.sketch.second_moment()
    
    def estimate_first_moment(self, other):
        return self.sketch.difference(other.sketch).first_moment()
    
    def difference(self, other):
        return self.sketch.difference(other.sketch).second_moment()
    
    def inner_product(self, other):
        return self.sketch.inner_product(other.sketch)
    
    def __iadd__(self, other):
        self.sketch = self.sketch.__iadd__(other.sketch)
        return self
    
    def copy(self):
        result = NetworkSketch(self.sketch.copy(), self.keys)
        return result
    
    def detect_link(self, other, threshold, loss_probability, incoming=True, nmax = 0):
        difference = self.difference(other)
        if difference < nmax: return False
        if incoming: 
            received = self.second_moment()
            sent = received + difference
        else:
            sent = self.second_moment()
            received = self.second_moment() - difference
        return binom.cdf(received, round(sent), 1-loss_probability) < threshold
    
    def corrupt(self, other, threshold, loss_probability, incoming, nmax=0):
        max_val = 1L << self.sketch.get_key_size()
        random_value = None
        while not other.detect_link(self, threshold, loss_probability, incoming, nmax):
            random_value = random.randrange(max_val)
            self.sketch.update(str(random_value), 1.)
        if random_value:
            self.sketch.update(str(random_value), -1.)
    
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
            except (TypeError, ValueError, AttributeError):
                print pkt
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
    
    def test_base(self, pcap, time_interval=None, num_packets=None, max_iter=10):
        import numpy as np
        if time_interval is None:
            if num_packets is None:
                return
            time=False
        else:
            time=True
        rows = self.sketch.get_rows()
        columns = self.sketch.get_columns()
        pkts = PcapReader(pcap)
        start_time = pkts.next().time
        pkts.close()
        pkts = PcapReader(pcap)
        if time: max_t = start_time + time_interval
        sketched_packets = 0.
        results = []
        iters = 0
        next_iter = False
        for pkt in pkts:
            # Check test condition
            if time:
                if pkt.time>max_t:
                    next_iter = True
                    # Update max_t
                    while pkt.time>max_t:
                        max_t += time_interval
            else:
                if sketched_packets >= num_packets:
                    next_iter = True
            if next_iter:
                iters +=1
                estimation = self.second_moment()
                result = (sketched_packets, estimation, time_interval, rows, 
                                columns, str(self.sketch.__class__))
                results.append(result)
                self.clear()
                sketched_packets = 0
                next_iter = False
            # Enough iterations?
            if iters >= max_iter:
                break
            # Update sketches:
            self.lazy_update(pkt)
            sketched_packets += 1
        results_dtype = [ ('SketchedPackets', 'float'), 
                          ('EstimatedPackets', 'float'),
                          ('TimeInterval', 'float'), 
                          ('SketchRows', 'float'), 
                          ('SketchColumns', 'float'), 
                          ('SketchClass','|S96')]
        return np.array(results, results_dtype)
