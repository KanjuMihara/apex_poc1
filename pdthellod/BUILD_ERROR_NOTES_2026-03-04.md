# AOSP build error notes (2026-03-04)

## Context
- Build target: `sdk_car_dev-ap1a-eng`
- Command: `source build/envsetup.sh && lunch sdk_car_dev-ap1a-eng && m -j12`

## Failures and fixes

### 1) SELinux `neverallow` failures (`product_sepolicy.cil`)
Observed errors:
- `neverallow check failed ... domain.te:900/919/966`
- Offending allow rules were from `pdthellod` and `pdthellod_exec`/`pdthellod_file_a_v2`.

Fix:
- Updated domain type to coredomain:
  - `product/oem/apex/pdthellod/sepolicy/product/pdthellod.te`
  - from: `type pdthellod, domain;`
  - to:   `type pdthellod, domain, coredomain;`

Verification:
- `m -j12 product_sepolicy.cil` succeeded.

---

### 2) APEX allowed deps failure
Observed error:
- `Diff packages/modules/common/build/allowed_deps.txt ...`
- Missing entry: `pdthellod(minSdkVersion:29)`

Fix:
- Ran:
  - `source build/envsetup.sh && lunch sdk_car_dev-ap1a-eng`
  - `packages/modules/common/build/update-apex-allowed-deps.sh`
- This regenerated `packages/modules/common/build/allowed_deps.txt` and added `pdthellod(minSdkVersion:29)`.

---

### 3) `version_policy` failure (`typetransition unsupported statement in attributee policy`)
Observed errors:
- While building `plat_pub_versioned.cil` / `product_mapping_file`:
- `typetransition unsupported statement in attributee policy`

Root cause:
- Product policy containing `typetransition` (`init_daemon_domain(...)`) was incorrectly placed in `PRODUCT_PUBLIC_SEPOLICY_DIRS`.

Fix:
- Moved policy directory from public to private:
  - `product/target/common/product_modules.mk`
  - from: `PRODUCT_PUBLIC_SEPOLICY_DIRS += ...`
  - to:   `PRODUCT_PRIVATE_SEPOLICY_DIRS += ...`

Verification:
- `m -j12 plat_pub_versioned.cil product_mapping_file` succeeded.

---

### 4) `sepolicy_test` failure (`Unrecognized entrypoint for pdthellod at /apex/...`)
Observed error:
- `Unrecognized entrypoint for pdthellod at /apex/com\.oem\.pdthellod/bin/pdthellod`

Fix:
- Removed product-global `/apex/...` file_contexts entry file:
  - deleted: `product/oem/apex/pdthellod/sepolicy/product/file_contexts`

Reason:
- This path entry is not required for APEX payload labeling itself.
- APEX payload labeling is still defined by:
  - `product/oem/apex/pdthellod/file_contexts` (`/bin/pdthellod -> pdthellod_exec`)
- Service process context is explicitly set in init rc:
  - `product/oem/apex/pdthellod/pdthellod_product.rc`
  - `seclabel u:r:pdthellod:s0`

Verification:
- `m -j12 sepolicy_test` succeeded.
- Full `m -j12` succeeded.

## Final state
- Full build completed successfully after the above fixes.
- Warnings about image size limits (`cache.img`, `userdata.img`) were shown but build completed.

## Changed files
- `product/oem/apex/pdthellod/sepolicy/product/pdthellod.te` (modified)
- `product/target/common/product_modules.mk` (modified)
- `packages/modules/common/build/allowed_deps.txt` (regenerated)
- `product/oem/apex/pdthellod/sepolicy/product/file_contexts` (deleted)

