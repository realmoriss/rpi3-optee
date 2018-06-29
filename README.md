# Raspberry Pi 3 OP-TEE buildroot system

## Build instructions
The easiest way to build OP-TEE for the Raspberry Pi 3 is to use our buildroot system in a Docker container. This gives us a stable and reproducible environment for building the components while making it easier to manage the dependencies. This tutorial shows you how to set up a Docker environment and how to build a basic system with OP-TEE by using our sources.

The first step is to build the Docker image and create a container. You can specify a directory in your hosts system which will be mounted inside the container. This allows you to copy the built system easily. The current directory must contain the provided Dockerfile which will be compiled into an image. The directory called `build` will contain the sources and the images which will be built later.
```
mkdir -p build
docker build -t rpi3-optee .
docker create -it --name rpi3-optee --mount type=bind,source="$(pwd)"/build,destination=/build rpi3-optee
```

You can start the container if it was created successfully. This command gives you an interactive shell inside the container.
```
docker start -ia rpi3-optee
```

The next step is to clone the Buildroot sources and our Buildroot layers:
```
git clone -b "2018.05.x" https://github.com/buildroot/buildroot.git
git clone https://github.com/realmoriss/rpi3-optee.git
```

Now you have two directories called `buildroot` and `rpi3-optee`. The `buildroot` directory contains the buildroot sources and `rpi3-optee` contains the OP-TEE, ARM TF, Raspberry Pi Firmware and U-Boot packages. The next step is to create a build directory called `out` which will contain the downloaded package source files and the output images. We will use this directory for an out-of-tree Buildroot build, by adding the `O=` parameter to the make command. We have to specify the directories where the external trees are stored, which can be done by adding the `BR2_EXTERNAL=` parameter to make (We can specify multiple directories by using `:` as a separator). The default config file is called `rpi3_defconfig` which is inside the `rpi3-optee/configs` directory.
```
mkdir -p out && cd out
make -C ../buildroot O="$(pwd)" BR2_EXTERNAL="../rpi3-optee" rpi3_defconfig
```

After the configuration has been finalized you can issue the make command to start building the sources. This can take a long time, so be patient.
```
make
```

If the build was successfull, you will have two tarballs which contains the `boot` and `root` file system. These tarballs are stored inside the `out/images` directory and they are called `bootfs.tar` and `rootfs.tar` respectively. You can extract these tarballs onto the appropriate file systems on the SD Card. The `boot` partition should be formatted to FAT32 and the `root` partition should be formatted to EXT4.

The default build uses DHCP for network connection via ethernet, and an SSH server is enabled by default. The default root password is `root`.

## Customization options
### Adding new packages
The default build can be customized by adding new packages provided by the buildroot repository. This can be done by issuing the `make menuconfig` command after the default config has been created. After the new config file has been saved, you can issue the `make` command again which will build the new packages and put the new files into the tarballs. If you want to remove a package, you have to deselect it from the config, then issue the `make clean` command and rebuild everything with `make`.

### Adding custom files
Buildroot provides an easy way to add new files into the target file system with rootfs overlays. A default rootfs overlay can be found inside the `rpi3-optee/board/rpi3/rootfs-overlay` directory. The contents of this directory will be copied into the target file system with retaining the folder structure. You can customize the permissions of the target files by adding new entries into `rpi3-optee/board/rpi3/device_table.txt`. More information about device tables can be found in the [Buildroot documentation](https://buildroot.org/downloads/manual/manual.html#makedev-syntax).

### Custom host key for the SSH deamon
If you want to use a custom SSH daemon host key, you can copy it into the rootfs overlay directory called `etc/ssh`. If no key files are present, ssh-keygen will be used to generate the default keys during build. This enables us to use the same key for the following builds and aviod host fingerprint changes.
