# c++ 软渲染器

一个很简单的渲染器, 只有三个头文件和一个 cpp 文件

 

## 简介&特征

+ 坐标变换采用 opengl 标准
+ 实现 Vector/Matrix 数学库
+ 实现 Bitmap 工具库, 包含加载纹理,采样纹理,画点,画线等操作
+ 定点着色器和片元着色器由 c++ 编写
+ 光栅化采用 Edge Equation 方法
+ 利用重心坐标插值
+ 采样使用双线性插值



## 结构

结构也很简单

```text
ToyRenderer --
    |-- include
        |-- Bitmap.h    -- 操作 bitmap 的工具库
        |-- Renderer.h  -- 渲染核心
        |-- Vector.h    -- 数学库,实现了 Vector 和 Matrix
        |-- Model.h		-- 模型的加载
    | -- src
    	| -- main.cpp   -- 程序入口

```



## 效果

<img src="https://aolixin-typora-image.oss-cn-beijing.aliyuncs.com/image-20240213211846234.png" alt="image-20240213211846234" style="zoom: 67%;" />



<img src="https://aolixin-typora-image.oss-cn-beijing.aliyuncs.com/image-20240213205633277.png" alt="image-20240213205633277" style="zoom:67%;" />



## 主要参考

https://github.com/skywind3000/RenderHelp

https://bbs.huaweicloud.com/blogs/358776

https://github.com/ssloy/tinyrenderer