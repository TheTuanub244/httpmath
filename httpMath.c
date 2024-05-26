#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
void *client_proc(int *arg);
int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind failed: ");
        return 1;
    };
    if (listen(listener, 5))
    {
        perror("listen failed: ");
        return 1;
    }
    while (1)
    {
        printf("Waiting for new client...\n");
        int client = accept(listener, NULL, NULL);
        printf("New client accepted: %d\n", client);
        pthread_t threadid;
        pthread_create(&threadid, NULL, client_proc, &client);
        pthread_detach(threadid);
    }
    close(listener);
    return 1;
}
void *client_proc(int *arg)
{
    int client = *(int *)arg;
    char buf[256];

    int ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0)
    {
        close(client);
        return;
    };
    float a;
    float b;
    float ans;
    char operation[20];
    char result[1024];
    char anss[256];
    char method[32];
    if (strncmp(buf, "GET /get", 8) == 0)
    {
        strncpy(method, "GET", 3);

        char *current = buf;
        while ((current = strchr(current, '=')) != NULL)
        {
            char index = *(current - 1);
            current++;
            char *next_ampersand = strchr(current, '&');
            if (next_ampersand == NULL)
            {
                next_ampersand = buf + strlen(buf);
            }
            int length = next_ampersand - current;
            char value[length + 1];
            strncpy(value, current, length);
            value[length] = '\0';
            if (index == 'a')
            {
                a = atof(value);
            }
            else if (index == 'b')
            {
                b = atof(value);
            }
            else if (index == 'd')
            {
                strncpy(operation, value, 3);
            }
            current = next_ampersand + 1;
        }
        if (strncmp(operation, "mul", 3) == 0)
        {
            ans = a * b;
        }
        else if (strncmp(operation, "add", 3) == 0)
        {
            ans = a + b;
        }
        else if (strncmp(operation, "sub", 3) == 0)
        {
            ans = a - b;
        }
        else if (strncmp(operation, "div", 3) == 0)
        {
            ans = a / b;
        }
    }
    else if (strncmp(buf, "POST /post", 10) == 0)
    {
        strncpy(method, "POST", 4);
        char *current = buf;
        while ((current = strchr(current, '=')) != NULL)
        {
            char index = *(current - 1);
            current++;
            char *next_ampersand = strchr(current, '&');
            if (next_ampersand == NULL)
            {
                next_ampersand = buf + strlen(buf);
            }
            int length = next_ampersand - current;
            char value[length + 1];
            strncpy(value, current, length);
            value[length] = '\0';
            printf("%s\n", operation);
            printf("current: %c\n", index);
            if (index == 'a')
            {
                a = atof(value);
            }
            else if (index == 'b')
            {
                b = atof(value);
            }
            else if (index == 'd')
            {
                strncpy(operation, value, 3);
            }
            current = next_ampersand + 1;
        }
        if (strncmp(operation, "mul", 3) == 0)
        {
            ans = a * b;
        }
        else if (strncmp(operation, "add", 3) == 0)
        {
            ans = a + b;
        }
        else if (strncmp(operation, "sub", 3) == 0)
        {
            ans = a - b;
        }
        else if (strncmp(operation, "div", 3) == 0)
        {
            ans = a / b;
        }
    }
    snprintf(result, sizeof(result), "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n", strlen(result));

    send(client, result, strlen(result), 0);

    sprintf(anss, "\n<html><body><h1>%s: %.2f %s %.2f = %.2f</h1></body></html>", method, a, operation, b, ans);

    send(client, anss, strlen(anss), 0);

    close(client);
    pthread_exit(NULL);
    return;
}