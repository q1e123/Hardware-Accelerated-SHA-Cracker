import pprint
import sys
from cracker import Cracker
from crypto import get_sha256
from benchmark import Benchmarker

if len(sys.argv) != 3:
    print('USAGE: python main.py <wordlist> <hash_file>')
    exit()
wordlist_path = sys.argv[1]
hash_path = sys.argv[2]

cracker = Cracker(wordlist_path, get_sha256)

benchmarker = Benchmarker()
results, duration = benchmarker.benchmark(cracker.crack, [hash_path])
cracked_hash_number = sum(result is not None for result in results.values())

print('Results:')
pprint.pprint(results)


print()
print(100 * '-')
print()

print(f'Cracked {cracked_hash_number} in {duration} seconds')
