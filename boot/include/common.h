ReadSector:
	push %bp
	mov %sp,%bp
	sub $2,%sp
	mov %cl,-2(%bp)
	push %bx
	mov BPB_SecPerTrk,%bl
	div %bl
	inc %ah
	mov %ah,%cl
	mov %al,%dh
	shr $1,%al
	mov %al,%ch
	and $1,%dh
	mov (BS_DrvNum),%dl
	pop %bx
GoOnReading:
	mov $2,%ah
	mov -2(%bp),%al
	int $0x13
	jc GoOnReading
	add $2,%sp
	pop %bp
	ret

GetFatEntry:
	push %es
	push %bx
	push %ax
	mov $BaseOfLoader,%ax	
	sub $0x100,%ax
	mov %ax,%es
	pop %ax
	movb $0,(FlagOdd)
	mov $3,%bx
	mul %bx
	mov $2,%bx
	div %bx
	movb %dl,(FlagOdd)
LABEL_EVEN:
	xor %dx,%dx
	mov (BPB_BytesPerSec),%bx
	div %bx
	push %dx
	add $1,%ax
	mov $2,%cl
	mov $0,%bx
	call ReadSector	
	pop %dx
	add %dx,%bx
	mov %es:(%bx),%ax
	cmpb $0,(FlagOdd)
	jz Even_2
	shr $4,%ax
Even_2:
	and $0x0FFF,%ax	
Get_FatEntry_OK:
	pop %bx
	pop %es
	ret
