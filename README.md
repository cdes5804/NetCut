# NetCut :scissors:

## Introduction

A simple tool to ban people's Internet connection with ARP spoofing.

**Note**:

1. This tool is developed and tested on Linux platforms (Ubuntu 20.04, Arch Linux and alpine), currently it has no support for macOS and Windows.
2. This tool does not perform any advanced attack beyond ARP spoofing. So if your target is aware of the attack, he/she may be able to nullify the tool.
3. This tool is only effective against targets in the same network.

## How to install

1. Clone the repository on github (https://github.com/cdes5804/NetCut)

2. `cd` into the directory and `make`.

    <img src="https://imgur.com/xIRcPLb.gif" alt="Peek 2021-06-11 22-52" style="zoom:67%;" />

## How to use

1. After `make`, run the program with `sudo bin/main`. `sudo` is required for performing the attack.

2. After the program is started, it scans the network for available targets automatically.

3. Choose the targets by entering their numbers, separated by space if there are many. Easy as that.

   <img src="https://imgur.com/VtJh4QW.gif" alt="Peek 2021-06-11 22-58" style="zoom:67%;" />

4. Bam! Wait and see the people start complaining about the Internet.

   <img src="https://imgur.com/LEkwsgg.gif" alt="Peek 2021-06-11 22-59" style="zoom: 80%;" />

5. To restore their Internet connections, just select them again.

   <img src="https://imgur.com/C5bYGAR.gif" alt="Peek 2021-06-11 23-01" style="zoom:67%;" />
