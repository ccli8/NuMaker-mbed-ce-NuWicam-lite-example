# Example for NuWicam-lite on Nuvoton's Mbed CE enabled boards

This is an example to demo communication over Mosbus over RS-485 on Nuvoton's Mbed CE enabled boards.

Check out [Mbed CE](https://github.com/mbed-ce)
for details on Mbed OS community edition.

> **ℹ️ Information**
>
> Refer to [NuMaker NuWicam](https://github.com/OpenNuvoton/NuMaker_NuWicam_Samples/) for more details.

## Support development tools

Use cmake-based build system.
Check out [hello world example](https://github.com/mbed-ce/mbed-ce-hello-world) for getting started.

> **⚠️ Warning**
>
> Legacy development tools below are not supported anymore.
> - [Arm's Mbed Studio](https://os.mbed.com/docs/mbed-os/v6.15/build-tools/mbed-studio.html)
> - [Arm's Mbed CLI 2](https://os.mbed.com/docs/mbed-os/v6.15/build-tools/mbed-cli-2.html)
> - [Arm's Mbed CLI 1](https://os.mbed.com/docs/mbed-os/v6.15/tools/developing-mbed-cli.html)

For [VS Code development](https://github.com/mbed-ce/mbed-os/wiki/Project-Setup:-VS-Code)
or [OpenOCD as upload method](https://github.com/mbed-ce/mbed-os/wiki/Upload-Methods#openocd),
install below additionally:

-   [NuEclipse](https://github.com/OpenNuvoton/Nuvoton_Tools#numicro-software-development-tools): Nuvoton's fork of Eclipse
-   Nuvoton forked OpenOCD: Shipped with NuEclipse installation package above.
    Checking openocd version `openocd --version`, it should fix to `0.10.022`.

## Developer guide

In the following, we take **NuMaker-PFM-NUC472** board as an example for Mbed CE support.

### Build the example

1.  Clone the example and navigate into it
    ```
    $ git clone https://github.com/mbed-nuvoton/NuMaker-mbed-ce-NuWicam-lite-example
    $ cd NuMaker-mbed-ce-NuWicam-lite-example
    $ git checkout -f master
    ```

1.  Deploy necessary libraries
    ```
    $ git submodule update --init
    ```
    Or for fast install:
    ```
    $ git submodule update --init --filter=blob:none
    ```

    Deploy further for `NuMaker-mbed-ce-Modbus-driver` library:
    ```
    $ cd NuMaker-mbed-ce-Modbus-driver; \
    git submodule update --init; \
    cd ..
    ```
    Or for fast install:
    ```
    $ cd NuMaker-mbed-ce-Modbus-driver; \
    git submodule update --init --filter=blob:none; \
    cd ..
    ```

1.  Compile with cmake/ninja
    ```
    $ mkdir build; cd build
    $ cmake .. -GNinja -DCMAKE_BUILD_TYPE=Develop -DMBED_TARGET=NUMAKER_PFM_NUC472
    $ ninja
    $ cd ..
    ```

### Flash the image

Flash by drag-n-drop built image `NuMaker-mbed-ce-NuWicam-lite-example.bin` or `NuMaker-mbed-ce-NuWicam-lite-example.hex` onto **NuMaker-PFM-NUC472** board
