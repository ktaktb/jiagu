_author_="lyy"

def doit():
    #cmd_1 = "cd ../keySwitch"
    cmd_2 = "python genkeyTrans.py"
    cmd_3 = "cp ./_keyTrans.so ../apktool"
    cmd_4 = "cp ./keyTrans.py ../apktool"
    cmd_5 = "cd ../apktool"
    #os.system(cmd_1)
    os.system(cmd_2)
    os.system(cmd_3)
    os.system(cmd_4)
    os.system(cmd_5)