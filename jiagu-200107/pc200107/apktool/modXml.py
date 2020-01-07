__author__ = 'lwl'

from os import system

def modify(xmlpath):
	f=open('AndroidManifest.xml','w')
	result = system('./manifestAmbiguity -m '+xmlpath+'/AndroidManifest.xml -o'+xmlpath+'/AndroidManifest.xml')
	f.close()
	return result
