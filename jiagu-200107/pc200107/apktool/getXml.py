__author__ = 'lwl'

from os import system

def Axml2Xml(xmlpath, toolpath):
	f=open(toolpath+'/AndroidManifest.xml','w')
	result = system('java -jar AXMLPrinter2.jar '+xmlpath+'/AndroidManifest.xml > '+toolpath+'/AndroidManifest.xml')
	f.close()
	return result
