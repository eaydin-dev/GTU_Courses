class Node:
    col = -1                # column index of the criteria to be tested
    value = None            # the value that column must match to be result true
    results = None          # keeps results for this branch as dictionary
    true_branch = None      # child node
    false_branch = None

    def __init__(self, col=-1, value=None, results=None, tb=None, fb=None):
        self.col = col
        self.value = value
        self.results = results
        self.tb = tb
        self.fb = fb