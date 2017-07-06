/*----------------------------------------------------------------------------*/
/*-- cdrom.inc                                                              --*/
/*-- Rebuild CD sector fields                                               --*/
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
#ifndef CDROM
#define CDROM

/*----------------------------------------------------------------------------*/
//                +--------------------------+--------------------------+
//                | -------FIELD SIZE------- | ------FIELD OFFSET------ |
// +--------------+--------------------------+--------------------------+
// | Mode         | -M0- -M1- -M2- -F1- -F2- | -M0- -M1- -M2- -F1- -F2- |
// +--------------+--------------------------+--------------------------+
// | Sync         |    - 000C 000C 000C 000C |    - 0000 0000 0000 0000 |
// | Header       |    - 0004 0004 0004 0004 |    - 000C 000C 000C 000C |
// | Subheader    |    -    -    - 0008 0008 |    -    -    - 0010 0010 |
// | User Data    | 0800 0800 0920 0800 0914 | 0000 0010 0010 0018 0018 |
// | EDC          |    - 0004    - 0004 0004 |    - 0810    - 0818 092C |
// | Intermediate |    - 0008    -    -    - |    - 0814    -         - |
// | ECC-P        |    - 00AC    - 00AC    - |    - 081C    - 081C    - |
// | ECC-Q        |    - 0068    - 0068    - |    - 08C8    - 08C8    - |
// +--------------+--------------------------+--------------------------+
// | Sector Size  | 0800 0930 0930 0930 0930 | 0800 0930 0930 0930 0930 |
// +--------------+--------------------------+--------------------------+

#define MODE_NO          -1    // mode not supported
#define MODE_M0          0     // mode 0 (CD-ROM)
#define MODE_M1          1     // mode 1 (CD-ROM)
#define MODE_M2          2     // mode 2 (CD-ROM)
#define MODE_F1          21    // mode 2 form 1 (CD-ROM XA)
#define MODE_F2          22    // mode 2 form 2 (CD-ROM XA)

#define LEN_SECTOR_M0    0x800 // M0
#define LEN_SECTOR_M1    0x930 // M1
#define LEN_SECTOR_M2    0x930 // M2
#define LEN_SECTOR_F1    0x930 // F1
#define LEN_SECTOR_F2    0x930 // F2

#define LEN_SYNC         0x00C // M1-M2-F1-F2
#define LEN_HEADER       0x004 // M1-M2-F1-F2
#define LEN_SUBHEADER    0x008 // F1-F2
#define LEN_DATA_M0      0x800 // M0
#define LEN_DATA_M1      0x800 // M1
#define LEN_DATA_M2      0x920 // M2
#define LEN_DATA_F1      0x800 // F1
#define LEN_DATA_F2      0x914 // F2
#define LEN_EDC          0x004 // M1-F1-F2
#define LEN_INTERMEDIATE 0x008 // M1          
#define LEN_ECC_P        0x0AC // M1-F1       = 2*(43+43) for two 24*43 tables
#define LEN_ECC_Q        0x068 // M1-F1       = 2*(26+26) for two 26*43 tables

#define POS_SYNC         0x000 // M1-M2-F1-F2 = POS_SYNC
#define POS_HEADER       0x00C // M1-M2-F1-F2 = POS_SYNC + LEN_SYNC
#define POS_MINUTE       0x00C // M1-M2-F1-F2 minute 0-99 in BCD
#define POS_SECOND       0x00D // M1-M2-F1-F2 second 0-59 in BCD
#define POS_FRACTION     0x00E // M1-M2-F1-F2 fraction 0-74 in BCD
#define POS_MODE         0x00F // M1-M2-F1-F2 mode 1/2
#define POS_SUBHEADER    0x010 // F1-F2       = POS_HEADER + LEN_HEADER
#define POS_FILENUMBER   0x010 // F1-F2       0 standard data, 1-255 interleaved
#define POS_CHANNEL      0x011 // F1-F2       0-15 ADPCM, 0-31 data/video
#define POS_SUBMODE      0x012 // F1-F2       see below (SM_xxx)
#define POS_CODINGINFO   0x013 // F1-F2       see below (CI_xxx)
#define POS_DATA_M0      0x000 // M0          = POS_SYNC
#define POS_DATA_M1      0x010 // M1          = POS_HEADER + LEN_HEADER
#define POS_DATA_M2      0x010 // M2          = POS_HEADER + LEN_HEADER
#define POS_DATA_F1      0x018 // F1          = POS_SUBHEADER + LEN_SUBHEADER
#define POS_DATA_F2      0x018 // F2          = POS_SUBHEADER + LEN_SUBHEADER
#define POS_EDC_M1       0x810 // M1          = POS_DATA_M1 + LEN_DATA_M1
#define POS_EDC_F1       0x818 // F1          = POS_DATA_F1 + LEN_DATA_F1
#define POS_EDC_F2       0x92C // F2          = POS_DATA_F2 + LEN_DATA_F2
#define POS_INTERMEDIATE 0x814 // M1          = POS_EDC_M1 + LEN_EDC
#define POS_ECC_P        0x81C // M1-F1       = POS_EDC_F1 + LEN_EDC
#define POS_ECC_Q        0x8C8 // M1-F1       = POS_ECC_P + LEN_ECC_P

#define CALC_EDC_M1      0x810 // M1          = POS_EDC_M1 - POS_SYNC
#define CALC_EDC_F1      0x808 // F1          = POS_EDC_F1 - POS_SUBHEADER
#define CALC_EDC_F2      0x91C // F2          = POS_EDC_F2 - POS_SUBHEADER

#define CALC_ECC_P       0x810 // M1-F1       = POS_ECC_P - POS_HEADER = 2*24*43
#define CALC_ECC_Q       0x8BC // M1-F1       = POS_ECC_Q - POS_HEADER = 2*26*43

#define SM_EOR           0x01  // end of record
#define SM_VIDEO         0x02  // video --+
#define SM_AUDIO         0x04  // audio --+---> only 1
#define SM_DATA          0x08  // data ---+
#define SM_TRIGGER       0x10  // trigger
#define SM_FORM2         0x20  // form 2
#define SM_REALTIME      0x40  // real time
#define SM_EOF           0x80  // end of file

#define CI_MONOSTEREO    0x01  // 0=mono, 1=stereo
#define CI_BIT1          0x02  // always 0
#define CI_SAMPLERATE    0x04  // 0=37.8KHz (level A/B) , 1=18.9KHz (level C)
#define CI_BIT3          0x08  // always 0
#define CI_BITSPERSAMPLE 0x10  // 0=4bits (level B/C), 1=8bits (level A)
#define CI_BIT5          0x20  // always 0
#define CI_EMPHASIS      0x40  // emphasis audio volume
#define CI_RESERVED      0x80  // not used

#define TABLES           2     // even matrix and odd matrix
#define P_ROWS           24    // P: 24*43 matrix with the sector data
#define P_COLUMNS        43
#define P_SIZE           ((P_ROWS) * (P_COLUMNS))
#define P_ROW_INC        (P_COLUMNS)
#define P_COLUMN_INC     1
#define Q_ROWS           26    // Q: 26*43 matrix with the diagonal P matrix
#define Q_COLUMNS        43
#define Q_SIZE           ((Q_ROWS) * (Q_COLUMNS))
#define Q_ROW_INC        (P_COLUMNS)
#define Q_COLUMN_INC     ((P_COLUMN_INC) + (P_ROW_INC))

/*----------------------------------------------------------------------------*/
void CDROM_Init_Tables(void);
char CDROM_NUM2BCD(char value);
char CDROM_BCD2NUM(char value);
int  CDROM_Update(char *sector, int lba, int cdmode, int flags, int check);
int  CDROM_Check(char *sector, int cdmode);
void CDROM_Put_Sync(char *source);
void CDROM_Put_Header(char *source, int lba, int cdmode);
void CDROM_Put_SubHeader(char *source, int flags);
void CDROM_Put_Intermediate(char *source);
void CDROM_Put_EDC(char *source, int length, char *target);
void CDROM_Put_ECC_P(char *source, char *target);
void CDROM_Put_ECC_Q(char *source, char *target);

/*----------------------------------------------------------------------------*/
unsigned int  CDROM_crc[256];
unsigned char CDROM_exp[256];
unsigned char CDROM_log[256];

unsigned int  CDROM_enabled = 0;

/*----------------------------------------------------------------------------*/
void CDROM_Init_Tables(void) {
  unsigned int edc, ecc;
  unsigned int i, j;

  for(i = 0; i < 0x100; i++) {
    edc = i;
    for (j = 0; j < 8; j++) {
      edc = edc & 1 ? (edc >> 1) ^ 0xD8018001 : edc >> 1;
    }
    CDROM_crc[i] = edc;

    ecc = i & 0x80 ? (i << 1) ^ 0x11D : i << 1;
    CDROM_exp[i      ] = ecc;
    CDROM_log[i ^ ecc] = i;
  }
}

/*----------------------------------------------------------------------------*/
char CDROM_NUM2BCD(char value) {
  return(((value / 10) << 4) | (value % 10));
}

/*----------------------------------------------------------------------------*/
char CDROM_BCD2NUM(char value) {
  return(((value >> 4) * 10) | (value & 0xF));
}

/*----------------------------------------------------------------------------*/
int CDROM_Update(char *sector, int lba, int cdmode, int flags, int check) {
  if (!CDROM_enabled) {
    CDROM_Init_Tables();
    CDROM_enabled = 1;
  }

  if (check && CDROM_Check(sector, cdmode)) return(1);

  switch (cdmode) {
    case MODE_M1:
      CDROM_Put_Sync(sector + POS_SYNC);
      CDROM_Put_Header(sector + POS_HEADER, lba, MODE_M1);
      CDROM_Put_Intermediate(sector + POS_INTERMEDIATE);
      CDROM_Put_EDC(sector + POS_SYNC, CALC_EDC_M1, sector + POS_EDC_M1);
      CDROM_Put_ECC_P(sector + POS_HEADER, sector + POS_ECC_P);
      CDROM_Put_ECC_Q(sector + POS_HEADER, sector + POS_ECC_Q);
      break;
    case MODE_M2:
      CDROM_Put_Sync(sector + POS_SYNC);
      CDROM_Put_Header(sector + POS_HEADER, lba, MODE_M2);
      break;
    case MODE_F1:
      CDROM_Put_Sync(sector + POS_SYNC);
    //CDROM_Put_Header(sector + POS_HEADER, lba, MODE_M2);
      CDROM_Put_SubHeader(sector + POS_SUBHEADER, flags);
      CDROM_Put_EDC(sector + POS_SUBHEADER, CALC_EDC_F1, sector + POS_EDC_F1);
      *(unsigned int *)(sector + POS_HEADER) = 0;
      CDROM_Put_ECC_P(sector + POS_HEADER, sector + POS_ECC_P);
      CDROM_Put_ECC_Q(sector + POS_HEADER, sector + POS_ECC_Q);
      CDROM_Put_Header(sector + POS_HEADER, lba, MODE_M2);
      break;
    case MODE_F2:
      CDROM_Put_Sync(sector + POS_SYNC);
      CDROM_Put_Header(sector + POS_HEADER, lba, MODE_M2);
      CDROM_Put_SubHeader(sector + POS_SUBHEADER, flags);
      CDROM_Put_EDC(sector + POS_SUBHEADER, CALC_EDC_F2, sector + POS_EDC_F2);
      break;
    default:
      break;
  }

  return(0);
}

/*----------------------------------------------------------------------------*/
int CDROM_Check(char *sector, int cdmode) {
  int tmp1, tmp2, tmp3;

  switch (cdmode) {
    case MODE_M1:
      if (*(unsigned int *)(sector + POS_SYNC    ) != 0xFFFFFF00) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 4) != 0xFFFFFFFF) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 8) != 0x00FFFFFF) return(1);
      if (sector[POS_MODE] != MODE_M1) return(1);
      if (*(int *)(sector + POS_INTERMEDIATE    )) return(1);
      if (*(int *)(sector + POS_INTERMEDIATE + 4)) return(1);
      // EDC, ECC-P, ECC-Q
      break;
    case MODE_M2:
      if (*(unsigned int *)(sector + POS_SYNC    ) != 0xFFFFFF00) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 4) != 0xFFFFFFFF) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 8) != 0x00FFFFFF) return(1);
      if (sector[POS_MODE] != MODE_M2) return(1);
      break;
    case MODE_F1:
      if (*(unsigned int *)(sector + POS_SYNC    ) != 0xFFFFFF00) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 4) != 0xFFFFFFFF) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 8) != 0x00FFFFFF) return(1);
      if (sector[POS_MODE] != MODE_M2) return(1);
      tmp1 = *(int *)(sector + POS_SUBHEADER);
      tmp2 = *(int *)(sector + POS_SUBHEADER + 4);
      tmp3 = sector[POS_SUBMODE] & SM_FORM2;
      if (!tmp1 || (tmp1 != tmp2) || tmp3) return(1);
      tmp1 = sector[POS_SUBMODE] & SM_VIDEO;
      tmp2 = sector[POS_SUBMODE] & SM_AUDIO;
      tmp3 = sector[POS_SUBMODE] & SM_DATA;
      if (tmp1 && (tmp2 || tmp3)) return(1);
      if (tmp2 && (tmp3 || tmp1)) return(1);
      if (tmp3 && (tmp1 || tmp2)) return(1);
      // EDC, ECC-P, ECC-Q
      break;
    case MODE_F2:
      if (*(unsigned int *)(sector + POS_SYNC    ) != 0xFFFFFF00) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 4) != 0xFFFFFFFF) return(1);
      if (*(unsigned int *)(sector + POS_SYNC + 8) != 0x00FFFFFF) return(1);
      if (sector[POS_MODE] != MODE_M2) return(1);
      tmp1 = *(int *)(sector + POS_SUBHEADER);
      tmp2 = *(int *)(sector + POS_SUBHEADER + 4);
      tmp3 = sector[POS_SUBMODE] & SM_FORM2;
      if (!tmp1 || (tmp1 != tmp2) || !tmp3) return(1);
      tmp1 = sector[POS_SUBMODE] & SM_VIDEO;
      tmp2 = sector[POS_SUBMODE] & SM_AUDIO;
      tmp3 = sector[POS_SUBMODE] & SM_DATA;
      if (tmp1 && (tmp2 || tmp3)) return(1);
      if (tmp2 && (tmp3 || tmp1)) return(1);
      if (tmp3 && (tmp1 || tmp2)) return(1);
      // EDC
      break;
    default:
      return(1);
      break;
  }

  return(0);
}

/*----------------------------------------------------------------------------*/
void CDROM_Put_Sync(char *source) {
  *(unsigned int *)(source    ) = 0xFFFFFF00;
  *(unsigned int *)(source + 4) = 0xFFFFFFFF;
  *(unsigned int *)(source + 8) = 0x00FFFFFF;
}

/*----------------------------------------------------------------------------*/
void CDROM_Put_Header(char *source, int lba, int cdmode) {
  lba += 150;

  *(source    ) = CDROM_NUM2BCD((lba / 75) / 60);
  *(source + 1) = CDROM_NUM2BCD((lba / 75) % 60);
  *(source + 2) = CDROM_NUM2BCD((lba % 75)     );
  *(source + 3) = cdmode;
}

/*----------------------------------------------------------------------------*/
void CDROM_Put_SubHeader(char *source, int flags) {
  *(unsigned int *)(source    ) = flags;
  *(unsigned int *)(source + 4) = flags;
}

/*----------------------------------------------------------------------------*/
void CDROM_Put_Intermediate(char *source) {
  *(unsigned int *)(source    ) = 0x00000000;
  *(unsigned int *)(source + 4) = 0x00000000;
}

/*----------------------------------------------------------------------------*/
void CDROM_Put_EDC(char *source, int length, char *target) {
  unsigned int edc;
  unsigned int i;

  edc = 0;
  for (i = 0; i < length; i++) {
    edc = (edc >> 8) ^ CDROM_crc[(edc ^ source[i]) & 0xFF];
  }

  *(unsigned int *)target = edc;
}

/*----------------------------------------------------------------------------*/
void CDROM_Put_ECC_P(char *source, char *target) {
  unsigned int  table, column, row, index;
  unsigned char ecc, xor;

  for (table = 0; table < TABLES; table++) {
    for (column = 0; column < P_COLUMNS; column++) {
      ecc = xor = 0;
      for (row = 0; row < P_ROWS; row++) {
        index = (P_COLUMN_INC * column + P_ROW_INC * row) % P_SIZE;
        ecc ^= source[TABLES * index + table]; ecc = CDROM_exp[ecc];
        xor ^= source[TABLES * index + table];
      }
      ecc = CDROM_log[CDROM_exp[ecc] ^ xor];

      target[TABLES *  column              + table] = ecc;
      target[TABLES * (column + P_COLUMNS) + table] = ecc ^ xor;
    }
  }
}

/*----------------------------------------------------------------------------*/
void CDROM_Put_ECC_Q(char *source, char *target) {
  unsigned int  table, row, column, index;
  unsigned char ecc, xor;

  for (table = 0; table < TABLES; table++) {
    for (row = 0; row < Q_ROWS; row++) {
      ecc = xor = 0;
      for (column = 0; column < Q_COLUMNS; column++) {
        index = (Q_ROW_INC * row + Q_COLUMN_INC * column) % Q_SIZE;
        ecc ^= source[TABLES * index + table]; ecc = CDROM_exp[ecc];
        xor ^= source[TABLES * index + table];
      }
      ecc = CDROM_log[CDROM_exp[ecc] ^ xor];

      target[TABLES *  row           + table] = ecc;
      target[TABLES * (row + Q_ROWS) + table] = ecc ^ xor;
    }
  }
}

/*----------------------------------------------------------------------------*/
#endif

/*----------------------------------------------------------------------------*/
/*--  EOF                                           Copyright (C) 2012 CUE  --*/
/*----------------------------------------------------------------------------*/
