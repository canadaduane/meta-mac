# Meta Mac

Enables a Mac OS-like shortcut key experience on Pop!_OS (and most Ubuntu-based Linux distros).

# Dependencies

- libudev
- [keyd](https://github.com/rvaiya/keyd)

## Installation


1. Install [keyd](https://github.com/rvaiya/keyd)

```
sudo apt install libudev-dev
git clone git@github.com:rvaiya/keyd.git
cd keyd && make && sudo make install
sudo systemctl enable keyd && sudo systemctl start keyd
```

2. Add the Mac-like keyd config file

```
sudo cp keyd/default.cfg /etc/keyd/
sudo systemctl restart keyd
```

3. Remove gnome/mutter's overlay key:

```
gsettings set org.gnome.mutter overlay-key ''
```

Done!

## Which Shortcut Keys?

- For cut/copy/paste key combos, translate to universal clipboard keys (works in terminal as well as other apps):
  - Cut: `Command-X` -> `Ctrl-Del` 
  - Copy: `Command-C` -> `Ctrl-Ins`
  - Paste: `Command-V` -> `Shift-Ins`

- For window switching, let key combos pass through:
  - Switch: `Command-Tab` -> `Meta-Tab`
  - Switch backwards: ``Command-` `` => `Shift-Meta-Tab`

- Switch directly to an open tab, e.g. Firefox or VS Code:
  - `Command-1` -> `Alt-1`
  - `Command-2` -> `Alt-2`
  - ... etc.

- The remaning `Command-*` key combos are translated to `Ctrl-*`, e.g.
  - Close Window: `Command-W` -> `Ctrl-W`
  - New Tab: `Command-T` -> `Ctrl-T`
  - Bold: `Command-B` -> `Ctrl-B`
  - Italics: `Command-I` -> `Ctrl-I`
  - etc.

- Mouse clicks:
  - `Command-Click` -> `Ctrl-Click`


## Why?

If I'm honest, the biggest discomfort I have with Linux shortcut keys is the mismatch between the terminal's idea of copy-paste (Ctrl+Shift+C/Ctrl+Shift+V) and everything else in Linux (Ctrl+C/Ctrl+V). And Linux isn't alone--Windows, too, has a weird relationship with copy/paste when it comes to the command shell. Only on the Mac have I experienced the sensible default that Command-C will copy to the clipboard, and Command-V will paste, regardless of which app is running.

But why stop there? If many of us have experience with Mac OS, and if we have the ability to make customizations in Linux (and boy, do we!) then why not make Pop!_OS as comfortable as possible?

On a personal note, I've lived and worked on a Mac for the past 15 years, and while I still admire Apple's technical and design capability, our values have slowly been drifting apart over the years. Apple is no longer the underdog, and it's clear that it doesn't need to design for developers any more.

I'm very excited to joining the free culture movement once again via my first [frame.work](https://frame.work) laptop and [Pop!_OS](https://pop.system76.com/). I've written META Mac in the hope that it will ease the transition for others along this path as well.

## TODO

- Consider global copy/paste shortcut key combination over single-key:
  - Cut: Shift + Delete
  - Copy: Ctrl + Insert
  - Paste: Shift + Insert
- Fix that holding meta and hitting a clipboard key followed by a non-clipboard key (e.g. 'z' to undo) requires letting go of meta key first.
- Is there a way to make Command+Click work?

## Thanks

Thanks to Francisco Lopes' [interception](https://gitlab.com/interception/linux/tools) and deafmute's [interception-tools debian package](https://github.com/deafmute1/deb-pkg).