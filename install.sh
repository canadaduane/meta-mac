#!/usr/bin/ssh-agent bash

# We will be using the `meta` key as mac-os 'Cmd' key, so we
# must prevent it from doing the regular gnome overlay action:
gsettings set org.gnome.mutter overlay-key ''

sudo mkdir -p /etc/keyd/
sudo cp keyd/default.conf /etc/keyd/default.conf

# Construct the app.conf file from apps/*.conf dir
DIR="$HOME/.config/keyd/"
mkdir -p "$DIR"
echo "" > $HOME/.config/keyd/app.conf
pushd apps
for APP in *.conf; do
  echo "[${APP%.*}]" >> "$DIR/app.conf"
  cat "$APP" >> "$DIR/app.conf"
  (echo && echo) >> "$DIR/app.conf"
done
