# UPD

![pic](https://github.com/6PATyCb/willow/blob/JC3636W518EN-support/photo_2024-12-30_21-32-38.jpg?raw=true)

Added support to run willow on JC3636W518EN 

Added font to support Cyrillic characters ([how to create font](https://forum.lvgl.io/t/esp32-lvgl-example-with-cyrillic-fonts/9756))

Updated esp-adf to 2.7 and esp-idf to 5.1.4

# How to build firmware

This example for Windows 11 and WSL2

1. Unpack current project to some dir (for me it's: `c:\Users\6PATyCb\Downloads\willow\my_git\willow_rus_JC3636\willow\`)

2. Start `cmd`, and go to `current project dir` for create docker image:
```
docker build -t willow_idf_5_1_4 .
```
3. Start `cmd` and create container from docker image:
```
docker run -it --rm --name willow -v c:\Users\6PATyCb\Downloads\willow\my_git\willow_rus_JC3636\willow\:/willow  willow_idf_5_1_4
```
4. Inside container

run once:
```
./utils.sh install
```
then (optional) you can customize esp-idf:
```
./utils.sh config
```
then make compile and dist:
```
./utils.sh build && ./utils.sh dist 
```
after all that you can get your firmware file at `current project dir/build/willow-dist.bin` and upload to your `JC3636W518EN` with `Google Chrome` by [web installer](https://tasmota.github.io/install/) using USB data cable

# The Willow Inference Server has been released!

Willow users can now self-host the [Willow Inference Server](https://github.com/toverainc/willow-inference-server) for lightning-fast language inference tasks with Willow and other applications (even WebRTC) including STT, TTS, LLM, and more!

# Hello Willow Users!

Many users across various forums, social media, etc are starting to receive their hardware! I have enabled Github [discussions](https://github.com/toverainc/willow/discussions) to centralize these great conversations - stop by, introduce yourself, and let us know how things are going with Willow! Between Github discussions and issues we can all work together to make sure our early adopters have the best experience possible!

# Documentation

Visit official documentation on [heywillow.io](https://heywillow.io).
