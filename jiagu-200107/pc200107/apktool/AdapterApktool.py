__author__ = 'zhr'

from os import system


def decompile(apkpath, packagepath="toolpack"):
    system('apktool d ' + apkpath + " -o " + packagepath)


def rebuild(packagepath, apkname):
    cmd_rebuild = r'apktool b %s -o %s' % (packagepath, apkname)
    return system(cmd_rebuild)



