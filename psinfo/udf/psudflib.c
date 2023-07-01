#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ibase.h>
#include "example.h"
#include <stdio.h>

#include <psinfo/ps_utils.h>

#ifdef __LINUX__

    #define ARQUIVOCFG  "/etc/psfarma.dat"
    #define ARQUIVOLOG  "/tmp/logfirebird"
    #define UDFDIR      "/opt/firebird/UDF/"

#else
    #define ARQUIVOCFG        "c:\\psfarma.dat"
    #define ARQUIVOLOG  "c:\\logfirebird.txt"
    #define UDFDIR   "c:\\Arquivos de Programas\\Firebird\\UDF\\"
#endif /* __LINUX__*/

#ifndef IB_START_YEAR
        #define IB_START_YEAR 1900
#endif

#define AJUSTEDADATA  3

struct SPSDados { int idloja; int replicar; };
typedef struct SPSDados TSPSDados;

TSPSDados PSDados={0,0};

/*int idloja=0;*/

typedef struct blob {
        short        (*blob_get_segment) ();
        void        *blob_handle;
        long        blob_number_segments;
        long        blob_max_segment;
        long        blob_total_length;
        void        (*blob_put_segment) ();
} *BLOB;




#if 0
#define LOGUDF(fase,msg) \
{ \
   fn_pslogfirebird(fase);\
   fn_pslogfirebird(msg);\
}
#else
#define LOGUDF(fase,msg)
#endif


void lerdados(void)
{
   if(PSDados.idloja<1)
     {
        FILE* f=fopen(ARQUIVOCFG,"r+");
        if(f)
          {
             fread(&PSDados,1,sizeof(PSDados),f);
             fclose(f);
          }
     }
}

int gravardados(void)
{
   FILE* f;
   f=fopen(ARQUIVOCFG,"w+");
   if(!f)return;
   fwrite(&PSDados,1,sizeof(PSDados),f);
   fclose(f);
}

long EXPORT fn_pssetreplicar(ARG(int *, novo))
  ARGLIST(int* novo)
{
   if(!novo) return 1;
   PSDados.replicar=*novo==0?0:1;
   gravardados();
   return 0;
}

long EXPORT fn_psreplicar(void)
{
   lerdados();
   return PSDados.replicar;
}

long EXPORT fn_pssetidloja(ARG(int *, novoid))
ARGLIST(int* novoid)
{
   if(!novoid) return 1;
   PSDados.idloja=*novoid<0?0:*novoid;
   gravardados();
   return 0;
}

long EXPORT fn_psidloja(void)
{
   lerdados();
   return PSDados.idloja;
}

/* Retorna o proprio numero */
long EXPORT fn_bllnumero(ARG(int *, l1))
ARGLIST(int* l1)
{
   return l1?*l1:l1;
}


long EXPORT fn_psmod(ARG(int *, l1),ARG(int *, l2))
ARGLIST(int* l1)
ARGLIST(int* l2)
{
    if(!l1||!l2||*l2==0)return 0;
    return *l1%*l2;
}

/*
    Função que retorna 0 se senha é inválida ou
    1 se senha é válida

    Nome real da funcao ConfereSenha
*/

int EXPORT fn_psfnp1 (ARG(BLOB, b),ARG(char*,senha))
ARGLIST(BLOB b)
ARGLIST(char* senha)
{
    char *buf,*p,*s=senha;
        short length, actual_length;
    int l=0;
    int r=0;
        /* Valores nulos */
        if (!b->blob_handle||!senha)
                return 0;

    length=b->blob_max_segment+1;
        buf=(char*)malloc(length);
    while ((*b->blob_get_segment)(b->blob_handle, buf,length,&actual_length)){
            buf[actual_length]=0;
        p=buf;
        while(*p){
            if(!*s||*p++!=*s++)
                goto erro;
            l++;
        }
    }
    if(l==strlen(senha))
        r=1;
  erro:
    free(buf);
    return r;
}

/*
  Função que criptografa a senha
  Nome real da funcao GeraSenha
*/

void EXPORT fn_psfnp2 (ARG(char*,novasenha),ARG(BLOB,destino))
ARGLIST(char* novasenha)
ARGLIST(BLOB destino)
{
        short length;
        /* Valores nulos */
        if (!destino->blob_handle||!novasenha)
                return;
    length=strlen(novasenha);
    if(!length)return;
    (*destino->blob_put_segment)(destino->blob_handle, novasenha,length);
    return;
}

/*
  Função para adicao de strings em blobs
*/

void EXPORT fn_blobplusstr (ARG(BLOB,origem),ARG(char*,texto),ARG(BLOB,destino))
ARGLIST(BLOB origem)
ARGLIST(char* texto)
ARGLIST(BLOB destino)
{
        short length;
        char *buffer;
        short actual_length;
        /* Valores nulos */
        if (!destino||!destino->blob_handle||!origem||!origem->blob_handle||!texto)
                return;
        length = origem->blob_max_segment;   
        buffer = (char*)malloc(length+1);

        while ((*origem->blob_get_segment)(origem->blob_handle, buffer,length,&actual_length))
     	   (*destino->blob_put_segment)(destino->blob_handle, buffer, actual_length );

        free ( buffer );
        buffer = NULL;
        length=strlen(texto);
        
        
        if(!length)return;
        (*destino->blob_put_segment)(destino->blob_handle, texto, length);
        return;
}

long EXPORT fn_psdow (ARG(ISC_DATE*, a))
ARGLIST(ISC_DATE *a)
{
   if(a&&*a!=0)
      return ((*a+AJUSTEDADATA)%7)+1;
   else
      return 0;
}

int rndiniciado=0;

long EXPORT fn_psrandomize (void)
{
#ifndef __LINUX__
    randomize();
#endif
    return 1;
}

long EXPORT fn_psrandom (ARG(int* ,n))
ARGLIST(int*n)
{
    if(!rndiniciado){
        rndiniciado=1;
#ifndef __LINUX__
        randomize();
#endif
    }
#ifdef __LINUX__
    return 1+(int)(((double)*n)*rand()/(RAND_MAX+1.0));
#else
    return random(*n)+1;
#endif
}

char* EXPORT fn_psrua (ARG(char*, s))
ARGLIST(char *s)
{
    char tmp[31];
    char *buf;
    char*buffer;
    if(!s||!*s)return NULL;
    buffer=(char*)malloc(31);
    strncpy(tmp,s,sizeof(tmp)-1);
    tmp[30]=0;
    s=tmp;
    buf=buffer;
    while(*s&&*s!=','&&*s!='-')
        *buf++=*s++;
    *buf=0;
    return buffer;
}

char* EXPORT fn_pstelefonelimpo (ARG(char*, s))
ARGLIST(char *s)                        /* VARCHAR input */
{
        char *buf;
        short length = 0;
        char *buffer;
        length=(short)*s;
    buffer=(char*)malloc(length+1);
    s+=2;
        buf=buffer;
        while (*s){
        if(*s>='0'&&*s<='9')
            *buf++=*s;
        s++;
    }
        *buf=0;
        return buffer;
}

char* EXPORT fn_formatar_cnpjcpf (ARG(char*, s))
ARGLIST(char *s)                        /* VARCHAR input */
{
        char *buf;
        short length=0;
        char *buffer;
    if(!s)return s;
        length=(short)*s; 
    buffer=(char*)malloc(21);
    if(length==0) {
       buffer[0]=0;
       return buffer;
    }   
    s+=2;
    return PSFormatar_CNPJCPF(s,buffer,21);
}

int EXPORT fn_cnpj_valido(ARG(char*, s))
ARGLIST(char *s)                        /* VARCHAR input */
{
    return s?PSCNPJ_Valido(s+2):0;
}

int EXPORT fn_cpf_valido(ARG(char*, s))
ARGLIST(char *s)                        /* VARCHAR input */
{
    return s?PSCPF_Valido(s+2):0;
}

char * EXPORT fn_ps_crypto(ARG(char*, s))
ARGLIST(char *s)                        /* VARCHAR input */
{

/*select a.*, pscrypto(senha) from usuarios a*/
        short length=0;
        char *buffer;
    if(!s) return NULL;

        length=(short)*s;
    if(length<1){
        return NULL;
    }
    buffer=(char*)malloc(DIGESTLENDBL+11);
    s+=2;
    return PSCrypto(s,strlen(s),buffer,DIGESTLENDBL+1);
}

#if 0
int EXPORT fn_ps_cryptocmp(ARG(char*, s))
ARGLIST(char *s)                        /* VARCHAR input */
{
/* retorna 0 se ok */
        short length=0;
    if(!s) return 1;
        length=(short)*s;
    if(length<1) return 1;
    s+=2;
    return PSCompareCrypto(s,strlen(s),buffer,DIGESTLENDBL+1);
}
#endif

ISC_UINT64 EXPORT fn_ps_testelargeint(ARG(ISC_UINT64*, s))
ARGLIST(ISC_UINT64 *s)                        /* ISC_INT64 input */
{
    unsigned short *j;
    ISC_UINT64 r=0;
    if(!s)return r;
    r=*s;
    j=&r;
    j[3]=(unsigned short)(fn_psidloja());
    return r;
}

int EXPORT fn_pscmplarge(ARG(ISC_UINT64*, l), ARG(ISC_UINT64*, r) )
ARGLIST(ISC_UINT64 *l)
ARGLIST(ISC_UINT64 *r)
{
    if(l==r)return 1;
    if(!l)
        return 0;
    else if(!r)
        return 0;
    else
        return *r==*l;
}

int EXPORT fn_pscmpdate(ARG(ISC_DATE*, l), ARG(ISC_DATE*, r) )
ARGLIST(ISC_DATE *l)
ARGLIST(ISC_DATE *r)
{
    if(l==r)return 1;
    if(!l)
        return 0;
    else if(!r)
        return 0;
    else
        return *r==*l;
}

int EXPORT fn_pscmpstr(ARG(char*, l), ARG(char*, r) )
  ARGLIST(char *l)
    ARGLIST(char *r)
{

       if(l==r)return 1;
       if(!l)
             return 0;
       else if(!r)
             return 0;
       else
             return strcmp(l,r)==0;
}

int EXPORT fn_pscmptimestamp(ARG(ISC_TIMESTAMP*, l), ARG(ISC_TIMESTAMP*, r) )
  ARGLIST(ISC_TIMESTAMP *l)
    ARGLIST(ISC_TIMESTAMP *r)
{

       if(l==r)return 1;
       if(!l)
             return 0;
       else if(!r)
             return 0;
       else
          return r->timestamp_date==l->timestamp_date&&r->timestamp_time==l->timestamp_time;
}


int EXPORT fn_psperiodo(ARG(ISC_DATE*, data), ARG(ISC_DATE*, datainicial), ARG(ISC_LONG*, dias) )
ARGLIST(ISC_DATE *data)
ARGLIST(ISC_DATE *datainicial)
ARGLIST(ISC_LONG *dias)

{
        int d=dias?*dias:1;
    int diferenca;
    if(!data||!datainicial)return 0;
    diferenca=*data-*datainicial;
    if(d<1)
            d=1;
    return (diferenca/d)+1;
}

char* EXPORT fn_pspad (ARG(char*, s),ARG(int*,tamanho))
ARGLIST(char *s)                        /* VARCHAR input */
ARGLIST(int *tamanho)                        /* VARCHAR input */
{
        char *buf;
        short length=0;
        char *buffer;
    int size;
    int x=0;
    if(!tamanho){
        buffer=malloc(1);
        *buffer=0;
        return buffer;
    }
    size=*tamanho;
    if(!s)return s;
    if(size<1){
        buffer=malloc(1);
        *buffer=0;
        return buffer;
    }
        length=(short)*s;
    buffer=(char*)malloc(size+1);
    buffer[0]=0;
    if(length==0)return buffer;
    s+=2;
    buf=s;
    x=strlen(buf);
    buffer[size]=0;
    while(size>0){
        if(x<1){
            while(size--)
                buffer[size]=' ';
        } else
            buffer[--size]=buf[--x];
    }
    return buffer;
}

char* EXPORT fn_psupper (ARG(char*, s)) 
ARGLIST(char *s)
{
    short length;
    char *buffer;
    if(!s)return s;
        length=(short)*s;
    s+=2;
    if(length<0)return NULL;
    buffer=(char*)malloc(length+5);
/*
   if ( length > 280 ) length = 280;
*/
    buffer[length]=0;
    if(length==0)return buffer;
//    strncpy(buffer,s,length);
    PSUpperCase2(0,s,length,buffer);
    return buffer;
}

int* EXPORT fn_psupperlike (ARG(char*, s), ARG(char*, str ) ) 
ARGLIST(char *s)
ARGLIST(char *str)    
{
    short length;
    char *buffer;   
    if(!s||!str)return 0;
    buffer = fn_psupper ( s );
    if ( !buffer ) return 0;
    str+=2;
    free(buffer);
    return 0;
}

char* EXPORT fn_psupperblob (ARG(BLOB, blob)) 
ARGLIST(BLOB blob)
{
    short length;
    long l1,al1,lidos;
    char *buffer, *buffer2;
    if(!blob)return blob;
    length=(short)blob->blob_total_length;
    if(length<0)return NULL;
    buffer=(char*)malloc(length+5);
    buffer[length]=0;
    if(length==0)return buffer;
    l1=blob->blob_max_segment+1;
    LOGUDF("fn_psuppercmpblob","Inicio do WHILE...");
    lidos=al1=0;
    while ((*blob->blob_get_segment)(blob->blob_handle, &buffer[lidos],l1,&al1))
       lidos+=al1;
   
    PSUpperCase(0,buffer,buffer);
    return buffer;
}

char* EXPORT fn_blob2txt (ARG(BLOB, blob)) 
ARGLIST(BLOB blob)
{
    short length;
    long l1,al1,lidos;
    char *buffer, *buffer2;
    if(!blob)return blob;
    length=(short)blob->blob_total_length;
    if(length<0)return NULL;
    buffer=(char*)malloc(length+5);
    buffer[length]=0;
    if(length==0)return buffer;
    l1=blob->blob_max_segment+1;
    LOGUDF("fn_blob2txt","Inicio do WHILE...");
    lidos=al1=0;
    while ((*blob->blob_get_segment)(blob->blob_handle, &buffer[lidos],l1,&al1))
       lidos+=al1;
   
    return buffer;
}


int EXPORT fn_psuppertbl (ARG(int *, tabela), ARG(unsigned char*, entrada),ARG(unsigned char*, saida))
ARGLIST(int *tabela)
ARGLIST(unsigned char *entrada)
ARGLIST(unsigned char *saida)
{
    if(!entrada||!saida||!tabela)return 1;
        if((short)*entrada<1||(short)*saida<1)return 2;
    if(*tabela<0||*tabela>1)return 3;
    saida+=2;
    entrada+=2;
    PSGerarByteUpperCase(*tabela,*entrada,*saida);
    return 0;
}

int EXPORT fn_psuppercasechar (ARG(int *, tabela),ARG(unsigned int*, entrada))
ARGLIST(int *tabela)
ARGLIST(unsigned int *entrada){
    if(!entrada||!tabela)return 0;
    return PSUpperCaseChar(*tabela,*entrada);
}

double EXPORT fn_pscoeficiente (ARG(double *, vl1),ARG(double*, vl2))
ARGLIST(double *vl1)
ARGLIST(double *vl2){
        /*Objetivo:
            calcular o coeficiente entre vl1 e vl2
            VL1: Valor Base
        VL2: Valor p/ cálculo
    */
    if(!vl1||!vl2)
            return 0.0;
    if(*vl1==0.00)
            return 0.0;
    else
            return (*vl2/(*vl1))*100.0;
}

double EXPORT fn_psmargemaplicada (ARG(double *, vl1),ARG(double*, vl2))
  ARGLIST(double *vl1)
    ARGLIST(double *vl2)
{
   
           /*Objetivo:
	    *             calcular a margem aplicada entre vl1 e vl2
	    *             VL1: Valor Base
	    *         VL2: Valor p/ cálculo
	    *     */
       if(!vl1||!vl2)
                 return 0.0;
       if(*vl2==0.00)
                 return 0.0;
       else
                 return ((*vl1-*vl2)/(*vl2))*100.0;
}




double DiasPrioridades[]={
                1.0,
                3.0,
                6.0,
                12.0,
                20.0,
                30.0,
                45.0,
                55.0,
                60.0,
                70.0,
                80.0,
                90.0
        };

double EXPORT fn_psprioridade(ARG(int *, atraso),ARG(int*, prioridade))
ARGLIST(int *atraso)
ARGLIST(int *prioridade){
    /*
        Objetivo:
            calcular um coeficiente de prioridade
            atraso: Dias de Atraso
        prioridade: Nivel da prioridade (1 Maior / 4 Menor
    */
    double a;
    int p;
    if(!prioridade)
            return 0.0;
    p=*prioridade;
    if(!atraso)
       a=0;
    else
       a=*atraso;
    if(p<1||p>10)
        p=1;
    if(a<0)
        a=0;
    return a/DiasPrioridades[p-1];
}

int EXPORT fn_psbitset(ARG(int *, valor),ARG(int*, bit))
ARGLIST(int *valor)
ARGLIST(int *bit){
    /*
        Objetivo:
            calcular se o bit está ativo
            atraso: Valor desejado
        prioridade: Bit (0 à 31)
        retorna: 1 se está ativo, 0 caso contrario
    */
    int v, b, p;
    if(!valor||!bit)
            return 0;
    v=*valor;
    b=*bit;
    if(b<0||b>31)
       return 0;
    p=powl(2,b);
    return p&v;
}

BLOB EXPORT fn_file2blob (ARG(char*, s), ARG(BLOB,b1))
    ARGLIST(char *s)                        /* VARCHAR input */
    ARGLIST(BLOB b1)
{
   
      FILE* f=NULL;
      if(!s||!*s||!b1||!b1->blob_handle){
//	 fn_pslogfirebird("retornando sem importar...");
	 return 0;
      }
   
      f=fopen(s,"r");
      if(f)
      {
	 int length;
	 void *buffer;
	 int lidos;
	 length = 40;/*b1->blob_max_segment;*/
	 buffer = malloc(length+1);
         while ( 1 ) {
	    lidos = fread ( buffer, 1, length, f );
//    fn_pslogfirebird("Lidos...");

	    if ( lidos < 1 ) {
//	       fn_pslogfirebird("Lidos... < 1");
	       
	       
	       fclose ( f );
	       free(buffer);
	       break;
	    }
	    
	    (*b1->blob_put_segment)(b1->blob_handle, buffer, lidos );
	 } 
      } else {   
//	fn_pslogfirebird("Erro no arquivo...");
//	fn_pslogfirebird(s);
      }
   	
      return b1;
}

int EXPORT fn_blob2file (ARG(char*, s), ARG(BLOB,b1))
    ARGLIST(char *s)                        /* VARCHAR input */
    ARGLIST(BLOB b1)
{
   int total;
   
      FILE* f=NULL;
      if(!s||!*s||!b1||!b1->blob_handle){
//	 fn_pslogfirebird("retornando sem importar...");
	 return -1;
      }
   
      f=fopen(s,"w");
      if(f)
      {
	 short length, actual_length;
	 char *buffer;
	 int total = 0;
	 length = b1->blob_max_segment;
	 if ( length < 1 ) 
	   {
	      fclose(f);
	      return -2;
	   }
	 
	 buffer = (char*)malloc(length+1);
	 while ( ( *b1->blob_get_segment ) ( b1->blob_handle, buffer, length, &actual_length ) ) 
	   {
	      if ( actual_length < 1 ) 
		{
		   free(buffer);
		   fclose(f);
		   return total;		   
		}
	      total+=actual_length;
	      fwrite ( buffer, 1,  actual_length, f );
	   }
	 fclose(f);
	 free(buffer);
	 return total;
      } else 
     return -3;
   
}

int EXPORT fn_pscmpblob (ARG(BLOB, b1),ARG(BLOB, b2))
  ARGLIST(BLOB b1)
  ARGLIST(BLOB b2)
{
   char *buf1,buf2;
   char BufferLog[1024];
   long l1,l2,al1,al2;
   int r=1;
   int i;
   if(b1&&!b2)
     return 0;
   else if(!b1&&b2)
     return 0;
   else if(b1==b2)
     return 1;
   if(b1->blob_handle&&!b2->blob_handle)
     return 0;
   else if(!b1->blob_handle&&b2->blob_handle)
     return 0;
   else if(b1->blob_handle==b2->blob_handle){
        LOGUDF("fn_pscmpblob","Handle são iguais...");
        return 1;
   }
   if(b1->blob_total_length!=b2->blob_total_length)
     return 0;
   l1=b1->blob_max_segment+1;
   l2=b2->blob_max_segment+1;
   buf1=(char*)malloc(l1);
   LOGUDF("fn_pscmpblob","Inicio do WHILE...");
   al1=0;
   while ((*b1->blob_get_segment)(b1->blob_handle, buf1,l1,&al1))
     {
        sprintf(BufferLog, "1 - %u - %u",al1,l1);
        LOGUDF("fn_pscmpblob",BufferLog);
        i=0;
        al2=0;
        while(i<al1&&(*b2->blob_get_segment)(b2->blob_handle, &buf2,1,&al2))
          {
             sprintf(BufferLog, "2 - %u - %u",al2, l2);
             LOGUDF("fn_pscmpblob",BufferLog);
             if(buf1[i++]!=buf2){
                LOGUDF("fn_pscmpblob","Valores diferentes...");
                r=0;
                goto erro;
             }
          }
     }
   LOGUDF("fn_pscmpblob","Fim do WHILE...");
   erro:
       free(buf1);
       return r;
}


int EXPORT fn_pslogfirebird (ARG(char*, s))
  ARGLIST(char *s)                        /* VARCHAR input */
{
   FILE* f=NULL;
   if(!s||!*s)return 0;
   f=fopen(ARQUIVOLOG,"a+");
   if(f){
      fprintf(f,"%s\n",s);
      fclose(f);
   }
   return 1;
}

/*fn_psidlojacodigo64:
 * Objetivo: Juntar dois numeros de 32 bits em 1 unico de 64 bits
 * N1 -> parte alta
 * N2 -> parte baixa
 * */
ISC_UINT64 *EXPORT fn_psidlojacodigo64(ARG(unsigned int*, n1), ARG(unsigned int*,n2))
  ARGLIST(unsigned int*n1)
  ARGLIST(unsigned int*n2){
     unsigned int *n;
     if(!n1||!n2)
       return NULL;
     n=(unsigned int*)malloc(sizeof(ISC_UINT64));
     n[0]=*n2;
     n[1]=*n1;
     return n;
}

/*fn_psidlojacodigo_int:
 *  * Objetivo: Juntar dois numeros de 32 bits em 1 unico de 32 bits
 *  * N1 -> parte alta  - 10 bits
 *  * N2 -> parte baixa - 22 bits
 *  * */
unsigned int *EXPORT fn_psidlojacodigo32(ARG(unsigned int*, n1), ARG(unsigned int*,n2))
    ARGLIST(unsigned int*n1)
      ARGLIST(unsigned int*n2)
{

        unsigned int *n;
        unsigned int x;
        if(!n1||!n2)
            return NULL;
        n=(unsigned int*)malloc(sizeof(unsigned int));
        *n=*n2&0x003fffff;
        x=*n1;
        x<<=22;
        *n|=x;
        return n;
}

char *EXPORT fn_udfdir ( void ) 
{ 
   return UDFDIR;
}

char *EXPORT fn_psextrairnumeros(ARG(char*, c))
      ARGLIST(char *c)
{
   int l;
   char *d;
   if(!c)return c;
   l=strlen(c);
   d=(char*)malloc(l+1);
   strcpy(d,c);
   return Extrair_Numeros(d);
}

char *EXPORT fn_gerardigitos2(ARG(char*, c), ARG(unsigned int*, digitos ) )
      ARGLIST(char *c)
      ARGLIST(unsigned int*digitos)
{
   char *d;
   char *buf[256];
   if(!c)return c;
   if(!digitos)return NULL;
   strcpy(buf,c);
   Extrair_Numeros(buf);
   Gerar_Digitos_2(buf, *digitos);
   d=(char*)malloc(strlen(buf)+1);
   strcpy(d,buf);
   return d;
}

char *EXPORT fn_gerardigitoEAN(ARG(char*, c) )
      ARGLIST(char *c)
{
   char *d;
   char *buf[256];
   if(!c)return c;
   strcpy(buf,c);
   Extrair_Numeros(buf);
   Gerar_DigitoEAN(buf);
//   fn_pslogfirebird(buf);
   d=(char*)malloc(strlen(buf)+1);
   strcpy(d,buf);
   return d;
}

int EXPORT fn_roundstep(ARG(int*, valor), ARG(int*, passo))
  ARGLIST(int*valor)
  ARGLIST(int*step)
      /* Tem por objetivo arredondar um valor para cada passo...
      * ex.: 3 com passo=5 arredonda para 5
      *    3 com passo=10 arredonda para 10
      *    11 com passo 5 arredonda para 15
      *    11 com passo=3 arredonda para 12
      *
      */


{
   int i,j;
   if(!passo||!valor)return 0;/*retorna pois naun recebemos um valor*/
   if((*passo)<1)return *valor; /*retornamos se o passo é inválido*/
   i=(*valor)%(*passo);
   j=((*valor)-i) /(*passo);
   i=(*passo)*j;
   return (i<(*valor))?(i+(*passo)):i;
}

int EXPORT fn_testestatic(ARG(int*, valor))
    ARGLIST(int*valor)
{
   static int ultimo;
   int i=ultimo;//valorestatico;
   if(!valor)return 0;
   ultimo/*valorestatico*/=*valor;
   return i;
}

int EXPORT fn_mesesentredatas(ARG(ISC_DATE*, di), ARG(ISC_DATE*, df) )
ARGLIST(ISC_DATE *l)
ARGLIST(ISC_DATE *r)
{
    struct tm tm1;
    int mesesi, mesesf;
    if(di==df)return 0;
    if(!di)return 0;
    else if(!df)return 0;
    isc_decode_date(di, &tm1);
    mesesi=(tm1.tm_year*12)+tm1.tm_mon+1;
    isc_decode_date(df, &tm1);
    mesesf=(tm1.tm_year*12)+tm1.tm_mon+1;
    return mesesf-mesesi;
}


# define Myisleap(year) \
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))




long EXPORT fn_daypermonth(ARG(long*, m), ARG(long*, y))
ARGLIST(long *m)
ARGLIST(long *y)
{
        short daysinmonth[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if ((*m < 1 ) || (*m > 12)) return 0;
        return ((*m==2) && Myisleap(*y))
                                 ? daysinmonth[*m-1]+1 : daysinmonth[*m-1] ;
}

struct tm * _incdate(struct tm* tm1, long d, long m, long y)
{
        div_t x;
        long dd, mm, yy, dsign, dpm;

        dd = tm1->tm_mday;
        mm = tm1->tm_mon + 1;
        yy = tm1->tm_year += IB_START_YEAR;

        dpm = fn_daypermonth(&mm, &yy) - dd;
        yy += y;

        dsign = (mm + m < 1) ? -1 : 1;

        x = div(dsign * (mm + m - 1), 12);

        yy = yy + dsign * x.quot;

        if (dsign+1)
                mm = x.rem + 1;
        else
        {
                yy = yy - 1;
                mm = 12 - x.rem + 1;
        }

        if (!dpm)
                dd = fn_daypermonth(&mm, &yy);
        else
                dd = tm1->tm_mday;
        dd = (dd > fn_daypermonth(&mm, &yy)) ? fn_daypermonth(&mm, &yy) : dd;

        dd += d;
        dsign = (dd < 0) ? 0 : 1;
        if (dsign==1)
        {
                while ((dpm = fn_daypermonth(&mm, &yy)) < dd)
                {
                        dd -= dpm;
                        if (++mm > 12)
                        {
                                yy++;
                                mm = 1;
                        }
                }
        }
        else
        {
                while (dd <= 0)
                {
                        if (!--mm)
                        {
                                yy--;
                                mm = 12;
                        }
                        dd += dpm = fn_daypermonth(&mm, &yy);
                }
        }

        tm1->tm_mday = dd;
        tm1->tm_mon = mm - 1;
        tm1->tm_year = yy - IB_START_YEAR;

        return tm1;
}

ISC_DATE EXPORT fn_incdate(ARG(ISC_DATE*, dt), ARG(long*, d), ARG(long*, m), ARG(long*, y) )
ARGLIST(ISC_DATE* dt)
ARGLIST(long *d)
ARGLIST(long *m)
ARGLIST(long *y)
{
        struct tm tm1;
        ISC_DATE data;
        if ( !dt )
              return NULL;
//        data=(ISC_DATE*)malloc(sizeof(ISC_DATE));

        isc_decode_sql_date(dt, &tm1);

        _incdate(&tm1, d?*d:0, m?*m:0, y?*y:0);

        isc_encode_sql_date(&tm1, &data);

//        data = 5000;

        return data;
}

ISC_DATE EXPORT fn_datefirstdaymonth(ARG(ISC_DATE*, dt) )
ARGLIST(ISC_DATE* dt)
{
        struct tm tm1;
        ISC_DATE data;
        if ( !dt )
              return NULL;

        isc_decode_sql_date(dt, &tm1);

        tm1.tm_mday = 1;

        isc_encode_sql_date(&tm1, &data);

        return data;
}

ISC_DATE EXPORT fn_datelastdaymonth(ARG(ISC_DATE*, dt) )
ARGLIST(ISC_DATE* dt)
{
        struct tm tm1;
        long mes, ano;
        ISC_DATE data;
         if ( !dt )
              return NULL;

        isc_decode_sql_date(dt, &tm1);

        mes = tm1.tm_mon+1;
        ano = tm1.tm_year+IB_START_YEAR;

        tm1.tm_mday = fn_daypermonth( &mes, &ano );

        isc_encode_sql_date(&tm1, &data);

        return data;
}

int EXPORT fn_strlen(ARG(char*, l) ) 
  ARGLIST(char *l)
{
   
          if(l==NULL)return 0;
          return strlen(l);
}


#if 0
Relativos a soma...
#define dtype_null      0
#define dtype_text      1
#define dtype_cstring   2
#define dtype_varying   3

#define dtype_packed    6
#define dtype_byte      7
#define dtype_short     8
#define dtype_long      9
#define dtype_quad      10
#define dtype_real      11
#define dtype_double    12
#define dtype_d_float   13
#define dtype_sql_date  14
#define dtype_sql_time  15
#define dtype_timestamp 16
#define dtype_blob      17
#define dtype_array     18
#define dtype_int64     19
#define DTYPE_TYPE_MAX  20

typedef struct paramdsc 
{
   
       unsigned char       dsc_dtype;
       signed char         dsc_scale;
       ISC_USHORT          dsc_length;
       short               dsc_sub_type;
       ISC_USHORT          dsc_flags;
       unsigned char       *dsc_address;
}
 PARAMDSC;
#endif

int EXPORT fn_subtipocampo ( ARG(PARAMDSC*, b))
      ARGLIST(PARAMDSC * b)
{  
      if ( b ) {
	 if ( b->dsc_flags & DSC_null )
	   return -2;
	 else
	   return b->dsc_sub_type;
     }
          else return -1;
}


int EXPORT fn_tipocampo ( ARG(PARAMDSC*, b))
    ARGLIST(PARAMDSC * b)
{
   if ( b ) {
      if ( b->dsc_flags & DSC_null )
	return -2;
      else
	return b->dsc_dtype;
   } else return -1;
}

int EXPORT fn_dsc_length ( ARG(PARAMDSC*, b))
      ARGLIST(PARAMDSC * b)
{
   if ( b ) 
     {
	if ( b->dsc_dtype == dtype_varying ) 
	  {
	     signed short *p = b->dsc_address;
	     return *p;
	  }
	return b->dsc_length;
     }
   
   else
     return -1;
}

void somabuffer ( int *r, int *sinal, char *ptr, ISC_USHORT len ) 
{   
   if ( !ptr ) 
     return;
   while ( len-- > 0 ) 
     {
	*r += *ptr++ * *sinal;
	*sinal *= -1;
     }
   return;
}

int EXPORT fn_somablob ( ARG(BLOB, b), ARG(int *, psinal))
  ARGLIST(BLOB b)
  ARGLIST(int *psinal)			 
{ 
   char *buf;
/*   int sinal = 1; */
   
   int r=-1;
   ISC_USHORT length, actual_length;
   int l=0;
   int sinal;
   if ( psinal )
     sinal = *psinal;
   if ( sinal == 0 ) sinal = 1;
   /* Valores nulos */
   if ( !b || !b->blob_handle ) return r;
   
   length=b->blob_max_segment;
   buf=(char*)malloc(1+(int)length);
   if ( !buf ) return 0;
   
   while ((*b->blob_get_segment)(b->blob_handle, buf, length, &actual_length)) {
      /*if ( actual_length == 0 ) break;*/
      somabuffer ( &r, &sinal, buf, actual_length );
   }
   free(buf);
   return r;
}

int EXPORT fn_somadsc ( ARG(PARAMDSC*, b), ARG(int *, psinal ))
  ARGLIST(PARAMDSC * b)
  ARGLIST(int *psinal )			 
{  
   int r = -1;
   int i;
   char *p;
   int sinal; 
   unsigned short *ss;
   short length, actual_length;   
   BLOB v;
   
   if ( !b ) return -1;   

   if ( psinal )
    sinal = *psinal;
   if ( sinal == 0 ) sinal = 1;
   
   if ( ( !b->dsc_address ) ||( b->dsc_flags & DSC_null ) ) return -1;
   
   p = b->dsc_address;   
   
   switch ( b->dsc_dtype ) {
    case dtype_varying: 
      ss = (unsigned short*)p;
      p+=2;
      somabuffer ( &r, &sinal, p, *ss );
      break;
    case dtype_array:         
    case dtype_unknown:      
    case dtype_packed:
    case dtype_byte:
          /* Tipos sem significado no firebird*/
    case dtype_quad: /* Estou em duvidas */
    case dtype_d_float: /* Estou em duvidas */  
    case dtype_cstring:
    case dtype_text:
    case dtype_short:
    case dtype_long:
    case dtype_real:      
    case dtype_double:
    case dtype_int64:
    case dtype_timestamp:
    case dtype_sql_time:  
    case dtype_sql_date:
      somabuffer ( &r, &sinal, p, b->dsc_length );
      break;
    case dtype_blob:
      break;
    default:
      r = -2;
     }
   return r;	
}

