#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_SPEED 1000000

#define SINGLE_CHANNEL 1
#define DIFFERENTIAL_CHANNEL 0


#define VREF 3.3

///  SPI STUFF

int openSPI(char *spidev)
{
    int spi_handle;
    unsigned char spi_mode=0;
    unsigned char spi_bitsPerWord=8;
    unsigned long  _spi_speed =SPI_SPEED;

    spi_handle = open(spidev,O_RDWR);

    if(spi_handle < 0)
         {
           perror("Error - Could not open SPI device");
           return -1;
         }

     ioctl(spi_handle, SPI_IOC_WR_MODE, &spi_mode);
     ioctl(spi_handle, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
     ioctl(spi_handle, SPI_IOC_WR_MAX_SPEED_HZ, &_spi_speed);

    return(spi_handle);
}



int my_spi_transfer(int spi_handle,unsigned char *txbuff, unsigned char *rxbuff, int len)
{
  int ret;
int loop;

//  printf("TX ");
//  for(loop=0;loop<len;loop++)
//  printf("%02X ",txbuff[loop]);

  struct spi_ioc_transfer spi_tr = {
                .tx_buf = (unsigned long)  txbuff,
                .rx_buf = (unsigned long)  rxbuff,
                .len = len,
                .delay_usecs = 0,
                .speed_hz = SPI_SPEED,
                .bits_per_word = 8,
              };
        ret = ioctl(spi_handle,SPI_IOC_MESSAGE(1), &spi_tr);
        if(ret < 1)
         {
          perror("Can't send spi message\n");
          return 0;
        }

//printf("\nRX ");
//for(loop=0;loop<len;loop++)
//  printf("%02X ",rxbuff[loop]);
//printf("\n");


 return 1;
}



// MCP3208 FUNCTION
// read_adc(spi_handle,singleDiff,channel)
// spi_handle  handle of the SPI
// singleDIFF  !0= single  0=Differential
// channel     channel 0 to 7 to read
//
// return  -1 if error otherwise the A2D value between 0 and 1023

int read_adc(int spi_handle,int singleDiff,int channel)
{
   unsigned char data[3];
    short value;

   if(spi_handle <0) return -1; // is spi handle valid
   if((channel <0) || (channel > 7)) return -1; // if channel number valid

   data[0]=4 | (singleDiff ? 2 : 0) | ( (channel>>2) &1);  // start bit + sig/diff + D2
   data[1]=  (channel & 3)<<6;
   data[2]=0;
   if(my_spi_transfer(spi_handle,data,data,3))
     {
       value = (((unsigned short) data[1])<<8) & 0xf00;
       value |= data[2];
       return value;
     }
 return -1;
}



char * spidevName = "/dev/spidev0.0";

int main(void)
{
  int channel;



  int spiHandle = openSPI(spidevName);

  if(spiHandle <0)
    {
      printf("unable to open spi device %s.\n", spidevName);
      return(-1);
    }


   for(channel=0;channel<8;channel++)
     {
       int value =read_adc(spiHandle,SINGLE_CHANNEL,channel);
       printf("Channel %d: ",channel);
       if(value < 0)
          printf("Error\n");
       else
          printf("%4d  %1.03fV\n",value, VREF * value / 4095.0);

     }

  close(spiHandle);
  return 0;
}
