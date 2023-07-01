#ifndef __PS_UTILSH__
#define __PS_UTILSH__

#if defined __BORLANDC__ && defined __WIN32__
#ifndef EXPORT
    #define EXPORT _export
#endif
#else
#define EXPORT
#endif

#ifdef  __cplusplus
extern "C" {
#endif


extern char *Gerar_DigitoEAN ( char *Numero );
extern char *Extrair_Numeros(char *numero);
extern char *PSFormatar_CNPJ(char *Numero, char *destino, int buflen);
extern char *PSFormatar_CPF(char* Numero, char *destino, int buflen);
extern char *PSFormatar_CNPJCPF(char* Numero, char* buffer, int buflen);
extern char *PSUpperCase(int NumeroTabela,unsigned char* Entrada, unsigned char* Saida);
extern char *PSUpperCase2(int NumeroTabela,unsigned char* Entrada, int BytesIn, unsigned char* Saida);
extern unsigned char PSUpperCaseChar(int NumeroTabela,unsigned char Entrada);
extern int PSGerarByteUpperCase(int NumeroTabela,unsigned char Entrada, unsigned char Saida);
extern int PSCNPJ_Valido(char *Numero);
extern int PSCPF_Valido(char *Numero);
extern char* Gerar_Digitos_2(char *numero, int digitos);
extern char* Gerar_Digitos_1(char *numero, int digitos);
   
extern char *PSCrypto(char *dados, int dbytes, unsigned char *buffer, int bbytes);
extern int PSCompareCrypto(char *dados, int dbytes, unsigned char *crypto, int cbytes);

#ifdef  __cplusplus
    }
#endif


#define PSUSARSHA 1
#if PSUSARSHA
    #include <openssl/sha.h>
    #define DIGESTLEN   SHA_DIGEST_LENGTH
    #define METODOCRIPTO    "SHA1"
    #define CRIPTCTX    SHA_CTX
    #define CRIPTINIT   SHA1_Init
    #define CRIPTUPDATE SHA1_Update
    #define CRIPTFINAL  SHA1_Final
#else
    #define DIGESTLEN   MD5_DIGEST_LENGTH
    #define METODOCRIPTO    "MD5"
    #include <openssl/md5.h>
    #define CRIPTCTX    MD5_CTX
    #define CRIPTINIT   MD5_Init
    #define CRIPTUPDATE MD5_Update
    #define CRIPTFINAL  MD5_Final
#endif

#define DIGESTLENDBL    (DIGESTLEN*2)



#endif /*__PS_UTILSH__*/
