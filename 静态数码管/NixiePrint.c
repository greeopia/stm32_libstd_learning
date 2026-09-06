#include <REGX52.H>
void print (unsigned char location,character) {
	switch (character){
		case '0': P0 = 0x3F; break;   // a b c d e f
        case '1': P0 = 0x06; break;   // b c
        case '2': P0 = 0x5B; break;   // a b g e d
        case '3': P0 = 0x4F; break;   // a b g c d
        case '4': P0 = 0x66; break;   // f g b c
        case '5': P0 = 0x6D; break;   // a f g c d
        case '6': P0 = 0x7D; break;   // a f g c d e
        case '7': P0 = 0x07; break;   // a b c
        case '8': P0 = 0x7F; break;   // ??
        case '9': P0 = 0x67; break;   // a b c d f g
		
		case 'C': P0 = 0x39; break;  
        case 'i': P0 = 0x10; break;  
        case 'a': P0 = 0xDC; break;  
        case 'l': P0 = 0x38; break;  
        case 'o': P0 = 0x5C; break;  
        case '~': P0 = 0x40; break;  
        default:  P0 = 0x00; break;   
	}
	switch (location){
		case 1: P2_4=1; P2_3=1; P2_2=1; break;
        case 2: P2_4=1; P2_3=1; P2_2=0; break;
		case 3: P2_4=1; P2_3=0; P2_2=1; break;
		case 4: P2_4=1; P2_3=0; P2_2=0; break;
		case 5: P2_4=0; P2_3=1; P2_2=1; break;
		case 6: P2_4=0; P2_3=1; P2_2=0; break;
		case 7: P2_4=0; P2_3=0; P2_2=1; break;
		case 8: P2_4=0; P2_3=0; P2_2=0; break;
	}
}
void print_PWM(unsigned char location,character,speed){
	
}