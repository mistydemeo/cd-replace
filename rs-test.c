/*----------------------------------------------------------------------------*/
/*-- rs-test.c                                                              --*/
/*-- Reed-Solomon Test                                                      --*/
/*-- Copyright (C) 2012 CUE                                                 --*/
/*--                                                                        --*/
/*-- This program is free software: you can redistribute it and/or modify   --*/
/*-- it under the terms of the GNU General Public License as published by   --*/
/*-- the Free Software Foundation, either version 3 of the License, or      --*/
/*-- (at your option) any later version.                                    --*/
/*--                                                                        --*/
/*-- This program is distributed in the hope that it will be useful,        --*/
/*-- but WITHOUT ANY WARRANTY; without even the implied warranty of         --*/
/*-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           --*/
/*-- GNU General Public License for more details.                           --*/
/*--                                                                        --*/
/*-- You should have received a copy of the GNU General Public License      --*/
/*-- along with this program. If not, see <http://www.gnu.org/licenses/>.   --*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------*/
#define TOTAL_TABLES  2                   // 2 matrices (par e impar)
#define P_ROWS        24                  // la matriz para P es de 24*43
#define P_COLUMNS     43                  // con los elementos seguidos
#define P_NEXT_ROW    (P_COLUMNS)         // incremento para otra fila P
#define P_NEXT_COLUMN 1                   // incremento para otra columna P
#define Q_ROWS        26                  // la matriz para P es de 26*43
#define Q_COLUMNS     43                  // con los elementos de la diagonal P
#define Q_NEXT_ROW    (Q_COLUMNS)         // incremento para otra fila Q
#define Q_NEXT_COLUMN (1 + Q_NEXT_ROW)    // incremento para otra columna Q

#define L2_RAW (1024*TOTAL_TABLES)        // bytes de datos por página
#define L2_P   (2*P_COLUMNS*TOTAL_TABLES) // bytes de paridad P
#define L2_Q   (2*Q_ROWS*TOTAL_TABLES)    // bytes de paridad Q

/*----------------------------------------------------------------------------*/
unsigned int  tcrc[256];   // tabla de CRC32 inverso
unsigned char gexp[256*2]; // tabla de potencias
unsigned char glog[256];   // tabla de índices

/*----------------------------------------------------------------------------*/
unsigned char DP[2][P_ROWS] = {
  {231,229,171,210,240, 17, 67,215, 43,120,  8,199, 74,102,220,251,
    95,175, 87,166,113, 75,198, 25,},
  {230,172,211,241, 18, 68,216, 44,121,  9,200, 75,103,221,252, 96,
   176, 88,167,114, 76,199, 26,  1,},
};
unsigned char DQ[2][Q_COLUMNS] = {
  {190, 96,250,132, 59, 81,159,154,200,  7,111,245, 10, 20, 41,156,
   168, 79,173,231,229,171,210,240, 17, 67,215, 43,120,  8,199, 74,
   102,220,251, 95,175, 87,166,113, 75,198, 25,},
  { 97,251,133, 60, 82,160,155,201,  8,112,246, 11, 21, 42,157,169,
    80,174,232,230,172,211,241, 18, 68,216, 44,121,  9,200, 75,103,
   221,252, 96,176, 88,167,114, 76,199, 26,  1,},
};

/*----------------------------------------------------------------------------*/
char NUM2BCD(char value);
void Init(void);
void Generate_EDC(char *source, int length);
void Generate_ECC_P(char *source);
void Generate_ECC_Q(char *source);

/*----------------------------------------------------------------------------*/
int main(int argc, char **argv) {
  FILE *fp;
  unsigned char mode, form2, old[0x930], new[0x930];
  unsigned int  sect, synz, sync, hdrz, subz, edcx, edcz, eccx;
  unsigned int  m1, f1, f2;
  unsigned int  i, j;

  printf("\nReed-Solomon Test - Copyright (C) 2012 CUE\n\n");

  if (argc != 2) { printf("Sintaxis: %s cdiso\n", argv[0]); return(1); }

  if ((fp = fopen(argv[1], "rb")) == NULL) {
    printf("Error abriendo la imagen\n");
    return(1);
  }

  Init();

  sect = filelength(fileno(fp)) / 0x930;
  if (!sect) {
    printf("La imagen debe tener al menos un sector\n");
    return(1);
  } else if (sect * 0x930 != filelength(fileno(fp))) {
    printf("AVISO: La imagen tiene el sector final imcompleto\n\n");
  }

  mode = 0;
  m1 = f1 = f2 = 0;
  synz = sync = hdrz = subz = edcx = edcz = eccx = 0;

  for (i = 0; i < sect; i++) {
    printf("\r%d ", i);

    if (fread(old, 1, 0x930, fp) != 0x930) {
      printf("Error leyendo el sector\n");
      return(1);
    }

    if (!*(int *)(old + 0x000)) {
      if (!*(int *)(old + 0x004)) {
        if (!*(int *)(old + 0x008)) {
          if (mode) { synz++; continue; }
        }
      }
    }
    if (*(int *)(old + 0x000) != 0xFFFFFF00) {
      if (mode) { sync++; continue; }
      printf("Error en campo SYNCHRONIZATION\n");
      return(1);
    }
    if (*(int *)(old + 0x004) != 0xFFFFFFFF) {
      if (mode) { sync++; continue; }
      printf("Error en campo SYNCHRONIZATION\n");
      return(1);
    }
    if (*(int *)(old + 0x008) != 0x00FFFFFF) {
      if (mode) { sync++; continue; }
      printf("Error en campo SYNCHRONIZATION\n");
      return(1);
    }
    if (!*(int *)(old + 0x00C)) { hdrz++; continue; }
    if (old[0x00C] != NUM2BCD(((i + 150) / 75) / 60)) {
      printf("Error en campo MINUTE\n");
      return(1);
    }
    if (old[0x00D] != NUM2BCD(((i + 150) / 75) % 60)) {
      printf("Error en campo SECOND\n");
      return(1);
    }
    if (old[0x00E] != NUM2BCD((i + 150) % 75)) {
      printf("Error en campo FRACTION\n");
      return(1);
    }
    if (!mode) {
      mode = old[0x00F];
      if ((mode != 1) && (mode != 2)) {
        printf("El modo del sector 0 no es correcto\n");
        return(1);
      }
    } else if (old[0x00F] != mode) {
      printf("Error en campo MODE\n");
      return(1);
    }

    if (mode == 1) {
      if (*(int *)(old + 0x814) || *(int *)(old + 0x818)) {
        printf("Error en campo INTERMEDIATE\n");
        return(1);
      }
      m1++;
    } else {
      if (*(int *)(old + 0x010) != *(int *)(old + 0x014)) {
        printf("Error en campo SUBHEADER\n");
        return(1);
      }
      if (!*(int *)(old + 0x010)) { subz++; continue; }
      form2 = old[0x012] & 0x20;
      if (!form2) f1++;
      else        f2++;
    }

    for (j = 0; j < 0x930; j++) new[j] = old[j];

    if (mode == 1) {
      Generate_EDC(new, 0x810);
      Generate_ECC_P(new + 0x00C);
      Generate_ECC_Q(new + 0x00C);
      for (j = 0x810; j < 0x930; j++) {
        if (old[j] != new[j]) {
          if (j < 0x814) { edcx++; break; }
          else           { eccx++; break; }
        }
      }
    } else {
      if (!form2) {
        Generate_EDC(new + 0x010, 0x808);
        *(int *)(new + 0x00C) = 0;
        Generate_ECC_P(new + 0x00C);
        Generate_ECC_Q(new + 0x00C);
        *(int *)(new + 0x00C) = *(int *)(old + 0x00C);
        for (j = 0x818; j < 0x930; j++) {
          if (old[j] != new[j]) {
            if (j < 0x81C) { edcx++; break; }
            else           { eccx++; break; }
          }
        }
      } else {
        Generate_EDC(new + 0x010, 0x91C);
        for (j = 0x92C; j < 0x930; j++) {
          if (old[j] != new[j]) { edcz++; break; }
        }
      }
    }
  }

  fclose(fp);

  printf("\r");
  printf("%6d sectores\n", sect);
  printf("\n");
  if (m1) printf("%6d sectores con MODE1\n", m1);
  if (f1) printf("%6d sectores con MODE2/FORM1\n", f1);
  if (f2) printf("%6d sectores con MODE2/FORM2\n", f2);
  printf("%6d sectores con SYNCHRONIZATION a 0 (pre/postgap?)\n", synz);
  printf("%6d sectores con SYNCHRONIZATION mal (audio?)\n", sync);
  printf("%6d sectores con HEADER a 0 (pre/postgap?)\n", hdrz);
  if (mode == 2) {
    printf("%6d sectores con SUBHEADER a 0 (PS2 o pre/postgap?)\n", subz);
  }
  printf("\n");
  printf("%6d sectores con EDC mal\n", edcx);
  if (edcz) printf("%6d sectores con EDC mal en MODE2/FORM2\n", edcz);
  printf("%6d sectores con ECC mal\n", eccx);

  return(0);
}

/*----------------------------------------------------------------------------*/
char NUM2BCD(char value) {
  return(((value / 10) << 4) | (value % 10));
}

/*----------------------------------------------------------------------------*/
void Init(void) {
  unsigned int  edc;
  unsigned char mask;
  unsigned int  i, j;

  // polinomio: x^32 + x^31 + x^16 + x^15 + x^4 + x^3 + x^1 + x^0
  // constante = 18001801B -> la inversa es D8018001
  for(i = 0; i < 256; i++) {
    edc = i;
    for(j = 0; j < 8; j++)
      edc = edc & 1 ? (edc >> 1) ^ 0xD8018001 : edc >> 1;
    tcrc[i] = edc;
  }

  // polinomio irreducible: x^8 + x^4 + x^3 + x^2 + x^0
  // constante = 11D
  mask = 1;
  for(i = 0; i < 256; i++) {
    gexp[i]        = mask;
    gexp[i + 255]  = mask;
    glog[mask]     = i;
    mask = mask & 0x80 ? (mask << 1) ^ 0x1D : mask << 1;
  }
}

/*----------------------------------------------------------------------------*/
void Generate_EDC(char *source, int length) {
  unsigned int edc = 0;

  while (length--) edc = (edc >> 8) ^ tcrc[(edc ^ *source++) & 0xFF];

  *(unsigned int *)source = edc;
}

/*----------------------------------------------------------------------------*/
void Generate_ECC_P(char *source) {
  unsigned char *target, ch;
  unsigned int   size, column, row, table, index;

  size = L2_RAW + 16;
  target = source + size;

  for (ch = 0; ch < L2_P; ch++) target[ch] = 0;

  for (column = 0; column < P_COLUMNS; column++) {
    for (row = 0; row < P_ROWS; row++) {
      index = (column * P_NEXT_COLUMN + row * P_NEXT_ROW) * TOTAL_TABLES;
      for (table = 0; table < TOTAL_TABLES; table++) {
        ch = *(source + (index + table) % size);
        if (ch) {
          *(target+table)                        ^= gexp[glog[ch] + DP[0][row]];
          *(target+table+P_COLUMNS*TOTAL_TABLES) ^= gexp[glog[ch] + DP[1][row]];
        }
      }
    }
    target += TOTAL_TABLES;
  }
}

/*----------------------------------------------------------------------------*/
void Generate_ECC_Q(char *source) {
  unsigned char *target, ch;
  unsigned int   size, column, row, table, index;

  size = L2_RAW + 16 + L2_P;
  target = source + size;

  for (ch = 0; ch < L2_Q; ch++) target[ch] = 0;

  for (row = 0; row < Q_ROWS; row++) {
    for (column = 0; column < Q_COLUMNS; column++) {
      index = (row * Q_NEXT_ROW + column * Q_NEXT_COLUMN) * TOTAL_TABLES;
      for (table = 0; table < TOTAL_TABLES; table++) {
        ch = *(source + (index + table) % size);
        if (ch) {
          *(target+table)                     ^= gexp[glog[ch] + DQ[0][column]];
          *(target+table+Q_ROWS*TOTAL_TABLES) ^= gexp[glog[ch] + DQ[1][column]];
        }
      }
    }
    target += TOTAL_TABLES;
  }
}

/*----------------------------------------------------------------------------*/
/*--  EOF                                           Copyright (C) 2012 CUE  --*/
/*----------------------------------------------------------------------------*/
