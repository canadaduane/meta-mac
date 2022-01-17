#!/bin/bash

# Enable C-S-k to clear & reset terminal (disabled by default)
gsettings set org.gnome.Terminal.Legacy.Keybindings:/org/gnome/terminal/legacy/keybindings/ reset-and-clear '<Primary><Shift>k'

# Enable C-S-a to select all (disabled by default)
gsettings set org.gnome.Terminal.Legacy.Keybindings:/org/gnome/terminal/legacy/keybindings/ select-all '<Primary><Shift>a'
