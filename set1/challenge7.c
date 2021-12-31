#include <stdio.h>
#include <assert.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../bbuf.h"
#include "../bytes.h"
#include "../base64.h"

#define DATA_FILENAME "challenge7.txt"
#define KEY "YELLOW SUBMARINE"

// things needed
// - brew install openssl and manual makefile link (avoid symlink over libressl)
// - this page https://www.openssl.org/docs/manmaster/man3/EVP_CIPHER_CTX_new.html

int main()
{
    bbuf encoded_cyphertext_b = bbuf_new(), cyphertext_b = bbuf_new(), plaintext_buf = bbuf_new();
    char *plaintext;
    int written, total_written;
    EVP_CIPHER_CTX *ctx;

    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    encoded_cyphertext_b = from_file(DATA_FILENAME);
    cyphertext_b = decode_base64(&encoded_cyphertext_b);
    bbuf_destroy(&encoded_cyphertext_b);

    bbuf_init_to(&plaintext_buf, cyphertext_b.len);

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (const unsigned char *)KEY, NULL);

    total_written = written = 0;

    if (!EVP_CipherUpdate(ctx, plaintext_buf.buf, &written, cyphertext_b.buf, cyphertext_b.len))
    {
        printf("OpenSSL Cipher update error\n");
        exit(1);
    }
    total_written += written;
    if (!EVP_CipherFinal(ctx, plaintext_buf.buf + total_written, &written))
    {
        printf("OpenSSL Cipher finalize error\n");
        exit(1);
    }
    total_written += written;

    assert(((int)plaintext_buf.len - 16) < total_written && total_written <= (int)plaintext_buf.len);

    plaintext_buf.len = cyphertext_b.len;

    plaintext = to_string(&plaintext_buf);
    printf("decoded to\n\"%s\"\n", plaintext);

    EVP_CIPHER_CTX_free(ctx);
    free(plaintext);
    bbuf_destroy(&plaintext_buf);
    bbuf_destroy(&cyphertext_b);
}
