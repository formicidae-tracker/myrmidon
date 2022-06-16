import unittest
import py_fort_myrmidon_utestdata as md

TestData = None


def setUpModule():
    TestData = md.UTestData("/foo")


def tearDownModule():
    TestData = None
