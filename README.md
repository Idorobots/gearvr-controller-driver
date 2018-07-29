# GearVR Controller input driver

An attempt at creating an input driver for the GearVR controller.

## Why?
I really want to use it as a remote for an RPi running Kodi.

## How?
At the moment, using [gattlib](https://github.com/labapart/gattlib) with some Bluez hacks here and there.

# Building & running

Running `make` should be enough to produce a single executable - `gearvr-test` - that dumps sensor data from a GearVR controller:

```
$ gearvr-test <address of your controller>
```

# License
See the LICENSE file for details.
