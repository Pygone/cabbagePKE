# challenge X  

## 指令介绍  

`make build`: 构建  
`make run`: 运行shell  
`make test`: 测试各项单独的challenge  
`make clean`: 清理  
`make run-mul`: 测试多核启动  

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

- `make run-mul`: 运行lab-2-3的测试样例  
  - `lab 2`
    - `challenge 3`

- `make run`: 运行shell
  - `lab 3`
    - `challenge 1` wait 用来顺序执行exec
  - `lab 4`
    - `challenge 2` exec 嵌入shell
    - `challenge 3` shell 实现  
