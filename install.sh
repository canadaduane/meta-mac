#!/usr/bin/ssh-agent /bin/bash

sudo mkdir -p /etc/interception
sudo chmod 0755 /etc/interception

# Build source code & install in /usr/local/bin
gcc src/meta_mac.cc -o bin/meta_mac &&
    sudo cp bin/meta_mac /usr/local/bin/ &&
gcc src/swap_super_alt.cc -o bin/swap_super_alt &&
    sudo cp bin/swap_super_alt /usr/local/bin/ &&
gcc src/caps2esc.cc -o bin/caps2esc &&
    sudo cp bin/caps2esc /usr/local/bin/

# Copy settings
cp settings/udevmon.yaml /etc/interception/udevmon.yaml &&
cp settings/clipboard_keys.yaml /etc/interception/clipboard_keys.yaml

# Install from DEB
INTERCEPTION_DEB="https://launchpad.net/~deafmute/\
+archive/ubuntu/interception/\
+files/interception-tools_0.6.4~groovy~ppa4_amd64.deb"
TEMP_DEB="$(mktemp).deb" &&
  wget -O "$TEMP_DEB" "$INTERCEPTION_DEB" &&
  sudo apt install "$TEMP_DEB"
rm -f "$TEMP_DEB"

# Configure Gnome Terminal to use the KEY_COPY and KEY_PASTE keys as
# the 'copy' and 'paste' keys (instead of Ctrl+Shift+C / Ctrl+Shift+V)
dconf write /org/gnome/terminal/legacy/keybindings/copy "'Copy'"
dconf write /org/gnome/terminal/legacy/keybindings/paste "'Paste'"
