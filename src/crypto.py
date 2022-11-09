import hashlib

def get_sha256(word):
    return hashlib.sha256(bytes(word,'utf-8')).hexdigest()
