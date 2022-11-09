import unittest
import sys

sys.path.append('../src')

from crypto import get_sha256

class CryptoTest(unittest.TestCase):
    def test_sha256(self):
        self.assertEqual(get_sha256('QWE'),  '41e5c285822f1c0702dcea6969221914c8a6887bec55f36ebe84a63b33827027')
        self.assertEqual(get_sha256('FOO'),  '9520437ce8902eb379a7d8aaa98fc4c94eeb07b6684854868fa6f72bf34b0fd3')
        self.assertEqual(get_sha256('45asd6qwehj213asduu34y2ij423ij132'),  '32168097920da2bffa363a29dae88e095bc1a00cb0d1923b37cd18d56ef2e3d5')


if __name__ == '__main__':
    unittest.main()