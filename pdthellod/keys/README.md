# keys directory

このディレクトリには秘密鍵をコミットしません。

## 鍵の作成手順

```bash
cd product/oem/apex/pdthellod/keys
openssl genrsa -out com.oem.pdthellod.pem 4096
avbtool extract_public_key --key com.oem.pdthellod.pem --output com.oem.pdthellod.avbpubkey
```

## この2つでできること

1. `com.oem.pdthellod.pem`: APEX 署名に使用できます。
2. `com.oem.pdthellod.avbpubkey`: 検証用公開鍵として組み込みできます。

