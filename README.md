# cemu

- `cemu` is a Pure C emulator for riscv.
- More item detail for [Docs Page](https://lancerstadium.github.io/cemu/html/index.html)
- README introduce for [Website](https://lancerstadium.github.io/cemu)


## Usage

```
riscv64-linux-gnu-gcc-12 ./test/temp_01.c -o ./test/temp_01.out

```


## Project Management

- Create Project:

```
# ==== Install cemu ==== #
wget https://xmake.io/shget.text -O - | bash
git submodule update --init --recursive

# ==== Install xmake ==== #
# zsh <(wget https://xmake.io/shget.text -O -)
# bash <(wget https://xmake.io/shget.text -O -)
source ~/.xmake/profile
xmake --help
xmake update
# xmake update --uninstall
xmake create uemu & cd uemu

```


- Compile Project:

```
# ==== Local Compile ==== #
xmake f -a x86_64 --toolchain=gcc                      # config
xmake -v                                               # compile
xmake r                                                # exec
xmake c -a                                             # clean all
# xmake install -o [dir]                               # install

# ==== Cross Compile ==== #
xmake f -a aarch64 -p linux --cross=aarch64-linux-gnu-
xmake -v
# xmake r
qemu-aarch64 ./build/linux/aarch64/release/uemu        # emulate

```


- Riscv Tool Chain
```
cd package
git submodule add -f https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv
make && make linux
export PATH=$PATH:/opt/riscv/bin/
```


- Package Manage:

```
# ==== Usage xrepo ==== #
xrepo search unicorn                                   # search package
xrepo info unicorn                                     # display info
xrepo scan                                             # lookup packages
xrepo clean                                            # clean cache
xrepo install unicorn                                  # install package
xrepo remove unicorn --all                             # remove package
```

```
add_requires("unicorn", {alias = "unicorn", configs = {arch = host_arch}})
add_requires("gflags", {alias = "gflags"})
add_requires("elfio >=3.10", {alias = "elfio"})
```


## C & CPP Package Source

- conan:

```
# ==== Install conan ==== #
pip install conan
pip install conan --upgrade
conan --version

# ==== Usage conan ==== #
conan search -r conancenter unicorn
conan install unicorn/2.0.1
conan inspect -r conancenter unicorn

# ==== Install Package ==== #
conan install unicorn/2.0.1
conan install gflags/2.2.2
conan install elfio/3.12

xrepo install conan::unicorn
xrepo search conan::unicorn
```


```
add_requires("conan::unicorn/2.0.1", {alias = "unicorn"})
add_requires("conan::gflags/2.2.2", {alias = "gflags"})
add_requires("conan::elfio/3.12", {alias = "elfio"})
```


- vcpkg
  
```
# ==== Install vcpkg ==== #
cd ~
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg/
./bootstrap-vcpkg.sh
sudo ln -s /home/lancer/things/vcpkg/vcpkg /usr/bin    # link

# ==== Usage vcpkg ==== #
vcpkg list                                             # lisk packages
vcpkg search unicorn                                   # search package
vcpkg help triplet                                     # display info
vcpkg install unicorn                                  # install package
vcpkg install bullet3                                  # test

vcpkg install unicorn
vcpkg install gflags
vcpkg install elfio

```

```
add_requires("vcpkg::unicorn", {alias = "unicorn"})
add_requires("vcpkg::gflags", {alias = "gflags"})
add_requires("vcpkg::elfio", {alias = "elfio"})
```

- Intelligence:
  
```
"compileCommands": "${workspaceFolder}/.vscode/compile_commands.json"

```



## Project Document

```
sudo apt-get install doxygen graphviz doxygen-latex
git submodule add https://github.com/jothepro/doxygen-awesome-css.git
cd doxygen-awesome-css
git checkout v2.3.1

git submodule add https://github.com/unicorn-engine/unicorn.git
```


```
GENERATE_TREEVIEW      = YES # required!
DISABLE_INDEX          = NO
FULL_SIDEBAR           = NO
HTML_EXTRA_STYLESHEET  = package/doxygen-awesome-css/doxygen-awesome.css \
                        package/doxygen-awesome-css/doxygen-awesome-sidebar-only.css
HTML_COLORSTYLE        = AUTO_DARK # required with Doxygen >= 1.9.5

```