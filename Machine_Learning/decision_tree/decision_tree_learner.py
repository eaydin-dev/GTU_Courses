
class Node:
    def __init__(self, col=-1, value=None, results=None, tb=None, fb=None):
        self.col = col              # column index of the criteria to be tested
        self.value = value          # the value that column must match to be result true
        self.results = results      # keeps results for this branch as dictionary
        self.tb = tb                # true branch
        self.fb = fb                # false branch


def split_set(rows, column, value):
    if isinstance(value, int) or isinstance(value, float):
        true_set = [row for row in rows if row[column] >= value]
        false_set = [row for row in rows if not row[column] >= value]
    else:
        true_set = [row for row in rows if row[column] == value]
        false_set = [row for row in rows if not row[column] == value]

    return (true_set, false_set)


def unique_counts(rows):
    results = {}
    for row in rows:
        r = row[len(row) - 1]
        if r not in results:
            results[r] = 0
        results[r] += 1

    return results


def variance(rows):
    if len(rows) == 0:
        return 0
    data = [float(row[len(row) - 1]) for row in rows]
    mean = sum(data) / len(data)
    return sum([(d - mean) ** 2 for d in data]) / len(data)


def print_tree(tree, indent=''):
    if tree.results is not None: # leaf node?
        print str(tree.results)
    else:
        print str(tree.col) + ':' + str(tree.value) + '? ' # criteria
        print indent + 'T->',
        print_tree(tree.tb, indent + ' ') # true branch
        print indent + 'F->',
        print_tree(tree.fb, indent + ' ') # false branch


def prune(tree, min_gain):
    # if the branches are not leaf, prune them
    if tree.tb.results is None:
        prune(tree.tb, min_gain)
    if tree.fb.results is None:
        prune(tree.fb, min_gain)

    # if both the subbranches are leaf, check if they should merged
    if tree.tb.results is not None and tree.fb.results is not None:
        tb, fb = [], []
        for v, c in tree.tb.results.items():
            tb += [[v]] * c
        for v, c in tree.fb.results.items():
            fb += [[v]] * c
        delta = variance(tb + fb) - (variance(tb) + variance(fb) / 2)  # test the reduction in variance
        if delta < min_gain:  # merge
            tree.tb, tree.fb = None, None
            tree.results = unique_counts(tb + fb)


def load_data(path):
    file_ptr = open(path, "r")
    text = file_ptr.read()
    rows = text.splitlines()
    rows = rows[1:]  # skip name row

    result = []
    for row in rows:
        values = row.split(',')
        l = [int(values[0]), int(values[1]), values[2], values[3], float(values[4]), float(values[5]), float(values[6]),
             float(values[7]), float(values[8]), int(values[9]), float(values[10]), float(values[11]),
             float(values[12])]

        result.append(l)

    file_ptr.close()
    return result


def build_tree(rows):
    if len(rows) == 0: return Node()
    current_score = variance(rows)

    best_gain = 0.0
    best_criteria = None
    best_sets = None
    column_count = len(rows[0]) - 1
    for col in range(0, column_count):
        # get the different values in this col
        column_values = {}
        for row in rows:
            column_values[row[col]] = 1

        # divide this column
        for value in column_values.keys():
            (set1, set2) = split_set(rows, col, value)
            p = float(len(set1)) / len(rows)  # information gain
            gain = current_score - p * variance(set1) - (1 - p) * variance(set2)
            if gain > best_gain and len(set1) > 0 and len(set2) > 0:  # a better one
                best_gain = gain
                best_criteria = (col, value)
                best_sets = (set1, set2)

    # create the subbranches
    if best_gain > 0:
        true_branch = build_tree(best_sets[0])
        false_branch = build_tree(best_sets[1])
        return Node(col=best_criteria[0], value=best_criteria[1], tb=true_branch, fb=false_branch)
    else:
        return Node(results=unique_counts(rows))


def classify(observation, tree):
    if tree.results is not None:
        return tree.results
    else:
        v = observation[tree.col]
        branch = None
        if isinstance(v, int) or isinstance(v, float):
            if v >= tree.value:
                branch = tree.tb
            else:
                branch = tree.fb
        else:
            if v == tree.value:
                branch = tree.tb
            else:
                branch = tree.fb
        return classify(observation, branch)


def concat_lists(lists, e):
    res_list = []
    for i in range(0, len(lists)):
        if i != e:
            res_list += lists[i]

    return res_list


def k_fold_cross_validation(k, rows):
    from random import shuffle
    shuffle(rows)
    length = len(rows) / k
    chunks = [rows[x:x + length] for x in xrange(0, len(rows), length)]

    res_lst = []

    for i in range(0, k):
        local_total = 0
        test_set = []
        test_set += chunks[i]
        train_set = concat_lists(chunks, i)
        tree = build_tree(train_set)
        prune(tree, 1.0)

        for entry in test_set:
            question = entry[:len(entry) - 1]
            ans = entry[len(entry) - 1]
            res = classify(question, tree)
            res = dict_mean(res)  # res.items()[0][0]
            # print res
            local_total += (res - ans) ** 2

        r = local_total / (len(test_set) - 2)
        print 'k', str(i + 1) + ':', r
        res_lst.append(r)

    print res_lst
    print 'avg:', str(sum(res_lst) / len(res_lst))


def dict_mean(d):
    divide_num = sum(d.values())
    num_to_divide = sum(d.keys())
    return num_to_divide / divide_num
