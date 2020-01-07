# coding=utf-8
__author__ = 'lwl'

import shutil
import os
import xml.etree.ElementTree as ET
import re

class shellcreate:
    apkname=''

    def __init__(self,name):
        self.apkname=name

    def shell(self):
        tree = ET.parse('toolpack/AndroidManifest.xml')
        root=tree.getroot()
        packagename=root.get('package')

 
        applicationname=''
        mainactivityname=''


        for elem in tree.iter(tag='application'):
            elemkeys=elem.keys()
            applicationname=0
            for elem1 in elemkeys:
                pattern=re.compile('\{.*\}name')
                match=pattern.match(elem1)
                if match:
                    applicationname=elem.get(elem1)
        print "this is applicationname"
        print applicationname


        if not applicationname==0:
            appname=applicationname
            pattern=re.compile('\..*')
            match=pattern.match(appname)
            if match:
                appname=packagename+appname
            if appname==applicationname:
                appname=packagename+'.'+appname       
            apptemp2=applicationname.split('.')
            if len(apptemp2)>=2:
                if len(apptemp2[0])>0:
                    appname=applicationname 

            print "this is packagename"
            print packagename
            print "this is appname"
            print appname


        if not applicationname==0:
            appdirs=os.path.split(appname.replace('.','/'))
            apptemp=str(appdirs[0])+'/MyDemoApplication'
            apptemp=apptemp.replace('/','.')
            print "this is apptemp"
            print apptemp
        

        if not os.path.lexists('toolpack/assets/'):
            os.makedirs('toolpack/assets/')
        apktar='toolpack/assets/test1.apk'
        shutil.copyfile(self.apkname,apktar)
        if os.path.lexists('toolpack/smali/'):
            shutil.rmtree('toolpack/smali/')
        os.mkdir('toolpack/smali/')
        #os.mkdir('smali/android/')
        #shutil.copytree('shellfile/android','toolpack/smali/android')
        #os.chdir('toolpack/smali/')

        mgrtardirs='toolpack/smali/com/example/forshell/'
        mgrfile="shellfile/ClassLoaderMgr.smali"
        smifile="shellfile/Smith.smali"
        reffile="shellfile/Refinvoke.smali"
        mgrfile2="shellfile/noapplication/ClassLoaderMgr.smali"
        smifile2="shellfile/noapplication/Smith.smali"
        reffile2="shellfile/noapplication/Refinvoke.smali"
        customloader="shellfile/CustomerClassLoader.smali"

        os.makedirs(mgrtardirs)
        shutil.copy(customloader,mgrtardirs)
        if not applicationname==0:
            shutil.copy(mgrfile,mgrtardirs)
            shutil.copy(smifile,mgrtardirs)
            shutil.copy(reffile,mgrtardirs)
        else:
            shutil.copy(mgrfile2,mgrtardirs)
            shutil.copy(smifile2,mgrtardirs)
            shutil.copy(reffile2,mgrtardirs)


       

        if not applicationname==0:
            ################################
            print "this is appname"
            print appname
            ################################
            appdirs=appname.replace('.','/')
            appdirs=appdirs+'.smali'
            appfilepath=os.path.split(appdirs)
            #################################################
            print "this is appfilepath"
            print appfilepath
            #################################################
            appfile="shellfile/MyDemoApplication.smali"
            apptarfile='toolpack/smali/'+appfilepath[0]+'/MyDemoApplication.smali'
            ##################################
            print "this is apptarfile"
            print apptarfile
            ##################################
            apptemp=appfilepath[0]+'/MyDemoApplication'
            ##########################################################
            print "this is apptemp"
            print apptemp
            ##########################################################
            if apptemp=='/MyDemoApplication':
                apptemp="MyDemoApplication"
            ##########################################
            print "this is modify apptemp"
            print apptemp
            ##########################################
            apppath='toolpack/smali/'+appfilepath[0]
            if not os.path.lexists(apppath):
                os.makedirs(apppath)
            shutil.copy(appfile,apptarfile)
            appf=open(apptarfile,'r')
            applines=appf.readlines()
            appf.close()
            appfs=open(apptarfile,'w')
            for line in applines:
                appfs.write(line.replace('com/test/testshell/MyDemoApplication',str(apptemp))\
                            .replace('com.test.testshell.MyApplication',str(appname)))
            appfs.close()

        else:
            print "this apk has no application"
            appfile2="shellfile/noapplication/MyDemoApplication.smali"
            appfilepath=packagename.replace('.','/')
            print "this is noapplication--appfilepath"
            print appfilepath
            apptarfile2='toolpack/smali/'+appfilepath+'/MyDemoApplication.smali'
            print "this is noapplication--apptarfile"
            print apptarfile2
            apptemp=appfilepath+'/MyDemoApplication'
            if apptemp=='/MyDemoApplication':
                apptemp="MyDemoApplication"
            print "this is noapplication--apptemp"
            print apptemp
            apppath='toolpack/smali/'+appfilepath
            if not os.path.lexists(apppath):
                os.makedirs(apppath)
            shutil.copy(appfile2,apptarfile2)
            appf=open(apptarfile2,'r')
            applines=appf.readlines()
            appf.close()
            appfs=open(apptarfile2,'w')
            for line in applines:
                appfs.write(line.replace('com/test/testshell/MyDemoApplication',str(apptemp)))
            appfs.close()



        



