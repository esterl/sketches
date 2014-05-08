from network_sketches import *
import sketches
from experiment_sketches import *

"""
Short version

from network_sketches import *
import sketches
from experiment_sketches import *



sketch = sketches.FastCount64(16,16)
net_sketch = NetworkSketch(sketch)
results = test_corrupt(net_sketch, 16, 16, max_tests=1)
"""
############################ Different hash size ###############################
# Test FastCount8:
#for i in range(10):
#    sketch = sketches.FastCount8(16,16)
#    net_sketch = NetworkSketch(sketch)
#    results = test_corrupt(net_sketch, 16, 16)

# Test FastCount16:
for i in range(10):
    sketch = sketches.FastCount16(16,16)
    net_sketch = NetworkSketch(sketch)
#    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))
    results = test_corrupt(net_sketch, 16,16)

print 1

# Test FastCount32:
for i in range(10):
    sketch = sketches.FastCount32(16,16)
    net_sketch = NetworkSketch(sketch)
    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))

print 2

# Test FastCount64:
for i in range(10):
    sketch = sketches.FastCount64(16,16)
    net_sketch = NetworkSketch(sketch)
    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))

print 3

# Test FastCount128:
for i in range(10):
    sketch = sketches.FastCount128(16,16)
    net_sketch = NetworkSketch(sketch)
    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))

np.savetxt("corrupt_hash_16-16.cvs", results, delimiter=',', fmt="%s")

print "End first tests"

"""
from network_sketches import *
import sketches
from experiment_sketches import *
b = sketches.FastCount32(16,16)
a = NetworkSketch(b)
results = test_sketch(a, 16,16, max_tests=1, interval=0.0001)
"""

############################ Different sketch size #############################

num_buckets = [4, 8 , 16]
num_rows = [4, 8, 16]
results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FastCount16(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_corrupt(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_corrupt(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount16.cvs", results, delimiter=',', fmt="%s")

print 1

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FastCount32(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_corrupt(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_corrupt(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount32.cvs", results, delimiter=',', fmt="%s")

print 2 

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FastCount64(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_corrupt(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_corrupt(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount64.cvs", results, delimiter=',', fmt="%s")

print 3

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FastCount128(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_corrupt(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_corrupt(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount128.cvs", results, delimiter=',', fmt="%s")

print 4
print "End second test"
