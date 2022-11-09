from utils import get_lines_from_file

class Cracker():
    def __init__(self, wordlist_path, hash_function) -> None:
        self.wordlist_path = wordlist_path
        self.hash_function = hash_function
        self.word_list = get_lines_from_file(self.wordlist_path)
        self.rainbow_table = self.get_rainbow_table()

    def get_rainbow_table(self):
        rainbow_table = {}
        for word in self.word_list:
            hash = self.hash_function(word)
            rainbow_table[hash] = word
        return rainbow_table

    def crack(self, hashes_path):
        hash_list = get_lines_from_file(hashes_path)
        result_map = {}
        for hash in hash_list:
            if hash in self.rainbow_table:
                result_map[hash] = self.rainbow_table[hash]
            else:
                result_map[hash] = None
        return result_map
