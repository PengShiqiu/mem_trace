##   C/C++ Linux环境内存泄漏检测工具

------------

## 1.原理

通过重写（劫持）malloc和free函数，在对内存进行申请和释放时增加trace信息，并通过写文件方式记录内存申请，内存释放时删除文件，程序运行结束后查看存储在文件系统中是否存在未被删除的.trace文件，在通过addr2line工具确认泄漏位置。

## 1.编译

clone 代码到本地：`git@github.com:PengShiqiu/mem_trace.git`

编译测试代码：`cd mem_trace` 
            `mkdir build`
            `cmake ..`
            `make`

## 2.测试
运行测试demo结果：`./mem_trace_test `

## 3.查看trace文件
查看trace信息：`ls ./mem_trace`
