# Computer-Organization-Projects
The following repository has some projects that I have personally completed in AARCH64 (ASM), C, and C++ programming languages. These projects focus on looking at how a computer works and runs programs. A thorough understanding of these projects would indicate an understanding of what goes on behind the scenes in a computer when we run a program.


## Enumerate Args
This project provides an introduction into deconstructed C and AARCH64 assembly language. The purpose of this project is to print out what the user types in the command line and each word's length. Deconstructed C is similar to normal C, however it lacks loops which requires us to use labels and goto statements. The translation from deconstructed C to AARCH64 is fairly simple. Notice the number of instructions it takes in assembly language to do something so simple. Common things we use in higher level programming like args++ are not just one instruction in assembly.  

## Sin(x) Curve
This project involved taking a C program written using the ncurses library and translating it over to AARCH64 assembly language. The purpose of both these programs is using the ncurses library, print out a sin(x) curve made up of various nonletter symbols. Completion of this project demonstrates a ability to work with floating point numbers in AARCH64 programming. Notice that floatng point numbers require a different kind of register than an int or double would. Floating point numbers also have their own set of arthimetic operation instructions to make use of.
