# NetCut :scissors:

## Introduction

A simple tool to ban people's Internet connection with ARP spoofing.

**Note**:

1. This tool does not perform any advanced attack beyond ARP spoofing. So if your target is aware of the attack, he/she may be able to nullify the tool.
2. This tool is only effective against targets in the same network.

## How to install

1. Clone the repository on github (https://github.com/cdes5804/NetCut)

2. `cd` into the directory and run `docker-compose run --rm netcut` (`rm` is unnecessary, it cleans up the container after the program exits).

## How to use

1. After the program is started, it scans the network for available targets automatically.

2. Choose the targets by entering their numbers, separated by space if there are many. Easy as that.

   <img src="https://imgur.com/VtJh4QW.gif" alt="Peek 2021-06-11 22-58" style="zoom:67%;" />

3. Bam! Wait and see the people start complaining about the Internet.

   <img src="https://imgur.com/LEkwsgg.gif" alt="Peek 2021-06-11 22-59" style="zoom: 80%;" />

4. To restore their Internet connections, just select them again.

   <img src="https://imgur.com/C5bYGAR.gif" alt="Peek 2021-06-11 23-01" style="zoom:67%;" />
