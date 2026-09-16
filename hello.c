// hello.c —— 不使用任何 C 标准库，直接调用 Linux 系统调用(Unuse any C standard library, directly call Linux system calls)
// 编译：gcc -static -nostdlib -o hello hello.c(compile: gcc -static -nostdlib -o hello hello.c)

// 系统调用号（x86_64 Linux）[syscall numbers for x86_64 Linux]
#define SYS_write 1
#define SYS_exit  60

// 标准输出文件描述符(standard output file descriptor)
#define STDOUT_FILENO 1

// 通用系统调用封装：把 syscall 指令包成 C 函数(General system call wrapper: wrap the syscall instruction into a C function)
static long sys_call3(long n, long a, long b, long c)
{
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)                    // 输出：rax 存返回值(output: rax stores the return value)
        : "a"(n), "D"(a), "S"(b), "d"(c) // 输入：rax=号, rdi/rsi/rdx=参数(input: rax=number, rdi/rsi/rdx=parameters)
        : "rcx", "r11", "memory"       // 被破坏的寄存器(clobbered registers)
    );
    return ret;
}

static long sys_write(int fd, const void *buf, unsigned long len)
{
    return sys_call3(SYS_write, fd, (long)buf, len);
}

static void sys_exit(int code)
{
    sys_call3(SYS_exit, code, 0, 0);
    __builtin_unreachable(); // 告诉编译器这里不会返回(tell the compiler this point will not return)
}

// 关键：不链接 C 库时，入口不是 main，而是 _start(key point: when not linking C library, the entry point is not main, but _start)
void _start(void)
{
    const char msg[] = "Hello, syscall!\n";
    // 编译器会把 sizeof(msg)-1 在编译期算出，无需 strlen(The compiler will calculate sizeof(msg)-1 at compile time, no need for strlen()
    sys_write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    sys_exit(0);
}
