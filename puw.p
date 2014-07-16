// pu.p  V1
// Use PRUSS uart to read data from ADAFRUIT Ultimate GPS
// reads data to nl, put in buffer and signal linux side.
.setcallreg r2.w0  //  Going to use r31 for interrupt
.origin 0
.entrypoint TB
TB:
       ldi r20,0
       ldi r21,0
       ldi r22,0
       mov r0,0
       sbbo r0,r20,r21,4
       zero &r0,64             //zero 16 registers
TB05:
       sbbo r0,r20,r21,64      //zero some of pru0 local memory
       add r21,r21,64          // for linux string functions
       add r22,r22,1
       qbgt TB05,r22,20
TB1:
       ldi r3,4               //Uart divisor  =1250 = 0x04E2
       sbco r3,c7,0x24,4      // 9600 at 16x
       ldi r3,0xE2            //in DLL & DLH
       sbco r3,c7,0x20,4

       ldi r3,0x3             //LCR = 3, 8 none & 1
       sbco r3,c7,0x0C,4      //

       mov r3,0x6001          //Power &
       sbco r3,c7,0x30,4      // = tx on, rx on & Free to enable
       ldi r4,0
       ldi r5,0
       ldi r3,0
       ldi r20,0
       ldi r21,0
       mov r7,256
       mov r9,276
       ldi r11,0
       ldi r24,0
TB2:
                              // r5 points to start of buffer
                              // r4 is offset into buffer
       lbco r3,c7,0x14,4      //check LSR for data ready
       qbbs TSN,r24.t0        //check for write cmd from linux
                              //first time will not be set
       lbbo r24,r7,8,4        //get write cmd from linux
TB21:
       qbbc TB2,r3.t0         //lsr zero bit set means data ready

       lbco r3,c7,0,4         //data is ready, get from RBR
       sbbo r3,r5,r4,1        // and put in buffer
       add r4,r4,1

       qbne TB2,r3,0xA        // do until nl received
       sbbo r20,r5,r4,1       // put null to terminate
       ldi r4,0               // zero buffer pointer
       mov r31.b0,35          // signal linux
       jmp TB2
TB9:
      
       mov r31.b0,35
       ldi r4,0xFFFF
       ldi r5,0
TB10:
       add r5,r5,1
       qbgt TB10,r5,r4     

       HALT

TSN:
       qbbc TB21,r3.t5        // check for empty transmitt buffer
                              // go to receive if not empty
       lbbo r10,r9,r11,1      // get next char to trnsmit
       add r11,r11,1
       qbeq TSN4,r10,0        // if char == NULL go to done
       sbco r10,c7,0,4        // otherwise put in transmit buffer
       ldi r12,3
       sbbo r12,r7,8,4        // set linux cmd flag to 3 so linux
                              // doesn't start another transmit, need
                              // bit zero set to continue transmit
                              // so can't use even number here
       jmp TB21

TSN4:
       ldi r24,0              //to reset linux cmd flag to allow anoterh
                              // transmit request
       ldi r11,0              // reset message pointer
       sbbo r24,r7,8,4        // put linux cmd flag into pru memory
       jmp TB21

