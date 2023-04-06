#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "dwm1001.h"
#include "web.h"

const char WIFI_NETWORK_NAME[] = "PH1";
const char WIFI_PASSWORD[] = "12345678";

int main() {
    // struct DWM1001_Position position;
    // position.x = 0;
    // position.y = 0;
    // position.z = 0;

    stdio_init_all();

    dwm1001_init_communication();
    sleep_ms(2000);

    web_init(WIFI_NETWORK_NAME, WIFI_PASSWORD, "Arven", NULL, NULL, NULL);
 
    while (true) {
        char buff[300];
    char coordBuff[200];
        sleep_ms(500);
        struct DWM1001_Position position = dwm1001_request_position();
        sprintf(buff, "\nmain: /set_user_location/x/%d/y/%d/z/%d", position.x, position.y, position.z);
        printf(buff);
        //web_request(buff);
    }
}