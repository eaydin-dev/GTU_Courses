from entry import Entry
from random import shuffle
import timeit
import os

# ***** ***** ***** ***** ***** *****
def knn_load(data_file):
    file = open(data_file, "r")
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
            # e.set(variables, c)
            entries.append(e)
            # lines.append(line)

    '''for line in lines:
        print line
    '''
    file.close();
    return entries

'''
    function to get second element of tuple while sorting
'''

def fun(a):
    return a[1]


# ***** ***** ***** ***** ***** *****
#   True: manhattan
#   False: euclidean
def find_most_common_class(given_entry, entries, k, option):
    list_ = []
    classes = []
    for entry in entries:
        if option:
            list_.append((entry.c, entry.manhattan_distance(given_entry.v)))
        else:
            list_.append((entry.c, entry.euclidean_distance(given_entry.v)))

        classes.append(entry.c)
        classes.append(entry.c)

    classes = sorted(set(classes))

    sorted_list = sorted(list_, key=fun)
    sub_list = []
    for e in sorted_list:
        sub_list.append(e)
        if len(sub_list) == k:
            break

    count_list = []
    for c in classes:
        num = count_class(sub_list, c)
        count_list.append((c, num))

    sorted_count_list = sorted(count_list, key=fun, reverse=True)
    return sorted_count_list[0][0]


def count_class(tuples, c):
    counter = 0

    for a, b in tuples:
        if a == c:
            counter += 1

    return counter

def concat_lists(lists, e):
    res_list = []
    for i in range(0, len(lists)):
        if i != e:
            res_list += lists[i]

    return res_list


#   True: manhattan
#   False: euclidean
def k_fold(k, entries, option):
    length = len(entries) / k
    chunks = [entries[x:x + length] for x in xrange(0, len(entries), length)]
    total_true = 0
    total_false = 0
    total_tp = 0
    total_fp = 0

    for i in range(0, k):
        #print "looking at the fold: " + str(i+1)
        match_counter = 0
        false_counter = 0
        test_set = []
        test_set += chunks[i]
        train_set = concat_lists(chunks, i)

        #print "test_set: " + str(len(test_set)) + ", train_set: " + str(len(train_set))
        for entry in test_set:
            res = find_most_common_class(entry, train_set, 5, option)
            if res == entry.c:
                match_counter += 1
                #print 'matched: ' + ' '.join(str(s) for s in entry.v) + " - " + str(entry.c)
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


# ***** ***** ***** ***** ***** *****

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))

    entries = knn_load(dir_path + '\data\iris.dat')
    print "\n"
    # for entry in entries:
    #	print ' '.join(str(s) for s in entry.v) + " - " + str(entry.c)

    #res = find_most_common_class(entries[85], entries, 20)
    #print res
    shuffle(entries)
    start = timeit.default_timer()
    k = 10
    print 'data size: ' + str(len(entries))
    print 'calling k-fold cross validation with distance metric = euclidean and k=' + str(k) + '\n'
    k_fold(k, entries, False)
    stop = timeit.default_timer()
    print ''
    print 'run time: ' + str(stop - start) + ' sec.\n'
    
    start = timeit.default_timer()
    print ''
    print 'calling k-fold cross validation with distance metric = manhattan and k=' + str(k) + '\n'
    k_fold(k, entries, True)
    stop = timeit.default_timer()
    print ''
    print 'run time: ' + str(stop - start) + ' sec.\n'


# ***********************
# main call
main()
