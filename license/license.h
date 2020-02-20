#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#if defined WIN32
#if defined(license_EXPORTS)
#define LICENSE __declspec(dllexport) 
#elif defined(license_IMPORTS)
#define LICENSE __declspec(dllimport) 
#else
#define LICENSE
#endif
#else
#define LICENSE
#endif

/// sizes in bytes
#define UID_LEN 44
#define BIGN128_PRI_SIZE 32
#define BIGN128_PUB_SIZE 64

#define LICENSE_VERSION 2

enum license_err_t
{
  license_success = 0,

  license_internal_error,
  license_wrong_uid,
  license_wrong_version,
  license_wrong_product,
  license_wrong_uid_len,
  license_wrong_pri_len,
  license_wrong_pub_len,
  license_wrong_lic_len,
  license_wrong_unp_len,
  license_wrong_org_len,

  license_device_error,
  license_device_not_found,
  license_device_not_authenticated,
  license_device_obj_not_found,

  license_wrong_sign,
};

/**
 * Get version from UID file
 * @param[in] uid - buffer with data to get version.
 * @param[in] uid_len - length of file.
 * @param[out] version - version of license.
 * @return err_t - error code result.
 */
int LICENSE lic_get_uid_version(const unsigned char *uid, unsigned int uid_len, unsigned short *version);

/**
 * Get version from license file
 * @param[in] lic - buffer with data to get version.
 * @param[in] lic_len - length of file.
 * @param[out] version - version of license.
 * @return err_t - error code result.
 */
int LICENSE lic_get_version(const unsigned char *lic, unsigned int lic_len, unsigned short *version);

/**
 * UID-data generation.
 * @param[in] prod - product code for which license will be emited.
 * @param[out] uid - uid-data of license.
 * @param[in, out] uid_len - buffer size for uid-data.
 * @return err_t - error code result.
 */
int LICENSE lic_get_uid( const unsigned int prod, unsigned char *uid, unsigned int *uid_len );

/**
 * License emit.
 * @param[in] validity_seconds - validity time in seconds.
 * @param[in] uid - uid-data of license.
 * @param[in] uid_len - buffer size for uid-data (UID_LEN).
 * @param[in] pri - private key data for license emit.
 * @param[in] pri_len - buffer size for private key data (BIGN128_PRI_SIZE).
 * @param[in] prod - product code for which license will be emited.
 * @param[out] lic - data of license.
 * @param[in, out] lic_len - buffer size for data of license.
 * @return err_t - error code result.
 */
int LICENSE lic_emit( unsigned int validity_seconds, 
             const unsigned char *uid, unsigned int uid_len, 
             const unsigned char *pri, unsigned int pri_len, 
             const unsigned int prod, 
             unsigned char *lic, unsigned int *lic_len );



/**
 * License emit with private key on device.
 * @param[in] validity_seconds - validity time in seconds.
 * @param[in] uid - uid-data of license.
 * @param[in] uid_len - buffer size for uid-data.
 * @param[in] prod - product code for which license will be emited.
 * @param[in] device - authed context of device (with dkey_cimpl).
 * @param[in] pri_name - name of private key on device.
 * @param[in] pri_name_len - size of pri_name.
 * @param[out] lic - data of license.
 * @param[in, out] lic_len - buffer size for data of license.
 * @return err_t - error code result.
 */
int LICENSE lic_emit_dkey( unsigned int validity_seconds, 
             const unsigned char *uid, unsigned int uid_len,
             const unsigned int prod,
             void *device,
             const char *pri_name, unsigned int pri_name_len,
             unsigned char *lic, unsigned int *lic_len );

/**
 * License verification.
 * @param[in] uid - uid-data of license.
 * @param[in] uid_len - buffer size for uid-data.
 * @param[in] pub - public key data for license verification.
 * @param[in] pub_len - buffer size for public key data (BIGN128_PUB_SIZE).
 * @param[in] prod - product code for which license will be emited.
 * @param[in] lic - data of license.
 * @param[in] lic_len - buffer size for data of license.
 * @return err_t - error code result.
 */
int LICENSE lic_verify( const unsigned char *uid, unsigned int uid_len, 
               const unsigned char *pub, unsigned int pub_len, 
               const unsigned int prod, 
               unsigned char *lic, unsigned int lic_len );

/**
 * UID-data generation and license verification.
 * @param[in] pub - public key data for license verification.
 * @param[in] pub_len - buffer size for public key data (BIGN128_PUB_SIZE).
 * @param[in] lic - data of license.
 * @param[in] lic_len - buffer size for data of license.
 * @param[in] prod - product code for which license will be emited.
 * @return err_t - error code result.
 */
int LICENSE lic_calc_verify( const unsigned char *pub, unsigned int pub_len, 
                    unsigned char *lic, unsigned int lic_len, 
                    const unsigned int prod );
#ifdef __cplusplus
}
#endif
