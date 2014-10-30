#start instructions

	#Loop while I < SIZE
WHILE_1:	LDR	R1	I
			LDR	R2	SIZE
			CMP	R1	R2
			BNZ	R1	ENDWHILE_1

			#start loop code
			LDR	R3	SUM
			LDA R4	ARR
			LDR	R5	INTSIZE
			LDR	R1	I
			MUL	R5	R1
			ADD	R4	R5
			LDR	R5	(R4)
			ADD	R3	R5
			STR	R3	SUM
			LDR R0	ONE
			AND	R0	R5
			BNZ R0	EVEN
ODD:		MOV	R0	R5
			TRP	$1
			LDA	R1	STR_ODD
			JMP	PRINTF_1
			JMP	END_ODDEVEN	
EVEN:		MOV	R0	R5
			TRP $1
			LDA	R1	STR_EVEN
			JMP	PRINTF_1
END_ODDEVEN:	LDR R1	I
				ADI	R1	$1
				STR	R1	I
				JMP WHILE_1
ENDWHILE_1:	LDA	R1	STR_SUM
			JMP	PRINTF_2
RET_PF2:	LDR	R0	SUM
			TRP	$1
			LDR	R0	NEWLINE
			TRP	$3

	#PART 2
			LDA R1	GADS
			LDB	R0	(R1)
			TRP $3
			ADI	R1	$1
			LDB	R0	(R1)
			TRP	$3
			ADI	R1	$1
			LDB	R0	(R1)
			TRP	$3
			ADI	R1	$1
			LDB	R0	(R1)
			TRP	$3
			LDB	R0 SPACE
			TRP	$3
			LDR R0	GADS
			TRP	$1
			MOV	R0	R5
			LDR	R0	NEWLINE
			TRP	$3

			#Switch D and G
			LDA R1	GADS
			LDB R2	(R1)
			ADI	R1	$2
			LDB	R3	(R1)
			STB R2	(R1)
			LDA	R1	GADS
			STB R3 	(R1)

			LDA R1	GADS
			LDB	R0	(R1)
			TRP $3
			ADI	R1	$1
			LDB	R0	(R1)
			TRP	$3
			ADI	R1	$1
			LDB	R0	(R1)
			TRP	$3
			ADI	R1	$1
			LDB	R0	(R1)
			TRP	$3
			LDB	R0 SPACE
			TRP	$3
			LDR R0	GADS
			TRP	$1
			MOV	R0	R4
			LDR	R0	NEWLINE
			TRP	$3

			SUB	R4	R5
			MOV	R0	R4
			TRP $1
			LDR	R0	NEWLINE
			TRP	$3	



	#END PROGRAM
			TRP	$0



#subroutines

	#ASSUME passed in arguments is R0 = char[]
PRINTF_1:	LDB	R0	(R1)	
		LDB	R2	NULL
		CMP	R2	R0
		BNZ	R0	END_ODDEVEN  #return if null
		TRP	$3
		ADI	R1	$1
		JMP	PRINTF_1

PRINTF_2:	LDB	R0	(R1)	
		LDB	R2	NULL
		CMP	R2	R0
		BNZ	R0	RET_PF2  #return if null
		TRP	$3
		ADI	R1	$1
		JMP	PRINTF_2
#end instructions


#Start directives
SIZE	.INT	10

ARR	.INT	10
	.INT 	2
	.INT 	3
	.INT 	4
	.INT 	15
	.INT 	-6
	.INT 	7
	.INT 	8
	.INT 	9
	.INT 	10

NULL	.BYT	0x00

STR_EVEN	.BYT	' '
	.BYT	'i'
	.BYT	's'
	.BYT	' '
	.BYT	'e'
	.BYT	'v'
	.BYT	'e'
	.BYT	'n'
	.BYT	'\n'
	.BYT	0x00

STR_ODD	.BYT	' '
	.BYT	'i'
	.BYT	's'
	.BYT	' '
	.BYT	'o'
	.BYT	'd'
	.BYT	'd'
	.BYT	'\n'
	.BYT	0x00

STR_SUMSIZE	.INT	7

STR_SUM	.BYT	'S'
	.BYT	'u'
	.BYT	'm'
	.BYT	' '
	.BYT	'i'
	.BYT	's'
	.BYT	' '
	.BYT	0x00

GADS	.BYT	'D'
		.BYT	'A'
		.BYT	'G'
		.BYT	'S'

NEWLINE	.BYT	'\n'
SPACE	.BYT	' '
EQUALS	.BYT	'='	

I	.INT	0
SUM	.INT	0
TEMP	.INT	0
RESULT	.INT	0
INTSIZE	.INT	4
ONE	.INT	1
ZERO	.INT	0

#end Directives