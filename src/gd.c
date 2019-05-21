/*
HTTP protocol -- web data
TCP -- guarantees data integrity
IP -- deals with routing and delivery
Ethernet -- deals with sending data on the LAN
*/

/*
1. start listening for conenctions
2. when a new connection arrives receive the request data.
3. parse the request data
4. build response data 
5. send response data
6. close the connection and wait for the next one.

*/

//sscanf();

// #include <stdio.h>

/*
GET /foobar HTTP/1.1
Host: www.example.com
Connection: close
X-Header: whatever
*/

/* int main(void){
    char *s = "GET /foobar HTTP/1.1\nHost: www.example.com\nConnection:close\nX-Header: whatever";

    char method[200];
    char path[8192];

    sscanf(s, "%s %s", method, path);

    printf("method: \"%s\"\n", method);
    printf("path: \"%s\"\n", path);
} */

// #include <stdio.h>
// #include <string.h>

// int main(void)
// {
//     char response[500000];

//     //body
//     char *body = "<h1>Hello, world!</h1>";

//     //compute the length of that string
//     int length = strlen(body);

//     //build response

//     sprintf(response, "HTTP/1.1 200 OK\n"
//                     "Content-Type: text/html\n"
//                     "Content-Length: %d\n"
//                     "Connection: close\n"
//                     "\n"
//                     "%s",
//                     length, body);

//     printf("%s", response); //send() 
// }