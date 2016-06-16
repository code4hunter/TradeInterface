#ifndef ENCODEDLLWRAPPERH
#define ENCODEDLLWRAPPERH

#include <memory>
#include <mutex>
#include <windows.h>
#include "KDEncodeCli.h"

class encode_dll_wrapper {
    typedef char *(*DOGSKIN_ENCODE)(const char *);

    typedef int (*KDENCODE)(int nEncode_Level,
                            unsigned char *pSrcData, int nSrcDataLen,
                            unsigned char *pDestData, int nDestDataBufLen,
                            void *pKey, int nKeyLen);

    typedef std::unique_ptr<encode_dll_wrapper> ENCODEDLLPTR;

private:
    HMODULE hDll1, hDll2;
    DOGSKIN_ENCODE DEncodeFun;
    KDENCODE KEncodeFun;
    static ENCODEDLLPTR _instance;
    std::mutex _mutex;
protected:

public:
    static encode_dll_wrapper *instance(void) {
        return _instance.get();
    }

    encode_dll_wrapper() {
        hDll1 = NULL;
        hDll2 = NULL;
        DEncodeFun = NULL;
        KEncodeFun = NULL;

        if (hDll1 == NULL)
            hDll1 = LoadLibrary("Dogskin.dll");
        if (!hDll1) {
            throw std::runtime_error("Could not LoadLibrary Dogskin.dll!!");
        }

        DEncodeFun = (DOGSKIN_ENCODE) ::GetProcAddress(hDll1, "Encode");
        if (!DEncodeFun) {
            throw std::runtime_error("Could not get the address of the Encode function!!!");
        }

        if (hDll2 == NULL)
            hDll2 = LoadLibraryA("KDEncodeCli.dll");
        if (!hDll2) {
            throw std::runtime_error("Could not Load Library KDEncodeCli.dll!!");
        }

        KEncodeFun = (KDENCODE) GetProcAddress(hDll2, "KDEncode");
        if (!KEncodeFun) {
            throw std::runtime_error("Could not get the address of the KDEncode function!!!");
        }
    }

    ~encode_dll_wrapper() {
        if (hDll1) {
            FreeLibrary(hDll1);
            hDll1 = NULL;
        }

        if (hDll2) {
            FreeLibrary(hDll2);
            hDll2 = NULL;
        }
    }

    int Encrypt(const char *pSrc, char *pDst, const char *key, int encryptType) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (encryptType == 1) {
            if (DEncodeFun != NULL) {
                strcpy(pDst, DEncodeFun((const char *) pSrc));
            }
            else return -1;
        }
        else {
            //key的内容
            //第一次登录是登录请求号
            //以后的登录就是客户号
            if (KEncodeFun != NULL) {
                KEncodeFun(KDCOMPLEX_ENCODE,(unsigned char*)pSrc,strlen(pSrc),(unsigned char*)pDst,254+1,
                           (unsigned char*)key,strlen(key));
            }
            else return -1;
        }

        return 0;
    }
};

//---------------------------------------------------------------------------
#endif
