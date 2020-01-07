encode：提供生成壳程序的回编译环境；
libs：安全so库目录；
shellfile：壳程序smali模板文件（AES算法模式）；
test：代加固程序存放目录；
wb：白盒解密的查找表的存放目录；

maketest：程序运行入口
PCflow：程序加壳流程
unpack：对文件解压缩
AdapterApktool：对文件回编译
getXml：解析xml文件获取明文
modXml：修改xml组件信息
smalishell：修改smali文件组件信息（AES算法模式）
smalishellsm4：修改smali文件组件信息（SM4算法模式）
GenPassword：产生16字节随机密钥
keyTrans：密钥变换的脚本接口文件（AES算法模式）
keyTranssm4：密钥变换的脚本接口文件（SM4算法模式）
EncMode：生成classes.bin文件的脚本接口文件（AES算法模式）
EncModesm4：生成classes.bin文件的脚本接口文件（SM4算法模式）
encryptso：对so库进行加密
ge_wb_data：基于密钥生成解密需要的查找表
reSign：对apk进行签名

