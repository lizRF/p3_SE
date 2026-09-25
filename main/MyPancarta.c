#include "MyPancarta.h"
#include "MyUART.h"


// ---------- letras en mayúscula ----------
static const uint8_t font_A[7] = {0b01110,0b10001,0b10001,0b11111,0b10001,0b10001,0b10001};
static const uint8_t font_B[7] = {0b11110,0b10001,0b10001,0b11110,0b10001,0b10001,0b11110};
static const uint8_t font_C[7] = {0b01111,0b10000,0b10000,0b10000,0b10000,0b10000,0b01111};
static const uint8_t font_D[7] = {0b11110,0b10001,0b10001,0b10001,0b10001,0b10001,0b11110};
static const uint8_t font_E[7] = {0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b11111};
static const uint8_t font_F[7] = {0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b10000};
static const uint8_t font_G[7] = {0b01111,0b10000,0b10000,0b10011,0b10001,0b10001,0b01111};
static const uint8_t font_H[7] = {0b10001,0b10001,0b10001,0b11111,0b10001,0b10001,0b10001};
static const uint8_t font_I[7] = {0b01110,0b00100,0b00100,0b00100,0b00100,0b00100,0b01110};
static const uint8_t font_J[7] = {0b00111,0b00010,0b00010,0b00010,0b00010,0b10010,0b01100};
static const uint8_t font_K[7] = {0b10001,0b10010,0b10100,0b11000,0b10100,0b10010,0b10001};
static const uint8_t font_L[7] = {0b10000,0b10000,0b10000,0b10000,0b10000,0b10000,0b11111};
static const uint8_t font_M[7] = {0b10001,0b11011,0b10101,0b10101,0b10001,0b10001,0b10001};
static const uint8_t font_N[7] = {0b10001,0b11001,0b10101,0b10101,0b10011,0b10001,0b10001};
static const uint8_t font_O[7] = {0b01110,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110};
static const uint8_t font_P[7] = {0b11110,0b10001,0b10001,0b11110,0b10000,0b10000,0b10000};
static const uint8_t font_Q[7] = {0b01110,0b10001,0b10001,0b10001,0b10101,0b10010,0b01101};
static const uint8_t font_R[7] = {0b11110,0b10001,0b10001,0b11110,0b10100,0b10010,0b10001};
static const uint8_t font_S[7] = {0b01111,0b10000,0b10000,0b01110,0b00001,0b00001,0b11110};
static const uint8_t font_T[7] = {0b11111,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100};
static const uint8_t font_U[7] = {0b10001,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110};
static const uint8_t font_V[7] = {0b10001,0b10001,0b10001,0b10001,0b10001,0b01010,0b00100};
static const uint8_t font_W[7] = {0b10001,0b10001,0b10001,0b10101,0b10101,0b10101,0b01010};
static const uint8_t font_X[7] = {0b10001,0b10001,0b01010,0b00100,0b01010,0b10001,0b10001};
static const uint8_t font_Y[7] = {0b10001,0b10001,0b01010,0b00100,0b00100,0b00100,0b00100};
static const uint8_t font_Z[7] = {0b11111,0b00001,0b00010,0b00100,0b01000,0b10000,0b11111};

// ---------- minúsculas (forma compacta, sin descendentes reales) ----------
//static const uint8_t font_a[7] = {0b00000,0b00000,0b01110,0b00001,0b01111,0b10001,0b01111};
static const uint8_t font_a[7] = {0b00000,0b00000,0b00000,0b01110,0b10001,0b01111,0b00001};
static const uint8_t font_b[7] = {0b10000,0b10000,0b11110,0b10001,0b10001,0b10001,0b11110};
static const uint8_t font_c[7] = {0b00000,0b00000,0b01111,0b10000,0b10000,0b10000,0b01111};
static const uint8_t font_d[7] = {0b00001,0b00001,0b01111,0b10001,0b10001,0b10001,0b01111};
static const uint8_t font_e[7] = {0b00000,0b00000,0b01110,0b10001,0b11111,0b10000,0b01111};
static const uint8_t font_f[7] = {0b00110,0b01001,0b01000,0b11110,0b01000,0b01000,0b01000};
static const uint8_t font_g[7] = {0b00000,0b00000,0b01111,0b10001,0b01111,0b00001,0b01110};
static const uint8_t font_h[7] = {0b10000,0b10000,0b10110,0b11001,0b10001,0b10001,0b10001};
static const uint8_t font_i[7] = {0b00100,0b00000,0b01100,0b00100,0b00100,0b00100,0b01110};
static const uint8_t font_j[7] = {0b00010,0b00000,0b00110,0b00010,0b00010,0b10010,0b01100};
static const uint8_t font_k[7] = {0b10000,0b10000,0b10010,0b10100,0b11000,0b10100,0b10010};
static const uint8_t font_l[7] = {0b01100,0b00100,0b00100,0b00100,0b00100,0b00100,0b01110};
static const uint8_t font_m[7] = {0b00000,0b00000,0b11010,0b10101,0b10101,0b10101,0b10101};
static const uint8_t font_n[7] = {0b00000,0b00000,0b10110,0b11001,0b10001,0b10001,0b10001};
static const uint8_t font_o[7] = {0b00000,0b00000,0b01110,0b10001,0b10001,0b10001,0b01110};
static const uint8_t font_p[7] = {0b00000,0b00000,0b11110,0b10001,0b11110,0b10000,0b10000};
static const uint8_t font_q[7] = {0b00000,0b00000,0b01111,0b10001,0b01111,0b00001,0b00001};
static const uint8_t font_r[7] = {0b00000,0b00000,0b10110,0b11001,0b10000,0b10000,0b10000};
static const uint8_t font_s[7] = {0b00000,0b00000,0b01111,0b10000,0b01110,0b00001,0b11110};
static const uint8_t font_t[7] = {0b01000,0b01000,0b11110,0b01000,0b01000,0b01001,0b00110};
static const uint8_t font_u[7] = {0b00000,0b00000,0b10001,0b10001,0b10001,0b10011,0b01101};
static const uint8_t font_v[7] = {0b00000,0b00000,0b10001,0b10001,0b10001,0b01010,0b00100};
static const uint8_t font_w[7] = {0b00000,0b00000,0b10001,0b10001,0b10101,0b10101,0b01010};
static const uint8_t font_x[7] = {0b00000,0b00000,0b10001,0b01010,0b00100,0b01010,0b10001};
static const uint8_t font_y[7] = {0b00000,0b00000,0b10001,0b10001,0b01111,0b00001,0b01110};
static const uint8_t font_z[7] = {0b00000,0b00000,0b11111,0b00010,0b00100,0b01000,0b11111};

// ---------- numero del 0 a 9  ----------
static const uint8_t font_0[7] = {0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110};
static const uint8_t font_1[7] = {0b00100,0b01100,0b00100,0b00100,0b00100,0b00100,0b01110};
static const uint8_t font_2[7] = {0b01110,0b10001,0b00001,0b00010,0b00100,0b01000,0b11111};
static const uint8_t font_3[7] = {0b11111,0b00010,0b00100,0b00010,0b00001,0b10001,0b01110};
static const uint8_t font_4[7] = {0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010};
static const uint8_t font_5[7] = {0b11111,0b10000,0b11110,0b00001,0b00001,0b10001,0b01110};
static const uint8_t font_6[7] = {0b00110,0b01000,0b10000,0b11110,0b10001,0b10001,0b01110};
static const uint8_t font_7[7] = {0b11111,0b00001,0b00010,0b00100,0b01000,0b01000,0b01000};
static const uint8_t font_8[7] = {0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110};
static const uint8_t font_9[7] = {0b01110,0b10001,0b10001,0b01111,0b00001,0b00010,0b01100};

// ---------- símbolos ----------
static const uint8_t font_SPACE[7]       = {0,0,0,0,0,0,0};
static const uint8_t font_PUNTO[7]       = {0b00000,0b00000,0b00000,0b00000,0b00000,0b01100,0b01100};
static const uint8_t font_EXCLAMACION[7] = {0b00100,0b00100,0b00100,0b00100,0b00100,0b00000,0b00100};
static const uint8_t font_MAS[7]         = {0b00000,0b00100,0b00100,0b11111,0b00100,0b00100,0b00000};
static const uint8_t font_GUION[7]       = {0b00000,0b00000,0b00000,0b11111,0b00000,0b00000,0b00000};

// Tablas indexadas
static const uint8_t* const font_mayus[26] = {
    font_A, font_B, font_C, font_D, font_E, font_F, font_G, font_H, font_I,
    font_J, font_K, font_L, font_M, font_N, font_O, font_P, font_Q, font_R,
    font_S, font_T, font_U, font_V, font_W, font_X, font_Y, font_Z
};

static const uint8_t* const font_minus[26] = {
    font_a, font_b, font_c, font_d, font_e, font_f, font_g, font_h, font_i,
    font_j, font_k, font_l, font_m, font_n, font_o, font_p, font_q, font_r,
    font_s, font_t, font_u, font_v, font_w, font_x, font_y, font_z
};

static const uint8_t* const font_digitos[10] = {
    font_0, font_1, font_2, font_3, font_4,
    font_5, font_6, font_7, font_8, font_9
};

static const uint8_t* obtener_glifo(char c){
    if(c >= 'A' && c <= 'Z') return font_mayus[c - 'A'];
    if(c >= 'a' && c <= 'z') return font_minus[c - 'a'];
    if(c >= '0' && c <= '9') return font_digitos[c - '0'];
    if(c == '.') return font_PUNTO;
    if(c == '!') return font_EXCLAMACION;
    if(c == '+') return font_MAS;
    if(c == '-') return font_GUION;
    return font_SPACE; // espacio o carácter no soportado
}

void imprimir_pancarta(uart_port_t uart_num, const char *texto){
    uint8_t len = 0;
    while(texto[len] != '\0') len++;

    for(int fila = 0; fila < FONT_ROWS; fila++){
        for(uint8_t i = 0; i < len; i++){
            const uint8_t *glifo = obtener_glifo(texto[i]);
            uint8_t bits = glifo[fila];
            for(int col = FONT_COLS - 1; col >= 0; col--){
                if(bits & (1 << col)){
                    uart_putchar(uart_num, '#');
                } else {
                    uart_putchar(uart_num, ' ');
                }
            }
            uart_putchar(uart_num, ' '); // separación entre letras
        }
        uart_puts(uart_num, "\r\n");
    }
}