#!/bin/sh

cd "$( dirname "$0" )"

if [ -x /usr/bin/sudo -a $UID -ne 0 ]; then
   exec /usr/bin/sudo $0 $*
fi

echo "Installing KX audio driver kernel extension..."

if [ -d /System/Library/Extensions/kXAudioDriver_LibraryLess.kext ]; then

	echo "This driver is already Installed, it will be unloaded and overwritten!"

	kextunload /System/Library/Extensions/kXAudioDriver_LibraryLess.kext > /dev/null
	kextunload /System/Library/Extensions/kXAudioDriver_LibraryLess.kext > /dev/null
	
	kextunload /System/Library/Extensions/kXAudioDriver_LibraryLess.kext > /dev/null
	kextunload /System/Library/Extensions/kXAudioDriver_LibraryLess.kext > /dev/null
	
	rm -R /System/Library/Extensions/kXAudioDriver_LibraryLess.kext
fi

echo "Installing a fresh copy of the driver ..."

cp -R kXAudioDriver_LibraryLess.kext /System/Library/Extensions/

find /System/Library/Extensions/kXAudioDriver_LibraryLess.kext -type d -exec /bin/chmod 0755 {} \;
find /System/Library/Extensions/kXAudioDriver_LibraryLess.kext -type f -exec /bin/chmod 0744 {} \;
chmod -R 755        /System/Library/Extensions/kXAudioDriver_LibraryLess.kext
chown -R root:wheel /System/Library/Extensions/kXAudioDriver_LibraryLess.kext

kextload -t /System/Library/Extensions/kXAudioDriver_LibraryLess.kext

echo "Installation finished - enjoy! ITzTravelInTime"