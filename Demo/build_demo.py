import sys
sys.path.append('../src')

import utils
from crypto import get_sha256

import random
import string

def get_random_string(length):
    dictionary = string.ascii_lowercase
    result = ''.join(random.choice(dictionary) for i in range(length))
    return result

password_list = utils.get_lines_from_file('1000-most-common-passwords.txt')
with open('demo-hashes', 'w+') as file:
    for password in password_list:
        sha = get_sha256(password)
        file.write(sha + '\n')

with open('demo-hashes', 'a+') as file:
    for _ in range(int(1e5)):
        random_string = get_random_string(8)
        sha = get_sha256(random_string)
        file.write(sha + '\n')
