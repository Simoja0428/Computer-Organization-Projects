                .text //Says that code is about to be written
                .align 2 //Tells us that the memory should start at an even index
                .global main //User for the linker to find the main method

MyStrLen: //Label for the MyStrLen function
                mov x13, x0 //Moves the contents of char s* x0 into x13
                mov w14, wzr //This sets w14 register 
        mtop: //Label for the top in mystrlen kind of acts like a loop
                ldrb w0, [x13], 1 //Takes address of argv and adds 1 to it
                cbz x0, mbottom //Check to make sure the value in x0 is not null otherwise go to bottom
                add w14, w14, 1 //Adds 1 to the value of w14 stores in w14
                b mtop //Goes back up to the label kind of acting like a loop
        mbottom: //Bottom label for the function acts as exiter to loop
                mov w0, w14 //Moves the value of w14 into w0
                ret //Returns the value in w0

main: //Main function for the program
                str x30, [sp, -16]! //This protects the x30 register (Pre)
                mov x25, x1 //This moves the values in x1 (argv) to x25 
        top: //Top label for the main acts kind of like a loop
                ldr x21, [x25]  //Takes address of argv and stores in x21
                cbz x21, bottom //Checks to see if argv is null if it is exits the program
                ldr x22, [x25], 8 //Takes address of argv and stores in x22 adds 8
                mov x0, x22 //Moves x22 into x0 register
                bl MyStrLen //Calls on the string length function
                mov w1, w0 //Moves the value in w0 into w1 to store it
                mov x2, x22 //moves argv from x22 to x2
                adr x0, PrintState //Assigns the print statement to be the x0 register
                bl printf //Prints out Print statement - x0, Para 1 - w1, Para 2 - x2
                b top //This returns back to the top same as goto() in C  
        bottom: //Bottom label used for exit condition of program  
                ldr x30, [sp], 16 //Restores register x30 (Post)
                mov w0, wzr //Sets w0 to 0 so program will return
                ret //Returns ending the program

                .data //Used for system to know we are creating a variable holding a print statement
PrintState:     .asciz "[%d] [%s] \n" //Used for the print statement of the program
                .end //Signifies no code may come after

 
