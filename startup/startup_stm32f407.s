.syntax unified
.cpu cortex-m4
.thumb

.global  _start                 // program starts from here
.global  Reset_Handler          // go to Reset_Handler function line #35

/* Stack pointer */
.word    _estack

/* Vector Table */
.section .isr_vector, "a", %progbits
.type    _vectors, %object
_vectors:
    .word _estack           // Stack pointer */
    .word Reset_Handler     // Reset handler */
    .word 0                 // NMI */
    .word 0                 // HardFault */
    .word 0                 // MemManage */
    .word 0                 // BusFault */
    .word 0                 // UsageFault */
    .word 0                 // Reserved */
    .word 0
    .word 0
    .word 0
    .word 0                 // SVC */
    .word 0                 // DebugMon */
    .word 0
    .word 0                 // PendSV */
    .word 0                 // SysTick */


/* Reset Handler */
.section .text.Reset_Handler
.type Reset_Handler, %function
Reset_Handler:

    /* COPY .data: FLASH SRAM */
    ldr r0, =_sdata                 // r0 = SRAM destination start  
    ldr r1, =_edata                 // r1 = SRAM destination end    */
    ldr r2, =_sidata                // r2 = FLASH source start      
    movs r3, #0                     // r3 = offset = 0              
    b   Data_Loop_Check             // check condition first        

Data_Copy:
    ldr r4, [r2, r3]                // read 4 bytes from FLASH      
    str r4, [r0, r3]                // write 4 bytes to SRAM        
    adds r3, r3, #4                 // offset += 4                  

Data_Loop_Check:
    adds r4, r0, r3                 // r4 = current SRAM address    
    cmp  r4, r1                     // current == end?              
    bcc  Data_Copy                  // no  --> copy more              
                                    // yes --> fall through to .bss   

    /* ZERO .bss in SRAM */
    ldr  r2, =_sbss                 // r2 = .bss start in SRAM      
    ldr  r4, =_ebss                 // r4 = .bss end   in SRAM      
    movs r3, #0                     // r3 = value to write (zero)   
    b    Bss_Loop_Check             // check condition first        

Bss_Zero:
    str  r3, [r2]                   // write 0 to current address   
    adds r2, r2, #4                 // move to next address         

Bss_Loop_Check:
    cmp r2, r4                      // current == end?              
    bcc Bss_Zero                    // no --> zero more              
                                    // yes--> fall through to main   

    /* Call main */
    bl main                         // call main()                  
    bx lr                           // return (safety net)        

/*
    Reset_Handler
    |
    | Data_Loop_Check --> r4 < r1? --> YES ==> Data_Copy
    |        |                                    |
    |        |____________________________________|
    |
    |  (r4 >= r1, .data done)
    |
    | Bss_Loop_Check --> r2 < r4? --> YES ==> Bss_Zero
    |         |                                    |
    |         |____________________________________|
    |
    |  (r2 >= r4, .bss done)
    |
    | bl main

*/

