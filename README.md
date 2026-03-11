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

* LG C5
* 4k 120fps HDR H.265 300mbps of Bitrate
* Got 16ms ~ 20ms over usb to ethernet adapter
* 1ms ~3ms and 0 variation of network
* 5 ~ 8ms host encoding
* 8 ~ 10 ms decoding

* How the Keyboard Works?
* You can call the keyboard by Controller Red Button (Soft Keyboard), or Yellow Button calls immediatly the keyboard.
* You can select combined commands as Alt + Tab, Alt + F4 or others two combined buttons.
* Casse sensitive keyboard in case of Shift selected or Capslock button
* To close the keyboard you can press the Back controller button or Gamepad B aswell
* Gamepad commands was the same as the Windows Gamepad Keyboard (Y - Space, X backspace or Start Enter)
<img width="2270" height="1702" alt="1 7 1 Keyboard" src="https://github.com/user-attachments/assets/43ad2dbf-a64a-4ffb-86a5-c0fbd0694c01" />

* Compact performance status
<img width="2242" height="1682" alt="1 7 1 Performance Compact" src="https://github.com/user-attachments/assets/0d3cf07a-b048-4108-a44d-3c04fc469793" />
resolution + hdr + real refresh rate + Network lattency/variation + Host lattency + Decoding Lattency + Total Lattency + Frame drops + Codec + bitrate
