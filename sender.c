#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

static volatile sig_atomic_t ack_received = 0;
static pid_t receiver_pid = 0;

static void ack_handler(int signo)
{
    (void)signo;
    ack_received = 1;
}

static void setup_handlers(void)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = ack_handler;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

static void wait_for_ack(const sigset_t *suspend_mask)
{
    while (!ack_received)
    {
        sigsuspend(suspend_mask);
    }
    ack_received = 0;
}

static void send_bit(int bit, const sigset_t *suspend_mask)
{
    int sig = bit ? SIGUSR1 : SIGUSR2;
    if (kill(receiver_pid, sig) == -1)
    {
        perror("Failed to send signal");
        exit(EXIT_FAILURE);
    }
    wait_for_ack(suspend_mask);
}

int main(void)
{
    printf("Sender PID: %d\n", getpid());
    printf("Enter receiver PID: ");
    fflush(stdout);

    if (scanf("%d", &receiver_pid) != 1 || receiver_pid <= 0)
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
        int32_t number = 0;
        printf("Enter integer to send: ");
        fflush(stdout);

        if (scanf("%" SCNd32, &number) != 1)
        {
            fprintf(stderr, "Failed to read number.\n");
            break;
        }

        uint32_t value = (uint32_t)number;
        for (int i = 31; i >= 0; --i)
        {
            int bit = (value >> i) & 1U;
            send_bit(bit, &suspend_mask);
        }

        printf("Number %" PRId32 " sent. Continue? (y/n): ", number);
        fflush(stdout);

        char answer = 'n';
        if (scanf(" %c", &answer) != 1 || (answer != 'y' && answer != 'Y'))
        {
            break;
        }
    }

    printf("Sender finished.\n");
    return EXIT_SUCCESS;
}
