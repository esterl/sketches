from network_sketches import *
import sketches
from experiment_sketches import *

"""
Short version

from network_sketches import *
import sketches
from experiment_sketches import *

sketch = sketches.CountMin64(16,16)
aux = sketch.copy()
sketch.update(4,2)
aux.update(5,2)
dif = sketch.difference(aux)
dif.second_moment()
dif.first_moment()

sketch = sketches.FastCount64(16,16)
net_sketch = NetworkSketch(sketch)
results1 = test_corrupt(net_sketch, max_tests=1)

sketch = sketches.AGMS64(16,16)
net_sketch = NetworkSketch(sketch)
results2 = test_corrupt(net_sketch, max_tests=1)

sketch = sketches.CountMin64(16,16)
net_sketch = NetworkSketch(sketch)
results3 = test_corrupt(net_sketch, max_tests=1)
"""
############################ Different hash size ###############################
# Test FastCount8:
#for i in range(10):
#    sketch = sketches.FastCount8(16,16)
#    net_sketch = NetworkSketch(sketch)
#    results = test_corrupt(net_sketch, 16, 16)

# Test FastCount16:
#for i in range(10):
#    sketch = sketches.FastCount16(16,16)
#    net_sketch = NetworkSketch(sketch)
##    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))
#    results = test_corrupt(net_sketch, 16,16)

#print 1

## Test FastCount32:
#for i in range(10):
#    sketch = sketches.FastCount32(16,16)
#    net_sketch = NetworkSketch(sketch)
#    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))

#print 2

## Test FastCount64:
#for i in range(10):
#    sketch = sketches.FastCount64(16,16)
#    net_sketch = NetworkSketch(sketch)
#    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))

#print 3

## Test FastCount128:
#for i in range(10):
#    sketch = sketches.FastCount128(16,16)
#    net_sketch = NetworkSketch(sketch)
#    results = np.hstack((results, test_corrupt(net_sketch, 16, 16)))

#np.savetxt("corrupt_hash_16-16.cvs", results, delimiter=',', fmt="%s")

#print "End first tests"

"""
from network_sketches import *
import sketches
from experiment_sketches import *
b = sketches.FastCount32(16,16)
a = NetworkSketch(b)
results = test_sketch(a, 16,16, max_tests=1, interval=0.0001)
"""

############################ Different sketch size #############################

#TODO buckets y rows al reves
num_buckets = [4, 8 , 16, 32, 64]
num_rows = [4, 8, 16, 32, 64]
results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FastCount16(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount16.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        for i in range(10):
            sketch = sketches.FastCount32(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount32.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FastCount64(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount64.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FastCount128(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FastCount128.cvs", results, delimiter=',', fmt="%s")

#FAGMS
results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FAGMS16(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FAGMS16.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        for i in range(10):
            sketch = sketches.FAGMS32(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FAGMS32.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FAGMS64(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FAGMS64.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.FAGMS128(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_FAGMS128.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.AGMS16(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_AGMS16.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        for i in range(10):
            sketch = sketches.AGMS32(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_AGMS32.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.AGMS64(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_AGMS64.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.AGMS128(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_AGMS128.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.CountMin16(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_CountMin16.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        for i in range(10):
            sketch = sketches.CountMin32(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_CountMin32.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.CountMin64(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_CountMin64.cvs", results, delimiter=',', fmt="%s")

results = None
for buckets in num_buckets:
    for rows in num_rows:
        #Several iterations
        for i in range(10):
            sketch = sketches.CountMin128(buckets,rows)
            net_sketch = NetworkSketch(sketch)
            if results is None:
                results = test_drop(net_sketch, buckets, rows)
            else:
                results = np.hstack((results, test_drop(net_sketch, buckets, rows)))
np.savetxt("corrupt_sketch_CountMin128.cvs", results, delimiter=',', fmt="%s")



