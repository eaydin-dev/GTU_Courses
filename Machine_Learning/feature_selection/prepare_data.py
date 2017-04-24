
# prepare_data.py

import numpy as np
from sklearn import preprocessing


def load_data(path):
    file_ptr = open(path, "r")
    text = file_ptr.read()
    rows = text.splitlines()

    result = []
    for row in rows:
        values = row.split(',')
        l = []
        for i in range(0, len(values)-1):
            l.append(float(values[i]))

        l.append(values[len(values)-1])
        result.append(np.asarray(l))

    file_ptr.close()
    return result


def get(path):
    raw_data = load_data(path)
    print "data size:", len(raw_data)
    print "# features:", len(raw_data[0]) - 1

    data = []
    target = []
    le = preprocessing.LabelEncoder()

    for row in raw_data:
        data.append(le.fit_transform(row[:len(row) - 1]))
        target.append(row[len(row) - 1])

    data = np.asarray(data)
    target = np.asarray(target)

    return np.asarray(data), np.asarray(target)
