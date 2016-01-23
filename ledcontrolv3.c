#include <pthread.h>
#include <pigpio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lirc/lirc_client.h>
#include <time.h>
#include <unistd.h>

#include <wiringPi.h>

void sLED (int red,int blue,int green,int mode);


#define RED 21
#define BLUE 16
#define GREEN 20

int  MODE;

/* This is our thread function.  It is like main(), but for a thread*/
void *threadFunc(void *arg)
{
    struct lirc_config *config;
    int buttonTimer = millis();

    char *code;
    char *c;

    //Initiate LIRC. Exit on failure
    if(lirc_init("lirc",1)==-1)
        exit(EXIT_FAILURE);

    //Read the default LIRC config at /etc/lirc/lircd.conf This is the config for your remot$
    if(lirc_readconfig(NULL,&config,NULL)==0)
    {
        //Do stuff while LIRC socket is open 0=open -1=closed.
        while(lirc_nextcode(&code)==0)
        {            
	    //If code = NULL, meaning nothing was returned from LIRC socket,
	    //then skip lines below and start while loop again.
            if(code==NULL) continue;{
                if (millis() - buttonTimer  > 400){ 
		    //Check to see if the string "KEY_#" appears anywhere within the string $
                    if(strstr (code,"KEY_F")){
                        printf("MATCH on KEY_F\n");
                        sLED(0,200,0,0);
			buttonTimer = millis();
                    }
                    else if(strstr (code,"KEY_G")){
                        printf("MATCH on KEY_G\n");
			MODE = 2;
			buttonTimer = millis();
                    }
                    else if(strstr (code,"KEY_H")){
                        printf("MATCH on KEY_H\n");
			MODE = 1;
			buttonTimer = millis();
                    }
		    else if(strstr (code,"KEY_E")){
			printf("MATCH on KEY_E\n");
			sLED(0,0,100,0);
			buttonTimer = millis();
		    }
		    else if(strstr (code,"KEY_A")){
                        printf("MATCH on KEY_A\n");
			sLED(0,0,0,0);
			buttonTimer = millis();
		    }
		    
                }
            }
            //Need to free up code before the next loop
            free(code);
        }
        //Frees the data structures associated with config.
        lirc_freeconfig(config);
    }
    //lirc_deinit() closes the connection to lircd and does some internal clean-up stuff.
    lirc_deinit();
    //exit(EXIT_SUCCESS);
    return NULL;
}

int main(void)
{
 //initialise pigio and pins
      if (gpioInitialise() < 0 )
        {
                printf("pigpio initialise failed\n");
                return 1;
        }

        gpioSetMode(BLUE, PI_OUTPUT);
        gpioSetMode(RED, PI_OUTPUT);
        gpioSetMode(GREEN, PI_OUTPUT);
        gpioSetPWMrange(BLUE, 200);
        gpioSetPWMrange(RED, 200);
        gpioSetPWMrange(GREEN, 200);


        pthread_t pth;  // this is our thread identifier

        pthread_create(&pth,NULL,threadFunc,NULL);

	int i;

	while(1){
	   if(MODE != 0){
		for (i=0;i<201;i++){
		    if (MODE == 1){
			sLED(0,0,i,1);
			usleep(20000);
		    }
		    else if(MODE == 2){
			sLED(i,i/2,i/4,1);
			usleep(20000);
		    }
		    else if(MODE == 0){
			break;
		    }
		}
		sleep(1);
		for(i=200;i>=0;i--){
		    if(MODE == 1){
			sLED(0,0,i,1);
			usleep(20000);
		    }
		    else if(MODE == 2){
                        sLED(i,i/2,i/4,1);
                        usleep(20000);
                    }
		    else if(MODE == 0){
			break;
		    }
		}
            }
	}
        pthread_join(pth,NULL);

        return 1;
}

void sLED (int red,int blue,int green,int mode)
{
	if(mode == 1){
		gpioPWM(RED,red);
		gpioPWM(BLUE,blue);
		gpioPWM(GREEN,green);
	}
	else if(mode == 0){
		MODE = 0;
		gpioPWM(RED,red);
                gpioPWM(BLUE,blue);
                gpioPWM(GREEN,green);
	}
}

