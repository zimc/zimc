客户端


使用说明: 
(0.1)
1 使用时运行的是 test.exe，不是 zimc.exe. 
2 如果想改变服务器 IP 和端口，手工配置 test.exe 属性. 

(0.2)
1 本版本运行不再需要配置 ip 和端口, 但是域名被硬编码在代码中. 
  如需修改, 需要改变程序并重新编译. 
2 因此也不需要 test.exe. 





注意：
1 程序崩溃时，会生成一个 CrashMiniDumpFile.dmp 文件，
  请将 *** CrashMiniDumpFile.dmp，zimc.exe 和 zimc.pdb *** 一起
  打包发送给我。 
2 联系方式: 624092275 (qq). 





VS2005 编译问题: 
1 推荐使用 VS2008 编译, 如果使用 VS2005 编译, 使用 
  zimc.VS2005.sln
2 zimc\res\zimc2.ico 在 VS2005 中可能会有问题，因此需要使
  用 vs2005 可以使用的 ico 代替. 


程序界面与图片: 
1 关于程序所使用的图片, 以前的程序在 bin 目录下总是有一个 
  zimc.skin 目录, 此目录存放了程序所使用的图片. 
2 但是实际的发布程序是没有此目录, 其中多了一个 zimc.dat 文件, 
  实际上 zimc.dat 就是 zimc.skin 目录下的所有东西, 使用 zip 压缩
  工具打包得来的. 可以使用解压工具查看. 
3 因此: ******************************* 
  对于使用者, 如果改变图片或者界面的一些样式, 需要将相应的图片压
  缩打包进 zimc.dat 包中, 才能得到想要的效果.  

