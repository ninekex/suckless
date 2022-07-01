# suckless
all my patched and configured versions of [suckless's sofware](https://suckless.org/)

the patches are included as files

needs font awesome and nkmono (my own font, ill upload it soon once i figure out how font licenses work)

the status bar in dwm crashes when clicked on idk why but im guessing is bc i applied status2d or awesome bar incorrectly or smth.
recommend runing dwm in a while loop in your `.xinitrc` so that it just restarts instead of crashing and killing your programs.

example:

make a random script file `startdwm`
``` bash
#!/bin/bash

while true; do
    #log stderr to a file
    dwm 2> ~/.dwm.log
done
```

in `.xinitrc`
``` bash
[...]

# change wallpapers every 10 min
while true; do
    feh --bg-fill --randomize ~/Pictures/wallpapers
    sleep 10m
done &

# epiku status bar
slstatus &

# sitelen pona (experimental for the glx renderer and box blur)
picom --experimental-backends &

# exec for better security or smth idk i dont understand this lol
exec startdwm
```
