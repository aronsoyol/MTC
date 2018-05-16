# Mongolian Text Component

这是一个用c++实现的在Windows上的蒙古文显示软件。

现在的蒙古文UNICODE编码被设计成了需要复杂渲染（选型）引擎的支持才能正确显示的编码。
可是遗憾的是几乎所有的软件都不支持蒙古文的特殊的选型规则。
而且蒙古文是竖排的，这一点也是很多软件，或者控件不支持的。
所以想要解决蒙古文的显示问题，一定要从底层的图形显示，以及文本渲染等等开始着手开发。

这里提供的C++代码展示了如何在VC++环境中调用FreeType和HarfBuzz来渲染和显示蒙古文的方法。

希望能给年轻的蒙古文软件开发者一点启示。

***


## 两点注意

1. JDK的位数（32， 64）必须和工程匹配
2. 系统的PATH里必须有JAVA的路径（如果报找不到JVM.dll的错误，可能就是path里没有java路径的原因）

## 使用JDK的理由

使用了jdk的BreakIterator进行切字，也可以使用ICU。

如果使用JDK就定义JNI宏，使用ICU就定义ICU宏，二选一。

## 何为"切字"

[http://userguide.icu-project.org/boundaryanalysis](http://userguide.icu-project.org/boundaryanalysis)

如果你会java就试一下BreakIterator的功能。


## 安卓版：

码云：[https://gitee.com/aronsoyol/MTCA](https://gitee.com/aronsoyol/MTCA)

github:[https://www.github.com/aronsoyol/mtca](https://www.github.com/aronsoyol/mtca)
