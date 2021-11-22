import subprocess
import signal
import time
import dbus
import re
import os
import sys
import glob
import dbus.mainloop.glib
from gi.repository import GLib

# Start:
# sudo DBUS_SESSION_BUS_ADDRESS=$DBUS_SESSION_BUS_ADDRESS python watch.py

class WindowChangeDetector():
    # TODO: support all the things...

    def __init__(self, on_window_change):
        # Ugly kludge to get dbus to behave...
        uid = int(re.match('.*/run/user/([0-9]+)',
                  os.getenv('DBUS_SESSION_BUS_ADDRESS'))[1])
        os.seteuid(uid)

        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        self.con = dbus.SessionBus()

        os.seteuid(0)

        self.introspect = self.get_dbus_object("org.gnome.Shell.Introspect",
                                               "/org/gnome/Shell/Introspect")

        self.shell = self.get_dbus_object(
            "org.gnome.Shell", "/org/gnome/Shell")

        self.on_window_change = on_window_change
        self.introspect.connect_to_signal(
            "RunningApplicationsChanged", lambda: self._on_window_change())

    def load_config(self, path):
        for entry in os.scandir(path):
            if entry.is_file():
                print(entry.name)

    def get_dbus_object(self, interface, path):
        return dbus.Interface(self.con.get_object(interface, path), interface)

    def get_window_class(self):
        return self.shell.Eval('global.display.focus_window.get_wm_class()')[1].strip('"')

    def _on_window_change(self):
        self.on_window_change(self.get_window_class())

    def run(self):
        loop = GLib.MainLoop()
        loop.run()


class Config():
    def __init__(self, config_base_dir):
        self.config_base_dir = config_base_dir
        self.reload()

    def reload(self):
        self.main_config = ''
        self.configs_top_level = {}
        self.configs_cmd_level = {}
        self.get_configs(self.config_base_dir)

    def get_config_entries(self, base_dir):
        return glob.iglob(f"{base_dir}/**/*.cfg", recursive=True)

    def get_configs(self, config_base_dir):
        for entry in self.get_config_entries(config_base_dir):
            name, ext = os.path.basename(entry).split('.', 1)
            with open(entry, 'r') as file:
                body = file.read()
                if name == '_main':
                    self.main_config = body
                elif ext == 'cfg':
                    pattern = self.extract_pattern(body)
                    self.configs_top_level[name] = {
                        'body': body,
                        'pattern': pattern
                    }
                elif ext == 'top.cfg':
                    pattern = self.extract_pattern(body)
                    self.configs_cmd_level[name] = {
                        'body': body,
                        'pattern': pattern
                    }

        if self.main_config == None:
            raise Exception('_main.cfg required')

    def extract_pattern(self, body):
        """Look for '## /pattern/' regex pattern in config file body"""
        result = re.search(r'##\s*/([^/]+)/', body)
        if (result):
            return re.compile(result.group(1))
        else:
            return re.compile('.*')

    def assemble(self, window_class):
        """Assemble a complete keyd .cfg file from configs whose pattern matches"""
        top_content = ''
        for key in self.configs_top_level:
            config = self.configs_top_level[key]
            if re.search(config['pattern'], window_class):
                top_content += config['body'] + '\n\n'
                
        cmd_content = ''
        for key in self.configs_cmd_level:
            config = self.configs_cmd_level[key]
            if re.search(config['pattern'], window_class):
                cmd_content += config['body'] + '\n\n'

        assembled = self.main_config
        assembled = re.sub(r'##\s*\{APP_SPECIFIC_TOP\}\s*', top_content, assembled)
        assembled = re.sub(r'##\s*\{APP_SPECIFIC_CMD\}\s*', cmd_content, assembled)

        return assembled


class Keyd():
    def __init__(self):
        self.proc = subprocess.Popen(["keyd"])

    def set_config(self, config):
        # TODO: Make this signal driven to avoid potential initialization based race conditions.
        # Add the ability to use a custom config directory

        open('/etc/keyd/default.cfg', 'w').write(config)
        if self.proc:
            signal.signal(signal.SIGCHLD, signal.SIG_IGN)
            self.proc.kill()
            self.proc.wait()
            signal.signal(signal.SIGCHLD, lambda a, b: exit(1))

        self.proc = subprocess.Popen(["keyd"])


# Main
config_base_dir = os.path.join(sys.path[0], '..', 'config')
config = Config(config_base_dir)

signal.signal(signal.SIGCHLD, lambda a, b: exit(1))
keyd = Keyd()


def on_window_change(window_class):
    print(f'Setting up bindings for {window_class}')
    assembled_config = config.assemble(window_class)

    print(assembled_config)
    keyd.set_config(assembled_config)

on_window_change('meta_mac_start')

WindowChangeDetector(on_window_change).run()
