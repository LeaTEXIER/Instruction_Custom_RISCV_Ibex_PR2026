
#define UART_OUT 0x20000
#define SIM_HALT 0x20008

typedef unsigned int uint32_ptr_t;

void simple_exc_handler(void) { while(1); }
void simple_timer_handler(void) { while(1); }

void print_char(char c) {
	volatile unsigned int *p = (unsigned int *) UART_OUT;
	*p = c;
}
void print_str(const char *s) {
	while (*s) print_char(*s++);
}

void stop_simulation() {
	volatile unsigned int *p = (unsigned int *)SIM_HALT;
	*p = 1;
}

void print_int(int n) {
	if (n==0) {
	 	print_char('0');
	 	return;
	}
	if (n < 0) {
 		print_char('-');
 		n = -n;
	}
	char buf[12];  // 12 > 32-bit + signe
	int i = 0;
	while (n > 0) {
		buf[i++] = (n % 10) + '0' ;
		n /= 10;
	}
	while (i > 0) {
		print_char(buf[--i]);
	}
}

uint32_ptr_t rotate_left(uint32_ptr_t value, uint32_ptr_t shift) {
	shift &= 31;
	if (shift == 0) return value;
	return (value << shift) | (value >> 32 - shift);
}
// Normalement on a implémenté celle-ci
uint32_ptr_t rotate_right(uint32_ptr_t value, uint32_ptr_t shift) {
	shift &= 31;
	if (shift == 0) return value;
	return (value >> shift) | (value << 32 - shift);
}
/*
int main(){
    int a,b,c,d, rot_i, rot_u, rot_right, rot_left;
    a = 15;
    b = 4;
    d = a % b;
    
    rot_right = rotate_right(a,b);
    rot_left = rotate_left(a,b);
    
    asm volatile
    (
    "mod   %[z], %[x], %[y]\n\t"
    : [z] "=r" (c)
    : [x] "r" (a), [y] "r" (b)
    );
    print_str("Résultat du mod: ");
    print_int(d);
    print_str("\nRésultat du MOD(asm): ");
    print_int(c);
    print_str("\n");
    
    // Rotation droite
    
    asm volatile
    (
    "mroli   %[z], %[x], 4\n\t"
    : [z] "=r" (rot_i)
    : [x] "r" (a)
    );
    //asm volatile
    //(
    //"mdroli   %[z], 0x12345\n\t"
    //: [z] "=r" (rot_u)
    //:
    //);
    print_str("Résultat du ROTI: ");
    print_int(rot_i);
    //print_str("Résultat du ROTU: ");
    //print_int(rot_u);
    print_str("\nRésultat du rotate_right: ");
    print_int(rot_right);
    print_str("\nRésultat du rotate_left: ");
    print_int(rot_left);
    print_str("\n");
    
    print_str("Fin de simulation. \n");
    stop_simulation();
    
    return 0;
}*/

int main() {
    int a = 15;
    int b = 4;
    int c, d, e;
     
    
    ///////////
    // MROLI //
    ///////////
    
    print_str("TEST MROLI \n");
    
    int m, n, o, p;
    m = 15;
    
    // --- TEST 1 : Dépendance RAW (Read-After-Write) ---

    asm volatile
    (
    "mroli   %[z], %[x], 4\n\t"
    : [z] "=r" (o)
    : [x] "r" (m)
    );
    
    asm volatile
    (
    "mroli   %[z], %[x], 4\n\t"
    : [z] "=r" (n)
    : [x] "r" (o)
    );


    // --- TEST 2 : Dépendance avec du code C standard ---
    // On vérifie que le compilateur ne réordonne pas le C après l'ASM de manière incorrecte.
    p = n + 10; // Dépend de 'd' calculé par l'ASM

    // --- Affichage des résultats ---
    print_str("Test 1 (15 roti 4): ");
    print_int(o);
    
    print_str("\nTest 2 (Result1 roti 4): ");
    print_int(n);
    
    // --- TEST 4 : Collé ---
    int k =305419896;
    int r, s, t, u;
    asm volatile (
        "mroli %[w], %[v], 5 \n\t"
        "mroli %[x], %[w], 4 \n\t" 
        "mroli %[y], %[x], 3 \n\t" 
        "mroli %[z], %[y], 2 \n\t" 
        : [w] "=r" (r), [x] "=r" (s), [y] "=r" (t), [z] "=r" (u) //OUTPUTS
        : [v] "r" (k)			//INPUTS
    );  
    
    
    print_str("Fin de simulation.\n");
    stop_simulation();
    return 0;
}
























