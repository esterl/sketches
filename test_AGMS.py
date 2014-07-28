from network_sketches import *
import sketches
import sys
import numpy as np
import time

########################## Test the error vs columns ###########################
# Variables
sketch_type = sys.argv[1]
random_generator = sys.argv[2]
hash_length = int(sys.argv[3])
max_iter = int(sys.argv[4])
pcap = sys.argv[5]
run_id = int(sys.argv[6])
num_cols = [16,32,64,128,256,512,1024]
rows = 14
num_pkts = 100
drop_probability = 0.1
results = None

for columns in num_cols:
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
    net_sketch = NetworkSketch(sketch)
    start_time = time.time()
    result = net_sketch.test(pcap, drop_probability, is_random=False, max_iter=max_iter, num_packets=num_pkts)
    elapsed_time = time.time() - start_time
    print elapsed_time
    if results is None:
        results = result
    else:
        results = np.hstack((results, result))

header = ",".join(results.dtype.names)
np.savetxt("/Data/Sketches/AGMS/test_columns_%s_%s_%i_%i_%i.csv" % (sketch_type, random_generator, hash_length, num_pkts, run_id), results, delimiter=',', fmt="%s", header=header)

########################## Test the error vs rows ##############################
# Variables
columns = 512
num_rows = [2,4,8,16,32,64]
num_pkts = 100
drop_probability = 0.1
results = None

for rows in num_rows:
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
    net_sketch = NetworkSketch(sketch)
    start_time = time.time()
    result = net_sketch.test(pcap, drop_probability, is_random=False, max_iter=max_iter, num_packets=num_pkts)
    elapsed_time = time.time() - start_time
    print elapsed_time
    if results is None:
        results = result
    else:
        results = np.hstack((results, result))

header = ",".join(results.dtype.names)
np.savetxt("/Data/Sketches/AGMS/test_rows_%s_%s_%i_%i_%i.csv" % (sketch_type, random_generator, hash_length, num_pkts, run_id), results, delimiter=',', fmt="%s", header=header)

########################## Test the error vs F2 ################################
# Variables
#columns = 512
#rows = [2,4,8,16,32,64]
#num_pkts = 100
#probabilities = [0., 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
#max_iter = int(sys.argv[6])
#results = None

#for rows in num_rows:
#    if hash_length==8:
#        sketch = sketches.AGMS8(columns, rows, random_generator)
#    elif hash_length==16:
#        sketch = sketches.AGMS16(columns, rows, random_generator)
#    elif hash_length==32:
#        sketch = sketches.AGMS32(columns, rows, random_generator)
#    elif hash_length==64:
#        sketch = sketches.AGMS64(columns, rows, random_generator)
#    elif hash_length==128:
#        sketch = sketches.AGMS128(columns, rows, random_generator)
#    else:
#        raise AttributeError('Hash length not valid: %i' % hash_length)
#    net_sketch = NetworkSketch(sketch)
#    start_time = time.time()
#    result = net_sketch.test(pcap, drop_probability, is_random=False, max_iter=max_iter, num_packets=num_pkts)
#    elapsed_time = time.time() - start_time
#    print elapsed_time
#    if results is None:
#        results = result
#    else:
#        results = np.hstack((results, result))

#header = ",".join(results.dtype.names)
#np.savetxt("/Data/Sketches/AGMS/test_rows_%s_%s_%i_%i_%i.csv" % (sketch_type, random_generator, hash_length, num_pkts, run_id), results, delimiter=',', fmt="%s", header=header)
