__author__ = 'sunn'

from os import system

def encso(key, sopath):
	system('./shellAdder2 '+key+' '+sopath)

def encappso(key, sopath):
	system('./shellAdder3 '+key+' '+sopath)
