import hashlib
import csv
from concurrent.futures import ThreadPoolExecutor
import time


def SHA256_software(bstr):
    return hashlib.sha256(bstr).hexdigest()


if __name__ == '__main__':
    tests = []

    with open('sha256_test.csv') as csvfile:
        csvreader = csv.reader(csvfile, delimiter=',')

        for row in csvreader:
            tests.append(row[0])

    arr = [bytes(test, encoding='utf-8') for test in tests]

    print('size,t1,t2,t3,t4,t5,t6,t7,t8,for')

    for i in range(15):
        print(len(arr), end=',')

        for t in range(16):
            c = int(len(arr) / (t+1)) + 1

            with ThreadPoolExecutor(max_workers=t + 1) as executor:
                start_time = time.time()
                executor.map(SHA256_software, arr, chunksize=c)
                print((time.time() - start_time), end=',')

        start_time = time.time()
        for a in arr:
            SHA256_software(a)
        print((time.time() - start_time))
        arr = arr + arr


