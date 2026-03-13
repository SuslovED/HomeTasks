#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define PORT 8080
#define SERVER "127.0.0.1"

int sock;

void handle_sigint(int sig) {
    close(sock);
    printf("\n");
    exit(0);
}

int main() {
    struct sockaddr_in addr;
    char buf[256];
    int n;
    
    signal(SIGINT, handle_sigint);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER);
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }
    
    printf("Connected to server\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        
        buf[strcspn(buf, "\n")] = '\0';
        
        if (strcmp(buf, "exit") == 0 || strcmp(buf, "\\exit") == 0) break;
        
        char temp[256];
        sprintf(temp, "%s\n", buf);
        write(sock, temp, strlen(temp));
        
        memset(buf, 0, sizeof(buf));
        n = read(sock, buf, sizeof(buf) - 1);
        if (n <= 0) break;
        
        printf("%s", buf);
    }
    
    close(sock);
    return 0;
}