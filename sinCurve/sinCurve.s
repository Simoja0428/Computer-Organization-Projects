//Jacob Simonek || Computer Org || Week of: 02/07/21
        .text 
        .align 2 
        .global main 

main: 
    str     x30, [sp, -16]! //Preserves x30 since we call other functions (Pre)
    stp     d9, d10, [sp, -16]! //Preserves d9 and d10 for usage (Pre)
    stp     d11, d12, [sp, -16]! //Preserves d11 and d12 for usage (Pre)
    stp     x21, x22, [sp, -16]! //Preserves x21 and x22 for usage (Pre)
    stp     x23, x24, [sp, -16]! //Preserves x23 and x24 for usage (Pre)
    stp     x25, x26, [sp, -16]! //Preserves x25 and x26 for usage (Pre)
    bl      initscr //Calls on the initscr function
    adr     x23, levels //This loads the the pointer to levels into x23
    ldr     d9, pi //This loads the global var pi into d9
    fadd    d9, d9, d9 //This initializes tpi in d9 by adding pi + pi (same as *2)
    fsub    d10, d9, d9 //This intializes phase d10 as 0 by subtracting pi from itself
    fsub    d11, d9, d9 //This intializes theta d11 as 0 by subtracting pi from itself
    ldr     x26, =COLS //This loads a pointer to external variable COLS into x26
    ldr     w26, [x26] //This loads the value (an int) of external COLS into w26
    scvtf   d0, w26 //This casts COLS as a double (previously it was an int) into d0
    fdiv    d12, d9, d0 //This initializes increment (d12) by dividing tpi by COLS
top: 
    bl      erase //Calls on the erase function, no parameters
    fadd    d10, d10, d12 //Adds increment to phase and then stores in phase
    mov     w21, 0 //Sets l (w21) register as zero
sinner:
    ldr     x25, =LINES //Loads a pointer to external var LINES into x25
    ldr     w25, [x25] //Loads the value of LINES (an int) into w25
    cmp     w21, w25 //Compares l (w21) register to LINES (w25) register
    bge     bottom //If l is greater than or equal to LINES then it branches to bottom
    fsub    d11, d9, d9 //Initializes theta by subtracting pi from itself
    mov     w22, 0 //Sets c (w22) register as zero
tinner:
    ldr     x26, =COLS //Loads a ponter to external var COLS into x26
    ldr     w26, [x26] //Loads the value of x26 (an int) into w26
    cmp     w22, w26 //Compares c (w22) register to COLS (w26) register
    bge     binner //If c is greater than or equal to COLS goto binner
    fadd    d0, d10, d11 //Adds phase to theta and stores in d0 for para
    bl      sin //Calls on the sin function
    fmov    d1, 1.0 //Moves the value of 1.0 into d1
    fadd    d0, d0, d1 //Adds the value of 1.0 (d1) to sin(theta+phase) (d0)
    fmov    d2, 2.0 //Moves the value of 2.0 into d2
    fdiv    d0, d0, d2 //Divides the value of 2.0 (d2) by prev calculated val (d0)
    fmov    d3, 10.0 //Stores value of 10.0 into d3
    fmul    d0, d0, d3 //Multiplies the value of 10.0 (d3) by prev calculate val (d0)
    fcvtzs  w24, d0 //Casts the double intensity as an int
    mov     w0, w21 //Moves l (w21) into w0 as first para
    mov     w1, w22 //Moves c (w22) into w1 as second para
    uxtb    x24, w24 //Changes intensity to x register for the offset does not use third para 
    ldrb    w2, [x23, x24] //Offsets levels (x23) by intensity (w24) for third para
    bl      mvaddch //Calls on the mvaddch function w0 - l, w1 - c, w2 - levels[intensity]
    fadd    d11, d11, d12 //Adds increment to theta and stores in theta
    mov     w2, 1 //Moves the value of 1 to w2
    add     w22, w22, w2 //Adds 1 (w2) to c (w22) and stores in w22
    b       tinner //Goes back up to the tinner label's start
binner:
    mov     w2, 1 //Moves 1 into w2 register
    add     w21, w21, w2 //Adds 1 (w2) to the value of l (w21) stores in l
    b       sinner //Goes to start of sinner label
bottom:
    ldr     x0, =stdscr //Loads pointer to external variable stdscr into x0
    ldr     x0, [x0] //Loads the value of stdscr into x0 as first para
    mov     w1, 0 //Sets the value of w1 as 0 for the second para
    mov     w2, 0 //Sets the value of w2 as 0 for the third para
    bl      box //Calls on the box function x0 - stdscr, w1 - 0, w2 - 0
    bl      refresh //Calls on refresh function no paras
    b       top //Goes back to the start of the top label
    bl      endwin //Calls on the endwin function no paras
    ldp     x25, x26, [sp], 16 //Restores the x25 and x26 registers after use (Post)
    ldp     x23, x24, [sp], 16 //Restores the x23 and x24 registers after use (Post)
    ldp     x21, x22, [sp], 16 //Restores the x21 and x22 registers after use (Post)
    ldp     d11, d12, [sp], 16 //Restores the d11 and d12 registers after use (Post)
    ldp     d9, d10, [sp], 16 //Restores the d9 and d10 registers after use (Post)
    ldr     x30, [sp], 16 //Restores the x30 register since we called functions (Post)
    ret

.data

pi:         .double      3.14159265359 //Declares double pi for usage in main
levels:     .asciz      " .:-=+*#%@" //Declares char* levels for usage in main

.end 