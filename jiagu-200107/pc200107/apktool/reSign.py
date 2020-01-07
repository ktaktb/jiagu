__author__ = 'zhr'

import os
import os.path


def reSigner(oldapk, newapk, keystore, storepass, keypass, keystorealias):
  #  cmd_sign = r'jarsigner -digestalg SHA1 -sigalg MD5withRSA  -keystore %s -storepass %s -signedjar %s %s %s' % (keystore, keypass, newapk, oldapk, keystore)
    cmd_sign = r'jarsigner -digestalg SHA1 -sigalg MD5withRSA -verbose -keystore %s -storepass %s -keypass %s -signedjar %s %s %s' % (keystore, storepass, keypass, newapk, oldapk, keystorealias)
    print u"cmd_sign..."+cmd_sign
    return os.system(cmd_sign)

# print "please input the unsigned apk name, keystore name, keypass num"
# unsignapkname = raw_input("input the unsigned apk name:")
# keystorename = raw_input("input the keystore name:")
# keypassnum = raw_input("input the keypass num:")
#
# signedapkname = "sign" + unsignapkname
# reSigner(unsignapkname, signedapkname, keystorename, keypassnum)