/*
test_DHTXXD.c
2016-02-16
Public Domain
*/

/*

REQUIRES

One or more DHT11/DHT21/DHT22/DHT33/DHT44.

TO BUILD

gcc -Wall -pthread -o DHTXXD test_DHTXXD.c DHTXXD.c -lpigpiod_if2

TO RUN

./DHTXXD -g18 # one reading from DHT connected to GPIO 17

./DHTXXD -g18 -i3 # read DHT connected to GPIO 14 every 3 seconds

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <pigpiod_if2.h>
#include <time.h>
#include "DHTXXD.h"

void fatal(char *fmt, ...)
{
   char buf[128];
   va_list ap;

   va_start(ap, fmt);
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   fprintf(stderr, "%s\n", buf);

   fflush(stderr);

   exit(EXIT_FAILURE);
}

void usage()
{
   fprintf(stderr, "\n" \
      "Usage: DHTXXD [OPTION] ...\n" \
      "   -g value, gpio, 0-31,                       default 4\n" \
      "   -i value, reading interval in seconds\n" \
      "             0=single reading,                 default 0\n" \
      "   -m value, model 0=auto, 1=DHT11, 2=other,   default auto\n" \
      "   -h string, host name,                       default NULL\n" \
      "   -p value, socket port, 1024-32000,          default 8888\n" \
      "EXAMPLE\n" \
      "DHTXXD -g11 -i5\n" \
      "   Read a DHT connected to GPIO 11 every 5 seconds.\n\n");
}

int optGPIO     = 4;
char *optHost   = NULL;
char *optPort   = NULL;
int optModel    = DHTAUTO;
int optInterval = 0;
int day = 0;
int month=0;
int year=0;
static uint64_t getNum(char *str, int *err)
{
   uint64_t val;
   char *endptr;

   *err = 0;
   val = strtoll(str, &endptr, 0);
   if (*endptr) {*err = 1; val = -1;}
   return val;
}

static void initOpts(int argc, char *argv[])
{
   int opt, err, i;

   while ((opt = getopt(argc, argv, "g:h:i:m:p:")) != -1)
   {
      switch (opt)
      {
         case 'g':
            i = getNum(optarg, &err);
            if ((i >= 0) && (i <= 31)) optGPIO = i;
            else fatal("invalid -g option (%d)", i);
            break;

         case 'h':
            optHost = malloc(sizeof(optarg)+1);
            if (optHost) strcpy(optHost, optarg);
            break;

         case 'i':
            i = getNum(optarg, &err);
            if ((i>=0) && (i<=86400)) optInterval = i;
            else fatal("invalid -i option (%d)", i);
            break;

         case 'm':
            i = getNum(optarg, &err);
            if ((i >= DHTAUTO) && (i <= DHTXX)) optModel = i;
            else fatal("invalid -m option (%d)", i);
            break;

         case 'p':
            optPort = malloc(sizeof(optarg)+1);
            if (optPort) strcpy(optPort, optarg);
            break;

        default: /* '?' */
           usage();
           exit(-1);
        }
    }
}

void cbf(DHTXXD_data_t r)
{
   time_t t = time(NULL);
   struct tm tm = *localtime(&t);
   printf("%d %.1f %.1f\n",r.status, r.temperature, r.humidity);
   gpioSetMode(21,PI_OUTPUT);
    gpioWrite(21,1);
   if(r.temperature>27) {
    gpioSetMode(21,PI_OUTPUT);
    gpioWrite(21,1);
    gpioSetMode(20,PI_OUTPUT);
    gpioWrite(20,0);
   }
   else{
    gpioSetMode(21,PI_OUTPUT);
    gpioWrite(21,0);
    gpioSetMode(20,PI_OUTPUT);
    gpioWrite(20,1);
   }
   
   FILE *file = fopen("/var/www/homify.local/data.json", "w");
      FILE *histFile = fopen("/var/www/homify.local/daylog.csv", "r");
   fseek(histFile, -24, SEEK_END);
   char buffer[3];
   fread(buffer,1,3,histFile);
   day=atoi(buffer);
   fseek(histFile, -27, SEEK_END);
   fread(buffer,1,3,histFile);
   month=atoi(buffer);
   fseek(histFile, -32, SEEK_END);
   fread(buffer,1,5,histFile);
   fclose(histFile);
   year=atoi(buffer);
   if(day==tm.tm_mday){
      FILE *dayhistory = fopen("/var/www/homify.local/daylog.csv", "a");
      fprintf(dayhistory, "%d-%02d-%02d;%02d:%02d:%02d;%.2f;%.2f\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, r.temperature, r.humidity);
      fclose(dayhistory);
      printf("\n%d %d %d\n",day,month,year);
      }
   
   else{
      float daytemp=0;
      char buff[50];
      int readedLines=0;
      FILE *calcFile=fopen("/var/www/homify.local/daylog.csv", "r");
      fseek(calcFile, 20, SEEK_SET);
      fread(buff,1,5,calcFile);
      daytemp=atof(buff);
      while(fgets(buff,50,calcFile)){
      fseek(calcFile, 20, SEEK_CUR);
      fread(buff,1,5,calcFile);
      daytemp=daytemp+atof(buff);
      readedLines++;
      }
      FILE *dayhistory = fopen("/var/www/homify.local/daylog.csv", "w");
      fprintf(dayhistory, "%d-%02d-%02d;%02d:%02d:%02d;%.2f;%.2f\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, r.temperature, r.humidity);
      fclose(dayhistory);
      printf("%f %d",daytemp/readedLines,day);
   }
   
   fprintf(file, "[{\"Temperature\":\"%.1f\",\"Humidity\":\"%.1f\"}]", r.temperature, r.humidity);
   fclose(file);
   
}

int main(int argc, char *argv[])
{
   FILE *histFile = fopen("/var/www/homify.local/daylog.csv", "r");
   fseek(histFile, -24, SEEK_END);
   char buffer[3];
   fread(buffer,1,3,histFile);
   day=atoi(buffer);
   fseek(histFile, -27, SEEK_END);
   fread(buffer,1,3,histFile);
   month=atoi(buffer);
   fseek(histFile, -32, SEEK_END);
   fread(buffer,1,5,histFile);
   fclose(histFile);
   year=atoi(buffer);
   int pi;
   DHTXXD_t *dht;
   initOpts(argc, argv);
   gpioInitialise();
   pi = pigpio_start(optHost, optPort); /* Connect to local Pi. */

   if (pi >= 0)
   {
      dht = DHTXXD(pi, optGPIO, optModel, cbf); /* Create DHTXX. */

      if (optInterval)
      {
         DHTXXD_auto_read(dht, optInterval);
         while (1) time_sleep(60);
      }
      else
      {
         DHTXXD_manual_read(dht);
      }

      DHTXXD_cancel(dht); /* Cancel DHTXX. */

      pigpio_stop(pi); /* Disconnect from local Pi. */
   }
   return 0;
}

