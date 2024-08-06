ORG 0x7c00
BITS 16

CODE_SEGMENT_SELECTOR equ gdt_code-gdt_start
DATA_SEGMENT_SELECTOR equ gdt_data-gdt_start

jmp short start
nop

; FAT16 Header BPB(BIOS Parameter Block)
OEMIdentifier           db 'MIKERNEL'
BytesPerSector          dw 0x200
SectorsPerCluster       db 0x80
ReservedSectors         dw 200
FATCopies               db 0x02
RootDirEntries          dw 0x40
NumSectors              dw 0x00
MediaType               db 0xF8
SectorsPerFat           dw 0x100
SectorsPerTrack         dw 0x20
NumberOfHeads           dw 0x40
HiddenSectors           dd 0x00
SectorsBig              dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber             db 0x80
WinNTBit                db 0x00
Signature               db 0x29
VolumeID                dd 0xD105
VolumeIDString          db 'PEACHOS BOO'
SystemIDString          db 'FAT16   '


start:
    jmp 0:step1

step1:
    cli
    mov ax,0
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov sp,0x7c00
    sti

.loadGDT:
    cli
    lgdt[gdt_description]
    mov eax,cr0
    or eax,1
    mov cr0,eax
    jmp CODE_SEGMENT_SELECTOR:load32

;GDT
gdt_start:       ;Various Segment Descriptors.
gdt_null:
    dd 0
    dd 0
gdt_code:
    dw 0xffff    ;First 16 bits of Limit
    dw 0         ;First 16 bits of Base
    db 0         ;Next 8 bitsof Base
    db 0x9a      ;Access code 
    db 0xcf      ;Last nibble of Limit +flags
    db 0         ;Last 8 bits of Base
gdt_data:
    dw 0xffff    ;First 16 bits of Limit
    dw 0         ;First 16 bits of Base
    db 0         ;Next 8 bitsof Base
    db 0x92      ;Access code 
    db 0xcf      ;Last nibble of Limit +flags
    db 0         ;Last 8 bits of Base
gdt_end:
gdt_description:
    dw gdt_end-gdt_start-1
    dd gdt_start



 [BITS 32]
 load32:
    mov eax, 1
    mov ecx, 100
    mov edi, 0x0100000
    call ata_lba_read
    jmp CODE_SEGMENT_SELECTOR:0x0100000

ata_lba_read:
    mov ebx, eax, ; Backup the LBA
    ; Send the highest 8 bits of the lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; Select the  master drive
    mov dx, 0x1F6
    out dx, al
    ; Finished sending the highest 8 bits of the lba

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read

    ; Send more bits of the LBA
    mov eax, ebx ; Restore the backup LBA
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; Restore the backup LBA
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; Restore the backup LBA
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory
.next_sector:
    push ecx

; Checking if we need to read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

; We need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw
    pop ecx
    loop .next_sector
    ; End of reading sectors into memory
    ret
times 510-($ -$$) db 0
dw 0xAA55