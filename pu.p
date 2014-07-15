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
       sbco r3,c7,0x30,4      // = tx on, rx on & Free ot enable
       ldi r4,0
       ldi r5,0
       ldi r3,0
       ldi r20,0
       ldi r21,0
TB2:
                              // r5 points to start of buffer
                              // r4 is offset into buffer
       lbco r3,c7,0x14,4      //check LSR for date ready
       qbbc TB2,r3.t0

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
