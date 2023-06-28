  # hexmain.asm
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
main:
	li	$a0,10		# change this to test different values

	jal	hexasc		# call hexasc
	nop			# delay slot filler (just in case)	

	move	$a0,$v0		# copy return value to argument register

	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window
	stop:   j   stop        # stop after one run
       nop              # delay slot filler (just in case)
hexasc: 
andi $v0,$a0,0xf        # save only low 4 bits
addi $v0,$v0,0x30   # turn into ASCII
ble $v0,0x39,L2     # if it's > ASCII '9'
addi $v0,$v0,7      #   add additional 7 for 'A'-'F'
L2:
jr $ra      


