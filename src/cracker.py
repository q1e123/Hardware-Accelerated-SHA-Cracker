from utils import get_lines_from_file

class Cracker():
    def __init__(self, wordlist_path, hashes_path, hash_function) -> None:
        self.wordlist_path = wordlist_path
        self.hashes_path = hashes_path
        self.hash_function = hash_function
        self.init_files()

    def init_files(self):
        self.word_list = get_lines_from_file(self.wordlist_path)
        self.hashes_path = get_lines_from_file(self.hashes_path)