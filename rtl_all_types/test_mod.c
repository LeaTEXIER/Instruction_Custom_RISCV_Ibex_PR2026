
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

int main() {
    
    int a = 15;
    int b = 4;
    int c, d, e;
    
    
    /////////
    // MOD //
    /////////
    
    print_str("TEST MOD \n");

    // --- TEST 1 : Dépendance RAW (Read-After-Write) ---
    // On écrit dans 'c', et on utilise immédiatement 'c' pour le calcul suivant.
    // Le compilateur doit garantir que 'c' est prêt avant le second bloc asm.
    
    asm volatile (
        "mod %[z], %[x], %[y]\n\t"
        : [z] "=r" (c)
        : [x] "r" (a), [y] "r" (b)
    );

    asm volatile (
        "mod %[z], %[x], %[y]\n\t"
        : [z] "=r" (d)
        : [x] "r" (c), [y] "r" (b) // Utilise 'c' (15%4 = 3) -> 3%4 = 3
    );

    // --- TEST 2 : Dépendance avec du code C standard ---
    // On vérifie que le compilateur ne réordonne pas le C après l'ASM de manière incorrecte.
    e = d + 10; // Dépend de 'd' calculé par l'ASM

    // --- Affichage des résultats ---
    print_str("Test 1 (15 % 4): ");
    print_int(c); // Attendu: 3
    
    print_str("\nTest 2 (Result1 % 4): ");
    print_int(d); // Attendu: 3
    
    print_str("\nTest 3 (Result2 + 10): ");
    print_int(e); // Attendu: 13
    print_str("\n");


    // --- TEST 3 : Stress Test (Pipeline) ---
    int q = 20;
    int i = 12;
    while (i > 0) {
    	asm volatile (
            "mod %[z], %[x], %[y]\n\t"
            : [z] "=r" (i)
            : [x] "r" (q), [y] "r" (i)
        );
    }
    
    print_str("Stress Test (chain): ");
    print_int(i);
    print_str("\n");
    
    // --- TEST 4 : Collé ---
    int f = 51;
    int g = 28;
    int h;
    asm volatile (
        "mod %[z], %[x], %[y] \n\t" // h = f % g
        "mod %[x], %[y], %[z] \n\t" // f = g % h
        "mod %[z], %[x], %[y] \n\t" // h = f % g
        "mod %[z], %[x], %[y] \n\t" // h = f % g
        : [z] "+r" (h), [x] "+r" (f) 
        : [y] "r"  (g)
        : "memory"
    );
    print_str("Résultat final h: ");
    print_int(h);
    print_str(",\nf: ");
    print_int(f);
    print_str("\n\n");  
    
    
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
    p = n + 10;

    // --- Affichage des résultats ---
    print_str("Test 1 (15 mroli 4): ");
    print_int(o);
    print_str("\nExpected: ");
    print_int(rotate_right(m, 4));
    
    print_str("\nTest 2 (Result1 mroli 4): ");
    print_int(n); 
    print_str("\nExpected: ");
    print_int(rotate_right(o, 4));
    
    print_str("\nTest 3 (Result2 + 10): ");
    print_int(p);
    print_str("\n");


    
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
    print_str("\nRésultat intermédiaire: ");
    print_int(r);
    print_str("\nExpected: ");
    print_int(rotate_right(k,5));
    print_str("\nRésultat intermédiaire: ");
    print_int(s);
    print_str("\nExpected: ");
    print_int(rotate_right(r,4));
    print_str("\nRésultat intermédiaire: ");
    print_int(t);  
    print_str("\nExpected: ");
    print_int(rotate_right(s,3));
    print_str("\nRésultat final: ");
    print_int(u);
    print_str("\nExpected: ");
    print_int(rotate_right(t,2));
    print_str("\n\n");   
    
    
    ////////////
    // MDROLI //
    ////////////
    
    print_str("TEST MDROLI \n");
    
    // Test 1
    
    int val;
    
    asm volatile (
        "mdroli %0, 0x12345"
        : "=r" (val)
    );    
    
    print_str("\nRésultat: ");
    print_int(val);
    print_str("\nExpected: ");
    print_int(rotate_right(74565,7)); //386 342 915 = 0x17072003  
    print_str("\n");
    
    // Test 2
    
    int val2;
    
    asm volatile (
    	"mdroli %0, 0x6789A"
        : "=r" (val2)
    );
    
    val = val2 + 10;
    
    print_str("\nRésultat: ");
    print_int(val);
    print_str("\nExpected: ");
    print_int(rotate_right(424090,7) + 10); //386 342 915 = 0x17072003  
    print_str("\n");
    
    //////////
    // JUMB //
    //////////
    print_str("\n");
    print_str("TEST JUMB \n");
    
    int src1, dummy, rot;
    src1 = 3;
    dummy = 0;
    
    asm volatile (
        "jumb %1, %1, 1f \n\t"
        "mdroli %0, 0x123 \n\t"
        "mdroli %0, 0x1C8 \n\t"	//456 = 0x1C8
        "nop \n\t"
        "1: \n\t"
        : "=r" (rot)
        : "r" (src1)
    );
    
    print_str("\nRésultat: ");
    print_int(rot);
    print_str("\nExpected: ");
    print_int(rotate_right(456,7)); 
    print_str("\n");
    
    asm volatile (
        "jumb %1, %2, 1f \n\t"
        "mdroli %0, 0x123 \n\t"	//291 = 0x123
        "mdroli %2, 0x1C8 \n\t"	//456 = 0x1C8
        "nop \n\t"
        "1: \n\t"
        : "=r" (rot)
        : "r" (src1), "r" (dummy)
    );
    
    /* Pour tester l'instruction "jumb", on écrit un label dans l'instruction assembleur car autrement 
    à cause du type "bimm12" cela ne compile pas.
    D'où la nécésité des lignes:
    	"nop \n\t"
        "1: \n\t"
    */

    
    print_str("\nRésultat: ");
    print_int(rot);
    print_str("\nExpected: ");
    print_int(rotate_right(291,7)); 
    print_str("\n");
    

    print_str("Fin de simulation.\n");
    stop_simulation();
    return 0;
}























