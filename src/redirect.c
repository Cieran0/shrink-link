#define _GNU_SOURCE
#include "redirect.h"
#include "stdio.h"


char* generate_redirect(const char* location) {
    if(location == NULL){
        return NULL;
    }
    
    char* redirect;

    asprintf(&redirect, "HTTP/1.1 301 Moved Permanently\r\nLocation: %s\r\nContent-Length: 0\r\nConnection: close\r\n\r\n", location);

    return redirect;
}
#undef _GNU_SOURCE