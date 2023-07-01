#include "ps_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PSUSARSHA 1

#ifdef __LINUX__
    #define ARQUIVOTABELA "/etc/psupper.tbl"
#else
    #define ARQUIVOTABELA "c:\\psupper.tbl"
#endif
int Verificar_Digitos(char *Numero, int digitos, int verificandocpf);
unsigned char Tabela[2][256];
int TabelaIniciada=0;

void IniciarTabela(void){
    if(!TabelaIniciada){
        FILE*f=fopen(ARQUIVOTABELA,"r+b");
        if(f){
            fread(Tabela,1,sizeof(Tabela),f);
            fclose(f);
        } else {
           int i;
           for(i=0;i<=255;i++)
              Tabela[0][i]=Tabela[1][i]=i;
        }
        TabelaIniciada=1;
    }
}

char * EXPORT PSUpperCase(int NumeroTabela,unsigned char* Entrada, unsigned char* Saida){
    if(NumeroTabela<0||NumeroTabela>1)return NULL;
    if(!Entrada||!Saida)return Saida;
    IniciarTabela();
    while(*Entrada){
        *Saida=Tabela[NumeroTabela][*Entrada];
        Saida++;
        Entrada++;
    }
    *Saida=0;
    return Saida;
}

char * EXPORT PSUpperCase2 ( int NumeroTabela, unsigned char* Entrada, int BytesIn, unsigned char* Saida ){
    if(NumeroTabela<0||NumeroTabela>1)return NULL;
    if(!Entrada||!Saida)return Saida;
    IniciarTabela();    
    while ( BytesIn-- /* *Entrada*/ ){
        *Saida=Tabela[NumeroTabela][*Entrada];
        Saida++;
        Entrada++;
    }
    *Saida=0;
    return Saida;
}

unsigned char EXPORT PSUpperCaseChar(int NumeroTabela,unsigned char Entrada){
    if(NumeroTabela<0||NumeroTabela>1)return 0;
    IniciarTabela();
    return Entrada?Tabela[NumeroTabela][Entrada]:0;
}

int EXPORT PSGerarByteUpperCase(int NumeroTabela,unsigned char Entrada, unsigned char Saida){
    FILE*f;
    if(NumeroTabela<0||NumeroTabela>1)return 1;
    f=fopen(ARQUIVOTABELA,"r+b");
    if(f){
        fread(Tabela,1,sizeof(Tabela),f);
        fclose(f);
    } else {
        int i;
        for(i=0;i<=255;i++)
           Tabela[0][i]=Tabela[1][i]=(unsigned char)i;
    }
    Tabela[NumeroTabela][Entrada]=Saida;
    f=fopen(ARQUIVOTABELA,"w+b");
    fwrite(Tabela,1,sizeof(Tabela),f);
    fclose(f);
    TabelaIniciada=0;
    return 0;
}

char* EXPORT Gerar_Digitos_2(char *numero, int digitos) {
    int indice,tamanho,soma=0, coef, resultado;
    digitos--;
    if (digitos >= 0) {
        tamanho=strlen(numero);
        for (indice=0;indice<tamanho;indice++) {
		    coef=((tamanho-(indice+1)+1) % (tamanho+1))+1;
			soma += (numero[indice]-48) * coef;
        }
        resultado=11-(soma%11);
        if (resultado>9)
            resultado=0;
        numero[tamanho++]=(char)(resultado+48);
        numero[tamanho]=0;
        Gerar_Digitos_2(numero, digitos);
    }
    return numero;
}

char* EXPORT Gerar_Digitos_1(char *numero, int digitos) {   
    int resultado,indice,tamanho,soma=0, coef;

    digitos--;
    if (digitos >= 0) {
        tamanho=strlen(numero);
        for (indice=0;indice<tamanho;indice++) {
            coef=(tamanho-(indice+1)+1) % 9;
            if( (tamanho-(indice+1)) > 7)
                coef+=2;
            else
			    coef++;
            soma += (numero[indice]-48) * coef;
        }
        resultado=11-(soma%11);
        if (resultado>9)
            resultado=0;
        numero[tamanho++]=(char)(resultado+48);
        numero[tamanho]=0;
        Gerar_Digitos_1(numero, digitos);
    }
    return numero;
}


char *EXPORT Gerar_DigitoEAN ( char *Numero ) { 
   int factor = 3;
   int sum = 0;
   int index;
   char cs;

   int i = strlen ( Numero );

   if ( !Numero ) return NULL;
   for ( index = i; index > 0; ) {
      sum += ( ( Numero [ --index ] - '0' ) * factor );
      factor = 4 - factor;
   }
   cs = ( ( 1000 - sum ) % 10 );
   
   Numero [ i++ ] = cs + '0';
   Numero [ i ] = 0;
   
   return Numero;
      
}


char* EXPORT Extrair_Numeros(char *numero) {
    int j=strlen(numero),i, s=0;
    for (i=0;i<j;i++) {
    if ( (numero[i]>=48)&&(numero[i]<=57))
        numero[s++] = numero[i];
    }
    numero[s]=0;
    return numero;
}

void EXPORT PSCopiaStr(char *Destino, char*Origem, int Count){
    if(!Origem||!Destino)return;
    if(Count>0){
        strncpy(Destino,Origem,Count);
        Destino[Count]=0;
    }else
        Destino[0]=0;
}

char* EXPORT Truncar_Esquerda(char *valor, char elemento, int tamanho) {
    char *tmp, *tmp2;
    int l=tamanho-strlen(valor);
    if (l>0) {
        tmp2=(char *)valor+tamanho-l;
        for(tmp=valor+tamanho;tmp2>=valor;tmp--)
            *tmp=*tmp2--;
        while(l--)
        *(char*)(tmp-l)=elemento;
    }
    return valor;
}

char * EXPORT strreplaceall(char *buffer, char o, char d){
	char *p=buffer;
	if(*p&&o)
	    while(*p)
		    if(*p++==o)
			    *(p-1)=d;
    return buffer;
}

int EXPORT PSCNPJ_Valido(char *Numero) {
  if (!Numero)return 0;
  return Verificar_Digitos(Numero, 2, 0);
}

int EXPORT PSCNPJ_Valido_cpf(char *Numero) {
  if (!Numero)return 0;
  return Verificar_Digitos(Numero, 2, 1);
}

int EXPORT PSCPF_Valido(char *Numero) {
  int l;
  char cpf[50], cpftmp[50];
  /*memset(cpf,0,50);*/
  if(!Numero)return 0;
  l=strlen(Numero);
  strncpy(cpf,Numero,l);
  cpf[l]=0;
  Extrair_Numeros(cpf);
  l=strlen(cpf);
  if (l<3)
    return 0;
  if(l>11&&PSCNPJ_Valido_cpf(Numero))
  	return 0;
  strcpy(cpftmp,cpf);
  cpftmp[l-2]=0;
  Gerar_Digitos_2(cpftmp,2);
  return !(strcmp(cpf,cpftmp));
}

int EXPORT Verificar_Digitos(char *Numero, int digitos, int verificandocpf) {
  char tmp1[50], tmp2[50];
  int l,i=strlen(Numero);
  strncpy(tmp1,Numero,i);
  tmp1[i]=0;
  Extrair_Numeros(tmp1);
  strcpy(tmp2, tmp1);
  i=strlen(tmp1);
  if ((i-digitos)<1)
    return 0;
  tmp2[i-digitos]=0;
  Gerar_Digitos_1(tmp2,digitos);
  l = !strcmp(tmp1,tmp2);
  if (l&&!verificandocpf&&PSCPF_Valido(Numero)) {
    l=strlen(tmp1)>11;
  }
  return l;
}

char * EXPORT PSFormatar_CNPJ(char *Numero, char *destino, int buflen) {
  char vlr[50], result[50];
  int i;
  char *s;
  if (!Numero)return Numero;
  s=result;
  result[0]=0;
  i=strlen(Numero);
  strncpy(vlr,Numero,i);
  vlr[i]=0;
  Truncar_Esquerda(Extrair_Numeros(vlr),'0',14);
  for (i=0;i<14;i++) {
    s[i]=vlr[i];
    switch (i) {
      case 1:
      case 4:
        (++s)[i]='.';
        break;
      case 7:
        (++s)[i]='/';
        break;
      case 11:
        (++s)[i]='-';
    }
  }
  s[i]=0;
  if(buflen<0)buflen=0;
  if(destino&&buflen){
      strncpy(destino,result,buflen-1);
      destino[buflen-1]=0;
  }

  return destino;
}

char * EXPORT PSFormatar_CPF(char* Numero, char *destino, int buflen) {
  char vlr[50], result[50];
  int i;
  char *s;
  if(!Numero)return Numero;
  s=result;
  result[0]=0;
  i=strlen(Numero);
  strncpy(vlr,Numero,i);
  vlr[i]=0;
  Truncar_Esquerda(Extrair_Numeros(vlr),'0',11);
  for (i=0;i<11;i++) {
    s[i]=vlr[i];
    switch (i) {
      case 2:
      case 5:
        (++s)[i]='.';
        break;
      case 8:
        (++s)[i]='-';
    }
  }
  s[i]=0;
  if(buflen<0)buflen=0;
  if(destino&&buflen){
      strncpy(destino,result,buflen-1);
      destino[buflen-1]=0;
  }
  return destino;
}

char * EXPORT PSFormatar_CNPJCPF(char* Numero, char* buffer, int buflen) {
    if (!Numero)return Numero;
    if (PSCNPJ_Valido(Numero))
        return PSFormatar_CNPJ(Numero,buffer,buflen);
    else
        if (PSCPF_Valido(Numero))
            return PSFormatar_CPF(Numero,buffer,buflen);
        else {
            char tmp[50];
            int i=strlen(Numero);
            strncpy(tmp,Numero,i);
            tmp[i]=0;
            Extrair_Numeros(tmp);
            if(buflen<0)
                buflen=0;
            if(buffer&&buflen){
                strncpy(buffer,tmp,buflen-1);
                buffer[buflen-1]=0;
            }
            return buffer;
        }
}

int EXPORT PSCompareCrypto(char *dados, int dbytes, unsigned char *crypto, int cbytes){
    char lbuf[DIGESTLENDBL+1];
    if(!dados||!crypto||dbytes<0||cbytes<DIGESTLENDBL)return 1;
    PSCrypto(dados,dbytes,lbuf,sizeof(lbuf));
    return memcmp(lbuf,crypto,DIGESTLENDBL)!=0;
}

char * EXPORT PSCrypto(char *dados, int dbytes, unsigned char *buffer, int bbytes){
    CRIPTCTX c;
    char *p;
    int i;
    char lbuf[DIGESTLEN+1];
    if(!dados||dbytes<1||!buffer||bbytes<DIGESTLENDBL+1)return buffer;
    CRIPTINIT(&c);
    CRIPTUPDATE(&c,dados,dbytes);
    memset(lbuf,0,sizeof(lbuf));
    CRIPTFINAL(lbuf,&c);

    lbuf[DIGESTLEN]=0;
    buffer[0]=0;
    p=buffer;
    for(i=0;i<DIGESTLEN;i++,p+=2)
        sprintf(p,"%2.2X",(unsigned char)lbuf[i]);
    buffer[DIGESTLENDBL]=0;
    return buffer;
}

#ifdef PSDEBUG
int main(int argc, char* argv[]){
    char buffer[DIGESTLENDBL+1];
    int i;
    if(argc<2){
        printf("Uso: %s valor\n\tOnde valor é o numero a ser formatado.\n",argv[0]);
        return 1;
    }
    for(i=1;i<argc;i++){
        printf("Formatando.......: %s\n",PSFormatar_CNPJCPF(argv[i],buffer,sizeof(buffer)));
        printf("Original.........: %s\n",argv[i]);
        printf("CPF..............: %d\n",PSCPF_Valido(buffer));
        printf("CPF..............: %d\n",PSCPF_Valido(argv[i]));
        printf("Original.........: %s\n",argv[i]);
        printf("CNPJ.............: %d\n",PSCNPJ_Valido(buffer));
        printf("CNPJ.............: %d\n",PSCNPJ_Valido(argv[i]));
        printf("Original.........: %s\n",argv[i]);

        printf("Método Cripto ...: %s\n",METODOCRIPTO);
        printf("DIGEST_LENGTH....: %d\n",DIGESTLEN);
        printf("Crypto...........: %s\n",PSCrypto(argv[i],strlen(argv[i]),buffer,sizeof(buffer)));
#define VALOR   "1234"
        printf("Comparar.........: %d\n",PSCompareCrypto(VALOR,sizeof(VALOR)-1, buffer, sizeof(buffer)));
        printf("sizeof(VALOR)....: %d\n",sizeof(VALOR));

        printf("\n--------------------------------\n");
    }
    return 0;
}
#endif

