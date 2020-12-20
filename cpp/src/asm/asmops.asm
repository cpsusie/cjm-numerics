;
; Copyright (C) 2020 by Jesper G. Mikkelsen
;
; https://github.com/JesperMikkelsen/Big-Numbers/tree/master/Lib/Src/Math
;
; Permission is hereby granted, free of charge, to any person obtaining a copy of this
; software and associated documentation files (the "Software"), to deal in the Software
; without restriction, including without l> imitation the rights to use, copy, modify, merge,
; publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to
; whom the Software is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all copies or
; substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
; EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
; HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
; IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
; IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.
;
; These functions implements the basic operations for _int128 type
; running on 64-bit intel CPU.
; They are (almost) identical to MS's workhorse-functions that do
; __int64-math using 32-bit registers. Every memory reference that uses
; dword ptr(4 byte) are replaced by qword ptr(8 byte), and registernames
; eax, ebx, ecx... are replaced by rax, rbx, rcx etc.
; Parameters are passed in rcx(1st argument) and rdx(2nd argument) and
; return value is in rax, where the runtime-stack is used in x86 mode.
; So the code is modified to reflect this environment
;

.CODE

;void int128add(_int128 &dst, const _int128 &x); do assignop dst += x;
int128add PROC
    mov         rax, qword ptr[rdx]
    add         qword ptr[rcx], rax
    mov         rax, qword ptr[rdx+8]
    adc         qword ptr[rcx+8], rax
    ret
int128add ENDP

;void int128sub(_int128 &dst, const _int128 &x); do assignop dst -= x;
int128sub PROC
    mov         rax, qword ptr[rdx]
    sub         qword ptr[rcx], rax
    mov         rax, qword ptr[rdx+8]
    sbb         qword ptr[rcx+8], rax
    ret
int128sub ENDP

;void int128mul(_int128 &dst, const _int128 &x); do assignop dst *= x;
int128mul PROC
    push        rbx
    mov         rax, qword ptr[rdx+8]          ; rax  = x.hi
    mov         rbx, qword ptr[rcx+8]          ; rbx  = dst.hi
    or          rbx, rax                       ; rbx |= x.hi
    mov         rbx, qword ptr[rcx]            ; rbx  = dst.lo
    jne         Hard                           ; if(x.hi|dst.hi) goto Hard
                                               ; else simple int64 multiplication
    mov         rax, qword ptr[rdx]            ; rax  = x.lo
    mul         rbx                            ; rdx:rax = rax * rbx
    mov         qword ptr[rcx]  , rax          ; dst.lo = rax
    mov         qword ptr[rcx+8], rdx          ; dst.hi = rdx
    pop         rbx
    ret
Hard:                                          ; Assume rax==x.hi, rbx==dst.lo
    push        rsi
    mov         rsi, rdx                       ; need rdx for highend of mul, so rsi = &x
    mul         rbx                            ; rdx:rax = x.hi*dst.lo
    mov         r9 , rax                       ; r9      = lo(x.hi*dst.lo)
    mov         rax, qword ptr[rsi]            ; rax     = x.lo
    mul         qword ptr[rcx+8]               ; rdx:rax = x.lo*dst.hi
    add         r9, rax                        ; r9     += lo(x.lo*dst.hi);
    mov         rax, qword ptr[rsi]            ; rax     = x.lo
    mul         rbx                            ; rdx:rax = x.lo*dst.lo
    add         rdx, r9                        ; rdx    += lo(x.hi*dst.lo) + lo(x.lo*dst.hi)
    mov         qword ptr[rcx]  , rax
    mov         qword ptr[rcx+8], rdx
    pop         rsi
    pop         rbx
    ret
int128mul ENDP

;void int128div(_int128 &dst, _int128 &x); do assignop dst /= x; if (x < 0) x = -x; !!
int128div PROC
    push        rbx                            ;
    push        rdi                            ;
    push        rsi                            ;
    mov         r8, rcx                        ; r8  = &dst
    mov         r9, rdx                        ; r9  = &x
    xor         edi, edi                       ; edi = 0
    mov         rax, qword ptr[r8+8]           ; rax = dst.hi
    or          rax, rax                       ;
    jge         DSTPOSITIVE                    ; if(dst<0) {
    inc         edi                            ;   edi++
    mov         rdx, qword ptr[r8]             ;   rax:rdx==dst
    neg         rax                            ;   dst = -dst
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r8+8], rax           ;
    mov         qword ptr[r8], rdx             ; }
DSTPOSITIVE:                                   ; Assume dst>=0, edi==[original dst<0]
    mov         rax, qword ptr[r9+8]           ;
    or          rax, rax                       ;
    jge         UIDIV                          ; if(x<0) {
    dec         edi                            ;   edi--
    mov         rdx, qword ptr[r9]             ;   rax:rdx==x
    neg         rax                            ;   x = -x
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r9+8], rax           ;
    mov         qword ptr[r9], rdx             ; }
UIDIV:                                         ; Assume dst>=0, rax==x.hi, x>0, edi==0 <=> #negative arguments is even
    or          rax, rax                       ;
    jne         L1                             ; if(x.hi==0) {
    mov         rcx, qword ptr[r9]             ;   rcx     = x.lo (==x)
    mov         rax, qword ptr[r8+8]           ;   rax     = dst.hi
    xor         rdx, rdx                       ;   rax:rdx = dst.hi:0
    div         rcx                            ;   rax:rdx = (dst.hi / x):(dst.hi % x)
    mov         rbx, rax                       ;   rbx     = q.hi
    mov         rax, qword ptr[r8]             ;   rdx:rax = (dst.hi % x):dst.lo
    div         rcx                            ;   rax     = q.lo, rdx==(dst % x)(ignore)
    mov         rdx, rbx                       ;   rdx:rax==|quotient|
    jmp         L5                             ; }
L1: bsr         rcx, rax                       ; Assume rax==x.hi
    cmp         cl, 63                         ;
    je          MRS                            ; if(cl<63) {
    inc         cl                             ;   cl = #positions to shift both operands, to make x.hi==0 (cl<64)
    mov         rbx, qword ptr[r9]             ;   rax:rbx==x
    shrd        rbx, rax, cl                   ;   rshift x cl positions, x.hi==0
    mov         rdx, qword ptr[r8+8]           ;
    mov         rax, qword ptr[r8]             ;   rdx:rax==dst
    shrd        rax, rdx, cl                   ;   rshift dst cl positions, filling open bitpositions with 0
    shr         rdx, cl                        ;
    jmp         Divide                         ; } else {
MRS:mov         rbx, rax                       ;   Manual 64 bit rshift both operands
    mov         rax, qword ptr[r8+8]           ;
    xor         rdx, rdx                       ; }
Divide:                                        ; Assume rdx:rax==dst shifted, rbx==(x shifted).lo (x.hi==0)
    div         rbx                            ; rax = q, rdx = remainder(ignore)
    mov         rsi, rax                       ; rsi = q, (quotient==q or q-1), q.hi==0
    mul         qword ptr[r9+8]                ; rdx:rax = q * x.hi
    mov         rcx, rax                       ; rcx = (q*x.hi).lo
    mov         rax, qword ptr[r9]             ; rax = x.lo
    mul         rsi                            ; rdx:rax = x.lo * q
    add         rdx, rcx                       ; rdx:rax = t = q * x
    jb          L3                             ; if(t<0) quotient==q-1 (signed overflow)
    cmp         rdx, qword ptr[r8+8]           ;
    ja          L3                             ; if(t.hi>dst.hi) quotient==q-1
    jb          L4                             ;
    cmp         rax, qword ptr[r8]             ; hi words equals, now compare lo words
    jbe         L4                             ; if(t.lo>dst.lo) => quotient==q-1
L3: dec         rsi                            ;   q--
L4: xor         rdx, rdx                       ;
    mov         rax, rsi                       ; rdx:rax==|quotient|
L5: or          edi, edi                       ; set sign of dst occording to normal product sign rule
    je          L2                             ; if(edi!=0) {
    neg         rdx                            ;   quotient = -quotient
    neg         rax                            ;
    sbb         rdx, 0                         ; }
L2: pop         rsi                            ; Assume rdx:rax==quotient, restore stack
    pop         rdi                            ;
    pop         rbx                            ;
    mov         qword ptr[r8], rax             ; save quotient in dst
    mov         qword ptr[r8+8], rdx           ;
    ret                                        ;
int128div ENDP

;void int128rem(_int128 &dst, _int128 &x); do assignop dst %= x; if(x<0) x = -x; !!
int128rem PROC
    push        rbx                            ;
    push        rdi                            ;
    mov         r8, rcx                        ; r8  = &dst
    mov         r9, rdx                        ; r9  = &x
    xor         edi, edi                       ; edi = 0
    mov         rax, qword ptr[r8+8]           ; rax = dst.hi
    or          rax, rax                       ;
    jge         DSTPOSITIVE                    ; if(dst<0) {
    inc         edi                            ;   edi++
    mov         rdx, qword ptr[r8]             ;   rax:rdx==dst
    neg         rax                            ;   dst = -dst
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r8+8], rax           ;
    mov         qword ptr[r8], rdx             ; }
DSTPOSITIVE:                                   ; Assume dst>=0, edi==[original dst<0]
    mov         rax, qword ptr[r9+8]           ;
    or          rax, rax                       ;
    jge         UIREM                          ; if(x<0) {
    mov         rdx, qword ptr[r9]             ;   rax:rdx==x
    neg         rax                            ;   x = -x
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r9+8], rax           ;
    mov         qword ptr[r9], rdx             ; }
UIREM:                                         ; Assume dst>=0, rax==x.hi, x>0, edi==[original dst<0]. Dont care about orignal sign of x
    or          rax, rax                       ;
    jne         L1                             ; if(x.hi==0) {
    mov         rcx, qword ptr[r9]             ;   rcx     = x.lo (==x)
    mov         rax, qword ptr[r8+8]           ;   rax     = dst.hi
    xor         rdx, rdx                       ;   rax:rdx = dst.hi:0
    div         rcx                            ;   rax:rdx = (dst.hi / x):(dst.hi % x)
    mov         rax, qword ptr[r8]             ;   rdx:rax = (dst.hi % x):dst.lo
    div         rcx                            ;   rax==quotient(ignore), rdx==|final remainder|
    mov         rax, rdx                       ;
    xor         rdx, rdx                       ;   rdx:rax==0:|remainder|
    or          edi, edi                       ;
    jne         L5                             ;   if(edi!=0) change sign of remainder
    jmp         L2                             ; }

L1: bsr         rcx, rax                       ; Assume rax==x.hi
    cmp         cl, 63                         ;
    je          MRS                            ; if(cl<63) {
    inc         cl                             ;   cl = #positions to shift both operands, to make x.hi==0 (cl<64)
    mov         rbx, qword ptr[r9]             ;   rax:rbx==x
    shrd        rbx, rax, cl                   ;   rshift x cl positions, x.hi==0
    mov         rdx, qword ptr[r8+8]           ;
    mov         rax, qword ptr[r8]             ;   rdx:rax==dst
    shrd        rax, rdx, cl                   ;   rshift dst cl positions, filling open bitpositions with 0
    shr         rdx, cl                        ;
    jmp         Divide                         ; } else {
MRS:mov         rbx, rax                       ;   Manual 64 bit rshift both operands
    mov         rax, qword ptr[r8+8]           ;
    xor         rdx, rdx                       ; }
Divide:                                        ; Assume rdx:rax==dst shifted, rbx==(x shifted).lo (x.hi==0)
    div         rbx                            ; rax = q, rdx = remainder(ignore)
    mov         rcx, rax                       ; rcx = q, (quotient==q or q-1), q.hi==0
    mul         qword ptr[r9+8]                ; rdx:rax = q * x.hi
    xchg        rax, rcx                       ; rcx = (q*x.hi).lo, rax = q
    mul         qword ptr[r9]                  ; rdx:rax = q * x.lo
    add         rdx, rcx                       ; rdx:rax = t = q * x
    jb          L3                             ; if(t<0) quotient==q-1 (signed overflow)
    cmp         rdx, qword ptr[r8+8]           ;
    ja          L3                             ; if(t.hi>dst.hi) quotient==q-1
    jb          L4                             ;
    cmp         rax, qword ptr[r8]             ; hi words equals, now compare lo words
    jbe         L4                             ; if(t.lo>dst.lo) => quotient==q-1
L3: sub         rax, qword ptr[r9]             ; Assume rdx:rax==t, t==q*x==(quotient+1)*x
    sbb         rdx, qword ptr[r9+8]           ; t -= x
L4: sub         rax, qword ptr[r8]             ; Assume rdx:rax==t, t==quotient*x
    sbb         rdx, qword ptr[r8+8]           ; calculate rem = dst - t (==-|remainder|)
    or          edi, edi                       ;
    jne         L2                             ; if(edi!=0) remainder==--(rem)==rem
L5: neg         rdx                            ; Assume rdx:rax==rem==-remainder.
    neg         rax                            ; remainder = -remainder
    sbb         rdx, 0                         ;
L2: pop         rdi                            ; Assume rdx:rax==remainder, restore stack
    pop         rbx                            ;
    mov         qword ptr[r8], rax             ; save remainder in dst
    mov         qword ptr[r8+8], rdx           ;
    ret                                        ;
int128rem ENDP

;void int128neg(_int128 &x); set x = -x;
int128neg PROC
    neg         qword ptr[rcx + 8]
    neg         qword ptr[rcx]
    sbb         qword ptr[rcx + 8], 0
    ret
int128neg ENDP

;void int128inc(_int128 &x); set x = x + 1;
int128inc PROC
    add         qword ptr[rcx], 1
    adc         qword ptr[rcx+8], 0
    ret
int128inc ENDP

;void int128dec(_int128 &x); set x = x - 1;
int128dec PROC
    sub         qword ptr[rcx], 1
    sbb         qword ptr[rcx+8], 0
    ret
int128dec ENDP

;void int128shr(int shft, _int128 &x); do assignop x >>= shft; (if(x<0) shift 1-bits in from left, else 0-bits)
int128shr PROC
    mov         rax, qword ptr[rdx+8]          ; rax = x.hi
    cmp         cl, 40h
    jae         More64                         ; if(cl>=64) goto More64;
    sar         qword ptr[rdx+8], cl           ; shift x.hi
    shrd        qword ptr[rdx], rax, cl        ; shift x.lo taking new bits from x.hi (rax)
    ret
More64:                                        ; Assume rax==x.hi
    cmp         cl, 80h
    jae         RetSign                        ; if(cl>=128) goto RetSign;
    sar         qword ptr[rdx+8], 3Fh          ; set all bits in x.hi to sign-bit
    and         cl, 3Fh                        ; cl %= 64
    sar         rax, cl                        ; rax = x.hi >> cl
    mov         qword ptr[rdx], rax            ; x.lo = rax
    ret
RetSign:                                       ; Assume rax==x.hi
    sar         rax, 3Fh                       ; set all bits in rax to sign-bit
    mov         qword ptr[rdx], rax
    mov         qword ptr[rdx+8], rax
    ret
int128shr ENDP

;void int128shl(int shft, _int128 &x); do assignop x <<= shft;
int128shl PROC
    cmp         cl, 40h
    jae         More64                         ; if(cl>=64) goto More64;
    mov         rax, qword ptr[rdx]            ; rax = x.lo
    shl         qword ptr[rdx], cl             ; shift x.lo
    shld        qword ptr[rdx+8], rax, cl      ; shift x.hi taking new bits from x.lo (rax)
    ret
More64:
    cmp         cl, 80h
    jae         RetZero                        ; if(cl>=128) goto RetZero;
    and         cl, 3Fh                        ; cl %= 64
    mov         rax, qword ptr[rdx]            ; rax = x.lo
    shl         rax, cl                        ; shift rax (x.lo)
    mov         qword ptr[rdx+8], rax          ; x.hi = rax
    xor         rax, rax
    mov         qword ptr[rdx], rax            ; x.lo = 0
    ret
RetZero:
    xor         rax, rax                       ; return 0
    mov         qword ptr[rdx], rax
    mov         qword ptr[rdx+8], rax
    ret
int128shl ENDP

;int int128cmp(const _int128 &x1, const _int128 &x2); return sign(x1 - x2);
int128cmp PROC
    mov         rax, qword ptr[rcx+8]          ; x1.hi
    cmp         rax, qword ptr[rdx+8]          ; x2.hi
    jl          LessThan                       ; signed compare of x1.hi and x2.hi
    jg          GreaterThan
    mov         rax, qword ptr[rcx]            ; x1.lo
    cmp         rax, qword ptr[rdx]            ; x2.lo
    jb          LessThan                       ; unsigned compare of x1.lo and x2.lo
    ja          GreaterThan
    xor         eax, eax                       ; they are equal
    ret
GreaterThan:
    mov         eax, 1
    ret
LessThan:
    mov         eax, -1
    ret
int128cmp ENDP

;void uint128div(_uint128 &dst, const _uint128 &x); do assignop dst /= x;
uint128div PROC
    push        rbx                            ; same as signed division without sign check on arguments
    push        rsi                            ;
    mov         r8, rcx                        ; r8  = &dst
    mov         r9, rdx                        ; r9  = &x
    mov         rax, qword ptr[r9+8]           ; rax = x.hi
    or          rax, rax                       ;
    jne         L1                             ; if(x.hi==0) {
    mov         rcx, qword ptr[r9]             ;   rcx     = x.lo (==x)
    mov         rax, qword ptr[r8+8]           ;   rax     = dst.hi
    xor         rdx, rdx                       ;   rax:rdx = dst.hi:0
    div         rcx                            ;   rax:rdx = (dst.hi / x):(dst.hi % x)
    mov         rbx, rax                       ;   rbx     = q.hi
    mov         rax, qword ptr[r8]             ;   rdx:rax = (dst.hi % x):dst.lo
    div         rcx                            ;   rax     = q.lo, rdx==(dst % x)(ignore)
    mov         rdx, rbx                       ;   rdx:rax = quotient
    jmp         L2                             ; }
L1: bsr         rcx, rax                       ; Assume rax==x.hi
    cmp         cl, 63                         ;
    je          MRS                            ; if(cl<63) {
    inc         cl                             ;   cl = #positions to shift both operands, to make x.hi==0 (cl<64)
    mov         rbx, qword ptr[r9]             ;   rax:rbx==x
    shrd        rbx, rax, cl                   ;   rshift x cl positions, x.hi==0
    mov         rdx, qword ptr[r8+8]           ;
    mov         rax, qword ptr[r8]             ;   rdx:rax==dst
    shrd        rax, rdx, cl                   ;   rshift dst cl positions, filling open bitpositions with 0
    shr         rdx, cl                        ;
    jmp         Divide                         ; } else {
MRS:mov         rbx, rax                       ;   Manual 64 bit rshift both operands
    mov         rax, qword ptr[r8+8]           ;
    xor         rdx, rdx                       ; }
Divide:                                        ; Assume rdx:rax==dst shifted, rbx==(x shifted).lo (x.hi==0)
    div         rbx                            ; rax = q, rdx = remainder(ignore)
    mov         rsi, rax                       ; rsi = q, (quotient==q or q-1), q.hi==0
    mul         qword ptr[r9+8]                ; rdx:rax = q * x.hi
    mov         rcx, rax                       ; rcx = (q*x.hi).lo
    mov         rax, qword ptr[r9]             ; rax = x.lo
    mul         rsi                            ; rdx:rax = x.lo * q
    add         rdx, rcx                       ; rdx:rax = t = q * x
    jc          L3                             ; if(carry) quotient==q-1 (unsigned overflow)
    cmp         rdx, qword ptr[r8+8]           ;
    ja          L3                             ; if(t.hi>dst.hi) quotient==q-1
    jb          L4                             ;
    cmp         rax, qword ptr[r8]             ; hi words equals, now compare lo words
    jbe         L4                             ; if(t.lo>dst.lo) => quotient==q-1
L3: dec         rsi                            ; q--
L4: xor         rdx, rdx                       ;
    mov         rax, rsi                       ; rdx:rax==0:quotient
L2: pop         rsi                            ; Assume rdx:rax==quotient, restore stack
    pop         rbx                            ;
    mov         qword ptr[r8], rax             ; save quotient in dst
    mov         qword ptr[r8+8], rdx           ;
    ret                                        ;
uint128div ENDP

; calculates unsigned remainder
;void uint128rem(_uint128 &dst, const _uint128 &x); do assignop dst %= x;
uint128rem PROC
    push        rbx                            ; same as signed modulus without sign check on arguments
    mov         r8, rcx                        ; r8  = &dst
    mov         r9, rdx                        ; r9  = &x
    mov         rax, qword ptr[r9+8]           ; rax = x.hi
    or          rax, rax                       ;
    jne         L1                             ; if(x.hi==0) {
    mov         rcx, qword ptr[r9]             ;   rcx     = x.lo (==x)
    mov         rax, qword ptr[r8+8]           ;   rax     = dst.hi
    xor         rdx, rdx                       ;   rax:rdx = dst.hi:0
    div         rcx                            ;   rax:rdx = (dst.hi / x):(dst.hi % x)
    mov         rax, qword ptr[r8]             ;   rdx:rax = (dst.hi % x):dst.lo
    div         rcx                            ;   rax     = quotient(ignore), rdx = final remainder
    mov         rax, rdx                       ;
    xor         rdx, rdx                       ;   rdx:rax = 0:remainder
    jmp         L2                             ; }
L1: bsr         rcx, rax                       ; Assume rax==x.hi
    cmp         cl, 63                         ;
    je          MRS                            ; if(cl<63) {
    inc         cl                             ;   cl = #positions to shift both operands, to make x.hi==0 (cl<64)
    mov         rbx, qword ptr[r9]             ;   rax:rbx==x
    shrd        rbx, rax, cl                   ;   rshift x cl positions, x.hi==0
    mov         rdx, qword ptr[r8+8]           ;
    mov         rax, qword ptr[r8]             ;   rdx:rax==dst
    shrd        rax, rdx, cl                   ;   rshift dst cl positions, filling open bitpositions with 0
    shr         rdx, cl                        ;
    jmp         Divide                         ; } else {
MRS:mov         rbx, rax                       ;   Manual 64 bit rshift both operands
    mov         rax, qword ptr[r8+8]           ;
    xor         rdx, rdx                       ; }
Divide:                                        ; Assume rdx:rax==dst shifted, rbx==(x shifted).lo (x.hi==0)
    div         rbx                            ; rax     = q, rdx = remainder(ignore)
    mov         rcx, rax                       ; rcx     = q, (quotient==q or q-1), q.hi==0
    mul         qword ptr[r9+8]                ; rdx:rax = q * x.hi
    xchg        rax, rcx                       ; rcx     = (q*x.hi).lo, rax = q
    mul         qword ptr[r9]                  ; rdx:rax = q * x.lo
    add         rdx, rcx                       ; rdx:rax = t = q * x
    jc          L3                             ; if(carry) quotient==q-1 (unsigned overflow)
    cmp         rdx, qword ptr[r8+8]           ;
    ja          L3                             ; if(t.hi>dst.hi) quotient==q-1
    jb          L4                             ;
    cmp         rax, qword ptr[r8]             ; hi words equals, now compare lo words
    jbe         L4                             ; if(t.lo>dst.lo) => quotient==q-1
L3: sub         rax, qword ptr[r9]             ; Assume rdx:rax==t, t==q*x==(quotient+1)*x
    sbb         rdx, qword ptr[r9+8]           ; t -= x
L4: sub         rax, qword ptr[r8]             ; Assume rdx:rax==t, t==quotient*x
    sbb         rdx, qword ptr[r8+8]           ; calculate rem = -(t-=dst) (==dst-t)
    neg         rdx                            ;
    neg         rax                            ;
    sbb         rdx, 0                         ; rdx:rax==rem
L2: pop         rbx                            ; Assume rdx:rax==remainder, restore stack
    mov         qword ptr[r8], rax             ; save remainder in dst
    mov         qword ptr[r8+8], rdx           ;
    ret                                        ;
uint128rem ENDP

;typedef struct {
;  _uint128 quot;
;  _uint128 rem;
;} _ui128div_t;
;
;void uint128quotrem(const _uint128 &numer, const _uint128 &denom, _ui128div_t *qr);
uint128quotrem PROC
    push        rbx                            ; same as signed division
    push        rsi                            ; but without sign check on arguments
    push        rdi
    mov         rdi, r8                        ; rdi = qr
    mov         r8, rcx                        ; r8  = &num
    mov         r9, rdx                        ; r9  = &denom
    mov         rax, qword ptr[r9+8]           ; rax = denom.hi
    or          rax, rax                       ;
    jne         L1                             ; if(denom.hi==0) {
    mov         rcx, qword ptr[r9]             ;   rcx     = denom.lo (==denom)
    mov         rax, qword ptr[r8+8]           ;   rax     = numer.hi
    xor         rdx, rdx                       ;   rdx:rax = 0:numer.hi
    div         rcx                            ;   rax     = q.hi. rdx = remainder.hi
    mov         qword ptr[rdi+8], rax          ;   save q.hi
    mov         rax, qword ptr[r8]             ;   rdx:rax = remainder:lo word of dividend
    div         rcx                            ;   rax:rdx = q.lo:remainder
    mov         qword ptr[rdi], rax            ;   save q.lo
    mov         qword ptr[rdi+10h], rdx        ;   save remainder.lo
    xor         rax, rax                       ;
    mov         qword ptr[rdi+18h], rax        ;   remainder.hi = 0
    jmp         L2                             ; }
L1: bsr         rcx, rax                       ; Assume rax==denom.hi
    cmp         cl, 63                         ;
    je          MRS                            ; if(cl<63) {
    inc         cl                             ;   cl = #positions to shift both operands, to make denom.hi==0
    mov         rbx, qword ptr[r9]             ;   rax:rbx==denom
    shrd        rbx, rax, cl                   ;   rshift denom cl positions, denom.hi==0
    mov         rdx, qword ptr[r8+8]           ;
    mov         rax, qword ptr[r8]             ;   rdx:rax==dst (numer)
    shrd        rax, rdx, cl                   ;   rshift numer cl positions, filling open bitpositions with 0
    shr         rdx, cl                        ;
    jmp         Divide                         ; } else {
MRS:mov         rbx, rax                       ;   Manual 64 bit rshift both operands
    mov         rax, qword ptr[r8+8]           ;
    xor         rdx, rdx                       ; }
Divide:                                        ; Assume rdx:rax==numer shifted, rbx==(denom shifted).lo (denom.hi=0)
    div         rbx                            ; rax = q, rdx = remainder(ignore)
    mov         rsi, rax                       ; rsi = q, (quotient==q or q-1) q.hi==0
    mul         qword ptr[r9+8]                ; rdx:rax = q *= denom.hi
    mov         rcx, rax                       ; rcx = (q*denom.hi).lo
    mov         rax, qword ptr[r9]             ; rax = denom.lo
    mul         rsi                            ; rdx:rax = denom.lo * q
    add         rdx, rcx                       ; rdx:rax = t = q * denom
    jc          L3                             ; if(carry) quotient==q-1 (unsigned overflow)
    cmp         rdx, qword ptr[r8+8]           ;
    ja          L3                             ; if(t.hi > numer.hi) quotient==q-1
    jb          L4                             ;
    cmp         rax, qword ptr[r8]             ; hi words equals, now compare lo words
    jbe         L4                             ; if(t.lo>numer.lo) quotient==q-1
L3:                                            ; Assume rsi==q, rdx:rax==t==q*denom, quotient==q-1
    dec         rsi                            ; q--
    sub         rax, qword ptr[r9]             ; t -= denom
    sbb         rdx, qword ptr[r9+8]           ;
L4: sub         rax, qword ptr[r8]             ; t -= numer
    sbb         rdx, qword ptr[r8+8]           ;
    neg         rdx                            ; t = -t
    neg         rax                            ;
    sbb         rdx, 0                         ; rsi = quotient, rdx:rax = remainder
    mov         qword ptr[rdi], rsi            ; rq->quot.lo = rsi
    xor         rbx, rbx                       ;
    mov         qword ptr[rdi+8], rbx          ; rq->quot.hi = 0
    mov         qword ptr[rdi+10h], rax        ; rq->rem.lo  = rax
    mov         qword ptr[rdi+18h], rdx        ; rq->rem.hi  = rdx
L2: pop         rdi                            ; restore stack
    pop         rsi
    pop         rbx
    ret
uint128quotrem ENDP

;void uint128shr(int shft, void *x); do assignop x >>= shft. always shift 0-bits in from left
uint128shr PROC
    cmp         cl, 40h
    jae         More64                         ; if(cl >= 64) goto More64;
    mov         rax, qword ptr[rdx+8]          ; rax = x.hi
    shr         qword ptr[rdx+8], cl           ; shift x.hi
    shrd        qword ptr[rdx], rax, cl        ; shift x.lo taking new bits from x.hi (rax)
    ret
More64:
    cmp         cl, 80h
    jae         RetZero                        ; if(cl >= 128) goto RetZero;
    and         cl, 3Fh                        ; cl %= 64
    mov         rax, qword ptr[rdx+8]          ; rax = x.hi
    shr         rax, cl                        ; rax >>= cl
    mov         qword ptr[rdx], rax            ; x.lo = rax
    xor         rax, rax
    mov         qword ptr[rdx+8], rax          ; x.hi = 0
    ret
RetZero:
    xor         rax, rax                       ; return 0
    mov         qword ptr[rdx], rax
    mov         qword ptr[rdx+8], rax
    ret
uint128shr ENDP

;int uint128cmp(const _uint128 &x1, const _uint128 &x2); return sign(x1 - x2);
uint128cmp PROC
    mov         rax, qword ptr[rcx+8]          ; x1.hi
    cmp         rax, qword ptr[rdx+8]          ; x2.hi
    jb          LessThan                       ; unsigned compare of x1.hi and x2.hi
    ja          GreaterThan
    mov         rax, qword ptr[rcx]            ; x1.lo
    cmp         rax, qword ptr[rdx]            ; x2.lo
    jb          LessThan                       ; unsigned compare of x1.lo and x2.lo
    ja          GreaterThan
    xor         eax, eax                       ; they are equal
    ret
GreaterThan:
     mov        eax, 1
     ret
LessThan:
     mov        eax, -1
     ret
uint128cmp ENDP

END
