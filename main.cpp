// The libraries below can be find into the mbed website

#include "mbed.h"
#include "SDFileSystem.h"
#include "DHT.h"
#include "DS1302.h"

// Defines for the DS1302 timer module
#define SCLK    PTC1
#define IO      PTB19
#define CE      PTB18

SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd"); //MOSI, MISO, SCLK, SSEL. (SD Card) Tested on K64F, correct pins.
AnalogIn LM35(PTB2);
AnalogIn LDR(PTB3);
int count=0;
float ldrcalc= 0.00;
DigitalOut RedLed(LED1); // Error indication
DigitalOut GreenLed(LED2); // LM35 read indication
DigitalOut Blueled(LED3); // DHT11 read indication
DHT sensor(PTB20,DHT11); // Use the DHT11 sensor, on pin 
DS1302 clk(SCLK, IO, PTB18); // ports for the DS1302 time keeper
 
int main() {
    // the 6 lines below are for the time keeper chip
    #ifdef INITIAL_RUN
    clk.set_time(1403949720);
    #endif
    
    char storedByte = clk.recallByte(0);
    //printf("\r\nStored byte was %d, now increasing by one\r\n", storedByte);
    clk.storeByte(0, storedByte + 1);
    //
    
    int err;
    wait(1); // wait 1 second for DHT11 to stabilyze
    
    //printf("Hello World!\n");   
 
    mkdir("/sd/dados", 0777);
    
    FILE *fp = fopen("/sd/dados/data001.csv", "w");
    if(fp == NULL) {
        error("Could not open file for write\n");
        RedLed= 0;
        
        }
    while (1) {
    
    Blueled= 1;
    RedLed= 1;
    GreenLed= 1;
    
   if (count < 12000000){ //around 10 seconds before it reaches the count of 12000000
        count++;
        } else{  
        // --------------------------
       
        err = sensor.readData();
        if (err == 0) {
            GreenLed= 0;
            printf("Temperature is %4.2f \r\n",sensor.ReadTemperature(CELCIUS));
            //printf("Temperature is %4.2f F \r\n",sensor.ReadTemperature(FARENHEIT));
            //printf("Temperature is %4.2f K \r\n",sensor.ReadTemperature(KELVIN));
            printf("Humidity is %4.2f \r\n",sensor.ReadHumidity());
            //printf("Dew point is %4.2f  \r\n",sensor.CalcdewPoint(sensor.ReadTemperature(CELCIUS), sensor.ReadHumidity()));
            //printf("Dew point (fast) is %4.2f  \r\n",sensor.CalcdewPointFast(sensor.ReadTemperature(CELCIUS), sensor.ReadHumidity()));
            
        } else
            printf("\r\nErr %i \n",err);
            
        // ----------------------------  
    FILE *fp = fopen("/sd/dados/data001.csv", "a");
    if(fp == NULL) {
        error("Could not open file for write\n");
        
       }
    
    ldrcalc= LDR.read();
    ldrcalc= (1/ldrcalc)-1; //Transforms the LDR value into a 0-5 signal (integer)
    time_t seconds = clk.time(NULL);
    //fprintf(fp, "%s\r,%f,%f,%f\n", "LM35", "DHT11", "Humid", "Month", "Day", "Hour", "Year");
    fprintf(fp, "%s\r,%f,%f,%f,%f", ctime(&seconds), 333.333*LM35.read(), sensor.ReadTemperature(CELCIUS), sensor.ReadHumidity(), ldrcalc); 
    fclose(fp);  
    //printf("Goodbye World!\n");
    count=0;
    Blueled= 0;
    }
    }
    }