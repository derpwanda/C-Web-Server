/**
 * webserver.c -- A webserver written in C
 * 
 * Test with curl (if you don't have it, install it):
 * 
 *    curl -D - http://localhost:3490/
 *    curl -D - http://localhost:3490/d20
 *    curl -D - http://localhost:3490/date
 * 
 * You can also test the above URLs in your browser! They should work!
 * 
 * Posting Data:
 * 
 *    curl -D - -X POST -H 'Content-Type: text/plain' -d 'Hello, sample data!' http://localhost:3490/save
 * 
 * (Posting data is harder to test from a browser.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>
#include "net.h"
#include "file.h"
#include "mime.h"
#include "cache.h"

#define PORT "3490"  // the port users will be connecting to

#define SERVER_FILES "./serverfiles"
#define SERVER_ROOT "./serverroot"

/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 * 
 * Return the value from the send() function.
 */
int send_response(int fd, char *header, char *content_type, void *body, int content_length)
{
    const int max_response_size = 262144;
    char response[max_response_size];
    char *response_body = body;
    // Build HTTP response and store it in response

    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    time_t t = time(NULL);
    
    struct tm *local_time = localtime(&t);
    //the string returned by localtime already has a \n

    char *timestamp = asctime(local_time); 
    //asctime converts tm struct to a timestamp

    int response_length = sprintf(
                response, 
                "%s\n"
                "Connection: close\n"
                "Content-Length: %d\n"
                "Content-Type: %s\n"
                "Date: %s\n",
                header, 
                content_length, 
                content_type, 
                timestamp);
    
    //make the body interpretation agnostic
    memcpy(response + response_length, response_body, content_length);
    response_length += content_length;

    // printf("response_length: %d\n", response_length);
    // printf("RESPONSE: %s\n", response);

    // Send it all!
    int rv = send(fd, response, response_length, 0);

    if (rv < 0) {
        perror("send");
    }

    return rv;

}


/**
 * Send a /d20 endpoint response
 */
void get_d20(int fd)
{
    // Generate a random number between 1 and 20 inclusive
    
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    char number[16];
    // printf("INSIDE GET_D20\n");
    int random_num = rand() % 20 + 1;
    // printf("%d\n", random_num);
    sprintf(number, "%d\n", random_num);
    // Use send_response() to send it back as text/plain data

    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    send_response(fd, "HTTP/1.1 200 OK", "text/plain", number, sizeof(number));

    // file_free(number);

    //stop warnings
    // (void)fd;
}

/**
 * Send a 404 response
 */
void resp_404(int fd)
{
    char filepath[4096];
    struct file_data *filedata; 
    char *mime_type;

    // Fetch the 404.html file
    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_ROOT);
    filedata = file_load(filepath);

    if (filedata == NULL) {
        // TODO: make this non-fatal
        fprintf(stderr, "cannot find system 404 file\n");
        exit(3);
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 404 NOT FOUND", mime_type, filedata->data, filedata->size);

    file_free(filedata);
}

/**
 * Read and return a file from disk or cache
 */
void get_file(int fd, struct cache *cache, char *request_path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    char filepath[4096];
    struct file_data *filedata; 
    char *mime_type;


    if (strcmp(request_path, "/index.html") == 0)
    {
        sprintf(filepath, "%s/index.html", SERVER_ROOT);
    }else
    {
        sprintf(filepath, "%s%s", SERVER_ROOT, request_path);
    }
    
    
    filedata = file_load(filepath);
    if (filedata == NULL) {
        resp_404(fd);
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 200 OK", mime_type, filedata->data, filedata->size);

    file_free(filedata);

    // (void)fd; 
    (void)cache;
    // (void)request_path;
}

/**
 * Search for the end of the HTTP header
 * 
 * "Newlines" in HTTP can be \r\n (carriage return followed by newline) or \n
 * (newline) or \r (carriage return).
 */
char *find_start_of_body(char *header)
{
    ///////////////////
    // IMPLEMENT ME! // (Stretch)
    ///////////////////
    (void)header;
    return NULL; //stops control reaches end of non-void function
}

/**
 * Handle HTTP request and send response
 */
void handle_http_request(int fd, struct cache *cache)
{
    const int request_buffer_size = 65536; // 64K
    char request[request_buffer_size];

    char method[64];
    char path[64];
    char http[64];

    // Read request
    int bytes_recvd = recv(fd, request, request_buffer_size - 1, 0);

    if (bytes_recvd < 0) {
        perror("recv");
        return;
    }


    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    // Read the first two components of the first line of the request 
    sscanf(request, "%s %s %s", method, path, http);

    // printf("method: \"%s\"\n", method);
    // printf("path: \"%s\"\n", path);
    // printf("http: \"%s\"\n", http);
 
    // If GET, handle the get endpoints
    if (strcmp(method, "GET") == 0)
    {
    //    Check if it's /d20 and handle that special case
        if(strcmp(path, "/d20") == 0)
        {
            get_d20(fd);
        } else
    //    Otherwise serve the requested file by calling get_file()
        {
            get_file(fd, cache, path);
        }
        
        resp_404(fd); //newfd or listenfd, an int            
    }



    //stop warnings
    // (void)cache;


    // (Stretch) If POST, handle the post request
}

/**
 * Main
 */
int main(void)
{
    int newfd;  // listen on sock_fd, new connection on newfd
    struct sockaddr_storage their_addr; // connector's address information
    char s[INET6_ADDRSTRLEN];

    struct cache *cache = cache_create(10, 0);

    // Get a listening socket
    int listenfd = get_listener_socket(PORT);

    if (listenfd < 0) {
        fprintf(stderr, "webserver: fatal error getting listening socket\n");
        exit(1);
    }

    printf("webserver: waiting for connections on port %s...\n", PORT);


    // This is the main loop that accepts incoming connections and
    // responds to the request. The main parent process
    // then goes back to waiting for new connections.
    
    while(1) {
        socklen_t sin_size = sizeof their_addr;

        // Parent process will block on the accept() call until someone
        // makes a new connection:
        newfd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
            continue;
        }

        

        // Print out a message that we got the connection
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        // newfd is a new socket descriptor for the new connection.
        // listenfd is still listening for new connections.

        handle_http_request(newfd, cache);

        close(newfd);
    }

    // Unreachable code

    return 0;
}

