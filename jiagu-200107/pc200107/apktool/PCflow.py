#!/usr/bin/python
# -*- coding: utf-8 -*-
_author_="sunn"

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
import wx
#from wx.lib.pubsub import Publisher
from wx.lib.pubsub import setuparg1
from wx.lib.pubsub import pub as Publisher
import sys
reload(sys)
sys.setdefaultencoding('utf8')
#import pdb

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
        wx.CallAfter(Publisher.sendMessage, "update", "解压APK：失败")
        os.system("rm -rf " + dexpack)
        return 1
    else:
        print u"解压APK：成功..."
        wx.CallAfter(Publisher.sendMessage, "update", "解压APK：成功")
    #wx.CallAfter(Publisher().sendMessage, "update", msg)
    ####################################################################


    ###################################################################
    # 2. 解析AndroidManifest.xml
    toolpack="toolpack"
    os.system("mkdir "+toolpack)
    if not getXml.Axml2Xml(dexpack, toolpack) == 0:
        wx.CallAfter(Publisher.sendMessage, "update", "解析AndroidManifest.xml：失败")
        os.system("rm -rf " + toolpack)
        return 1
    else:
        wx.CallAfter(Publisher.sendMessage, "update", "解析AndroidManifest.xml：成功")
    ####################################################################

    ####################################################################
    # 3. 修改AndroidManifest.xml
    temp_smalishell = smalishell.shellcreate("fake.apk")
    temp_smalishell.shell()
    ####################################################################


    smalipath="toolpack/smali"
    smalidespath="encode"
    fakeapkname="fakeapk.apk"
    fakedexpack = "fakedexpack"
    os.system("cp -rf " + smalipath + " " + smalidespath)


    ####################################################################
    # 4. 加壳、重编译
    if not AdapterApktool.rebuild(smalidespath, fakeapkname) == 0:
        wx.CallAfter(Publisher.sendMessage, "update", "加壳、重编译：失败")
        os.system("rm -f " +  fakeapkname)
        return 1
    else:
        wx.CallAfter(Publisher.sendMessage, "update", "加壳、重编译：成功")

    os.system("rm -rf " + toolpack)
    ####################################################################

    ####################################################################
    # 5. 解压重编译的apk
    if not unpack.unzip(fakeapkname, fakedexpack) == 0:
        wx.CallAfter(Publisher.sendMessage, "update", "解压重编译的APK：失败")
        os.system("rm -f " +  fakeapkname)
        os.system("rm -rf " + fakedexpack)
        return 1
    else:
        wx.CallAfter(Publisher.sendMessage, "update", "解压重编译的APK：成功")

    os.system("rm -r " + smalidespath+"/smali")
    ####################################################################


    # 6. 密钥变换
    sopath = "libs/armeabi/libsec.so"
    sopathx86 = "libs/x86/libsec.so"
    sopathdb = "libs/armeabi/db/libsec.so"
    sopathx86db = "libs/x86/db/libsec.so"
    keyswitchname = "blackbox.m"

    keyInput = GenPassword.GenPassword(16)

    # 7. 压缩(加密压缩原始Dex)
    dexpath = "dexpack/classes.dex"
    fakedexpath = "fakedexpack/classes.dex"
    zipdexpath = "zipdex"
    if not unpack.zipdex(dexpath, zipdexpath, keyInput) == 0:
        wx.CallAfter(Publisher.sendMessage, "update", "DEX加密：失败")
        os.system("rm -f " + dexpath)
        os.system("rm -rf " + zipdexpath)
        os.system("mv -f " + fakedexpath + " " + dexpack)
        os.system("rm -f " + fakeapkname)
        os.system("rm -rf " + fakedexpack)
        return 1
    else:
        wx.CallAfter(Publisher.sendMessage, "update", "DEX加密：成功")

    configPath = "config.bin"

    if kind == 1:
        keySwitch = keyTrans.Keyswitch(keyInput, sopath, sopathx86, keyswitchname)
    else:
        keySwitch = keyTrans_db.Keyswitch(keyInput, sopathdb, sopathx86db, keyswitchname)

    # 对dex文件进行挖空处理以及对壳模板dex和so进行签名
    #pdb.set_trace()
    if kind == 1:
        EncMode.GenBinFlow(sopath, sopathx86, dexpath, zipdexpath, fakedexpath, keyInput, keySwitch, configPath)
    else:
        EncMode.GenBinFlow(sopathdb, sopathx86db, dexpath, zipdexpath, fakedexpath, keyInput, keySwitch, configPath)


    bin_des_path = dexpack + "/assets"
    if(not os.path.exists(bin_des_path)):
        os.makedirs(bin_des_path)

    os.system("mv -f class.bin " + bin_des_path)
    os.system("rm -f "+dexpath)
    os.system("rm -rf " + zipdexpath)
    os.system("mv -f "+fakedexpath+" "+dexpack)
    os.system("rm -f " +  fakeapkname)
    os.system("rm -rf " + fakedexpack)


    so_des_path = dexpack + "/lib/armeabi"
    so_dex_path_v7 = dexpack + "/lib/armeabi-v7a"
    so_des_path_x86 = dexpack + "/lib/x86"
    so_des_path_v8 = dexpack + "/lib/arm64-v8a"


    so_name_fp = open("./soname.txt", 'wb+')
    so_name_fp.close()
    is_so = readConfig(3)
    print "is_so: %s" % is_so

    if is_so == '1':
        so_name_fp = open("./soname.txt", 'wb+')
        if (os.path.exists(so_dex_path_v7)):
            for filename in os.listdir(so_dex_path_v7):
                print(filename)
                # global so_name_fp
                so_name_fp.writelines(filename + ' ')
                #            if( "libRDO" not in filename):
                #                encryptso.encappso(keyInput, so_des_path+"/"+filename)
                encryptso.encappso(keyInput, so_dex_path_v7 + "/" + filename)
            wx.CallAfter(Publisher.sendMessage, "update", "armeabi-v7a中so文件加密：成功")
            so_name_fp.close()
        so_name_fp = open("./soname.txt", 'rb')
        solist = so_name_fp.read()
        so_name_fp.close()

        so_name_fp = open("./soname.txt", 'ab+')
        if (os.path.exists(so_des_path)):
            for filename in os.listdir(so_des_path):
                print(filename)
                # global so_name_fp
                if (filename not in solist):
                    so_name_fp.writelines(filename + ' ')
                #            if( "libDRO" not in filename):
                #                encryptso.encappso(keyInput, so_dex_path_v7+"/"+filename)
                encryptso.encappso(keyInput, so_des_path + "/" + filename)
            wx.CallAfter(Publisher.sendMessage, "update", "armeabi中so文件加密：成功")
            so_name_fp.close()
    else:
        solist = ''

    lib_bin = open('./lib.bin', 'wb+')
    fp_libname = open('./soname.txt', 'rb')
    content = fp_libname.read()
    #    entent = base64.b64encode(content)
    #    len_entent = len(entent)
    len_content = len(content)

    fp_libname.close()
    lib_bin.write(str(len_content))
    lib_bin.writelines('\n')
    lib_bin.write(content)
    lib_bin.writelines('\n');
    lib_bin.close()

    lib_bin = open('./lib.bin', 'ab+')

    if (os.path.exists(so_dex_path_v7)):
        for filename in os.listdir(so_dex_path_v7):
            print(filename)
            # global so_name_fp
            # so_name_fp.writelines(filename + ' ')
            # if( "libRDO" not in filename):
            # encryptso.encappso(keyInput, so_des_path+"/"+filename)
            so_len = os.path.getsize(so_dex_path_v7 + "/" + filename)
            lib_bin.write(str(so_len))
            lib_bin.writelines('\n')
            so_fp = open(so_dex_path_v7 + "/" + filename, "rb")
            so_cont = so_fp.read()
            lib_bin.write(so_cont)
            lib_bin.writelines('\n');
            so_fp.close();
            os.remove(so_dex_path_v7 + "/" + filename)
    if (os.path.exists(so_des_path)):
        for filename in os.listdir(so_des_path):
            print(filename)
            # global so_name_fp
            # so_name_fp.writelines(filename + ' ')
            # if( "libRDO" not in filename):
            # encryptso.encappso(keyInput, so_des_path+"/"+filename)
            if (filename not in solist):
                so_len = os.path.getsize(so_des_path + "/" + filename)
                lib_bin.write(str(so_len))
                lib_bin.writelines('\n')
                so_fp = open(so_des_path + "/" + filename, "rb")
                so_cont = so_fp.read()
                lib_bin.write(so_cont)
                lib_bin.writelines('\n')
                so_fp.close();
            os.remove(so_des_path + "/" + filename)
    lib_bin.close()
    # end create



    if(os.path.exists(so_des_path_v8)):
        shutil.rmtree(so_des_path_v8)
    if(os.path.exists(so_des_path_x86)):
        shutil.rmtree(so_des_path_x86)
    try:

    #    shutil.copy(sopath, so_des_path)
    #    encryptso.encso(keyInput, so_des_path+"/libsec.so")
    #    wx.CallAfter(Publisher().sendMessage, "update", "libsec.SO文件：")
        if(os.path.exists(so_des_path)):
            print "00000000000000"
        else:
            os.makedirs(so_des_path)
            print "11111111111111"

        if kind == 1:
            shutil.copy(sopath, so_des_path)
            encryptso.encso(keyInput, so_des_path+"/libsec.so")
        else:
            shutil.copy(sopathdb, so_des_path)
            encryptso.encso(keyInput, so_des_path+"/libsec.so")
        wx.CallAfter(Publisher.sendMessage, "update", "核心so文件加密：成功")
    except :
        print "error!!!!!!!!!!!!!!!!!!!!!!!"


    if kind == 1:
        if(os.path.exists(so_dex_path_v7)):
            cmd_moveso_v7 = "cp -f " + sopath + " " + so_dex_path_v7
            os.system(cmd_moveso_v7)
            encryptso.encso(keyInput, so_dex_path_v7+"/libsec.so")
    else:
        if(os.path.exists(so_dex_path_v7)):
            cmd_moveso_v7 = "cp -f " + sopathdb + " " + so_dex_path_v7
            os.system(cmd_moveso_v7)
            encryptso.encso(keyInput, so_dex_path_v7+"/libsec.so")

    #################################################################################
    # 9. 修改xml
    if not modXml.modify(dexpack) == 0:
        wx.CallAfter(Publisher.sendMessage, "update", "修改AndroidManifest.xml：失败")
        return 1
    else:
        wx.CallAfter(Publisher.sendMessage, "update", "修改AndroidManifest.xml：成功")
    #################################################################################

    unpack.zipblankdex()
    bin_dex_path="dexpack/assets/classes.dex"


    cmd_moveclasses_dex = "mv -f classes1.dex " + bin_dex_path
    cmd_cpconfig = "cp -f config.bin " + bin_des_path
    cmd_cpcodedata2 = "mv -f codedata2.txt " + bin_des_path
    cmd_cpcodeinfo2 = "mv -f codeinfo2.txt " + bin_des_path
    ge_wb_data.GeWBData(keyInput)
    os.system("mv -f ./inv_M.m ./wb/inv_M.m")
    os.system("mv -f ./inv_tables.t ./wb/inv_tables.t")
    os.system("rm -f ./L.m")
    os.system("rm -f ./R.m")
    os.system("rm -f ./L_inv.m")
    os.system("rm -f ./R_inv.m")
    os.system("rm -f ./classes.dex")
    os.system("rm -f ./codedata.txt")
    os.system("rm -f ./codeinfo.txt")

    cmd_move_wb1 = "cp ./wb/inv_M.m " + bin_des_path
    cmd_move_wb2 = "cp ./wb/inv_tables.t " + bin_des_path
    cmd_move_wb3 = "cp ./soname.txt " + bin_des_path
    cmd_move_wb4 = "cp ./lib.bin " + bin_des_path
    os.system(cmd_moveclasses_dex)
    os.system(cmd_move_wb1)
    os.system(cmd_cpcodedata2)
    os.system(cmd_cpcodeinfo2)
    os.system(cmd_cpconfig)
    os.system(cmd_move_wb2)
    os.system(cmd_move_wb3)
    os.system(cmd_move_wb4)
    os.system("rm -f ./soname.txt")
    os.system("rm -f ./lib.bin")

    ################################################################################################
    # 10. apk签名
    cmd_remove_META="rm -rf " + dexpack + "/META-INF"
    os.system(cmd_remove_META)
    #apk_unsign_path = apkpath + "unsign.apk"
    apk_unsign_path="test/unsign.apk"

    unpack.zipapk(dexpack, apk_unsign_path)

    #apk_sign_path = apkpath + "sign.apk"
    apk_sign_path="test/sign.apk"
    #reSign.reSigner(apk_unsign_path, apk_sign_path, keystorepath)
    signResult = reSign.reSigner(apk_unsign_path, apk_sign_path, keystorepath, keystorePassword, keypass, keystoreAlias)

    cmd_cpfile="cp -f "+ apk_sign_path + " "+ apkpath + "sign.apk"
    os.system(cmd_cpfile)

    cmd_remove_signapk = "rm -f " + apk_sign_path
    cmd_remove_unsignapk = "rm -f " + apk_unsign_path

    cmd_remove_dexpack = "rm -rf " + dexpack
    cmd_remove_toolpack = "rm -rf " + toolpack
    cmd_remove_fakepack = "rm -rf " + fakedexpack
    cmd_remove_fakeapk = "rm -f " + fakeapkname

    os.system(cmd_remove_signapk)
    os.system(cmd_remove_unsignapk)
    os.system(cmd_remove_dexpack)
    os.system(cmd_remove_toolpack)
    os.system(cmd_remove_fakepack)
    os.system(cmd_remove_fakeapk)

    if not signResult  == 0:
        wx.CallAfter(Publisher.sendMessage, "update", "APK签名：失败! 请检查输入的密钥参数是否正确")
        return 1
    else:
        wx.CallAfter(Publisher.sendMessage, "update", "APK签名：成功")
        return 0
    ###############################################################################################

