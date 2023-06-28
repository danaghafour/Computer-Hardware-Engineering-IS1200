  # timetemplate.asm
  # Written 2015 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

.macro	PUSH (%reg)
	addi	$sp,$sp,-4
	sw	%reg,0($sp)
.end_macro

.macro	POP (%reg)
	lw	%reg,0($sp)
	addi	$sp,$sp,4
.end_macro

	.data
	.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
	.text
main:
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0,2
	jal	delay
	nop
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime
	lw	$a1,0($t0)
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time
	addiu	$t0,$t0,1	# increase
	andi	$t1,$t0,0xf	# check lowest digit
	sltiu	$t2,$t1,0xa	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line
  #
hexasc:
andi $v0,$a0,0xf        # save only low 4 bits
addi $v0,$v0,0x30   # turn into ASCII
ble $v0,0x39,L2     # if it's > ASCII '9'
addi $v0,$v0,7      #   add additional 7 for 'A'-'F'
L2:
jr $ra      
stop:   j   stop        # stop after one run
       nop              # delay slot filler (just in case)
       
       delay:
       # ms = t0
       # s0 = i
PUSH    ($s0)
PUSH    ($ra)

addi    $s0,$0,0            #i = 0
addi    $t1,$0,4711 
move    $t0,$a0         #move the argument to t0
while:
bgt     $0,$t0,done        #branch to done if 0=>ms
addi    $t0,$t0,-1      #decrement ms by 1
for:
beq     $s0,$t1,done
addi    $s0,$s0,1
j       for
j       while
done:

POP ($ra)
POP ($s0)
jr  $ra

time2string:
    # Extract NBCD-coded minutes/seconds from $a1.
    andi    $t0, $a1, 0xF000    # $t0 = zeros concat with most significant
    sra $t0, $t0, 12        # NBCD-coded digit (tens of minutes)
    andi    $t1, $a1, 0xF00     # $t1 = zeros contact with second most significant
    sra $t1, $t1, 8     # NBCD-coded digit (minutes)
    andi    $t2, $a1, 0xF0      # t2 = zeros concat with second lest significant
    sra     $t2, $t2, 4     # NBCD-coded digit (tens of seconds)
    andi    $t3, $a1, 0xF       # t3 = zeros concat with least significant
                    # NBCD-coded digit (seconds)

    # Save necessery overwritten constants.
    move    $t4, $a0        # $t4 = memory address for time2string output
    move    $t5, $ra        # $t5 = return address to main

    # Convert NBCD-coded minutes/seconds to ASCII-coded digits.
    # Arrange ASCII-coded minutes/seconds into the above specified sequence of
    # characters in the area of memory pointed to by $t4 = $a0.
    move    $a0, $t0    # a0 = NBCD-coded tens of minutes digit
    jal hexasc      # $v0 = ASCII-coded tens of minutes digit
    nop
    sb  $v0, 0($t4) # store 8 least significant bits of $v0 at mem[reg($t4)]

    move    $a0, $t1    # $a0 = NBCD-coded minutes digit
    jal hexasc      # $v0 = ASCII-coded minutes digit
    nop
    sb  $v0, 1($t4) # store 8 least significant bits of $v0 at 
                # mem[reg($t4 + sgnxt(1)]

    li  $t6, 0x3A   # $t6 = ASCII-code for colon
    sb  $t6, 2($t4) # store 8 least significant bits of $t6 at
                # mem[reg($t4 + sgnxt(2)]               

    move    $a0, $t2    # $a0 = NBCD-coded tens of seconds digit
    jal hexasc      # $v0 = ASCII-coded tens of seconds digit
    nop
    sb  $v0, 3($t4) # store 8 least significant bits of $v0 at
                # mem[reg($t4 + sgnxt(2)]

    move    $a0, $t3    # $a0 = NBCD-coded seconds digit
    jal hexasc      # $v0 = ASCII-coded seconds digit
    nop
    sb  $v0, 4($t4) # store 8 least significant bits of $v0 at
                # mem[reg($t4 + sgnxt(3)]

    li  $t6, 0x00   # $t6 = ASCII-code for NUL
    sb  $t6, 5($t4) 

    jr  $t5
    nop
#####################################################################################

while:
    ble    $a0,$0,done    # Here, we break if ms <= 0 
    nop			  # We create no operation
    addi    $a0,$a0,-1    # ms = ms - 1;
    li    $t2,0                    # we set i = 0;
    li      $t3,4711
for:
    beq    $t2,$t3,while    # i < 4711;
    nop
    addi    $t2,$t2,1    # i++;
    j    for
    nop
done:
    jr    $ra
    nop 