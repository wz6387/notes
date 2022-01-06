​    Go引入了一个新概念：**协程**。协程与线程不同，它处于用户态，而线程属于内核态。协程必须绑定线程才能运行。由于它属于用户态，所以它的切换非常的轻量快速。

​    它的特点：

+ 占用内存更小(几kb)
+ 调度更灵活

![img](https://img-blog.csdnimg.cn/20201222203035441.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0MjA1Mjcy,size_16,color_FFFFFF,t_70)



**GMP模型**

![img](https://img-blog.csdnimg.cn/20201222203831442.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0MjA1Mjcy,size_16,color_FFFFFF,t_70)

线程想运行goroutine，必须先获取P，P中还包含了可运行的G队列

![img](https://img-blog.csdnimg.cn/20201222204129565.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0MjA1Mjcy,size_16,color_FFFFFF,t_70#pic_center)

+ 全局队列：存放等待运行的G(协程)
+ P的本地队列：存放的也是等待运行的G，存的数量有限，不超过256个。新建G时，G会优先加入到P的本地队列，如果队列满了，则会把本地队列的一半G移到全局队列
+ P：所有的P都在程序启动时创建，并保存在数组中，最多有GOMAXPROCS(可配置)个
+ M：线程想要运行任务就得获取P，从P的本地队列获取G，P队列为空时，M也会尝试从全局队列拿一批G放到P的本地队列，或从其它P的本地队列偷一半放到自己P的本地队列



**调度流程**



![img](https://img-blog.csdnimg.cn/20201222210424467.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0MjA1Mjcy,size_16,color_FFFFFF,t_70#pic_center)

+ 通过go func创建一个goroutine；
+ 有两个存储 G 的队列，一个是局部调度器 P 的本地队列、一个是全局 G 队列。新创建的 G 会先保存在 P 的本地队列中，如果 P 的本地队列已经满了就会保存在全局的队列中；
+ G 只能运行在 M 中，一个 M 必须持有一个 P，M 与 P 是 1：1 的关系。M 会从 P 的本地队列弹出一个可执行状态的 G 来执行，如果 P 的本地队列为空，就会想其他的 MP 组合偷取一个可执行的 G 来执行；
+ 一个 M 调度 G 执行的过程是一个循环机制；
+ 当 M 执行某一个 G 时候如果发生了 syscall 或则其余阻塞操作，M 会阻塞，如果当前有一些 G 在执行，runtime 会把这个线程 M 从 P 中摘除 (detach)，然后再创建一个新的操作系统的线程 (如果有空闲的线程可用就复用空闲线程) 来服务于这个 P；
+ 当 M 系统调用结束时候，这个 G 会尝试获取一个空闲的 P 执行，并放入到这个 P 的本地队列。如果获取不到 P，那么这个线程 M 变成休眠状态， 加入到空闲线程中，然后这个 G 会被放入全局队列中