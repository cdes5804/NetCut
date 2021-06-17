# NetCut :scissors:

## Introduction

A simple tool to ban people's Internet connection with ARP spoofing.

**Note**:

1. This tool does not perform any advanced attack beyond ARP spoofing. So if your target is aware of the attack, he/she may be able to nullify the tool.
2. This tool is only effective against targets in the same network.

## How to use

1. Clone the repository on github (https://github.com/cdes5804/NetCut)

2. `cd` into the directory and run `docker-compose run --rm netcut` (`--rm` is optional, it cleans up the container after the program exits). You can specify additional parameters for the application:
   * `--port`: the port of the APIs. Default: 9090
   * `--attack_interval`: Occasionally, the attacked target will issue ARP requests. The attack loses effect when the target receives the correct mac addresses. So we continously perform ARP spoofing (automatically), this flag specify the interval in milliseconds between continuous spoofing. Default: 10000
   * `--scan_interval`: The application scans the network for newly joined targets. This flag specify the **minimum** time in milliseconds we wait before the next scanning. Default: 5000

   Example: `docker-compose run netcut --port 8888 --attack_interval 12000 --scan_interval 8000`

## APIs

Currently the application has the following APIs:

* `GET /ping`: Used to check if the application is running. The response is a string **"OK"**;
* `GET /get_targets`: Used to retrieve the available targets in the network. The response is a stringified json of an array of available targets. Each target has the following properties:

   * `IP Address`: IP address of the target
   * `MAC Address`: MAC Address of the target
   * `Status`: The current status (has been cut or not) of the target. The value is either `Cut` or `Normal`.
* `GET /get_status/:ip`: Get the status of the target specified by its IP address. The response is a stringified json of an object with the following fields:

   * `Status`: The status of the target. The value is one of `Cut`, `Normal`, `Target Not Found`.
   * `Target`: Repeat the specified IP address.

* `POST /action/:ip`: Take action on the target specified by its IP address. The response is a stringified json of an object with the following fields:

   * `Status`: The status of the action. The value is one of:
      * `Target Not Found`: The target is not found in the network.
      * `Cut`: The target originally had Internet connection, but now it doesn't anymore.
      * `Recovered`: The target was cut before, and now its Internet connection is restored.
   * `Target`: Repeat the specified IP address.

* `Post /quit`: Restore the Internet connections of every target that was attacked, clean up the resources, and exit. Before exiting, the application sends a string **"OK"** as response.

## Other versions

If you prefer to use the CLI version, you can build it from [here](https://github.com/cdes5804/NetCut/tree/cli). Note that the version is **not** tested on many different platforms and may have portability issue. C++17 is required to build the version.

