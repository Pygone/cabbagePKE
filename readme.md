# challenge X  

## 框架介绍

使用cmake 重构了框架, 使得clangd 可以正常工作, 并且可以使用makefile 进行编译和测试.

优化了用户添加测试程序的需求, 避免了冗余的makefile 编写.

目前尚未添加对应的debug 指令, 请参照pke-doc 进行, 更推荐使用sprint debug

## 指令介绍  

`make build`: 构建  
`make run`: 运行shell  
`make test`: 测试各项单独的challenge  
`make clean`: 清理  

## 展现challenge 方式

- `make test`: 输出测试结果在output目录  
  - `lab 1`
    - `challenge 1`
    - `challenge 2`
  - `lab 2`
    - `challenge 1`
    - `challenge 2`
  - `lab 3`
    - `challenge 2`
    - `challenge 3`
  - `lab 4`
    - `challenge 1`

- `make run`: 运行shell
  - `lab 3`
    - `challenge 1` wait 用来顺序执行exec
  - `lab 4`
    - `challenge 2` exec 嵌入shell
    - `challenge 3` shell 实现  
  - `lab 2`  
    - `challenge 3` test 程序即测试样例
