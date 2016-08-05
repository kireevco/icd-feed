# I2CD documentation


## Build the firmware from sources

This section describes how to build the firmware for I2CD from source codes.


### Host environment

The following operations are performed under an Ubuntu LTS 14.04.4 environment. For a Windows or a Mac OS X host computer, you can install a VM for having the same environment:
* Download Ubuntu 14.04.4 LTS image from [http://www.ubuntu.com](http://www.ubuntu.com)
* Install this image with VirtualBox (http://virtualbox.org) on the host machine. 50GB disk space reserved for the VM is recommanded


### Steps

In the Ubuntu system, open the *Terminal* application and type the following commands:

1. Install prerequisite packages for building the firmware:

    ```
    sudo apt-get install git g++ make libncurses5-dev subversion libssl-dev gawk libxml-parser-perl unzip wget python xz-utils
    ```

2. Download LEDE source codes:

    ```
    git clone git@github.com:FloTechnologies/icd-lede.git
    ```

3. Prepare the default configuration file for feeds:

    ```
    cd /path/to/lede
    cp feeds.conf.default feeds.conf
    ```

4. Add the I2CD feed:

    ```
    echo src-git i2cd git@github.com:FloTechnologies/icd-feed.git >> feeds.conf
    ```

5. Update the feed information of all available packages for building the firmware:

    ```
    ./scripts/feeds update
    ```

6. Install all packages:

    ```
    ./scripts/feeds install -a
    ```

7. Prepare the kernel configuration to inform LEDE that we want to build a firmware for I2CD:

    ```
    cp feeds/i2cd/i2cd_config .config
    ```

8. Start the compilation process:

    ```
    make V=99
    ```

9. After the build process completes, the resulted firmware file will be at `bin/targets/ramips/mt7620/lede-<version>-ramips-mt7620-i2cd-squashfs-sysupgrade.bin`. Depending on the H/W resources of the host environment, the build process may **take more than 2 hours**.


## License

Except where otherwise noted, all parts of this software is licensed under [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html).
