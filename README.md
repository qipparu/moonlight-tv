# Moonlight TV

Moonlight TV is a community version of [Moonlight GameStream Client](https://moonlight-stream.org/), made for large
screens. It works on LG webOS powered TVs, and Raspberry Pi running Raspbian.

![Download Stats](https://img.shields.io/github/downloads/mariotaku/moonlight-tv/total)

# Why and for who this fork was created?

This fork was created because my TV LG C1 over wifi recognizes more than 200mbps of internet connection, so even if
the official LG Documentation says that the TV can handle until 100mbps of decoding bitrate, it "could" be possible
to use higher bitrates, in my case it worked, the image quality is very different from the limited 65mbps that i had 
experienced.
This fork is for people that have a LG OLED TV model between (C1...C5) that wants to test the TV capability and limits.
You must to know that this is a fork, so you'll have to install manually by  the WebOS Dev Manager.

## Features

* High performance streaming for webOS
* UI optimized for large screen and remote controller
* Supports up to 4 controllers
* Easy to port to other OSes (Now runs on macOS, Arch, Debian, Raspbian and Windows)
* New keyboard implementation
* New status performance checker and more compact (Like Artemis lite mode)
* Bitrate limit setted to 300mbps (Not compatible with all devices)

## Tests and Results

* LG C1:
* 4k 120fps HDR H.265 180mbps of Bitrate
* Got 22ms of total lattency
* 3 ~ 7ms of network lattency (Over wifi 5ghz very near from the TV)
* 8 ~ 10ms of decoding lattency
* 5ms of host lattency
* Tested on controllers:
* Xbox Series S (Rumble recognized - low lattency)
* 8bitdo Ultimate Wireless (D-Input mode, rumble not recognized - low lattency)