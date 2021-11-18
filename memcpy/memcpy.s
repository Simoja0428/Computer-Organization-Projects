//Jacob Simonek - Computer Org - Week of 3/11/21
		.text //Lets the compiler know that code is about to be written
		.align 2 //Sets the memory to start at an even number
		.global MEMCPY //Used to link to main.cpp file so it knows this is method

dest	.req x23 //Reserves x23 register for us to move the destination to
sour	.req x24 //Reserves x24 register for us to move the source to
size	.req x25 //Reserves x25 register so we have the size of the what we are copying

MEMCPY: //Label for the MEMCPY function, Parameters: x0 - u_int8_t * dest, x1 - u_int8_t * sour, x2- size_t size
	stp 	x26, x30,[sp,-16]! //Backs up x30 register because we call other functions (also backs up x26 for us)
	stp 	x20, x21, [sp,-16]! //Backs up the x20 and x21 registers for later use (Pre)
	stp 	x22, x23, [sp,-16]! //Backs up the x22 and x23 registers for later use (Pre)	
	stp 	x24, x25, [sp,-16]! //Backs up the x24 and x25 registers for later use (Pre)		
	mov 	x23, x0 //Moves the first parameter, the dest*, to x23 to fill dest 
	mov 	x24, x1 //Moves the second parameter, the sour*, to x24 to fill sour
	mov 	w25, w2 //Moves the third parameter, the size, to x25 to fill size
	mov 	w1, 32 //Moves value of 32 into w1 so we can calculate the remainder of the size % 32
	udiv 	x2, size ,x1 //Divides size by 32 to give us the whole number portion needed to calculate size % 32
	msub 	x20,x2,x1,size //Calculates size % 32 and stores in x20 register, this is the remainder for the size of what we are copying
	cbz 	x20, sizeWith32 //If there is no remainder (size is divisible by 32) skip special cases 
	remainderSizes: //Copies over bytes that aren't divisible by 32
		sub    size, size, x20 //Subtracts the remainder value from size
		cmp 	w20,16 //Checks to see if the remainder value is greater than or equal to 16
		bge		sizeWith16 //If it is then it branches to copy 16 bytes, otherwise it skips
		cmp 	w20,8 //Checks to see if the remainder value is greater than or equal to 8	
		bge 	sizeWith8 //If it is then it branches to copy 8 bytes, otherwise it skips
		cmp 	w20,4  //Checks to see if the remainder value is greater than or equal to 4
		bge 	sizeWith4 //If it is then it branches to copy 4 bytes, otherwise it skips
		cmp 	w20,2  //Checks to see if the remainder value is greater than or equal to 2
		bge 	sizeWith2 //If it is then it branches to copy 2 bytes, otherwise it skips	
		cmp 	w20,1  //Checks to see if the remainder value is greater than or equal to 1
		bge 	sizeOdd //If it is then it branches to copy 1 byte, otherwise it skips
		b 		sizeWith32 //Now that special cases are handled, branches to copy over increments of 32
		sizeWith16: //Label to copy over 16 bytes based on remainder
			ldr 	x21,[sour],16 //Loads value of source into x21 and increments source by 16
			str 	x21,[dest],16 //Stores the value of x21 into destination offset by 16
			sub 	x20,x20,16 //Subtracts 16 from the remainder to account for copied bytes
			b 		remainderSizes //Branches back to top of remainder cases 
		sizeWith8: //Label to copy over 8 bytes based on remainder
			ldr 	x21,[sour],8 //Loads value of source into x21 and increments source by 8
			str 	x21,[dest],8 //Stores the value of x21 into destination offset by 8
			sub 	x20,x20,8 //Subtracts 8 from the remainder to account for copied bytes
			b		remainderSizes //Branches back to top of remainder cases 
		sizeWith4: //Label to copy over 4 bytes based on remainder
			ldr 	w21,[sour],4  //Loads value of source into x21 and increments source by 4
			str 	w21,[dest],4 //Stores the value of x21 into destination offset by 4
			sub 	x20,x20,4 //Subtracts 4 from the remainder to account for copied bytes
			b 		remainderSizes //Branches back to top of remainder cases 
		sizeWith2: //Label to copy over 2 bytes based on remainder
			ldrh 	w21,[sour],2 //Loads value of source into x21 and increments source by 2
			strh 	w21,[dest],2 //Stores the value of x21 into destination offset by 2
			sub 	x20,x20,2 //Subtracts 2 from the remainder to account for copied bytes
			b		remainderSizes //Branches back to top of remainder cases 
		sizeOdd: //Label to copy over 1 byte based on remainder	
			ldrb 	w21,[sour],1 //Loads value of source into x21 and increments source by 1
			strb 	w21,[dest],1 //Stores the value of x21 into destination offset by 1
			sub 	x20,x20,1 //Subtracts 1 from the remainder to account for copied bytes
	sizeWith32: //Copies over the bytes that can be copied in increments of 32 
		mov 	w1, 32 //Moves the number 32 into w1 register
		udiv 	x2, size, x1 //Divides the value of size by 32 and stores in x2
		msub 	x3, x2,  x1, size  //Calculates the remainder of count % 8
		udiv    x22, x2, x1   //Calculates the valuee of n by dividing by 32 (divide by 8 * 4 bytes per instruction)
	
	cbz 	x3, copying //If the value of the x3 is 0 then it branches
	adr 	x0, copying //Otherwise we assign the address of the copying branch label to x0
	mov 	w1, 8 //Moves the number 8 into w1 register
	sub 	w26, w1, w3 //Subtracts 8 from the value of the register holding the reference to the copying label
	mul  	x3, x26, x1 //Multiplies calculated value by 8 (x1 register)
	add 	w0, w0, w3 //Adds the previously calc'd value to the register holding copying label stores in that register
	br 		x0 //Branches down to the jump table, specifically the case value we calculated

	copying: //Copying acts as the jump table for memcpy() it is modeled based off of the one we did in class
		ldp 	q0,q1,[sour],32 //First occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //First occurence of unrolling loop to store values in destination
		ldp 	q0,q1,[sour],32 //Second occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //Second occurence of unrolling loop to load values in destination
		ldp 	q0,q1,[sour],32 //Third occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //Third occurence of unrolling loop to load values in destination
		ldp 	q0,q1,[sour],32 //Fourth occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //Fourth occurence of unrolling loop to load values in destination
		ldp 	q0,q1,[sour],32 //Fifth occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //Fifth occurence of unrolling loop to load values in destination
		ldp 	q0,q1,[sour],32 //Sixth occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //Sixth occurence of unrolling loop to load values in destination
		ldp 	q0,q1,[sour],32 //Seventh occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //Seventh occurence of unrolling loop to load values in destination
		ldp 	q0,q1,[sour],32 //Eigth occurence of unrolling loop to load values in source
		stp 	q0,q1,[dest],32 //Eigth occurence of unrolling loop to load values in destination
		cbz 	x22, bottom //Checks to see if the value of n is zero, if it is then go to termination sequence
		sub 	w22,w22,1 //Subtracts one from the value of n and stores in n
		b 		copying //Returns to top of unrolling statement
		
	bottom: //Botttom label acts as exit sequence for the function
		ldp 	x24, x25, [sp],16 //Restores x24 and x25 registers (Post)
		ldp 	x22, x23, [sp],16 //Restores x22 and x23 registers (Post)
		ldp     x20, x21, [sp],16 //Restores x20 and x21 registers (Post)
		ldp 	x26, x30, [sp],16 //Restores x26 and x30 registers (Post)
		ret //Returns from file back to main.cpp
