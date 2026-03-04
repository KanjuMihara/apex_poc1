# keys directory

このディレクトリには秘密鍵をコミットしません。

## 鍵の作成手順

```bash
cd product/oem/apex/pdthello/keys
openssl genrsa -out com.oem.pdthello.pem 4096
avbtool extract_public_key --key com.oem.pdthello.pem --output com.oem.pdthello.avbpubkey
```

## この2つでできること

1. `com.oem.pdthello.pem`: APEX 署名に使用できます。
2. `com.oem.pdthello.avbpubkey`: 検証用公開鍵として組み込みできます。

