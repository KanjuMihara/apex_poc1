# keys directory

このディレクトリには秘密鍵をコミットしません。

## 鍵の作成手順

```bash
cd product/oem/apex/pdt2apk/keys
openssl genrsa -out com.oem.pdt2apk.pem 4096
avbtool extract_public_key --key com.oem.pdt2apk.pem --output com.oem.pdt2apk.avbpubkey
```

## この2つでできること

1. `com.oem.pdt2apk.pem`: APEX 署名に使用できます。
2. `com.oem.pdt2apk.avbpubkey`: 検証用公開鍵として組み込みできます。

