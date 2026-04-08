# Tablet Pen Cursor KWin Effect

Custom cursor overlay for pen tablets on KDE Plasma 6 (KWin Wayland).

When you hover your pen over the tablet, this shows a custom cursor (an SVG you can replace). When you touch the pen to the screen, the cursor disappears so it doesn't get in the way.

It also respects app-specific cursors. If an app like Xournal++ sets its own cursor, this won't override it.

## Installing

### AUR
https://aur.archlinux.org/packages/kwin-pen-cursor

### Building

```bash
cmake -B build
cmake --build build
sudo cmake --install build
```

or just

```bash
./build.sh
```

### Fedora RPM

```bash
./release.sh
```

Installs all build deps automatically, outputs the `.rpm` to `release/`.


Then log out and back in or reload the Plasma shell.

## Settings

Its Settings are in → Workspace Behavior → Desktop Effects.<br>
Currently only supporting changing the custom cursor.<br>
You may need to log out and back in for or reload the Plasma shell to see applied changes.

## License

MIT - see source files for details.
