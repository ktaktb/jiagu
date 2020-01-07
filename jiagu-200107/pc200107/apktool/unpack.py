__author__ = 'zhr'

from os import system


def unzip(apkpath, despath):
    print u"unzip000000..."
    return system( 'unzip '+apkpath + ' -d '+despath )
def zipdex(dexpath, zdexpath, password):
    return system("zip -P " + password + " zipdex.apk " + dexpath + ";mv -f zipdex.apk " + zdexpath)
def zip(srcpath, despath):
    cmd_in="cd "+srcpath+';zip  -r cal.apk  * '+";mv -f cal.apk "+"../"+despath
    return system(cmd_in)
def zipblankdex():
    return system("zip " + "classes1.dex " + "classes.dex")

def zipapk(srcpath, despath):
    cmd_in="cd "+srcpath+";zip -r cal.apk * ;mv -f cal.apk ../" + despath
    return system(cmd_in)


