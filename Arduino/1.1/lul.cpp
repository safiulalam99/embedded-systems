#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include<cstdlib>

using namespace std;

void led_ON(){
    printf("led ON \n");
}

void led_OFF(){
    printf("led OFF \n");
} 

void led_on(void)
{
    //yellow bgn
    printf("\033[30;103m]");
    printf(" ");
    fflush(stdout);
}

void led_off(void)
{
    //black bgn
    printf("\033[40m");
    printf(" ");
    fflush(stdout);
}

int main(int argc, char *argv[]){
    printf("VM traffic lights led blinking \n");
    for (int i = 0; i < 4; i++){
        led_on();

        usleep(100);

        led_off();

        usleep(100);
    }

    printf("\n Blink end \n");
    return 0;
}