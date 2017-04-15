
from sklearn.tree import DecisionTreeRegressor
from sklearn.model_selection import cross_val_score
from sklearn import preprocessing
import numpy as np

import decision_tree_learner

# data = decision_tree_learner.load_data('forestfires.csv')
# decision_tree_learner.k_fold_cross_validation(10, data)


raw_data = decision_tree_learner.load_data('forestfires.csv')

data = []
target = []
le = preprocessing.LabelEncoder()

for row in raw_data:
    data.append(le.fit_transform(row[:len(row)-1]))
    target.append(row[len(row)-1])

regressor = DecisionTreeRegressor(random_state=0)
list = cross_val_score(regressor, data, np.asarray(target), cv=10, scoring='neg_mean_squared_error')
print list
print 'average:', sum(list) / len(list)


# tree = decision_tree_learner.build_tree(data, scoref=decision_tree_learner.variance)
# decision_tree_learner.draw_tree(tree, jpeg='treeview.jpg')
# #res = decision_tree_learner.classify_instance([8,6,'aug','tue',96.1,181.1,671.2,14.3,21.6,65,4.9,0.8], tree)
# #decision_tree_learner.prune(tree, 1.0)
# decision_tree_learner.print_tree(tree)
#
# res = decision_tree_learner.classify([8,6,'sep','fri',91.1,91.3,738.1,7.2,20.7,46,2.7,0], tree)
# print
# print type(res)
# print res.items()[0][0]
# print decision_tree_learner.count_to_prob(res)
#

# tree = decision_tree_learner.build_tree(my_data)
# decision_tree_learner.print_tree(tree)
# #decision_tree_learner.draw_tree(tree, jpeg='treeview.jpg')
# res = decision_tree_learner.classify(['google','UK','no',21],tree)
# print res
# print decision_tree_learner.count_to_prob(res)


'''
learner = DecisionTreeLearner()
(s1, s2) = decision_tree_learner.split_set(my_data, 2, 'yes')

for r in s1:
    print r[len(r)-1]

print

for r in s2:
    print r[len(r)-1]

'''

my_data=[['slashdot','USA','yes',18,'None'],
['google','France','yes',23,'Premium'],
['digg','USA','yes',24,'Basic'],
['kiwitobes','France','yes',23,'Basic'],
['google','UK','no',21,'Premium'],
['(direct)','New Zealand','no',12,'None'],
['(direct)','UK','no',21,'Basic'],
['google','USA','no',24,'Premium'],
['slashdot','France','yes',19,'None'],
['digg','USA','no',18,'None'],
['google','UK','no',18,'None'],
['kiwitobes','UK','no',19,'None'],
['digg','New Zealand','yes',12,'Basic'],
['slashdot','UK','no',21,'None'],
['google','UK','yes',18,'Basic'],
['kiwitobes','France','yes',19,'Basic']]
