#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "portable.h"
#include "nmea.h"
#include "sixbit.h"
#include "vdm_parse.h"

int decode_ais_gps(char * aismsg,long *mmsir,double *lat_ddr,double *long_ddr);

int main( int argc, char *argv[] )
{
char buf[]={"!AIVDM,1,1,,B,B52R1M@008w6T73Uu8WJ;wa5oP06,0*03\n"};
long mmsi;
double lat_dd,long_dd;
mmsi=0L;
lat_dd=0;
long_dd=0;
if(decode_ais_gps(buf,&mmsi,&lat_dd,&long_dd)==-1) printf("error decoding\n");
printf( "    { mmsi: \"%09ld\",\n", mmsi );
printf( "      Point: {coordinates: [%0.6f, %0.6f, 0] }\n    },\n", long_dd, lat_dd );
exit(0);
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
    printf("aismsg: %s\n",aismsg);    
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
