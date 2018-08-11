/******************************************************************************/
/*                                                                            */
/*  Module name :     IEEERES                                                 */
/*                                                                            */
/*  Function :        Provides SAPs and link stations info for a LAN adapter  */
/*                                                                            */
/*  Usage :           IEEERES [Adapter={0/1}]                                 */
/*                                                                            */
/*                    default: Adapter=0                                      */
/*                                                                            */
/*                    IEEERES                                                 */
/*                                                                            */
/*  Return codes :    0    if successful                                      */
/*                    1    if a parameter was bad and/or Help was given       */
/*                    x    if an IEEE CCB fails the ccb.retcode is returned   */
/*                                                                            */
/*  Author :          Franck Minssieux                                        */
/*                                                                            */
/******************************************************************************/

#define E32TO16

#include <stdio.h>
#include <os2.h>
#include <ctype.h>
#include <string.h>


/* Include programming language support declarations    */

#include "lan_7_c.h"   /* general typedefs */
#include "lan_1_c.h"   /* DLC defines      */
#include "lan_3_c.h"   /* CCB structures   */
#include "lan_6_c.h"   /* ACSLAN external definitions */


/*   program declarations   */

#define COMMAND_IN_PROCESS 0xFF

struct ccb_pt
   {
   struct command_control_block_dlr ccb;
   struct dir_status_parms status;
   };

address Bad_CCB_Ptr;
byte ret_code;
byte Lana;                      /* 0 or 1                             */
byte PermAddr[6];               /* Burnt-in address of adapter        */
byte NodeAddr[6];               /* Network address of adapter         */
byte MaxSap;                    /* Maximum allowable SAPs             */
byte OpenSap;                   /* Number of SAPs opened              */
byte MaxSta;                    /* Maximum allowable link stations    */
byte OpenSta;                   /* Number of link stations opened     */
byte AvailSta;                  /* Number of link stations available  */
struct ccb_pt workccb;


/**************************************************************/
/*                                                            */
/*  dir_status  implements the IEEE DIR.STATUS function       */
/*                                                            */
/**************************************************************/

dir_status (ccb_parm,adapternum)
struct ccb_pt *ccb_parm;
byte adapternum;
{
        struct command_control_block_dlr *status_ccb;
        struct dir_status_parms *pt;

        status_ccb=&ccb_parm->ccb;
        pt=&ccb_parm->status;
        memset (status_ccb,0,sizeof(struct command_control_block_dlr));

        status_ccb->ccb_command     = LLC_DIR_STATUS;
        status_ccb->ccb_adapter     = adapternum;
        status_ccb->ccb_retcode     = COMMAND_IN_PROCESS;
        status_ccb->parm_1.ccb_parm_offset = (address) pt;

        memset(pt,0,sizeof(struct dir_status_parms));

        ACSLAN((char *)status_ccb,(char *)&Bad_CCB_Ptr);
        while (status_ccb->ccb_retcode==COMMAND_IN_PROCESS);
        return;
}


/**************************************************************/
/*                                                            */
/*  Help        Displays directions for running this program  */
/*                                                            */
/**************************************************************/

Help()
{
        puts( " " );
        puts( " The syntax is:           ");
        puts( "                          ");
        puts( " IEEERES [Adapter={0|1}]  ");
        puts( "                          ");
        puts( " default: Adapter=0       ");
        puts( " ");
        return;
}


/**************************************************************/
/*                                                            */
/*                  Main Program                              */
/*                                                            */
/**************************************************************/

main(argc,argv)
int argc;
char *argv[];
{
        char *cp;
        byte i;

        if (argc==1)
                Lana=0;
        else    {
                if (argc!=2)  {
                        help();
                        exit(1);
                }

                cp = argv[1];

                switch(toupper(*cp))    {

                        case 'A':       cp = strchr(cp,'=');
                                        if (!*cp) {
                                            printf("\nBad adapter number\n");
                                            exit(1);
                                        }
                                        if (*++cp=='0')
                                            Lana=0;
                                        else if (*cp=='1')
                                            Lana=1;
                                        else {
                                            printf("\nBad adapter number\n");
                                            exit(1);
                                        }
                                        break;

                        case '?':       help();
                                        exit(1);

                         default:       printf("\nUnknown parameter on command line: %s\n",cp);
                                        help();
                                        exit(1);
                }
        }

        printf("\nReading the adapter information...\n");

        /****************************************************************/
        /*    Get the SAPs and link stations info for the adapter       */
        /****************************************************************/

        dir_status(&workccb,Lana);

        ret_code=workccb.ccb.ccb_retcode;
        if (ret_code!=LLC_COMMAND_ACCEPTED && ret_code!=LLC_COMMAND_CMPL_NOT_OPENED) {
                printf("\nError %2X\n",ret_code);
                exit (ret_code);
        }
        memcpy(PermAddr,workccb.status.burned_in_addr,6);
        memcpy(NodeAddr,workccb.status.node_address,6);
        MaxSap=workccb.status.max_sap;
        OpenSap=workccb.status.open_sap;
        MaxSta=workccb.status.max_station;
        OpenSta=workccb.status.open_station;
        AvailSta=workccb.status.avail_station;

        printf("\nThe burned-in address of LAN adapter %u is ",Lana);
        for(i=0;i<6;++i)
                printf("%02X",PermAddr[i]);
        printf("\nThe current network address of LAN adapter %u is ",Lana);
        for(i=0;i<6;++i)
                printf("%02X",NodeAddr[i]);
        printf("\n");
        printf("\nThe Maximum number of allowable SAPs is %d",MaxSap);
        printf("\nThe Number of opened SAPs is %d",OpenSap);
        printf("\nThe Maximum number of allowable link stations is %d",MaxSta);
        printf("\nThe Number of opened link stations is %d",OpenSta);
        printf("\nThe Number of available link stations is %d",AvailSta);
        printf("\n");
        exit(0);
}
