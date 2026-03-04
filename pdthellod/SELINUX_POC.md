# pdthellod SELinux APEX update PoC

## Goal
Verify whether changing APEX `file_contexts` from v1 to v2 changes SELinux behavior.

## Design
- `pdthellod` always tries to open `/apex/com.oem.pdthellod/etc/fileA`.
- `pdthellod_file_a_v1` and `pdthellod_file_a_v2` are both defined in policy.
- Access is allowed only to `pdthellod_file_a_v2`.
- APEX v1 labels `fileA` as `pdthellod_file_a_v1` (expect deny).
- APEX v2 labels `fileA` as `pdthellod_file_a_v2` (expect allow).

## Files
- `file_contexts` (current v1): labels `/etc/fileA` -> `pdthellod_file_a_v1`
- `file_contexts.v2`: labels `/etc/fileA` -> `pdthellod_file_a_v2`

## Verify on device
1. Boot/install v1 and start service.
2. Check label:
   - `adb shell ls -Z /apex/com.oem.pdthellod/etc/fileA`
3. Check deny logs:
   - `adb shell logcat -b all | grep -i -E "avc:|pdthellod"`
4. Build/install v2 by replacing `file_contexts` with `file_contexts.v2`.
5. Reboot, then run steps 2-3 again.
6. Confirm label changed to `pdthellod_file_a_v2` and open/read succeeds.
