#ifndef morse_h
#define morse_h

#include <stdint.h>


/* 
ASCII Code Assignments, Prosign Mapping 
WK maps several unused ASCII codes to standard prosigns as shown in Table 14. 
Additional prosigns can be generated using the merge character command. 
Some abbreviations are not mapped (null), WK will ignore these if received. 
Codes below 0x20 are commands, 0x30 to 0x39 are numerals 0-9, 0x41 to 0x5A are letters A-Z. 
All codes above 0x5D are ignored except for  (0x7c) which is the ½ dit pad code.  
ASCII   Hex                     Prosign     ASCII    Hex                    Prosign             
        0x20    Is space                    .       0x2E    Is period    
!       0x21    Is mapped to    (null)      /       0x2F    Is mapped to    DN    
“       0x22    Is mapped to    RR          :       0x3A    Is mapped to    KN     
#       0x23    Is mapped to    (null)      ;       0x3B    Is mapped to    AA     
$       0x24    Is mapped to    SX          <       0x3C    Is mapped to    AR     
%       0x25    Is mapped to    (null)      =       0x3D    Is mapped to    BT     
&       0x26    Is mapped to    (null)      >       0x3E    Is mapped to    SK     
‘       0x27    Is mapped to    WG          ?       0x3F    Is question    
(       0x28    Is mapped to    KN          @       0x40    Is mapped to    AC     
)       0x29    Is mapped to    KK          [       0x5B    Is mapped to    AS    
*       0x2A    Is mapped to    (null)      \       0x5C    Is mapped to    DN    
+       0x2B    Is mapped to    AR          ]       0x5D    Is mapped to    KN     
,       0x2C    Is comma                    |       0x7C    Is ½ dit pad    
-       0x2D    Is mapped to    DU


Storing as byte   dih = 0, dah = 1 , from left to right, add an additional 00000001 in front
loop:
    if code is 1, next character, else if lsb = 0 --> dih if lsb =1 --> dah, shift right 
*/  

const uint8_t code[] = {
 /* 0x20    */     1,
 /* 0x21    */     1,
 /* 0x22 RR */     0b01101101,
 /* 0x23    */     1,
 /* 0x24 SX */     0b11001000,
 /* 0x25    */     1,
 /* 0x26    */     1,
 /* 0x27 WG */     0b01011110,
 /* 0x28 KN */     0b00110101,
 /* 0x29 KK */     0b01101101,
 /* 0x2A    */     1,
 /* 0x2B AR */     0b00110110,
 /* 0x2C ,  */     0b01110011,
 /* 0x2D DU */     0b01100001,
 /* 0x2E .  */     0b01101010,
 /* 0x2F DN */     0b00101001,
 /* 0x30 0  */     0b00111111,
 /* 0x31 1  */     0b00111110,
 /* 0x32 2  */     0b00111100,
 /* 0x33 3  */     0b00111000,
 /* 0x34 4  */     0b00110000,
 /* 0x35 5  */     0b00100000,
 /* 0x36 6  */     0b00100001,
 /* 0x37 7  */     0b00100011,
 /* 0x38 8  */     0b00100111,
 /* 0x39 9  */     0b00101111,
 /* 0x3A KN */     0b00101101,
 /* 0x3B AA */     0b00011010,
 /* 0x3C AR */     0b00101010,
 /* 0x3D BT */     0b00110001,
 /* 0x3E SK */     0b00101000,
 /* 0x3F ?  */     0b01001100,
 /* 0x40 AC */     0b01010110,
 /* 0x41 A  */     0b00000110,
 /* 0x42 B  */     0b00010001,
 /* 0x43 C  */     0b00010101,
 /* 0x44 D  */     0b00001001,
 /* 0x45 E  */     0b00000010,
 /* 0x46 F  */     0b00010100,
 /* 0x47 G  */     0b00001011,
 /* 0x48 H  */     0b00010000,
 /* 0x49 I  */     0b00000100,
 /* 0x4A J  */     0b00011110,
 /* 0x4B K  */     0b00001101,
 /* 0x4C L  */     0b00010010,
 /* 0x4D M  */     0b00000111,
 /* 0x4E N  */     0b00000101,
 /* 0x4F O  */     0b00001111,
 /* 0x50 P  */     0b00010110,
 /* 0x51 Q  */     0b00011011,
 /* 0x52 R  */     0b00001010,
 /* 0x53 S  */     0b00001000,
 /* 0x54 T  */     0b00000011,
 /* 0x55 U  */     0b00001100,
 /* 0x56 V  */     0b00011000,
 /* 0x57 W  */     0b00001110,
 /* 0x58 X  */     0b00011001,
 /* 0x59 Y  */     0b00011101,
 /* 0x5A Z  */     0b00010011,
 /* 0x5B AS */     0b00100010,
 /* 0x5C DN */     0b00100101,
 /* 0x5D KN */     0b00101101
};





#endif





