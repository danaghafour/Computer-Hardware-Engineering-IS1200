  # labwork.S
  # Written 2015 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

.data
.global hexasc
.global time2string
.global delay
.text

 hexasc:
  	andi 	$a0,$a0,0xf     # save only low 4 bits to register $a0. 
	addi 	$v0,$a0,0x30   	# turn into ASCII
	ble 	$v0,0x39,L2     # Branch on less than equal to if it's > ASCII '9'. 
	nop 
	addi 	$v0,$a0,0x37    # add additional 7 for 'A'-'F' in ascii-code representation, for the hexadecimal digit. 
	L2:
	jr $ra      
	#j   stop    	    # stop after one run
       	nop                 # delay slot filler (just in case)
       	
  delay:
while:
    ble    $a0,$0,done    # Here, we break if ms <= 0 
    nop			  # We create no operation
    addi    $a0,$a0,-1    # ms = ms - 1;
    li    $t2,0                    # we set i = 0;
    li      $t3,4711
for:
    beq    $t2,$t3,while    # The brench happen when i < 4711;
    nop
    addi    $t2,$t2,1    # i++;
    j    for
    nop
done:
    jr    $ra
    nop 

 time2string:
    # Extract NBCD-coded minutes/seconds from $a1.
    
    PUSH $s0
    PUSH $s1
    PUSH $ra
    
    
    move	$s0,$a0
    move	$s1,$a1
    
    
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

    POP $ra
    POP $s1
    POP $s0
    
    
    jr $ra
    nop 
    

