import pprint
import sys
from cracker import Cracker
from crypto import get_sha256
from benchmark import Benchmarker

if __name__ == '__main__':    
    if len(sys.argv) != 3:
        print('USAGE: python main.py <wordlist> <hash_file>')
        exit()
    wordlist_path = sys.argv[1]
    hash_path = sys.argv[2]

    cracker = Cracker(wordlist_path, get_sha256,hash_path)
    benchmarker = Benchmarker()
    _, rainbow_table_duration = benchmarker.benchmark(cracker.set_rainbow_table, [])

    _, single_core_crack_duration = benchmarker.benchmark(cracker.crack, [])
    _, multi_core_crack_duration = benchmarker.benchmark(cracker.crack_multi_core, [])
    #cracked_hash_number = sum(result is not None for result in cracker.result_map.values())

    print('Results:')
    '''
    pprint.pprint(cracker.result_map)


    print()
    print(100 * '-')
    print()

    #print(f'Cracked {cracked_hash_number} in {duration} seconds')
    '''
    
    print(f'Rainbow table computation: {rainbow_table_duration} seconds')
    print(f'Single-core crack: {single_core_crack_duration} seconds')
    print(f'Multi-core crack: {multi_core_crack_duration} seconds')