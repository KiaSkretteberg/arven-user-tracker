#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "dwm1001.h"
#include "web.h"

const char WIFI_NETWORK_NAME[] = "PH1";
const char WIFI_PASSWORD[] = "12345678";

int main() {
    float x = 0;
    float y = 0;
    float z = 0;

    stdio_init_all();

    dwm1001_init_communication();
    sleep_ms(2000);

    web_init(WIFI_NETWORK_NAME, WIFI_PASSWORD, "Arven", NULL, NULL, NULL);
 
    while (true) {
        char buff[300];
        sleep_ms(500);
        dwm1001_request_position();
        sprintf(buff, "/set_user_location/x/%f/y/%f/z/%f", x, y, z);
        web_request(buff);
    }
}