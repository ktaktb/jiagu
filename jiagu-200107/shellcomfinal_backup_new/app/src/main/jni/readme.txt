本目录是为了生成安全so文件，主要实现了防调试、dex签名验证、dex文件保护、so库保护及二次加载等功能。

sec.cpp：为程序主流程序，完成各功能模块的调用；
godin_io_hook.cpp：完成需要hook函数的注册流程；
dexdec：解析classes.bin文件，获取黑盒密钥、壳程序dex签名值和原程序dex密文；
dexld：修改dalvik模式下dex转odex的源码，将输入输出过程对文件的操作变为内存的操作，保证安全性；
godin-hook：提供hook框架，改变目标进程执行流程，替换原函数功能，实现加密等功能；
include：提供hook框架，改变目标进程执行流程，替换原函数功能，实现加密等功能；
javaadb：监控进程端口，防止java调试；
nativeadb：创建进程环相互监控，实现防native debug；
seclib：加密库，实现签名、AES等算法。
