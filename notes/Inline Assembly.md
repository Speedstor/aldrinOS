# Inline Assembly
> [!info]
> gcc uses ATT assembly syntax


| idea | detail |
| --- | --- |
| % | prefix for register |
| %\% | prefix for template strings |
| general format | `asm ( assembler template`<br/>`    : output operands`<br/>`    : input operands`<br/>`    : clobbered registers list`<br/>`    );` |
|  |  |


### Examples
```c
int a = 10, b;
asm ("movl %1, %%eax; \
	 movl %%eas, %0;"
	 :"=r"(b)
	 :"r"(a)
	 :"%eax"
	 );
```