PUBLIC AddTwoIntsAsm
PUBLIC ComputeHistogramAsm
PUBLIC ByteBoundAsm
PUBLIC NormalizeKernelAsm
PUBLIC ConvolvePixelAsm

.code

; ============================================================
; ComputeHistogramAsm - zoptymalizowana wersja SSE2
; ============================================================
; Parametry (Windows x64 calling convention):
;   RCX = data (const uint8_t*) tablica bajtów z ca³ego obrazka leci RGB
;   EDX = totalPixels (int)
;   R8D = channel (int)
;   R9D = channels (int)
;   [RSP+40] = outTable (int*)
; ============================================================

ComputeHistogramAsm PROC
    
    mov     r10, [rsp + 40]         ; R10 = outTable
    
    ; === Zerowanie tablicy (256 × 4 = 1024 bajtów) czyli 0 do 255 wartoœci pikseli i 4 bajty bo to integer (32 bity)
    pxor    xmm0, xmm0              ;zerowanie rejestru (taki xor tylko 128 bit)
    mov     eax, 64                 ; 1024 / 16 = 64 iteracje, ;dzielimy przez 16 bo xmm0 ma 16 bajtów (128 bitów) - bêd¹ nam potrzebne 64 iteracje by wyzerowaæ tablice
    mov     r11, r10                ;r11 wskaŸnik roboczy na tablice
    
zero_loop: ;pêtla zeruj¹ca 1024 bajtow¹ tablice histogramu
    movdqu  xmmword ptr [r11], xmm0  ; zapisuje 16 bitów z xmm0 do pamiêci pod r11 ptr po to by informowac o rozmiarze operacji
    add     r11, 16                 ;przesuwamy wskaŸnik o 16 bajtów
    dec     eax                     ;zmiejszamy licznik
    jnz     zero_loop                  ;wracamy az skonczymy
    
    ; === Przygotowanie g³ównej pêtli ===
    test    edx, edx                ;edx posiada total pixels - czyli ile jest do polczienia pikseli, test robi and ale nie zapisuje wartosc tu doputy nie policzy bêdzie siê wykonywa³o
    jle     done                    ; Jeœli totalPixels <= 0 skoczy je¿eli ju¿ nie ma pikseli do policzenia
    
    movsxd  rax, r8d               ; channel -> 64-bit ; przenosimy ale ze znakiem czylu rozszerzamy do 64 bitów. bo opareacje adresowwe potrzebuj¹ 64 bity
    add     rcx, rax               ; data += channel ; przesuwamy tak byœmy byli w odpowiednim kanale RGB
    movsxd  r8, r9d                ; channels -> 64-bit (krok) ; zapamiêtujemy o ile siê przesuwamy (tutaj o 3 bo rgb) równie¿ przechodzimy do 64 bitów
    
    ; === G³ówna pêtla (rozwiniêta 4×) ===
    mov     eax, edx               ;zapamiêtujê liczbê pikseli
    shr     eax, 2                 ; eax = totalPixels / 4 
    jz      remainder              ; Jeœli < 4 piksele ;jeœli liczba pikseli jest teraz mniejsza od zera skacz
    
    lea     r9, [r8 + r8]          ; r9 = 2 * channels ;drugi channell
    lea     r11, [r9 + r8]         ; r11 = 3 * channels ; trzeci channel (3*channel)
    
unrolled_4x:
    ; Piksel 0
    movzx   edx, byte ptr [rcx] ;branie jednego bajtu z pamiêci, jest to ten jeden piksej channel
    inc     dword ptr [r10 + rdx*4] ;inkrementacja odpowiedniego bajtu
    
    ; Piksel 1
    movzx   edx, byte ptr [rcx + r8] ;zapamiêtuje wartoœæ piksela, kana³u
    inc     dword ptr [r10 + rdx*4] ;adres pocz¹tku tablicy histogramu + rdx (dolne edx), czyli ++ dal tych samych wartoœci channel
    
    ; Piksel 2
    movzx   edx, byte ptr [rcx + r9] ;to samo tylko kolejny piksel
    inc     dword ptr [r10 + rdx*4]
    
    ; Piksel 3
    movzx   edx, byte ptr [rcx + r11] ;to samo tylko kolejny piksel
    inc     dword ptr [r10 + rdx*4]
    
    lea     rcx, [rcx + r8*4]      ; data += 4 * channels, przechodzimy 4 piksele
    dec     eax ;jedna iteracja skoñczona
    jnz     unrolled_4x ; je¿eli zosta³y iteracje kontynuujemy
    
    ; === Resztka (0-3 piksele) ===
remainder:
    mov     eax, [rsp + 48]        ; Odzyskaj totalPixels
    and     eax, 3                 ; totalPixels % 4 je¿eli nie ma pikseli to koniec
    jz      done
    
remainder_loop:
    movzx   edx, byte ptr [rcx] ;bierze i wpisuje wartoœæ kana³u piksela
    inc     dword ptr [r10 + rdx*4] ; ++dla  wartoœci
    add     rcx, r8 ;przesuniêcie na kolejny piksel
    dec     eax ;zmniejszamy liczbe pikseli co trzeba policzyc
    jnz     remainder_loop
    
done:
    ret

ComputeHistogramAsm ENDP

; ============================================================
; AddTwoIntsAsm
; ============================================================
AddTwoIntsAsm PROC
    lea     rax, [rcx + rdx]
    ret
AddTwoIntsAsm ENDP


; ============================================================
; ByteBoundAsm - SSE saturacja RAV - int v - czyli piksel musi byæ miedzy 0 - 255 ECX = v ; typ int 32 wiêc ecx
; ============================================================
ByteBoundAsm PROC
    
    xor     eax, eax            ; eax = 0 zerujemy
    cmp     ecx, eax            ; Porównaj z 0 zf = 1 jeœli mniejsze ustafia sf = 1 oraz cf=1, równe zf =1, wieksze wszystko 0
    cmovs   ecx, eax            ; Jeœli v < 0, v = 0 jeœci 
    mov     eax, 255            ;  255 do eax
    cmp     ecx, eax            ; porównanie czy v jest wieksze od 255
    cmovg   ecx, eax            ; Jeœli v > 255, v = 255
    test    ecx, ecx            ; Test na ujemne
     mov     al, cl              ; byœmy wynik zwracali jak batjt (uint8t)
    ret

ByteBoundAsm ENDP


; ============================================================
; NormalizeKernelAsm 
; ============================================================
; Parametry (Windows x64):
;   RCX = kernel (double*)
;   EDX = count (int)
;   XMM2 = sum (double) ;dla float/double double 8 bajtów ; xmm ma 16 bajtów
; ============================================================
NormalizeKernelAsm PROC
    
    ; SprawdŸ czy count > 0
    test    edx, edx  ;je¿eli count jest 0 lub mniejszy od zera to koniec
    jle     done
    
       
loop_start:
    movsd   xmm0, qword ptr [rcx]   ;  Za³aduj kernel[i]
    divsd   xmm0, xmm2               ; kernel[i] / sum normalizacja kernel/suma
    movsd   qword ptr [rcx], xmm0    ; Zapisz wynik
    
    add     rcx, 8                   ; Nastêpny element (double = 8 bajtów)
    dec     edx                      ; count--
    jnz     loop_start               ; Powtórz jeœli count != 0
    
done:
    ret

NormalizeKernelAsm ENDP



; 
; ConvolvePixelAsm - SSE2/SSE3
; 
; Parametry:
; RCX = data (const uint8_t*) 0 - 255 bito3w
; EDX = w (int) int 32 bity
; R8D = h (int)
; R9D = channels (int)
; [rsp+28h] = channel (int)
; [rsp+30h] = kernel (double*)
; [rsp+38h] = ker_size (int)
; [rsp+40h] = ker_center (int)
; [rsp+48h] = pos_x (int) teraŸniejszy wiersz!!!!!!!
; [rsp+50h] = pos_y (int) teraŸniejsza kolumna
; Zwraca: XMM0 = wynik (double)

ConvolvePixelAsm PROC

    ; pocz¹tek
    push    rbx  ;zapisujemy na stosie po to by nie zmieniaæ wartoœci co wesz³y - zachowujemy rejestry non volatile (nie ulotne) - nie œmieciowe musz¹ wróciæ takie jakie by³y kompilator móg³ gdzieœ tego uc¿yæ
    push    rsi
    push    rdi
    push    r12
    push    r13
    push    r14
    push    r15
    sub     rsp, 48 ;rezerwacja 48 bitów na stosie na zmienne lokalne

    ; === Za³aduj parametry === do non vlatile bezpieczne miejsce nie nadpisze siê
    mov     rbx, rcx                        ; data 
    mov     r12d, edx                       ; w
    mov     r13d, r8d                       ; h
    mov     r14d, r9d                       ; channels

    ; Parametry ze stosu (7*8 + 48 = 104 = 68h offset) h - szesnastokow ; dajemy z pamiêci do rejestru by dostêp by³ szybszy zmieni³y siê rejestry poniewa¿ zrobiliœmy 7 razy push i sub rsp,48 czylir ezerwacje 48 bitów na stosie
    mov     r15d, dword ptr [rsp+90h]       ; channel d jako dolna po³owa r15 czyli 32 bity, dword 4 bajty czyli 32 bity, rsp 90 to teraz czanel po przesuniêciu
    mov     rsi,  qword ptr [rsp+98h]       ; kernel ;  qwordod bo 64 bity - ka¿dy wskaŸnik w tej architekturze ma 64 bity
    mov     edi,  dword ptr [rsp+0A0h]      ; ker_size ; dword bo 32 bity, 4 bajty
    mov     r8d,  dword ptr [rsp+0A8h]      ; ker_center ;same
    mov     r9d,  dword ptr [rsp+0B0h]      ; pos_x ;same int
    mov     r10d, dword ptr [rsp+0B8h]      ; pos_y  ;same int

    ; Zapisz na stosie
    mov     [rsp], r9d                      ; pos_x  ;zapisanie bli¿ej potem bêdziemy korzystali z  r9d itp i moglibyœmy straciæ
    mov     [rsp+4], r10d                   ; pos_y
    mov     [rsp+8], r8d                    ; ker_center

    ; Przelicz sta³e
    mov     eax, r12d  ;
    imul    eax, r14d
    mov     [rsp+12], eax                   ; row_length = w * channels zapisujemy sobie d³ugoœæ wiersza 

    ; === Inicjalizuj akumulatory SSE ===
    xorpd   xmm0, xmm0                      ; suma g³ówna XOR Packed Double-precision,  potrzebne potem do przetwarzania 2x równoleg³ego
    xorpd   xmm5, xmm5                      ; suma pomocnicza - dla pojedynczych elementow po xmm5 jest non voletitle - nieulotne

    ; === FAST PATH CHECK === ;sprawdzamy czy kernel wychodzi poza granice
    mov     eax, r9d                        ; pos_x do eax - numer wiersza pixela który modyfiukjemy
    sub     eax, r8d                        ; pozycja piksela - ker_center (ile pikseli od krawêdzi do œrodka) numer wiersza - promien kernela
    js      slow_path                       ; pos_x - ker_center < 0 jezeli nie wychodzimy poza krawedz obrazu robimy szybka wersje
    
    mov     ecx, r9d                        ;zapamiêtujemy pozycje x - numer wiersza
    add     ecx, r8d                        ;dodajemy do niej promien kernela
    cmp     ecx, r13d                       ; je¿eli  przekraczamy iloœæ wierszy to ZF = 0, sf = 0 przy jge nast¹pi jmp, zf=0 gdy równe
    jge     slow_path                       ; pos_x + ker_center >= h jump greater or equal h zaczyna siê od zera wiêc gdy tu bêdzie zero jesteœmy ju¿ poza tablic¹
    
    mov     eax, r10d                       ; ³adujemy numer kolumny
    sub     eax, r8d                        ; odejmuje promein
    js      slow_path                       ; pos_y - ker_center < 0
    
    mov     ecx, r10d   ;to samo tylko kolumny
    add     ecx, r8d
    cmp     ecx, r12d
    jge     slow_path                       ; pos_y + ker_center >= w

    ;
    ; FAST PATH - bez sprawdzania granic 
  
    
    ; Oblicz bazowy indeks piksela czyli lewy górny róg tylko w 1 d () base_pixel = ((pos_x - ker_center) * w + (pos_y - ker_center)) * channels + channel
    mov     eax, r9d                        ; numer wiersza œrodki piksela srodka kernela
    sub     eax, r8d                        ; pos_x - ker_center obliczamy numer wiersza najbardziej górnego kernela
    imul    eax, r12d                       ; * w mnozymy razy ilosc kolumn
    add     eax, r10d                       ; dodajemy do tego kolumne na obrazie w której znajduje sie centrum kernela
    sub     eax, r8d                        ; i odejmujemy promien by zgadzalo sie z baseline
    imul    eax, r14d                       ; * channels musimy przemnozyc to przez liczbe kanalow (tablica jendowymiarowa to RGBRGBRGB)
    add     eax, r15d                       ; + channel i w zaleznosci w jakim kanale jestesmy doajemy channel (r - 0, g-1, b-2)
    mov     r11d, eax                       ; base_pixel pod r11d

    xor     r9d, r9d                        ; index tablicy kernela to 0 
    xor     r10d, r10d                      ; licznik wierszy kernela

fast_outer:
    cmp     r10d, edi                       ;porównanie licznik wierszy z wielkosci¹ kernela (ker_size)
    jge     fast_done                       ;Jeœli i >= ker_size, koniec pêtli

    xor     ecx, ecx                        ; j = 0

    ; Oblicz  pixel_row = base_pixel + (wiersz_kernela × row_length) //indeks pocz¹tkowy danego wiersza
    mov     eax, r10d                       ;zapisujemy licznik wierszy kerenla 
    imul    eax, [rsp+12]                   ; i * row_Length (w*channels opbliczne w sta³ych) ile musimy przeskoczyc pe³nych bajtów
    add     eax, r11d                       ; + base_pixel dodajemy wartosc bazowa - uwzglêdnia te¿ ona pozycjê kernela
    mov     [rsp+20], eax                   ; pixel_row adres startowy wiersza kernela (adres bazowego piksela w tablicy jednowymiarowej + (wiersz [0 - n-1]*width)) czyli bie¿¹ca pozycja

    ; Przetwarzaj 2 elementy na raz (SSE2)
    mov     edx, edi                        ;zamapiêtujemy wielkosc kernela
    shr     edx, 1                          ;ker_size / 2 bêdziemy przy jeden rotacji dawali dwa elementy kernela na raz )(shift right)

fast_inner_2x:
    test    edx, edx                        ;and tego czegoœ bez zapamiêtywania dopiero dla ostatniego piksela bedzie fast_inner_1x (czy s¹ jeszcze pary?)
    jz      fast_inner_1x                   ;Jeœli 0, skocz (brak par, ewentualnie reszta)

    ; Za³aduj 2 wartoœci kernela
    movupd  xmm1, xmmword ptr [rsi + r9*8] ; rsi - wskaŸnik na kernela (1wymiarowego), r9 index tablicy kernela - ³adujemy do xmm1 po 2 liczby ( 0 i 1, 2 i 3 itd) (³adujemy 16 bajtów czyli 2 double czyli kernel[x]i kernel [x+1]) MOVe Unaligned Packed Double 

    ; Za³aduj 2 piksele i konwertuj do double
    mov     eax, [rsp+20]                  ; bie¿¹ca pozycja
    movzx   r8d, byte ptr [rbx + rax]      ;  data (czyli tablica z pikselamu ) + bie¿¹ca pozycja, Move with Zero eXtend wype³nia góre zerami
    add     eax, r14d                      ;dodaj ilosc channels przesun jeden piksel
    movzx   ecx, byte ptr [rbx + rax]      ;kolejny piksel chwilowo zapisujemy
    add     eax, r14d                      ;kolejny piskel przeusniecei channels (3)
    mov     [rsp+20], eax                   ;zapisujemy pozycje

    ; Konwersja int -> double (SSE2)
    cvtsi2sd xmm2, r8d                      ;pixel 0 do dolnej czêsci 64 bitów  Convert Signed Integer to Scalar Double
    cvtsi2sd xmm3, ecx                      ;pixel 1
    unpcklpd xmm2, xmm3                     ; [pixel0, pixel1] pakowanie do jednego UNPaCK Low Packed Double dolna czesc xmm3 idzie teraz do gornej czesci xmm2

    ; Mno¿enie i akumulacja
    mulpd   xmm1, xmm2 ;wartoœæ z kernela * wartoœæ piksela
    addpd   xmm0, xmm1  ;dodaj do wyniku

    add     r9d, 2   ;indeks tablicy kernela zwieksza sie o 2 jako ze przerobilismy 2 piskele na raz
    dec     edx         ; zmniejszamy wielkosc kernela (a raczej kernela/2)
    jmp     fast_inner_2x

fast_inner_1x:
    ; SprawdŸ czy jest nieparzysta reszta
    test    edi, 1 ;ostatni bit jak parzysta zf = 1
    jz      fast_next_row ;leci do kolejnej wiersza

    ; Pojedynczy element
    movsd   xmm1, qword ptr [rsi + r9*8] ;pobieran wartos tablicy kernela czyli ptr na tablice + który element*8 do xmm1
    mov     eax, [rsp+20]   ;pobiera bie¿¹cc¹ pozycje w tablicy pikseli
    movzx   eax, byte ptr [rbx + rax] ;pocz¹tek + be¿¹ca pozycja pixel
    cvtsi2sd xmm2, eax ;³adujemy piksel do xmm2
    mulsd   xmm1, xmm2  ;mnoŸymy wartoœæ piksela (wartosc r lub g lub b) * kernela
    addsd   xmm5, xmm1 ; dodaj do wyniku ale tylko dolen basty
    inc     r9d ;inktrementujemy indeks tablicy kernela

fast_next_row:
    inc     r10d ;kolecjny wiersz
    jmp     fast_outer ;od pocz¹tku

fast_done:
    ; Zsumuj akumulatory (SSE3 haddpd)
    haddpd  xmm0, xmm0                      ; xmm0[0] = xmm0[0] + xmm0[1] czyli po znajduje siê suma obu w tym dla obu po³owek wynik taki sam Horizontal Add Packed Double
    addsd   xmm0, xmm5                      ; dodajemy dolne bajty z xmm5 Add Scalar Double
    jmp     epilog

    ; 
    ; SLOW PATH - ze sprawdzaniem granic
    ;
slow_path:
    mov     eax, [rsp+8]                    ; ker_center centum kernela (1 dal 3x3)
    neg     eax                             ; i = -ker_center
    mov     [rsp+16], eax                   ;zapsisujemy na stos ujemdn¹ wartoœæ centrum kernela bêdzie to zmienna pêtli od -ker_center do ker_cen

slow_outer:
    mov     eax, [rsp+16]                   ; i
    cmp     eax, [rsp+8]                    ; i <= ker_center?
    jg      epilog                          ;jak wiêksze to pêtla skonczona for(-ker_cen ..... do ker cent (<=))

    ; position_x = pos_x + i
    mov     ecx, [rsp] ;œrodek kernela wiersz
    add     ecx, eax  ;ktory wiersz teraz
    
    ; SprawdŸ granice X
    test    ecx, ecx    ;je¿eli poza granicami obrazu (od góry) jazda do lokejnego czyli ecx<0 
    js      slow_next_i
    cmp     ecx, r13d   ;je¿eli poza granicami obrazu (od do³u) jazda do lokejnego czyli ecx<0 
    jge     slow_next_i
    
    mov     r10d, ecx                       ; position_x zachowanie pozycji bo poprawna
    
    ; j = -ker_center
    mov     ecx, [rsp+8] ;do pêtli -ker do +ker 
    neg     ecx

slow_inner:
    cmp     ecx, [rsp+8] ; jesli "j" wieksze od ker center koniec petli wewnetrznej
    jg      slow_next_i

    ; position_y = pos_y + j
    mov     edx, [rsp+4] ;srodek kernela w obrazie (piksela raczej) - w sps+4 pozycja y w obrazie pixela nad którym pracujemy
    add     edx, ecx ;prawdziwa kolumna piksela co siê zajmujemy
    
    ; SprawdŸ granice Y
    test    edx, edx ; sprawdzczy mniejsza od zera jesli tak to nastepna kolumna
    js      slow_next_j
    cmp     edx, r12d ; sprawdz wieksze od ilosci kolumn jesli ta to skacz
    jge     slow_next_j

    mov     r11d, edx                       ; position_y nie potrzeben
   

    ; kernel_idx = (i + ker_center) * ker_size + (j + ker_center) ; knel 2d na 1d
    mov     eax, [rsp+16]  ;zapisujemy tu i czyli wiersz pixela w kernelu
    add     eax, [rsp+8] ; dodajemy centrum kernela - promien
    imul    eax, edi     ; * ker_size
    add     eax, ecx      ; +j
    add     eax, [rsp+8]    ;ker_center
    
    ; Za³aduj kernel
    movsd   xmm1, qword ptr [rsi + rax*8] ;move scalar double - ³adowanie wartosci kernela, pobieran wartos tablicy kernela czyli ptr na tablice + który element*8 do xmm1, RSI = adres tablicy kernel, rax indekst w kernelu

    ; pixel_idx = (position_x * w + position_y) * channels + channel ;pixel na którym chcemy u¿yæ do odczytu splotu
    mov     eax, r10d ;wiersz
    imul    eax, r12d ;*ilosc kolumn
    add     eax, r11d ;+kolumna
    imul    eax, r14d ;ilosc kanalow
    add     eax, r15d ;kanal wybrany tu
    
    ; Za³aduj piksel
    movzx   eax, byte ptr [rbx + rax] ; £adowanie piksela z pamiêci - Move with Zero eXtend wype³nia górne wartoœci zerami, wskaŸnik na dane obrazu + pixel obliczony przed chwila
    cvtsi2sd xmm2, eax ;konwersja na double Convert Signed Integer to Scalar Double

    ; A += kernel * pixel
    mulsd   xmm1, xmm2 ;kernel razy piksel, xmm1 wartosc w kernelu, xmm2 - wartosc pixela
    addsd   xmm0, xmm1 ;do opowiedzi dodajemy

  

slow_next_j: ;kolejna kolumna
    inc     ecx;j++
    jmp     slow_inner ;powrot petla wewnetrzna

slow_next_i: ;kolejny wiersz
    inc     dword ptr [rsp+16] ;i++(na stosie)
    jmp     slow_outer ;powrot petla zewnetrzna

    ; 
    ; EPILOG
    ;
epilog: ;zworcenie wartosci nonvolatile
    add     rsp, 48
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rdi
    pop     rsi
    pop     rbx
    ret

ConvolvePixelAsm ENDP

END