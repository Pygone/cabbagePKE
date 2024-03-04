//
// Created by Pygon on 24-3-4.
//
#include "user_lib.h"
#define N 5
#define BASE 5

int main()
{
    char path1[30] = "/bin/alloc";
    int pid = fork();
    if (pid == 0)
    {
        test(path1);
    }
    else
    {
        yield();

        void *p[N];
        for (int i = 0; i < N; i++)
        {
            p[i] = naive_malloc();
            int *pi = p[i];
            *pi = BASE + i;
            printu(">>> user alloc 1 @ vaddr 0x%x\n", p[i]);
        }

        for (int i = 0; i < N; i++)
        {
            int *pi = p[i];
            printu(">>> user 1: %d\n", *pi);
            naive_free(p[i]);
        }
        wait(pid);
    }
    exit(0);
}
