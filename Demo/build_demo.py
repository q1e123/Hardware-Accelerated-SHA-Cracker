import sys
sys.path.append('../src')

import utils
from crypto import get_sha256

password_list = utils.get_lines_from_file('1000-most-common-passwords.txt')
with open('demo-hashes', 'w+') as file:
    for password in password_list:
        sha = get_sha256(password)
        for _ in range(100):
            file.write(sha + '\n')
