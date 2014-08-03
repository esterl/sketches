from network_sketches import *
import sketches
import sys
import numpy as np
import time

def get_sketch(sketch_type, hash_length, columns, rows, random_generator):
    if sketch_type=='AGMS':
        if hash_length==8:
            sketch = sketches.AGMS8(columns, rows, random_generator)
        elif hash_length==16:
            sketch = sketches.AGMS16(columns, rows, random_generator)
        elif hash_length==32:
            sketch = sketches.AGMS32(columns, rows, random_generator)
        elif hash_length==64:
            sketch = sketches.AGMS64(columns, rows, random_generator)
        elif hash_length==128:
            sketch = sketches.AGMS128(columns, rows, random_generator)
        else:
            raise AttributeError('Hash length not valid: %i' % hash_length)
    elif sketch_type=='FAGMS':
        if hash_length==8:
            sketch = sketches.FAGMS8(columns, rows, random_generator)
        elif hash_length==16:
            sketch = sketches.FAGMS16(columns, rows, random_generator)
        elif hash_length==32:
            sketch = sketches.FAGMS32(columns, rows, random_generator)
        elif hash_length==64:
            sketch = sketches.FAGMS64(columns, rows, random_generator)
        elif hash_length==128:
            sketch = sketches.FAGMS128(columns, rows, random_generator)
        else:
            raise AttributeError('Hash length not valid: %i' % hash_length)
    elif sketch_type=='FastCount':
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
    elif sketch_type=='CountMin':
        if hash_length==8:
            sketch = sketches.CountMin8(columns, rows, random_generator)
        elif hash_length==16:
            sketch = sketches.CountMin16(columns, rows, random_generator)
        elif hash_length==32:
            sketch = sketches.CountMin32(columns, rows, random_generator)
        elif hash_length==64:
            sketch = sketches.CountMin64(columns, rows, random_generator)
        elif hash_length==128:
            sketch = sketches.CountMin128(columns, rows, random_generator)
        else:
            raise AttributeError('Hash length not valid: %i' % hash_length)
    else:
        raise AttributeError('Sketch type not valid: %s' % sketch_type)
    return sketch


########################## Test the error vs columns ###########################
# Variables
sketch_type = sys.argv[1]
random_generator = sys.argv[2]
hash_length = int(sys.argv[3])
max_iter = int(sys.argv[4])
pcap = sys.argv[5]
run_id = sys.argv[6]
num_cols = [16,32,64,128,256,512,1024]
rows = 14
num_pkts = 100
drop_probability = 0.1
results = None
save_folder = 'output/'

for columns in num_cols:
    sketch = get_sketch(sketch_type, hash_length, columns, rows, random_generator)
    net_sketch = NetworkSketch(sketch)
    start_time = time.time()
    result = net_sketch.test(pcap, drop_probability, is_random=False, max_iter=max_iter, num_packets=num_pkts)
    elapsed_time = time.time() - start_time
    print elapsed_time
    if results is None:
        results = result
    else:
        results = np.hstack((results, result))

header = ','.join(results.dtype.names)
np.savetxt(save_floder + 'test_columns_%s_%s_%i_%i_%s.csv' % (sketch_type, random_generator, hash_length, num_pkts, run_id), results, delimiter=',', fmt="%s", header=header)

########################## Test the error vs rows ##############################
# Variables
columns = 512
num_rows = [2,4,8,16,32,64]
num_pkts = 100
drop_probability = 0.1
results = None

for rows in num_rows:
    sketch = get_sketch(sketch_type, hash_length, columns, rows, random_generator)
    net_sketch = NetworkSketch(sketch)
    start_time = time.time()
    result = net_sketch.test(pcap, drop_probability, is_random=False, max_iter=max_iter, num_packets=num_pkts)
    elapsed_time = time.time() - start_time
    print elapsed_time
    if results is None:
        results = result
    else:
        results = np.hstack((results, result))

header = ','.join(results.dtype.names)
np.savetxt(save_floder + 'test_rows_%s_%s_%i_%i_%s.csv' % (sketch_type, random_generator, hash_length, num_pkts, run_id), results, delimiter=',', fmt="%s", header=header)

########################## Test the error vs F2 ################################
# Variables
columns = 256
rows = 32
num_pkts = 1000
probabilities = [0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5]
results = None

for drop_probability in probabilities:
    sketch = get_sketch(sketch_type, hash_length, columns, rows, random_generator)
    net_sketch = NetworkSketch(sketch)
    start_time = time.time()
    result = net_sketch.test(pcap, drop_probability, is_random=False, max_iter=max_iter, num_packets=num_pkts)
    elapsed_time = time.time() - start_time
    print elapsed_time
    if results is None:
        results = result
    else:
        results = np.hstack((results, result))

header = ','.join(results.dtype.names)
np.savetxt(save_folder + 'test_F2_%s_%s_%i_%i_%s.csv' % (sketch_type, random_generator, hash_length, num_pkts, run_id), results, delimiter=',', fmt="%s", header=header)
