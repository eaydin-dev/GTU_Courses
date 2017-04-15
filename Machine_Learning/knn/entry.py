import math


class Entry():
    def __init__(self, v, c):
        self.v = v      # vectoral values of the entry
        self.c = int(c) # class value of the entry

    def euclidean_distance(self, vec):
        c = [(float(a) - float(b)) ** 2 for a, b in zip(self.v, vec)]
        c = math.sqrt(sum(c))
        return c

    def manhattan_distance(self, vec):
        c = [abs(float(a) - float(b)) for a, b in zip(self.v, vec)]
        val = sum(c)
        return val
