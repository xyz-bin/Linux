Create a .deb package - simple 'hello deb' program.
Package Directory Structure
1. Set up the package directory:
```
mkdir -p ~/hello-package/DEBIAN
mkdir -p ~/hello-package/usr/local/bin
```
2. Add the control file:
```
nano ~/hello-package/DEBIAN/control
```
Paste the following:
```
Package: hello-deb
Version: 1.0
Architecture: all
Maintainer: Your Name <your.email@example.com>
Description: A simple Hello Deb package for testing .deb creating on WSL
```
3. Create the script:
```
echo -e '#!/bin/bash\necho "Hello, Deb!"' > ~/hello-package/usr/local/bin/hello-deb
chmod +x ~/hello-package/usr/local/bin/hello-deb
```
Build the .deb Package
Run:
```
dpkg-deb --build ~/hello-package
```
You should now see hello-package.deb in your home directory

After building the package, install it using dpkg:
```
sudo dpkg -i ~/hello-package.deb
```
Run the installed script:
```
hello-deb
```
