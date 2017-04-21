"""

Part 1: Open loop top-down feature selection algorithm implementation and test.
o You start with the entire set of features and remove combinations of features starting from one at a time two at a time and more.
o At each iteration run the classifier of your choice and record the performance.
o Pick up a dataset for classification (should be a classification task with features/attributes with real and categorical values).
o Report which set of features results in the best performance. The best performance is considered in terms of accuracy. 
10-fold cross validation should be used.

data used: https://archive.ics.uci.edu/ml/datasets/Contraceptive+Method+Choice


"""

from sklearn import preprocessing
from sklearn.model_selection import cross_val_score
from sklearn.tree import DecisionTreeClassifier
import sklearn.svm
from sklearn import linear_model

import numpy as np


def load_data(path):
    file_ptr = open(path, "r")
    text = file_ptr.read()
    rows = text.splitlines()

    result = []
    for row in rows:
        values = row.split(',')
        l = [int(values[0]), values[1], values[2], int(values[3]), values[4], values[5], values[6], values[7],
             values[8], values[9]]
        result.append(np.asarray(l))

    file_ptr.close()
    return result


raw_data = load_data('contraceptive_methods.data')
print "data size:", len(raw_data), "dims:", len(raw_data[0])

data = []
target = []
le = preprocessing.LabelEncoder()

for row in raw_data:
    data.append(le.fit_transform(row[:len(row) - 1]))
    target.append(row[len(row) - 1])

# classifier = sklearn.svm.SVC(C=1.0, cache_size=200, class_weight=None, coef0=0.0,
#     decision_function_shape=None, degree=3, gamma='auto', kernel='rbf',
#     max_iter=-1, probability=False, random_state=None, shrinking=True,
#     tol=0.001, verbose=False)

classifier = sklearn.multiclass.OneVsRestClassifier(sklearn.svm.SVC())
#classifier.fit(data, target)
res = cross_val_score(classifier, data, target, cv=10)
print (res)
