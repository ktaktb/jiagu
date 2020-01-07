#encoding:utf-8
_author_="sunn"
_modifier_="kt"
import getXml
import AdapterApktool
import GenPassword
import unpack
import reSign
import EncMode
import keyTrans
import keyTrans_db
import os
import smalishell
import shutil
import modXml
import encryptso
import ge_wb_data
# import wx
# from wx.lib.pubsub import Publisher
import sys
reload(sys)
sys.setdefaultencoding('utf8')

def readConfig(index):
    f = open('config.bin', 'r')
    config = f.read().splitlines()

    print 'pc flow        readConfig'
    print 'index: %d'%index
    print config
    return config[index]
	
def pcworkflow(apkpath, keystorepath, keystorePassword, keypass, keystoreAlias):
  kind = 1

    #####################################################################
    # 1、解压apk
    dexpack = "dexpack"
    print u"解压apk..."
    if not unpack.unzip(apkpath, dexpack) == 0:
        wx.CallAfter(Publisher().sendMessage, "update", "解压APK：失败")
        os.system("rm -rf " + dexpack)
        return 1
    else:
        print u"解压APK：成功..."
        wx.CallAfter(Publisher().sendMessage, "update", "解压APK：成功")
    #wx.CallAfter(Publisher().sendMessage, "update", msg)
    ####################################################################


    ###################################################################
    # 2. 解析AndroidManifest.xml
    toolpack="toolpack"
    os.system("mkdir "+toolpack)
    if not getXml.Axml2Xml(dexpack, toolpack) == 0:
        wx.CallAfter(Publisher().sendMessage, "update", "解析AndroidManifest.xml：失败")
        os.system("rm -rf " + toolpack)
        return 1
    else:
        wx.CallAfter(Publisher().sendMessage, "update", "解析AndroidManifest.xml：成功")
    ####################################################################