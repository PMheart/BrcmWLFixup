BrcmWLFixup Installation & Usage
===================================

#### System Requirements
Currently BrcmWLFixup needs macOS ***v10.13*** (macOS High Sierra) or greater.

#### Installation
It's highly recommended to let the bootloader inject BrcmWLFixup, otherwise you'll need [LiluFriend](https://github.com/PMheart/LiluFriend) to ensure BrcmWLFixup will work properly.

#### Available kernel flags
Add `-brcmdbg` to enable debug logging (ONLY available in DEBUG binaries).

Add `-brcmoff` to disable BrcmWLFixup entirely.

Add `-brcmbeta` to enable BrcmWLFixup on unsupported OS versions.
