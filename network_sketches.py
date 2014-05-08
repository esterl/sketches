import hashlib

# TODO: Find a name for the block
class NetworkSketch():
    #2-way hash
    #sketch
    #buffer
    
    def __init__(self,sketch, key=0L):
        self.buffer = []
        self.sketch = sketch
        self.key=key
        self.mask = (1L << sketch.get_key_size() ) -1
    
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
    
    def buffer_packet(self, pkt):
        self.buffer.append(long(hashlib.md5(str(pkt['IP'])).hexdigest(),base=16))
    
    def update(self, pkt):
        packet_hash = long(hashlib.md5(str(pkt['IP'])).hexdigest(),base=16)
        self.sketch.update(str((packet_hash^self.key)&self.mask),1.)
    
    def estimate_first_moment(self, other):
        return self.sketch.difference(other.sketch).first_moment()
    
    def estimate_second_moment(self, other):
        return self.sketch.difference(other.sketch).second_moment()
    
    def copy(self):
        result = NetworkSketch(self.sketch.copy(), self.key)
        return result
