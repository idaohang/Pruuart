/*pu.c - PRUSS UART
 *       uses PRUSS Uart to read data from ADAFRUIT Ultimate GPS
 *
 * The C-code is derived from:
 * PRU_memAccessPRUDataRam.c
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2010-12
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

/*****************************************************************************
 * Copyright (c) John Stampfl 2014
 * This code is provided for your reading pleasure.
 * anyother use is at your own risk.  
 *
 *pu.c uses the PRUSS to read data from
 *pu.c the ADAFRUIT Ultimate GPS
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

//define RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
//define B57600 "$PMTK251,57600*2C\r\n"
//#define RATE10 "$PMTK220,100*2F\r\n"
#define PRU_NUM 	0
#define AM33XX

static int LOCAL_exampleInit ( unsigned short pruNum );
static int LOCAL_pr ( unsigned short pruNum );
static int LOCAL_wr ( unsigned short pruNum );
static void *pruDataMem;
static unsigned int *pruDataMem_int;
static char *pdata;
static char *loutdata;     //will be address of message in pru memory
                           // set in  LOCAL_exampleInit()
static const char RMCONLY[] = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
void set_mesg(int messno);  //put message into pru memory

int main (void)
{
    int ret1,i,cnt;
    int fd,len,msig,messno;

    unsigned int ret,value;
    tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

    
    printf("\nINFO: Starting %s example.\r\n", "pt - SR04");

    cnt = 0;
    messno = 1;
    /* Initialize the PRU */
    prussdrv_init ();

    /* Open PRU Interrupt */
    ret = prussdrv_open(PRU_EVTOUT_0);
    if (ret)
    {
        printf("prussdrv_open open failed\n");
        return (ret);
    }

    /* Get the interrupt initialized */
    prussdrv_pruintc_init(&pruss_intc_initdata);

    /* Initialize example */
    printf("\tINFO: Initializing example.\r\n");
    LOCAL_exampleInit(PRU_NUM);

    /* Execute example on PRU */
    printf("\tINFO: Executing example.\r\n");
    prussdrv_exec_program (PRU_NUM, "./puw.bin");
    pruDataMem_int[66] = 0;
    set_mesg(messno);        //put message into pru memory
    messno = 0;              //set message number
    printf("\tINFO: Waiting for \r\n");
    while(1) {
    prussdrv_pru_wait_event (PRU_EVTOUT_0);
    prussdrv_pru_clear_event (PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
    // Wait for 10 messages to come, before sending message
    if (cnt == 10) {
        ret1 = LOCAL_wr(PRU_NUM);    //command pru to send message
    }
    cnt++;
    ret1 = LOCAL_pr(PRU_NUM); // go get & print result
    }
    usleep (500000); //Just pause so don't go too fast
    prussdrv_pru_disable (PRU_NUM);
    prussdrv_exit ();
    sleep(20);
    return(0);

}

static int LOCAL_exampleInit ( unsigned short pruNum )
{
    //Initialize pointer to PRU data memory
    if (pruNum == 0)
    {
      prussdrv_map_prumem (PRUSS0_PRU0_DATARAM, &pruDataMem);
    }
    else if (pruNum == 1)
    {
      prussdrv_map_prumem (PRUSS0_PRU1_DATARAM, &pruDataMem);
    }
    pdata = (char*) pruDataMem;
    pruDataMem_int  = (unsigned int*) pruDataMem;
    loutdata = pdata + 276;
return(0);
}

static int LOCAL_pr ( unsigned short pruNum )
{
	int i;
	//Here read data from pru memory
	printf("data  %s\n",pdata);

        return 0;
}
//     WRITE	- set write flag in pru memory
//     message to write should be in pru memory
static int LOCAL_wr (unsigned short pruNum) {
	if (pruDataMem_int[66] == 0) {
		pruDataMem_int[66] = 1;
	}
	printf("in write\n");
}
//     WRITE    - put message to write into pru memory
void set_mesg(int messno) {
	if (messno == 1 ) {
		strcpy(loutdata,RMCONLY);
	}
}
