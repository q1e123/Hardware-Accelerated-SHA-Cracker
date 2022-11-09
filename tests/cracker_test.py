import unittest
import sys

sys.path.append('../src')

from cracker import Cracker
from crypto import get_sha256

class CryptoTest(unittest.TestCase):
    def test_sha256_cracker(self):
        cracker = Cracker('word_list.txt', get_sha256)
        results = cracker.crack('hash_list.txt')
        self.assertEqual(results['41e5c285822f1c0702dcea6969221914c8a6887bec55f36ebe84a63b33827027'],  'QWE')
        self.assertEqual(results['9520437ce8902eb379a7d8aaa98fc4c94eeb07b6684854868fa6f72bf34b0fd3'],  'FOO')
        self.assertEqual(results['32168097920da2bffa363a29dae88e095bc1a00cb0d1923b37cd18d56ef2e3d5'],  '45asd6qwehj213asduu34y2ij423ij132')
        self.assertIsNone(results['32168597920da2bffa363a29dae88e095bc1a00cb0d1923b37cd18d56ef2e3d5'])

if __name__ == '__main__':
    unittest.main()