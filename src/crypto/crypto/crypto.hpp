#pragma once

#include <openssl/aes.h>
#include <openssl/x509.h>
#include <string>

namespace crypto {

/**
 * @brief Generates length random bytes using a cryptographically secure pseudo random generator (CSPRNG)
 */
std::string random(int length);

/**
 * Encrypt the given msg using AES ecb at 128 bit
 *
 * @param msg: the message to be encrypted
 * @param enc_key: the key used for encryption
 * @param iv: optional, if not provided a random one will be generated
 * @param padding: optional, enables or disables padding
 * @return: the encrypted string
 */
std::string aes_encrypt_ecb(const std::string &msg,
                            const std::string &enc_key,
                            const std::string &iv = random(AES_BLOCK_SIZE),
                            bool padding = false);

/**
 * Decrypt the given msg using AES ecb at 128 bit
 *
 * @param msg: the message to be encrypted
 * @param enc_key: the key used for encryption
 * @param iv: optional, if not provided a random one will be generated
 * @param padding: optional, enables or disables padding
 * @return: the decrypted string
 */
std::string aes_decrypt_ecb(const std::string &msg,
                            const std::string &enc_key,
                            const std::string &iv = random(AES_BLOCK_SIZE),
                            bool padding = false);

/**
 * Will sign the given message using the private key
 * @param msg: the message to be signed
 * @param private_key: the key used for signing
 * @return: the signature binary data
 */
std::string sign(const std::string &msg, const std::string &private_key);

/**
 * Will verify that the signature for the given message has been generated by the public_key
 * @param msg: the message that was originally signed
 * @param signature: the signature data
 * @param public_key: the public key, used to verify the signature
 * @return: true if the signature is correct, false otherwise.
 */
bool verify(const std::string &msg, const std::string &signature, const std::string &public_key);

/**
 * @brief returns the SHA256 hash of the given str
 */
std::string sha256(const std::string &str);

/**
 * @brief converts the given input into a HEX string
 */
std::string str_to_hex(const std::string &input);

/**
 * @brief takes an HEX vector and returns a string representation of it.
 */
std::string hex_to_str(const std::string &hex, bool reverse);

} // namespace crypto

/**
 * @brief Wrappers on top of OpenSSL methods in order to deal with x509 certificates
 *
 * Adapted from: https://gist.github.com/nathan-osman/5041136
 */
namespace x509 {
/**
 * @brief Generates a 2048-bit RSA key.
 *
 * @return EVP_PKEY*: a ptr to a private key
 */
EVP_PKEY *generate_key();

/**
 * @brief Generates a self-signed x509 certificate.
 *
 * @param pkey: a private key generated with generate_key()
 * @return X509*: a pointer to a x509 certificate
 */
X509 *generate_x509(EVP_PKEY *pkey);

/**
 * @brief Reads a X509 certificate string
 */
X509 *cert_from_string(const std::string &cert);

/**
 * @brief Reads a X509 certificate from file
 */
X509 *cert_from_file(const std::string &cert_path);

/**
 * @brief Reads a private key from file
 */
EVP_PKEY *pkey_from_file(const std::string &pkey_path);

/**
 * @brief Write cert and key to disk
 *
 * @param pkey: a private key generated with generate_key()
 * @param pkey_filename: the name of the key file to be saved
 * @param x509: a certificate generated with generate_x509()
 * @param cert_filename: the name of the cert file to be saved
 * @return true when both pkey and x509 are stored on disk
 * @return false when one or both failed
 */
bool write_to_disk(EVP_PKEY *pkey, const std::string &pkey_filename, X509 *x509, const std::string &cert_filename);

/**
 * @param pkey_filename: the name of the key file to be saved
 * @param cert_filename: the name of the cert file to be saved
 * @return true when both files are present
 */
bool cert_exists(const std::string &pkey_filename, const std::string &cert_filename);

/**
 * @return the certificate signature
 */
std::string get_cert_signature(const X509 *cert);

/**
 * @param private_key: set to true if EVP_PKEY is a private key, set to false for public keys
 * @return the key content in plaintext
 */
std::string get_key_content(EVP_PKEY *pkey, bool private_key);

/**
 * @return the private key content
 */
std::string get_pkey_content(EVP_PKEY *pkey);

/**
 * @return the certificate in pem format
 */
std::string get_cert_pem(const X509 &x509);

/**
 *
 * @return the certificate public key content
 */
std::string get_cert_public_key(X509 *cert);

/**
 * @brief: cleanup pointers after use
 *
 * TODO: replace with smart pointers?
 *
 * @param pkey: a private key generated with generate_key()
 * @param x509: a certificate generated with generate_x509()
 */
void cleanup(EVP_PKEY *pkey, X509 *cert);
} // namespace x509