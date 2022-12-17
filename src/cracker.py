import multiprocessing

from utils import get_lines_from_file

class Cracker():

    def __init__(self, wordlist_path, hash_function, hashes_path) -> None:
        self.wordlist_path = wordlist_path
        self.hash_function = hash_function
        self.word_list = get_lines_from_file(self.wordlist_path)
        self.rainbow_table = None
        self.hash_list = get_lines_from_file(hashes_path)

    def set_rainbow_table(self):
        self.rainbow_table = {}
        for word in self.word_list:
            hash = self.hash_function(word)
            self.rainbow_table[hash] = word
        

    def crack(self,hash_list_start=None, hash_list_end=None):
        result_map = {}
        if hash_list_start is None:
            hash_list_start = 0
        if hash_list_end is None:
            hash_list_end = len(self.hash_list)
        for hash in self.hash_list[hash_list_start:hash_list_end]:
            if hash in self.rainbow_table:
                result_map[hash] = self.rainbow_table[hash]
            else:
                result_map[hash] = None
        return result_map

    def crack_multi_core(self):
        cpu_cores = multiprocessing.cpu_count()
        processes = []
        hash_list_size = len(self.hash_list)
        for i in range(0,cpu_cores):
            task = multiprocessing.Process(target=self.crack, args=(i*hash_list_size,(i+1)*hash_list_size))
            processes.append(task)
            task.start()
        for one_process in processes:
            one_process.join()

