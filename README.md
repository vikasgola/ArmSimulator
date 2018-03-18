# ArmSimulator
Basic Instructions Arm Simulator for Arm.

# Use
Go to main folder.

----Compilation Process------ 

open terminal and type "./run.sh".

----Only run-----------

open terminal and type "./simulator".

There is two modes to run your code one is to 'Run' which will completly run your code and give output live and also saves the output after each instruction in output.txt file. The other mode is 'Debug' mode which will run live one by one instruction and show you output after each instruction. 

Your can give your input file name(Program file name on run time). Whole output of code can be viewed on output file named output.txt. You can also check the debug file named as debug.txt.


# Availability

*Memory is of 100 words.

*Instructions name can be small or large

	rd - destination addresses
	rn , rn1 , rn2 - registers
	c,c2 - constants

*--- addition support------

	add rd , rn , #c
	add rd , #c2 , #c
	add rd , rn1 , rn2

*--- substraction support------

	sub rd , rn , #c
	sub rd , #c2 , #c
	sub rd , rn1 , rn2

*--- multiplication support------

	mul rd , rn , #c
	mul rd , #c2 , #c
	mul rd , rn1 , rn2

*--- mov support------

	mov rd , rn
	mov rd , #c

*--- load support------

	ldr rd, [ rn , #c ]
	ldr rd , =labelname
	ldr rd , [rn]

*--- str support------

	str rd, [ rn , #c ]
	str rd , [rn]

*----cmp support------

	cmp rd , rn
	cmp rd , #c

*----bne support------

	bne labelname

*----bge support------

	bge labelname

*----b support------

	b labelname

*----bl support------

	bl labelname

*----swi exit support------

	swi SWI_Exit

# Tested on code given below
	.equ SWI_Exit, 0x11
	
	.text
	mov r1, #40
	mov r2, #1
	ldr r3, =AA	

	StoreIntegers:
		str r2, [r3]	
		add r3, r3, #4 		
		add r2, r2, #1 		
		sub r1, r1, #1 		
		cmp r1, #0 	        
		bne StoreIntegers
	
	mov r1, #40
	mov r4, #0
	ldr r3, =AA 

	LoadAddIntegers:
		ldr r2, [r3]     	
		add r4, r4, r2          
		add r3, r3, #4   	
		sub r1, r1, #1    	
		cmp r1, #0        	
		bne LoadAddIntegers	

	swi SWI_Exit		

	.data
	AA:	
		.space 40
		.end

