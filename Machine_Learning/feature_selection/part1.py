"""

Part 1: Open loop top-down feature selection algorithm implementation and test.
o You start with the entire set of features and remove combinations of features starting from one at a time two at a time and more.
o At each iteration run the classifier of your choice and record the performance.
o Pick up a dataset for classification (should be a classification task with features/attributes with real and categorical values).
o Report which set of features results in the best performance. The best performance is considered in terms of accuracy. 
10-fold cross validation should be used.

data used: https://archive.ics.uci.edu/ml/datasets/Fertility


"""

import itertools
from datetime import datetime

import numpy as np
import sklearn.svm
from sklearn.model_selection import cross_val_score

import prepare_data

start = datetime.now()
data, target = prepare_data.get('fertility.data')

classifier = sklearn.svm.SVC()

print 'Original classification result:'
res = cross_val_score(classifier, data, target, cv=10)
print res
print 'Mean:', np.mean(res), '\n'

combinations = []
transposed = data.T

for i in range(1, len(data[0])):
    for sub in itertools.combinations(range(0, len(data[0])), len(data[0])-i):
        combinations.append(np.array(sub))

results = []
for i in range(1, len(data[0])):
    print 'progress:', i, 'in', len(data[0])-1
    subs = []
    for sub in itertools.combinations(transposed, len(transposed)-i):
        subs.append(np.array(sub))
    for d in subs:
        res = cross_val_score(classifier, d.T, target, cv=10)
        results.append(np.mean(res))

print 'time:', datetime.now()-start, '\n'
res_str = []
for i in range(0, len(results)):
    res_str.append(str(results[i]) + ' for ' + str(combinations[i]))

res_str.sort()
print 'Result:', res_str[len(res_str)-1]

