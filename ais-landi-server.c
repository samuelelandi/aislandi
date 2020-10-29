/**
** AIS SERVER FOR RASPBERRY PI BOARD + AIS RECIEVER (https://shop.wegmatt.com/products/daisy-2-dual-channel-ais-receiver-with-nmea-0183
** it listens on port tcp/8001 for location request of nearby vessels 
**/
#include <stdio.h>   /* Standard library definition */
#include <stdlib.h>  /* Standard library definition */
#include <string.h>  /* Standard library definition */
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */
#include <pthread.h> /* threads definitions		   */
#include <sys/socket.h> /* socket definitions		   */ 
#include <sys/types.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <syslog.h>
#include "portable.h"
#include "nmea.h"
#include "sixbit.h"
#include "vdm_parse.h"

// FUNCTION DEFINITION
void uma_exit_error(char *);
int uma_open_rs232(char *);
void uma_close_rs232(int);
void * uma_read_rs232(void *);
void uma_write_log(char *);
int decode_ais_gps(char *,long *,double *,double *);

//*** CUSTOMIZATION PARAMETERS
#define RS232 "/dev/ttyACM0"
//*** GLOBAL VARIABLES
char umaais[100][128];
/* main function */
void main(void){
    int rs232=0;
    pthread_t t;
    long nt=0;
    int rc=0;
    int i;
    struct sockaddr_in servaddr, cli; 
    int sockfd,connfd,len;
    char clientip[256];
    char aismsg[128];
    char buf[512];
    long mmsi;
    double lat_dd,long_dd;
    int j;
    long mmsr[100];
    for(i=0;i<100;i++){
        memset(umaais[i],0x0,128);
    }
    //OPEN SERIAL LINE
    rs232=uma_open_rs232(RS232); 
    // LAUNCH THREAD TO READ SERIAL LINE WITH AIS POSITIONS IN GLOBAL VAR umaais
    rc=pthread_create(&t,NULL,uma_read_rs232,(void *) rs232);    
    // SERVER SOCKET PORT 8001 LISTENING FOR REQUEST
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  /** SOCKET CREATION 	*/    
    if (sockfd == -1) uma_exit_error("2003 - Error creating socket");
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(8001);
    //bzero(&(servaddr.sin_zero),8); 
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) uma_exit_error("2004 - Error binding socket");
    if ((listen(sockfd, 100)) != 0) uma_exit_error("2005 - Error listening to socket");
    while(1){
          len=sizeof(cli);
          printf("uma-ais-server.c: Server Listening on port 8001\n");
          connfd = accept(sockfd, (struct sockaddr *)&cli, &len);     
          if(connfd<0) uma_exit_error("2006 - Error accepting connection to socket"); 
          strncpy(clientip, inet_ntoa(cli.sin_addr),256);  
          printf("uma-ais-server.c: connection from: %s \n",clientip);
          for(i=0;i<100;i++) 
              mmsr[i]=0;
          //*SENDING AIS DATA LOOP
          for(i=0;i<100;i++)
          {
            strncpy(aismsg,umaais[i],128);
            mmsi=0L;
            lat_dd=0;
            long_dd=0;
            if(decode_ais_gps(aismsg,&mmsi,&lat_dd,&long_dd)==0){
              memset(buf,0x0,512);
              snprintf(buf,512,"{\"mmsi\":\"%09ld\",\"latitude\":\"%0.6f\",\"longitude\":\"%0.6f\"}\n", mmsi,lat_dd,long_dd);
              for(j=0;j<100;j++){
                  if(mmsr[j]==mmsi)
                      break;
              }
              if(j==100){
                  write(connfd, buf, strlen(buf));
                  puts(buf);
              }
              mmsr[i]=mmsi;
            }
          }
          //END SENDING DATA
          shutdown(connfd,SHUT_RDWR);
          close(connfd);
    }
    //CLOSE SERIAL LINE
    uma_close_rs232(rs232);
    exit(0);
}
/**
** FUNCTION TO READ DATA
**/
void * uma_read_rs232(void *fd){
    char buffer[512];
    int lenbuf=0;
    int rs232=(int)fd;
    int i;
    while(1){
        memset(buffer,0,512);
        lenbuf=read(rs232,buffer,82);
        //scroll down umaais array of one row
        for(i=98;i>=0;i--){
          memcpy(umaais[i+1],umaais[i],128);
        }
        memset(umaais[0],0x0,128);
        strncpy(umaais[0],buffer,128);   
        uma_write_log(buffer);
    }

}

/**
** FUNCTION TO OPEN SERIAL LINE RS-232
**/
int uma_open_rs232(char * dev){
    int fd=0;
    fd = open(dev,O_RDWR | O_NOCTTY);
    if(fd==-1)
         uma_exit_error("2000 - Error opening serial line");
    struct termios SerialPortSettings; 	/* Create the structure                          */
    tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */
    /* Setting the Baud rate */
    cfsetispeed(&SerialPortSettings,B38400); /* Set Read  Speed as 38400                       */
    cfsetospeed(&SerialPortSettings,B38400); /* Set Write Speed as 38400                       */
    /* 8N1 Mode */
    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
    SerialPortSettings.c_cflag &= ~CRTSCTS;  /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */
    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/
    /* Setting Time outs */
    SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
    SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */
    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/{
         close(fd);
         uma_exit_error("2002 - Error setting rs232 parameters");
    }
    tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer            */
    return(fd);
}
/**
** FUNCTION TO CLOSE SERIAL LINE RS-232
**/
void uma_close_rs232(int fd){
    if(close(fd)==-1)
        uma_exit_error("2010 - Error closing serial line");
    return;
}
/**
** FUNCTION TO PRINT/LOG THE ERROR AND EXIT 
**/
void uma_exit_error(char * error){
    puts(error);
    if(errno!=0) printf("errno: %d - %s",errno,strerror(errno));
    
    exit(-1);
}	
/**
** FUNCTION TO WRITE TO LOG
**/
void uma_write_log(char *msg){
openlog("uma-ais-server",LOG_USER,LOG_PID);
syslog(LOG_INFO,msg);
closelog();
}
/** 
* FUNCTION TO DECODE AIS MESSAGE AND GET BACK MMSI (VESSEL ID) WITH COORDINATES
**/
int decode_ais_gps(char * aismsg,long *mmsir,double *lat_ddr,double *long_ddr){
    ais_state     ais;
    long mmsi=0;
    double lat_dd=0,long_dd=0;
    /* AIS message structures, only parse ones with positions */
    aismsg_1  msg_1;
    aismsg_2  msg_2;
    aismsg_3  msg_3;
    aismsg_4  msg_4;
    aismsg_9  msg_9;
    aismsg_18 msg_18;
    aismsg_19 msg_19;
    //printf("aismsg: %s\n",aismsg);    
    /* Clear out the structures */
    memset( &ais, 0, sizeof( ais_state ) );

    if (assemble_vdm( &ais, aismsg ) == 0)
    {
            /* Get the 6 bit message id */
            ais.msgid = (unsigned char) get_6bit( &ais.six_state, 6 );
            
            /* process message with appropriate parser */
            switch( ais.msgid ) {
                case 1:
                    if( parse_ais_1( &ais, &msg_1 ) == 0 )
                    {
                        mmsi = msg_1.userid;
                        pos2ddd( msg_1.latitude, msg_1.longitude, &lat_dd, &long_dd );
                    }
                    break;
                            
                case 2:
                    if( parse_ais_2( &ais, &msg_2 ) == 0 )
                    {
                        mmsi = msg_2.userid;
                        pos2ddd( msg_2.latitude, msg_2.longitude, &lat_dd, &long_dd );
                    }
                    break;
                            
                case 3:
                    if( parse_ais_3( &ais, &msg_3 ) == 0 )
                    {
                        mmsi = msg_3.userid;
                        pos2ddd( msg_3.latitude, msg_3.longitude, &lat_dd, &long_dd );
                    }
                    break;
                            
                case 4:
                    if( parse_ais_4( &ais, &msg_4 ) == 0 )
                    {
                        mmsi = msg_4.userid;
                        pos2ddd( msg_4.latitude, msg_4.longitude, &lat_dd, &long_dd );
                    }
                    break;
                            
                case 9:
                    if( parse_ais_9( &ais, &msg_9 ) == 0 )
                    {
                        mmsi = msg_9.userid;
                        pos2ddd( msg_9.latitude, msg_9.longitude, &lat_dd, &long_dd );
                    }
                    break;
                    
                case 18:
                    if( parse_ais_18( &ais, &msg_18 ) == 0 )
                    {
                        mmsi = msg_18.userid;
                        pos2ddd( msg_18.latitude, msg_18.longitude, &lat_dd, &long_dd );
                    }
                    break;
                    

                case 19:
                    if( parse_ais_19( &ais, &msg_19 ) == 0 )
                    {
                        mmsi = msg_19.userid;
                        pos2ddd( msg_19.latitude, msg_19.longitude, &lat_dd, &long_dd );
                    }
                    break;
                    
            }  /* switch msgid */
            memcpy(mmsir,&mmsi,sizeof(mmsi));
            memcpy(lat_ddr,&lat_dd,sizeof(lat_dd));
            memcpy(long_ddr,&long_dd,sizeof(long_dd));
            return(0);
        }
        else{
            mmsi=0L;
            lat_dd=0;
            long_dd=0;
            return(-1);
        }
}
