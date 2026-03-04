# product APEX に APK を 2 つ同梱する PoC 手順（`pdt2apk`・非破壊版）

## 目的
- 既存 `com.oem.pdthellod` を変更せず、別 APEX モジュール `com.oem.pdt2apk` で「1つの APEX に APK 2つ同梱」を検証する。
- 判定基準は「ビルド成功」かつ「APEX 内に APK 2つが存在」かつ「端末で2パッケージ認識」。

## 方針（重要）
- 既存: `product/oem/apex/pdthellod` は一切編集しない。
- 新規: `product/oem/apex/pdt2apk` を作成する。
- APEX 名は `com.oem.pdt2apk`（既存と衝突しない名前）を使う。
- PoC では daemon/service は持たず、`apps` 検証のみに絞る。

## 1. 新規 PoC ディレクトリを作成
```bash
mkdir -p product/oem/apex/pdt2apk/{keys,apk/pocapp1,apk/pocapp2}
```

## 2. `apex_manifest.json` を作成
`product/oem/apex/pdt2apk/apex_manifest.json`

```json
{
  "name": "com.oem.pdt2apk",
  "version": 1
}
```

## 3. `file_contexts` を作成
`product/oem/apex/pdt2apk/file_contexts`

```text
(/.*)?                  u:object_r:system_file:s0
```

## 4. APK 用 Manifest を2つ作成
`product/oem/apex/pdt2apk/apk/pocapp1/AndroidManifest.xml`

```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.oem.pdt2apk.app1" />
```

`product/oem/apex/pdt2apk/apk/pocapp2/AndroidManifest.xml`

```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.oem.pdt2apk.app2" />
```

## 5. APEX鍵を用意
PoC 専用鍵を作る（既存鍵流用でもビルドは可能だが、分離のため専用推奨）。

```bash
openssl genrsa -out product/oem/apex/pdt2apk/keys/com.oem.pdt2apk.pem 4096
avbtool extract_public_key \
  --key product/oem/apex/pdt2apk/keys/com.oem.pdt2apk.pem \
  --output product/oem/apex/pdt2apk/keys/com.oem.pdt2apk.avbpubkey
```

## 6. `Android.bp` を作成
`product/oem/apex/pdt2apk/Android.bp`

```bp
apex_key {
    name: "com.oem.pdt2apk.key",
    public_key: "keys/com.oem.pdt2apk.avbpubkey",
    private_key: "keys/com.oem.pdt2apk.pem",
}

android_app {
    name: "Pdt2ApkApp1",
    manifest: "apk/pocapp1/AndroidManifest.xml",
    sdk_version: "current",
    min_sdk_version: "29",
    apex_available: ["com.oem.pdt2apk"],
}

android_app {
    name: "Pdt2ApkApp2",
    manifest: "apk/pocapp2/AndroidManifest.xml",
    sdk_version: "current",
    min_sdk_version: "29",
    apex_available: ["com.oem.pdt2apk"],
}

apex {
    name: "com.oem.pdt2apk",
    manifest: "apex_manifest.json",
    key: "com.oem.pdt2apk.key",
    product_specific: true,
    updatable: true,
    min_sdk_version: "29",
    apps: [
        "Pdt2ApkApp1",
        "Pdt2ApkApp2",
    ],
    file_contexts: "file_contexts",
}
```

## 7. product package に PoC APEX を追加
`product/target/common/product_modules.mk` の `PRODUCT_PACKAGES +=` に追加:

```make
PRODUCT_PACKAGES += \
    com.oem.pdt2apk
```

注意:
- `com.oem.pdthellod` は消さない（共存させる）。
- `pdthellod_product_rc` など既存 daemon 用エントリは PoC 側で不要。

## 8. ビルド
```bash
source build/envsetup.sh
lunch sdk_car_dev-ap1a-eng
m -j12 com.oem.pdt2apk
```

`allowed_deps` エラーが出た場合:

```bash
source build/envsetup.sh
lunch sdk_car_dev-ap1a-eng
packages/modules/common/build/update-apex-allowed-deps.sh
m -j12 com.oem.pdt2apk
```

## 9. APEX 内の APK 2つを確認
```bash
out/host/linux-x86/bin/deapexer \
  --debugfs_path out/host/linux-x86/bin/debugfs_static \
  --fsckerofs_path out/host/linux-x86/bin/fsck.erofs \
  list out/target/product/<target>/product/apex/com.oem.pdt2apk.apex
```

期待:
- `./app/.../Pdt2ApkApp1.apk`
- `./app/.../Pdt2ApkApp2.apk`

## 10. 端末確認（任意）
```bash
adb shell ls -R /apex/com.oem.pdt2apk
adb shell pm list packages | grep com.oem.pdt2apk
adb shell pm path com.oem.pdt2apk.app1
adb shell pm path com.oem.pdt2apk.app2
```

## PoC 成功条件
- `m com.oem.pdt2apk` が成功。
- `deapexer list` で APK 2つ確認。
- 端末 `pm` で 2 package 確認。

## ロールバック（非破壊）
- `product/target/common/product_modules.mk` から `com.oem.pdt2apk` を削除。
- `product/oem/apex/pdt2apk` ディレクトリを削除。
- 既存 `product/oem/apex/pdthellod` は未変更のため影響なし。
