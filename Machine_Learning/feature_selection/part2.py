"""

Part 2: Feature importance using random forests (RFs).
o Consider the above dataset.
o Use RF to calculate the relative importance of the features.
o Compare your results with the ones in Part 1. Your comparison should consider the RF outputs and your top-down selection algorithm outputs.

"""

import numpy as np
from sklearn.ensemble import ExtraTreesClassifier

import prepare_data

data, target = prepare_data.get('fertility.data')
forest = ExtraTreesClassifier(n_estimators=250, random_state=0)

forest.fit(data, target)
importances = forest.feature_importances_
std = np.std([tree.feature_importances_ for tree in forest.estimators_], axis=0)
indices = np.argsort(importances)[::-1]
print("Feature ranking:")
for f in range(data.shape[1]):
    print("%d. feature %d (%f)" % (f + 1, indices[f], importances[indices[f]]))
