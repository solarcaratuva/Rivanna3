import unittest
import sys
import os
import time

# just checks if test went through and returned true 
class TestExample(unittest.TestCase):
    def test_simple_math(self):
        self.assertEqual(2 + 2, 4)

    def test_logic(self):
        value = True
        self.assertTrue(value)
