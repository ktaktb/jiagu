# -*- coding: utf-8 -*-
_author_ = "tangqi"

import os
import time
import wx
import wx.lib.filebrowsebutton as filebrowse
import wx.lib.buttons  as  buttons
import PCflow

import sys

reload(sys)
sys.setdefaultencoding('utf8')

from threading import Thread

#from wx.lib.pubsub import Publisher
from wx.lib.pubsub import setuparg1
from wx.lib.pubsub import pub as Publisher

# ---------------------------------------------------------------------------

global apkFileName  # APK文件名
apkFileName = ""

global keystoreFileName  # 密钥文件名
keystoreFileName = ""

global keystoreAlias  # 密钥别名
keystoreAlias = ""

global keystorePassword  # 密钥密码storepass
keystorePassword = ""

global keypass  # 密钥密码keypass
keypass = ""

global reinforce_config  # 加固可选配置list ([0:"防内存dump",1:"so库加密",2:"防调试", 3:"数据防护"])
reinforce_config = []

global runtime_config_timelimit  # 启动次数限制
runtime_config_timelimit = -1  # 默认为-1，表示不设置次数

global rumtime_config_imeilimit  # IMEI号限制,多个IMEI以 ; 分隔
rumtime_config_imeilimit = ""


# ---------------------------------------------------------------------------

class PCflowThread(Thread):
   def __init__(self):
       Thread.__init__(self)
       self.start()


   def run(self):
       print u"正在加壳..."
       sys.stdout.flush()
       print u"正在加壳000000..."
       result = PCflow.pcworkflow(apkFileName, keystoreFileName, keystorePassword, keypass, keystoreAlias)
       if result == 0:
           wx.CallAfter(Publisher.sendMessage, "update", "Thread finished")
       else:
           wx.CallAfter(Publisher.sendMessage, "update", "Thread error")


# ---------------------------------------------------------------------------

class MyPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent)
        self.log = log

        # 初始化控件
        self.mainPanel = wx.Panel(self)
        self.text_staticbox = wx.StaticBox(self.mainPanel, -1, u"请选择要加固的APK文件")
        self.filebrowseBtn = filebrowse.FileBrowseButton(self.mainPanel, -1,
                                                         size=(560, -1),
                                                         labelText=u"选择APK",
                                                         buttonText=u"浏览文件",
                                                         dialogTitle=u"请选择要加固的APK文件",
                                                         fileMask=u"*.apk",
                                                         fileMode=wx.FD_OPEN,
                                                         changeCallback=self.OnSelectApkFile)

        self.keystore_staticbox = wx.StaticBox(self.mainPanel, -1, u"请选择签名使用的密钥文件")
        self.keystoreFilebrowseBtn = filebrowse.FileBrowseButton(self.mainPanel, -1,
                                                                 size=(560, -1),
                                                                 labelText=u"选择密钥",
                                                                 buttonText=u"浏览文件",
                                                                 dialogTitle=u"请选择签名使用的密钥文件",
                                                                 fileMask=u"*.keystore",
                                                                 fileMode=wx.FD_OPEN,
                                                                 changeCallback=self.OnSelectKeystore)
        self.keyalias_lable = wx.StaticText(self.mainPanel, -1, u"密钥别名")
        self.keyalias_text = wx.TextCtrl(self.mainPanel, -1, "", size=(110, -1))
        self.password_lable = wx.StaticText(self.mainPanel, -1, u"密钥库密码")
        self.password_text = wx.TextCtrl(self.mainPanel, -1, "", size=(110, -1), style=wx.TE_PASSWORD)

        self.keypassword_lable = wx.StaticText(self.mainPanel, -1, u"私钥密码")
        self.keypassword_text = wx.TextCtrl(self.mainPanel, -1, "", size=(110, -1), style=wx.TE_PASSWORD)

        # 加固可配置选项
        self.reinforce_config_staticbox = wx.StaticBox(self.mainPanel, -1, u"加固可选配置", size=(270, -1), )
        self.reinforce_config_checklistbox = wx.CheckListBox(self.mainPanel, -1,
                                                             choices=[u"防内存dump", u"so库加密", u"防调试", u"数据防护"],
                                                             size=(250, -1))
        self.reinforce_config_tip = wx.StaticText(self.mainPanel, -1, u"*开启数据防护后,程序运行效率可能会降低!")

        # 运行时可选配置
        self.runtime_config_staticbox = wx.StaticBox(self.mainPanel, -1, u"运行时可选配置", size=(300, -1), )
        self.runtime_config_timelimit_lable = wx.StaticText(self.mainPanel, -1, u"启动次数限制")
        list1 = [u'不限制']
        for i in range(0, 100):
            list1.append(str(i))
        self.runtime_config_timelimit_list = wx.Choice(self.mainPanel, -1, choices=list1, size=(170, -1))
        self.runtime_config_timelimit_list.SetSelection(0)
        self.runtime_config_imeilimit_lable = wx.StaticText(self.mainPanel, -1, u"IMEI号限制  ")
        self.runtime_config_imeilimit_text = wx.TextCtrl(self.mainPanel, -1, "", size=(170, -1))
        self.runtime_config_imeilimit_tip = wx.StaticText(self.mainPanel, -1, u"   *多个IMEI号请用英文';'分隔")

        # 开始加固按钮
        self.reinforceBtn = wx.Button(self.mainPanel, -1, u"\n立即加固\n", size=(590, 45))

        # 日志信息
        self.log_lable = wx.StaticText(self.mainPanel, -1, u"加固日志信息")
        self.log_text = wx.TextCtrl(self.mainPanel, -1, "", style=wx.TE_MULTILINE)

        # 绑定回调方法
        self.Bind(wx.EVT_BUTTON, self.OnReinforceApk, self.reinforceBtn)
        self.Bind(wx.EVT_CHECKLISTBOX, self.EvtCheckBox, self.reinforce_config_checklistbox)
        self.Bind(wx.EVT_CHOICE, self.EvtChoice, self.runtime_config_timelimit_list)

        # 布局
        frameSizer = wx.BoxSizer(wx.VERTICAL)
        panelSizer = wx.BoxSizer(wx.VERTICAL)
        boxSizer1 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer2 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer3 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer5 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer6 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer7 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer8 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer9 = wx.BoxSizer(wx.HORIZONTAL)
        boxSizer10 = wx.BoxSizer(wx.HORIZONTAL)

        staticboxSizer = wx.StaticBoxSizer(self.text_staticbox, wx.HORIZONTAL)
        staticboxSizer.Add(self.filebrowseBtn, 0, wx.ALL, 10)

        boxSizer3.Add((6, 6))
        boxSizer3.Add(self.keyalias_lable, 0, wx.ALL, 0)
        boxSizer3.Add((6, 6))
        boxSizer3.Add(self.keyalias_text, 0, wx.ALL, 0)
        boxSizer3.Add((10, 6))

        boxSizer3.Add(self.password_lable, 0, wx.ALL, 0)
        boxSizer3.Add((6, 6))
        boxSizer3.Add(self.password_text, 0, wx.ALL, 0)
        boxSizer3.Add((10, 6))
        boxSizer3.Add(self.keypassword_lable, 0, wx.ALL, 0)
        boxSizer3.Add((6, 6))
        boxSizer3.Add(self.keypassword_text, 0, wx.ALL, 0)

        staticboxSizer1 = wx.StaticBoxSizer(self.keystore_staticbox, wx.VERTICAL)
        staticboxSizer1.Add(self.keystoreFilebrowseBtn, 0, wx.ALL, 10)
        staticboxSizer1.Add(boxSizer3, 0, wx.ALL, 6)

        # reinforce config----------------------------
        staticboxSizer2 = wx.StaticBoxSizer(self.reinforce_config_staticbox, wx.VERTICAL)
        staticboxSizer2.Add(self.reinforce_config_checklistbox, 0, wx.ALL, 10)
        staticboxSizer2.Add(self.reinforce_config_tip, 0, wx.ALL, 0)
     #   boxSizer7.Add(20, 10)

        boxSizer7.Add((20, 10))
        boxSizer7.Add(staticboxSizer2, 0, wx.ALL, 0)
        # --------------------------------------------

        # runtime config------------------------------
        staticboxSizer3 = wx.StaticBoxSizer(self.runtime_config_staticbox, wx.VERTICAL)
        staticboxSizer3.Add(boxSizer8, 0, wx.ALL, 10)
        staticboxSizer3.Add(boxSizer9, 0, wx.ALL, 10)
        staticboxSizer3.Add(self.runtime_config_imeilimit_tip, 0, wx.ALL, 0)
        staticboxSizer3.Add((20, 10))

        boxSizer8.Add(self.runtime_config_timelimit_lable, 0, wx.ALL, 0)
        boxSizer8.Add((6, 6))
        boxSizer8.Add(self.runtime_config_timelimit_list, 0, wx.ALL, 0)

        boxSizer9.Add(self.runtime_config_imeilimit_lable, 0, wx.ALL, 0)
        boxSizer9.Add((6, 6))
        boxSizer9.Add(self.runtime_config_imeilimit_text, 0, wx.ALL, 0)
        boxSizer7.Add((20, 10))
     #   boxSizer7.Add(20, 10)
        boxSizer7.Add(staticboxSizer3, 0, wx.ALL, 0)
        # --------------------------------------------
        boxSizer1.Add((20, 20))
        boxSizer1.Add(staticboxSizer, 0, wx.ALL, 0)

        boxSizer2.Add((20, 20))
        boxSizer2.Add(staticboxSizer1, 0, wx.ALL, 0)

        boxSizer5.Add((20, 20))
        boxSizer5.Add(self.log_lable, 0, wx.ALL, 0)

        boxSizer6.Add((20, 20))
        boxSizer6.Add(self.log_text, 1, wx.EXPAND | wx.ALL, 0)
        boxSizer6.Add((20, 20))

        boxSizer10.Add((20, 20))
        boxSizer10.Add(self.reinforceBtn, 0, wx.ALL, 0)

        panelSizer.Add(boxSizer1, 0, wx.ALL, 5)
        panelSizer.Add(boxSizer2, 0, wx.ALL, 5)
        panelSizer.Add(boxSizer7, 0, wx.ALL, 5)
        panelSizer.Add(boxSizer10, 0, wx.ALL, 5)
        panelSizer.Add(boxSizer5, 0, wx.ALL, 5)
        panelSizer.Add(boxSizer6, 1, wx.EXPAND | wx.ALL, 5)

        panelSizer.Add((20, 20))

        self.mainPanel.SetSizer(panelSizer)
        frameSizer.Add(self.mainPanel, 1, wx.EXPAND)
        self.SetSizer(frameSizer)
        frameSizer.Layout()

        # 监听update消息，回调updateDisplay事件

        Publisher.subscribe(self.updateDisplay, "update")

    def EvtCheckBox(self, evt):
        global reinforce_config
        reinforce_config = self.reinforce_config_checklistbox.GetChecked()
        print reinforce_config

    def EvtChoice(self, evt):
        global runtime_config_timelimit
        runtime_config_timelimit = self.runtime_config_timelimit_list.GetCurrentSelection()-1
        print u"runtime_config_timelimit: " + str(runtime_config_timelimit)

    def OnSelectApkFile(self, evt):
        global apkFileName
        apkFileName = evt.GetString()
        self.log_text.AppendText(u"选择APK文件：" + apkFileName + "\n")

        print u"选择APK文件：" + apkFileName
        sys.stdout.flush()



    def OnSelectKeystore(self, evt):
        global keystoreFileName
        keystoreFileName = evt.GetString()
        self.log_text.AppendText(u"选择密钥文件：" + keystoreFileName + "\n")

        print u"选择密钥文件：" + keystoreFileName
        sys.stdout.flush()

    def writeConfig(self):
        print u"runtime_config_timelimit: " + str(runtime_config_timelimit)
        # 获取运行时IMEI限制
        print u"rumtime_config_imeilimit::"+rumtime_config_imeilimit
        # [0:"防内存dump", 1:"so库加密", 2:"防调试", 3:"数据防护"]
        print u"reinforce_config: " + str(reinforce_config)

        f = open('config.bin', 'r+')
        config = f.read().splitlines()
        config[0] = '0'
        config[1] = str(runtime_config_timelimit)
        if runtime_config_timelimit == -1:
            self.log_text.AppendText(u"加固应用对启动次数无限制\n")
        else:
            self.log_text.AppendText(u"加固应用限制启动次数:"+str(runtime_config_timelimit)+"\n")

        if rumtime_config_imeilimit == "":
            self.log_text.AppendText(u"加固应用对设备IMEI号无限制\n")
        else:
            self.log_text.AppendText(u"加固应用仅支持的设备IMEI号:"+rumtime_config_imeilimit+"\n")

        if 0 in reinforce_config:
            config[2] = '1'
            self.log_text.AppendText(u"加固应用支持防内存dump\n")
        else:
            config[2] = '0'

        if 1 in reinforce_config:
            config[3] = '1'
            self.log_text.AppendText(u"加固应用支持so文件加密\n")
        else:
            config[3] = '0'

        if 2 in reinforce_config:
            config[4] = '1'
            self.log_text.AppendText(u"加固应用支持防调试\n")
        else:
            config[4] = '0'

        if 3 in reinforce_config:
            config[5] = '1'
            self.log_text.AppendText(u"加固应用支持数据防护\n")
        else:
            config[5] = '0'

        f.seek(0)
        f.write('\n'.join(config))
        f.flush()
        f.close()

    def OnReinforceApk(self, evt):
        global keystorePassword
        keystorePassword = self.password_text.GetValue()

        global keypass
        keypass = self.keypassword_text.GetValue()

        global keystoreAlias
        keystoreAlias = self.keyalias_text.GetValue()

        global rumtime_config_imeilimit
        rumtime_config_imeilimit = self.runtime_config_imeilimit_text.GetValue()

        

        if apkFileName == "":
            dlg = wx.MessageDialog(self, u"\n请选择一个要加固的APK文件! ",
                                   u"错误提示",
                                   wx.OK | wx.ICON_ERROR)
            dlg.ShowModal()
            dlg.Destroy()

        elif keystoreFileName == "":
            dlg = wx.MessageDialog(self, u"\n请选择APK签名使用的密钥文件! ",
                                   u"错误提示",
                                   wx.OK | wx.ICON_ERROR)
            dlg.ShowModal()
            dlg.Destroy()

        elif keystoreAlias == "":
            dlg = wx.MessageDialog(self, u"\n请输入密钥的别名! ",
                                   u"错误提示",
                                   wx.OK | wx.ICON_ERROR)
            dlg.ShowModal()
            dlg.Destroy()

        elif keystorePassword == "":
            dlg = wx.MessageDialog(self, u"\n请输入密钥库的密码! ",
                                   u"错误提示",
                                   wx.OK | wx.ICON_ERROR)
            dlg.ShowModal()
            dlg.Destroy()


        elif keypass == "":
            dlg = wx.MessageDialog(self, u"\n请输入私钥的密码! ",
                                   u"错误提示",
                                   wx.OK | wx.ICON_ERROR)
            dlg.ShowModal()
            dlg.Destroy()

        else:
        
            self.writeConfig()
            print u"密钥别名：" + keystoreAlias
            print u"storepass密钥密码：" + keystorePassword
            print u"keypass密钥密码：" + keypass
            sys.stdout.flush()
            self.log_text.AppendText(u"\n=============================加固开始==============================\n")
            PCflowThread()
            btn = evt.GetEventObject()
            btn.Disable()


    def updateDisplay(self, msg):
        if (msg.data == "Thread finished"):
            str = u"加固完成! 生成文件：" + apkFileName + "sign.apk"
            self.log_text.AppendText(str + "\n")
            self.log_text.AppendText(u"\n=============================加固完成==============================\n")
            dlg = wx.MessageDialog(self, "\n" + str,
                                   u"信息提示",
                                   wx.OK | wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()

            self.reinforceBtn.Enable()
        elif (msg.data == "Thread error"):
            str = u"加固失败! "
            self.log_text.AppendText(str + "\n")
            self.log_text.AppendText(u"\n=============================加固完成==============================\n")
            dlg = wx.MessageDialog(self, "\n" + str,
                                   "u错误提示",
                                   wx.OK | wx.ICON_ERROR)
            dlg.ShowModal()
            dlg.Destroy()

            self.reinforceBtn.Enable()
        else:
            self.log_text.AppendText(msg.data + "\n")


# ---------------------------------------------------------------------------

class MainWindow(wx.Frame):

    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(640, 580))
        panel = MyPanel(self, -1)
        self.SetMinSize((660, 700))


# ---------------------------------------------------------------------------

class MyApp(wx.App):

    def OnInit(self):
        frame = MainWindow(None, u"Android手机应用软件加固系统 v2.0")
        self.SetTopWindow(frame)
        frame.Show()

        return True


# ---------------------------------------------------------------------------

def main():
    app = MyApp(False)  # 不重定向
    app.MainLoop()


# ---------------------------------------------------------------------------

if __name__ == '__main__':
    __name__ = 'Main'
    print wx.__version__
    main()

# ---------------------------------------------------------------------------