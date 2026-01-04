# Tablet Pen Cursor KWin Effect

Custom cursor overlay for pen tablets on KDE Plasma 6 (KWin Wayland).

When you hover your pen over the tablet, this shows a custom cursor (an SVG you can replace). When you touch the pen to the screen, the cursor disappears so it doesn't get in the way.

It also respects app-specific cursors. If an app like Xournal++ sets its own cursor, this won't override it.

## Building

```bash
cmake -B build
cmake --build build
sudo cmake --install build
```

or just

```bash
./install.sh
```


Then log out and back in or reload the plasma shell.

Its Settings are in → Workspace Behavior → Desktop Effects.

## Custom Cursor

Replace `/usr/share/kwin/effects/pen-point-cursor.svg` with your own SVG.

## License

MIT - see source files for details.