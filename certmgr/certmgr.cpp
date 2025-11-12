// certmgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "certmgr.h"

struct destruct
{
	std::function <void ()> endtask = nullptr;
	destruct (std::function <void ()> pfunc): endtask (pfunc) {}
	~destruct () { if (endtask) endtask (); }
};
BOOL LoadCertFromCertFile (LPCWSTR lpCertFile)
{
	constexpr LPCWSTR storeNameROOT = L"Root";
	constexpr LPCWSTR storeNamePublisher = L"TrustedPublisher";  // 添加 TrustedPublisher
	HCERTSTORE hCertStore = CertOpenStore (
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG,
		storeNameROOT
	);
	destruct relcst ([&] () {
		if (hCertStore) CertCloseStore (hCertStore, 0);
		hCertStore = nullptr;
	});
	if (!hCertStore) return false;
	HCERTSTORE hFileCertStore = nullptr;
	destruct relfcs ([&] () {
		if (hFileCertStore) CertCloseStore (hFileCertStore, 0);
		hFileCertStore = nullptr;
	});
	PCCERT_CONTEXT pCertContext = nullptr;
	destruct relcc ([&] () {
		if (pCertContext) CertFreeCertificateContext (pCertContext);
		pCertContext = nullptr;
	});
	// 使用 CryptQueryObject 自动检测证书格式
	if (!CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,  // 证书文件类型
		lpCertFile,
		CERT_QUERY_CONTENT_FLAG_CERT |       // X.509 证书
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED,// PKCS7 格式
		CERT_QUERY_FORMAT_FLAG_ALL,          // 允许所有格式
		0,
		NULL, NULL, NULL,
		&hFileCertStore,  // 输出证书存储区
		NULL,             // 不处理 CRL 或 PKCS7 签名者信息
		NULL
	)) return false;
	// 获取证书上下文
	pCertContext = CertEnumCertificatesInStore (hFileCertStore, NULL);
	if (!pCertContext) return false;
	// 将证书添加到 ROOT 存储
	if (!CertAddCertificateContextToStore (hCertStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) return false;
	// 导入到 TrustedPublisher 存储
	HCERTSTORE hPublisherStore = CertOpenStore (CERT_STORE_PROV_SYSTEM, 0, NULL, CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG, storeNamePublisher);
	destruct relps ([&] () {
		if (hPublisherStore) CertCloseStore (hPublisherStore, 0);
		hPublisherStore = nullptr;
	});
	if (!hPublisherStore) return false;
	if (!CertAddCertificateContextToStore (hPublisherStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) return false;
	return true;
}
BOOL LoadCertFromSignedFile (LPCWSTR lpSignedFile)
{
	constexpr LPCWSTR storeNameROOT = L"Root";
	constexpr LPCWSTR storeNamePublisher = L"TrustedPublisher";  // 添加 TrustedPublisher
	HCERTSTORE hStore = nullptr;
	destruct relstore ([&] () {
		if (hStore) CertCloseStore (hStore, 0);
		hStore = nullptr;
	});
	PCCERT_CONTEXT pCertContext = nullptr;
	destruct relcc ([&] () {
		if (pCertContext) CertFreeCertificateContext (pCertContext);
		pCertContext = nullptr;
	});
	// 打开已签名的文件并获取证书存储区
	if (!CryptQueryObject (
		CERT_QUERY_OBJECT_FILE, lpSignedFile,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY, 0,
		NULL, NULL, NULL, &hStore, NULL, NULL
	)) return false;
	pCertContext = CertEnumCertificatesInStore (hStore, NULL);
	if (!pCertContext) return false;
	// 打开目标存储区
	HCERTSTORE hTargetStore = CertOpenStore (
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG,
		storeNameROOT
	);
	destruct relts ([&] () {
		if (hTargetStore) CertCloseStore (hTargetStore, 0);
		hTargetStore = nullptr;
	});
	if (!hTargetStore) return false;
	// 导入证书到 ROOT 存储区
	if (!CertAddCertificateContextToStore (hTargetStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) return false;
	// 导入到 TrustedPublisher 存储
	HCERTSTORE hPublisherStore = CertOpenStore (
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG,
		storeNamePublisher
	);
	destruct relps ([&] () {
		if (hPublisherStore) CertCloseStore (hPublisherStore, 0);
		hPublisherStore = nullptr;
	});
	if (!hPublisherStore) return false;
	if (!CertAddCertificateContextToStore (hPublisherStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) return false;
	return true;
}