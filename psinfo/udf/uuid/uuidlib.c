/*****************************************************************
 * File Name:        uuidlib.c
 * Description:
 * This module contains UDFs to generate and manipulate uuids.
 *
 * Software distributed under the License is distributed on an
 * "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express
 * or implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
*****************************************************************/

#include <stdlib.h>
#include <string.h>
#include "ibase.h"
#include <time.h>
#ifdef WIN32
       #include <windows.h>
       #include <winuser.h>
       #include <rpcdce.h>
#else

// # include "uuid.h"
#endif


#include "../example.h"
#include "sysdep.h"

#include "uuid.h"

#include "uuidlib.h"

/*****************************************************************
 * Note.
 * Throughout this code the terms _uuid, uuid, and guid are:
 * _uuid - 128bit (16 byte) raw value (internal only)
 * guid - 36 characters (eg. e902893a-9d22-3c7e-a7b8-d6e313b71d9f)
 * uuid - interbase uuid is 19 char value (see fn_i_uuid_create)
 ****************************************************************/


 #define MASK_1_2        0x03

 #define MASK_1_4        0x0f

 #define MASK_1_6        0x3f
 #define MASK_3_6        0x3c
 #define MASK_5_6        0x30

 #define MASK_3_8        0xfc
 #define MASK_5_8        0xf0
 #define MASK_7_8        0xc0

 #define MASK_5_10        0x03f0

 #define MASK_7_12        0x0fc0

 #define MASK_13_16        0xf000
 #define MASK_11_16        0xfc00

 #define MASK_13_18        0x0003f000

 #define MASK_19_24        0x00fc0000

 #define MASK_25_30 0x3f000000

 #define MASK_31_32 0xc0000000

static char        urlchars[64] = "!*0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/*****************************************************************
 * fn_uuid_create
 * returns the created _uuid as a 22 character, URL compatable,
 * humanly readable string, with the entropy in the low order
 * bits for interbase to efficiently hash it.
 * Valid URL chars A-Z|a-z|0-9|"-"|"_"|"."|"!"|"~"|"*"|"'"|"("|")"
 * and we use a subset of 63 of these, *A-Za-z0-9
****************************************************************/

#ifdef _WINDOWS_
       HMODULE hRPCTR4;
       int Conseguiu=0;
       int Iniciou=0;
       typedef HRESULT __stdcall (*TUuidCreateSequential)(void * lpGUID);
       TUuidCreateSequential crtseq;
#endif

void DoUUID(uuid_tt*u){

#ifdef _WINDOWS_
       if(!Iniciou){
           hRPCTR4=LoadLibrary("rpcrt4.dll");
           if(hRPCTR4!=0){
              crtseq=GetProcAddress(hRPCTR4, "UuidCreateSequential");
              if(crtseq)
                 Conseguiu=1;
              else
                  FreeLibrary(hRPCTR4);
           }
           Iniciou=1;
       }
       if(Conseguiu)
          crtseq(u);
       else
           UuidCreate(u);
#else
        uuid_generate_time(u);
#endif
}

char* EXPORT fn_uuid_create(char* uuid)
{
        uuid_tt        _uuid;
        DoUUID(&_uuid);
        return _uuid_to_uuid(&_uuid, uuid);
}

/*****************************************************************
 *  * fn_idservidor
 *  * returns the ID of this server as 12byte
 *  ****************************************************************/
char* EXPORT fn_idservidor(char* aguid)
{  
           uuid_tt _uuid;
           char guid[50];
           DoUUID(&_uuid);
           _uuid_to_guid(&_uuid, guid);
           strncpy(aguid, &guid[24], 12);
           aguid[12] = 0;
           return aguid;
}


/*****************************************************************
 * fn_guid_create
 * returns the created uuid as a 36 char guid
 ****************************************************************/
char* EXPORT fn_guid_create(char* guid)
{
        uuid_tt _uuid;
        DoUUID(&_uuid);
        return _uuid_to_guid(&_uuid, guid);
}

/*****************************************************************
 * fn_squash_guid
 * returns the passed in guid squashed down to a uuid
 ****************************************************************/
char* EXPORT fn_squash_guid(char* guid, char* uuid)
{
        uuid_tt _uuid;

        guid_to__uuid(guid, &_uuid);
        return _uuid_to_uuid(&_uuid, uuid);
}

/*****************************************************************
 * fn_expand_uuid
 * returns the passed in uuid expanded to a guid
 ****************************************************************/
char* EXPORT fn_expand_uuid(char* uuid, char* guid){
        uuid_tt _uuid;

        uuid_to__uuid(uuid, &_uuid);

        return _uuid_to_guid(&_uuid, guid);
}

/*****************************************************************
 * _uuid_to_guid
 * returns the passed in _uuid as a 36 char guid
 ****************************************************************/
char* EXPORT _uuid_to_guid(uuid_tt* _uuid, char* guid)
{
        int i, j;
        // Copy it into a human readable string
        sprintf(guid,"%8.8x-%4.4x-%4.4x-%2.2x%2.2x-", _uuid->time_low, _uuid->time_mid,
           _uuid->time_hi_and_version, _uuid->clock_seq_hi_and_reserved,
           _uuid->clock_seq_low);
        for (i = 0, j=24; i < 6; i++, j+=2)
           sprintf(guid+j,"%2.2x", _uuid->node[i]);

        return guid;
}


/*****************************************************************
 * guid_to__uuid
 * returns the passed in guid as a _uuid
 ****************************************************************/
char* EXPORT guid_to__uuid(char* guid, uuid_tt* _uuid)
{
        int i,j;

        char *time_low, *time_mid, *time_hi_and_version;
        char *clock_seq_hi_and_reserved, *clock_seq_low;
        char *node[6];
        char *eptr; // dummy for strtol
        char *hi_time_low, *lo_time_low;        // see _WINDOWS_ below. UGH

        time_low = (char*) malloc(8);
        time_mid = (char*) malloc(4);
        time_hi_and_version = (char*) malloc(4);
        clock_seq_hi_and_reserved = (char*) malloc(2);
        clock_seq_low = (char*) malloc(2);
        for(i=0;i<6;i++) {
                node[i] = (char*) malloc(2);
        }

        strncpy(time_low,guid, 8);
        strncpy(time_mid,guid+9,4);
        strncpy(time_hi_and_version,guid+14,4);
        strncpy(clock_seq_hi_and_reserved,guid+19,2);
        strncpy(clock_seq_low,guid+21,2);
        for(i=0,j=24;i<6;i++,j+=2) {
                strncpy(node[i],guid+j,2); //null terminate
        }
#ifdef _WINDOWS_        //Nasty hack to fix lack of strtoll()
        hi_time_low = (char*) malloc(4);
        lo_time_low = (char*) malloc(4);
        strncpy(hi_time_low,guid,4);
        strncpy(lo_time_low,guid+4,4);
        _uuid->time_low = strtol(hi_time_low,&eptr,16);
        _uuid->time_low <<= 16;
        _uuid->time_low |= strtol(lo_time_low,&eptr,16);
        free(hi_time_low);
        free(lo_time_low);
#else
        _uuid->time_low = strtoll(time_low,&eptr,16);
#endif
        _uuid->time_mid = strtol(time_mid,&eptr,16);
        _uuid->time_hi_and_version = strtol(time_hi_and_version,&eptr,16);
        _uuid->clock_seq_hi_and_reserved = strtol(clock_seq_hi_and_reserved,&eptr,16);
        _uuid->clock_seq_low = strtol(clock_seq_low,&eptr,16);
        for(i=0;i<6;i++) {
                _uuid->node[i] = strtol(node[i],&eptr,16);
        }

        free(time_low);
        free(time_mid);
        free(time_hi_and_version);
        free(clock_seq_hi_and_reserved);
        free(clock_seq_low);
        for(i=0;i<6;i++) {
                free(node[i]);
        }
        return _uuid;
}

/*****************************************************************
 * _uuid_to_uuid
 * returns the 19char string from the passed in _uuid
 ****************************************************************/
char* EXPORT _uuid_to_uuid(uuid_tt* _uuid, char* uuid)
{
        char        sq_u[22];
        int                i;

        sq_u[21] = _uuid->time_low & MASK_1_6;
        sq_u[20] = (_uuid->time_low & MASK_7_12) >> 6;
        sq_u[19] = (_uuid->time_low & MASK_13_18) >> 12;
        sq_u[18] = (_uuid->time_low & MASK_19_24) >> 18;
        sq_u[17] = (_uuid->time_low & MASK_25_30) >> 24;
        sq_u[16] = (_uuid->time_low & MASK_31_32) >> 30;

        sq_u[16] |= (_uuid->time_mid & MASK_1_4) << 2;
        sq_u[15] = (_uuid->time_mid & MASK_5_10) >> 4;
        sq_u[14] = (_uuid->time_mid & MASK_11_16) >> 10;

        sq_u[13] = _uuid->time_hi_and_version & MASK_1_6;
        sq_u[12] = (_uuid->time_hi_and_version & MASK_7_12) >> 6;
        sq_u[11] = (_uuid->time_hi_and_version & MASK_13_16) >> 12;

        sq_u[11] |= (_uuid->clock_seq_hi_and_reserved & MASK_1_2) << 4;
        sq_u[10] = (_uuid->clock_seq_hi_and_reserved & MASK_3_8) >> 2;

        sq_u[9] = (_uuid->clock_seq_low & MASK_1_6);
        sq_u[8] = (_uuid->clock_seq_low & MASK_7_8) >> 6;

        sq_u[8] |= (_uuid->node[5] & MASK_1_4) << 2;
        sq_u[7] = ((_uuid->node[5] & MASK_5_8) >> 4) | ((_uuid->node[4] & MASK_1_2) << 4);
        sq_u[6] = (_uuid->node[4] & MASK_3_8) >> 2;
        sq_u[5] = (_uuid->node[3] & MASK_1_6);
        sq_u[4] = ((_uuid->node[3] & MASK_7_8) >> 6) | ((_uuid->node[2] & MASK_1_4) << 2);
        sq_u[3] = ((_uuid->node[2] & MASK_5_8) >> 4) | ((_uuid->node[1] & MASK_1_2) << 4);
        sq_u[2] = ((_uuid->node[1] & MASK_3_8) >> 2);
        sq_u[1] = (_uuid->node[0] & MASK_1_6);
        sq_u[0] = (_uuid->node[0] & MASK_7_8) >> 6; // +4 unused bits

        for (i=0; i<22; i++) {
                uuid[i] = urlchars[(int)sq_u[i]];
        }

        return uuid;
}
extern int sq_to_int(char i);

/*****************************************************************
 * uuid_to__uuid
 * returns the _uuid from the passed in uuid
 ****************************************************************/
uuid_tt* EXPORT uuid_to__uuid(char* uuid, uuid_tt* _uuid)
{
        int i;

        _uuid->time_low = sq_to_int(uuid[16]);
        _uuid->time_low <<= 6;
        _uuid->time_low |= sq_to_int(uuid[17]);
        _uuid->time_low <<= 6;
        _uuid->time_low |= sq_to_int(uuid[18]);
        _uuid->time_low <<= 6;
        _uuid->time_low |= sq_to_int(uuid[19]);
        _uuid->time_low <<= 6;
        _uuid->time_low |= sq_to_int(uuid[20]);
        _uuid->time_low <<= 6;
        _uuid->time_low |= sq_to_int(uuid[21]);

        _uuid->time_mid = sq_to_int(uuid[14]);
        _uuid->time_mid <<= 6;
        _uuid->time_mid |= sq_to_int(uuid[15]);
        _uuid->time_mid <<= 4;
        _uuid->time_mid |= ((sq_to_int(uuid[16]) & MASK_3_6) >> 2);

        _uuid->time_hi_and_version = sq_to_int(uuid[11]);
        _uuid->time_hi_and_version <<= 6;
        _uuid->time_hi_and_version |= sq_to_int(uuid[12]);
        _uuid->time_hi_and_version <<= 6;
        _uuid->time_hi_and_version |= sq_to_int(uuid[13]);

        _uuid->clock_seq_hi_and_reserved = sq_to_int(uuid[10]);
        _uuid->clock_seq_hi_and_reserved <<= 2;
        _uuid->clock_seq_hi_and_reserved |= ((sq_to_int(uuid[11]) & MASK_5_6) >> 4);

        _uuid->clock_seq_low = sq_to_int(uuid[8]);
        _uuid->clock_seq_low <<= 6;
        _uuid->clock_seq_low |= sq_to_int(uuid[9]);

        _uuid->node[0] = (sq_to_int(uuid[0]) << 6);
        _uuid->node[0] |= sq_to_int(uuid[1]);

        _uuid->node[1] = (sq_to_int(uuid[2]) << 2);
        _uuid->node[1] |= (sq_to_int(uuid[3]) >> 4);

        _uuid->node[2] = (sq_to_int(uuid[3]) << 4);
        _uuid->node[2] |= (sq_to_int(uuid[4]) >> 2);

        _uuid->node[3] = (sq_to_int(uuid[4]) << 6);
        _uuid->node[3] |= sq_to_int(uuid[5]);

        _uuid->node[4] = (sq_to_int(uuid[6]) << 2);
        _uuid->node[4] |= (sq_to_int(uuid[7]) >> 4);

        _uuid->node[5] = (sq_to_int(uuid[7]) << 4);
        _uuid->node[5] |= (sq_to_int(uuid[8]) >> 2);

return _uuid;
}

int sq_to_int(char i)
{
        int range=0;

        if (i=='!') range=1;
        if (i=='*') range=2;
        if (i>='0' && i<='9') range=3;
        if (i>='A' && i<='Z') range=4;
        if (i>='a' && i<='z') range=5;

        switch(range) {
                case 1 : return 0;
                        break;
                case 2 : return 1;
                        break;
                case 3 : return (int) i-46;        //0 is 2
                        break;
                case 4 : return (int) i-53;        //A is 12
                        break;
                case 5 : return (int) i-59;        //a is 38
                        break;
                case 0        :
                default        : return -1;
                        break;
        }
}

static int ultimo;

int EXPORT fn_testestatic(ARG(int*, valor))
      ARGLIST(int*valor)
{

      int i=ultimo;
      if(!valor)return 0;
      ultimo=*valor;
      return i;
}

//#if 0
ISC_TIMESTAMP * EXPORT fn_uuid2time(char *uuid,  ISC_TIMESTAMP* pstime)
{
   time_t t,*x;
   uuid_tt _uuid;
   struct tm* tempo;

   if(!uuid||!pstime)
     return pstime;
   uuid_to__uuid(uuid,&_uuid);
#ifdef _WINDOWS_
       x=&_uuid;
       t=*x;
//   return pstime;
#else
   t=uuid_time(&_uuid,NULL);
#endif
   tempo=localtime(&t);
   isc_encode_timestamp(tempo,pstime);
   return pstime;
}
//#endif
