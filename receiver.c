#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

static volatile sig_atomic_t bit_received = 0;
static volatile sig_atomic_t last_bit = 0;
static pid_t sender_pid = 0;

static void bit_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        last_bit = 1;
    }
    else
    {
        last_bit = 0;
    }
    bit_received = 1;

    if (sender_pid > 0)
    {
        kill(sender_pid, SIGUSR1);
    }
}

static void setup_handlers(void)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = bit_handler;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

static void wait_for_bit(const sigset_t *suspend_mask)
{
    while (!bit_received)
    {
        sigsuspend(suspend_mask);
    }
    bit_received = 0;
}

int main(void)
{
    printf("Receiver PID: %d\n", getpid());
    printf("Enter sender PID: ");
    fflush(stdout);

    if (scanf("%d", &sender_pid) != 1 || sender_pid <= 0)
    {
        fprintf(stderr, "Invalid PID.\n");
        return EXIT_FAILURE;
    }

    setup_handlers();

    sigset_t block_mask, suspend_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGUSR1);
    sigaddset(&block_mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &block_mask, NULL);

    sigemptyset(&suspend_mask);

    for (;;)
    {
        uint32_t value = 0;
        for (int i = 31; i >= 0; --i)
        {
            wait_for_bit(&suspend_mask);
            value = (value << 1) | (uint32_t)last_bit;
        }

        int32_t number = (int32_t)value;
        printf("Received number: %" PRId32 "\n", number);
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}
