#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

typedef void (*socket_handler)(int port);

void delay() {
    for (int i = 0; i < 10; i++)
        sleep(1);
}

// Create and handle tcp socket connections
void tcp_server(int port) {
    printf("TCP: %d\n", port);

    // Just a delay to simulate a server
    delay();

    exit(0);
}

// Create and handle udp socket connections
void udp_server(int port) {
    printf("UDP: %d\n", port);

    // Just a delay to simulate a server
    delay();

    exit(0);
}

int start_process(int port, socket_handler handler) {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("fork()");
        return -1;
    }

    if (pid == 0) {
        // Starting handler
        handler(port); 
    } 

    return pid;
}

int main(int argc, char *argv[]) {
    pid_t tcp_pid = start_process(2222, tcp_server);
    pid_t udp_pid = start_process(5555, udp_server);
    
    printf("Process TCP pid: %d has started\n", tcp_pid);
    printf("Process UDP pid: %d has started\n", udp_pid);

    // Simple way to finish both processes
    do {
        int status;
        pid_t finished_pid = waitpid(-1, &status, 0);

        printf("Process %d finished\n", finished_pid);

        if (finished_pid == tcp_pid)
            tcp_pid = -1;
        if (finished_pid == udp_pid)
            udp_pid = -1;

    } while (tcp_pid != -1 || udp_pid != -1);

    return 0;
}
