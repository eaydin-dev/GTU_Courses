
"""

Part 3: Principal component analysis for classification.
o Pick a dataset for classification.
o Apply PCA on the real-valued attributes of the data.
o Choose a set of principle components to train a classification algorithm of your choice.
o Draw the graph showing the accuracy vs the number of components used.
o Use 10-fold cross validation in your tests.

"""

import numpy as np
import prepare_data
import sklearn.svm
from sklearn import decomposition
import matplotlib.pyplot as plt

data, target = prepare_data.get('fertility.data')
classifier = sklearn.svm.SVC()
acc_results = []

for i in range(1, len(data[0]) + 1):
    d = data
    pca = decomposition.PCA(n_components=i)
    pca.fit(d)
    data_new = pca.transform(d)
    res = sklearn.model_selection.cross_val_score(classifier, data_new, target, cv=10)
    acc_results.append(np.mean(res))
    print '# attributes:', i, 'Mean:', np.mean(res)

plt.figure(1, figsize=(4, 3))
plt.clf()
plt.axes([.1, .1, .7, .7])
plt.plot(acc_results, linewidth=1)
plt.xlabel('n_components')
plt.ylabel('accuracy')
plt.show()
