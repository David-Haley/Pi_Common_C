/* SPI Server for RPI
   Author    : David Haley
   Created   : 28/06/2017
   Last Edit : 14/10/2017
   Listens to the port specified (default 5000). When a block of data is
   received it is transferred to the SPI device. The reply from the SPI device
   is sent back to the the client. It will work with network addresses,
   Use the reserved name localhoast if the client and server reside on the same 
   machine.
   19/08/2017 : SPI transmission rate optionally read from command line.
      Command line port number made optional 
   16/08/2017 : Transmission rate reduced to 1 MHz
   14/10/2017 : Tramsmission rate set to 8 MHz. Test at 32 MHz using an AD7091R-2
                was successful. No useful speed increase was observed over above
                4 MHz due to GPIO overhead. 

   Usage spi_udp_server {port {rate}}
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
/* SPI related header files */
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define SPI_TX_Buffer_Length 1024

#define Server_Request_Error 1
#define SPI_Error 2
#define Reply_Error 3

typedef uint8_t Buffers [SPI_TX_Buffer_Length];

void error (const char* Error_Message, int Exit_Code);
/* Error message handler */

void End_Service (int Signal_Number);
/* Signal handler to allow server to clean up before termination */

static int Server_File_Descriptor, SPI_File_Descriptor;

int main(int argc, char *argv[])
{
  int UDP_Port = 5000; /* default port*/
  int Length_of_Request, Length_of_Reply;
  static Buffers SPI_TX_Buffer, SPI_RX_Buffer;
  /* Both SPI_TX_Buffers must be the same length, it is only possible to receive
     as many bits as were transmitted! */
  static struct spi_ioc_transfer SPI_Request;
  socklen_t Client_Address_Length;
  struct sockaddr_in Server_Address, Client_Address;
  /* SPI related values*/
  const char* SPI_Device = "/dev/spidev0.0";
  const uint8_t bits = 8;
  uint32_t speed = 7812500; /* (bit rate in Hz) default 8.0 Mhz */
  const uint16_t delay = 1;
  Client_Address_Length = sizeof(Client_Address);
  signal (SIGTERM, End_Service);
  if (argc > 1)
    {
      if (argc > 2)
	{
	  speed = atoi(argv[2]);
	}
      UDP_Port = atoi(argv[1]);
    }
  printf ("UDP SPI Server for Raspberry Pi version 20171014\n");
  printf ("Server port number : %d\nSPI frequency : %d Hz\n",
	  UDP_Port, speed);
  Server_File_Descriptor = socket(AF_INET, SOCK_DGRAM, 0);
  if (Server_File_Descriptor < 0) 
    error("ERROR opening socket", Server_Request_Error);
  SPI_File_Descriptor = open (SPI_Device, O_RDWR);
  if (SPI_File_Descriptor < 0)
    error ("Opening Spi device", SPI_Error);
  bzero ((char *) &Server_Address, sizeof (Server_Address));
  Server_Address.sin_family = AF_INET;
  Server_Address.sin_addr.s_addr = INADDR_ANY;
  Server_Address.sin_port = htons(UDP_Port);
  /* Ensure correct byte ordering independent of host archetiture */
  if (bind (Server_File_Descriptor,
	    (struct sockaddr *) &Server_Address,
	    sizeof(Server_Address)) < 0) 
    error ("ERROR on binding", Server_Request_Error);
  while (1) {
    /* serve ultil SIGTERM is received */
    Length_of_Request = recvfrom (Server_File_Descriptor,
				  SPI_TX_Buffer,
				  SPI_TX_Buffer_Length,
				  0,
				  (struct sockaddr *) &Client_Address,
				  &Client_Address_Length);
    if (Length_of_Request < 1)
      error ("ERROR reading from socket", Server_Request_Error);
    SPI_Request.tx_buf = (unsigned long) SPI_TX_Buffer;
    SPI_Request.rx_buf = (unsigned long) SPI_RX_Buffer;
    SPI_Request.len = Length_of_Request;
    SPI_Request.speed_hz = speed;
    SPI_Request.delay_usecs = delay;
    SPI_Request.bits_per_word = bits;
    if (ioctl (SPI_File_Descriptor, SPI_IOC_MESSAGE(1), &SPI_Request) < 1 )
      error ("Error in SPI transfer", SPI_Error);
    Length_of_Reply = sendto (Server_File_Descriptor,
			      SPI_RX_Buffer,
			      Length_of_Request,
			      0,
			      (struct sockaddr *) &Client_Address,
			      Client_Address_Length);
    if (Length_of_Reply != Length_of_Request)
      error ("ERROR writing reply", Reply_Error);
  } /* while (1) */
} /* main */

void error(const char *Error_Message, int Exit_Code)
/* Error message handeler */

{
  perror(Error_Message);
  exit(Exit_Code);
} /* error */

void End_Service (int Signal_Number)
/* Signal handler to allow server to clean up before termination */

{
  close (Server_File_Descriptor);
  close (SPI_File_Descriptor);
  exit (Signal_Number);
} /* End_Service */
