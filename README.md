# 软光栅



## 简介&特征

+ 实现标准渲染管线
+ 着色器由 c++ 编写
+ 坐标系采用 directX 标准
+ 实现 Vector/Matrix 数学库, 包括常用矢量算法和矩阵算法
+ 实现 DDA, Bresenham 画线算法
+ 光栅化参数用重心插值
+ 实现 [ blinn-phong光照模型 ] 和 [ Gouraud光照模型 ]
+ 实现 obj 模型工具库
+ 加载 tga 纹理工具库
+ 实现 [ 双线性插值 ] 和 [ 双三次插值 ] 纹理采样算法
+ 实现 mipmap
+ 实现 msaa 抗锯齿算法



## 画线算法

![image.png](img/image.png)

## 画立方体

![image.png](img/image%201.png)

![image.png](img/image%202.png)

## 光栅化

![image.png](img/image%203.png)

![image.png](img/image%204.png)

## 加顶点光照

![632e4c57d1bf717ec8a547ae63c709e.png](img/632e4c57d1bf717ec8a547ae63c709e.png)

## reverse-z

![image.png](img/image%205.png)

![image.png](img/image%206.png)

## 抗锯齿 — msaa

![msaa.png](img/msaa.png)

![msaa2.png](img/msaa2.png)

## 纹理插值

![双线性插值.png](img/%25E5%258F%258C%25E7%25BA%25BF%25E6%2580%25A7%25E6%258F%2592%25E5%2580%25BC.png)

![双三次插值.png](img/%25E5%258F%258C%25E4%25B8%2589%25E6%25AC%25A1%25E6%258F%2592%25E5%2580%25BC.png)

![双三次插值2.png](img/%25E5%258F%258C%25E4%25B8%2589%25E6%25AC%25A1%25E6%258F%2592%25E5%2580%25BC2.png)

## mipmap

![mipmap生成.png](img/mipmap%25E7%2594%259F%25E6%2588%2590.png)

## level 可视化

![mipmap level 可视化.png](img/mipmap_level_%25E5%258F%25AF%25E8%25A7%2586%25E5%258C%2596.png)

## 效果

![mipmap.png](img/mipmap.png)





## 主要参考

https://github.com/skywind3000/RenderHelp

https://bbs.huaweicloud.com/blogs/358776

https://github.com/ssloy/tinyrenderer