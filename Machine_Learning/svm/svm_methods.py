
import os
from entry import Entry
from scikits.learn import svm
from random import shuffle
import timeit


iris_data_path = '\data\iris.dat'
leaf_data_path = '\data\leaf.dat'

lin_or_poly = False    # True: linear svm, False: polynomial svm
iris_or_leaf = False   # True: iris data, False: leaf data
k = 10                # k-cross validation's k


def main():
    start = timeit.default_timer()
    dir_path = os.path.dirname(os.path.realpath(__file__))
    
    if lin_or_poly:
        print 'linear svm'
    else:
        print 'poly svm'
        
    if iris_or_leaf:
        print 'iris data'
        entries = load_iris_data(dir_path)
    else:
        print 'leaf data'
        entries = load_leaf_data(dir_path)
    
    shuffle(entries)
    k_fold(k, entries, lin_or_poly)

    stop = timeit.default_timer()
    print ''
    print 'run time: ' + str(stop - start) + ' sec.'
    
def concat_lists(lists, e):
    res_list = []
    for i in range(0, len(lists)):
        if i != e:
            res_list += lists[i]

    return res_list
    


# method to initialize and train the svm classifier
def svm_method(entries, lin_or_poly):
    feature_vectors = []
    classes = []
    for e in entries:
        feature_vectors.append(e.v)
        classes.append(e.c)
    
    if lin_or_poly:
        curr_svm = svm.LinearSVC()
    else:
        curr_svm = svm.SVC(kernel='poly')
    
    curr_svm.fit(feature_vectors, classes)
    return curr_svm

# evaluate the svm with k_fold cross validation method
def k_fold(k, entries, lin_or_poly):
    length = len(entries) / k
    chunks = [entries[x:x + length] for x in xrange(0, len(entries), length)]
    
    total_true = 0
    total_false = 0
    total_tp = 0
    total_fp = 0

    for i in range(0, k):
        match_counter = 0
        false_counter = 0
        test_set = []
        test_set += chunks[i]
        train_set = concat_lists(chunks, i)
        svm_model = svm_method(train_set, lin_or_poly)

        for entry in test_set:
            question = []
            question.append(entry.v)
            #print ('l: ' + str(question))
            res = svm_model.predict(question)
            if res == entry.c:
                match_counter += 1
            else:
                false_counter += 1

        total_true += match_counter
        total_false += false_counter
        
        tp = float(match_counter)/len(test_set)
        fp = 1 - tp
        
        total_tp += tp
        total_fp += fp
        print 'k=' + str(i+1) + ' -> ' + 'match: ' + str(match_counter) + ', false: ' + str(false_counter) + ', tp: ' + str(tp) + ', fp: ' + str(fp)
    
    avg_tp = float(total_tp)/k
    avg_fp = float(total_fp)/k
    
    print ''
    print 'total_true: ' + str(total_true) + ', total_false: ' + str(total_false)
    print 'average tp: ' + str(avg_tp) + ', average fp: ' + str(avg_fp)


# method to load iris.dat file as list of entry objects
def load_iris_data(workspace):
    file = open(workspace + iris_data_path, "r")
    text = file.read()
    temp_lines = text.splitlines();
    entries = []

    for line in temp_lines:
        line = line.strip(' ')
        tokens = line.split()
        if len(tokens) > 1:
            c = tokens[-1]
            variables = tokens[0:len(tokens) - 1]
            e = Entry(variables, c)
            entries.append(e)

    file.close();
    print("files loaded.\n")
    return entries
    

def load_leaf_data(workspace):
    file = open(workspace + leaf_data_path, "r")
    text = file.read()
    temp_lines = text.splitlines();
    entries = []

    for line in temp_lines:
        line = line.strip(' ')
        tokens = line.split()
        if len(tokens) > 1:
            c = tokens[0]
            variables = tokens[1:len(tokens)]
            e = Entry(variables, c)
            entries.append(e)

    file.close();
    return entries


main()
