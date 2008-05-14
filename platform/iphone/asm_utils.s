@ vim:filetype=armasm


.global _vidCpy8to16 @ void *dest, void *src, short *pal, int lines|(flags<<16),
                    @ flags=is32col[0], no_even_lines[1], no_odd_lines[2]

_vidCpy8to16:
    stmfd   sp!, {r4-r8,r12,lr}

    and     r4, r3, #0xff0000
    and     r3, r3, #0xff
    tst     r4, #0x10000
    mov     r3, r3, lsr #1
    orr     r3, r3, r3, lsl #8
    orreq   r3, r3, #0x27000000 @ 40 col mode
    orrne   r3, r3, #0x1F000000 @ 32 col mode
    addne   r0, r0, #64
    orr     r3, r3, r4
    add     r1, r1, #8
    mov     lr, #0xff
    mov     lr, lr, lsl #1

    @ no even lines?
    tst     r3, #0x20000
    addne   r0, r0, #640
    addne   r1, r1, #328
    bne     vcloop_odd

    @ even lines first
vcloop_aligned:
    ldr     r12, [r1], #4
    ldr     r7,  [r1], #4

    and     r4, lr, r12,lsl #1
    ldrh    r4, [r2, r4]
    and     r5, lr, r12,lsr #7
    ldrh    r5, [r2, r5]
    and     r6, lr, r12,lsr #15
    ldrh    r6, [r2, r6]
    orr     r4, r4, r5, lsl #16

    and     r5, lr, r12,lsr #23
    ldrh    r5, [r2, r5]
    and     r8, lr, r7, lsl #1
    ldrh    r8, [r2, r8]
    orr     r5, r6, r5, lsl #16

    and     r6, lr, r7, lsr #7
    ldrh    r6, [r2, r6]
    and     r12,lr, r7, lsr #15
    ldrh    r12,[r2, r12]
    and     r7, lr, r7, lsr #23
    ldrh    r7, [r2, r7]
    orr     r8, r8, r6, lsl #16

    subs    r3, r3, #0x1000000
    orr     r12,r12, r7, lsl #16

    stmia   r0!, {r4,r5,r8,r12}
    bpl     vcloop_aligned

    tst     r3, #0x10000
    add     r1, r1, #336             @ skip a line and 1 col
    addne   r1, r1, #64              @ skip more for 32col mode
    add     r0, r0, #640
    @add     r0, r0, #2
    addne   r0, r0, #128
    addeq   r3, r3, #0x27000000
    addne   r3, r3, #0x1F000000
    sub     r3, r3, #1
    tst     r3, #0xff
    bne     vcloop_aligned

    @ no odd lines?
    tst     r3, #0x40000
    ldmnefd sp!, {r4-r8,r12,pc}

    and     r4, r3, #0xff00
    orr     r3, r3, r4, lsr #8
    mov     r4, r4, lsr #7
    sub     r6, r4, #1
    mov     r5, #640
    @add     r5, r5, #2
    mul     r4, r5, r6
    sub     r0, r0, r4
    mov     r5, #328
    mul     r4, r5, r6
    sub     r1, r1, r4

    @sub     r0, r0, #2
vcloop_odd:
    mov     r8, #0

vcloop_unaligned:
    ldr     r12, [r1], #4
    ldr     r7,  [r1], #4

    and     r6, lr, r12, lsl #1
    ldrh    r6, [r2, r6]
    and     r5, lr, r12, lsr #7
    ldrh    r5, [r2, r5]
    orr     r4, r8, r6, lsl #16

    and     r6, lr, r12, lsr #15
    ldrh    r6, [r2, r6]
    and     r8, lr, r12, lsr #23
    ldrh    r8, [r2, r8]
    orr     r5, r5, r6, lsl #16

    and     r6, lr, r7, lsl #1
    ldrh    r6, [r2, r6]
    and     r12,lr, r7, lsr #7
    ldrh    r12,[r2, r12]
    orr     r6, r8, r6, lsl #16

    and     r8, lr, r7, lsr #15
    ldrh    r8, [r2, r8]

    subs    r3, r3, #0x1000000
    and     r7, lr, r7, lsr #23
    orr     r12,r12,r8, lsl #16

    ldrh    r8, [r2, r7]

    stmia   r0!, {r4,r5,r6,r12}
    bpl     vcloop_unaligned

    strh    r8, [r0]
    mov     r8, #0

    tst     r3, #0x10000
    add     r1, r1, #336             @ skip a line and 1 col
    addne   r1, r1, #64              @ skip more for 32col mode
    add     r0, r0, #640
    @add     r0, r0, #2
    addne   r0, r0, #128
    addeq   r3, r3, #0x27000000
    addne   r3, r3, #0x1F000000
    sub     r3, r3, #1
    tst     r3, #0xff
    bne     vcloop_unaligned

    ldmfd   sp!, {r4-r8,r12,lr}
    bx      lr




@ Convert 0000bbb0 ggg0rrr0
@ to      rrrrrggg gggbbbbb

@ r2,r3,r11 - scratch, lr = 0x001c001c, r8 = 0x00030003
.macro convRGB565 reg
    and     r2,   lr,   \reg,lsl #1
    and     r11,   r8,   \reg,lsr #2
    orr     r2,   r2,   r11           @ r2=red
    and     r3,   lr,   \reg,lsr #7
    and     r11,   r8,   \reg,lsr #10
    orr     r3,   r3,   r11           @ r3=blue
    and     \reg, \reg, lr,  lsl #3
    orr     \reg, \reg, \reg,lsl #3  @ green
    orr     \reg, \reg, r2,  lsl #11 @ add red back
    orr     \reg, \reg, r3           @ add blue back
.endm

.global _vidConvCpyRGB565

_vidConvCpyRGB565: @ void *to, void *from, int pixels
    stmfd   sp!, {r4-r8,r11-r12,lr}

    mov     r12, r2, lsr #3  @ repeats
    mov     lr, #0x001c0000
    orr     lr, lr,  #0x01c  @ lr == pattern 0x001c001c
    mov     r8, #0x00030000
    orr     r8, r8,  #0x003

.loopRGB565:
    ldmia   r1!, {r4-r7}
    subs    r12, r12, #1
    convRGB565 r4
    str     r4, [r0], #4
    convRGB565 r5
    str     r5, [r0], #4
    convRGB565 r6
    str     r6, [r0], #4
    convRGB565 r7
    str     r7, [r0], #4

    bgt     .loopRGB565

    ldmfd   sp!, {r4-r8,r11-r12,lr}
    bx      lr

@ utility
.global _blockcpy @ void *dst, void *src, size_t n

_blockcpy:
    stmfd   sp!, {r4,r5,r12}
    mov     r2, r2, lsr #4
blockcpy_loop:
    ldmia   r1!, {r3-r5,r12}
    subs    r2, r2, #1
    stmia   r0!, {r3-r5,r12}
    bne     blockcpy_loop
    ldmfd   sp!, {r4,r5,r12}
    bx      lr


.global _blockcpy_or @ void *dst, void *src, size_t n, int pat

_blockcpy_or:
    stmfd   sp!, {r4-r6,r12}
    orr     r3, r3, r3, lsl #8
    orr     r3, r3, r3, lsl #16
    mov     r2, r2, lsr #4
blockcpy_loop_or:
    ldmia   r1!, {r4-r6,r12}
    subs    r2, r2, #1
    orr     r4, r4, r3
    orr     r5, r5, r3
    orr     r6, r6, r3
    orr     r12,r12,r3
    stmia   r0!, {r4-r6,r12}
    bne     blockcpy_loop_or
    ldmfd   sp!, {r4-r6,r12}
    bx      lr
