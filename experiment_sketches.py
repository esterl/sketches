#! /usr/bin/env python
from scapy.all import *
import random
import os
import numpy as np
import hashlib

""" Tests a sketch """
# TODO Obtain rows and columns from sketch
def test_drop(sketch, rows, columns, interval=0.001, max_tests=10):
    pkts = PcapReader('../equinix-chicago.dirB.20130529-135900.UTC.anon.pcap')
    start_time = pkts.next().time
    pkts.close()
    pkts = PcapReader('../equinix-chicago.dirB.20130529-135900.UTC.anon.pcap')
    drop_probabilities = [0., 0.1, 0.2, 0.3, 0.5, 0.8, 1.]
    max_t = start_time + interval;
    sketches_out = [ sketch.copy() for drop_probability in drop_probabilities ]
    input_packets = 0.
    output_packets = [ 0. for drop_probability in drop_probabilities ]
    results = []
    n = 0
    for pkt in pkts:
        try:
            complete_pkt = pkt/Raw(os.urandom(pkt.len-len(pkt)))
        except (TypeError, ValueError):
            continue
        if pkt.time>max_t:
            # Record the result:
            n += 1
            for i in range(len(drop_probabilities)):
                sketch_out = sketches_out[i]
                drop_probability = float(input_packets - output_packets[i]) / input_packets
                est_dropped = np.sqrt(sketch.estimate_difference(sketch_out))
                est_probability = float(est_dropped) / np.sqrt(sketch.sketch.second_moment())
                sketch_out.clear()
                result = ( drop_probability, est_probability, input_packets, 
                           output_packets[i], input_packets - output_packets[i],
                           est_dropped, interval, rows, columns, 
                           str(sketch.sketch.__class__))
                results.append(result)
                # Restart counters
                output_packets[i] = 0
            input_packets = 0
            sketch.clear()
            #update the time
            while pkt.time>max_t:
                max_t += interval
        # Update the sketches:
        # TODO complete_packet should be a string
        sketch.update(complete_pkt)
        input_packets += 1
        for i in range(len(drop_probabilities)):
            if random.random() > drop_probabilities[i]:
                output_packets[i] += 1
                sketches_out[i].update(complete_pkt)
        if n>max_tests:
            break
    results_dtype = [ ('DropProbability', 'float'), 
                      ('EstimatedProbability', 'float'),
                      ('NumPacketsIn', 'float'), 
                      ('NumPacketsOut', 'float'), 
                      ('NumPacketsLost', 'float'), 
                      ('EstimatedLostPackets', 'float'), 
                      ('TimeInterval', 'float'), 
                      ('SketchRows', 'float'), 
                      ('SketchColumns', 'float'), 
                      ('SketchClass','|S96')]
    return np.array(results, results_dtype)

def test_corrupt(sketch, rows, columns, interval=0.001, max_tests=10):
    pkts = PcapReader('../equinix-chicago.dirB.20130529-135900.UTC.anon.pcap')
    start_time = pkts.next().time
    pkts.close()
    pkts = PcapReader('../equinix-chicago.dirB.20130529-135900.UTC.anon.pcap')
    corrupt_probabilities = [0., 0.1, 0.2, 0.3, 0.5, 0.8, 1.]
    max_t = start_time + interval;
    sketches_out = [ sketch.copy() for corrupt_probability in corrupt_probabilities ]
    input_packets = 0.
    corrupt_packets = [ 0. for corrupt_probability in corrupt_probabilities ]
    results = []
    n = 0
    for pkt in pkts:
        try:
            complete_pkt = pkt/Raw(os.urandom(pkt.len-len(pkt)))
        except (TypeError, ValueError):
            continue
        if pkt.time>max_t:
            # Record the result:
            n += 1
            for i in range(len(corrupt_probabilities)):
                sketch_out = sketches_out[i]
                corrupt_probability = float(corrupt_packets[i]) / input_packets
                est_corrupt = np.sqrt(sketch.estimate_difference(sketch_out))
                est_probability = float(est_corrupt) / np.sqrt(sketch.sketch.second_moment())
                sketch_out.clear()
                result = ( corrupt_probability, est_probability, input_packets, 
                           corrupt_packets[i], input_packets - corrupt_packets[i],
                           est_corrupt, interval, rows, columns, 
                           str(sketch.sketch.__class__))
                results.append(result)
                # Restart counters
                corrupt_packets[i] = 0
            input_packets = 0
            sketch.clear()
            #update the time
            while pkt.time>max_t:
                max_t += interval
        # Update the sketches:
        # TODO complete_packet should be a string
        sketch.update(complete_pkt)
        input_packets += 1
        for i in range(len(corrupt_probabilities)):
            if random.random() > corrupt_probabilities[i]:
                sketches_out[i].update(complete_pkt)
            else:
                corrupt_packets[i] += 1
                corrupt_pkt = pkt/Raw(os.urandom(pkt.len-len(pkt)))
                sketches_out[i].update(corrupt_pkt)
        if n>max_tests:
            break
    results_dtype = [ ('CorruptProbability', 'float'), 
                      ('EstimatedProbability', 'float'),
                      ('NumPacketsIn', 'float'), 
                      ('NumPacketsCorrupt', 'float'), 
                      ('NumPacketsOk', 'float'), 
                      ('EstimatedCorruptPackets', 'float'), 
                      ('TimeInterval', 'float'), 
                      ('SketchRows', 'float'), 
                      ('SketchColumns', 'float'), 
                      ('SketchClass','|S96')]
    return np.array(results, results_dtype)
