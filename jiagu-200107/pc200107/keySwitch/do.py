_author_="lyy"
import os
def doit():
    #cmd_1 = "cd ../keySwitch"
    cmd_2 = "python ./genkeyTrans.py"
    cmd_3 = "cp ./_keyTrans.so ../apktool"
    cmd_4 = "cp ./keyTrans.py ../apktool"
    #os.system(cmd_1)
    if os.system(cmd_2) == 0:
        print(cmd_2)
    if os.system(cmd_3) == 0:
        print(cmd_3)
    if os.system(cmd_4) == 0:
        print(cmd_4)

if __name__ == '__main__':
    doit()
